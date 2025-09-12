// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "view_lib.h"
#include <ncurses.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Using: %s <width> <height>\n", argv[0]);
    return 1;
  }

  game_t *game = open_game_memory(
      sizeof(game_t) + (atoi(argv[1]) * atoi(argv[2]) * sizeof(int)));
  sync_t *sync = open_sync_memory(sizeof(sync_t));

  initialize_ncurses();

  terminal_color_check();

  define_color_pairs();

  nodelay(stdscr, TRUE);

  while (true) {
    sem_wait_check(&sync->have_to_print);

    print_board_ncurses(game);

    if (game->finished) {
      mvprintw(LINES - 2, 0, "Game over! Press any key to exit...");
      refresh();
      sem_post_check(&sync->finished_printing);
      nodelay(stdscr, FALSE);
      getch();
      break;
    }

    sem_post_check(&sync->finished_printing);

    napms(50);
  }

  endwin();
  close_memory("/game_state", game,
               sizeof(game_t) + (atoi(argv[1]) * atoi(argv[2]) * sizeof(int)),
               OPEN);
  close_memory("/game_sync", sync, sizeof(sync_t), OPEN);
  return 0;
}
