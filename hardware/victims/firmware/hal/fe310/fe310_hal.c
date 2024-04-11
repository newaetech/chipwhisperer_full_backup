#include <metal/gpio.h>
#include <metal/uart.h>
#include <metal/machine.h>
#include <metal/drivers/sifive_uart0.h>

#include "fe310_hal.h"

static struct metal_gpio *gpio_device;

void platform_init(void)
{
    gpio_device = metal_gpio_get_device(0);
}

void init_uart(void)
{
    ;
}

void trigger_setup(void)
{
    metal_gpio_enable_output(gpio_device, 13);
}

void trigger_high(void)
{
    metal_gpio_set_pin(gpio_device, 13, 1);
}

void trigger_low(void)
{
    metal_gpio_set_pin(gpio_device, 13, 0);
}   

char getch(void)
{
    int c;
    do {
     metal_uart_getc((struct metal_uart *)&__metal_dt_serial_10013000, &c);     
    }while(c == -1);
    return (char)(c & 0xff);
}

void putch(char c)
{
    metal_uart_putc((struct metal_uart *)&__metal_dt_serial_10013000, c);
}

