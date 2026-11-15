#pragma once

#include "pico/stdlib.h"
#include <stdbool.h>


void button_task_handle(void);
void button_task_init(void);
bool button_task_is_pressed(void);
uint button_task_get_press_count(void);