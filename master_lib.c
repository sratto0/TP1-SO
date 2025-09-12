// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "master_lib.h"

void argument_amount_check(int argc) {
  if (argc < 3) {
    err_msg("Incorrect number of arguments\n");
  }
}

void get_arguments(int argc, char *argv[], unsigned short *width,
                   unsigned short *height, unsigned int *delay,
                   unsigned int *timeout, unsigned int *seed, char **view_path,
                   char **players_paths, int *player_count) {
  int opt;

  while ((opt = getopt(argc, argv, "w:h:d:t:s:v:p:")) != -1) {
    switch (opt) {
    case 'w':
      unsigned short w = atoi(optarg);
      if (w < 10) {
        err_msg("Minimal board width: 10");
      }
      *width = w;
      break;
    case 'h':
      unsigned short h = atoi(optarg);
      if (h < 10) {
        err_msg("Minimal board height: 10");
      }
      *height = h;
      break;
    case 'd':
      *delay = atoi(optarg);
      break;
    case 't':
      *timeout = atoi(optarg);
      break;
    case 's':
      *seed = atoi(optarg);
      break;
    case 'v':
      *view_path = optarg;
      break;
    case 'p':
      if (*player_count < 9) {
        players_paths[(*player_count)++] = optarg;
      }
      while (optind < argc && argv[optind][0] != '-') {
        players_paths[(*player_count)++] = argv[optind++];
      }
      break;
    default:
      fprintf(stderr, "Using: %s [-w width] [-h height] [-v view]\n", argv[0]);
      exit(EXIT_FAILURE);
      break;
    }
  }
}

void check_player_count(int count) {
  if (count < 1 || count > 9) {
    err_msg("Invalid number of players");
  }
}

void init_sync(sync_t *sync) {
  init_semaphore(&sync->have_to_print, 0);
  init_semaphore(&sync->finished_printing, 0);
  init_semaphore(&sync->writer_mutex, 1);
  init_semaphore(&sync->state_mutex, 1);
  init_semaphore(&sync->readers_count_mutex, 1);
  for (int i = 0; i < MAX_PLAYERS; i++) {
    init_semaphore(&sync->players_ready[i], 1);
  }
  sync->readers_count = 0;
}

void init_semaphore(sem_t *sem, int value) {
  if (sem_init(sem, 1, value) == -1)
    err_exit("sem_init");
}

void init_game(game_t *game, unsigned short width, unsigned short height,
               unsigned int player_count) {
  game->width = width;
  game->height = height;
  game->player_count = player_count;
  game->finished = false;
}

void fill_board(game_t *game) {
  srand(time(NULL));
  for (int i = 0; i < (game->width * game->height); i++) {
    game->board[i] = (rand() % 9) + 1;
  }
}

void print_configuration(unsigned short width, unsigned short height,
                         unsigned int delay, unsigned int timeout,
                         unsigned int seed, char *view_path,
                         char **players_paths, int player_count) {
  printf(
      "width = %hu\nheight = %hu\ndelay = %ums\ntimeout = %us\nseed=%u\nview "
      "= %s\nplayers:\n",
      width, height, delay, timeout, seed,
      view_path != NULL ? view_path : " - ");

  for (int i = 0; i < player_count; i++) {
    printf("  %s\n", players_paths[i]);
  }
}

void create_view_process(char *path, char *width, char *height, pid_t *pid) {
  if (path != NULL) {
    if (access(path, X_OK) == 0) {
      *pid = fork();
      if (*pid == -1) {
        err_exit("Fork error");
      } else if (*pid == 0) {
        char *argv[] = {path, width, height, NULL};
        if (execve(path, argv, NULL) == -1) {
          err_exit("Execve error");
        }
      }
    } else {
      printf("View path is not executable or does not exist\n");
      path = NULL;
    }
  }
}

void init_players_data_and_pipes(game_t *game, int players_pipes[][2],
                                 int player_count, int *max_fd) {
  *max_fd = 0;
  for (int i = 0; i < player_count; i++) {
    char name[] = {'P', 'l', 'a', 'y', 'e', 'r', '_', '0' + i, '\0'};
    strcpy(game->players[i].name, name);
    game->players[i].score = 0;
    game->players[i].valid_requests = 0;
    game->players[i].invalid_requests = 0;
    game->players[i].blocked = false;

    if (pipe(players_pipes[i]) == -1) {
      err_exit("pipe");
    }
    if (players_pipes[i][0] > *max_fd) {
      *max_fd = players_pipes[i][0];
    }
  }
  player_pos(game);
}

