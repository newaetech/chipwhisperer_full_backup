/* present_common.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
/**
 * present_common.c
 * a implementation of the PRESENT block-cipher
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * */
 
#include <string.h>
#include <stdint.h> 
#include "memxor.h"
#include <avr/pgmspace.h>

uint8_t present_sbox(uint8_t b){
	static const uint8_t sb[] PROGMEM = {
		0xC, 0x5, 0x6, 0xB,
		0x9, 0x0, 0xA, 0xD,
		0x3, 0xE, 0xF, 0x8,
		0x4, 0x7, 0x1, 0x2
	};
	return (pgm_read_byte(&sb[b >> 4]) << 4) | pgm_read_byte(&sb[b & 0xf]);
}

uint8_t present_sbox_inv(uint8_t b){
	static const uint8_t sb[] PROGMEM = {
		0x5, 0xE, 0xF, 0x8,
		0xC, 0x1, 0x2, 0xD,
		0xB, 0x4, 0x6, 0x3,
		0x0, 0x7, 0x9, 0xA
	};
	return (pgm_read_byte(&sb[b >> 4]) << 4) | pgm_read_byte(&sb[b & 0xf]);
}

void present_p(uint8_t* o, uint8_t* i){
	uint8_t m,n=0,idx=0;
	for(m=0; m<64; ++m){
		o[idx] <<= 1;
		o[idx] |= i[n] >> 7;
		i[n] <<= 1;
		idx = (idx + 2) & 7;
		if((m & 7) == 7){
			++n;
		}
		if(m == 31){
			idx += 1;
		}
	}
}


void present_generic_enc(void* buffer, uint8_t* ctx, uint8_t ksize_B,
	void(*update)(uint8_t*, uint8_t)){
	uint8_t i,j,tmp[8], k[ksize_B];
	memcpy(k, ctx, ksize_B);
	memxor(buffer, k, 8);
	for(i=1; i<32; ++i){
		j = 7;
		do{
			tmp[j] = present_sbox(((uint8_t*)buffer)[j]);
		}while(j--);
		present_p(buffer, tmp);
		update(k, i);
		memxor(buffer, k, 8);
	}
}

void present_generic_dec(void* buffer, uint8_t* ctx, uint8_t ksize_B,
	void(*update)(uint8_t*, uint8_t)){
	uint8_t j,tmp[8], k[ksize_B];
	uint8_t i;
	memcpy(k, ctx + ksize_B, ksize_B);
	memxor(buffer, k, 8);
	i = 31;
	do{
		present_p(tmp, buffer);
		present_p(buffer, tmp);
		j = 7;
		do{
			((uint8_t*)buffer)[j] = present_sbox_inv(((uint8_t*)buffer)[j]);
		}while(j--);
		update(k, i);
		memxor(buffer, k, 8);
	}while(--i);
}

