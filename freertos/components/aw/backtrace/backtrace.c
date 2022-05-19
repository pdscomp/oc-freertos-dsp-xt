#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <xtensa_context.h>
#include <backtrace.h>

#define AW_OK (0)
#define AW_FAIL (-1)


/* dsp memory mapping of non-cacheable */
#define SOC_SRAM_LOW            0x00020000
#define SOC_SRAM_HIGH           0x00027FFF
#define SOC_OUTER_IRAM0_LOW     0x00028000
#define SOC_OUTER_IRAM0_HIGH    0x00037FFF
#define SOC_OUTER_DRAM0_LOW     0x00038000
#define SOC_OUTER_DRAM0_HIGH    0x0003FFFF
#define SOC_OUTER_DRAM1_LOW     0x00040000
#define SOC_OUTER_DRAM1_HIGH    0x00047FFF
#define SOC_INNER_IRAM0_LOW     0x00400000
#define SOC_INNER_IRAM0_HIGH    0x0040FFFF
#define SOC_INNER_DRAM0_LOW     0x00420000
#define SOC_INNER_DRAM0_HIGH    0x00427FFF
#define SOC_INNER_DRAM1_LOW     0x00440000
#define SOC_INNER_DRAM1_HIGH    0x00447FFF
#define SOC_DRAM_LOW            0x10000000
#define SOC_DRAM_HIGH           0x1FFFFFFF

/* dsp memory mapping of cacheable */
#define SOC_CACHE_SRAM_LOW      0x20020000
#define SOC_CACHE_SRAM_HIGH     0x20027FFF
#define SOC_CACHE_IRAM0_LOW     0x20028000
#define SOC_CACHE_IRAM0_HIGH    0x20037FFF
#define SOC_CACHE_DRAM0_LOW     0x20038000
#define SOC_CACHE_DRAM0_HIGH    0x2003FFFF
#define SOC_CACHE_DRAM1_LOW     0x20040000
#define SOC_CACHE_DRAM1_HIGH    0x20047FFF
#define SOC_CACHE_PSRAM_LOW     0x30000000
#define SOC_CACHE_PSRAM_HIGH    0x3FFFFFFF

/* dsp memory mapping of special dram */
#define SOC_SPECIAL_DRAM_NON_CACHE_LOW      0x40000000
#define SOC_SPECIAL_DRAM_NON_CACHE_HIGH     0x7FFFFFFF
#define SOC_SPECIAL_DRAM0_CACHE_LOW         0x80000000
#define SOC_SPECIAL_DRAM0_CACHE_HIGH        0xBFFFFFFF
#define SOC_SPECIAL_DRAM1_CACHE_LOW         0xC0000000
#define SOC_SPECIAL_DRAM1_CACHE_HIGH        0xFFFFFFFF


#define EXCCAUSE_INSTR_PROHIBITED   20
#define CONSOLE_UART (0)

typedef struct
{
    uint32_t pc;
    uint32_t sp;
    uint32_t next_pc;
} backtrace_frame_t;

extern void esp_backtrace_get_start(uint32_t *pc, uint32_t *sp, uint32_t *next_pc);


static bool check_dsp_memory_mapping_of_non_cacheable(uint32_t ip)
{
        return ((ip >= SOC_SRAM_LOW && ip < SOC_SRAM_HIGH)
            || (ip >= SOC_OUTER_IRAM0_LOW && ip < SOC_OUTER_IRAM0_HIGH)
            || (ip >= SOC_OUTER_DRAM0_LOW && ip < SOC_OUTER_DRAM0_HIGH)
            || (ip >= SOC_OUTER_DRAM1_LOW && ip < SOC_OUTER_DRAM1_HIGH)
            || (ip >= SOC_INNER_IRAM0_LOW && ip < SOC_INNER_IRAM0_HIGH)
            || (ip >= SOC_INNER_DRAM0_LOW && ip < SOC_INNER_DRAM0_HIGH)
            || (ip >= SOC_INNER_DRAM1_LOW && ip < SOC_INNER_DRAM1_HIGH)
            || (ip >= SOC_DRAM_LOW && ip < SOC_DRAM_HIGH));

}

static bool check_dsp_memory_mapping_of_cacheable(uint32_t ip)
{
        return ((ip >= SOC_CACHE_SRAM_LOW && ip < SOC_CACHE_SRAM_HIGH)
            || (ip >= SOC_CACHE_IRAM0_LOW && ip < SOC_CACHE_IRAM0_HIGH)
            || (ip >= SOC_CACHE_DRAM0_LOW && ip < SOC_CACHE_DRAM0_HIGH)
            || (ip >= SOC_CACHE_DRAM1_LOW && ip < SOC_CACHE_DRAM1_HIGH)
            || (ip >= SOC_CACHE_PSRAM_LOW && ip < SOC_CACHE_PSRAM_HIGH));

}

static bool check_dsp_memory_mapping_of_specail_dram(uint32_t ip)
{
    return 0;
    /*
    return ((ip >= SOC_SPECIAL_DRAM_NON_CACHE_LOW && ip < SOC_SPECIAL_DRAM_NON_CACHE_HIGH)
         || (ip >= SOC_SPECIAL_DRAM0_CACHE_LOW && ip < SOC_SPECIAL_DRAM0_CACHE_HIGH)
         || (ip >= SOC_SPECIAL_DRAM1_CACHE_LOW && ip < SOC_SPECIAL_DRAM1_CACHE_HIGH));*/

}

