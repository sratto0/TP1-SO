#ifndef PLAYER_LIB_H
#define PLAYER_LIB_H

#include "structs.h"

void enter_reader(sync_t *sync);

void exit_reader(sync_t *sync);

unsigned char choose_move(int local_board[8]);

void get_state(game_t *game, unsigned int player_id, int local_board[8]);

#endif