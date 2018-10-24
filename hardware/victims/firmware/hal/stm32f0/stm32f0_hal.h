#ifndef STM32F0_HAL_H
#define STM32F0_HAL_H

void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

void led_error(unsigned int status);
void led_ok(unsigned int status);

#endif // STM32F0_HAL_H
