#ifndef STM32F4_HAL_H
#define STM32F4_HAL_H

//You probably don't need this from rest of code
//#include "stm32f4_hal_lowlevel.h"

#include <stdint.h>

void init_uart(void);
void putch(char c);
char getch(void);

uint32_t get_rand(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);

void HW_AES128_Init(void);
void HW_AES128_LoadKey(uint8_t* key);
void HW_AES128_Enc_pretrigger(uint8_t* pt);
void HW_AES128_Enc(uint8_t* pt);
void HW_AES128_Enc_posttrigger(uint8_t* pt);
void HW_AES128_Dec(uint8_t *pt);

#endif // STM32F4_HAL_H
