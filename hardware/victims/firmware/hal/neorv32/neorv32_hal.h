
#ifndef NEORV_HAL_H
#define NEORV_HAL_H
#include <stdint.h>

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

void neorv32_uart0_print(const char *s);
void neorv32_uart0_printf(const char *format, ...);
#define uart_printf neorv32_uart0_printf
#define uart_puts neorv32_uart0_print

#endif // STM32F3_HAL_H