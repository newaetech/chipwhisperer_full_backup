/* cubehash.c */
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
 * \file     cubehash.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2010-02-20
 * \license  GPLv3 or later
 *
 */


#include "memxor.h"
#include "cubehash.h"
#include "cubehash_rotates.h"
#include "xchg.h"
#include <string.h>
#include <stdint.h>

/*
• Add    x_0jklm into    x_1jklm modulo 2**32 , for each (j, k, l, m).
• Rotate x_0jklm upwards by 7 bits, for each (j, k, l, m).
• Swap   x_00klm with    x_01klm , for each (k, l, m).
• Xor    x_1jklm into    x_0jklm , for each (j, k, l, m).
• Swap   x_1jk0m with    x_1jk1m , for each (j, k, m).
• Add    x_0jklm into    x_1jklm modulo 2**32 , for each (j, k, l, m).
• Rotate x_0jklm upwards by 11 bits, for each (j, k, l, m).
• Swap   x_0j0lm with    x_0j1lm , for each (j, l, m).
• Xor    x_1jklm into    x_0jklm , for each (j, k, l, m).
• Swap   x_1jkl0 with    x_1jkl1 , for each (j, k, l).
*/

static void cubehash_round(cubehash_ctx_t* ctx){
	uint8_t i;
	uint32_t t, t2;
	for(i=0; i<16; ++i){
		ctx->a[i+16] += t = ctx->a[i];
		ctx->a[i] = rotate7left(t);
	}
	xchg32_array(&(ctx->a[0]), &(ctx->a[8]), 8);
	for(i=0; i<16; i+=4){
		t = ctx->a[i+16];
		t2 = ctx->a[i] ^= t;
		ctx->a[i+16] = ctx->a[i+18] + t2;
		ctx->a[i] = rotate11left(t2);
		t2 = ctx->a[i+2] ^= ctx->a[i+18];
		ctx->a[i+18] = t + t2;
		ctx->a[i+2] = rotate11left(t2);
		t = ctx->a[i+17];
		t2 = ctx->a[i+1] ^= t;
		ctx->a[i+17] = ctx->a[i+19] + t2;
		ctx->a[i+1] = rotate11left(t2);
		t2 = ctx->a[i+3] ^= ctx->a[i+19];
		ctx->a[i+19] = t + t2;
		ctx->a[i+3] = rotate11left(t2);
	}
	xchg32_array(&(ctx->a[0]), &(ctx->a[4]), 4);
	xchg32_array(&(ctx->a[8]), &(ctx->a[12]), 4);
	for(i=0; i<16; i+=2){
		ctx->a[i] ^= t = ctx->a[i+16];
		ctx->a[i+1] ^= ctx->a[i+16] = ctx->a[i+17];
		ctx->a[i+17] = t;
	}
}

void cubehash_init(uint8_t r, uint8_t b, uint16_t h, cubehash_ctx_t* ctx){
	memset(ctx->a, 0, 32*4);
	ctx->a[0] = h/8;
	ctx->a[1] = b;
	ctx->a[2] = r;
	ctx->rounds = r;
	ctx->blocksize_B = b;
	for(b=0; b<10*r; ++b){
		cubehash_round(ctx);
	}
}

void cubehash_nextBlock(cubehash_ctx_t* ctx, void* block){
	uint8_t i;
	memxor(ctx->a, block, ctx->blocksize_B);
	for(i=0; i<ctx->rounds; ++i){
		cubehash_round(ctx);
	}
}

void cubehash_lastBlock(cubehash_ctx_t* ctx, void* block, uint16_t length_b){
	while(length_b>=ctx->blocksize_B*8){
		cubehash_nextBlock(ctx, block);
		block = (uint8_t*)block + ctx->blocksize_B;
		length_b -= ctx->blocksize_B*8;
	}
	uint8_t buffer[ctx->blocksize_B];
	uint8_t i;
	memset(buffer, 0, ctx->blocksize_B);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80 >> (length_b&7);
	cubehash_nextBlock(ctx, buffer);
	ctx->a[31] ^= 1;
	for(i=0; i<10*(ctx->rounds); ++i){
		cubehash_round(ctx);
	}
}

void cubehash_ctx2hash(void* dest, uint16_t length_b, cubehash_ctx_t* ctx){
	memcpy(dest, ctx->a, (length_b+7)/8);
}

/******************************************************************************/

void cubehash224_init(cubehash_ctx_t* ctx){
	cubehash_init(16, 32, 224, ctx);
}

void cubehash224_ctx2hash(void* dest, cubehash_ctx_t* ctx){
	cubehash_ctx2hash(dest, 224, ctx);
}

/******************************************************************************/

void cubehash256_init(cubehash_ctx_t* ctx){
	cubehash_init(16, 32, 256, ctx);
}

void cubehash256_ctx2hash(void* dest, cubehash_ctx_t* ctx){
	cubehash_ctx2hash(dest, 256, ctx);
}

/******************************************************************************/

void cubehash384_init(cubehash_ctx_t* ctx){
	cubehash_init(16, 32, 384, ctx);
}

void cubehash384_ctx2hash(void* dest, cubehash_ctx_t* ctx){
	cubehash_ctx2hash(dest, 384, ctx);
}

/******************************************************************************/

void cubehash512_init(cubehash_ctx_t* ctx){
	cubehash_init(16, 32, 512, ctx);
}

void cubehash512_ctx2hash(void* dest, cubehash_ctx_t* ctx){
	cubehash_ctx2hash(dest, 512, ctx);
}
