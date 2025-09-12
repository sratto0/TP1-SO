#ifndef VIEW_LIB_H
#define VIEW_LIB_H

#include "structs.h"
#include "utils.h"
#include <errno.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define COLOR_PAIR_BOARD 1
#define COLOR_PAIR_NUMBER 2
#define COLOR_PAIR_TITLE 3
#define COLOR_PAIR_INFO 4

enum {
  COLOR_PAIR_PLAYER0 = 5,
  COLOR_PAIR_PLAYER1,
  COLOR_PAIR_PLAYER2,
  COLOR_PAIR_PLAYER3,
  COLOR_PAIR_PLAYER4,
  COLOR_PAIR_PLAYER5,
  COLOR_PAIR_PLAYER6,
  COLOR_PAIR_PLAYER7,
  COLOR_PAIR_PLAYER8
};

int get_player_color(int player_id);
void print_board_ncurses(game_t *game);
void print_scoreboard(game_t *game, int start_y);
void define_color_pairs();
void initialize_ncurses();
void terminal_color_check();

#endif