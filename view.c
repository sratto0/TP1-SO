// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "view.h"
#include <ncurses.h>
#include <unistd.h>

// Colores para el tablero
#define COLOR_PAIR_BOARD 1
#define COLOR_PAIR_NUMBER 2
#define COLOR_PAIR_PLAYER1 3
#define COLOR_PAIR_PLAYER2 4
#define COLOR_PAIR_PLAYER3 5
#define COLOR_PAIR_TITLE 6
#define COLOR_PAIR_INFO 7

// Función para obtener el color del jugador
int get_player_color(int player_id) {
  switch (player_id) {
  case 0:
    return COLOR_PAIR_PLAYER1;
  case 1:
    return COLOR_PAIR_PLAYER2;
  case 2:
    return COLOR_PAIR_PLAYER3;
  default:
    return COLOR_PAIR_PLAYER1;
  }
}

// Función para verificar si un jugador está bloqueado
bool is_player_blocked(game_t *game, unsigned int player_id) {
  // Un jugador está bloqueado si no tiene movimientos válidos disponibles
  int x = game->players[player_id].x;
  int y = game->players[player_id].y;

  // Direcciones: arriba, abajo, izquierda, derecha
  int dx[] = {0, 0, -1, 1};
  int dy[] = {-1, 1, 0, 0};

  for (int i = 0; i < 4; i++) {
    int new_x = x + dx[i];
    int new_y = y + dy[i];

    // Verificar si está dentro del tablero
    if (new_x >= 0 && new_x < game->width && new_y >= 0 &&
        new_y < game->height) {
      // Verificar si la celda no está ocupada por otro jugador
      bool occupied = false;
      for (unsigned int p = 0; p < game->player_count; p++) {
        if (p != player_id && game->players[p].x == new_x &&
            game->players[p].y == new_y) {
          occupied = true;
          break;
        }
      }
      if (!occupied) {
        return false; // Tiene al menos un movimiento válido
      }
    }
  }
  return true; // No tiene movimientos válidos, está bloqueado
}

