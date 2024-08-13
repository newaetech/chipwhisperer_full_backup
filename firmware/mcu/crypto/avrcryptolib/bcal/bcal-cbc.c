/* bcal-cbc.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010  Daniel Otte (daniel.otte@rub.de)

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
#include "bcal-cbc.h"
#include "bcal-basic.h"
#include "memxor.h"

uint8_t bcal_cbc_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, bcal_cbc_ctx_t* ctx){
	ctx->desc = (bcdesc_t*)desc;
	ctx->blocksize_B = (bcal_cipher_getBlocksize_b(desc)+7)/8;
	ctx->prev_block = malloc(ctx->blocksize_B);

	if(ctx->prev_block==NULL){
		return 0x11;
	}
	return bcal_cipher_init(desc, key, keysize_b, &(ctx->cctx));
}

void bcal_cbc_free(bcal_cbc_ctx_t* ctx){
	bcal_cipher_free(&(ctx->cctx));
	free(ctx->prev_block);
}


void bcal_cbc_loadIV(const void* iv, bcal_cbc_ctx_t* ctx){
	if(iv){
		memcpy(ctx->prev_block, iv, ctx->blocksize_B);
	}
}

void bcal_cbc_encNext(void* block, bcal_cbc_ctx_t* ctx){
	memxor(block, ctx->prev_block, ctx->blocksize_B);
	bcal_cipher_enc(block, &(ctx->cctx));
	memcpy(ctx->prev_block, block, ctx->blocksize_B);
}

void bcal_cbc_decNext(void* block, bcal_cbc_ctx_t* ctx){
	uint8_t tmp[ctx->blocksize_B];
	memcpy(tmp, block, ctx->blocksize_B);
	bcal_cipher_dec(block, &(ctx->cctx));
	memxor(block, ctx->prev_block, ctx->blocksize_B);
	memcpy(ctx->prev_block, tmp, ctx->blocksize_B);
}
void bcal_cbc_decRand(void* block, const void* prev_block, bcal_cbc_ctx_t* ctx){
	bcal_cipher_dec(block, &(ctx->cctx));
	memxor(block, prev_block, ctx->blocksize_B);
}

void bcal_cbc_encMsg(const void* iv, void* msg, uint16_t msg_blocks, bcal_cbc_ctx_t* ctx){
	bcal_cbc_loadIV(iv, ctx);
	while(msg_blocks--){
		bcal_cbc_encNext(msg, ctx);
		msg = (uint8_t*)msg + ctx->blocksize_B;
	}
}

void bcal_cbc_decMsg(const void* iv, void* msg, uint16_t msg_blocks, bcal_cbc_ctx_t* ctx){
	msg=(uint8_t*)msg + (msg_blocks-1)*ctx->blocksize_B;
	while(msg_blocks > 1){
		bcal_cbc_decRand(msg, (uint8_t*)msg-ctx->blocksize_B, ctx);
		msg_blocks -= 1;
		msg=(uint8_t*)msg-ctx->blocksize_B;
	}
	bcal_cbc_decRand(msg, iv, ctx);
}

