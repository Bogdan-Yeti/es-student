#include "led-task.h"
#include "systime/systime.h"

#include "platform/platform.h"

static uint64_t half_period = 1000 * 1000 / 2;
static led_state_t led_state = LED_STATE_BLINK;
static uint64_t last_toggle_us;
static bool state = false;

static void led_set(bool on)
{
    state = on;
    platform_led_set(on);
}

static void led_toggle(void)
{
    led_set(!state);
}

void led_task_set_state(led_state_t state)
{
    led_state = state;
    switch (led_state)
    {
        case LED_STATE_OFF:
            led_set(false);
            break;
        case LED_STATE_ON:
            led_set(true);
            break;
        case LED_STATE_BLINK:
            last_toggle_us = 0;
            break;
    }
}

led_state_t led_task_get_state(void) {
    return led_state;
}

bool led_task_set_period_ms(uint32_t period_ms)
{
    if (period_ms == 0) {
        return false;
    }

    half_period = (uint64_t)period_ms * 1000 / 2;
    return true;
}

uint32_t led_task_get_period_ms(void) {
    return (uint32_t)(half_period / 1000 * 2);
}

void led_task_handle(void)
{
    switch (led_state)
    {
        case LED_STATE_OFF:
        case LED_STATE_ON:
            break;
        case LED_STATE_BLINK:
            if (systime_period_elapsed(&last_toggle_us, 500000)) {
                led_toggle();
            }
            break;
    }
}

void led_task_next_state(void) {
    switch (led_state)
    {
        case LED_STATE_OFF:
            led_state = LED_STATE_ON;
            break;
        case LED_STATE_ON:
            led_state = LED_STATE_BLINK;
            break;
        case LED_STATE_BLINK:
            led_state = LED_STATE_OFF;
            break;
    }
}