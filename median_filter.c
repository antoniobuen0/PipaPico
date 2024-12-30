#include "median_filter.h"
#include "config.h"
#include <string.h>
#include <hardware/sync.h>

// Declaramos extern las variables globales definidas en main.c
extern uint8_t frameBuffer[FRAME_SIZE];
extern critical_section_t frameBuffer_mutex;

// Estructura auxiliar para histograma 3x3
typedef struct {
    // Ventana de 3x3 = 9 píxeles
    uint8_t  values[MEDIAN_WINDOW * MEDIAN_WINDOW];
    int      head;
    int      tail;
    uint16_t counts[256];
} CircularHistogram;

// Inicializa la estructura de histograma
static void initCircularHistogram(CircularHistogram* hist) {
    memset(hist->values, 0, sizeof(hist->values));
    memset(hist->counts, 0, sizeof(hist->counts));
    hist->head = 0;
    hist->tail = 0;
}

// Actualiza el histograma al mover la ventana un pixel
static void updateHistogram(CircularHistogram* hist, uint8_t newValue, uint8_t oldValue) {
    hist->counts[oldValue]--;
    hist->counts[newValue]++;
}

// Implementación del filtro de mediana
void optimizedMedianFilter() {
    uint8_t tempBuffer[FRAME_SIZE];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    // Posición de la mediana en una ventana de 9 píxeles
    const int medianPos = (MEDIAN_WINDOW * MEDIAN_WINDOW) / 2; // 4 para 3x3

    // Recorre filas y columnas válidas
    for (int y = 1; y < HEIGHT - 1; y++) {
        // Inicializa histograma para x=1 en esta fila
        CircularHistogram hist;
        initCircularHistogram(&hist);

        // Llenado inicial de la ventana 3x3
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int idx = (y + dy) * WIDTH + (1 + dx);
                uint8_t val = frameBuffer[idx];
                hist.values[hist.head] = val;
                hist.counts[val]++;
                hist.head = (hist.head + 1) % (MEDIAN_WINDOW * MEDIAN_WINDOW);
            }
        }

        // Recorre de x=1 a x=WIDTH-2
        for (int x = 1; x < WIDTH - 1; x++) {
            // Buscar la mediana
            int count = 0;
            int median = 0;
            while (count <= medianPos && median < 256) {
                count += hist.counts[median];
                median++;
            }
            tempBuffer[y * WIDTH + x] = (uint8_t)(median - 1);

            // Deslizar la ventana a la siguiente columna
            if (x < WIDTH - 2) {
                for (int dy = -1; dy <= 1; dy++) {
                    int oldIdx = (y + dy) * WIDTH + (x - 1);
                    int newIdx = (y + dy) * WIDTH + (x + 2);
                    uint8_t oldVal = frameBuffer[oldIdx];
                    uint8_t newVal = frameBuffer[newIdx];
                    updateHistogram(&hist, newVal, oldVal);
                }
            }
        }
    }

    // Copiar resultado al frameBuffer con protección de sección crítica
    critical_section_enter(&frameBuffer_mutex);
    memcpy(frameBuffer, tempBuffer, FRAME_SIZE);
    critical_section_exit(&frameBuffer_mutex);
}