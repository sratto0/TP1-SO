// #include "view.h"

// void print_board(game_t *game) {
//   for (int j = 0; j < game->width; j++) {
//     printf(" ___");
//   }

//   printf("\n");

//   for (int i = 0; i < game->height; i++) {
//     for (int j = 0; j < game->width; j++) {
//       int cell_num = game->board[i * game->width + j];
//       printf("|%3d", cell_num);
//     }
//     printf("|\n");
//     for (int j = 0; j < game->width; j++) {
//       printf("|___");
//     }
//     printf("|\n");
//   }
// }

// int main() {
//   game_t *game = open_game_memory();
//   sync_t *sync = open_sync_memory();

//   while (true) {
//     if (sem_wait(&sync->master_to_view) == -1) {
//       if (errno == EINTR)
//         continue;
//       perror("sem_wait master_to_view");
//       break;
//     }

//     print_board(game);

//     if (sem_post(&sync->view_to_master) == -1) {
//       perror("sem_post view_to_master");
//       break;
//     }

//     if (game->finished) {
//       break;
//     }
//   }
// }








// #include "view.h"
// #include <ncurses.h>
// #include <unistd.h>

// // Colores para el tablero
// #define COLOR_PAIR_BORDER    1
// #define COLOR_PAIR_NUMBER_1  2
// #define COLOR_PAIR_NUMBER_2  3
// #define COLOR_PAIR_NUMBER_3  4
// #define COLOR_PAIR_NUMBER_4  5
// #define COLOR_PAIR_NUMBER_5  6
// #define COLOR_PAIR_NUMBER_6  7
// #define COLOR_PAIR_NUMBER_7  8
// #define COLOR_PAIR_NUMBER_8  9
// #define COLOR_PAIR_NUMBER_9  10
// #define COLOR_PAIR_PLAYER    11
// #define COLOR_PAIR_TITLE     12

// // Función para obtener color según el valor
// int get_color_pair(int value) {
//     if (value <= 0) return COLOR_PAIR_NUMBER_1;
//     if (value <= 1) return COLOR_PAIR_NUMBER_2;
//     if (value <= 2) return COLOR_PAIR_NUMBER_3;
//     if (value <= 3) return COLOR_PAIR_NUMBER_4;
//     if (value <= 4) return COLOR_PAIR_NUMBER_5;
//     if (value <= 5) return COLOR_PAIR_NUMBER_6;
//     if (value <= 6) return COLOR_PAIR_NUMBER_7;
//     if (value <= 7) return COLOR_PAIR_NUMBER_8;
//     return COLOR_PAIR_NUMBER_9;
// }

// // Función para dibujar una celda
// void draw_cell(int y, int x, int value, bool has_player) {
//     int color_pair = get_color_pair(value);
    
//     // Dibujar borde de la celda
//     attron(COLOR_PAIR(COLOR_PAIR_BORDER) | A_BOLD);
//     mvaddch(y, x, '+');
//     mvaddch(y, x+4, '+');
//     mvaddch(y+2, x, '+');
//     mvaddch(y+2, x+4, '+');
    
//     mvaddch(y, x+1, '-');
//     mvaddch(y, x+2, '-');
//     mvaddch(y, x+3, '-');
    
//     mvaddch(y+2, x+1, '-');
//     mvaddch(y+2, x+2, '-');
//     mvaddch(y+2, x+3, '-');
    
//     mvaddch(y+1, x, '|');
//     mvaddch(y+1, x+4, '|');
//     attroff(COLOR_PAIR(COLOR_PAIR_BORDER) | A_BOLD);
    
//     // Dibujar valor o jugador
//     if (has_player) {
//         attron(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
//         mvaddch(y+1, x+2, 'P');
//         attroff(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
//     } else {
//         attron(COLOR_PAIR(color_pair) | A_BOLD);
//         mvprintw(y+1, x+1, "%3d", value);
//         attroff(COLOR_PAIR(color_pair) | A_BOLD);
//     }
// }

// // Función para imprimir el tablero con ncurses
// void print_board_ncurses(game_t *game) {
//     clear();
    
//     // Imprimir título
//     attron(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
//     mvprintw(0, 0, "Tablero de Juego (%dx%d)", game->width, game->height);
//     attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    
//     // Imprimir información de jugadores
//     int start_y = 2;
//     int start_x = game->width * 5 + 5; // A la derecha del tablero
    
//     mvprintw(start_y, start_x, "JUGADORES:");
    
//     for (int i = 0; i < game->player_count; i++) {
//         attron(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
//         mvprintw(start_y + i + 2, start_x, "Jugador %d: ", i);
//         attroff(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
        
//         mvprintw(start_y + i + 2, start_x + 11, "(%d,%d) Score: %d",
//                  game->players[i].x, game->players[i].y, game->players[i].score);
        
//         mvprintw(start_y + i + 3, start_x + 2, "Válidos: %d  Inválidos: %d",
//                  game->players[i].valid_requests, game->players[i].invalid_requests);
//     }
    
//     // Imprimir tablero
//     start_y = 3;
//     start_x = 2;
    
//     for (int i = 0; i < game->height; i++) {
//         for (int j = 0; j < game->width; j++) {
//             int cell_value = game->board[i * game->width + j];
            
//             // Verificar si hay un jugador en esta celda
//             bool has_player = false;
//             for (int p = 0; p < game->player_count; p++) {
//                 if (game->players[p].x == j && game->players[p].y == i) {
//                     has_player = true;
//                     break;
//                 }
//             }
            
//             draw_cell(start_y + i*3, start_x + j*5, cell_value, has_player);
//         }
//     }
    
//     // Instrucciones
//     mvprintw(start_y + game->height*3 + 2, 2, "Presiona 'q' para salir");
    
