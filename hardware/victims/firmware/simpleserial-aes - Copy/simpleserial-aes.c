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
#include "aes.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static uint8_t my_key[16] = {0};


void *local_memset(void *ptr, int val, size_t num)
{
    return memset(ptr, val, num);
}

void *local_memcpy(void *dst, const void *src, size_t num)
{
    return memcpy(dst, src, num);
}

uint32_t state = 0xFAFFB001;
int get_random(uint8_t *buf, size_t len)
{
    for (int i = 0; i < len; i++) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        buf[i] = state;
    }
    return 0x00;
}
STRUCT_AES aes_state;
uint8_t get_mask(uint8_t* m)
{
  return 0x00;
}

uint8_t get_key(uint8_t* k)
{
    for (int i = 0 ; i < 16; i++) {
        my_key[i] = k[i];
    }
    uint8_t ret = aes(MODE_KEYINIT, &aes_state, my_key, NULL, NULL, NULL, NULL);
	return ret;
}

uint8_t get_pt(uint8_t* pt)
{
    uint8_t my_ct[16] = {0};
    
	trigger_high();
    uint8_t ret = aes(MODE_ENC, &aes_state, my_key, pt, my_ct, NULL, NULL);

	trigger_low();
    
    
	simpleserial_put('r', 16, my_ct);
	return ret;
}

uint8_t reset(uint8_t* x)
{
    // Reset key here if needed
	return 0x00;
}

int main(void)
{
	uint8_t tmp[KEY_LENGTH] = {DEFAULT_KEY};
    aes(MODE_AESINIT_ENC, &aes_state, NULL, NULL, NULL, NULL, NULL);

    platform_init();
    init_uart();
    trigger_setup();

    /* Uncomment this to get a HELLO message for debug */

	simpleserial_init();
    simpleserial_addcmd('k', 16, get_key);
    simpleserial_addcmd('p', 16,  get_pt);
    simpleserial_addcmd('x',  0,   reset);
    simpleserial_addcmd('m', 18, get_mask);
    while(1)
        simpleserial_get();
}
