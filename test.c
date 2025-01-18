#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    const uint LED_PIN = 25; // Pin del LED integrado
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1); // Encender LED
        sleep_ms(500);       // Esperar 500ms
        gpio_put(LED_PIN, 0); // Apagar LED
        sleep_ms(500);       // Esperar 500ms
    }
    return 0;
}