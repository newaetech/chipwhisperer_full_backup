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
#include "hal.h"
#include "xmega_hal.h"

void platform_init(void)
{  
    OSC.XOSCCTRL = 0x00;
    OSC.PLLCTRL = 0x00;
    OSC.CTRL |= OSC_XOSCEN_bm;
    
    //wait for clock
    while((OSC.STATUS & OSC_XOSCRDY_bm) == 0);
    
    //Switch clock source
    CCP = CCP_IOREG_gc;
    CLK.CTRL = CLK_SCLKSEL_XOSC_gc;    
    
    //Turn off other sources besides external    
    OSC.CTRL = OSC_XOSCEN_bm;
    
 #if PLATFORM == CW303
    PORTA.DIRSET = PIN5_bm | PIN6_bm;
    PORTA.OUTSET = PIN5_bm | PIN6_bm;
 #endif
}

#if HWCRYPTO
#include "XMEGA_AES_driver.h"
static uint8_t enckey[16];

void HW_AES128_Init(void)
{
    return;
}

void HW_AES128_LoadKey(uint8_t * key)
{
	for(uint8_t i=0; i < 16; i++){
		enckey[i] = key[i];
	}
}

void HW_AES128_Enc(uint8_t * pt)
{
    AES_encrypt(pt, pt, enckey);
}
#endif