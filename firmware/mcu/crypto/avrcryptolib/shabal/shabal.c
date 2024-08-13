/* shabal.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
/*
 * \file    shabal.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-17
 * \license GPLv3 or later
 * 
 */

#include <stdint.h>
#include "shabal.h"
#include <string.h>

#define SHABAL_O1 13
#define SHABAL_O2  9
#define SHABAL_O3  6


static inline
uint32_t shabal_u(uint32_t a){
	return (a<<1)+a; /* a*3 */
}

static inline
uint32_t shabal_v(uint32_t a){
	return (a<<2)+a; /* a*5 */
}

#define ROTL32(a,n) (((a)<<(n))|((a)>>(32-(n))))

static
void shabal_p(shabal_ctx_t* ctx, const void* m){
	uint8_t i,j;
	for(i=0;i<16;++i){
		ctx->b[i] = ROTL32(ctx->b[i],17);
	}
	for(j=0;j<SHABAL_P;j++){
		for(i=0;i<16;++i){
			ctx->a[(i+j*16)%SHABAL_R] = 
				shabal_u(ctx->a[(i+j*16)%SHABAL_R] 
					^ shabal_v(ROTL32(ctx->a[(i+j*16+SHABAL_R-1)%SHABAL_R],15))
				    ^ ctx->c[(8-i+16)%16])
				^ ctx->b[(i+SHABAL_O1)%16]
				^ ((ctx->b[(i+SHABAL_O2)%16]) & ~(ctx->b[(i+SHABAL_O3)%16]))
				^ ((uint32_t*)m)[i];
			ctx->b[i] = ROTL32(ctx->b[i], 1) ^ ~(ctx->a[(i+j*16)%SHABAL_R]);
		}
	}
	
	for(j=0;j<36;++j){
		ctx->a[j%SHABAL_R] += ctx->c[(j+3)%16];
	}
}

void shabal_nextBlock(shabal_ctx_t* ctx, const void* block){
	uint8_t i;
	uint32_t* t;
	for(i=0;i<16;++i){
		ctx->b[i] += ((uint32_t*)block)[i];
	}
	ctx->a[0] ^= ctx->w.w32[0];
	ctx->a[1] ^= ctx->w.w32[1];
	shabal_p(ctx, block);
	for(i=0;i<16;++i){
		ctx->c[i] -= ((uint32_t*)block)[i];
	}
	ctx->w.w64++;
	t = ctx->c;
	ctx->c = ctx->b;
	ctx->b = t;
}

void shabal_lastBlock(shabal_ctx_t* ctx, const void* block, uint16_t length_b){
	uint8_t i,j;
	uint32_t* t;
	uint8_t buffer[64];
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(ctx, block);
		block = (uint8_t*)block + SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	memset(buffer, 0, 64);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b%8);
	
	for(i=0;i<16;++i){
		ctx->b[i] += ((uint32_t*)buffer)[i];
	}

	for(j=0; j<4;++j){
		ctx->a[0] ^= ctx->w.w32[0];
		ctx->a[1] ^= ctx->w.w32[1];
		shabal_p(ctx, buffer);
		t = ctx->c;
		ctx->c = ctx->b;
		ctx->b = t;
	}
	 
}

void shabal_ctx2hash(void* dest, const shabal_ctx_t* ctx, uint16_t outlength_b){
	memcpy(dest, &(ctx->c[16-outlength_b/32]), outlength_b/8);
}
