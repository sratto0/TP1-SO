#ifndef STRUCTS_H
#define STRUCTS_H

#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
  char name[16];                 // Nombre del jugador
  unsigned int score;            // Puntaje
  unsigned int invalid_requests; // Cantidad de solicitudes de movimientos
                                 // inválidas realizadas
  unsigned int valid_requests; // Cantidad de solicitudes de movimientos válidas
                               // realizadas
  unsigned short x, y; // Coordenadas x e y en el tablero
  pid_t process_id;    // Identificador de proceso
  bool blocked;        // Indica si el jugador está bloqueado
} player_t;

typedef struct {
  unsigned short width;      // Ancho del tablero
  unsigned short height;     // Alto del tablero
  unsigned int player_count; // Cantidad de jugadores
  player_t players[9];       // Lista de jugadores
  bool finished;             // Indica si el juego se ha terminado
  int board[]; // Puntero al comienzo del tablero. fila-0, fila-1, ..., fila-n-1
} game_t;

typedef struct {
  sem_t master_to_view; // El máster le indica a la vista que hay cambios por
                        // imprimir
  sem_t view_to_master; // La vista le indica al máster que terminó de imprimir
  sem_t writer_lock;   // Mutex para evitar inanición del máster al acceder al
                       // estado
  sem_t state_lock;    // Mutex para el estado del juego
  sem_t readers_mutex; // Mutex para la siguiente variable
  unsigned int readers_count; // Cantidad de jugadores leyendo el estado
  sem_t players_ready[9];     // Le indican a cada jugador que puede enviar 1
                              // movimiento
} sync_t;

#endif