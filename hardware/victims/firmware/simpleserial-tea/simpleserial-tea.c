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

#include "hal.h"
#include <stdint.h>
#include <stdlib.h>

#include "tea.h"

#include "simpleserial.h"

void swap_endian(uint8_t* buf, int len)
{
	// Assumes that len is even
	uint8_t end = len - 1;
	for(int i = 0; i < len/2; i++)
	{
		uint8_t temp = buf[i];
		buf[i] = buf[end-i];
		buf[end-i] = temp;
	}
}

// Buffers to store data
uint8_t pt[TEA_BLOCK_BYTES];
uint8_t key[TEA_KEY_BYTES];

void update_key(uint8_t* k, uint8_t len)
{
	swap_endian(key+0 , 4);
	swap_endian(key+4 , 4);
	swap_endian(key+8 , 4);
	swap_endian(key+12, 4);
	for(int i = 0; i < TEA_KEY_BYTES; i++)
	{
		key[i] = k[i];
	}
}
void encrypt(uint8_t* pt, uint8_t len)
{
	trigger_high();
	tea_encrypt((uint32_t*)pt, (uint32_t*)key);
	swap_endian(pt+0, 4);
	swap_endian(pt+4, 4);
	trigger_low();

	simpleserial_put('r', TEA_BLOCK_BYTES, pt);
}

void no_op(uint8_t* x, uint8_t len)
{
}

int main(void)
{
    platform_init();
	init_uart();
	trigger_setup();

 	/* Uncomment this to get a HELLO message for debug */
	/*
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	*/

	simpleserial_addcmd('k', TEA_KEY_BYTES, update_key);
    simpleserial_addcmd('p', TEA_BLOCK_BYTES, encrypt);
    simpleserial_addcmd('x', 0, no_op);
    while(1)
        simpleserial_get();
}

