/* scal-nessie.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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
#include "streamcipher_descriptor.h"
#include "scal-basic.h"
#include "nessie_common.h"
#include "memxor.h"
#include <avr/pgmspace.h>

static
uint8_t estream_flag = 0;

void scal_nessie_set_estream(uint8_t v){
	estream_flag = v?1:0;
}

uint8_t scal_nessie_get_estream(void){
	return estream_flag?1:0;
}



static const uint8_t normal_hooks[] PROGMEM = {
		0, 192/64, 256/64, 448/64
};

static const uint16_t long_hooks[] PROGMEM = {
		0, 65472/64, 65536/64, 131008/64
};

static const char stream0_n[] PROGMEM = "stream[0..63]";
static const char stream1_n[] PROGMEM = "stream[192..255]";
static const char stream2_n[] PROGMEM = "stream[256..319]";
static const char stream3_n[] PROGMEM = "stream[448..511]";

static const char streamX_n_estream[] PROGMEM = "xor-digest";
static const char streamX_n_nessie[] PROGMEM = "stream[0..511]xored";

static const char* stream_n_str[] = {
		stream0_n,
		stream1_n,
		stream2_n,
		stream3_n,
		streamX_n_nessie
};

static const char stream1_l[] PROGMEM = "stream[65472..65535]";
static const char stream2_l[] PROGMEM = "stream[65536..65599]";
static const char stream3_l[] PROGMEM = "stream[131008..131071]";
static const char streamX_l_estream[] PROGMEM = "xor-digest";
static const char streamX_l_nessie[] PROGMEM = "stream[0..131071]xored";

static const char* stream_l_str[] = {
		stream0_n,
		stream1_l,
		stream2_l,
		stream3_l,
		streamX_l_nessie
};

static const uint8_t list_of_keys[][20] PROGMEM = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc,
	  0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x00, 0x00, 0x00, 0x00 },
	{ 0x67, 0x45, 0x23, 0x01, 0xef, 0xcd, 0xab, 0x89, 0x98, 0xba,
	  0xdc, 0xfe, 0x10, 0x32, 0x54, 0x76, 0xc3, 0xd2, 0xe1, 0xf0 }
};

static const uint8_t list_of_ivs[][4] PROGMEM = {
	{ 0x00, 0x00, 0x00, 0x00 },
	{ 0x01, 0x01, 0x01, 0x01 },
	{ 0x01, 0x35, 0x77, 0xaf }
};

static
void normal_block(scgen_ctx_t *ctx){
	uint8_t xor_block[64];
	uint8_t block[64];
	PGM_VOID_P hook_ptr = normal_hooks;
	const char* *hook_str_ptr = stream_n_str;
	char str[21];
	uint8_t i;

	memset(xor_block, 0, 64);
	for(i=0; i<=448/64; ++i){
		scal_cipher_gen_fillblock(block, 64, ctx);
		memxor(xor_block, block, 64);
		if(i==pgm_read_byte(hook_ptr)){
			hook_ptr = (uint8_t*)hook_ptr + 1;
			strcpy_P(str, *(hook_str_ptr));
			hook_str_ptr = (const char **)((uint8_t*)hook_str_ptr + 2);
			nessie_print_item(str, block, 64);
		}
	}
	strcpy_P(str, *(hook_str_ptr));
	nessie_print_item(str, xor_block, 64);
}

static
void long_block(scgen_ctx_t *ctx){
	uint8_t xor_block[64];
	uint8_t block[64];
	PGM_VOID_P hook_ptr = long_hooks;
	const char* *hook_str_ptr = stream_l_str;
	char str[24];
	uint16_t i;

	memset(xor_block, 0, 64);
	for(i=0; i<=131008/64; ++i){
		scal_cipher_gen_fillblock(block, 64, ctx);
		memxor(xor_block, block, 64);
		if(i==pgm_read_word(hook_ptr)){
			hook_ptr = (uint8_t*)hook_ptr + 2;
			strcpy_P(str, *(hook_str_ptr));
			hook_str_ptr =  (const char **)((uint8_t*)hook_str_ptr + 2);
			nessie_print_item(str, block, 64);
		}
		if(i%64==0){
			NESSIE_SEND_ALIVE;
		}
	}
	strcpy_P(str, *(hook_str_ptr));
	nessie_print_item(str, xor_block, 64);
}

void scal_nessie_stream_run(const scdesc_t *desc, uint16_t keysize_b, uint16_t ivsize_b){
	uint8_t name_length = strlen_P((PGM_VOID_P)pgm_read_word(&(desc->name)));
	char name[name_length+1];
	memcpy_P(name, (PGM_VOID_P)pgm_read_word(&(desc->name)), name_length+1);

	uint8_t key[(keysize_b+7)/8];
	uint8_t iv[(ivsize_b+7)/8];
	uint16_t v;
	scgen_ctx_t ctx;
	nessie_print_header(name, keysize_b, 0, 0, 0, ivsize_b?ivsize_b:((uint16_t)-1));
	if(estream_flag){
		stream_n_str[4] = streamX_n_estream;
		stream_l_str[4] = streamX_l_estream;
	}else{
		stream_n_str[4] = streamX_n_nessie;
		stream_l_str[4] = streamX_l_nessie;
	}
	memset(iv, 0, (ivsize_b+7)/8);
	memset(key, 0, (keysize_b+7)/8);
	/***  Test SET 1 ***/
	nessie_print_setheader(1);
	for(v=0;v<keysize_b; v+=estream_flag?9:1){
		nessie_print_set_vector(1,v);
		key[v/8] |= 0x80>>(v&7);
		nessie_print_item("key", key, (keysize_b+7)/8);
		if(ivsize_b){
			nessie_print_item("IV", iv, (ivsize_b+7)/8);
		}
		scal_cipher_init(desc, key, keysize_b, iv, ivsize_b, &ctx);
		normal_block(&ctx);
		key[v/8] = 0;
		scal_cipher_free(&ctx);
	}
	/***  Test SET 2 ***/
	nessie_print_setheader(2);
	for(v=0;v<256; v+=estream_flag?9:1){
		nessie_print_set_vector(2,v);
		memset(key, v&0xff, (keysize_b+7)/8);
		nessie_print_item("key", key, (keysize_b+7)/8);
		if(ivsize_b){
			nessie_print_item("IV", iv, (ivsize_b+7)/8);
		}
		scal_cipher_init(desc, key, keysize_b, iv, ivsize_b, &ctx);
		normal_block(&ctx);
		scal_cipher_free(&ctx);
	}
	/***  Test SET 3 ***/
	nessie_print_setheader(3);
	for(v=0;v<256; v+=estream_flag?9:1){
		uint8_t i;
		nessie_print_set_vector(3,v);
		for(i=0; i<((keysize_b+7)/8); ++i){
			key[i]=(i+v)&0xff;
		}
		nessie_print_item("key", key, (keysize_b+7)/8);
		if(ivsize_b){
			nessie_print_item("IV", iv, (ivsize_b+7)/8);
		}
		scal_cipher_init(desc, key, keysize_b, iv, ivsize_b, &ctx);
		normal_block(&ctx);
		scal_cipher_free(&ctx);
	}
	/***  Test SET 4 ***/
	nessie_print_setheader(4);
	for(v=0;v<4; ++v){
		uint8_t i;
		nessie_print_set_vector(4,v);
		for(i=0; i<((keysize_b+7)/8); ++i){
			key[i]=(i*0x53+v*5)&0xff;
		}
		nessie_print_item("key", key, (keysize_b+7)/8);
		if(ivsize_b){
			nessie_print_item("IV", iv, (ivsize_b+7)/8);
		}
		scal_cipher_init(desc, key, keysize_b, iv, ivsize_b, &ctx);
		long_block(&ctx);
		scal_cipher_free(&ctx);
	}
	if(ivsize_b==0){ /* exit if there is no IV */
		nessie_print_footer();
		return;
	}
	/***  Test SET 5 ***/
	nessie_print_setheader(5);
	memset(key, 0, (keysize_b+7)/8);
	for(v=0;v<ivsize_b; v+=estream_flag?9:1){
		nessie_print_set_vector(5,v);
		iv[v/8] |= 0x80>>(v&7);
		nessie_print_item("key", key, (keysize_b+7)/8);
		nessie_print_item("IV", iv, (ivsize_b+7)/8);
		scal_cipher_init(desc, key, keysize_b, iv, ivsize_b, &ctx);
		normal_block(&ctx);
		scal_cipher_free(&ctx);
		iv[v/8] = 0;
	}
	/***  Test SET 6 ***/
	nessie_print_setheader(6);
	for(v=0;v<4; ++v){
		uint8_t i;
		nessie_print_set_vector(6,v);
		for(i=0; i<((keysize_b+7)/8); ++i){
			key[i]=(i*0x53+v*5)&0xff;
		}
		for(i=0; i<((ivsize_b+7)/8); ++i){
			iv[i]=(i*0x67+v*9+13)&0xff;
		}
		nessie_print_item("key", key, (keysize_b+7)/8);
		nessie_print_item("IV", iv, (ivsize_b+7)/8);
		scal_cipher_init(desc, key, keysize_b, iv, ivsize_b, &ctx);
		long_block(&ctx);
		scal_cipher_free(&ctx);
	}
	/***  Test SET 7 ***/
	if(!estream_flag){
		nessie_print_setheader(7);
		uint8_t u;
		for(v=0;v<3; ++v){
			for(u=0; u<3; ++u){
				uint8_t i;
				nessie_print_set_vector(7,v*3+u);
				for(i=0; i<((keysize_b+7)/8); ++i){
					key[i]=pgm_read_byte(list_of_keys+20*v+(i%20));
				}
				for(i=0; i<((ivsize_b+7)/8); ++i){
					key[i]=pgm_read_byte(list_of_keys+4*u+(i%4));
				}
			}
			nessie_print_item("key", key, (keysize_b+7)/8);
			nessie_print_item("IV", iv, (ivsize_b+7)/8);
			scal_cipher_init(desc, key, keysize_b, iv, ivsize_b, &ctx);
			long_block(&ctx);
			scal_cipher_free(&ctx);
		}
	}
	nessie_print_footer();
}

void scal_nessie_run(const scdesc_t* desc){
	uint16_t keysizes_count, ivsizes_count,i,j;
	uint16_t *keysizes=NULL, *ivsizes=NULL;
	keysizes_count = get_keysizes((PGM_VOID_P)pgm_read_word(&(desc->valid_keysize_desc)), &keysizes);
	ivsizes_count = get_keysizes((PGM_VOID_P)pgm_read_word(&(desc->valid_ivsize_desc)), &ivsizes);
	for(i=0; i<keysizes_count; ++i){
		for(j=0; j<ivsizes_count; ++j){
			scal_nessie_stream_run(desc, keysizes[i], ivsizes[j]);
		}
	}
}
