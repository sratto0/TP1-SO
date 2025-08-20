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
  print_board(game);
}
