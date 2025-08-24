#include "master.h"

int main(int argc, char *argv[]) {
    game_t *game = open_game_memory();
    sync_t *sync = open_sync_memory();

    init_semaphores(sync);

    for(int i = 0; i < argc; i++){
        switch (argv[i]){
        case "-w":
            game->width = atoi(argv[i+1]);
            i++;
            break;
        case "-h":
            game->height = atoi(argv[i+1]);
            i++;
            break;
        case "-d":
            break;
        case "-t":
            break;
        case "-s":
            break;
        case "-v":
            break;
        case "-p":
            break;
        default:
            break;
        }
    }
}


void init_semaphores(sync_t *sync) {
    if (sem_init(&sync->master_to_view, 1, 0) == -1)
        err_exit("sem_init master_to_view");
    
    if (sem_init(&sync->view_to_master, 1, 1) == -1)
        err_exit("sem_init view_to_master");
    
    if (sem_init(&sync->writer_lock, 1, 1) == -1)
        err_exit("sem_init writer_lock");
    
    if (sem_init(&sync->state_lock, 1, 1) == -1)
        err_exit("sem_init state_lock");
    
    if (sem_init(&sync->readers_mutex, 1, 1) == -1)
        err_exit("sem_init readers_mutex");
    
    sync->readers_count = 0;
    
    for (int i = 0; i < 9; i++) {
        if (sem_init(&sync->players_ready[i], 1, 0) == -1)
            err_exit("sem_init players_ready");
    }
}


