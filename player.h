#ifndef PLAYER_H
#define PLAYER_H

#include "structs.h"
#include "utils.h"
#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned char choose_move(int local_board[8]);
void get_state(game_t *game, unsigned int player_id, int local_board[8]);
#endif