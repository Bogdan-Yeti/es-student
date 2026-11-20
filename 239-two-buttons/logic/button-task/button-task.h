#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "platform/platform.h"

void button_task_handle(void);
bool button_task_is_pressed(platform_button_t btn);
uint16_t button_task_get_press_count(platform_button_t btn);

typedef void (*button_task_callback_t)(void);
void button_task_init(platform_button_t btn, button_task_callback_t on_press);