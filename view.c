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








#include "view.h"
#include <ncurses.h>
#include <unistd.h>

// Colores para el tablero
#define COLOR_PAIR_BORDER    1
#define COLOR_PAIR_NUMBER_1  2
#define COLOR_PAIR_NUMBER_2  3
#define COLOR_PAIR_NUMBER_3  4
#define COLOR_PAIR_NUMBER_4  5
#define COLOR_PAIR_NUMBER_5  6
#define COLOR_PAIR_NUMBER_6  7
#define COLOR_PAIR_NUMBER_7  8
#define COLOR_PAIR_NUMBER_8  9
#define COLOR_PAIR_NUMBER_9  10
#define COLOR_PAIR_PLAYER    11
#define COLOR_PAIR_TITLE     12

// Función para obtener color según el valor
int get_color_pair(int value) {
    if (value <= 0) return COLOR_PAIR_NUMBER_1;
    if (value <= 1) return COLOR_PAIR_NUMBER_2;
    if (value <= 2) return COLOR_PAIR_NUMBER_3;
    if (value <= 3) return COLOR_PAIR_NUMBER_4;
    if (value <= 4) return COLOR_PAIR_NUMBER_5;
    if (value <= 5) return COLOR_PAIR_NUMBER_6;
    if (value <= 6) return COLOR_PAIR_NUMBER_7;
    if (value <= 7) return COLOR_PAIR_NUMBER_8;
    return COLOR_PAIR_NUMBER_9;
}

// Función para dibujar una celda
void draw_cell(int y, int x, int value, bool has_player) {
    int color_pair = get_color_pair(value);
    
    // Dibujar borde de la celda
    attron(COLOR_PAIR(COLOR_PAIR_BORDER) | A_BOLD);
    mvaddch(y, x, '+');
    mvaddch(y, x+4, '+');
    mvaddch(y+2, x, '+');
    mvaddch(y+2, x+4, '+');
    
    mvaddch(y, x+1, '-');
    mvaddch(y, x+2, '-');
    mvaddch(y, x+3, '-');
    
    mvaddch(y+2, x+1, '-');
    mvaddch(y+2, x+2, '-');
    mvaddch(y+2, x+3, '-');
    
    mvaddch(y+1, x, '|');
    mvaddch(y+1, x+4, '|');
    attroff(COLOR_PAIR(COLOR_PAIR_BORDER) | A_BOLD);
    
    // Dibujar valor o jugador
    if (has_player) {
        attron(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
        mvaddch(y+1, x+2, 'P');
        attroff(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
    } else {
        attron(COLOR_PAIR(color_pair) | A_BOLD);
        mvprintw(y+1, x+1, "%3d", value);
        attroff(COLOR_PAIR(color_pair) | A_BOLD);
    }
}

// Función para imprimir el tablero con ncurses
void print_board_ncurses(game_t *game) {
    clear();
    
    // Imprimir título
    attron(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    mvprintw(0, 0, "Tablero de Juego (%dx%d)", game->width, game->height);
    attroff(COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    
    // Imprimir información de jugadores
    int start_y = 2;
    int start_x = game->width * 5 + 5; // A la derecha del tablero
    
    mvprintw(start_y, start_x, "JUGADORES:");
    
    for (int i = 0; i < game->player_count; i++) {
        attron(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
        mvprintw(start_y + i + 2, start_x, "Jugador %d: ", i);
        attroff(COLOR_PAIR(COLOR_PAIR_PLAYER) | A_BOLD);
        
        mvprintw(start_y + i + 2, start_x + 11, "(%d,%d) Score: %d",
                 game->players[i].x, game->players[i].y, game->players[i].score);
        
        mvprintw(start_y + i + 3, start_x + 2, "Válidos: %d  Inválidos: %d",
                 game->players[i].valid_requests, game->players[i].invalid_requests);
    }
    
    // Imprimir tablero
    start_y = 3;
    start_x = 2;
    
    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            int cell_value = game->board[i * game->width + j];
            
            // Verificar si hay un jugador en esta celda
            bool has_player = false;
            for (int p = 0; p < game->player_count; p++) {
                if (game->players[p].x == j && game->players[p].y == i) {
                    has_player = true;
                    break;
                }
            }
            
            draw_cell(start_y + i*3, start_x + j*5, cell_value, has_player);
        }
    }
    
    // Instrucciones
    mvprintw(start_y + game->height*3 + 2, 2, "Presiona 'q' para salir");
    
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
    curs_set(0); // Ocultar cursor
    
    if (!has_colors()) {
        endwin();
        printf("Tu terminal no soporta colores.\n");
        exit(1);
    }
    
    // Definir pares de colores
    init_pair(COLOR_PAIR_BORDER, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_1, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_2, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_3, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_6, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_7, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_8, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_PAIR_NUMBER_9, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_PAIR_PLAYER, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR_TITLE, COLOR_WHITE, COLOR_BLUE);
    
    // Hacer no bloqueante la entrada
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
        
        // Verificar si el usuario presionó 'q' para salir
        // int ch = getch();
        // if (ch == 'q') {
        //     game->finished = true;
        // }
        
        if (game->finished) {
            mvprintw(LINES-2, 0, "¡Juego terminado! Presiona cualquier tecla para salir...");
            refresh();
            nodelay(stdscr, FALSE);
            getch();
            break;
        }
        
        // Pequeña pausa para reducir uso de CPU
        napms(50);
    }
    
    // Finalizar ncurses
    endwin();
    
    return 0;
}
