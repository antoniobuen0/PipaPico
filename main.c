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
// Definiciones de pines y parámetros I2C
// ---------------------------------------------------------
#define OV7670_ADDR   0x42  // Dirección real: 0x42 (0x21 << 1)
#define PIN_XCLK      15    // PWM para generar reloj
#define PIN_VSYNC     2
#define PIN_HSYNC     3
#define PIN_PCLK      4
#define PIN_DATA0     5     // D0-D7 consecutivos
#define PIN_DATA1     6
#define PIN_DATA2     7
#define PIN_DATA3     8
#define PIN_DATA4     9
#define PIN_DATA5     10
#define PIN_DATA6     11
#define PIN_DATA7     12
#define PIN_SIOC      13    // SCL
#define PIN_SIOD      14    // SDA

// Configuración corregida OV7670 para modo Y (8-bit)
const uint8_t ov7670_init[][2] = {
    {0x12, 0x80},  // COM7: Reset todos los registros
    {0x12, 0x00},  // COM7: Formato YUV
    {0x11, 0x80},  // CLKRC: Clock divisor
    {0x3a, 0x04},  // TSLB: Setup UV output order and enable manual scaling
    {0x8c, 0x00},  // RGB444: Disable RGB444
    {0x04, 0x00},  // COM1: Disable CCIR656
    {0x40, 0xC0},  // COM15: Full 0-255 output, RGB 565
    {0x14, 0x1a},  // COM9: AGC Enabled, some reserved bits
    {0x3D, 0x40},  // COM13: Gamma enable, UV auto adjust
    {0x11, 0x80},  // CLKRC: Use external clock directly
    {0x6B, 0x4A},  // DBLV: PLL control
    {0x0C, 0x00},  // COM3: No scale, DCW disable
    {0x3E, 0x00},  // COM14: No DCW and PCLK scaling
    {0, 0xff}      // Marca fin de config
};

static void xclk_init(void) {
    // Configurar PWM para XCLK exactamente a 12MHz
    gpio_set_function(PIN_XCLK, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PIN_XCLK);
    pwm_set_wrap(slice_num, 9);    // 125MHz/(10) = 12.5MHz
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 5); // 50% duty cycle
    pwm_set_enabled(slice_num, true);
}

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
    // Umbralización adaptativa simple
    uint32_t sum = 0;
    for(int i = 0; i < FRAME_SIZE; i++) {
        sum += frameBuffer[i];
    }
    uint8_t threshold = (sum / FRAME_SIZE) + 20; // Umbral adaptativo

    for(int i = 0; i < FRAME_SIZE; i++) {
        processedBuffer[i] = (frameBuffer[i] > threshold) ? 255 : 0;
    }
}

static inline uint8_t readCameraData() {
    // Optimizado para lectura de 8 bits
    return (gpio_get_all() >> PIN_DATA0) & 0xFF;
}

void initCamera() {
    // Inicializar XCLK
    xclk_init();
    
    // Configurar pines
    gpio_init(PIN_VSYNC); gpio_set_dir(PIN_VSYNC, GPIO_IN);
    gpio_init(PIN_HSYNC); gpio_set_dir(PIN_HSYNC, GPIO_IN);
    gpio_init(PIN_PCLK);  gpio_set_dir(PIN_PCLK, GPIO_IN);
    
    for(int i = 0; i < 8; i++) {
        gpio_init(PIN_DATA0 + i);
        gpio_set_dir(PIN_DATA0 + i, GPIO_IN);
    }
    
    // Configurar I2C
    i2c_init(i2c0, 100000);
    gpio_set_function(PIN_SIOD, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SIOC, GPIO_FUNC_I2C);
    
    // Escribir configuración
    for(int i = 0; ov7670_init[i][0] != 0xff; i++) {
        uint8_t buf[] = {ov7670_init[i][0], ov7670_init[i][1]};
        i2c_write_blocking(i2c0, OV7670_ADDR, buf, 2, false);
        sleep_ms(1);  // Dar tiempo entre escrituras
    }
}

void captureImage() {
    // Sincronización mejorada con VSYNC
    while (!gpio_get(PIN_VSYNC)) tight_loop_contents(); // Esperar flanco ascendente
    while (gpio_get(PIN_VSYNC)) tight_loop_contents();  // Esperar flanco descendente

    for (int row = 0; row < HEIGHT; row++) {
        // Sincronización con HSYNC
        while (!gpio_get(PIN_HSYNC)) tight_loop_contents();
        
        // Captura optimizada de píxeles
        for (int col = 0; col < WIDTH; col++) {
            while (!gpio_get(PIN_PCLK)) tight_loop_contents();
            frameBuffer[row * WIDTH + col] = readCameraData();
            while (gpio_get(PIN_PCLK)) tight_loop_contents();
        }
        
        // Esperar fin de línea
        while (gpio_get(PIN_HSYNC)) tight_loop_contents();
    }
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

        // 1. Capturar imagen
        captureImage();

        // 2. Aplicar filtro de mediana directamente sobre frameBuffer
        optimizedMedianFilter();

        // 3. Umbralizar y guardar en processedBuffer
        processPipasImage();

        // 4. Detectar pipas (ya no necesitamos copiar a processedBuffer)
        int numPipas = 0;
        detectPipas(&numPipas);

        // Debug y timing como antes
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
