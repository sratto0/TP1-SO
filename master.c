#include "master.h"

int main(int argc, char *argv[]) {
  game_t *game = open_game_memory();
  sync_t *sync = open_sync_memory();

  init_semaphores(sync);
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
  for(unsigned int i = 0; i < game->player_count; i++){
    char *argv_player[] = {players_paths[i], argv_width, argv_height, NULL};
    if(pipe(player_pipes[i]) == -1){
      err_exit("pipe");
    }
    pid_t cpid_player = fork();
    if (cpid_player == -1) {
      err_exit("fork");
    }
    if (cpid_player == 0) {
      if(close(player_pipes[i][0])==-1) {
        err_exit("close");
      }  //cierra read end para child

      if(dup2(player_pipes[i][1], STDOUT_FILENO) == -1){
        err_exit("dup2");
      }

      if(close(player_pipes[i][1])) {
        err_exit("close");
      }  // ya no lo necesita, lo tiene en stdout
      
      
      execve(argv_player[0], argv_player, NULL);
    } else {
      if(close(player_pipes[i][1]) == -1){
        err_exit("close");
      }  //cierra write end para parent
      game->players[i].process_id = cpid_player;
      game->players[i].blocked = false;
    }
  }

  while(true){
    //recibir movimientos 

    sem_wait(&sync->writer_lock);
    sem_wait(&sync->state_lock);
    sem_post(&sync->writer_lock);

    //ejecutar movimientos

    sem_post(&sync->state_lock);
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

void init_game(game_t *game) {
  game->width = 10;
  game->height = 10;
}

