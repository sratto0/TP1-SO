// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "utils.h"

void *open_memory(const char *name, size_t size, int flags) {
  int fd = shm_open(name, flags, 0);
  if (fd == -1)
    err_exit("shm_open");

  int prot;
  if (flags == O_RDONLY) {
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
  return (game_t *)open_memory("/game_state", size, O_RDONLY);
}

sync_t *open_sync_memory(int size) {
  return (sync_t *)open_memory("/game_sync", size, O_RDWR);
}

void *create_memory(const char *name, size_t size) {
  int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    err_exit("shm_open");
  }

  if (ftruncate(fd, size) == -1) {
    err_exit("ftruncate");
  }

  void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) {
    err_exit("mmap");
  }
  // close(fd);
  return mem;
}

game_t *create_game_memory(int size) {
  return (game_t *)create_memory("/game_state", size);
}

sync_t *create_sync_memory(int size) {
  return (sync_t *)create_memory("/game_sync", size);
}

void close_memory(const char * name, void *mem, size_t size) {
  if (munmap(mem, size) == -1) {
    err_exit("munmap");
  }
  if(shm_unlink(name) == -1){
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