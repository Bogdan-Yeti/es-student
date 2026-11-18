#pragma once

#include <stdint.h>

typedef struct {
    char *board_type;
    char serial_num[17];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    char *pico_sdk;
} device_info_t;

void device_get_info(device_info_t *info);