// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "master_lib.h"

const int directions[8][2] = {
    {0, -1},  // up
    {1, -1},  // up-right
    {1, 0},   // right
    {1, 1},   // down-right
    {0, 1},   // down
    {-1, 1},  // down-left
    {-1, 0},  // left
    {-1, -1}  // up-left
};


void init_sync(sync_t *sync) {
  init_semaphore(&sync->master_to_view, 0);
  init_semaphore(&sync->view_to_master, 1);
  init_semaphore(&sync->writer_mutex, 1);
  init_semaphore(&sync->state_mutex, 1);
  init_semaphore(&sync->readers_mutex, 1);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    init_semaphore(&sync->players_ready[i], 1);
  }
  sync->readers_count = 0;
}

void init_game(game_t *game, unsigned short width, unsigned short height,
               unsigned int player_count) {
  game->width = width;
  game->height = height;
  game->player_count = player_count;
  game->finished = false;
}

void fill_board(game_t *game) {
  srand(time(NULL));
  for (int i = 0; i < (game->width * game->height); i++) {
    game->board[i] = (rand() % 9) + 1;
  }
}

void init_semaphore(sem_t *sem, int value) {
  if (sem_init(sem, 1, value) == -1)
    err_exit("sem_init");
}

bool has_valid_moves(game_t *game, player_t *player) {
  for (int dir = 0; dir < 8; dir++) {
    if (is_valid_move(player->x + directions[dir][0],
                      player->y + directions[dir][1], game)) {
      return true;
    }
  }
  return false;
}

bool is_valid_move(int x, int y, game_t *game) {
  return (x >= 0 && x < game->width && y >= 0 && y < game->height &&
          game->board[y * game->width + x] >= 1 &&
          game->board[y * game->width + x] <= 9);
}

void init_players(game_t *game, int players_pipes[][2], int player_count, int * max_fd) {
  *max_fd = 0;
  for (int i = 0; i < player_count; i++) {
    char name[] = {'P', 'l', 'a', 'y', 'e', 'r', '_', '0' + i, '\0'};
    strcpy(game->players[i].name, name);
    game->players[i].score = 0;
    game->players[i].valid_requests = 0;
    game->players[i].invalid_requests = 0;
    game->players[i].blocked = false;

    if (pipe(players_pipes[i]) == -1) {
      err_exit("pipe");
    }
    if (players_pipes[i][0] > *max_fd) {
      *max_fd = players_pipes[i][0];
    }
  }
  player_pos(game);
}

void player_pos(game_t *game) {
  unsigned short a = (game->height / 2) * 0.7; // positioning factor for height
  unsigned short b = (game->width / 2) * 0.7;  // positioning factor for width

  if (game->player_count == 1) {
    game->players[0].x = game->width / 2;
    game->players[0].y = game->height / 2;
    game->board[game->players[0].y * game->width + game->players[0].x] = 0;
  } else {
    for (unsigned int i = 0; i < game->player_count; i++) {
      double theta = (2.0 * M_PI * i) / game->player_count;

      game->players[i].x = (unsigned short)((game->width / 2) + b * cos(theta));
      game->players[i].y =
          (unsigned short)((game->height / 2) + a * sin(theta));

      if (game->players[i].x < game->width && game->players[i].y < game->height) {
        game->board[game->players[i].y * game->width + game->players[i].x] = -i;
      }
    }
  }
}

void close_not_needed_fds(int players_fds[][2], int player_count,
                     int current_player) {
  for (int i = 0; i < player_count; i++) {
    if (i == current_player) {
      safe_close(players_fds[current_player][0]);
    } else {
      safe_close(players_fds[i][0]);
      safe_close(players_fds[i][1]);
    }
  }
}

void safe_close(int fd) {
  if (close(fd) == -1) {
    err_exit("Close pipe error\n");
  }
}

void game_over(game_t *game, sync_t *sync) {
  sem_wait_check(&sync->writer_mutex);
  sem_wait_check(&sync->state_mutex);
  game->finished = true;
  sem_post_check(&sync->writer_mutex);
  sem_post_check(&sync->state_mutex);
}


int game_ended(game_t * game) {
  int ended = 1;
  for (unsigned int i=0; i < game->player_count && ended; i++) {
    if (!game->players[i].blocked) {
      ended = 0;
    }
  }
  return ended;
}


//Que solo recive
// Devuelve -1 si EOF/error, 0 si inválido, 1 si válido
int receive_move(int fd, unsigned char *dir) {
    ssize_t n = read(fd, dir, 1);
    if (n == 0) {
        // EOF
        return -1;
    } else if (n < 0) {
        perror("read");
        return -1;
    }

    return 1; // Dirección leída correctamente
}


//para ejecutar
bool execute_move(game_t *game, sync_t *sync, int turno, unsigned char dir) {
    sem_wait_check(&sync->writer_mutex);
    sem_wait_check(&sync->state_mutex);
    sem_post_check(&sync->writer_mutex); 

    bool valid = false;

    if(dir > 7){
        game->players[turno].invalid_requests++;
    }else{
      int dx = directions[dir][0];
      int dy = directions[dir][1];
      int new_x = game->players[turno].x + dx;
      int new_y = game->players[turno].y + dy;
  
      if (!is_valid_move(new_x, new_y, game)) {
          game->players[turno].invalid_requests++;
      } else {
          int cell_value = game->board[new_y * game->width + new_x];
          game->players[turno].score += cell_value;
  
          game->board[new_y * game->width + new_x] = -turno; // capturar celda
          game->players[turno].x = new_x;
          game->players[turno].y = new_y;
          game->players[turno].valid_requests++;
  
  
          valid = true;
      }
    }
    sem_post_check(&sync->state_mutex);
    // sem_post_check(&sync->writer_mutex);

    return valid;
}

void close_sems(sync_t *sync, unsigned int player_count){

    for (unsigned int i = 0; i < player_count; i++) {
        sem_destroy_check(&sync->players_ready[i]);
    }

    sem_destroy_check(&sync->master_to_view);
    sem_destroy_check(&sync->view_to_master); 
    sem_destroy_check(&sync->writer_mutex);
    sem_destroy_check(&sync->state_mutex);
    sem_destroy_check(&sync->readers_mutex);
}

bool any_player_can_move(game_t *game) {
    for (unsigned int i = 0; i < game->player_count; i++) {
        // Skip blocked players
        if (game->players[i].blocked) {
            continue;
        }
        
        // Check if this player has any valid moves
        if (has_valid_moves(game, &game->players[i])) {
            return true;
        }
    }
    
    // No player can make a valid move
    return false;
}
