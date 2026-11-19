#pragma once

#include <stdbool.h>
#include <stdint.h>

void button_task_handle(void);
bool button_task_is_pressed(void);
uint16_t button_task_get_press_count(void);

typedef void (*button_task_callback_t)(void);
void button_task_init(button_task_callback_t on_press);