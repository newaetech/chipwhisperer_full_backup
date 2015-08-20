/* bca-eax.c */
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
#include "bcal-basic.h"
#include "blockcipher_descriptor.h"
#include "bcal-cmac.h"
#include "bcal-ctr.h"
#include "bcal-eax.h"
#include "memxor.h"

uint8_t bcal_eax_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, bcal_eax_ctx_t* ctx){
	uint8_t r;
	ctx->blocksize_B = (bcal_cipher_getBlocksize_b(desc)+7)/8;
	ctx->nonce = malloc(ctx->blocksize_B);
	if(ctx->nonce==NULL){
		return 0x81;
	}
	r = bcal_cmac_init(desc, key, keysize_b, &(ctx->ctag));
	if(r){
		return r;
	}
	r = bcal_cmac_init(desc, key, keysize_b, &(ctx->htag));
	if(r){
		return (r|0x10);
	}
	r = bcal_cmac_init(desc, key, keysize_b, &(ctx->ntag));
	if(r){
		return (r|0x20);
	}
	r = bcal_ctr_init(desc, key, keysize_b, NULL, &(ctx->cipher));
	if(r){
		return (r|0x30);
	}
	ctx->header_set=0;
	uint8_t tmp[ctx->blocksize_B];
	memset(tmp, 0, ctx->blocksize_B);
	bcal_cmac_nextBlock(&(ctx->ntag), tmp);
	tmp[ctx->blocksize_B-1]=1;
	bcal_cmac_nextBlock(&(ctx->htag), tmp);
	tmp[ctx->blocksize_B-1]=2;
	bcal_cmac_nextBlock(&(ctx->ctag), tmp);
	return 0;
}

void bcal_eax_free(bcal_eax_ctx_t* ctx){
	bcal_ctr_free(&(ctx->cipher));
	bcal_cmac_free(&(ctx->ctag));
	bcal_cmac_free(&(ctx->htag));
	bcal_cmac_free(&(ctx->ntag));
	free(ctx->nonce);
}

void bcal_eax_loadNonce(const void* nonce, uint16_t length_b, bcal_eax_ctx_t* ctx){
	bcal_cmac_lastBlock(&(ctx->ntag), nonce, length_b);
	bcal_cmac_ctx2mac(ctx->nonce, ctx->blocksize_B*8, &(ctx->ntag));
	bcal_ctr_loadIV(ctx->nonce, &(ctx->cipher));
}

void bcal_eax_addNextHeader(const void* header, bcal_eax_ctx_t* ctx){
	bcal_cmac_nextBlock(&(ctx->htag), header);
}

void bcal_eax_addLastHeader(const void* header, uint16_t length_b, bcal_eax_ctx_t* ctx){
	bcal_cmac_lastBlock(&(ctx->htag), header, length_b);
	ctx->header_set = 1;
}

void bcal_eax_encNextBlock(void* block, bcal_eax_ctx_t* ctx){
	bcal_ctr_encNext(block, &(ctx->cipher));
	bcal_cmac_nextBlock(&(ctx->ctag), block);
}

void bcal_eax_encLastBlock(void* block, uint16_t length_b, bcal_eax_ctx_t* ctx){
	bcal_ctr_encMsg(NULL, block, length_b, &(ctx->cipher));
	bcal_cmac_lastBlock(&(ctx->ctag), block, length_b);
}

void bcal_eax_decNextBlock(void* block, bcal_eax_ctx_t* ctx){
	bcal_cmac_nextBlock(&(ctx->ctag), block);
	bcal_ctr_decNext(block, &(ctx->cipher));
}

void bcal_eax_decLastBlock(void* block, uint16_t length_b, bcal_eax_ctx_t* ctx){
	bcal_cmac_lastBlock(&(ctx->ctag), block, length_b);
	bcal_ctr_decMsg(NULL, block, length_b, &(ctx->cipher));
}

void bcal_eax_ctx2tag(void* dest, uint16_t length_b, bcal_eax_ctx_t* ctx){
	uint8_t tmp[ctx->blocksize_B];
	if(ctx->header_set==0){
		bcal_cmac_lastBlock(&(ctx->htag), NULL, 0);
	}

	bcal_cmac_ctx2mac(tmp, ctx->blocksize_B*8, &(ctx->htag));
	memxor(ctx->nonce, tmp, ctx->blocksize_B);

	bcal_cmac_ctx2mac(tmp, ctx->blocksize_B*8, &(ctx->ctag));
	memxor(ctx->nonce, tmp, ctx->blocksize_B);
	memcpy(dest, ctx->nonce, (length_b+7)/8);
}

