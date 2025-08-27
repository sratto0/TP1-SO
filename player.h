#ifndef PLAYER_H

#include "structs.h"
#include "utils.h"
#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

unsigned char choose_move(unsigned int player_id);

#endif