/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

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

#include "aes-independant.h"
#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t expected[16];

uint8_t get_exp(uint8_t* t, uint8_t len)
{
	for(int i = 0; i < 16; i++)
		expected[i] = t[i];
	return 0x00;
}

uint8_t get_key(uint8_t* k, uint8_t len)
{
	aes_indep_key(k);
	return 0x00;
}

uint8_t get_pt(uint8_t* pt, uint8_t len)
{
	trigger_high();
	aes_indep_enc(pt); /* encrypting the data block */
	trigger_low();

	uint8_t ret = 0x10;
	for(int i = 0; i < 16; i++)
	{
		if(pt[i] != expected[i])
		{
			ret = 0x00;
			break;
		}
	}

	pt[0] = ret;
	for(int i = 1; i < 16; i++)
		pt[i] = 0;

	simpleserial_put('r', 16, pt);
	return 0x00;
}

uint8_t reset(uint8_t* x, uint8_t len)
{
    // Reset key here if needed
	return 0x00;
}

int main(void)
{
	uint8_t tmp[KEY_LENGTH] = {DEFAULT_KEY};

    platform_init();
    init_uart();
    trigger_setup();

	aes_indep_init();
	aes_indep_key(tmp);

    /* Uncomment this to get a HELLO message for debug */

    putch('h');
    putch('e');
    putch('l');
    putch('l');
    putch('o');
    putch('\n');

	simpleserial_init();
    simpleserial_addcmd('t', 16, get_exp);
    simpleserial_addcmd('k', 16, get_key);
    simpleserial_addcmd('p', 16,  get_pt);
    simpleserial_addcmd('x',  0,   reset);
    while(1)
        simpleserial_get();
}
