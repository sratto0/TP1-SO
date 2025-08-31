// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "player.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Using: %s <player_id>\n", argv[0]);
    return 1;
  }

  game_t *game = open_game_memory(sizeof(game_t) + (atoi(argv[1]) * atoi(argv[2]) * sizeof(int)));
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

    if (sem_wait(&sync->writer_mutex) == -1) {
      perror("sem_wait writer_mutex");
      break;
    }

    if (sem_post(&sync->writer_mutex) == -1) {
      perror("sem_post writer_mutex");
      break;
    }

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
      if (sem_wait(&sync->state_mutex) == -1) {
        perror("sem_wait state_mutex");
        break;
      }
    }

    if (sem_post(&sync->readers_mutex) == -1) {
      perror("sem_post readers_mutex");
      break;
    }

    unsigned char move = choose_move(
        player_id, game); // Esto está en el lugar incorrecto. Estamos eligiendo el
                    // movimiento antes de liberar el mutex del juego. Acá sólo
                    // se debería consultar el estado y no elegir el movimiento

    if (sem_wait(&sync->readers_mutex) == -1) {
      perror("sem_wait readers_mutex");
      break;
    }

    sync->readers_count--;
    if (sync->readers_count == 0) {
      if (sem_post(&sync->state_mutex) == -1) {
        perror("sem_post state_mutex");
        break;
      }
    }

    if (sem_post(&sync->readers_mutex) == -1) {
      perror("sem_post readers_mutex");
      break;
    }

    write(1, &move, sizeof(move));
  }

  return 0;
}

// Función para elegir el mejor movimiento basado en los valores de celdas
// adyacentes
unsigned char choose_move(unsigned int player_id, game_t * game) {
  int x = game->players[player_id].x;
  int y = game->players[player_id].y;
  int best_value = -1;
  unsigned char best_direction = 0;

  // Arreglos para calcular las 8 direcciones
  int dx[] = {0, 1, 1, 1, 0, -1, -1, -1}; // Cambios en x
  int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1}; // Cambios en y

  // Evaluar cada dirección
  for (unsigned char dir = 0; dir < 8; dir++) {
    int new_x = x + dx[dir];
    int new_y = y + dy[dir];

    // Verificar si la nueva posición está dentro del tablero
    if (new_x >= 0 && new_x < game->width && new_y >= 0 &&
        new_y < game->height) {
      int cell_value = game->board[new_y * game->width + new_x];

      // Si encontramos un valor mayor, actualizamos la mejor dirección
      if (cell_value > best_value) {
        best_value = cell_value;
        best_direction = dir;
      }
    }
  }

  // Imprimir información de depuración
  printf("Jugador %d en (%d,%d) eligió dirección %d con valor %d\n",
         (int)player_id, x, y, best_direction, best_value);

  return best_direction;
}