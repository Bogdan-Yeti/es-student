#pragma once

#include "pico/stdlib.h"
#include <stdbool.h>

typedef enum {
    BUTTON_RELEASED,
    BUTTON_PRESSED_Q,
    BUTTON_PRESSED,
    BUTTON_RELEASED_Q
} button_state_t;


void button_task_handle(void);
void button_task_init(void);
bool button_task_is_pressed(void);
uint button_task_get_press_count(void);