// Función para dibujar el tablero limpio
void print_board_ncurses(game_t *game) {
  clear();

  // Título del juego
  attron(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
  int title_x = (COLS - 20) / 2;
  mvprintw(1, title_x, "ChompChamps Board");
  attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);

  // Calcular posición del tablero (centrado)
  int board_start_y = 3;
  int board_start_x = (COLS - (game->width * 4 + 1)) / 2;

  // Dibujar borde superior del tablero
  attron(COLOR_PAIR(COLOR_PAIR_BOARD));
  mvaddch(board_start_y, board_start_x, ACS_ULCORNER);
  for (int j = 0; j < game->width; j++) {
    mvaddch(board_start_y, board_start_x + 1 + j * 4, ACS_HLINE);
    mvaddch(board_start_y, board_start_x + 2 + j * 4, ACS_HLINE);
    mvaddch(board_start_y, board_start_x + 3 + j * 4, ACS_HLINE);
    if (j < game->width - 1) {
      mvaddch(board_start_y, board_start_x + 4 + j * 4, ACS_TTEE);
    }
  }
  mvaddch(board_start_y, board_start_x + game->width * 4, ACS_URCORNER);

  // Dibujar filas del tablero
  for (int i = 0; i < game->height; i++) {
    int row_y = board_start_y + 1 + i * 2;

    // Línea con contenido
    mvaddch(row_y, board_start_x, ACS_VLINE);
    for (int j = 0; j < game->width; j++) {
      int cell_value = game->board[i * game->width + j];
      int cell_x = board_start_x + 1 + j * 4;

      // Verificar si hay un jugador en esta celda
      bool has_player = false;
      int player_id = -1;
      for (unsigned int p = 0; p < game->player_count; p++) {
        if (game->players[p].x == j && game->players[p].y == i) {
          has_player = true;
          player_id = p;
          break;
        }
      }

      if (has_player) {
        // Mostrar jugador
        attron(COLOR_PAIR(get_player_color(player_id)) | A_BOLD);
        mvprintw(row_y, cell_x, "P%d ", player_id + 1);
        attroff(COLOR_PAIR(get_player_color(player_id)) | A_BOLD);
      } else {
        // Mostrar número de la celda
        attron(COLOR_PAIR(COLOR_PAIR_NUMBER));
        mvprintw(row_y, cell_x, "%2d ", cell_value);
        attroff(COLOR_PAIR(COLOR_PAIR_NUMBER));
      }

      mvaddch(row_y, board_start_x + 4 + j * 4, ACS_VLINE);
    }

    // Línea separadora (excepto la última fila)
    if (i < game->height - 1) {
      int sep_y = row_y + 1;
      mvaddch(sep_y, board_start_x, ACS_LTEE);
      for (int j = 0; j < game->width; j++) {
        mvaddch(sep_y, board_start_x + 1 + j * 4, ACS_HLINE);
        mvaddch(sep_y, board_start_x + 2 + j * 4, ACS_HLINE);
        mvaddch(sep_y, board_start_x + 3 + j * 4, ACS_HLINE);
        if (j < game->width - 1) {
          mvaddch(sep_y, board_start_x + 4 + j * 4, ACS_PLUS);
        }
      }
      mvaddch(sep_y, board_start_x + game->width * 4, ACS_RTEE);
    }
  }

  // Borde inferior del tablero
  int bottom_y = board_start_y + 1 + (game->height - 1) * 2 + 1;
  mvaddch(bottom_y, board_start_x, ACS_LLCORNER);
  for (int j = 0; j < game->width; j++) {
    mvaddch(bottom_y, board_start_x + 1 + j * 4, ACS_HLINE);
    mvaddch(bottom_y, board_start_x + 2 + j * 4, ACS_HLINE);
    mvaddch(bottom_y, board_start_x + 3 + j * 4, ACS_HLINE);
    if (j < game->width - 1) {
      mvaddch(bottom_y, board_start_x + 4 + j * 4, ACS_BTEE);
    }
  }
  mvaddch(bottom_y, board_start_x + game->width * 4, ACS_LRCORNER);
  attroff(COLOR_PAIR(COLOR_PAIR_BOARD));

  // Scoreboard (abajo a la izquierda)
  int score_y = bottom_y + 3;
  attron(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);
  mvprintw(score_y, 2, "Scoreboard");
  attroff(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);

  for (unsigned int i = 0; i < game->player_count; i++) {
    attron(COLOR_PAIR(get_player_color(i)));
    mvprintw(score_y + i + 1, 2, "P%d", (int)(i + 1));
    attroff(COLOR_PAIR(get_player_color(i)));

    attron(COLOR_PAIR(COLOR_PAIR_INFO));
    mvprintw(score_y + i + 1, 6, "player%d", (int)(i + 1));

    // Verificar si está bloqueado usando la función
    bool blocked = is_player_blocked(game, i);
    mvprintw(score_y + i + 1, 20, "Score: %3d  Moves: %2d/%2d [%s]",
             (int)game->players[i].score, (int)game->players[i].valid_requests,
             (int)game->players[i].invalid_requests,
             blocked ? "BLOCKED" : "ACTIVE");
    attroff(COLOR_PAIR(COLOR_PAIR_INFO));
  }

  // Leyenda (abajo a la derecha) - Solo jugadores
  int legend_x = COLS - 20;
  attron(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);
  mvprintw(score_y, legend_x, "Legend");
  attroff(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);

  for (unsigned int i = 0; i < game->player_count; i++) {
    attron(COLOR_PAIR(get_player_color(i)));
    mvprintw(score_y + 1 + i, legend_x, "P%d - Player %d", (int)(i + 1),
             (int)(i + 1));
    attroff(COLOR_PAIR(get_player_color(i)));
  }

  refresh();
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Using: %s <width> <height>\n", argv[0]);
    return 1;
  }
  
  game_t *game = open_game_memory(sizeof(game_t) + (atoi(argv[1]) * atoi(argv[2]) * sizeof(int)));
  sync_t *sync = open_sync_memory(sizeof(sync_t));

  // Inicializar ncurses
  setenv("TERM", "xterm-256color", 1);
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);

  if (!has_colors()) {
    endwin();
    printf("Your terminal does not support colors.\n");
    exit(1);
  }

  // Definir pares de colores
  init_pair(COLOR_PAIR_BOARD, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_PAIR_NUMBER, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_PAIR_PLAYER1, COLOR_BLACK, COLOR_RED);
  init_pair(COLOR_PAIR_PLAYER2, COLOR_BLACK, COLOR_YELLOW);
  init_pair(COLOR_PAIR_PLAYER3, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(COLOR_PAIR_TITLE, COLOR_WHITE, COLOR_BLUE);
  init_pair(COLOR_PAIR_INFO, COLOR_CYAN, COLOR_BLACK);

  nodelay(stdscr, TRUE);

  while (true) {
    sem_wait_check(&sync->master_to_view);

    print_board_ncurses(game);
    
    sem_post_check(&sync->view_to_master); 
    
    if (game->finished) {
      mvprintw(LINES - 2, 0, "Game over! Press any key to exit...");
      refresh();
      nodelay(stdscr, FALSE);
      getch();
      break;
    }
    napms(50);
  }

  endwin();
  return 0;
}
