#include "profiling/profiling.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#if PROFILING_ENABLED

static uint32_t count[CLOCK_NUM];
static uint32_t sum[CLOCK_NUM];
static uint32_t max[CLOCK_NUM];
static uint32_t beg_time[CLOCK_NUM];

const char *cl_name[CLOCK_NUM];

void profiling_stopwatch_init(uint32_t id, const char *name) {
    if (id < CLOCK_NUM) {
        cl_name[id] = name;
    }
}

void profiling_start(uint32_t id) {
    if (id < CLOCK_NUM) {
        beg_time[id] = time_us_32();
        count[id]++;
    }
}

void profiling_stop(uint32_t id) {
    if (id < CLOCK_NUM) {
        uint32_t res = time_us_32() - beg_time[id];
        sum[id] += res;
        if (res > max[id]) {
            max[id] = res;
        }
    }
}

bool profiling_get(uint32_t id, profiling_result_t *result) {
    if (id >= CLOCK_NUM || !cl_name[id] || !result) {
        return false;
    }
    double avg = (count[id] > 0) ? ((double)sum[id] / count[id]) : 0.0;
    *result = (profiling_result_t){cl_name[id], count[id], avg, max[id]};
    return true;
}

void profiling_reset(void) {
    for (int i = 0; i < CLOCK_NUM; i++) {
        count[i] = 0;
        sum[i] = 0;
        max[i] = 0;
    }
}

#else

void profiling_stopwatch_init(uint32_t id, const char *name) {}

bool profiling_get(uint32_t id, profiling_result_t *result) {

    return false;
}

void profiling_reset(void) {}

#endif