#pragma once

#include <stdbool.h>

#define BUTTON_NUM 2

typedef enum {
    BUTTON_1,
    BUTTON_2
} platform_button_t;

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

void platform_led_set(bool on);

// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(platform_button_t btn);