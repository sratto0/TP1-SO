#include "utils.h"

#define err_exit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0);
#define MAP_FAILED ((void *) -1)

game_t * open_game_memory(){
    int fd = shm_open("/game_state", O_RDONLY, 0600);
    if (fd == -1){
        err_exit("shm_open");
    }

    game_t * game = mmap(NULL, sizeof(*game), PROT_READ, MAP_SHARED, fd, 0);

    if(game == MAP_FAILED){
        err_exit("mmap");
    }

    return game;
}

sync_t * open_sync_memory(){
    int fd2 = shm_open("/game_sync", O_RDONLY, 0600);
    if(fd2 == -1){
        err_exit("shm_open");
    }
    
    sync_t * sync = mmap(NULL, sizeof(*sync), PROT_READ, MAP_SHARED, fd2, 0);
    if(sync == MAP_FAILED){
        err_exit("mmap");
    }

    return sync;
}