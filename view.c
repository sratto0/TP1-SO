#include "view.h"

void print_board(game_t *game) {
  for (int j = 0; j < game->width; j++) {
    printf(" ___");
  }

  printf("\n");

  for (int i = 0; i < game->height; i++) {
    for (int j = 0; j < game->width; j++) {
      int cell_num = game->board[i * game->width + j];
      printf("|%3d", cell_num);
    }
    printf("|\n");
    for (int j = 0; j < game->width; j++) {
      printf("|___");
    }
    printf("|\n");
  }
}

int main() {
  game_t *game = open_game_memory();
  sync_t *sync = open_sync_memory();

  while (true) {
    if (sem_wait(&sync->master_to_view) == -1) {
      if (errno == EINTR)
        continue;
      perror("sem_wait master_to_view");
      break;
    }

    print_board(game);

    if (sem_post(&sync->view_to_master) == -1) {
      perror("sem_post view_to_master");
      break;
    }

    if (game->finished) {
      break;
    }
  }
}
