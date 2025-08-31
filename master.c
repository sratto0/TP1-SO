#include "master.h"

int main(int argc, char *argv[]) {
  game_t *game = open_game_memory();
  sync_t *sync = open_sync_memory();

  init_sync(sync);
  init_game(game);

  int opt;
  char *view_path = NULL;
  unsigned int delay = 200;
  unsigned int timeout = 10;
  unsigned int seed = time(NULL);
  char *players_paths[9] = {NULL};

  while ((opt = getopt(argc, argv, "w:h:d:t:s:v:p:")) != -1) {
    switch (opt) {
    case 'w':
      game->width = atoi(optarg);
      break;
    case 'h':
      game->height = atoi(optarg);
      break;
    case 'd':
      delay = atoi(optarg);
      break;
    case 't':
      timeout = atoi(optarg);
      break;
    case 's':
      seed = atoi(optarg);
      break;
    case 'v':
      view_path = optarg;
      break;
    case 'p':

      // primero con getopt habria que tener la cantidad de players
      players_paths[0] = "./player"; // hardcodeado para probarlo con 1 jugador
      break;
    default:
      fprintf(stderr, "Uso: %s [-w width] [-h height] [-v view]\n", argv[0]);
      exit(EXIT_FAILURE);
      break;
    }
  }

  pid_t cpid_view = fork();

  char argv_width[MAX_DIGITS] = {0};
  char argv_height[MAX_DIGITS] = {0};

  sprintf(argv_height, "%d", game->height);
  sprintf(argv_width, "%d", game->width);

  if (cpid_view == -1) {
    err_exit("fork");
  }
  if (cpid_view == 0) {
    char *argv_view[] = {view_path, argv_width, argv_height, NULL};
    execve(argv_view[0], argv_view, NULL);
  }

  int player_pipes[MAX_PLAYERS][2];

  for (unsigned int i = 0; i < game->player_count; i++) {
    if (pipe(player_pipes[i]) == -1) {
      err_exit("pipe");
    }
  }

  fd_set read_fds, active_fds;
  int max_fd = 0;

  FD_ZERO(&active_fds);

  for (unsigned int i = 0; i < game->player_count; i++) {
    pid_t cpid_player = fork();
    if (cpid_player == -1) {
      err_exit("fork");
    }
    if (cpid_player == 0) {
      if (close(player_pipes[i][0]) == -1) {
        err_exit("close");
      } // cierra read end para child

      if (dup2(player_pipes[i][1], STDOUT_FILENO) == -1) {
        err_exit("dup2");
      }

      if (close(player_pipes[i][1])) {
        err_exit("close");
      } // ya no lo necesita, lo tiene en stdout

      for (unsigned int j = 0; j < game->player_count; j++) {
        if (i != j) {
          close(player_pipes[j][0]);
          close(player_pipes[j][1]);
        }
      }

      char *argv_player[] = {players_paths[i], argv_width, argv_height, NULL};
      execve(argv_player[0], argv_player, NULL);
    } else {
      if (close(player_pipes[i][1]) == -1) {
        err_exit("close");
      } // cierra write end para parent

      FD_SET(player_pipes[i][0], &active_fds);
      if (player_pipes[i][0] > max_fd) {
        max_fd = player_pipes[i][0];
      }

      game->players[i].process_id = cpid_player;
      game->players[i].blocked = false;
    }
  }

  for (unsigned int p = 0; p < game->player_count; p++) {
    sem_post(&sync->players_ready[p]);
  }

  struct timeval tv;
  bool all_blocked;
  time_t current_time;
  time_t last_valid_moves;

  time(&current_time);
  last_valid_moves = current_time;

  while (!game->finished) {
    // recibir movimientos

    sem_wait(&sync->view_to_master);
    sem_post(&sync->master_to_view);

    tv.tv_sec = 0;
    tv.tv_usec = delay * 1000;

    read_fds = active_fds;

    int ready = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

    if (ready == -1) {
      if (errno == EINTR)
        continue; // interrumpido por señal
      err_exit("select");
    }

    time(&current_time);

    if (difftime(current_time, last_valid_moves) > timeout) {
      game->finished = true;
      sem_post(
          &sync->state_mutex); // Liberamos el lock en caso de haber terminado
      continue;
    }

    sem_wait(&sync->writer_mutex);
    sem_wait(&sync->state_mutex);
    sem_post(&sync->writer_mutex);

    all_blocked = true; // todos bloqueados hasta comprobar

    // verifico por cada jugador
    for (unsigned int i = 0; i < game->player_count; i++) {
      if (game->players[i].blocked)
        continue;

      if (!has_valid_moves(game, &game->players[i])) {
        game->players[i].blocked = true;
        FD_CLR(player_pipes[i][0], &active_fds);
        continue;
      }

      all_blocked = false;

      if (FD_ISSET(player_pipes[i][0], &read_fds)) {
        unsigned char move;
        ssize_t bytes_read = read(player_pipes[i][0], &move, sizeof(move));

        if (bytes_read == 0) {
          // EOF
          game->players[i].blocked = true;
          FD_CLR(player_pipes[i][0], &active_fds);
          continue;
        }
        if (bytes_read != sizeof(move)) {
          if (errno != EINTR)
            perror("read");
          continue;
        }

        int new_x = game->players[i].x + directions[move][0];
        int new_y = game->players[i].y + directions[move][1];

        if (is_valid_move(new_x, new_y, game)) {
          bool occupied = false;
          for (unsigned int j = 0; j < game->player_count; j++) {
            if (j != i && game->players[j].x == new_x &&
                game->players[j].y == new_y) {
              occupied = true;
              break;
            }
          }
          if (!occupied) {
            int cell_value = game->board[new_y * game->width + new_x];
            game->players[i].score += cell_value;
            game->players[i].x = new_x;
            game->players[i].y = new_y;
            game->players[i].valid_requests++;

            last_valid_moves = current_time;
          } else {
            game->players[i].invalid_requests++;
          }
        } else {
          game->players[i].invalid_requests++;
        }

        sem_post(&sync->players_ready[i]);
      }
    }

    // ejecutar movimientos

    sem_post(&sync->state_mutex);

    // si todos estan bloqueados se termina
    if (all_blocked)
      game->finished = true;

    return 0;
  }
}
