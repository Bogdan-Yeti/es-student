#pragma once

#include "pico/stdlib.h"

typedef enum {
    PI_NOT_RUNNING,
    PI_COUNTING,
    PI_READY
} pi_state_t;

bool pi_task_start(uint32_t n);
void pi_task_handle(void);
bool pi_task_get_result(double *result, uint32_t *time);
pi_state_t pi_task_get_state(void);