/* bcal-cfb_byte.c */
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
#include "bcal-cfb_byte.h"
#include "bcal-basic.h"
#include "memxor.h"


uint8_t bcal_cfb_B_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, uint16_t size_b, bcal_cfb_B_ctx_t* ctx){
	ctx->desc = (bcdesc_t*)desc;
	ctx->blocksize_B = (bcal_cipher_getBlocksize_b(desc)+7)/8;
	ctx->in_block=malloc(ctx->blocksize_B);
	if(ctx->in_block==NULL){
			return 0x11;
	}
	if(size_b&7){
		return 0x12;
	}
	ctx->size_B = size_b/8;
	return bcal_cipher_init(desc, key, keysize_b, &(ctx->cctx));
}

void bcal_cfb_B_free(bcal_cfb_B_ctx_t* ctx){
	free(ctx->in_block);
	bcal_cipher_free(&(ctx->cctx));
}

void bcal_cfb_B_loadIV(const void* iv, bcal_cfb_B_ctx_t* ctx){
	if(iv){
		memcpy(ctx->in_block, iv, ctx->blocksize_B);
	}
}

void bcal_cfb_B_encNext(void* block, bcal_cfb_B_ctx_t* ctx){
	uint8_t tmp[ctx->blocksize_B];
	memcpy(tmp, ctx->in_block, ctx->blocksize_B);
	bcal_cipher_enc(tmp, &(ctx->cctx));
	memxor(block, tmp, ctx->size_B);
	memmove(ctx->in_block, ctx->in_block+ctx->size_B, ctx->blocksize_B - ctx->size_B);
	memcpy(ctx->in_block+ctx->blocksize_B-ctx->size_B, block, ctx->size_B);
}

void bcal_cfb_B_decNext(void* block, bcal_cfb_B_ctx_t* ctx){
	uint8_t tmp[ctx->blocksize_B];
	uint8_t xblock[ctx->size_B];
	memcpy(xblock, block, ctx->size_B);
	memcpy(tmp, ctx->in_block, ctx->blocksize_B);
	bcal_cipher_enc(tmp, &(ctx->cctx));
	memxor(block, tmp, ctx->size_B);
	memmove(ctx->in_block, ctx->in_block+ctx->size_B, ctx->blocksize_B - ctx->size_B);
	memcpy(ctx->in_block+ctx->blocksize_B-ctx->size_B, xblock, ctx->size_B);
}

void bcal_cfb_B_encMsg(const void* iv, void* msg, uint16_t msg_blocks, bcal_cfb_B_ctx_t* ctx){
	bcal_cfb_B_loadIV(iv, ctx);
	while(msg_blocks--){
		bcal_cfb_B_encNext(msg, ctx);
		msg = (uint8_t*)msg+ctx->size_B;
	}
}

void bcal_cfb_B_decMsg(const void* iv, void* msg, uint16_t msg_blocks, bcal_cfb_B_ctx_t* ctx){
	bcal_cfb_B_loadIV(iv, ctx);
		while(msg_blocks--){
			bcal_cfb_B_decNext(msg, ctx);
			msg = (uint8_t*)msg+ctx->size_B;
		}
}

