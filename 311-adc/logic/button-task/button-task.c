#include "button-task.h"
#include "systime/systime.h"
#include "platform/platform.h"

typedef enum {
    BUTTON_RELEASED,
    BUTTON_PRESSED_Q,
    BUTTON_PRESSED,
    BUTTON_RELEASED_Q
} button_state_t;

static button_state_t button_state;
static uint64_t time_beg;
static uint16_t count = 0;
static button_task_callback_t func;

void button_task_init(button_task_callback_t on_press) {
    func = on_press;
}

void button_task_handle(void)
{
    bool state = platform_button_read();
    uint64_t time = systime_us();

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
                if (!func)
                    func();
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

uint16_t button_task_get_press_count(void) {
    return count;
}