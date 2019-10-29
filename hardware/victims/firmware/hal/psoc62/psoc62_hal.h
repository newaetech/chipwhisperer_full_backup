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

#ifndef PSOC62_HAL_H_
#define PSOC62_HAL_H_

#include "cy_device_headers.h"
#include "cycfg.h"

#define trigger_setup()
#define trigger_high()  {Cy_GPIO_Write(ioss_0_port_3_pin_2_PORT, 2, 1);}
#define trigger_low()   {Cy_GPIO_Write(ioss_0_port_3_pin_2_PORT, 2, 0);}

/*
#define aes_indep_init aes_init
#define aes_indep_key  aes_set_key
#define aes_indep_enc  aes_encrypt
*/

void init_uart(void);
void putch(char c);
char getch(void);

#endif //SAM4L_HAL_H_