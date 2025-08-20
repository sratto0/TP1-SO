#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <sys/stat.h>

#include "structs.h"

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); } while (0);
#define MAP_FAILED ((void *) -1)


void openMemory(){
    int fd = shm_open("/game_state", O_RDONLY, 0600);
    if (fd == -1){
        errExit("shm_open");
    }

    game_t * game = mmap(NULL, sizeof(*game), PROT_READ, MAP_SHARED, fd, 0);

    if(game == MAP_FAILED){
        errExit("mmap");
    }

    player_t * player = mmap(NULL, sizeof(*player), PROT_READ, MAP_SHARED, fd, 0);
    
    if(player == MAP_FAILED){
        errExit("mmap");
    }
    
    

    printf("Game Width: %d\n", game->width);
}