//     refresh();
// }

// int main() {
//     game_t *game = open_game_memory();
//     sync_t *sync = open_sync_memory();
    
//     // Inicializar ncurses

//     setenv("TERM", "xterm-256color", 1);
//     initscr();
//     start_color();
//     cbreak();
//     noecho();
//     keypad(stdscr, TRUE);
//     curs_set(0); // Ocultar cursor
    
//     if (!has_colors()) {
//         endwin();
//         printf("Tu terminal no soporta colores.\n");
//         exit(1);
//     }
    
//     // Definir pares de colores
//     init_pair(COLOR_PAIR_BORDER, COLOR_WHITE, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_1, COLOR_BLUE, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_2, COLOR_GREEN, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_3, COLOR_CYAN, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_4, COLOR_YELLOW, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_5, COLOR_MAGENTA, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_6, COLOR_RED, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_7, COLOR_GREEN, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_8, COLOR_BLUE, COLOR_BLACK);
//     init_pair(COLOR_PAIR_NUMBER_9, COLOR_RED, COLOR_BLACK);
//     init_pair(COLOR_PAIR_PLAYER, COLOR_BLACK, COLOR_WHITE);
//     init_pair(COLOR_PAIR_TITLE, COLOR_WHITE, COLOR_BLUE);
    
//     // Hacer no bloqueante la entrada
//     nodelay(stdscr, TRUE);
    
//     while (true) {
//         if (sem_wait(&sync->master_to_view) == -1) {
//             if (errno == EINTR)
//                 continue;
//             perror("sem_wait master_to_view");
//             break;
//         }
        
//         print_board_ncurses(game);
        
//         if (sem_post(&sync->view_to_master) == -1) {
//             perror("sem_post view_to_master");
//             break;
//         }
        
//         // Verificar si el usuario presionó 'q' para salir
//         // int ch = getch();
//         // if (ch == 'q') {
//         //     game->finished = true;
//         // }
        
//         if (game->finished) {
//             mvprintw(LINES-2, 0, "¡Juego terminado! Presiona cualquier tecla para salir...");
//             refresh();
//             nodelay(stdscr, FALSE);
//             getch();
//             break;
//         }
        
//         // Pequeña pausa para reducir uso de CPU
//         napms(50);
//     }
    
//     // Finalizar ncurses
//     endwin();
    
//     return 0;
// }

#include "view.h"
#include <ncurses.h>
#include <unistd.h>

// Colores para el tablero
#define COLOR_PAIR_BOARD     1
#define COLOR_PAIR_NUMBER    2
#define COLOR_PAIR_PLAYER1   3
#define COLOR_PAIR_PLAYER2   4
#define COLOR_PAIR_PLAYER3   5
#define COLOR_PAIR_TITLE     6
#define COLOR_PAIR_INFO      7

// Función para obtener el color del jugador
int get_player_color(int player_id) {
    switch(player_id) {
        case 0: return COLOR_PAIR_PLAYER1;
        case 1: return COLOR_PAIR_PLAYER2;
        case 2: return COLOR_PAIR_PLAYER3;
        default: return COLOR_PAIR_PLAYER1;
    }
}

// Función para verificar si un jugador está bloqueado
bool is_player_blocked(game_t *game, int player_id) {
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
        if (new_x >= 0 && new_x < game->width && new_y >= 0 && new_y < game->height) {
            // Verificar si la celda no está ocupada por otro jugador
            bool occupied = false;
            for (int p = 0; p < game->player_count; p++) {
                if (p != player_id && game->players[p].x == new_x && game->players[p].y == new_y) {
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
            for (int p = 0; p < game->player_count; p++) {
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
    
    for (int i = 0; i < game->player_count; i++) {
        attron(COLOR_PAIR(get_player_color(i)));
        mvprintw(score_y + i + 1, 2, "P%d", i + 1);
        attroff(COLOR_PAIR(get_player_color(i)));
        
        attron(COLOR_PAIR(COLOR_PAIR_INFO));
        mvprintw(score_y + i + 1, 6, "player%d", i + 1);
        
        // Verificar si está bloqueado usando la función
        bool blocked = is_player_blocked(game, i);
        mvprintw(score_y + i + 1, 20, "Score: %3d  Moves: %2d/%2d [%s]",
                 game->players[i].score,
                 game->players[i].valid_requests,
                 game->players[i].invalid_requests,
                 blocked ? "BLOCKED" : "ACTIVE");
        attroff(COLOR_PAIR(COLOR_PAIR_INFO));
    }
    
    // Leyenda (abajo a la derecha) - Solo jugadores
    int legend_x = COLS - 20;
    attron(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);
    mvprintw(score_y, legend_x, "Legend");
    attroff(COLOR_PAIR(COLOR_PAIR_INFO) | A_BOLD);
    
    for (int i = 0; i < game->player_count; i++) {
        attron(COLOR_PAIR(get_player_color(i)));
        mvprintw(score_y + 1 + i, legend_x, "P%d - Player %d", i + 1, i + 1);
        attroff(COLOR_PAIR(get_player_color(i)));
    }
    
    refresh();
}

int main() {
    game_t *game = open_game_memory();
    sync_t *sync = open_sync_memory();
    
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
        printf("Tu terminal no soporta colores.\n");
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
        if (sem_wait(&sync->master_to_view) == -1) {
            if (errno == EINTR)
                continue;
            perror("sem_wait master_to_view");
            break;
        }
        
        print_board_ncurses(game);
        
        if (sem_post(&sync->view_to_master) == -1) {
            perror("sem_post view_to_master");
            break;
        }
        
        if (game->finished) {
            mvprintw(LINES-2, 0, "¡Juego terminado! Presiona cualquier tecla para salir...");
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
