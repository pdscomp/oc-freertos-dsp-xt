#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h> // uint32_t
#include <hal_gpio.h>
#include <sunxi_hal_pwm.h>

#include <sunxi_hal_spi.h>

// Missing constants for bit manipulation
#define PINS_BANK_MASK      0x1F
#define MUX_OFFSET_BITS     5
#define MUX_OFFSET_MASK     0x3
#define MUX_SHIFT_MASK      0x7
#define MUX_SHIFT_BITS      2
#define PULL_OFFSET_BITS    4
#define PULL_OFFSET_MASK    0x1
#define PULL_SHIFT_MASK     0xF
#define PULL_SHIFT_BITS     1

// GPIO register structure for SUN8IW20
typedef struct {
    volatile uint32_t AFR[4];       // 0x00-0x0C: Alternate function (mux) registers
    volatile uint32_t ODR;          // 0x10: Output data register
    volatile uint32_t DLEVEL[2];    // 0x14-0x18: Drive level registers
    volatile uint32_t _reserved1[2];// 0x1C-0x20: Reserved
    volatile uint32_t PUPDR[2];     // 0x24-0x28: Pull up/down registers
    volatile uint32_t _reserved2[1];// 0x2C: Reserved to pad to 0x30
} GPIO_TypeDef;

// GPIO input pin structure
struct gpio_in {
    GPIO_TypeDef *regs;
    uint32_t bit;
    uint8_t pin;
};

// GPIO output pin structure
struct gpio_out {
    GPIO_TypeDef *regs;
    uint32_t bit;
    uint8_t pin;
};
extern struct gpio_out g_GAM_DBG_gpio1;
extern struct gpio_out g_GAM_DBG_gpio2;
extern struct gpio_out g_GAM_DBG_gpio3;
extern  struct gpio_out g_GAM_DBG_gpio4;

struct gpio_out gpio_out_setup(gpio_pin_t pin, gpio_data_t data);
void gpio_out_reset(struct gpio_out *g, uint32_t val);
void gpio_out_toggle_noirq(struct gpio_out *g);
void gpio_out_toggle(struct gpio_out *g);
void gpio_out_write(struct gpio_out *g, uint32_t val);


struct gpio_in gpio_in_setup(gpio_pin_t pin, int32_t pull);
void gpio_in_reset(struct gpio_in *g, gpio_pull_status_t pull_up);
uint8_t gpio_in_read(struct gpio_in *g);

struct gpio_pwm {
  void *reg;
};
struct gpio_pwm gpio_pwm_setup(uint8_t pin, uint32_t cycle_time, uint8_t val);
void gpio_pwm_write(struct gpio_pwm g, uint32_t val);

struct gpio_adc {
    void *adc;
    uint32_t chan;
};
int gpio_adc_setup(struct gpio_adc* g,uint32_t channel);
uint32_t gpio_adc_sample(struct gpio_adc *g);
uint16_t gpio_adc_read(struct gpio_adc *g);
void gpio_adc_cancel_sample(struct gpio_adc* g);

struct spi_config {
    hal_spi_master_port_t  port;
    hal_spi_master_config_t cfg;
};
struct spi_config spi_setup(uint32_t bus, uint8_t mode, uint32_t rate);
void spi_prepare(struct spi_config config);
void spi_transfer(struct spi_config config, uint8_t receive_data
                  , uint8_t len, uint8_t *data);

struct i2c_config {
    void *i2c;
    uint8_t addr;
};

struct i2c_config i2c_setup(uint32_t bus, uint32_t rate, uint8_t addr);
void i2c_write(struct i2c_config config, uint8_t write_len, uint8_t *write);
void i2c_read(struct i2c_config config, uint8_t reg_len, uint8_t *reg
              , uint8_t read_len, uint8_t *read);

void gpio_peripheral(struct gpio_in *g, gpio_muxsel_t mux, gpio_pull_status_t pull);

#endif // gpio.h
