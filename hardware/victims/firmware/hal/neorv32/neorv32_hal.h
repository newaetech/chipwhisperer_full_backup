
#ifndef NEORV_HAL_H
#define NEORV_HAL_H
#include <stdint.h>

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

#endif // STM32F3_HAL_H