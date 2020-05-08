/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2017 NewAE Technology Inc.

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

#ifndef SAM4L_HAL_H_
#define SAM4L_HAL_H_

#include "sam.h"

//Generic Platform
#include "uart.h"
#include "sam4l_aes.h"

//Pin PA06 choosen (mostly at random) to be GPIO4
//This can be overridden elsewhere
#define trigger_setup() {GPIO->GPIO_PORT[0].GPIO_ODERS = GPIO_PA06; GPIO->GPIO_PORT[0].GPIO_STERC = GPIO_PA06;}
#define trigger_high()  {GPIO->GPIO_PORT[0].GPIO_OVRS = GPIO_PA06;}
#define trigger_low()   {GPIO->GPIO_PORT[0].GPIO_OVRC = GPIO_PA06;}

void periclk_usart0_init(void);
void periclk_aesa_init(void);

#define init_uart uart0_init
#define putch uart0_putch
#define getch uart0_getch

void HW_AES128_Init(void);
void HW_AES128_LoadKey(uint8_t* key);
void HW_AES128_Enc(uint8_t* pt);
void HW_AES128_Enc_pretrigger(uint8_t* pt);
void HW_AES128_Enc_posttrigger(uint8_t* pt);
void HW_AES128_Dec(uint8_t *ct);

#endif //SAM4L_HAL_H_