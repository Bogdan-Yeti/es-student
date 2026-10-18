#include <stdio.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"
#include "memory.h"
#include "command.h"
#include "device.h"

#include <stdlib.h>

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

int main(void);

uint32_t data_variable = 100;
uint32_t bss_variable;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n", name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void fw_info(void)
{
    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));

    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }
    data_variable += 1;
    bss_variable += 1;
    printf("object\tadress\tvalue\n");
    printf("main\t0x%08x\t0x%08x\n", (uint16_t *)main, *(uint16_t *)((uintptr_t)main & ~1u));
    printf("fw_info\t0x%08x\t0x%08x\n", (uint16_t *)fw_info, *(uint16_t *)((uintptr_t)fw_info & ~1u));
    printf("commands\t0x%08x\n", &commands);
    for (uint i = 0; i < command_count; i++)
    {
        printf("- %s\t0x%08x\n", commands[i].name, commands[i].handler);
    }

    printf("DEVICE_PROJECT\t0x%08x\t%s\n", DEVICE_PROJECT, DEVICE_PROJECT);
    printf("DEVICE_BOARD\t0x%08x\t%s\n", DEVICE_BOARD, DEVICE_BOARD);

    printf("data_variable\t0x%08x\t%8u\n", &data_variable, data_variable);
    printf("bss_variable\t0x%08x\t%8u\n", &bss_variable, bss_variable);
    printf("stack_variable\t0x%08x\t%8u\n", &stack_variable, stack_variable);
    printf("heap_variable\t0x%08x\t%8u\n", heap_variable, *heap_variable);

    free(heap_variable);
}

void mem_info(void)
{
    printf("area    start   end size\n");
    row("flash", (uintptr_t)XIP_BASE, (uintptr_t)(XIP_BASE + PICO_FLASH_SIZE_BYTES));
    row("sram", (uintptr_t)SRAM_BASE, (uintptr_t)(SRAM_BASE + 0x40000));
    row("rom", (uintptr_t)ROM_BASE, (uintptr_t)(ROM_BASE + 0x4000));

    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    row("free", (uintptr_t)&__flash_binary_end, (uintptr_t)(XIP_BASE + PICO_FLASH_SIZE_BYTES));
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    row("text",(uintptr_t)& __boot2_end__, (uintptr_t)&__etext);
    row("data flash", (uintptr_t)&__etext, (uintptr_t)&__etext + (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__);
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    row("bss",(uintptr_t)& __bss_start__, (uintptr_t)&__bss_end__);
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    printf("total\n");
    printf("    flash image %8u = boot2 %8u + text %8u + data %8u\n",
        (uintptr_t)&__flash_binary_end - (uintptr_t)&__flash_binary_start, 
        (uintptr_t)&__boot2_end__ - (uintptr_t)&__boot2_start__, (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__);
    printf("    flash free %8u of %8u\n", 
        (uintptr_t)(PICO_FLASH_SIZE_BYTES - (uintptr_t)&__flash_binary_end + (uintptr_t)&__flash_binary_start),
        (uintptr_t)PICO_FLASH_SIZE_BYTES);
    printf("    ram used %8u = data %8u + bss %8u\n", 
        (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__ + (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__, 
        (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__, (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__);
    printf("    ram free %8u for heap and %8u for stack\n", 
        (uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__, (uintptr_t)&__StackTop - (uintptr_t)&__StackBottom);
}