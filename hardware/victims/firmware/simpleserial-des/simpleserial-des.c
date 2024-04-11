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

#include "des.h"
#define KEY_LENGTH 8
#define DEFAULT_KEY {0,1,2,3,4,5,6,7}

#include "simpleserial.h"

static uint8_t key[KEY_LENGTH] = {DEFAULT_KEY};

void update_key(uint8_t* k, uint8_t len)
{
	for(int i = 0; i < KEY_LENGTH; i++)
	{
		key[i] = k[i];
	}
}

void encrypt(uint8_t* pt, uint8_t len)
{
	/* Do Encryption */
	//NOTE: Setup for DES is huge - about 15000(!) clock cycles. This results in
	// a large and annoying offset.
	// Instead trigger_high() is called within the DES algorithm.
	//trigger_high();

	uint8_t ct[8];
	des_enc(ct, pt, key); /* encrypting the data block */
	trigger_low();

	simpleserial_put('r', 8, ct);
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

    simpleserial_addcmd('k', KEY_LENGTH, update_key);
	simpleserial_addcmd('p', 8, encrypt);
	simpleserial_addcmd('x', 0, no_op);
	while(1)
		simpleserial_get();
}



