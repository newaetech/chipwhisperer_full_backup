/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2015 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XMEGA_HAL_H_
#define XMEGA_HAL_H_

//Generic Platform
#include "uart.h"

//We want to use the AVR ADC-pins, since they have a seperate power rail
#define trigger_setup() PORTA.DIRSET = PIN0_bm
#define trigger_high()  PORTA.OUTSET = PIN0_bm
#define trigger_low()   PORTA.OUTCLR = PIN0_bm

#define init_uart init_uart0
#define putch output_ch_0
#define getch input_ch_0

#if PLATFORM == CW303
#define led_error(a) if (a) {PORTA.OUTCLR = PIN6_bm;} else {PORTA.OUTSET = PIN6_bm;}
#define led_ok(a) if (a) {PORTA.OUTCLR = PIN5_bm;} else {PORTA.OUTSET = PIN5_bm;}
#endif

void HW_AES128_Init(void);
void HW_AES128_LoadKey(uint8_t * key);
void HW_AES128_Enc(uint8_t * pt);

#endif //AVR_HAL_H_

   