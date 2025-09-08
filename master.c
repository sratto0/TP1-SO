// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "master_lib.h"

int main(int argc, char *argv[]) {

  if (argc < 3) {
    err_exit("Incorrect number of arguments\n");
  }

  int opt;
  char *view_path = NULL;
  unsigned int delay = 200, timeout = 10, seed = time(NULL);
  char *players_paths[9] = {NULL};
  int player_count = 0;
  unsigned short width = 20, height = 10;

  while ((opt = getopt(argc, argv, "w:h:d:t:s:v:p:")) != -1) {
    switch (opt) {
    case 'w':
      width = atoi(optarg);
      break;
    case 'h':
      height = atoi(optarg);
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
      if (player_count < 9) {
        players_paths[player_count++] = optarg;
      }
      while (optind < argc && player_count < 9 && argv[optind][0] != '-') {
        players_paths[player_count++] = argv[optind++];
      }
      break;
    default:
      fprintf(stderr, "Uso: %s [-w width] [-h height] [-v view]\n", argv[0]);
      exit(EXIT_FAILURE);
      break;
    }
  }

  if (player_count < 1 || player_count > 9) {
    err_exit("Invalid number of players");
  }

  game_t *game = create_game_memory(sizeof(game_t) + (width * height * sizeof(int)));
  sync_t *sync = create_sync_memory(sizeof(sync_t));

  init_sync(sync);
  init_game(game, width, height, player_count);

  fill_board(game);

  char argv_width[MAX_DIGITS] = {0};
  char argv_height[MAX_DIGITS] = {0};

  sprintf(argv_width, "%d", game->width);
  sprintf(argv_height, "%d", game->height);

  printf("width = %d\nheight = %d\ndelay = %dms\ntimeout = %ds\nseed=%d\nview "
         "= %s\nplayers:\n",
         width, height, delay, timeout, seed,
         view_path != NULL ? view_path : " - ");

  for (int i = 0; i < player_count; i++) {
    printf("  %s\n", players_paths[i]);
  }

  sleep(2);

  pid_t view_pid;
  if (view_path != NULL) {
    if (access(view_path, X_OK) == 0) {
      view_pid = fork();
      if (view_pid == -1) {
        err_exit("Fork error");
      } else if (view_pid == 0) {
        char *view_argv[] = {view_path, argv_width, argv_height, NULL};
        if (execve(view_path, view_argv, NULL) == -1) {
          err_exit("Execve error");
        }
      }
    } else {
      printf("View path is not executable or does not exist\n");
      view_path = NULL;
    }
  }

    int players_fds[player_count][2];
    int max_fd;
    
    init_players(game, players_fds, player_count, &max_fd);
    
    
    for (int i = 0; i < player_count; i++) {
      if (access(players_paths[i], X_OK) == 0) {
        pid_t player_pid = fork();
        if (player_pid == -1) {
          err_exit("Fork error");
        } else if (player_pid == 0) {
          close_not_needed_fds(players_fds, player_count, i);
          if (dup2(players_fds[i][1], STDOUT_FILENO) == -1) {
            err_exit("Dup2 error");
          }
          safe_close(players_fds[i][1]);
          char *player_argv[] = {players_paths[i], argv_width, argv_height,
                                 NULL};
          if (execve(player_argv[0], player_argv, NULL) == -1) {
            err_exit("Execve error");
          }
        } else {
          game->players[i].process_id = player_pid;
        }
      } else {
        game->players[i].process_id = 0;
      }
    }

    fd_set read_fds, active_fds;
    FD_ZERO(&active_fds);

    for (int i = 0; i < player_count; i++) {
      if (game->players[i].process_id == 0) {
        safe_close(players_fds[i][0]);
      } else {
        FD_SET(players_fds[i][0], &active_fds);
      }
      safe_close(players_fds[i][1]);
    }

    time_t last_move_time = time(NULL);
    

    int last_served = 0; // para implementar round-robin

    while (!game->finished) {
        if (game_ended(game)) {
            game_over(game, sync);
            break;
        }

        // Timeout global (tiempo sin movimientos válidos)
        if (time(NULL) - last_move_time > timeout) {
            printf("Timeout global alcanzado\n");
            game_over(game, sync);
            break;
        }

        read_fds = active_fds;
        struct timeval tv = { .tv_sec = timeout, .tv_usec = 0 };

        int ready = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        if (ready == -1) {
            perror("select");
            break;
        } else if (ready == 0) {
            //printf("Timeout: nadie movió\n");
            game_over(game, sync);
            break;
        }

        // Round-robin: recorrer jugadores a partir de last_served
        for (int j = 0; j < player_count; j++) {
            int i = (last_served + j) % player_count;

            //otro fd_isset para que se ajuste a tener una receive y una execute
            if (FD_ISSET(players_fds[i][0], &read_fds)) {

             // sem_post_check(&sync->players_ready[i]); 
              
              unsigned char dir;
              int result = receive_move(players_fds[i][0], &dir);

              if (result == -1) {
                  //printf("Jugador %s terminó (EOF)\n", game->players[i].name);
                  game->players[i].blocked = true;
              } else {
                  bool moved = execute_move(game, sync, i, dir);
                  if (moved) {
                      last_move_time = time(NULL);

                      // máster ↔ vista
                      sem_post_check(&sync->master_to_view);
                      sem_wait_check(&sync->view_to_master);

                      usleep(delay * 1000);

                      if (!any_player_can_move(game)) {
                          printf("El juego ha terminado: ningún jugador puede moverse\n");
                          game_over(game, sync);

                          sem_post_check(&sync->master_to_view);
                          sem_wait_check(&sync->view_to_master);
                          
                          break;
                      }
                  }
                  sem_post(&sync->players_ready[i]); // habilito jugador
              }
              
              last_served = (i + 1) % player_count;
              break; // solo un movimiento por ronda
            }
        }
    } //while

    for (int i = 0; i < player_count; i++) {
        if (game->players[i].process_id != 0) {
            sem_post_check(&sync->players_ready[i]);
        }
    }

    int view_ret;
    if (view_path != NULL) {
      waitpid(view_pid, &view_ret, 0);
    }

    for (int i = 0; i < player_count; i++) {
      int status;
      if (game->players[i].process_id != 0) {
        safe_close(players_fds[i][0]);
        waitpid(game->players[i].process_id, &status, 0);
      }else {
        status = 256;
      }
    }

    close_sems(sync, game->player_count);

    close_memory("/game_sync", sync, sizeof(sync_t));
    
    close_memory("/game_state", game, sizeof(game_t) + (width * height * sizeof(int)));
    
    return 0;
}