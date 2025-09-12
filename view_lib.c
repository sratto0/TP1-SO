#include "view_lib.h"
#include <ncurses.h>

int get_player_color(int player_id) {
    // Asume que COLOR_PAIR_PLAYER1 es el primer valor consecutivo
    return COLOR_PAIR_PLAYER1 + player_id;
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
  int board_width = game->width * 4 + 1;
  int board_start_x = (COLS - board_width) / 2;

  // Dibujar borde superior del tablero
  attron(COLOR_PAIR(COLOR_PAIR_BOARD));
  mvaddch(board_start_y, board_start_x, ACS_ULCORNER);
  for (int j = 0; j < game->width; j++) {
    for (int k = 0; k < 3; k++) {
      mvaddch(board_start_y, board_start_x + 1 + j * 4 + k, ACS_HLINE);
    }
    if (j < game->width - 1) {
      mvaddch(board_start_y, board_start_x + 4 + j * 4, ACS_TTEE);
    }
  }
  mvaddch(board_start_y, board_start_x + board_width - 1, ACS_URCORNER);
  attroff(COLOR_PAIR(COLOR_PAIR_BOARD));

  // Dibujar filas del tablero
  for (int i = 0; i < game->height; i++) {
    int row_y = board_start_y + 1 + i * 2;

    // Línea con contenido
    attron(COLOR_PAIR(COLOR_PAIR_BOARD));
    mvaddch(row_y, board_start_x, ACS_VLINE);
    attroff(COLOR_PAIR(COLOR_PAIR_BOARD));

    for (int j = 0; j < game->width; j++) {
      int cell_value = game->board[i * game->width + j];
      int cell_x = board_start_x + 1 + j * 4;

      // Verificar si hay un jugador en esta posición actual
      bool current_player_here = false;
      int current_player_id = -1;
      for (unsigned int p = 0; p < game->player_count; p++) {
        if (game->players[p].x == j && game->players[p].y == i) {
          current_player_here = true;
          current_player_id = p;
          break;
        }
      }

      if (current_player_here) {
        // Mostrar jugador actual
        attron(COLOR_PAIR(get_player_color(current_player_id)) | A_BOLD);
        mvprintw(row_y, cell_x, "P%d", current_player_id);
        attroff(COLOR_PAIR(get_player_color(current_player_id)) | A_BOLD);
      } else if (cell_value < 0) {
        // Celda visitada por jugador (mostrar con color del jugador)
        int player_id = -cell_value;
        attron(COLOR_PAIR(get_player_color(player_id)));
        mvprintw(row_y, cell_x, "##");
        attroff(COLOR_PAIR(get_player_color(player_id)));
      } else {
        // Celda con número
        attron(COLOR_PAIR(COLOR_PAIR_NUMBER));
        mvprintw(row_y, cell_x, "%2d", cell_value);
        attroff(COLOR_PAIR(COLOR_PAIR_NUMBER));
      }

      // Borde vertical derecho de la celda
      attron(COLOR_PAIR(COLOR_PAIR_BOARD));
      mvaddch(row_y, board_start_x + 4 + j * 4, ACS_VLINE);
      attroff(COLOR_PAIR(COLOR_PAIR_BOARD));
    }

    // Línea separadora horizontal (excepto la última fila)
    if (i < game->height - 1) {
      int sep_y = row_y + 1;
      attron(COLOR_PAIR(COLOR_PAIR_BOARD));
      mvaddch(sep_y, board_start_x, ACS_LTEE);
      
      for (int j = 0; j < game->width; j++) {
        for (int k = 0; k < 3; k++) {
          mvaddch(sep_y, board_start_x + 1 + j * 4 + k, ACS_HLINE);
        }
        if (j < game->width - 1) {
          mvaddch(sep_y, board_start_x + 4 + j * 4, ACS_PLUS);
        }
      }
      
      mvaddch(sep_y, board_start_x + board_width - 1, ACS_RTEE);
      attroff(COLOR_PAIR(COLOR_PAIR_BOARD));
    }
  }

  // Borde inferior del tablero
  int bottom_y = board_start_y + 1 + (game->height - 1) * 2 + 1;
  attron(COLOR_PAIR(COLOR_PAIR_BOARD));
  mvaddch(bottom_y, board_start_x, ACS_LLCORNER);
  for (int j = 0; j < game->width; j++) {
    for (int k = 0; k < 3; k++) {
      mvaddch(bottom_y, board_start_x + 1 + j * 4 + k, ACS_HLINE);
    }
    if (j < game->width - 1) {
      mvaddch(bottom_y, board_start_x + 4 + j * 4, ACS_BTEE);
    }
  }
  mvaddch(bottom_y, board_start_x + board_width - 1, ACS_LRCORNER);
  attroff(COLOR_PAIR(COLOR_PAIR_BOARD));

  // Imprimir scoreboard debajo del tablero
  print_scoreboard_below_board(game, bottom_y + 2);

  refresh();
// ...existing code...

  // // Calcular posición del tablero (centrado)
  // int board_start_y = 3;
  // int board_start_x = (COLS - (game->width * 4 + 1)) / 2;

  // // Dibujar borde superior del tablero
  // attron(COLOR_PAIR(COLOR_PAIR_BOARD));
  // mvaddch(board_start_y, board_start_x, ACS_ULCORNER);
  // for (int j = 0; j < game->width; j++) {
  //   mvaddch(board_start_y, board_start_x + 1 + j * 4, ACS_HLINE);
  //   mvaddch(board_start_y, board_start_x + 2 + j * 4, ACS_HLINE);
  //   mvaddch(board_start_y, board_start_x + 3 + j * 4, ACS_HLINE);
  //   if (j < game->width - 1) {
  //     mvaddch(board_start_y, board_start_x + 4 + j * 4, ACS_TTEE);
  //   }
  // }
  // mvaddch(board_start_y, board_start_x + game->width * 4, ACS_URCORNER);

  // // Dibujar filas del tablero
  // for (int i = 0; i < game->height; i++) {
  //   int row_y = board_start_y + 1 + i * 2;

  //   // Línea con contenido
  //   mvaddch(row_y, board_start_x, ACS_VLINE);
  //   for (int j = 0; j < game->width; j++) {
  //     int cell_value = game->board[i * game->width + j];
  //     int cell_x = board_start_x + 1 + j * 4;

  //     // Verificar si hay un jugador en esta celda
  //     bool has_player = false;
  //     int player_id = -1;
  //     for (unsigned int p = 0; p < game->player_count; p++) {
  //       if (game->players[p].x == j && game->players[p].y == i) {
  //         has_player = true;
  //         player_id = p;
  //         break;
  //       }
  //     }

  //     if (has_player) {
  //       // Mostrar jugador
  //       attron(COLOR_PAIR(get_player_color(player_id)) | A_BOLD);
  //       mvprintw(row_y, cell_x, "P%d ", player_id + 1);
  //       attroff(COLOR_PAIR(get_player_color(player_id)) | A_BOLD);
  //     } else {
  //       // Mostrar número de la celda
  //       attron(COLOR_PAIR(COLOR_PAIR_NUMBER));
  //       mvprintw(row_y, cell_x, "%2d ", cell_value);
  //       attroff(COLOR_PAIR(COLOR_PAIR_NUMBER));
  //     }

  //     mvaddch(row_y, board_start_x + 4 + j * 4, ACS_VLINE);
  //   }

  //   // Línea separadora (excepto la última fila)
  //   if (i < game->height - 1) {
  //     int sep_y = row_y + 1;
  //     mvaddch(sep_y, board_start_x, ACS_LTEE);
  //     for (int j = 0; j < game->width; j++) {
  //       mvaddch(sep_y, board_start_x + 1 + j * 4, ACS_HLINE);
  //       mvaddch(sep_y, board_start_x + 2 + j * 4, ACS_HLINE);
  //       mvaddch(sep_y, board_start_x + 3 + j * 4, ACS_HLINE);
  //       if (j < game->width - 1) {
  //         mvaddch(sep_y, board_start_x + 4 + j * 4, ACS_PLUS);
  //       }
  //     }
  //     mvaddch(sep_y, board_start_x + game->width * 4, ACS_RTEE);
  //   }
  // }

  // // Borde inferior del tablero
  // int bottom_y = board_start_y + 1 + (game->height - 1) * 2 + 1;
  // mvaddch(bottom_y, board_start_x, ACS_LLCORNER);
  // for (int j = 0; j < game->width; j++) {
  //   mvaddch(bottom_y, board_start_x + 1 + j * 4, ACS_HLINE);
  //   mvaddch(bottom_y, board_start_x + 2 + j * 4, ACS_HLINE);
  //   mvaddch(bottom_y, board_start_x + 3 + j * 4, ACS_HLINE);
  //   if (j < game->width - 1) {
  //     mvaddch(bottom_y, board_start_x + 4 + j * 4, ACS_BTEE);
  //   }
  // }
  // mvaddch(bottom_y, board_start_x + game->width * 4, ACS_LRCORNER);
  // attroff(COLOR_PAIR(COLOR_PAIR_BOARD));

  // // Scoreboard (abajo a la izquierda)
  // int score_y = bottom_y + 3;
  // attron(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);
  // mvprintw(score_y, 2, "Scoreboard");
  // attroff(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);

  // for (unsigned int i = 0; i < game->player_count; i++) {
  //   attron(COLOR_PAIR(get_player_color(i)));
  //   mvprintw(score_y + i + 1, 2, "P%d", (int)(i + 1));
  //   attroff(COLOR_PAIR(get_player_color(i)));

  //   attron(COLOR_PAIR(COLOR_PAIR_INFO));
  //   mvprintw(score_y + i + 1, 6, "player%d", (int)(i + 1));

  //   // Verificar si está bloqueado usando la función
  //   bool blocked = game->players[i].blocked;
  //   mvprintw(score_y + i + 1, 20, "Score: %3d  Moves: %2d/%2d [%s]",
  //            (int)game->players[i].score, (int)game->players[i].valid_requests,
  //            (int)game->players[i].invalid_requests,
  //            blocked ? "BLOCKED" : "ACTIVE");
  //   attroff(COLOR_PAIR(COLOR_PAIR_INFO));
  // }

  // // Leyenda (abajo a la derecha) - Solo jugadores
  // int legend_x = COLS - 20;
  // attron(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);
  // mvprintw(score_y, legend_x, "Legend");
  // attroff(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);

  // for (unsigned int i = 0; i < game->player_count; i++) {
  //   attron(COLOR_PAIR(get_player_color(i)));
  //   mvprintw(score_y + 1 + i, legend_x, "P%d - Player %d", (int)(i + 1),
  //            (int)(i + 1));
  //   attroff(COLOR_PAIR(get_player_color(i)));
  // }

  // refresh();
}

