#ifndef MASTER_LIB_H
#define MASTER_LIB_H

#include "structs.h"
#include "utils.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_DIGITS 15 // CHEQUEAR!!
#define MAX_PLAYERS 9

extern char **environ;
extern const int directions[8][2];

void init_sync(sync_t *sync);
void init_game(game_t *game, unsigned short width, unsigned short height,
               unsigned int player_count);
void init_semaphore(sem_t *sem, int value);
void check_player_count(int count);
bool has_valid_moves(game_t *game, player_t *player);
bool is_valid_move(int x, int y, game_t *game);
void fill_board(game_t *game);
void init_players(game_t *game, int players_fds[][2], int player_count,
                  int *max_fd);
void close_sems(sync_t *sync, unsigned int player_count);
void close_not_needed_fds(int players_fds[][2], int player_count,
                          int current_player);
void safe_close(int fd);
int game_ended(game_t *game);
void game_over(game_t *game, sync_t *sync);
int receive_move(int fd, unsigned char *dir);
bool execute_move(game_t *game, sync_t *sync, int turno, unsigned char dir);
void player_pos(game_t *game);
bool any_player_can_move(game_t *game);
void get_arguments(int argc, char *argv[], unsigned short *width,
                   unsigned short *height, unsigned int *delay,
                   unsigned int *timeout, unsigned int *seed, char **view_path,
                   char **players_paths, int *player_count);
void print_configuration(unsigned short width, unsigned short height,
                         unsigned int delay, unsigned int timeout,
                         unsigned int seed, char *view_path,
                         char **players_paths, int player_count);
void create_view(char *path, char *width, char *height, pid_t *pid);
void create_players(char **paths, int fds[][2], char *width, char *height,
                    int player_count, game_t *game);
void process_players(game_t *game, sync_t *sync, int player_count,
                     int players_fds[][2], fd_set read_fds, int *last_served,
                     time_t *last_move_time, unsigned int delay);
void sync_with_view(sync_t *sync, unsigned int delay);
void argument_amount_check(int argc);
void check_player_count(int player_count);
bool timeout_check(time_t last_move_time, unsigned int timeout, game_t *game, sync_t *sync);
void signal_all_players_ready(game_t *game, sync_t *sync, int player_count);

#endif