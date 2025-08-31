#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "structs.h"

#define err_exit(msg)                                                          \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0);

game_t *open_game_memory(int size);

sync_t *open_sync_memory(int size);

void sem_wait_check(sem_t *sem);

void sem_post_check(sem_t *sem);

void sem_destroy_check(sem_t *sem);

game_t *create_game_memory(int size);

sync_t *create_sync_memory(int size);

void close_memory(void *mem, size_t size);

#endif