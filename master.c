// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "master_lib.h"

int main(int argc, char *argv[]) {

  argument_amount_check(argc);

  char *view_path = NULL;
  unsigned int delay = 200, timeout = 10, seed = time(NULL);
  char *players_paths[9] = {NULL};
  int player_count = 0;
  unsigned short width = 10, height = 10;

  get_arguments(argc, argv, &width, &height, &delay, &timeout, &seed,
                &view_path, players_paths, &player_count);

  check_player_count(player_count);

  game_t *game =
      create_game_memory(sizeof(game_t) + (width * height * sizeof(int)));
  sync_t *sync = create_sync_memory(sizeof(sync_t));

  init_sync(sync);
  init_game(game, width, height, player_count);

  fill_board(game, seed);

  print_configuration(width, height, delay, timeout, seed, view_path,
                      players_paths, player_count);

  sleep(2);

  pid_t view_pid;

  char argv_width[MAX_DIGITS] = {0};
  char argv_height[MAX_DIGITS] = {0};

  sprintf(argv_width, "%d", game->width);
  sprintf(argv_height, "%d", game->height);

  create_view_process(view_path, argv_width, argv_height, &view_pid);

  int players_fds[player_count][2];
  int max_fd;

  init_players_data_and_pipes(game, players_fds, player_count, &max_fd);

  create_players_processes(players_paths, players_fds, argv_width, argv_height,
                           player_count, game);

  fd_set read_fds, active_fds;
 
  setup_fds_for_select(game, players_fds, player_count, &active_fds);

  time_t last_move_time = time(NULL);

  int last_served = 0;

  while (!game->finished) {
    if (game_ended(game)) {
        game_over(game, sync);
        sync_with_view(sync, delay);
        break;
    }

    time_t now = time(NULL);
    int remaining = timeout - (now - last_move_time);

    read_fds = active_fds;
    struct timeval tv = {.tv_sec = remaining, .tv_usec = 0};

    int ready = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
    if (ready == -1) {
        perror("select");
        break;
    } else if (ready == 0) {
        game_over(game, sync);
        sync_with_view(sync, delay);
        break;
    }
    process_players(game, sync, player_count, players_fds, &read_fds, &active_fds,
                    &last_served, &last_move_time, delay);
  }

  signal_all_players_ready(game, sync, player_count);

  wait_view(view_path, view_pid);
  close_and_wait_players(game, players_fds, player_count);

  close_sems(sync, game->player_count);
  close_memory("/game_sync", sync, sizeof(sync_t), CREATE);
  close_memory("/game_state", game,
               sizeof(game_t) + (width * height * sizeof(int)), CREATE);

  return 0;
}