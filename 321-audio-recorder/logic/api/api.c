#include "api.h"
#include "api-commands.h"
#include <stdio.h>
#include <string.h>
#include "platform/platform.h"

void api_handle(const command_t *command) {
    if (command->truncated) {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }
    
    if (strcmp(command->name, "help") == 0) {
        printf("help list of commands\n");
        for (int i = 0; i < api_command_count; i++) {
            printf("%s %s\n", api_commands[i].name, api_commands[i].help);
        }
        return;
    }

    for (int i = 0; i < api_command_count; i++)
    {
        if (strcmp(command->name, api_commands[i].name) == 0)
        {
            if (api_commands[i].callback != NULL)
            {
                api_commands[i].callback(command);
                return;
            }
        }
    }
    printf("error: unknown command '%s', try help\n", command->name);
}

void print_dma_cap(const uint16_t *samples, uint32_t count, uint32_t time_us) {
    uint64_t exp =  (uint64_t)count * 1000000 / PLATFORM_SAMPLE_RATE_HZ;
    uint16_t min = 0;
    uint16_t max = 0;
    uint32_t sum = 0;
    for (uint32_t i = 0; i != count; i++) {
        if (samples[i] < min || min == 0) {
            min = samples[i];
        }
        if (samples[i] > max || max == 0) {
            max = samples[i];
        }
        sum += samples[i];
    }
    printf("dma_cap: %lu samples in %lu us, expected %llu us, min %u max %u mean %.1f\n", 
           count, time_us, exp, min, max, sum * 1.0f / count);
}

void api_print_telemetry(const telemetry_record_t *record) {
    printf("tm: %llu %.3f %.1f\n", record->time_us, record->voltage_v, record->temp_c);
}

void api_send_frame(const stream_header_t *header, const uint8_t *data, uint32_t bytes)
{
    // байты заголовка и отсчётов как они лежат в памяти
    platform_write(header, sizeof(*header));
    platform_write(data, bytes);
}