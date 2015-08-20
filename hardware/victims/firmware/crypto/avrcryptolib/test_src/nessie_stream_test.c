/* nessie_stream_test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * a suit for running the nessie-tests for streamciphers
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_stream_test.h"
#include "nessie_common.h"
#include "memxor.h"

nessie_stream_ctx_t nessie_stream_ctx;


#define BLOCKSIZE_B 64


static 
void nessie_gen_block(void* ctx, uint8_t* block){
	uint16_t i;
	for(i=0; i<BLOCKSIZE_B; ++i){
		block[i] = nessie_stream_ctx.cipher_enc(ctx);
	}
}

static
void nessie_stream_enc(uint8_t* key){
	uint8_t ctx[nessie_stream_ctx.ctx_size_B];
	uint8_t buffer[BLOCKSIZE_B];
	uint8_t xorbuffer[BLOCKSIZE_B];
	uint8_t i;
	
	memset(xorbuffer, 0, BLOCKSIZE_B);
	
	nessie_print_item("key", key, (nessie_stream_ctx.keysize_b+7)/8);
	
	nessie_stream_ctx.cipher_genctx(key, nessie_stream_ctx.keysize_b, ctx);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[0..63]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((192-0)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[192..255]", buffer, BLOCKSIZE_B);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[256..319]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((448-256)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[448..511]", buffer, BLOCKSIZE_B);
	
	nessie_print_item("stream[0..511]xored", xorbuffer, BLOCKSIZE_B);
	
}


static
void nessie_stream_enc_large(uint8_t* key){
	uint8_t ctx[nessie_stream_ctx.ctx_size_B];
	uint8_t buffer[BLOCKSIZE_B];
	uint8_t xorbuffer[BLOCKSIZE_B];
	uint32_t i;
	
	memset(xorbuffer, 0, BLOCKSIZE_B);
	
	nessie_print_item("key", key, (nessie_stream_ctx.keysize_b+7)/8);
	
	nessie_stream_ctx.cipher_genctx(key, nessie_stream_ctx.keysize_b, ctx);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[0..63]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((65472-0)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
		NESSIE_SEND_ALIVE_A(i);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[65472..65535]", buffer, BLOCKSIZE_B);
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[65536..65599]", buffer, BLOCKSIZE_B);
	
	for(i=0; i<((131008-65536)/BLOCKSIZE_B-1); ++i){
		nessie_gen_block(ctx, buffer);
		memxor(xorbuffer, buffer, BLOCKSIZE_B);
		NESSIE_SEND_ALIVE_A(i);
	}
	
	nessie_gen_block(ctx, buffer);
	memxor(xorbuffer, buffer, BLOCKSIZE_B);
	nessie_print_item("stream[131008..131071]", buffer, BLOCKSIZE_B);
	
	nessie_print_item("stream[0..131071]xored", xorbuffer, BLOCKSIZE_B);
	
}

void nessie_stream_run(void){
	uint16_t i;
	uint8_t set;
	uint8_t key[(nessie_stream_ctx.keysize_b+7)/8];
	
	nessie_print_header(nessie_stream_ctx.name, nessie_stream_ctx.keysize_b,
	                    0, 0, 0, nessie_stream_ctx.ivsize_b);
	/* test set 1 */
	set=1;
	nessie_print_setheader(set);
	for(i=0; i<nessie_stream_ctx.keysize_b; ++i){
		nessie_print_set_vector(set, i);
		memset(key, 0, (nessie_stream_ctx.keysize_b+7)/8);
		key[i/8] |= 0x80>>(i%8);
		nessie_stream_enc(key);
	}
	/* test set 2 */
	set=2;
	nessie_print_setheader(set);
	for(i=0; i<256; ++i){
		nessie_print_set_vector(set, i);
		memset(key, i, (nessie_stream_ctx.keysize_b+7)/8);
		nessie_stream_enc(key);
	}
	/* test set 3 */
	set=3;
	nessie_print_setheader(set);
	for(i=0; i<256; ++i){
		uint8_t j;
		nessie_print_set_vector(set, i);
		for(j=0; j<(nessie_stream_ctx.keysize_b+7)/8; ++j){
			key[j]=(i+j)&0xff;
		}
		nessie_stream_enc(key);
	}
	/* test set 4 */
	set=4;
	nessie_print_setheader(set);
	for(i=0; i<4; ++i){
		uint8_t j;
		nessie_print_set_vector(set, i);
		for(j=0; j<(nessie_stream_ctx.keysize_b+7)/8; ++j){
			key[j]=(i*5+j*0x53)&0xff;
		}
		nessie_stream_enc_large(key);
	}

	nessie_print_footer();
}
