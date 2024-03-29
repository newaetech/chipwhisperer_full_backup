#pragma once
#include "numicro_8051.h"

void platform_init(void);
void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

#define set_trigger(val) P04 = val
#define led_error(status) P05 = status
#define led_ok(status) P12 = status
