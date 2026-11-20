#include "button-task.h"
#include "systime/systime.h"

typedef enum {
    BUTTON_RELEASED,
    BUTTON_PRESSED_Q,
    BUTTON_PRESSED,
    BUTTON_RELEASED_Q
} button_state_t;

static button_state_t button_state[BUTTON_NUM];
static uint64_t time_beg[BUTTON_NUM];
static uint16_t count[BUTTON_NUM];
static button_task_callback_t func[BUTTON_NUM];

void button_task_init(platform_button_t btn, button_task_callback_t on_press) {
    func[btn] = on_press;
}

void button_task_handle(void)
{
    for (int i = 0; i < BUTTON_NUM; i++) {
        bool state = platform_button_read(i);
        uint64_t time = systime_us();

        switch (button_state[i])
        {
            case BUTTON_RELEASED:
                if (state == false) {
                    time_beg[i] = time;
                    button_state[i] = BUTTON_PRESSED_Q;
                }
                break;
            case BUTTON_PRESSED_Q:
                if (state) {
                    button_state[i] = BUTTON_RELEASED;
                } else if (time - time_beg[i] > 20000) {
                    button_state[i] = BUTTON_PRESSED;
                    count[i]++;
                    if (func[i])
                        func[i]();
                }
                break;
            case BUTTON_PRESSED:
                if (state == true) {
                    time_beg[i] = time;
                    button_state[i] = BUTTON_RELEASED_Q;
                }
                break;
            case BUTTON_RELEASED_Q:
                if (!state) {
                    button_state[i] = BUTTON_PRESSED;
                } else if (time - time_beg[i] > 20000) {
                    button_state[i] = BUTTON_RELEASED;
                }
                break;
        }
    }
}

bool button_task_is_pressed(platform_button_t btn) {
    return button_state[btn] == BUTTON_PRESSED || button_state[btn] == BUTTON_PRESSED_Q;
}

uint16_t button_task_get_press_count(platform_button_t btn) {
    return count[btn];
}