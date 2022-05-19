// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <xtensa_context.h>
#include <console.h>
#include <hal_uart.h>
#include <backtrace.h>

#ifdef CONFIG_LINUX_DEBUG
#include <components/aw/linux_debug/debug_common.h>

void pr_registers(const void *f, int core)
{
    int x, y;

    XtExcFrame *frame = (XtExcFrame *) f;
    int *regs = (int *)frame;

    const char *sdesc[] =
    {
        "PC      ", "PS      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
        "A6      ", "A7      ", "A8      ", "A9      ", "A10     ", "A11     ", "A12     ", "A13     ",
        "A14     ", "A15     ", "SAR     ", "EXCCAUSE", "EXCVADDR", "LBEG    ", "LEND    ", "LCOUNT  "
    };

    log_save("Core %d register dump:\n",core);
    for (x = 0; x < 24; x += 4)
    {
        for (y = 0; y < 4; y++)
        {
            if (sdesc[x + y][0] != 0)
            {
                log_save("%s: 0x%x\n", sdesc[x + y], regs[x + y + 1]);
            }
        }
    }
    log_save("\r\n");
}


void show_exception_information_for_core(const void *f, int core)
{
    log_save("\r\n");
    log_save("-----------------------------------------\r\n");
    log_save("-----------------------------------------\r\n");
    pr_registers(f, core);
    log_save("-----xt-addr2line-----\r\n");
    arch_backtrace_exception(log_save, (void*)f);
    log_save("----------------------\r\n");
    while (1);
}

#else

#define CONSOLEBUF_SIZE 512
static char log_buf[CONSOLEBUF_SIZE];
void panic_print_char(const char c)
{
#ifndef BACKTRACE_SIM
    hal_uart_put_char(CONSOLE_UART, c);
#else
    /* for sim */
    putchar(c);
    fflush(stdout);
#endif
}

void panic_print_hex(int h)
{
    int x;
    int c;
    for (x = 0; x < 8; x++)
    {
        c = (h >> 28) & 0xf;
        if (c < 10)
        {
            panic_print_char('0' + c);
        }
        else
        {
            panic_print_char('a' + c - 10);
        }
        h <<= 4;
    }
}

void panic_print_dec(int d)
{
    int n1, n2;
    n1 = d % 10;
    n2 = d / 10;
    if (n2 == 0)
    {
        panic_print_char(' ');
    }
    else
    {
        panic_print_char(n2 + '0');
    }
    panic_print_char(n1 + '0');
}

void panic_print_str(const char *str)
{
    int i;
    for (i = 0; str[i] != 0; i++)
    {
        panic_print_char(str[i]);
    }
}

int printk(const char *fmt, ...)
{
    va_list args;
    size_t length;
    int i = 0;

    memset(&log_buf, 0, CONSOLEBUF_SIZE);

    va_start(args, fmt);

    length = vsnprintf(log_buf, sizeof(log_buf) - 1, fmt, args);
    if (length > CONSOLEBUF_SIZE - 1)
    {
        length = CONSOLEBUF_SIZE - 1;
    }

    while (length--)
    {
#ifndef BACKTRACE_SIM
        hal_uart_put_char(CONSOLE_UART, log_buf[i++]);
#else
        putchar(log_buf[i++]);
        fflush(stdout);
#endif
    }

    va_end(args);

    return 0;
}

void panic_print_registers(const void *f, int core)
{
    int x, y;

    XtExcFrame *frame = (XtExcFrame *) f;
    int *regs = (int *)frame;

    const char *sdesc[] =
    {
        "PC      ", "PS      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
        "A6      ", "A7      ", "A8      ", "A9      ", "A10     ", "A11     ", "A12     ", "A13     ",
        "A14     ", "A15     ", "SAR     ", "EXCCAUSE", "EXCVADDR", "LBEG    ", "LEND    ", "LCOUNT  "
    };

    panic_print_str("Core ");
    panic_print_dec(core);
    panic_print_str(" register dump:");

    for (x = 0; x < 24; x += 4)
    {
        panic_print_str("\r\n");
        for (y = 0; y < 4; y++)
        {
            if (sdesc[x + y][0] != 0)
            {
                panic_print_str(sdesc[x + y]);
                panic_print_str(": 0x");
                panic_print_hex(regs[x + y + 1]);
                panic_print_str("  ");
            }
        }
    }
    panic_print_str("\r\n");
}


void show_exception_information_for_core(const void *f, int core)
{
    panic_print_str("\r\n");
    panic_print_str("-----------------------------------------\r\n");
    panic_print_str("-----------------------------------------\r\n");
    panic_print_registers(f, core);
    panic_print_str("\r\n");

    panic_print_str("------------\r\n");
    arch_backtrace_exception(printk, (void*)f);
    panic_print_str("------------\r\n");
    while (1);
}
#endif

