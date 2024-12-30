#ifndef DETECTION_H
#define DETECTION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Detecta "pipas" en processedBuffer, etiquetando en labelBuffer.
// Retorna el número de pipas detectadas en *numPipas.
int detectPipas(int* numPipas);

#ifdef __cplusplus
}
#endif

#endif // DETECTION_H