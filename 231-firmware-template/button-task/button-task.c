#include "button-task.h"
#include "hardware/gpio.h"
#include "led-task.h"

typedef enum {
    BUTTON_RELEASED,
    BUTTON_PRESSED_Q,
    BUTTON_PRESSED,
    BUTTON_RELEASED_Q
} button_state_t;

static button_state_t button_state;
const static uint BUTTON_PIN = 15;
static uint64_t time_beg;
static uint count = 0;

void button_task_init(void) {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
}

void button_task_handle(void)
{
    bool state = gpio_get(BUTTON_PIN);
    uint64_t time = time_us_64();

    switch (button_state)
    {
        case BUTTON_RELEASED:
            if (state == false) {
                time_beg = time;
                button_state = BUTTON_PRESSED_Q;
            }
            break;
        case BUTTON_PRESSED_Q:
            if (state) {
                button_state = BUTTON_RELEASED;
            } else if (time - time_beg > 20000) {
                button_state = BUTTON_PRESSED;
                count++;
                led_task_next_state();
            }
            break;
        case BUTTON_PRESSED:
            if (state == true) {
                time_beg = time;
                button_state = BUTTON_RELEASED_Q;
            }
            break;
        case BUTTON_RELEASED_Q:
            if (!state) {
                button_state = BUTTON_PRESSED;
            } else if (time - time_beg > 20000) {
                button_state = BUTTON_RELEASED;
            }
            break;
    }
}

bool button_task_is_pressed(void) {
    return button_state == BUTTON_PRESSED || button_state == BUTTON_PRESSED_Q;
}

uint button_task_get_press_count(void) {
    return count;
}