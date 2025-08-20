#include "player.h"

int main(){
    game_t * game = open_game_memory();
    sync_t * sync = open_sync_memory();

    unsigned char move = choose_move();

    write(1, &move, sizeof(move));
}

unsigned char choose_move(){
    return 0;
}