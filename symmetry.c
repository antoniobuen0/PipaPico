#include <stdint.h>
#include "symmetry.h"
#include "config.h"

// labelBuffer es global (definido en main.c)
extern uint8_t labelBuffer[FRAME_SIZE];

float calculateSymmetry(int minX, int maxX, int minY, int maxY, int label) {
    int width  = maxX - minX + 1;
    int height = maxY - minY + 1;
    int matches = 0;
    int total   = 0;

    // Muestrear en horizontal (left-right)
    for (int y = minY; y <= maxY; y += SYMMETRY_SAMPLE_RATE) {
        for (int x = 0; x < (width / 2); x += SYMMETRY_SAMPLE_RATE) {
            int leftIdx  = y * WIDTH + (minX + x);
            int rightIdx = y * WIDTH + (maxX - x);

            if (labelBuffer[leftIdx] == label) {
                total++;
                if (labelBuffer[rightIdx] == label) {
                    matches++;
                }
            }
        }
    }

    return (total > 0) ? ((float)matches / total) : 0.0f;
}