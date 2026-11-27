#include "block-task.h"
#include "platform/platform.h"
#include <stddef.h>

static block_task_callback_t func;
static uint32_t count_dma;

bool block_task_start(uint32_t count, block_task_callback_t on_block) {
    if (platform_adc_busy() || count > PLATFORM_BLOCK_MAX) {
        return false;
    }
    func = on_block;
    count_dma = count; 
    platform_block_start(count);
    return true;
}

void block_task_handle(void) {
    if (func == NULL || !platform_block_done()) {
        return;
    }
    block_task_callback_t tmp = func;
    func = NULL;
    tmp(platform_block_samples(), count_dma, platform_block_time_us());
}