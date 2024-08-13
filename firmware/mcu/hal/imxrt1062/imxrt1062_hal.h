/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2019-2020 NewAE Technology Inc.

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

#ifndef IMXRT1062_HAL_H_
#define IMXRT1062_HAL_H_

#include <stdint.h>

void trigger_setup(void);
void trigger_high(void); 
void trigger_low(void);

void init_uart(void);
void putch(char c);
char getch(void);

/* MXRT1062 has Glitch Detector, which seems to detect power on/off mostly */
int hal_glitch_detected(void);
void hal_glitch_detect_reset(void);

void HW_AES128_Init(void);
void HW_AES128_LoadKey(uint8_t * key);
void HW_AES128_Enc_pretrigger(uint8_t * pt);
void HW_AES128_Enc(uint8_t * pt);
void HW_AES128_Enc_posttrigger(uint8_t * pt);

#endif //IMXRT1062