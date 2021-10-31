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
#include <stdint.h>
#include <stdlib.h>

#include "aes-independant.h"
#include "simpleserial.h"

#define ECB 0	// Electronic code book
#define CBC 1	// Cipher block chaining
#define CFB 2	// Cipher feedback
#define OFB 3	// Output feedback
#define CTR 4	// Counter

// Change this mode to pick block cipher mode
#define BLOCK_MODE ECB

uint8_t pt[16];   	// Plaintext
uint8_t ct[16];   	// Ciphertext

void update_key(uint8_t* k, uint8_t len)
{
	aes_indep_key(k);
}

void encrypt(uint8_t* pt, uint8_t len)
{
	trigger_high();
	static uint8_t input[16];
	static uint8_t output[16];

	// Find input
	switch(BLOCK_MODE)
	{
		case ECB:
			for(int i = 0; i < 16; i++)
				input[i] = pt[i];
			break;

		case CBC:
			for(int i = 0; i < 16; i++)
				input[i] = pt[i] ^ ct[i];
			break;

		case CFB:
			for(int i = 0; i < 16; i++)
				input[i] = ct[i];
			break;

		case OFB:
			for(int i = 0; i < 16; i++)
				input[i] = output[i];
			break;

		case CTR:
			// Poor man's CTR: just a 1 byte counter (can encrypt 256 blocks)
			input[0]++;
			break;
	}

	// Encrypt in place
	for(int i = 0; i < 16; i++)
		output[i] = input[i];
	aes_indep_enc(output);

	// Use output to calculate new ciphertext
	switch(BLOCK_MODE)
	{
		case ECB:
		case CBC:
			for(int i = 0; i < 16; i++)
				ct[i] = output[i];
			break;

		case CFB:
		case OFB:
		case CTR:
			for(int i = 0; i < 16; i++)
				ct[i] = output[i] ^ pt[i];
			break;
	}
	trigger_low();

	simpleserial_put('r', 16, ct);
}

void no_op(uint8_t* x, uint8_t len)
{
}

//Default key
uint8_t tmp[KEY_LENGTH] = {DEFAULT_KEY};

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

	//Initial key
	aes_indep_init();
	aes_indep_key(tmp);

	simpleserial_addcmd('k', 16, update_key);
    simpleserial_addcmd('p', 16, encrypt);
    simpleserial_addcmd('x', 0, no_op);
    while(1)
        simpleserial_get();
}
