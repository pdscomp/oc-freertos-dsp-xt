/*
 * ===========================================================================================
 * copy from melis3.0 script.h
 * ===========================================================================================
 */

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

typedef struct
{
    int main_key_count;
    int version[3];
} script_head_t;

typedef struct
{
    char main_name[32];
    int  lenth;
    int  offset;
} script_main_key_t;

typedef struct
{
    char sub_name[32];
    int  offset;
    int  pattern;
} script_sub_key_t;

typedef struct
{
    char gpio_name[32];
    int port;
    int port_num;
    int mul_sel;
    int pull;
    int drv_level;
    int data;
} user_gpio_set_t;

typedef struct
{
    char  *script_mod_buf;
    int    script_mod_buf_size;
    int    script_main_key_count;    
}script_parser_t;

#endif // __SCRIPT_H__


