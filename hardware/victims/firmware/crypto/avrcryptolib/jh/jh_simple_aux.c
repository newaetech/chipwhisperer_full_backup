/* jh_simple_speed.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2010 Daniel Otte (daniel.otte@rub.de)

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

#include <stdint.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include "memxor.h"
#include "jh_simple.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

void jh_encrypt(uint8_t *a);

void jh_init(uint16_t hashlen_b, jh_ctx_t* ctx){
	memset(ctx->a, 0, 128);
	ctx->a[0] = hashlen_b>>8;
	ctx->a[1] = hashlen_b&0xff;
	jh_encrypt(ctx->a);
	ctx->block_hashed=0;
}

void jh_nextBlock(jh_ctx_t* ctx, void* block){
	memxor(ctx->a, block, 64);
	jh_encrypt(ctx->a);
	memxor(ctx->a+64, block, 64);
	ctx->block_hashed++;
}

void jh_lastBlock(jh_ctx_t* ctx, void* block, uint16_t length_b){
	while(length_b>=64*8){
		jh_nextBlock(ctx, block);
		block = (uint8_t*)block + 64;
		length_b -= 64*8;
	}
	uint8_t buffer[64];
	uint64_t total_length;
	memset(buffer, 0, 64);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b%8);
	total_length=ctx->block_hashed*512+length_b;
	if(length_b==0){

	}else{
		jh_nextBlock(ctx, buffer);
		buffer[0]=0;
	}
	memset(buffer+1, 0, 64-8-1);
	buffer[63] = total_length&0xff;
	buffer[62] = (total_length>> 8)&0xff;
	buffer[61] = (total_length>>16)&0xff;
	buffer[60] = (total_length>>24)&0xff;
	buffer[59] = (total_length>>32)&0xff;
	buffer[58] = (total_length>>40)&0xff;
	buffer[57] = (total_length>>48)&0xff;
	buffer[56] = (total_length>>56)&0xff;
	jh_nextBlock(ctx, buffer);
}

void jh_ctx2hash(void* dest, uint16_t length_b, jh_ctx_t* ctx){
	memcpy(dest, ctx->a+128-(length_b+7)/8, (length_b+7)/8);
}


void jh224_init(jh_ctx_t* ctx){
	jh_init(224, ctx);
}

void jh224_ctx2hash(void* dest, jh_ctx_t* ctx){
	jh_ctx2hash(dest, 224, ctx);
}

void jh256_init(jh_ctx_t* ctx){
	jh_init(256, ctx);
}

void jh256_ctx2hash(void* dest, jh_ctx_t* ctx){
	jh_ctx2hash(dest, 256, ctx);
}

void jh384_init(jh_ctx_t* ctx){
	jh_init(384, ctx);
}

void jh384_ctx2hash(void* dest, jh_ctx_t* ctx){
	jh_ctx2hash(dest, 384, ctx);
}

void jh512_init(jh_ctx_t* ctx){
	jh_init(512, ctx);
}

void jh512_ctx2hash(void* dest, jh_ctx_t* ctx){
	jh_ctx2hash(dest, 512, ctx);
}