void print_scoreboard(game_t *game) {
      int score_start_x = 2;
    
    attron(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    mvprintw(start_y, score_start_x, "=== SCOREBOARD ===");
    attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    
    // Información de cada jugador
    for (unsigned int i = 0; i < game->player_count; i++) {
        int y = start_y + 2 + i;
        
        // Color del jugador
        attron(COLOR_PAIR(get_player_color(i)) | A_BOLD);
        mvprintw(y, score_start_x, "P%d", i);
        attroff(COLOR_PAIR(get_player_color(i)) | A_BOLD);
        
        // Información del jugador
        attron(COLOR_PAIR(COLOR_PAIR_INFO));
        mvprintw(y, score_start_x + 4, "Score: %3d  Valid: %2d  Invalid: %2d  [%s]",
                 game->players[i].score,
                 game->players[i].valid_requests,
                 game->players[i].invalid_requests,
                 game->players[i].blocked ? "BLOCKED" : "ACTIVE");
        attroff(COLOR_PAIR(COLOR_PAIR_INFO));
    }
    
    // Información del juego
    attron(COLOR_PAIR(COLOR_PAIR_INFO));
    mvprintw(start_y + game->player_count + 4, score_start_x, 
             "Board: %dx%d  |  Status: %s", 
             game->width, game->height,
             game->finished ? "FINISHED" : "PLAYING");
    attroff(COLOR_PAIR(COLOR_PAIR_INFO));
    // int score_start_y = game->height + 4;
    // int score_start_x = 0;
    
    // // Título del scoreboard
    // attron(COLOR_PAIR(COLOR_PAIR_TITLE));
    // mvprintw(score_start_y, score_start_x, "=== SCOREBOARD ===");
    // attroff(COLOR_PAIR(COLOR_PAIR_TITLE));
    
    // // Información de cada jugador
    // for (unsigned int i = 0; i < game->player_count; i++) {
    //     int y = score_start_y + 2 + i;
        
    //     // Color del jugador
    //     attron(COLOR_PAIR(get_player_color(i)));
    //     mvprintw(y, score_start_x, "Player %d:", i);
    //     attroff(COLOR_PAIR(get_player_color(i)));
        
    //     // Información del jugador
    //     attron(COLOR_PAIR(COLOR_PAIR_INFO));
    //     mvprintw(y, score_start_x + 12, "Score: %3d  Valid: %2d  Invalid: %2d  [%s]",
    //              game->players[i].score,
    //              game->players[i].valid_requests,
    //              game->players[i].invalid_requests,
    //              game->players[i].blocked ? "BLOCKED" : "ACTIVE");
    //     attroff(COLOR_PAIR(COLOR_PAIR_INFO));
    // }
    
    // // Información del juego
    // attron(COLOR_PAIR(COLOR_PAIR_INFO));
    // mvprintw(score_start_y + game->player_count + 4, score_start_x, 
    //          "Board: %dx%d  |  Status: %s", 
    //          game->width, game->height,
    //          game->finished ? "FINISHED" : "PLAYING");
    // attroff(COLOR_PAIR(COLOR_PAIR_INFO));
}

// void print_scoreboard_below_board(game_t *game, int start_y) {
//     int score_start_x = 2;
    
//     attron(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
//     mvprintw(start_y, score_start_x, "=== SCOREBOARD ===");
//     attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    
//     // Información de cada jugador
//     for (unsigned int i = 0; i < game->player_count; i++) {
//         int y = start_y + 2 + i;
        
//         // Color del jugador
//         attron(COLOR_PAIR(get_player_color(i)) | A_BOLD);
//         mvprintw(y, score_start_x, "P%d", i);
//         attroff(COLOR_PAIR(get_player_color(i)) | A_BOLD);
        
//         // Información del jugador
//         attron(COLOR_PAIR(COLOR_PAIR_INFO));
//         mvprintw(y, score_start_x + 4, "Score: %3d  Valid: %2d  Invalid: %2d  [%s]",
//                  game->players[i].score,
//                  game->players[i].valid_requests,
//                  game->players[i].invalid_requests,
//                  game->players[i].blocked ? "BLOCKED" : "ACTIVE");
//         attroff(COLOR_PAIR(COLOR_PAIR_INFO));
//     }
    
//     // Información del juego
//     attron(COLOR_PAIR(COLOR_PAIR_INFO));
//     mvprintw(start_y + game->player_count + 4, score_start_x, 
//              "Board: %dx%d  |  Status: %s", 
//              game->width, game->height,
//              game->finished ? "FINISHED" : "PLAYING");
//     attroff(COLOR_PAIR(COLOR_PAIR_INFO));
// }