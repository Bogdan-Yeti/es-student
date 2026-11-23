#include "telemetry-task.h"
#include "systime/systime.h"
#include "platform/platform.h"

static telemetry_state_t state;
static telemetry_task_callback_t callback;
static uint64_t period;
static uint64_t last;
static telemetry_record_t ans;

void telemetry_task_init(telemetry_task_callback_t on_record) {
    callback = on_record;
    state = TELEMETRY_STATE_IDLE;
}

bool telemetry_task_start(uint32_t period_ms) {
    if (period_ms < 10) 
        return false;
    
    period = period_ms * 1000;
    state = TELEMETRY_STATE_RUN;
    last = 0;
    return true;
}

void telemetry_task_handle(void) {
    switch (state) {
        case TELEMETRY_STATE_RUN:
            if (systime_period_elapsed(&last, period)) {
                ans = (telemetry_record_t){
                    systime_us(),
                    platform_voltage_read(),
                    platform_temp_read()
                };
                callback((const telemetry_record_t *)&ans);
            }
            break;
        case TELEMETRY_STATE_IDLE:
            break;
    }
}

void telemetry_task_stop(void) {
    state = TELEMETRY_STATE_IDLE;
}

telemetry_state_t telemetry_task_get_state(void) {
    return state;
}

uint32_t telemetry_task_get_period_ms(void) {
    return period / 1000;
}