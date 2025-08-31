#include "master_lib.h"

void init_sync(sync_t *sync) {
  init_semaphore(&sync->master_to_view, 0);
  init_semaphore(&sync->view_to_master, 1);
  init_semaphore(&sync->writer_mutex, 1);
  init_semaphore(&sync->state_mutex, 1);
  init_semaphore(&sync->readers_mutex, 1);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    init_semaphore(&sync->players_ready[i], 0);
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

void init_player(game_t *game, int players_pipes[][2], int player_count, int * max_fd) {
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
    for (int i = 0; i < game->player_count; i++) {
      double theta = (2.0 * M_PI * i) / game->player_count;

      game->players[i].x = (unsigned short)((game->width / 2) + b * cos(theta));
      game->players[i].y =
          (unsigned short)((game->height / 2) + a * sin(theta));

      if (game->players[i].x >= 0 && game->players[i].x < game->width &&
          game->players[i].y >= 0 && game->players[i].y < game->height) {
        game->board[game->players[i].y * game->width + game->players[i].x] = -i;
      }
    }
  }
}

close_not_needed_fds(int players_fds[][2], int player_count,
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


void receive_move(game_t * game, sync_t * sync, fd_set * read_fds, fd_set * active_fds, int players_fds[][2], char * finished, time_t * last_valid_move_time, int timeout){
  if(time(NULL) - *last_valid_move_time < timeout) {
    for (int i=0; i<game->player_count; i++) {
      sem_post_check(&sync->players_ready[i]);
      if ()
    }
  }
}

int game_ended(game_t * game, int player_count) {
  int ended = 1;
  for (int i=0; i < game->player_count && ended; i++) {
    if (!game->players[i].blocked) {
      ended = 0;
    }
  }
  return ended;
}