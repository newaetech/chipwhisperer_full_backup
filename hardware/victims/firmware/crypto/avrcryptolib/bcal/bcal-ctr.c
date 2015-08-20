/* bcal-ctr.c */
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
#include "bcal-ctr.h"
#include "memxor.h"

static void increment_be(void* block, uint8_t size_B){
	uint16_t c=1;
	do{
		--size_B;
		c += ((uint8_t*)block)[size_B];
		((uint8_t*)block)[size_B] = (uint8_t)c;
		c>>=8;
	}while(size_B);
}

uint8_t bcal_ctr_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, inc_fp_t inc_func, bcal_ctr_ctx_t* ctx){
	ctx->desc = (bcdesc_t*)desc;
	if(inc_func){
		ctx->inc_func = inc_func;
	}else{
		ctx->inc_func = increment_be;
	}
	ctx->blocksize_B = (bcal_cipher_getBlocksize_b(desc)+7)/8;
	ctx->in_block=malloc(ctx->blocksize_B);
	if(ctx->in_block==NULL){
			return 0x11;
	}
	return bcal_cipher_init(desc, key, keysize_b, &(ctx->cctx));
}

void bcal_ctr_free(bcal_ctr_ctx_t* ctx){
	free(ctx->in_block);
	bcal_cipher_free(&(ctx->cctx));
}

void bcal_ctr_loadIV(const void* iv, bcal_ctr_ctx_t* ctx){
	if(iv){
		memcpy(ctx->in_block, iv, ctx->blocksize_B);
	}
}

void bcal_ctr_encNext(void* block, bcal_ctr_ctx_t* ctx){
	uint8_t tmp[ctx->blocksize_B];
	memcpy(tmp, ctx->in_block, ctx->blocksize_B);
	bcal_cipher_enc(tmp, &(ctx->cctx));
	memxor(block, tmp, ctx->blocksize_B);
	ctx->inc_func(ctx->in_block, ctx->blocksize_B);
}

void bcal_ctr_decNext(void* block, bcal_ctr_ctx_t* ctx){
	bcal_ctr_encNext(block, ctx);
}

void bcal_ctr_encMsg(const void* iv, void* msg, uint32_t msg_len_b, bcal_ctr_ctx_t* ctx){
	bcal_ctr_loadIV(iv, ctx);
	uint16_t blocksize_b;
	blocksize_b = ctx->blocksize_B*8;
	while(msg_len_b>blocksize_b){
		bcal_ctr_encNext(msg, ctx);
		msg_len_b -= blocksize_b;
		msg = (uint8_t*)msg + ctx->blocksize_B;
	}
	uint8_t tmp[ctx->blocksize_B];
	memcpy(tmp, ctx->in_block, ctx->blocksize_B);
	bcal_cipher_enc(tmp, &(ctx->cctx));
	ctx->inc_func(ctx->in_block, ctx->blocksize_B);
	tmp[msg_len_b/8] = 0xff00>>(msg_len_b&7);
	memxor(msg, tmp, (msg_len_b+7)/8);
}

void bcal_ctr_decMsg(const void* iv, void* msg, uint32_t msg_len_b, bcal_ctr_ctx_t* ctx){
	bcal_ctr_encMsg(iv, msg, msg_len_b, ctx);
}

