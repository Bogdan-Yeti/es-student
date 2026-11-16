#include <stdbool.h>
#include "pi-task.h"
#include "pico/time.h"

// прикидка: за член ряда 4 операции с double, 175 + 110 + 190 + 110 = 585 тактов;
// 100 членов по 585 тактов при 125 МГц — около 0.00047 с
#define PI_TERMS_PER_CALL 100

static pi_state_t state_t = PI_NOT_RUNNING;
static uint32_t pi_k;
static uint32_t pi_n;
static double pi_sum;
static double pi_sign;
static double pi_result;

static uint32_t beg_time;
static uint32_t res_time;

bool pi_task_start(uint32_t n)
{
    if (state_t == PI_COUNTING)
        return false;
    pi_n = n;
    pi_k = 0;
    pi_sum = 0.0;
    pi_sign = 1.0;
    state_t = PI_COUNTING;
    return true;
}

void pi_task_handle(void)
{
    if (state_t != PI_COUNTING) {
        return;
    }

    if (!pi_k) beg_time = time_us_64() / 1000;
    uint32_t rem = pi_n - pi_k;
    uint32_t end = pi_k + (rem < PI_TERMS_PER_CALL ? rem : PI_TERMS_PER_CALL);
    for (; pi_k < end; pi_k++) {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k == pi_n) {
        pi_result = 4.0 * pi_sum;
        state_t = PI_READY;
        res_time = time_us_64() / 1000 - beg_time;
    }
}

bool pi_task_get_result(double *result, uint32_t *time)
{
    if (state_t != PI_READY) {
        return false;
    }
    *result = pi_result;
    *time = res_time;
    return true;
}

pi_state_t pi_task_get_state(void) {
    return state_t;
}