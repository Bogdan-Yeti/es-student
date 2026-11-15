#include "pico/types.h"
#include "stdio-text-protocol.h"
#include "pico/stdlib.h"
#include "profiling.h"
#include "device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "led-task.h"
#include "button-task.h"

typedef void (*api_callback_t)(const command_t *command);

typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

static bool parse_u32(const char *text, uint32_t *value)
{
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }

    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
}

static void cmd_info(const command_t *command)
{
    device_info();
}

static void cmd_uptime_info(const command_t *command) 
{
    printf("uptime: %.2f ms\n", (double)time_us_64() / 1000);
}

static void main_time_exec(const command_t *command) {
    printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

static void main_time_reset(const command_t *command) {
    profiling_reset_max();
    printf("max reset\n");
}

double calc_pi(uint n)
{
    double sum = 0.0;
    double sign = 1.0;

    for (int k = 0; k < n; k++) {
        sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }

    return 4.0 * sum;
}

void cmd_calc_pi(const command_t *command)
{
    uint32_t CALC_PI_TERMS = 1000000;
    if (command->argc > 1 || command->argc == 1 && !parse_u32(command->argv[0], &CALC_PI_TERMS)) {
        printf("error: usage pi_start [<terms>]\n");
        return;
    }
    if (CALC_PI_TERMS <= 0) {
        printf("error: terms must be greater than 0\n");
        return;
    }

    uint64_t start_us = time_us_64();
    double pi_result = calc_pi(CALC_PI_TERMS);
    uint64_t spent_us = time_us_64() - start_us;

    printf("pi: %.9f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}

static void print_led_state(void) {
    char *state;
    switch (led_task_get_state()) {
        case LED_STATE_OFF:
            state = "off";
            break;
        case LED_STATE_ON:
            state = "on";
            break;
        case LED_STATE_BLINK:
            state = "blink";
            break;
    }
    uint32_t period = led_task_get_period_ms();
    printf("led: %s, period %u ms\n", state, period);
}

static void cmd_led_period(const command_t *command)
{
    uint32_t period_ms;

    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    // годится ли такой период, решает задача светодиода
    if (!led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }

    // ответ: состояние и период светодиода
    print_led_state();
}

static void cmd_led_enable(const command_t *command) {
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

static void cmd_led_disable(const command_t *command) {
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

static void cmd_led_blink(const command_t *command) {
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}

static void cmd_button_enable(const command_t *command) {
    char *state;
    if (button_task_is_pressed()) {
        state = "pressed";
    } else {
        state = "released";
    }
    printf("button: %s, presses %u", state, button_task_get_press_count());
}

static const api_command_t commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime_info },
    { "calc_pi", "calculate pi: calc_pi [terms]", cmd_calc_pi },
    { "main_time_exec", "superloop iteration: average and maximum", main_time_exec },
    { "main_time_reset", "reset maximum iteration", main_time_reset },
    { "led_period", "", cmd_led_period},
    { "led_enable", "", cmd_led_enable},
    { "led_disable", "", cmd_led_disable},
    { "led_blink", "", cmd_led_blink},
    { "button", "", cmd_button_enable}
};

static const uint command_count = sizeof(commands) / sizeof(commands[0]);

void api_handle(const command_t *command) {
    if (command->truncated) {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }
    
    if (strcmp(command->name, "help") == 0) {
        printf("help list of commands\n");
        for (int i = 0; i < command_count; i++) {
            printf("%s %s\n", commands[i].name, commands[i].help);
        }
        return;
    }

    for (uint i = 0; i < command_count; i++)
    {
        if (strcmp(command->name, commands[i].name) == 0)
        {
            if (commands[i].callback != NULL)
            {
                commands[i].callback(command);
                return;
            }
        }
    }
    printf("error: unknown command '%s', try help\n", command->name);
}