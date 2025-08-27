#ifndef MASTER_H

#include "structs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_DIGITS 15 // CHEQUEAR!!
extern char **environ;

void init_game(game_t *game);

void init_semaphores(sync_t *sync);

#endif