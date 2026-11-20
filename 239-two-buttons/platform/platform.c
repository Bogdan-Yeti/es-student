#include "platform.h"
#include "pico/types.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

const static uint BUTTON_PIN[BUTTON_NUM] = {15, 14};
const static uint LED_PIN = 25;

void platform_init(void) {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for (int i = 0; i < BUTTON_NUM; i++) {
        gpio_init(BUTTON_PIN[i]);
        gpio_set_dir(BUTTON_PIN[i], GPIO_IN);
        gpio_pull_up(BUTTON_PIN[i]);
    }
}

void platform_led_set(bool on) {
    gpio_put(LED_PIN, on);
}

bool platform_button_read(platform_button_t btn) {
    return gpio_get(BUTTON_PIN[btn]);
}