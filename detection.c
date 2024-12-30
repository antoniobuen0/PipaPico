#include "detection.h"
#include "config.h"
#include "types.h"
#include "symmetry.h"
#include <string.h>
#include <stdio.h>

// Variables externas definidas en main.c
extern uint8_t processedBuffer[FRAME_SIZE];
extern uint8_t labelBuffer[FRAME_SIZE];
extern PipaProps pipaArray[MAX_PIPAS];

// StackPoint para flood fill
typedef struct {
    int16_t x;
    int16_t y;
} StackPoint;

// Detecta "pipas" con flood fill
int detectPipas(int* numPipas) {
    *numPipas = 0;
    memset(labelBuffer, 0, FRAME_SIZE);

    // Stack optimizado
    static StackPoint stack[MAX_STACK_SIZE];

    for (int y = 1; y < HEIGHT - 1 && *numPipas < MAX_PIPAS; y++) {
        for (int x = 1; x < WIDTH - 1 && *numPipas < MAX_PIPAS; x++) {
            int idx = y * WIDTH + x;
            // Si el pixel es "blanco" (255) y no ha sido etiquetado
            if ((processedBuffer[idx] == 255) && (labelBuffer[idx] == 0)) {
                // Tomar la pipa actual
                PipaProps* pipa = &pipaArray[*numPipas];
                int stackSize   = 0;

                // Inicializar propiedades
                pipa->area = 0;
                int minX = x, maxX = x;
                int minY = y, maxY = y;
                int sumX = 0, sumY = 0;

                // Iniciar flood fill
                stack[stackSize++] = (StackPoint){x, y};
                labelBuffer[idx] = (uint8_t)(*numPipas + 1);

                // Expandimos
                while ((stackSize > 0) && (stackSize < MAX_STACK_SIZE)) {
                    StackPoint sp = stack[--stackSize];
                    pipa->area++;
                    sumX += sp.x;
                    sumY += sp.y;

                    // Actualizar min/max
                    if (sp.x < minX) minX = sp.x;
                    if (sp.x > maxX) maxX = sp.x;
                    if (sp.y < minY) minY = sp.y;
                    if (sp.y > maxY) maxY = sp.y;

                    // 4-conectividad
                    static const int8_t dx[4] = {0, 0, -1, 1};
                    static const int8_t dy[4] = {-1, 1, 0, 0};
                    for (int i = 0; i < 4; i++) {
                        int16_t nx = sp.x + dx[i];
                        int16_t ny = sp.y + dy[i];
                        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
                            int nidx = ny * WIDTH + nx;
                            if ((processedBuffer[nidx] == 255) && (labelBuffer[nidx] == 0)) {
                                labelBuffer[nidx] = (uint8_t)(*numPipas + 1);
                                stack[stackSize++] = (StackPoint){nx, ny};
                            }
                        }
                    }
                }

                // Si se desborda el stack, descartamos la región
                if (stackSize >= MAX_STACK_SIZE) {
                    pipa->isValid = false;
                    // Podríamos “desetiquetar” la zona aquí si queremos
                    continue;
                }

                // Calcular propiedades finales
                pipa->centerX = sumX / pipa->area;
                pipa->centerY = sumY / pipa->area;
                float w = (float)(maxX - minX + 1);
                float h = (float)(maxY - minY + 1);
                pipa->aspectRatio = (h != 0.0f) ? (w / h) : 1.0f;

                // Calcular simetría (horizontal o combinada)
                pipa->symmetryScore = calculateSymmetry(minX, maxX, minY, maxY, (*numPipas + 1));

                // Validar
                pipa->isValid =
                    (pipa->area >= MIN_PIPA_AREA) &&
                    (pipa->area <= MAX_PIPA_AREA) &&
                    (pipa->aspectRatio >= MIN_ASPECT_RATIO) &&
                    (pipa->aspectRatio <= MAX_ASPECT_RATIO) &&
                    (pipa->symmetryScore > 0.7f);

                // Sólo incrementamos el recuento si la pipa es válida
                if (pipa->isValid) {
                    (*numPipas)++;
                }
            }
        }
    }

    return *numPipas;
}