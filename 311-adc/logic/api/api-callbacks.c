#include "api-commands.h"

#include <stdint.h>
#include <stdbool.h>
#include "profiling/profiling.h"
#include "device/device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "firmware.h"
#include "systime/systime.h"
#include "platform/platform.h"
#include "telemetry-task/telemetry-task.h"

#define TELEMETRY_DEFAULT_PERIOD_MS 100

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
    printf("project: %s\n", FIRMWARE_PROJECT);
    printf("repo: %s\n", FIRMWARE_REPO);
    printf("board: %s\n", info.board_type);
    printf("serial: %s\n", info.serial_num);
    printf("chip: manufacturer 0x%03lx, part 0x%04lx, revision %lu\n", info.manufacturer, info.part, info.revision);
    printf("pico-sdk: %s\n", info.pico_sdk);
}

static void cmd_uptime_info(const command_t *command) 
{
    printf("uptime: %lu ms\n", systime_ms());
}

static void cmd_start_pi(const command_t *command)
{
    uint32_t CALC_PI_TERMS = 1000000;
    if (command->argc > 1 || (command->argc == 1 && !parse_u32(command->argv[0], &CALC_PI_TERMS))) {
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

    printf("pi: started, %lu terms\n", CALC_PI_TERMS);
}

static void cmd_pi(const command_t *command) {
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
            printf("pi: %.9f in %lu ms\n", res, time);
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
        default:
            state = "unknown";
            break;
    }
    uint32_t period = led_task_get_period_ms();
    printf("led: %s, period %lu ms\n", state, period);
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
    if (!PROFILING_ENABLED) {
        printf("profiling is disabled in this build\n");
        return;
    }
    for (int i = 0; i != CLOCK_NUM; i++) {
        profiling_result_t res;
        profiling_get(i, &res);
        printf("%-10s count %-20lu mean %.2f us max %lu us\n", res.name, res.count, res.avg, res.max);
    }
}

static void cmd_profiling_reset(const command_t *command) {
    profiling_reset();
    printf("profiling reset\n");
}

static void cmd_get_voltage(const command_t *command) {
    printf("voltage: %.3f V\n", platform_voltage_read());
}

static void cmd_get_temp(const command_t *command) {
    printf("temp: %.1f C\n", platform_temp_read());
}

void api_print_telemetry(const telemetry_record_t *record) {
    printf("tm: %llu %.3f %.1f\n", record->time_us, record->voltage_v, record->temp_c);
}

static void cmd_tm_start(const command_t *command) {
    uint32_t period = TELEMETRY_DEFAULT_PERIOD_MS;
    if (command->argc > 1 || (command->argc == 1 && !parse_u32(command->argv[0], &period))) {
        printf("error: usage\n");
        return;
    }
    if (!telemetry_task_start(period)) {
        printf("error: period_ms must be at least 10\n");
        return;
    }
    printf("telemetry: started, period %lu ms\n", period);
}

static void cmd_tm_stop(const command_t *command) {
    telemetry_task_stop();
    printf("telemetry: stopped\n");
}

const api_command_t  api_commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime_info },
    { "pi_start", "calculate pi: pi_start [terms]", cmd_start_pi },
    { "pi", "result of calculate pi", cmd_pi },
    { "led_period", "set the blinking period", cmd_led_period},
    { "led_enable", "switch the LED to the 'on' state.", cmd_led_enable},
    { "led_disable", "switch the LED to the 'off' state.", cmd_led_disable},
    { "led_blink", "switch the LED to the 'blinking' state.", cmd_led_blink},
    { "button", "the state of the button and the number of presses.", cmd_button_enable},
    { "profiling", "the results of the stopwatches", cmd_profiling },
    { "profiling_reset", "reset the stopwatch results.", cmd_profiling_reset },
    { "get_voltage", "voltage on GP26, ADC input 0", cmd_get_voltage },
    { "get_temp", "chip temperature, degrees Celsius", cmd_get_temp },
    { "tm_start", "start telemetry", cmd_tm_start},
    { "tm_stop", "stop telemetry", cmd_tm_stop},
};

const uint32_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);