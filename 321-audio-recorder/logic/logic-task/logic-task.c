#include "logic-task.h"
#include "platform/platform.h"
#include <stddef.h>

static stream_task_callback_t func;
static stream_state_t state;
static uint8_t b;
static uint16_t num;
static uint32_t sum_counts;

static stream_header_t header;

void stream_task_init(stream_task_callback_t on_frame) {
    func = on_frame;
    state = STREAM_STATE_IDLE;
}

bool stream_task_start(uint8_t bits) {
    if (!platform_stream_start(bits)) {
        return false;
    }
    sum_counts = 0;
    num = 0;
    b = bits;
    state = STREAM_STATE_RUN;
    return true;
}

void stream_task_stop(void) {
    platform_stream_stop();
    state = STREAM_STATE_IDLE;
}

void stream_task_handle(void) {
    uint32_t count;
    const uint8_t *buf = platform_stream_next(&count);
    if (state != STREAM_STATE_RUN || buf == NULL) {
        return;
    }
    header.sync_word = STREAM_SYNC_WORD;
    header.number = num;
    header.count = count;

    uint32_t bytes = (b == 8) ? count : count * 2;

    func(&header, buf, bytes);
    num++;
    sum_counts += count;
}

uint32_t stream_task_get_frames(void) {
    return num;
}

uint32_t stream_task_get_samples(void) {
    return sum_counts;
}

uint32_t stream_task_get_overruns(void) {
    return platform_stream_overruns();
}