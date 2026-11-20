#pragma once

#include <stdint.h>

#define CLOCK_NUM 5

// по умолчанию профилирование включено
#ifndef PROFILING_ENABLED
#define PROFILING_ENABLED 1
#endif

#if PROFILING_ENABLED
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
#else
// профилирование выключено: вызовы ничего не делают
#define profiling_start(id) ((void)0)
#define profiling_stop(id) ((void)0)
#endif

typedef struct {
    const char *name;
    uint32_t count;
    double avg;
    uint32_t max;
} profiling_result_t;

void profiling_stopwatch_init(uint32_t id, const char *name);

// false — секундомер с таким номером не заведён
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);