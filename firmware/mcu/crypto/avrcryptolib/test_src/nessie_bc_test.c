/* nessie_bc_test.c */
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
 * a suit for running the nessie-tests for blockciphers
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_bc_test.h"
#include "nessie_common.h"

nessie_bc_ctx_t nessie_bc_ctx;

void nessie_bc_init(void){
	memset(&nessie_bc_ctx, 0, sizeof(nessie_bc_ctx_t));	
}
static
void nessie_bc_free(void* ctx){
	if(nessie_bc_ctx.cipher_free)
		nessie_bc_ctx.cipher_free(ctx);
}

void nessie_bc_enc(uint8_t* key, uint8_t* pt){
	uint8_t ctx[nessie_bc_ctx.ctx_size_B];
	uint8_t buffer[nessie_bc_ctx.blocksize_B];
	uint16_t i;
	
	/* single test */
	nessie_print_item("key", key, (nessie_bc_ctx.keysize_b+7)/8);
	nessie_bc_ctx.cipher_genctx(key, nessie_bc_ctx.keysize_b, ctx);
	
	memcpy(buffer, pt, nessie_bc_ctx.blocksize_B);
	nessie_print_item("plain", buffer, nessie_bc_ctx.blocksize_B);
	nessie_bc_ctx.cipher_enc(buffer, ctx);
	nessie_print_item("cipher", buffer, nessie_bc_ctx.blocksize_B);
	if(nessie_bc_ctx.cipher_dec){
		nessie_bc_ctx.cipher_dec(buffer, ctx);
		nessie_print_item("decrypted", buffer, nessie_bc_ctx.blocksize_B);
	}
	/* 100 times test */
	memcpy(buffer, pt, nessie_bc_ctx.blocksize_B);
	for(i=0; i<100; ++i){
		nessie_bc_ctx.cipher_enc(buffer, ctx);
		NESSIE_SEND_ALIVE_A(i);
	}
	nessie_print_item("Iterated 100 times", buffer, nessie_bc_ctx.blocksize_B);
#ifndef NESSIE_NO1KTEST	
	/* 1000 times test, we use the 100 precedig steps to fasten things a bit */
	for(; i<1000; ++i){
		nessie_bc_ctx.cipher_enc(buffer, ctx);
		NESSIE_SEND_ALIVE_A(i);
	}
	nessie_print_item("Iterated 1000 times", buffer, nessie_bc_ctx.blocksize_B);
#endif
	nessie_bc_free(ctx);
}

void nessie_bc_dec(uint8_t* key, uint8_t* ct){
	uint8_t ctx[nessie_bc_ctx.ctx_size_B];
	uint8_t buffer[nessie_bc_ctx.blocksize_B];
	
	/* single test */
	nessie_print_item("key", key, (nessie_bc_ctx.keysize_b+7)/8);
	nessie_bc_ctx.cipher_genctx(key, nessie_bc_ctx.keysize_b, ctx);
	memcpy(buffer, ct, nessie_bc_ctx.blocksize_B);
	nessie_print_item("cipher", buffer, nessie_bc_ctx.blocksize_B);
	nessie_bc_ctx.cipher_dec(buffer, ctx);
	nessie_print_item("plain", buffer, nessie_bc_ctx.blocksize_B);
	nessie_bc_ctx.cipher_enc(buffer, ctx);
	nessie_print_item("encrypted", buffer, nessie_bc_ctx.blocksize_B);
	nessie_bc_free(ctx);
}

