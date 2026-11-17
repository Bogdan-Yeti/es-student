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
#include "pi-task.h"

#include "pico/unique_id.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"
#include "pico/version.h"

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
    device_info_t info;
    device_get_info(&info);

    printf("name: %s\n", FIRMWARE_NAME);
    printf("version: %s\n", FIRMWARE_VERSION);
    printf("project: %s\n", DEVICE_PROJECT);
    printf("repo: %s\n", DEVICE_REPO);
    printf("board: %s\n", info.board_type);
    printf("serial: %s\n", info.serial_num);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", info.manufacturer, info.part, info.revision);
    printf("pico-sdk: %s\n", info.pico_sdk);
}

static void cmd_uptime_info(const command_t *command) 
{
    printf("uptime: %.2f ms\n", (double)time_us_64() / 1000);
}

void cmd_start_pi(const command_t *command)
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
    if (pi_task_get_state() == PI_COUNTING) {
        printf("error: pi is busy\n");
        return;
    }
    pi_task_start(CALC_PI_TERMS);

    printf("pi: started, %u terms\n", CALC_PI_TERMS);
}

void cmd_pi(const command_t *command) {
    double res;
    uint32_t time;
    switch (pi_task_get_state()) {
        case PI_NOT_RUNNING:
            printf("pi: not started\n");
            break;
        case PI_COUNTING:
            printf("pi: running\n");
            break;
        case PI_READY:
            pi_task_get_result(&res, &time);
            printf("pi: %.8f in %u ms\n", res, time);
            break;
    }
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
    printf("button: %s, presses %u\n", state, button_task_get_press_count());
}

static void cmd_profiling(const command_t *command) {
    for (int i = 0; i != CLOCK_NUM; i++) {
        profiling_result_t res;
        profiling_get(i, &res);
        printf("%-10s count %-20u mean %.2f us max %u\n", res.name, res.count, res.avg, res.max);
    }
}

static void cmd_profiling_reset(const command_t *command) {
    profiling_reset();
}

static const api_command_t commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime_info },
    { "pi_start", "calculate pi: pi_start [terms]", cmd_start_pi },
    { "pi", "result of calculate pi", cmd_pi },
    { "led_period", "", cmd_led_period},
    { "led_enable", "", cmd_led_enable},
    { "led_disable", "", cmd_led_disable},
    { "led_blink", "", cmd_led_blink},
    { "button", "", cmd_button_enable},
    { "profiling", "", cmd_profiling },
    { "profiling_reset", "", cmd_profiling_reset },
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