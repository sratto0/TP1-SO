#include "player.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Using: %s <player_id>\n", argv[0]);
    return 1;
  }

  game_t *game = open_game_memory();
  sync_t *sync = open_sync_memory();

  int player_id = 0;
  int pid_p = getpid();
  while (player_id < game->player_count) {
    if (game->players[player_id].process_id == pid_p) {
      break;
    }
    player_id++;
  }

  // for (int i=0; game->players[i].process_id != NULL; i++) {
  //   if (game->players[i].process_id == pid_p) {
  //     player_id = i + 1;
  //     break;
  //   }
  // }

  if (player_id < 0 || player_id >= game->player_count) {
    fprintf(stderr, "Error: ID de jugador %d no válido. Rango válido: 0-%d\n",
            player_id, game->player_count - 1);
    return 1;
  }

  while (true) {

    if (sem_wait(&sync->players_ready[player_id]) == -1) {
      if (errno == EINTR)
        continue;
      perror("sem_wait players_ready");
      break;
    }

    if (game->finished) {
      break;
    }

    if (sem_wait(&sync->readers_mutex) == -1) {
      perror("sem_wait readers_mutex");
      break;
    }

    sync->readers_count++;
    if (sync->readers_count == 1) {
      if (sem_wait(&sync->state_lock) == -1) {
        perror("sem_wait state_lock");
        break;
      }
    }

    if (sem_post(&sync->readers_mutex) == -1) {
      perror("sem_post readers_mutex");
      break;
    }

    unsigned char move = choose_move(player_id);

    if (sem_wait(&sync->readers_mutex) == -1) {
      perror("sem_wait readers_mutex");
      break;
    }

    sync->readers_count--;
    if (sync->readers_count == 0) {
      if (sem_post(&sync->state_lock) == -1) {
        perror("sem_post state_lock");
        break;
      }
    }

    if (sem_post(&sync->readers_mutex) == -1) {
      perror("sem_post readers_mutex");
      break;
    }
    // printf("movimientos invalidos: %d\n", game->players[0].invalid_requests);
    // fflush(stdout);

    write(1, &move, sizeof(move));
  }

  return 0;
}

unsigned char choose_move(int player_id) { return 0; }
