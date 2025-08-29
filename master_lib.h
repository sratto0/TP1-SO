#ifndef MASTER_LIB_H
#define MASTER_LIB_H

#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>

#define MAX_DIGITS 15 // CHEQUEAR!!
#define MAX_PLAYERS 9

const int directions[8][2] = {
    {1, 0},   // right
    {1, 1},   // down-right
    {0, 1},   // down
    {-1, 1},  // down-left
    {-1, 0},  // left
    {-1, -1}, // up-left
    {0, -1},  // up
    {1, -1}   // up-right
};

void init_sync(sync_t *sync);
void init_game(game_t *game);
void init_semaphore(sem_t *sem, int value);
bool has_valid_moves(game_t *game, player_t *player);
bool is_valid_move(int x, int y, game_t *game);
#endif