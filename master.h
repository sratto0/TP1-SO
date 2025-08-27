#ifndef MASTER_H

#include <stdio.h>
#include "structs.h"
#include <stdlib.h>
#include "utils.h"
#include <unistd.h>

#define MAX_DIGITS 15 // CHEQUEAR!!
extern char **environ;

void init_game(game_t * game);

void init_semaphores(sync_t *sync);


#endif