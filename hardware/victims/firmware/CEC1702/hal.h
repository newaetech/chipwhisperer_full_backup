#ifndef HAL_H
#define HAL_H

void putch(char c);

char getch(void);

void platform_init(void);

void init_uart(void);

void trigger_setup(void);

void trigger_low(void);

void trigger_high(void);

#endif