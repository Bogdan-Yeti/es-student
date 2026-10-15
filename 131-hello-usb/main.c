#include "pico/stdlib.h"
#include <stdio.h>

const unsigned int LED_PIN = 25;

int main() {
    stdio_init_all();

    while (1) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}