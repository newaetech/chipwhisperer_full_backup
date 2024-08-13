/* present128.c */
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
 * present128.c
 * a implementation of the PRESENT block-cipher
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * */
 
#include <string.h>
#include <stdint.h> 
#include <avr/pgmspace.h>
#include "memxor.h"
#include "present_common.h"
#include "present128.h"

static
void key_update_128(uint8_t* buffer, uint8_t round){
	uint8_t j;
	uint8_t t8;
	union __attribute__((packed)){
		uint8_t   v8[2];
		uint16_t v16;
	} tmp;
	/* rotate buffer 67 right */
	for(j=0; j<8; ++j){
		tmp.v8[0] = buffer[j];
		buffer[j] = buffer[j + 8];
		buffer[j + 8] = tmp.v8[0];
	}
	j=0;
	t8 = (uint16_t)buffer[15] << (5);
	do{
		tmp.v8[1] = buffer[j];
		tmp.v16 >>= 3;
		buffer[j] = tmp.v8[1] | t8;
		t8 = tmp.v8[0] & 0xe0;
	}while(++j<16);
	/* rotating done now substitution */
	buffer[0] = present_sbox(buffer[0]);
	/* xor with round counter */
	buffer[8] ^= round << 6;
	buffer[7] ^= round >> 2;
}


static
void key_update_128_inv(uint8_t* buffer, uint8_t round){
	uint8_t j;
	uint8_t t8;
	union __attribute__((packed)){
		uint8_t   v8[2];
		uint16_t v16;
	} tmp;
	/* xor with round counter */
	buffer[8] ^= round << 6;
	buffer[7] ^= round >> 2;

	/* rotating done now substitution */
	buffer[0] = present_sbox_inv(buffer[0]);

	/* rotate buffer 67 left */
	for(j=0; j<8; ++j){
		tmp.v8[0] = buffer[j];
		buffer[j] = buffer[j + 8];
		buffer[j + 8] = tmp.v8[0];
	}
	j=15;
	t8 = (uint16_t)buffer[0] >> (5);
	do{
		tmp.v8[0] = buffer[j];
		tmp.v16 <<= 3;
		buffer[j] = tmp.v8[0] | t8;
		t8 = tmp.v8[1] & 0x07;
	}while(j--);
}

void present128_init(const uint8_t* key, uint8_t keysize_b, present128_ctx_t* ctx){
	uint8_t i;
	memcpy(ctx->fwd_key, key, 16);
	memcpy(ctx->rev_key, key, 16);
	for(i=1; i<32; ++i){
		key_update_128(ctx->rev_key, i);
	}
}

void present128_enc(void* buffer, present128_ctx_t* ctx){
	present_generic_enc(buffer, (uint8_t*)ctx, 16, key_update_128);
}

void present128_dec(void* buffer, present128_ctx_t* ctx){
	present_generic_dec(buffer, (uint8_t*)ctx, 16, key_update_128_inv);
}

/*
void present128_enc(void* buffer, present128_ctx_t* ctx){
	uint8_t i,j,tmp[8], k[16];
	memcpy(k, ctx->fwd_key, 16);
	memxor(buffer, k, 8);
	for(i=1; i<32; ++i){
		j = 7;
		do{
			tmp[j] = present_sbox(((uint8_t*)buffer)[j]);
		}while(j--);
		present_p(buffer, tmp);
		key_update_128(k, i);
		memxor(buffer, k, 8);
	}
}

void present128_dec(void* buffer, present128_ctx_t* ctx){
	uint8_t j,tmp[8], k[16];
	uint8_t i;
	memcpy(k, ctx->rev_key, 16);
	memxor(buffer, k, 8);
	i = 31;
	do{
		present_p(tmp, buffer);
		present_p(buffer, tmp);
		j = 7;
		do{
			((uint8_t*)buffer)[j] = present_sbox_inv(((uint8_t*)buffer)[j]);
		}while(j--);
		key_update_128_inv(k, i);
		memxor(buffer, k, 8);
	}while(--i);
}
*/
