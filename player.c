// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "player.h"
#include "player_lib.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Using: %s <player_id>\n", argv[0]);
    return 1;
  }

  game_t *game = open_game_memory(
      sizeof(game_t) + (atoi(argv[1]) * atoi(argv[2]) * sizeof(int)));
  sync_t *sync = open_sync_memory(sizeof(sync_t));

  unsigned int player_id = 0;
  int pid_p = getpid();
  while (player_id < game->player_count) {
    if (game->players[player_id].process_id == pid_p) {
      break;
    }
    player_id++;
  }

  while (true) {

    sem_wait_check(&sync->players_ready[player_id]);

    sem_wait_check(&sync->writer_mutex);

    sem_post_check(&sync->writer_mutex);

    enter_reader(sync);

    if (game->finished) { // chequear
      exit_reader(sync);
      break;
    }

    int local_board[8];

    get_state(game, player_id, local_board);

    exit_reader(sync);

    unsigned char move = choose_move(local_board);

    write(1, &move, sizeof(move));
  }

  close_memory("/game_state", game,
               sizeof(game_t) + (atoi(argv[1]) * atoi(argv[2]) * sizeof(int)),
               OPEN);
  close_memory("/game_sync", sync, sizeof(sync_t), OPEN);

  return 0;
}
