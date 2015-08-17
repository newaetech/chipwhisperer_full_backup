/* bcal-cfb_bit.c */
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
#include "bcal-cfb_bit.h"
#include "bcal-basic.h"

static uint8_t read_bit(void* block, uint32_t index){
	uint8_t r;
	r=((uint8_t*)block)[index/8];
	r=(r&(0x80>>(index&7)))?0xff:0x00;
	return r;
}

static void write_bit(void* block, uint32_t index, uint8_t value){
	if(value){
		/* set bit */
		((uint8_t*)block)[index/8] |= 0x80>>(index&7);
	}else{
		/* clear bit */
		((uint8_t*)block)[index/8] &= ~(0x80>>(index&7));
	}
}

uint8_t bcal_cfb_b_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, uint16_t size_b, bcal_cfb_b_ctx_t* ctx){
	ctx->desc = (bcdesc_t*)desc;
	ctx->blocksize_B = (bcal_cipher_getBlocksize_b(desc)+7)/8;
	ctx->in_block=malloc(ctx->blocksize_B);
	if(ctx->in_block==NULL){
		return 0x11;
	}
	if(size_b>bcal_cipher_getBlocksize_b(desc)){
		return 0x12;
	}
	ctx->size_b = size_b;
	return bcal_cipher_init(desc, key, keysize_b, &(ctx->cctx));
}

void bcal_cfb_b_free(bcal_cfb_b_ctx_t* ctx){
	free(ctx->in_block);
	bcal_cipher_free(&(ctx->cctx));
}

void bcal_cfb_b_loadIV(const void* iv, bcal_cfb_b_ctx_t* ctx){
	if(iv){
		memcpy(ctx->in_block, iv, ctx->blocksize_B);
	}
}

void bcal_cfb_b_encNext(void* block, uint8_t offset, bcal_cfb_b_ctx_t* ctx){
	uint8_t tmp[ctx->blocksize_B];
	offset &= 7;
	memcpy(tmp, ctx->in_block, ctx->blocksize_B);
	bcal_cipher_enc(tmp, &(ctx->cctx));
	uint16_t i,j;
	uint8_t a;
	for(i=0; i<ctx->blocksize_B*8-ctx->size_b; ++i){
		a = read_bit(ctx->in_block, i+ctx->size_b);
		write_bit(ctx->in_block, i, a);
	}
	for(j=offset,i=0; i<ctx->size_b; ++i, ++j){
		a = read_bit(tmp, i) ^ read_bit(block, j);
		write_bit(ctx->in_block, ctx->blocksize_B*8-ctx->size_b+i, a);
		write_bit(block, j, a);
	}
}

void bcal_cfb_b_decNext(void* block, uint8_t offset, bcal_cfb_b_ctx_t* ctx){
	uint8_t tmp[ctx->blocksize_B];
	offset &= 7;
	memcpy(tmp, ctx->in_block, ctx->blocksize_B);
	bcal_cipher_enc(tmp, &(ctx->cctx));
	uint16_t i,j;
	uint8_t a,b;
	for(i=0; i<ctx->blocksize_B*8-ctx->size_b; ++i){
		a = read_bit(ctx->in_block, i+ctx->size_b);
		write_bit(ctx->in_block, i, a);
	}
	for(j=offset,i=0; i<ctx->size_b; ++i, ++j){
		a = read_bit(tmp, i);
		b = read_bit(block, j);
		a ^= b;
		write_bit(ctx->in_block, ctx->blocksize_B*8-ctx->size_b+i, b);
		write_bit(block, j, a);
	}
}

void bcal_cfb_b_encMsg(const void* iv, void* msg, uint8_t offset, uint32_t msg_blocks, bcal_cfb_b_ctx_t* ctx){
	bcal_cfb_b_loadIV(iv, ctx);
	uint32_t addr;
	addr = ((uint16_t)msg)*8+offset;
	while(msg_blocks--){
		msg = (void*)((uint16_t)(addr/8));
		offset = addr&7;
		bcal_cfb_b_encNext(msg, offset, ctx);
		addr += ctx->size_b;
	}
}

void bcal_cfb_b_decMsg(const void* iv, void* msg, uint8_t offset, uint32_t msg_blocks, bcal_cfb_b_ctx_t* ctx){
	bcal_cfb_b_loadIV(iv, ctx);
	uint32_t addr;
	addr = ((uint16_t)msg)*8+offset;
	while(msg_blocks--){
		msg = (void*)((uint16_t)(addr/8));
		offset = addr&7;
		bcal_cfb_b_decNext(msg, offset, ctx);
		addr += ctx->size_b;
	}
}
