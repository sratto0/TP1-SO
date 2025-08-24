#include "utils.h"

#define err_exit(msg)                                                          \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0);


void * open_memory(const char * name, size_t size, int flags){
  int fd = shm_open(name, flags, 0);
  if (fd == -1)
  err_exit("shm_open");

  int prot;
  if(flags == O_RDONLY){
    prot = PROT_READ;
  } else {
    prot = PROT_READ | PROT_WRITE;
  }

  void * mem = mmap(NULL, size, prot, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) {
    err_exit("mmap");
  }

  return mem;
}

game_t *open_game_memory() {
  return (game_t *)open_memory("/game_state", sizeof(game_t), O_RDONLY);
}

sync_t *open_sync_memory() {
  return (sync_t *)open_memory("/game_sync", sizeof(sync_t), O_RDWR);
}

void * create_memory(const char * name, size_t size) {
  int fd = shm_open(name, O_CREAT | O_RDWR, 0666); 
  if(fd == -1){
    err_exit("shm_open");
  }
  
  if(ftruncate(fd, size) == -1){
    err_exit("ftruncate");
  }
  
  void * mem = mmap(NULL, sizeof(size), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(mem == MAP_FAILED){
    err_exit("mmap");
  }
  //close(fd);
  return mem;
}

game_t * create_game_memory(){
  return (game_t *)create_memory("/game_state", sizeof(game_t));
}

sync_t * create_sync_memory(){
  return (sync_t *)create_memory("/game_sync", sizeof(sync_t));
}

