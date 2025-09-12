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

// Colores para el tablero
#define COLOR_PAIR_BOARD 1
#define COLOR_PAIR_NUMBER 2
#define COLOR_PAIR_PLAYER1 3
#define COLOR_PAIR_PLAYER2 4
#define COLOR_PAIR_PLAYER3 5
#define COLOR_PAIR_PLAYER4 6
#define COLOR_PAIR_PLAYER5 7
#define COLOR_PAIR_PLAYER6 8
#define COLOR_PAIR_PLAYER7 9
#define COLOR_PAIR_PLAYER8 10
#define COLOR_PAIR_PLAYER9 11
#define COLOR_PAIR_TITLE 12
#define COLOR_PAIR_INFO 13


int get_player_color(int player_id);
void print_board_ncurses(game_t *game);
void print_scoreboard(game_t *game);

#endif