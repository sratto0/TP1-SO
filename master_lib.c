#include "master_lib.h"

void init_sync(sync_t *sync) {
    init_semaphore(&sync->master_to_view, 0);
    init_semaphore(&sync->view_to_master, 1);
    init_semaphore(&sync->writer_lock, 1);
    init_semaphore(&sync->state_lock, 1);
    init_semaphore(&sync->readers_mutex, 1);
    for(int i = 0; i < MAX_PLAYERS; i++) {
        init_semaphore(&sync->players_ready[i], 0);
    }
    sync->readers_count = 0;
}

void init_game(game_t *game) {
  game->width = 10;
  game->height = 10;
}

void init_semaphore(sem_t *sem, int value) {
  if (sem_init(sem, 1, value) == -1)
    err_exit("sem_init");
}

bool has_valid_moves(game_t *game, player_t *player) {
  for(int dir = 0; dir < 8; dir++){
    if(is_valid_move(player->x + directions[dir][0], player->y + directions[dir][1], game)){
      return true;
    }
  }
  return false;
}

bool is_valid_move(int x, int y, game_t *game) {
    return (x >= 0 && x < game->width && y >= 0 && y < game->height && game->board[y * game->width + x] >= 1 && game->board[y * game->width + x] <= 9);
}