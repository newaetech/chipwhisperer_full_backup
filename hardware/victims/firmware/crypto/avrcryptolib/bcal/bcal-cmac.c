/* bcal-omac.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
#include <string.h>
#include "bcal-basic.h"
#include "bcal-cmac.h"
#include "memxor.h"


static uint8_t left_shift_be_block(void* block, uint8_t blocksize_B){
	uint8_t c1=0, c2;
	do{
		--blocksize_B;
		c2 = (((uint8_t*)block)[blocksize_B])>>7;
		(((uint8_t*)block)[blocksize_B]) <<= 1;
		(((uint8_t*)block)[blocksize_B]) |= c1;
		c1 = c2;
	}while(blocksize_B);
	return c1;
}

static const uint8_t const_128 = 0x87;
static const uint8_t const_64  = 0x1b;

uint8_t bcal_cmac_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, bcal_cmac_ctx_t* ctx){
	uint8_t r;
	ctx->desc = (bcdesc_t*)desc;
	ctx->blocksize_B = bcal_cipher_getBlocksize_b(desc)/8;
	if (ctx->blocksize_B!=128/8 && ctx->blocksize_B!=64/8){
		return 0x13;
	}
	ctx->accu = malloc(ctx->blocksize_B);
	if(ctx->accu==NULL){
		return 0x14;
	}
	ctx->k1 = malloc(ctx->blocksize_B);
	if(ctx->k1==NULL){
		return 0x15;
	}
	ctx->k2 = malloc(ctx->blocksize_B);
	if(ctx->k2==NULL){
		return 0x16;
	}
	ctx->lastblock = malloc(ctx->blocksize_B);
	if(ctx->lastblock==NULL){
		return 0x17;
	}
	r = bcal_cipher_init(desc, key, keysize_b, &(ctx->cctx));
	if(r){
		return r;
	}
	if(ctx->blocksize_B==128/8){
		r = const_128;
	}else{
		r = const_64;
	}
	/* subkey computation */
	memset(ctx->accu, 0x00, ctx->blocksize_B);
	memset(ctx->k1, 0x00, ctx->blocksize_B);
	bcal_cipher_enc(ctx->k1, &(ctx->cctx));
	if(left_shift_be_block(ctx->k1, ctx->blocksize_B)){
		ctx->k1[ctx->blocksize_B-1] ^= r;
	}
	memcpy(ctx->k2, ctx->k1, ctx->blocksize_B);
	if(left_shift_be_block(ctx->k2, ctx->blocksize_B)){
		ctx->k2[ctx->blocksize_B-1] ^= r;
	}
	ctx->last_set=0;
	return 0;
}

void bcal_cmac_free(bcal_cmac_ctx_t* ctx){
	free(ctx->accu);
	free(ctx->k1);
	free(ctx->k2);
	bcal_cipher_free(&(ctx->cctx));
}

void bcal_cmac_nextBlock (bcal_cmac_ctx_t* ctx, const void* block){
	if(ctx->last_set){
		memxor(ctx->accu, ctx->lastblock, ctx->blocksize_B);
		bcal_cipher_enc(ctx->accu, &(ctx->cctx));
	}
	memcpy(ctx->lastblock, block, ctx->blocksize_B);
	ctx->last_set=1;
}


void bcal_cmac_lastBlock(bcal_cmac_ctx_t* ctx, const void* block, uint16_t length_b){
	uint16_t blocksize_b;
	blocksize_b = ctx->blocksize_B*8;
	while(length_b>=blocksize_b){
		bcal_cmac_nextBlock(ctx, block);
		block = (uint8_t*)block + ctx->blocksize_B;
		length_b -= blocksize_b;
	}
	if(ctx->last_set==0){
		memxor(ctx->accu, block, (length_b+7)/8);
		memxor(ctx->accu, ctx->k2, ctx->blocksize_B);
		ctx->accu[length_b/8] ^= 0x80>>(length_b&7);
	}else{
		if(length_b==0){
			memxor(ctx->accu, ctx->lastblock, ctx->blocksize_B);
			memxor(ctx->accu, ctx->k1, ctx->blocksize_B);
		}else{
			memxor(ctx->accu, ctx->lastblock, ctx->blocksize_B);
			bcal_cipher_enc(ctx->accu, &(ctx->cctx));
			memxor(ctx->accu, block, (length_b+7)/8);
			memxor(ctx->accu, ctx->k2, ctx->blocksize_B);
			ctx->accu[length_b/8] ^= 0x80>>(length_b&7);
		}
	}
	bcal_cipher_enc(ctx->accu, &(ctx->cctx));
}

void bcal_cmac_ctx2mac(void* dest, uint16_t length_b, const bcal_cmac_ctx_t* ctx){
	memcpy(dest, ctx->accu, length_b/8);
	if(length_b&7){
		((uint8_t*)dest)[length_b/8] &= 0xff>>(length_b&7);
		((uint8_t*)dest)[length_b/8] |= (0xff00>>(length_b&7))&(ctx->accu[length_b/8]);
	}
}

void bcal_cmac(void* dest, uint16_t out_length_b, const void* block, uint32_t length_b, bcal_cmac_ctx_t* ctx){
	uint16_t blocksize_b;
	blocksize_b = ctx->blocksize_B*8;
	while(length_b>blocksize_b){
		bcal_cmac_nextBlock(ctx, block);
		block = (uint8_t*)block + ctx->blocksize_B;
		length_b -= blocksize_b;
	}
	bcal_cmac_lastBlock(ctx, block, length_b);
	bcal_cmac_ctx2mac(dest, out_length_b, ctx);
}