void player_pos(game_t *game) {
  unsigned short a = (game->height / 2) * 0.7;
  unsigned short b = (game->width / 2) * 0.7;

  if (game->player_count == 1) {
    game->players[0].x = game->width / 2;
    game->players[0].y = game->height / 2;
    game->board[game->players[0].y * game->width + game->players[0].x] = 0;
  } else {
    for (unsigned int i = 0; i < game->player_count; i++) {
      double theta = (2.0 * M_PI * i) / game->player_count;

      game->players[i].x = (unsigned short)((game->width / 2) + b * cos(theta));
      game->players[i].y =
          (unsigned short)((game->height / 2) + a * sin(theta));

      if (game->players[i].x < game->width &&
          game->players[i].y < game->height) {
        game->board[game->players[i].y * game->width + game->players[i].x] = -i;
      }
    }
  }
}

void create_players_processes(char **paths, int fds[][2], char *width,
                              char *height, int player_count, game_t *game) {
  for (int i = 0; i < player_count; i++) {
    if (access(paths[i], X_OK) == 0) {
      pid_t pid = fork();
      if (pid == -1) {
        err_exit("Fork error");
      } else if (pid == 0) {
        close_not_needed_fds(fds, player_count, i);
        if (dup2(fds[i][1], STDOUT_FILENO) == -1) {
          err_exit("Dup2 error");
        }
        safe_close(fds[i][1]);
        char *argv[] = {paths[i], width, height, NULL};
        if (execve(argv[0], argv, NULL) == -1) {
          err_exit("Execve error");
        }
      } else {
        game->players[i].process_id = pid;
      }
    } else {
      game->players[i].process_id = 0;
    }
  }
}

void close_not_needed_fds(int players_fds[][2], int player_count,
                          int current_player) {
  for (int i = 0; i < player_count; i++) {
    if (i == current_player) {
      safe_close(players_fds[current_player][0]);
    } else {
      safe_close(players_fds[i][0]);
      safe_close(players_fds[i][1]);
    }
  }
}

void safe_close(int fd) {
  if (close(fd) == -1) {
    err_exit("Close pipe error\n");
  }
}

void setup_fds_for_select(game_t *game, int players_fds[][2], int player_count,
                          fd_set *active_fds) {
  FD_ZERO(active_fds);

  for (int i = 0; i < player_count; i++) {
    if (game->players[i].process_id == 0) {
      safe_close(players_fds[i][0]);
    } else {
      FD_SET(players_fds[i][0], active_fds);
    }
    safe_close(players_fds[i][1]);
  }
}

int game_ended(game_t *game) {
  int ended = 1;
  for (unsigned int i = 0; i < game->player_count && ended; i++) {
    if (!game->players[i].blocked) {
      ended = 0;
    }
  }
  return ended;
}

void game_over(game_t *game, sync_t *sync) {
  sem_wait_check(&sync->writer_mutex);
  sem_wait_check(&sync->state_mutex);
  game->finished = true;
  sem_post_check(&sync->writer_mutex);
  sem_post_check(&sync->state_mutex);
}

bool timeout_check(time_t last_move_time, unsigned int timeout, game_t *game,
                   sync_t *sync) {
  if (time(NULL) - last_move_time > timeout) {
    printf("Reached global timeout\n");
    game_over(game, sync);
    return true;
  }
  return false;
}

