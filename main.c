#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>
#include <hardware/sync.h>
#include <hardware/i2c.h>

#include "config.h"
#include "types.h"
#include "median_filter.h"
#include "detection.h"
#include "symmetry.h"
#include "debug_comm.h"

// ---------------------------------------------------------
// Variables globales
// ---------------------------------------------------------
uint8_t  frameBuffer[FRAME_SIZE];
uint8_t  processedBuffer[FRAME_SIZE];
uint8_t  labelBuffer[FRAME_SIZE];
PipaProps pipaArray[MAX_PIPAS];

// Sección crítica para proteger accesos al frameBuffer (y otros recursos)
critical_section_t frameBuffer_mutex;

// ---------------------------------------------------------
// Stubs o ejemplos de funciones que el snippet original mencionaba
// ---------------------------------------------------------
void core1_processing(void) {
    // Ejemplo de bucle que corre en el 2º núcleo
    while (1) {
        // Podrías hacer tareas en paralelo, si lo deseas
        tight_loop_contents();
    }
}

void calibrateSystem(void) {
    // Ejemplo: calibración de sensores, offset, etc.
    // Se deja vacío como placeholder.
}

void processPipasImage(void) {
    // Ejemplo: supón que aquí haces algún umbralizado de la imagen
    // Se deja como placeholder para tu lógica real.
    // Copia simulada: frameBuffer -> processedBuffer (dummy):
    memcpy(processedBuffer, frameBuffer, FRAME_SIZE);
}

void initCamera() {
    // Configura pines GPIO para VSYNC, HSYNC, PCLK, etc.
    // Configura interfaz I2C con la cámara OV7670 (registros de configuración).
}

void captureImage() {
    // Llenar frameBuffer con datos de la cámara usando VSYNC, HSYNC, PCLK.
    // Ejemplo: leer cada byte de la cámara y escribirlo en frameBuffer.
}

// ---------------------------------------------------------
// Función principal
// ---------------------------------------------------------
int main() {
    // Inicialización estándar de la RP2040 (USB, reloj, etc.)
    stdio_init_all();
    
    // Si deseas inicializar UART0 físicamente, hazlo aquí:
    // (Ajusta pines según la documentación de RP2040-Zero)
    // Por ejemplo, GP0 => UART0_TX, GP1 => UART0_RX
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);  // TX
    gpio_set_function(1, GPIO_FUNC_UART);  // RX

    // Inicializar la sección crítica
    critical_section_init(&frameBuffer_mutex);

    // Iniciar procesamiento en el segundo núcleo (opcional)
    multicore_launch_core1(core1_processing);

    // Calibración inicial (ejemplo)
    calibrateSystem();

    // Inicializar la cámara
    initCamera();

    // Variables para control de envío de frames de debug
    static uint32_t lastDebugTime = 0;

    while (true) {
        uint32_t startTime = to_ms_since_boot(get_absolute_time());

        // Capturar la imagen desde la cámara
        captureImage();

        // Procesamiento “básico” de la imagen (umbralizado u otras operaciones)
        processPipasImage();  

        // Aplicamos el filtro de mediana al frameBuffer
        optimizedMedianFilter();

        // Copiamos el resultado filtrado a processedBuffer
        // (Dependiendo de tu flujo, a lo mejor ya se copió dentro de processPipasImage() 
        //  o se maneja de otra forma)
        critical_section_enter(&frameBuffer_mutex);
        memcpy(processedBuffer, frameBuffer, FRAME_SIZE);
        critical_section_exit(&frameBuffer_mutex);

        // Detectar pipas
        int numPipas = 0;
        detectPipas(&numPipas);

        // Enviar frame de depuración cada ~1 segundo (1000 ms)
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if ((now - lastDebugTime) > 1000) {
            sendDebugFrame(processedBuffer, numPipas);
            lastDebugTime = now;
        }

        // Medir el tiempo de procesamiento
        uint32_t endTime = to_ms_since_boot(get_absolute_time());
        uint32_t elapsed = endTime - startTime;
        printf("Tiempo: %u ms, Pipas: %d\n", elapsed, numPipas);

        // Pequeña pausa para no saturar la CPU (ajusta según tu proyecto)
        sleep_ms(100);
    }

    return 0;
}