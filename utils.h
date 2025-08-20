#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <sys/stat.h>

#include "structs.h"

game_t * open_game_memory();

sync_t * open_sync_memory();

#endif