#include "stdio-text-protocol/stdio-text-protocol.h"
#include "pico/stdlib.h"
#include <stdio.h>

static command_t pars;
static char buf[STDIO_TEXT_PROTOCOL_LINE_SIZE];
static uint end_ind = 0;

const command_t *stdio_text_protocol_handle(void)
{
    int c = getchar_timeout_us(0);
    if (c == PICO_ERROR_TIMEOUT)
        return NULL;
    
    if ((char)c == '\n' || (char)c == '\r') {
        if (end_ind == 0) {
            pars.truncated = false; // Сбрасываем флаг для пустой строки
            return NULL;
        }

        buf[end_ind] = '\0';
        putchar('\n');

        // Запоминаем, было ли переполнение длины при вводе
        bool input_truncated = pars.truncated; 

        pars.name = NULL;
        pars.argc = 0;
        pars.truncated = input_truncated; // Восстанавливаем этот флаг для парсинга

        uint start = 0;
        uint stop  = end_ind;

        // игнорируем пробелы в начале
        while (start < stop && buf[start] == ' ') {
            start++;
        }

        // игнорируем пробелы в конце
        while (stop > start && buf[stop - 1] == ' ') {
            stop--;
        }

        // если хвост был обрезан — нужно терминировать строку
        if (stop < end_ind) {
            buf[stop] = '\0';
        }

        if (start == stop) {
            end_ind = 0;
            pars.truncated = false; // Строка только из пробелов — это не ошибка
            return NULL;
        }

        for (uint i = start; i < stop; i++) {
            if (buf[i] == ' ') {
                buf[i] = '\0';
            } else if (i == start || buf[i - 1] == '\0') {
                if (pars.name == NULL) {
                    pars.name = &buf[i];
                } else {
                    if (pars.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS) {
                        pars.argv[pars.argc++] = &buf[i];
                    } else {
                        pars.truncated = true; // Переполнение по количеству аргументов
                    }
                }
            }
        }
        
        end_ind = 0; // Очищаем буфер для следующей команды
        return (const command_t *)&pars;
    }

    if (c == 0x08 || c == 0x7f) {
        if (end_ind > 0) {
            printf("\b \b");
            end_ind--;
            pars.truncated = false; // Снимаем пометку о переполнении при удалении
        }
        return NULL;
    }

    if (end_ind < STDIO_TEXT_PROTOCOL_LINE_SIZE - 1) {
        if (end_ind == 0) {
            pars.truncated = false; 
        }
        
        buf[end_ind++] = (char)c;
        putchar(c);
    } else {
        pars.truncated = true; // Строка переполнена
    }

    return NULL;
}


void stdio_text_protocol_init(void) {
    
}