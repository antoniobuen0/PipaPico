#ifndef CONFIG_H
#define CONFIG_H

// Resolución de la imagen
#define WIDTH  160
#define HEIGHT 120
#define FRAME_SIZE (WIDTH * HEIGHT)

// Parámetros para detección y filtrado
#define MAX_PIPAS             100
#define MEDIAN_WINDOW         3
#define SYMMETRY_SAMPLE_RATE  4  // Muestrear 1 de cada 4 píxeles para simetría

// Límites de "pipa"
#define MIN_PIPA_AREA     10
#define MAX_PIPA_AREA     2000
#define MIN_ASPECT_RATIO  0.5f
#define MAX_ASPECT_RATIO  2.0f

// Para dimensionar el stack de flood fill
#define MAX_STACK_SIZE (MAX_PIPA_AREA * 2)

#endif // CONFIG_H