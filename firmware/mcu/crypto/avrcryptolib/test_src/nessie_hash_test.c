/* nessie_hash_test.c */
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
 * a suit for running the nessie-tests for hashes
 * 
 * */
#include <stdint.h>
#include <string.h>
#include "nessie_hash_test.h"
#include "nessie_common.h"
#include "dbz_strings.h"

nessie_hash_ctx_t nessie_hash_ctx;
uint8_t           nessie_hash_quick=0;

#define HASHSIZE_B ((nessie_hash_ctx.hashsize_b+7)/8)
#define BLOCKSIZE_B (nessie_hash_ctx.blocksize_B)

static
void ascii_hash_P(PGM_P data, PGM_P desc){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[HASHSIZE_B];
	uint16_t sl;
	uint8_t buffer[BLOCKSIZE_B];
	
	NESSIE_PUTSTR_P(PSTR("\r\n                       message="));
	NESSIE_PUTSTR_P(desc);
	nessie_hash_ctx.hash_init(ctx);
	sl = strlen_P(data);
	while(sl>=BLOCKSIZE_B){
		memcpy_P(buffer, data, BLOCKSIZE_B);
		nessie_hash_ctx.hash_next(ctx, buffer);
		data += BLOCKSIZE_B;
		sl   -= BLOCKSIZE_B;
	}
	memcpy_P(buffer, data, sl);
	nessie_hash_ctx.hash_last(ctx, buffer, sl*8);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

// message=1 million times "a"

static
void amillion_hash(void){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	uint32_t n=1000000LL;
	uint16_t i=0;
	
	NESSIE_PUTSTR_P(PSTR("\r\n                       message="));
	NESSIE_PUTSTR_P(PSTR("1 million times \"a\""));
	memset(block, 'a', nessie_hash_ctx.blocksize_B);
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B){
		nessie_hash_ctx.hash_next(ctx, block);
		n    -= nessie_hash_ctx.blocksize_B;
		NESSIE_SEND_ALIVE_A(i++);
	}
	nessie_hash_ctx.hash_last(ctx, block, n*8);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}


