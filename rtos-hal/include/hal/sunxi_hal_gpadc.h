/*
 * drivers/input/sensor/sunxi_gpadc.h
 *
 * Copyright (C) 2016 Allwinner.
 * fuzhaoke <fuzhaoke@allwinnertech.com>
 *
 * SUNXI GPADC Controller Driver Header
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef HAL_GPADC_H
#define HAL_GPADC_H

#include "hal_clk.h"
#include "sunxi_hal_common.h"
#include <hal_log.h>
#include <interrupt.h>
#include <hal_osal.h>

/* Include platform-specific clock definitions */
#if defined(CONFIG_ARCH_SUN8IW20)
#include <ccmu/sunxi-ng/ccu-sun8iw20.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_DRIVERS_GPADC_DEBUG
#ifdef CONFIG_DRIVERS_GPADC_DEBUG
#define GPADC_INFO(fmt, arg...) hal_log_info(fmt, ##arg)
#else
#define GPADC_INFO(fmt, arg...) do {}while(0)
#endif

#define GPADC_ERR(fmt, arg...) hal_log_err(fmt, ##arg)

/* Platform-specific definitions for SUN8IW20 */
#define GPADC_BASE          0x05070000
#define CHANNEL_MAX_NUM     8
#define CHANNEL_NUM         CHANNEL_MAX_NUM

#if defined(CONFIG_CORE_DSP0) /* DSP */
#define SUNXI_GPADC_IRQ     (RINTC_IRQ_MASK | 37)
#elif defined(CONFIG_ARCH_SUN8IW20) /* ARM */
#define SUNXI_GPADC_IRQ     (32 + 73)
#elif defined(CONFIG_SOC_SUN20IW1) /* RISC-V */
#define SUNXI_GPADC_IRQ     (89)
#endif

/* GPADC register offsets */
#define GP_SR_REG           0x00
#define GP_CTRL_REG         0x04
#define GP_CS_EN_REG        0x08
#define GP_FIFO_INTC_REG    0x0c
#define GP_FIFO_INTS_REG    0x10
#define GP_FIFO_DATA_REG    0x14
#define GP_CB_DATA_REG      0x18
#define GP_DATAL_INTC_REG   0x20
#define GP_DATAH_INTC_REG   0x24
#define GP_DATA_INTC_REG    0x28
#define GP_DATAL_INTS_REG   0x30
#define GP_DATAH_INTS_REG   0x34
#define GP_DATA_INTS_REG    0x38
#define GP_CH0_CMP_DATA_REG 0x40
#define GP_CH0_DATA_REG     0x80

/* GPADC register bit definitions */
#define GP_SR_CON           (0xffff << 16)
#define GP_CALI_EN          (1 << 17)
#define GP_CH0_CMP_EN       (1 << 16)
#define GP_CH_DATA_MASK     0xfff
#define GP_MODE_SELECT      (3 << 18)
#define GP_ADC_EN           (1 << 16)

/* GPADC constants */
#define VOL_RANGE           1800000  /* 1.8V in microvolts */
#define VOL_VALUE_MASK      0xfff    /* 12-bit ADC */
#define COMPARE_LOWDATA     1000000  /* 1.0V in microvolts */
#define COMPARE_HIGDATA     1500000  /* 1.5V in microvolts */
#define DEFAULT_SR          1000     /* Default sample rate */
#define OSC_24MHZ           24000000 /* 24MHz oscillator */
#define IRQF_NO_SUSPEND     0x00004000

/* Clock constants */
#define HAL_CLK_SRC_HOSC24M     4
#if defined(CONFIG_ARCH_SUN8IW20)
#define HAL_CLK_PERIPH_GPADC    CLK_BUS_GPADC
#endif

enum
{
    GPADC_DOWN,
    GPADC_UP
};

typedef enum
{
    GP_CH_0 = 0,
    GP_CH_1,
    GP_CH_2,
    GP_CH_3,
    GP_CH_4,
    GP_CH_5,
    GP_CH_6,
    GP_CH_7,
    GP_CH_MAX
} hal_gpadc_channel_t;

typedef enum
{
    GPADC_IRQ_ERROR = -4,
    GPADC_CHANNEL_ERROR = -3,
    GPADC_CLK_ERROR = -2,
    GPADC_ERROR = -1,
    GPADC_OK = 0,
} hal_gpadc_status_t;

typedef enum gp_select_mode
{
    GP_SINGLE_MODE = 0,
    GP_SINGLE_CYCLE_MODE,
    GP_CONTINUOUS_MODE,
    GP_BURST_MODE,
} hal_gpadc_mode_t;

typedef int (*gpadc_callback_t)(uint32_t data_type, uint32_t data);

typedef struct
{
    uint32_t reg_base;
    uint32_t channel_num;
    uint32_t irq_num;
    uint32_t sample_rate;
#if defined(CONFIG_SOC_SUN20IW1)
    hal_clk_id_t bus_clk;
    hal_clk_id_t rst_clk;
    hal_clk_t mbus_clk;
#else
    hal_clk_id_t mclk;
    hal_clk_id_t pclk;
#endif
    hal_gpadc_mode_t mode;
    gpadc_callback_t callback[CHANNEL_MAX_NUM];
} hal_gpadc_t;

int hal_gpadc_init(void);
hal_gpadc_status_t hal_gpadc_deinit(void);
hal_gpadc_status_t hal_gpadc_channel_init(hal_gpadc_channel_t channal);
hal_gpadc_status_t hal_gpadc_channel_exit(hal_gpadc_channel_t channal);
hal_gpadc_status_t hal_gpadc_register_callback(hal_gpadc_channel_t channal,
        gpadc_callback_t user_callback);

/* Helper macros for register access */
#define m_gpadc_channel_irq_status(base)            readl((unsigned long)(base) + GP_DATA_INTS_REG)
#define m_gpadc_channel_clear_irq(base, flags)      writel(flags, (unsigned long)(base) + GP_DATA_INTS_REG)
#define m_gpadc_channel_lowirq_status(base)         readl((unsigned long)(base) + GP_DATAL_INTS_REG)
#define m_gpadc_channel_clear_lowirq(base, flags)   writel(flags, (unsigned long)(base) + GP_DATAL_INTS_REG)
#define m_gpadc_channel_highirq_status(base)        readl((unsigned long)(base) + GP_DATAH_INTS_REG)
#define m_gpadc_channel_clear_highirq(base, flags)  writel(flags, (unsigned long)(base) + GP_DATAH_INTS_REG)
#define m_gpadc_read_channel_irq_enable(base)       readl((unsigned long)(base) + GP_DATA_INTC_REG)
#define m_gpadc_read_channel_lowirq_enable(base)    readl((unsigned long)(base) + GP_DATAL_INTC_REG)
#define m_gpadc_read_channel_highirq_enable(base)   readl((unsigned long)(base) + GP_DATAH_INTC_REG)

#ifdef __cplusplus
}
#endif

#endif