void process_players(game_t *game, sync_t *sync, int player_count,
                     int players_fds[][2], fd_set read_fds, int *last_served,
                     time_t *last_move_time, unsigned int delay) {
  for (int j = 0; j < player_count; j++) {
    int i = (*last_served + j) % player_count;

    if (FD_ISSET(players_fds[i][0], &read_fds)) {
      unsigned char dir;
      int result = receive_move(players_fds[i][0], &dir);

      if (result == -1) {
        game->players[i].blocked = true;
      } else {
        bool moved = execute_move(game, sync, i, dir);
        if (moved) {
          *last_move_time = time(NULL);

          sync_with_view(sync, delay);

          if (!any_player_can_move(game)) {
            printf("The game has ended: no player can move\n");
            game_over(game, sync);
            sync_with_view(sync, delay);
            break;
          }
        }
        if (!game->players[i].blocked) {
          sem_post_check(&sync->players_ready[i]);
        }
      }

      *last_served = (i + 1) % player_count;
      break;
    }
  }
}

int receive_move(int fd, unsigned char *dir) {
  ssize_t n = read(fd, dir, 1);
  if (n == 0) {
    // EOF
    return -1;
  } else if (n < 0) {
    perror("read");
    return -1;
  }

  return 1;
}

bool execute_move(game_t *game, sync_t *sync, int turno, unsigned char dir) {
  sem_wait_check(&sync->writer_mutex);
  sem_wait_check(&sync->state_mutex);
  sem_post_check(&sync->writer_mutex);

  bool valid = false;

  if (dir > 7) {
    game->players[turno].invalid_requests++;
  } else {
    int dx = directions[dir][0];
    int dy = directions[dir][1];
    int new_x = game->players[turno].x + dx;
    int new_y = game->players[turno].y + dy;

    if (!is_valid_move(new_x, new_y, game)) {
      game->players[turno].invalid_requests++;
    } else {
      int cell_value = game->board[new_y * game->width + new_x];
      game->players[turno].score += cell_value;

      game->board[new_y * game->width + new_x] = -turno;
      game->players[turno].x = new_x;
      game->players[turno].y = new_y;
      game->players[turno].valid_requests++;

      valid = true;
    }
  }

  for (unsigned int i = 0; i < game->player_count; i++) {
    if (!game->players[i].blocked &&
        !has_valid_moves(game, &game->players[i])) {
      game->players[i].blocked = true;
    }
  }

  sem_post_check(&sync->state_mutex);

  return valid;
}

void sync_with_view(sync_t *sync, unsigned int delay) {
  sem_post_check(&sync->have_to_print);
  sem_wait_check(&sync->finished_printing);
  usleep(delay * 1000);
}

bool any_player_can_move(game_t *game) {
  for (unsigned int i = 0; i < game->player_count; i++) {
    if (game->players[i].blocked) {
      continue;
    }

    if (has_valid_moves(game, &game->players[i])) {
      return true;
    }
  }

  return false;
}

bool is_valid_move(int x, int y, game_t *game) {
  return (x >= 0 && x < game->width && y >= 0 && y < game->height &&
          game->board[y * game->width + x] >= 1 &&
          game->board[y * game->width + x] <= 9);
}

bool has_valid_moves(game_t *game, player_t *player) {
  for (int dir = 0; dir < 8; dir++) {
    if (is_valid_move(player->x + directions[dir][0],
                      player->y + directions[dir][1], game)) {
      return true;
    }
  }
  return false;
}

void signal_all_players_ready(game_t *game, sync_t *sync, int player_count) {
  for (int i = 0; i < player_count; i++) {
    if (game->players[i].process_id != 0) {
      sem_post_check(&sync->players_ready[i]);
    }
  }
}

void wait_view(char *path, pid_t pid) {
  int view_ret;
  if (path != NULL) {
    waitpid(pid, &view_ret, 0);
  }
}

void close_and_wait_players(game_t *game, int players_fds[][2],
                            int player_count) {
  for (int i = 0; i < player_count; i++) {
    int status;
    if (game->players[i].process_id != 0) {
      safe_close(players_fds[i][0]);
      waitpid(game->players[i].process_id, &status, 0);
    } else {
      status = 256;
    }
  }
}

void close_sems(sync_t *sync, unsigned int player_count) {

  for (unsigned int i = 0; i < player_count; i++) {
    sem_destroy_check(&sync->players_ready[i]);
  }

  sem_destroy_check(&sync->have_to_print);
  sem_destroy_check(&sync->finished_printing);
  sem_destroy_check(&sync->writer_mutex);
  sem_destroy_check(&sync->state_mutex);
  sem_destroy_check(&sync->readers_count_mutex);
}