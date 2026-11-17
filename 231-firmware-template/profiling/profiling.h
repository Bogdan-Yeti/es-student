#pragma once

#include "pico/types.h"

#define CLOCK_NUM 5

typedef struct {
    char *name;
    uint32_t count;
    double avg;
    uint32_t max;
} profiling_result_t;

void profiling_stopwatch_init(uint32_t id, const char *name);

void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);

// false — секундомер с таким номером не заведён
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);