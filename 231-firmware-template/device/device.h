#pragma once

#include <stdint.h>

#define FIRMWARE_NAME "SWAG"

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/Bogdan-Yeti/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

typedef struct {
    char *board_type;
    char serial_num[17];
    uint32_t part;
    uint32_t revision;
    char *pico_sdk;
} device_info_t;

void device_get_info(device_info_t *info);