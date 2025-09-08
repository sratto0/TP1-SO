// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "player.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Using: %s <player_id>\n", argv[0]);
    return 1;
  }

  game_t *game = open_game_memory(sizeof(game_t) + (atoi(argv[1]) * atoi(argv[2]) * sizeof(int)));
  sync_t *sync = open_sync_memory(sizeof(sync_t));

  unsigned int player_id = 0;
  int pid_p = getpid();
  while (player_id < game->player_count) {
    if (game->players[player_id].process_id == pid_p) {
      break;
    }
    player_id++;
  }

  while (true) {
    
    sem_wait_check(&sync->players_ready[player_id]);

    sem_wait_check(&sync->writer_mutex);
  
    sem_post_check(&sync->writer_mutex);

    sem_wait_check(&sync->readers_mutex);
    
    sync->readers_count++;
    if (sync->readers_count == 1) {
      sem_wait_check(&sync->state_mutex);
    }

    if (game->finished) { //chequear
      sem_post_check(&sync->readers_mutex);
      sem_post_check(&sync->readers_mutex);
      break;
    }

    sem_post_check(&sync->readers_mutex);

    int local_board[8];

    get_state(game, player_id, local_board);

    sem_wait_check(&sync->readers_mutex);

    sync->readers_count--;
    if (sync->readers_count == 0) {
      sem_post_check(&sync->state_mutex);
    }

    sem_post_check(&sync->readers_mutex);

    unsigned char move = choose_move(local_board);

    write(1, &move, sizeof(move));
  }

  return 0;
}

unsigned char choose_move(int local_board[8]){
  int best_value = -1;
  unsigned char best_direction = 0;
  for (unsigned char dir = 0; dir < 8; dir++) {
    if (local_board[dir] > best_value) {
      best_value = local_board[dir];
      best_direction = dir;
    }
  }
  return best_direction;
}

void get_state(game_t * game, unsigned int player_id, int local_board[8]) {
    int x = game->players[player_id].x;
    int y = game->players[player_id].y;
    int width = game->width;
    int height = game->height;
    int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    for (unsigned char dir = 0; dir < 8; dir++) {
        int new_x = x + dx[dir];
        int new_y = y + dy[dir];
        if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height) {
            local_board[dir] = game->board[new_y * (width) + new_x];
        } else {
            local_board[dir] = -1; // Valor inválido
        }
    }
}