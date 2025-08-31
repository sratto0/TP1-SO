#ifndef MASTER_H
#define MASTER_H

#include "master_lib.h"

extern char **environ;

void init_game(game_t *game, unsigned short width, unsigned short height,
               unsigned int player_count);

void init_semaphores(sync_t *sync);

#endif