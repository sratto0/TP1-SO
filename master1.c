#include "master.h"

int main(int argc, char *argv[]) {

  if (argc < 3) {
    err_exit("Incorrect number of arguments\n");
  }

  int opt;
  char *view_path = NULL;
  unsigned int delay = 200;
  unsigned int timeout = 10;
  unsigned int seed = time(NULL);
  char *players_paths[9] = {NULL};
  unsigned int player_count = 0;
  unsigned short width = 20;
  unsigned short height = 10;

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

      // primero con getopt habria que tener la cantidad de players
      players_paths[0] = "./player"; // hardcodeado para probarlo con 1 jugador
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

  game_t *game =
      open_game_memory(sizeof(game_t) + (width * height * sizeof(int)));
  sync_t *sync = open_sync_memory(sizeof(sync_t));

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

    struct timeval tv;
    time_t last_move_time = time(NULL);
    

    while(!game->finished){
      receive_move();  
      //  pensar semaforos        

      //  hacemos el select  
      read_fds = active_fds;
      tv.tv_sec = timeout;
      tv.tv_usec = 0;

      int ready = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
      if(ready == -1){
        err_exit("Select error");
      }else if(ready == 0){
        game_over(game, sync);
      }else{
        if(){ //logica para ver cual jugador movio

            usleep(delay * 1000);
        }
        if(game->finished){
          game_over(game, sync);
        }
      }
    }

    
  }
}