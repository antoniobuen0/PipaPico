#ifndef DEBUG_COMM_H
#define DEBUG_COMM_H

#include <stdint.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Estructura para enviar un frame de depuración binario
typedef struct __attribute__((packed)) {
    uint32_t timestamp;         // Microsegundos o ms según preferencia
    uint16_t numPipas;          // Conteo de objetos detectados
    uint8_t  data[FRAME_SIZE];  // Buffer de imagen
} DebugFrame;

// Envía un frame de depuración a través de UART0 en formato binario
void sendDebugFrame(const uint8_t* buffer, int numPipas);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_COMM_H