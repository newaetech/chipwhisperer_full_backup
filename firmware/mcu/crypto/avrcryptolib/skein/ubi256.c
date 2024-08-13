/* ubi256.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2010  Daniel Otte (daniel.otte@rub.de)

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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-12
 * \license GPLv3 or later
 * 
 */

#include <stdint.h>
#include <string.h>
#include "threefish.h"
#include "memxor.h"
#include "ubi.h"

void ubi256_init(ubi256_ctx_t* ctx, const void* g, uint8_t type){
	memset(ctx->tweak.v8, 0, 15);
	ctx->tweak.v8[15] = 0x40+type;
	memcpy(ctx->g, g, 32);
}

void ubi256_nextBlock(ubi256_ctx_t* ctx, const void* block){
	threefish256_ctx_t tfctx;
	ctx->tweak.v64[0] += UBI256_BLOCKSIZE_B;
	threefish256_init(ctx->g, ctx->tweak.v8, &tfctx);
	memcpy(ctx->g, block, UBI256_BLOCKSIZE_B);
	threefish256_enc(ctx->g, &tfctx);
	memxor(ctx->g, block, UBI256_BLOCKSIZE_B);
	ctx->tweak.v8[15] &= (uint8_t)~0x40;
} 


void ubi256_lastBlock(ubi256_ctx_t* ctx, const void* block, uint16_t length_b){
	threefish256_ctx_t tfctx;
	while(length_b>UBI256_BLOCKSIZE){
		ubi256_nextBlock(ctx, block);
		block = (uint8_t*)block + UBI256_BLOCKSIZE_B;
		length_b -= UBI256_BLOCKSIZE;
	}
	ctx->tweak.v8[15] |= 0x80;
	ctx->tweak.v64[0] += (length_b+7)/8;
	if(length_b & 0x07){
		ctx->tweak.v8[14] |= 0x80;
	}
	threefish256_init(ctx->g, ctx->tweak.v8, &tfctx);
	memset(ctx->g, 0, UBI256_BLOCKSIZE_B);
	memcpy(ctx->g, block, (length_b+7)/8);
	if(length_b & 0x07){
		ctx->g[((length_b+7)/8)-1] |= 0x80>>(length_b&7);
		ctx->g[((length_b+7)/8)-1] &= ~((0x80>>(length_b&7))-1);
	}
	threefish256_enc(ctx->g, &tfctx);
	memxor(ctx->g, block, (length_b+7)/8);
	if(length_b & 0x07){
		ctx->g[((length_b+7)/8)-1] ^= 0x80>>(length_b&7);
	}

} 

void ubi256_ctx2hash(void* dest, const ubi256_ctx_t* ctx){
	memcpy(dest, ctx->g, UBI256_BLOCKSIZE_B);
}

