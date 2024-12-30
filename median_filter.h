#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include <stdint.h>

// Filtra el frameBuffer (global o externo) con un filtro de mediana 3x3
void optimizedMedianFilter(void);

#endif // MEDIAN_FILTER_H