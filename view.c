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

  // Inicializar ncurses
  setenv("TERM", "xterm-256color", 1);
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);

  if (!has_colors()) {
    endwin();
    printf("Your terminal does not support colors.\n");
    exit(1);
  }

  // Definir pares de colores
  init_pair(COLOR_PAIR_BOARD, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_PAIR_NUMBER, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_PAIR_PLAYER1, COLOR_BLACK, COLOR_RED);
  init_pair(COLOR_PAIR_PLAYER2, COLOR_BLACK, COLOR_YELLOW);
  init_pair(COLOR_PAIR_PLAYER3, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(COLOR_PAIR_PLAYER4, COLOR_WHITE, COLOR_BLUE); 
  init_pair(COLOR_PAIR_PLAYER5, COLOR_WHITE, COLOR_CYAN); 
  init_pair(COLOR_PAIR_PLAYER6, COLOR_WHITE, COLOR_BLUE);     
  init_pair(COLOR_PAIR_PLAYER7, COLOR_WHITE, COLOR_MAGENTA);  
  init_pair(COLOR_PAIR_PLAYER8, COLOR_WHITE, COLOR_YELLOW); 
  init_pair(COLOR_PAIR_PLAYER9, COLOR_WHITE, COLOR_RED); 
  init_pair(COLOR_PAIR_TITLE, COLOR_WHITE, COLOR_BLUE);
  init_pair(COLOR_PAIR_INFO, COLOR_CYAN, COLOR_BLACK);


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
