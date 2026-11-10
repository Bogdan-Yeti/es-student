#include "stdio-text-protocol.h"
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
            return NULL;
        }

        buf[end_ind] = '\0';
        putchar('\n');

        pars.name = NULL;
        pars.argc = 0;

        for (uint i = 0; i < end_ind; i++) {
            if (buf[i] == ' ') {
                buf[i] = '\0';
            } else if ((i != 0 && buf[i - 1] == '\0') || i == 0) {
                if (pars.name == NULL) {
                    pars.name = &buf[i];
                } else {
                    if (pars.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS) {
                        pars.argv[pars.argc++] = &buf[i];
                    } else {
                        pars.truncated = true;
                    }
                }
            }
        }

        end_ind = 0;
        return (const command_t *)&pars;
    }

    if (c == 0x08 || c == 0x7f) {
        if (end_ind > 0) {
            printf("\b \b");
            end_ind--;
            pars.truncated = false;
        }
        return NULL;
    }

    if (end_ind < STDIO_TEXT_PROTOCOL_LINE_SIZE - 1) {
        buf[end_ind++] = (char)c;
        putchar(c);
    } else {
        if (end_ind == 0) {
            pars.truncated = false;
        }
        pars.truncated = true;
    }

    return NULL;
}


void stdio_text_protocol_init(void) {
    
}