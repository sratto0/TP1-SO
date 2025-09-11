#include "player_lib.h"
#include "utils.h"

void enter_reader(sync_t * sync){
    sem_wait_check(&sync->readers_mutex);
    sync->readers_count++;
    if (sync->readers_count == 1) {
      sem_wait_check(&sync->state_mutex);
    }
    sem_post_check(&sync->readers_mutex);
}

void exit_reader(sync_t * sync){
    sem_wait_check(&sync->readers_mutex);
    sync->readers_count--;
    if (sync->readers_count == 0) {
        sem_post_check(&sync->state_mutex);
    }
    sem_post_check(&sync->readers_mutex);
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