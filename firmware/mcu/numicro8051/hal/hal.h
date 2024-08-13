#pragma once
#include "numicro_8051.h"
#include "pin_defines.h"

void platform_init(void);
void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

#define set_trigger(val) PIN_LED_STATUS = val
#define led_error(status) PIN_LED_ERROR = status
#define led_ok(status) PIN_LED_OK = status
