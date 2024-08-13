/* scal-basic.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2011  Daniel Otte (daniel.otte@rub.de)

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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "streamcipher_descriptor.h"
#include "keysize_descriptor.h"

uint8_t scal_cipher_init(const scdesc_t* cipher_descriptor,
                         const void* key, uint16_t keysize_b,
                         const void* iv,  uint16_t ivsize_b, scgen_ctx_t* ctx){
	ctx->buffer = NULL;
	ctx->ctx = NULL;

	if(!is_valid_keysize_P((PGM_VOID_P)pgm_read_word(&(cipher_descriptor->valid_keysize_desc)), keysize_b)){
		return 1;
	}
	if(!is_valid_keysize_P((PGM_VOID_P)pgm_read_word(&(cipher_descriptor->valid_ivsize_desc)), ivsize_b)){
		return 2;
	}
	uint8_t flags;
	void_fpt init_fpt;
	flags = pgm_read_byte(&(cipher_descriptor->flags));
	ctx->desc_ptr = cipher_descriptor;
	ctx->keysize = keysize_b;
	ctx->ivsize  = ivsize_b;
	ctx->ctx = malloc(pgm_read_word(&(cipher_descriptor->ctxsize_B)));
	if(ctx->ctx==NULL){
		return 3;
	}
	init_fpt = (void_fpt)pgm_read_word(&(cipher_descriptor->init));
	switch(flags&SC_INIT_TYPE){
		case SC_INIT_TYPE_1:
			((sc_init1_fpt)init_fpt)(key, ctx->ctx);
			break;
		case SC_INIT_TYPE_2:
			((sc_init2_fpt)init_fpt)(key, iv, ctx->ctx);
			break;
		case SC_INIT_TYPE_3:
			((sc_init3_fpt)init_fpt)(key, keysize_b, ctx->ctx);
			break;
		case SC_INIT_TYPE_4:
			((sc_init4_fpt)init_fpt)(key, keysize_b, iv, ctx->ctx);
			break;
		case SC_INIT_TYPE_5:
			((sc_init5_fpt)init_fpt)(key, keysize_b, iv, ivsize_b, ctx->ctx);
			break;
		default:
			return 4;
	}
	uint16_t blocksize_b;
	blocksize_b = pgm_read_word(&(cipher_descriptor->gensize_b));
	if(blocksize_b>8){
		ctx->buffer=malloc((blocksize_b+7)/8);
		if(ctx->buffer==NULL){
			return 5;
		}
		ctx->index=0;
	}
	return 0;
}


void scal_cipher_free(scgen_ctx_t* ctx){
	if(ctx->buffer){
		free(ctx->buffer);
	}
	if(ctx->ctx){
		free(ctx->ctx);
	}
}

uint8_t scal_cipher_gen_byte(scgen_ctx_t* ctx){
	uint8_t flags;
	uint16_t blocksize_b;
	void_fpt gen_fpt;
	flags = pgm_read_byte(&(ctx->desc_ptr->flags));
	blocksize_b = pgm_read_word(&(ctx->desc_ptr->gensize_b));
	gen_fpt = (void_fpt)(pgm_read_word(&(ctx->desc_ptr->gen.genvoid)));

	if(blocksize_b==8){
		if((flags&SC_GEN_TYPE)==SC_GEN_TYPE_1){
			return ((sc_gen1_fpt)gen_fpt)(ctx->ctx);
		}else{
			uint8_t r;
			((sc_gen2_fpt)gen_fpt)(&r, ctx->ctx);
			return r;
		}
	}
	if(blocksize_b<8){
		uint8_t r=0;
		uint8_t fill=0;
		do{
			r |= ((((sc_gen1_fpt)gen_fpt)(ctx->ctx))&(0xff<<(8-blocksize_b)))>>fill;
			fill += blocksize_b;
		}while(fill<8);
		return r;
	}else{
		uint8_t r;
		if(ctx->index==0){
			((sc_gen2_fpt)gen_fpt)(ctx->buffer, ctx->ctx);
			ctx->index = blocksize_b;
		}
		r=ctx->buffer[(blocksize_b-ctx->index)/8];
		ctx->index -= 8;
		return r;
	}
}

void scal_cipher_gen_block(void* block, scgen_ctx_t* ctx){
	uint8_t flags;
	void_fpt gen_fpt;
	flags = pgm_read_byte(&(ctx->desc_ptr->flags));
	gen_fpt = (void_fpt)pgm_read_word(&(ctx->desc_ptr->gen));
	if((flags&SC_GEN_TYPE)==SC_GEN_TYPE_1){
		*((uint8_t*)block) = ((sc_gen1_fpt)gen_fpt)(ctx->ctx);
	}else{
		((sc_gen2_fpt)gen_fpt)(block, ctx->ctx);
	}
}

void scal_cipher_gen_fillblock(void* block, uint16_t blocksize_B, scgen_ctx_t* ctx){
	while(blocksize_B){
		*((uint8_t*)block) = scal_cipher_gen_byte(ctx);
		block = (uint8_t*)block + 1;
		blocksize_B -= 1;
	}
}

uint16_t scal_cipher_getBlocksize_b(const scdesc_t* desc){
	uint16_t blocksize_b;
	blocksize_b = pgm_read_word(&(desc->gensize_b));
	return blocksize_b;
}

PGM_VOID_P scal_cipher_getKeysizeDesc(const scdesc_t* desc){
	return (PGM_VOID_P)pgm_read_word(&(desc->valid_keysize_desc));
}

PGM_VOID_P scal_cipher_getIVsizeDesc(const scdesc_t* desc){
	return (PGM_VOID_P)pgm_read_word(&(desc->valid_ivsize_desc));
}


