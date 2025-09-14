# TP1-SO
Este proyecto implementa un juego llamado Chomp Champs, donde múltiples jugadores compiten en un tablero. El juego utiliza memoria compartida y semáforos para la sincronización entre procesos.

## Instrucciones de Compilación y Ejecución
Sigue los pasos a continuación para compilar y ejecutar el proyecto correctamente dentro del contenedor Docker provisto por la cátedra:

1. Dirígete al directorio raíz del proyecto en el contenedor Docker
2. Para compilar el proyecto utiliza:
`make`
3. Para ejecutar el proyecto utiliza:
`./master [parámetros]`

## Parámetros
A continuación se listan los parámetros que acepta el máster. Los parámetros entre corchetes son opcionales y tienen un valor por defecto.
- `[-w width]`: Ancho del tablero. Default y mínimo: 10
- `[-h height]`: Alto del tablero. Default y mínimo: 10
- `[-d delay]`: milisegundos que espera el máster cada vez que se imprime el estado. Default: 200
- `[-t timeout]`: Timeout en segundos para recibir solicitudes de movimientos válidos. Default: 10
- `[-s seed]`: Semilla utilizada para la generación del tablero. Default: time(NULL)
- `[-v view]`: Ruta del binario de la vista. Default: Sin vista.
- `-p player`: Ruta/s de los binarios de los jugadores. Mínimo: 1 Máximo: 9
    Ejemplo:
    `./master -p player player player`

## Requisitos
Docker: El contenedor Docker provisto por la cátedra es necesario para ejecutar el entorno de compilación y pruebas de manera controlada.

## Integrantes:
Castelnuovo, Carolina Alessia (65539)
Ravera, Josefina (65690)
Ratto, Sofia Ines (63621)