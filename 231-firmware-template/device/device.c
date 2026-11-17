#include "device.h"

#include "pico/unique_id.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"
#include "pico/version.h"

void device_get_info(device_info_t *info) {
    info->board_type = DEVICE_BOARD + '\0';
    pico_get_unique_board_id_string(info->serial_num, sizeof(info->serial_num));
    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    info->revision = (*chip_id & SYSINFO_CHIP_ID_REVISION_BITS) >> SYSINFO_CHIP_ID_REVISION_LSB;
    info->part = (*chip_id & SYSINFO_CHIP_ID_PART_BITS) >> SYSINFO_CHIP_ID_PART_LSB;
    info->manufacturer = (*chip_id & SYSINFO_CHIP_ID_MANUFACTURER_BITS) >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;
    info->pico_sdk = PICO_SDK_VERSION_STRING + '\0';
}