void nessie_bc_run(void){
	uint16_t i;
	uint8_t set;
	uint8_t key[(nessie_bc_ctx.keysize_b+7)/8];
	uint8_t buffer[nessie_bc_ctx.blocksize_B];
	
	nessie_print_header(nessie_bc_ctx.name, nessie_bc_ctx.keysize_b,
	                    nessie_bc_ctx.blocksize_B*8, 0, 0, 0);
	/* test set 1 */
	set=1;
	nessie_print_setheader(set);
	for(i=0; i<nessie_bc_ctx.keysize_b; ++i){
		nessie_print_set_vector(set, i);
		memset(key, 0, (nessie_bc_ctx.keysize_b+7)/8);
		key[i/8] |= 0x80>>(i%8);
		memset(buffer, 0, nessie_bc_ctx.blocksize_B);
		nessie_bc_enc(key, buffer);
	}
	/* test set 2 */
	set=2;
	nessie_print_setheader(set);
	for(i=0; i<nessie_bc_ctx.blocksize_B*8; ++i){
		nessie_print_set_vector(set, i);
		memset(key, 0, (nessie_bc_ctx.keysize_b+7)/8);
		memset(buffer, 0, nessie_bc_ctx.blocksize_B);
		buffer[i/8] |= 0x80>>(i%8);
		nessie_bc_enc(key, buffer);
	}
	/* test set 3 */
	set=3;
	nessie_print_setheader(set);
	for(i=0; i<256; ++i){
		nessie_print_set_vector(set, i);
		memset(key, i, (nessie_bc_ctx.keysize_b+7)/8);
		memset(buffer, i, nessie_bc_ctx.blocksize_B);
		nessie_bc_enc(key, buffer);
	}
	/* test set 4 */
	set=4;
	nessie_print_setheader(set);
	/* 4 - 0*/
	nessie_print_set_vector(set, 0);
	for(i=0; i<(nessie_bc_ctx.keysize_b+7)/8; ++i){
		key[i]=i;
	}
	for(i=0; i<nessie_bc_ctx.blocksize_B; ++i){
		buffer[i]=i*0x11;
	}
	nessie_bc_enc(key, buffer);
	/* 4 - 1 */
	nessie_print_set_vector(set, 1);	
    /* This is the test vectors in Kasumi */
    static uint8_t kasumi_key[] = {
           0x2B, 0xD6, 0x45, 0x9F, 0x82, 0xC5, 0xB3, 0x00,
           0x95, 0x2C, 0x49, 0x10, 0x48, 0x81, 0xFF, 0x48 };
    static uint8_t kasumi_plain[]={
           0xEA, 0x02, 0x47, 0x14, 0xAD, 0x5C, 0x4D, 0x84 };
	for(i=0; i<(nessie_bc_ctx.keysize_b+7)/8; ++i){
		key[i]=kasumi_key[i%sizeof(kasumi_key)];
	}
	for(i=0; i<nessie_bc_ctx.blocksize_B; ++i){
		buffer[i]=kasumi_plain[i%sizeof(kasumi_plain)];
	}
	nessie_bc_enc(key, buffer);
	/* half done ;-) */
	if(nessie_bc_ctx.cipher_dec==NULL)
		return;
	/* test set 5 */
	set=5;
	nessie_print_setheader(set);
	for(i=0; i<nessie_bc_ctx.keysize_b; ++i){
		nessie_print_set_vector(set, i);
		memset(key, 0, (nessie_bc_ctx.keysize_b+7)/8);
		key[i/8] |= 0x80>>(i%8);
		memset(buffer, 0, nessie_bc_ctx.blocksize_B);
		nessie_bc_dec(key, buffer);
	}
	/* test set 6 */
	set=6;
	nessie_print_setheader(set);
	for(i=0; i<nessie_bc_ctx.blocksize_B*8; ++i){
		nessie_print_set_vector(set, i);
		memset(key, 0, (nessie_bc_ctx.keysize_b+7)/8);
		memset(buffer, 0, nessie_bc_ctx.blocksize_B);
		buffer[i/8] |= 0x80>>(i%8);
		nessie_bc_dec(key, buffer);
	}
	/* test set 7 */
	set=7;
	nessie_print_setheader(set);
	for(i=0; i<256; ++i){
		nessie_print_set_vector(set, i);
		memset(key, i, (nessie_bc_ctx.keysize_b+7)/8);
		memset(buffer, i, nessie_bc_ctx.blocksize_B);
		nessie_bc_dec(key, buffer);
	}
	/* test set 8 */
	set=8;
	nessie_print_setheader(set);
	/* 8 - 0*/
	nessie_print_set_vector(set, 0);
	for(i=0; i<(nessie_bc_ctx.keysize_b+7)/8; ++i){
		key[i]=i;
	}
	for(i=0; i<nessie_bc_ctx.blocksize_B; ++i){
		buffer[i]=i*0x11;
	}
	nessie_bc_dec(key, buffer);
	/* 8 - 1 */
	nessie_print_set_vector(set, 1);	
 	for(i=0; i<(nessie_bc_ctx.keysize_b+7)/8; ++i){
		key[i]=kasumi_key[i%sizeof(kasumi_key)];
	}
	for(i=0; i<nessie_bc_ctx.blocksize_B; ++i){
		buffer[i]=kasumi_plain[i%sizeof(kasumi_plain)];
	}
	nessie_bc_dec(key, buffer);
	nessie_print_footer();
}
