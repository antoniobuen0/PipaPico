#include "debug_comm.h"
#include <string.h>
#include <hardware/uart.h>
#include <pico/time.h>

// Enviamos el frame binario vía UART0
void sendDebugFrame(const uint8_t* buffer, int numPipas) {
    DebugFrame frame;
    // Escoger si usar microsegundos o milisegundos
    // Por ejemplo, milisegundos desde el arranque:
    frame.timestamp = to_ms_since_boot(get_absolute_time());
    frame.numPipas  = (uint16_t)numPipas;

    // Copiar la imagen al campo data
    memcpy(frame.data, buffer, FRAME_SIZE);

    // Enviar por UART0 (asegurarse de que uart0 esté inicializado en main.c)
    uart_write_blocking(uart0, (uint8_t*)&frame, sizeof(frame));
}