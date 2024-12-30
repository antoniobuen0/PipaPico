#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

// Estructura para almacenar las propiedades de cada "pipa"
typedef struct {
    int   area;         // Número de píxeles de la región
    int   centerX;      // Centroide en X
    int   centerY;      // Centroide en Y
    float aspectRatio;  // Relación ancho/alto
    float symmetryScore;// Medida de simetría
    bool  isValid;      // Indica si cumple criterios
} PipaProps;

#endif // TYPES_H