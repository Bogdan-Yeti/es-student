cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(321_audio_recorder)

pico_sdk_init()

add_compile_options(-Wall)

add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)

add_library(platform STATIC
    platform/platform.c
)

target_include_directories(platform PUBLIC
    platform
)

target_link_libraries(platform PRIVATE
    pico_stdlib
    hardware_adc
    hardware_dma
    hardware_pwm
)

# логика прибора: задачи и команды, без SDK
add_library(logic STATIC
    logic/api/api.c
    logic/api/api-callbacks.c
    logic/button-task/button-task.c
    logic/led-task/led-task.c
    logic/pi-task/pi-task.c
    logic/telemetry-task/telemetry-task.c
    logic/block-task/block-task.c
    logic/stream-task/stream-task.c
)

target_include_directories(logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    logic
)

target_link_libraries(logic PUBLIC
    stdio_text_protocol
)

target_link_libraries(logic PRIVATE
    profiling
    device
    systime
    platform
)

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task
    ${CMAKE_CURRENT_SOURCE_DIR}/button-task
    ${CMAKE_CURRENT_SOURCE_DIR}/pi-task
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_unique_id
    device
    logging
    profiling
    stdio_text_protocol
    systime
    platform
    logic
)

# выключатель профилирования: 0 — вызовов секундомеров в образе нет
target_compile_definitions(profiling PUBLIC
    PROFILING_ENABLED=1
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    DEVICE_BOARD="${PICO_BOARD}"
)


pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