static
void zero_hash(uint16_t n){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	
	NESSIE_PUTSTR_P(PSTR("\r\n                       message="));
	if(n>=10000)
		NESSIE_PUTC('0'+n/10000);
	if(n>=1000)
		NESSIE_PUTC('0'+(n/1000)%10);
	if(n>=100)
		NESSIE_PUTC('0'+(n/100)%10);
	if(n>=10)
		NESSIE_PUTC('0'+(n/10)%10);
	NESSIE_PUTC('0'+n%10);
	NESSIE_PUTSTR_P(PSTR(" zero bits"));
	
	memset(block, 0, nessie_hash_ctx.blocksize_B); 
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(ctx, block);
		n   -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(ctx, block, n);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

static
void one_in512_hash(uint16_t pos){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.blocksize_B];
	uint16_t n=512;
	char* tab[8]={"80", "40", "20", "10", 
	              "08", "04", "02", "01" };

	pos&=511;
	NESSIE_PUTSTR_P(PSTR("\r\n                       message="));
	NESSIE_PUTSTR_P(PSTR("512-bit string: "));
	if((pos/8) >=10){
		NESSIE_PUTC('0'+(pos/8/10)%10);
	} else {
		NESSIE_PUTC(' ');
	}
	NESSIE_PUTC('0'+(pos/8)%10);
	NESSIE_PUTSTR_P(PSTR("*00,"));
	NESSIE_PUTSTR(tab[pos&7]);
	NESSIE_PUTC(',');
	if(63-(pos/8) >=10){
		NESSIE_PUTC('0'+((63-pos/8)/10)%10);
	} else {
		NESSIE_PUTC(' ');
	}
	NESSIE_PUTC('0'+(63-pos/8)%10);
	NESSIE_PUTSTR_P(PSTR("*00"));
	
	/* now the real stuff */
	memset(block, 0, 512/8);
	block[pos>>3] = 0x80>>(pos&0x7);
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(ctx, block);
		n   -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(ctx, block, n);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

static
void tv4_hash(void){
	uint8_t ctx[nessie_hash_ctx.ctx_size_B];
	uint8_t hash[(nessie_hash_ctx.hashsize_b+7)/8];
	uint8_t block[nessie_hash_ctx.hashsize_b/8];
	uint16_t n=nessie_hash_ctx.hashsize_b;
	uint32_t i;
	
	NESSIE_PUTSTR_P(PSTR("\r\n                       message="));
	if(nessie_hash_ctx.hashsize_b>=10000)
		NESSIE_PUTC('0' + (nessie_hash_ctx.hashsize_b/10000)%10);
	if(nessie_hash_ctx.hashsize_b>=1000)
		NESSIE_PUTC('0' + (nessie_hash_ctx.hashsize_b/1000)%10);
	if(nessie_hash_ctx.hashsize_b>=100)
		NESSIE_PUTC('0' + (nessie_hash_ctx.hashsize_b/100)%10);
	if(nessie_hash_ctx.hashsize_b>=10)
		NESSIE_PUTC('0' + (nessie_hash_ctx.hashsize_b/10)%10);
	NESSIE_PUTC('0' + nessie_hash_ctx.hashsize_b%10);

	NESSIE_PUTSTR_P(PSTR(" zero bits"));
	memset(block, 0, nessie_hash_ctx.hashsize_b/8);
	
	nessie_hash_ctx.hash_init(ctx);
	while(n>=nessie_hash_ctx.blocksize_B*8){
		nessie_hash_ctx.hash_next(ctx, block);
		n    -= nessie_hash_ctx.blocksize_B*8;
	}
	nessie_hash_ctx.hash_last(ctx, block, n);
	nessie_hash_ctx.hash_conv(hash, ctx);
	nessie_print_item("hash", hash, (nessie_hash_ctx.hashsize_b+7)/8);
	if(nessie_hash_quick)
		return;
	for(i=1; i<100000L; ++i){ /* this assumes BLOCKSIZE >= HASHSIZE */
		nessie_hash_ctx.hash_init(ctx);
		nessie_hash_ctx.hash_last(ctx, hash, nessie_hash_ctx.hashsize_b);
		nessie_hash_ctx.hash_conv(hash, ctx);
		NESSIE_SEND_ALIVE_A(i);
	}
	nessie_print_item("iterated 100000 times", hash, (nessie_hash_ctx.hashsize_b+7)/8);
}

/*
   "" (empty string)
   message="a"
   message="abc"
   message="message digest"
   message="abcdefghijklmnopqrstuvwxyz"
   message="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
   message="A...Za...z0...9"
   message=8 times "1234567890"
*/


void nessie_hash_run(void){
	uint16_t i;
	uint8_t set;
	
	nessie_print_header(nessie_hash_ctx.name, 0, 0, nessie_hash_ctx.hashsize_b, 0, 0);
	/* test set 1 */
	const char* challange_dbz= PSTR(
		  "\0"
		"\"\" (empty string)\0"
		  "a\0"
		"\"a\"\0"
		  "abc\0"
		"\"abc\"\0"
		  "message digest\0"
		"\"message digest\"\0"
		  "abcdefghijklmnopqrstuvwxyz\0"
		"\"abcdefghijklmnopqrstuvwxyz\"\0"
		  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\0"
		"\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\"\0"
		  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		  "abcdefghijklmnopqrstuvwxyz"
		  "0123456789\0"	 
		 "\"A...Za...z0...9\"\0"
		 "1234567890123456789012345678901234567890" 
		 "1234567890123456789012345678901234567890\0"
		 "8 times \"1234567890\"\0"
	);
	PGM_P challange[16];
	set=1;
	nessie_print_setheader(set);
	dbz_splitup_P(challange_dbz, challange);
	for(i=0; i<8; ++i){
		nessie_print_set_vector(set, i);
		ascii_hash_P(challange[2*i], challange[2*i+1]);
	}
	nessie_print_set_vector(set, i);
	if(!nessie_hash_quick)
		amillion_hash();
	/* test set 2 */
	set=2;
	nessie_print_setheader(set);
	for(i=0; i<1024; ++i){
		nessie_print_set_vector(set, i);
		zero_hash(i);
	}
	/* test set 3 */
	set=3;
	nessie_print_setheader(set);
	for(i=0; i<512; ++i){
		nessie_print_set_vector(set, i);
		one_in512_hash(i);
	}
	/* test set 4 */
	set=4;
	nessie_print_setheader(set);
	nessie_print_set_vector(set, 0);
	tv4_hash();

	nessie_print_footer();
}