static bool check_stack_ptr_is_sane(uint32_t ip)
{
    bool ret = 0;
    if ((ip & 0xF) != 0)
        return 0;

    ret = check_dsp_memory_mapping_of_non_cacheable(ip);
    if (ret)
        return ret;

    ret = check_dsp_memory_mapping_of_cacheable(ip);
    if (ret)
        return ret;

    ret = check_dsp_memory_mapping_of_specail_dram(ip);
    if (ret)
        return ret;

    return ret;

}

static uint32_t cpu_process_stack_pc(unsigned long pc)
{
    if (pc & 0x80000000)
    {
        pc = (pc & 0x3fffffff) | 0x00000000;
    }
    return pc;
}

static bool check_ptr_is_valid(const void *p)
{
    bool ret = 0;

    intptr_t ip = (intptr_t) p;

    ret = check_dsp_memory_mapping_of_non_cacheable(ip);
    if (ret)
        return ret;

    ret = check_dsp_memory_mapping_of_cacheable(ip);
    if (ret)
        return ret;

    ret = check_dsp_memory_mapping_of_specail_dram(ip);
    if (ret)
        return ret;

    return ret;

}

static bool backtrace_get_next_frame(backtrace_frame_t *frame)
{
    bool ret = 1;
    uint32_t remap_pc;
    void *base_save = (void *)frame->sp;
    frame->pc = frame->next_pc;
    frame->next_pc = *((unsigned long *)(base_save - 16));
    frame->sp =  *((uint32_t *)(base_save - 12));

    remap_pc = cpu_process_stack_pc(frame->pc);

    ret = check_ptr_is_valid((void *)remap_pc);
    if (ret == 0)
        return ret;

    ret = check_stack_ptr_is_sane(frame->sp);
    if (ret == 0)
        return ret;

    return ret;
}

static char *long2str(long num, char *str)
{
    char         index[] = "0123456789ABCDEF";
    unsigned long usnum   = (unsigned long)num;

    str[7] = index[usnum % 16];
    usnum /= 16;
    str[6] = index[usnum % 16];
    usnum /= 16;
    str[5] = index[usnum % 16];
    usnum /= 16;
    str[4] = index[usnum % 16];
    usnum /= 16;
    str[3] = index[usnum % 16];
    usnum /= 16;
    str[2] = index[usnum % 16];
    usnum /= 16;
    str[1] = index[usnum % 16];
    usnum /= 16;
    str[0] = index[usnum % 16];
    usnum /= 16;

    return str;
}

static int _backtrace(char *taskname, void *output[], int size, int offset, print_function print_func,
                      void *f, unsigned long exception_mode)
{
    int   level = 0;
    char backtrace_output_buf[] = "0x         \r\n";
    backtrace_frame_t stk_frame = {0};
    int depth = 100;

    if (output && size > 0)
    {
        memset(output, 0, size * sizeof(void *));
    }

    if (taskname)
    {
        return -1;
    }

    if (taskname == NULL && exception_mode != 1)
    {
        esp_backtrace_get_start(&(stk_frame.pc), &(stk_frame.sp), &(stk_frame.next_pc));
    }
    else if (exception_mode == 1)
    {
        XtExcFrame *frame = (XtExcFrame *) f;

        stk_frame.pc = frame->pc;
        stk_frame.sp = frame->a1;
        stk_frame.next_pc = frame->a0;
    }

    if (print_func != NULL)
    {
        long2str((long)cpu_process_stack_pc(stk_frame.pc), &backtrace_output_buf[2]);
        print_func(backtrace_output_buf);
    }

    if (output)
    {
        if (level >= offset && level - offset < size)
        {
            output[level - offset] = (void *)cpu_process_stack_pc(stk_frame.pc);
        }
        if (level - offset >= size)
        {
            goto out;
        }
    }
    level++;

    bool corrupted = !(check_stack_ptr_is_sane(stk_frame.sp) &&
                       (check_ptr_is_valid((void *)cpu_process_stack_pc(stk_frame.pc))));

    uint32_t i = ((depth <= 0) ? INT32_MAX : depth) - 1;
    while (i-- > 0 && stk_frame.next_pc != 0 && !corrupted)
    {
        if (!backtrace_get_next_frame(&stk_frame))
        {
            corrupted = true;
        }
        if (print_func != NULL)
        {
            long2str((long)cpu_process_stack_pc(stk_frame.pc), &backtrace_output_buf[2]);
            print_func(backtrace_output_buf);
        }

        if (output)
        {
            if (level >= offset && level - offset < size)
            {
                output[level - offset] = (void *)cpu_process_stack_pc(stk_frame.pc);
            }
            if (level - offset >= size)
            {
                goto out;
            }
        }
        level++;
    }

    if (corrupted && print_func)
    {
        print_func("backtrace : invalid pc\r\n");
    }

out:
    return level - offset < 0 ? 0 : level - offset;
}

int arch_backtrace(char *taskname, void *trace[], int size, int offset, print_function print_func)
{
    return _backtrace(taskname, trace, size, offset, print_func, NULL, 0);

}

int arch_backtrace_exception(print_function print_func,
                             void *frame)
{
    return _backtrace(NULL, NULL, 0, 0, print_func, frame, 1);
}
