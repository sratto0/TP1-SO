#ifndef MASTER_H
#define MASTER_H

#include "master_lib.h"

extern char **environ;

void init_game(game_t *game);

void init_semaphores(sync_t *sync);

#endif