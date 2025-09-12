// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "utils.h"

void *memory_manager(const char *name, size_t size, int open_flags,
                     char create) {
  int fd = shm_open(name, open_flags, 0666);
  if (fd == -1)
    err_exit("shm_open");

  if (create) {
    if (ftruncate(fd, size) == -1) {
      err_exit("ftruncate");
    }
  }

  int prot;

  if (open_flags == O_RDONLY) {
    prot = PROT_READ;
  } else {
    prot = PROT_READ | PROT_WRITE;
  }

  void *mem = mmap(NULL, size, prot, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) {
    err_exit("mmap");
  }

  return mem;
}

game_t *open_game_memory(int size) {
  return (game_t *)memory_manager("/game_state", size, O_RDONLY, OPEN);
}

sync_t *open_sync_memory(int size) {
  return (sync_t *)memory_manager("/game_sync", size, O_RDWR, OPEN);
}

game_t *create_game_memory(int size) {
  return (game_t *)memory_manager("/game_state", size, O_CREAT | O_RDWR,
                                  CREATE);
}

sync_t *create_sync_memory(int size) {
  return (sync_t *)memory_manager("/game_sync", size, O_CREAT | O_RDWR, CREATE);
}

void close_memory(const char *name, void *mem, size_t size, char unlink_shm) {
  if (munmap(mem, size) == -1) {
    err_exit("munmap");
  }
  if (unlink_shm && shm_unlink(name) == -1) {
    err_exit("shm_unlink");
  }
}

void sem_wait_check(sem_t *sem) {
  if (sem_wait(sem) == -1) {
    err_exit("sem_wait");
  }
}

void sem_post_check(sem_t *sem) {
  if (sem_post(sem) == -1) {
    err_exit("sem_post");
  }
}

void sem_destroy_check(sem_t *sem) {
  if (sem_destroy(sem) == -1) {
    err_exit("sem_destroy");
  }
}