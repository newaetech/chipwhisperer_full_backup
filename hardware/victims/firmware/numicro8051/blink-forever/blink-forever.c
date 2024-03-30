/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2020 NewAE Technology Inc.

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

#include "hal.h"
#include "Common.h"
#include "isp_uart0.h"
#include <stdint.h>
#include <stdlib.h>
#include "Delay.h"
#include "clock.h"

// Half a second
#define BLINK_DELAY 50

int main(void)
{
    P03_PUSHPULL_MODE;
    P12_PUSHPULL_MODE;
    P05_PUSHPULL_MODE;
    P03 = 1;
    P12 = 0;
    P05 = 0;
    while(1){
        led_ok(1);
        Timer1_Delay10ms(BLINK_DELAY);
        led_ok(0);
        Timer1_Delay10ms(BLINK_DELAY);
    }
}
