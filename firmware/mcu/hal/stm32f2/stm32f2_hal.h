#ifndef STM32F2_HAL_H
#define STM32F2_HAL_H

//You probably don't need this from rest of code
//#include "stm32f2_hal_lowlevel.h"

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

#endif // STM32F2_HAL_H
