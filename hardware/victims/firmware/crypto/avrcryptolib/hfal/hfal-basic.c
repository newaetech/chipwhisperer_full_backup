/* hfal-basic.c */
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

#include <avr/pgmspace.h>
#include "hashfunction_descriptor.h"
#include "hfal-basic.h"
#include <stdlib.h>

uint8_t hfal_hash_init(const hfdesc_t* hash_descriptor, hfgen_ctx_t* ctx){
	hf_init_fpt f;
	uint16_t tmp;
	ctx->desc_ptr = (hfdesc_t*)hash_descriptor;
	tmp = pgm_read_word(&(hash_descriptor->ctxsize_B));
	if(!(ctx->ctx=malloc(tmp)))
		return 3;
	f= (hf_init_fpt)pgm_read_word(&(hash_descriptor->init));
	f(ctx->ctx);
	return 0;
}

void hfal_hash_nextBlock(hfgen_ctx_t* ctx, const void* block){
	hf_nextBlock_fpt f;
	hfdesc_t* x=(ctx->desc_ptr);
	f =(hf_nextBlock_fpt)pgm_read_word(&(x->nextBlock));
	f(ctx->ctx, block);
}

void hfal_hash_lastBlock(hfgen_ctx_t* ctx, const void* block, uint16_t length_b){
	hf_lastBlock_fpt f;
	hfdesc_t* x=ctx->desc_ptr;
	f =(hf_lastBlock_fpt)pgm_read_word(&(x->lastBlock));
	f(ctx->ctx, block, length_b);
}

void hfal_hash_ctx2hash(void* dest, hfgen_ctx_t* ctx){
	hf_ctx2hash_fpt f;
	hfdesc_t* x=ctx->desc_ptr;
	f =(hf_ctx2hash_fpt)pgm_read_word(&(x->ctx2hash));
	f(dest, ctx->ctx);
}

void hfal_hash_free(hfgen_ctx_t* ctx){
	hf_free_fpt f;
	hfdesc_t* x=ctx->desc_ptr;
	f =(hf_free_fpt)pgm_read_word(&(x->free));
	if(f)
		f(ctx->ctx);
	free(ctx->ctx);
}

void hfal_hash_mem(const hfdesc_t* hash_descriptor, void* dest, const void* msg, uint32_t length_b){
	void_fpt f;
	f = (void_fpt)pgm_read_word(&(hash_descriptor->mem));
	if(f){
		((hf_mem_fpt)f)(dest, msg, length_b);
	}else{
		uint16_t bs,bsb;
		uint8_t ctx[pgm_read_word(&(hash_descriptor->ctxsize_B))];
		f=(void_fpt)(pgm_read_word(&(hash_descriptor->init)));
		((hf_init_fpt)f)(ctx);
		bs=pgm_read_word(&(hash_descriptor->blocksize_b));
		bsb=bs/8;
		f=(void_fpt)(pgm_read_word(&(hash_descriptor->nextBlock)));
		while(length_b>bs){
			((hf_nextBlock_fpt)f)(ctx, msg);
			length_b -= bs;
			msg = (uint8_t*)msg + bsb;
		}
		f=(void_fpt)(pgm_read_word(&(hash_descriptor->lastBlock)));
		((hf_lastBlock_fpt)f)(ctx, msg, length_b);
		f=(void_fpt)(pgm_read_word(&(hash_descriptor->ctx2hash)));
		((hf_ctx2hash_fpt)f)(dest, ctx);
	}
}

uint16_t hfal_hash_getBlocksize(const hfdesc_t* hash_descriptor){
	uint16_t ret;
	ret = pgm_read_word(&(hash_descriptor->blocksize_b));
	return ret;
}

uint16_t hfal_hash_getHashsize(const hfdesc_t* hash_descriptor){
	uint16_t ret;
	ret = pgm_read_word(&(hash_descriptor->hashsize_b));
	return ret;
}

uint16_t hfal_hash_getCtxsize_B(const hfdesc_t* hash_descriptor){
	uint16_t ret;
	ret = pgm_read_word(&(hash_descriptor->ctxsize_B));
	return ret;
}
