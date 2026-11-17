#include "profiling.h"
#include "pico/stdlib.h"
#include "pico/time.h"

static uint32_t count[CLOCK_NUM];
static uint32_t sum[CLOCK_NUM];
static uint32_t max[CLOCK_NUM];

static uint32_t beg_time[CLOCK_NUM];

char *cl_name[CLOCK_NUM];

void profiling_stopwatch_init(uint32_t id, const char *name) {
    cl_name[id] = name;
}

void profiling_start(uint32_t id) {
    beg_time[id] = time_us_32();
    count[id]++;
}

void profiling_stop(uint32_t id) {
    uint32_t res = time_us_32() - beg_time[id];
    sum[id] += res;
    if (res > max[id]) {
        max[id] = res;
    }
}

bool profiling_get(uint32_t id, profiling_result_t *result) {
    if (!cl_name[id]) {
        return false;
    }
    *result = (profiling_result_t){cl_name[id], count[id], (double)sum[id] / count[id], max[id]};
    return true;
}

void profiling_reset(void) {
    for (int i = 0; i != CLOCK_NUM; i++) {
        count[i] = 0;
        sum[i] = 0;
        max[i] = 0;
    }
}