#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>
#include <stdbool.h> 
#include <semaphore.h> 

typedef struct {
    char name[16]; // Nombre del jugador
    unsigned int score; // Puntaje
    unsigned int invalid_requests; // Cantidad de solicitudes de movimientos inválidas realizadas
    unsigned int valid_requests; // Cantidad de solicitudes de movimientos válidas realizadas
    unsigned short x, y; // Coordenadas x e y en el tablero
    pid_t process_id; // Identificador de proceso
    bool blocked; // Indica si el jugador está bloqueado
} player;

typedef struct {
    unsigned short width; // Ancho del tablero
    unsigned short height; // Alto del tablero
    unsigned int player_count; // Cantidad de jugadores
    player players[9]; // Lista de jugadores
    bool finish; // Indica si el juego se ha terminado
    int board[]; // Puntero al comienzo del tablero. fila-0, fila-1, ..., fila-n-1
} game;

typedef struct {
    sem_t A; // El máster le indica a la vista que hay cambios por imprimir
    sem_t B; // La vista le indica al máster que terminó de imprimir
    sem_t C; // Mutex para evitar inanición del máster al acceder al estado
    sem_t D; // Mutex para el estado del juego
    sem_t E; // Mutex para la siguiente variable
    unsigned int F; // Cantidad de jugadores leyendo el estado
    sem_t G[9]; // Le indican a cada jugador que puede enviar 1 movimiento
} ZZZ;

#endif