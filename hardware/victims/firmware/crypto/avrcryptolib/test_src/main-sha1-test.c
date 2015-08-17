/* main-sha1-test.c */
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
/*
 * SHA-1 test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "sha1.h"
#include "nessie_hash_test.h"
#include "hfal_sha1.h"
#include "hfal-performance.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "shavs.h"
#include "hfal_sha1.h"
#include "dump.h"

char* algo_name = "SHA-1";

const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&sha1_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_sha1(void){
	nessie_hash_ctx.hashsize_b  = 160;
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(sha1_ctx_t);
	nessie_hash_ctx.name = algo_name;
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)sha1_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)sha1_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)sha1_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)sha1_ctx2hash;

	nessie_hash_run();
}

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void sha1_ctx_dump(sha1_ctx_t *s){
	uint8_t i;
	cli_putstr("\r\n==== sha1_ctx_dump ====");
	for(i=0;i<5;++i){
		cli_putstr("\r\na["); cli_hexdump(&i, 1); cli_putstr("]: ");
		cli_hexdump(&(s->h[i]), 4);
	}
	cli_putstr("\r\nlength"); cli_hexdump(&i, 8);
}

void testrun_sha1(void){
	sha1_hash_t hash;
	sha1(&hash,"abc",3*8);
	cli_putstr_P(PSTR("\r\nsha1(\"abc\") = \r\n\t"));
	cli_hexdump(hash,SHA1_HASH_BITS/8);

	sha1(&hash,"\0\0\0\0\0\0\0\0", 8*8);
	cli_putstr_P(PSTR("\r\nsha1(8 x 0x00) = \r\n\t"));
	cli_hexdump(hash,SHA1_HASH_BITS/8);
/*
   Len = 496
   Msg = 46fe5ed326c8fe376fcc92dc9e2714e2240d3253b105ad
         fbb256ff7a19bc40975c604ad7c0071c4fd78a7cb64786
         e1bece548fa4833c04065fe593f6fb10
   MD  = f220a7457f4588d639dc21407c942e9843f8e26b
*/
	sha1(&hash,"\x46\xfe\x5e\xd3\x26\xc8\xfe\x37"
	           "\x6f\xcc\x92\xdc\x9e\x27\x14\xe2"
	           "\x24\x0d\x32\x53\xb1\x05\xad\xfb"
	           "\xb2\x56\xff\x7a\x19\xbc\x40\x97"
	           "\x5c\x60\x4a\xd7\xc0\x07\x1c\x4f"
	           "\xd7\x8a\x7c\xb6\x47\x86\xe1\xbe"
	           "\xce\x54\x8f\xa4\x83\x3c\x04\x06"
	           "\x5f\xe5\x93\xf6\xfb\x10", 496);
	cli_putstr_P(PSTR("\r\nsha1(tv_496) = \r\n\t"));
	cli_hexdump(hash,SHA1_HASH_BITS/8);

	sha1(&hash,"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",448);
//	cli_putstr_P(PSTR("\r\nsha1(\"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\") = \r\n\t"));
	cli_hexdump(hash,SHA1_HASH_BITS/8);
	cli_putstr("\r\nsha1(1,000,000 * 'a') = \r\n\t");
	{
		uint8_t block[SHA1_BLOCK_BITS/8];
		uint16_t i;
		sha1_ctx_t s;
		memset(block,'a',SHA1_BLOCK_BITS/8);
		sha1_init(&s);
		for(i=0;i<15625; ++i){ /* (1000000/(SHA1_BLOCK_BITS/8)) */
			sha1_nextBlock(&s, block);
		}
		sha1_lastBlock(&s,block,0);
		sha1_ctx2hash(&hash, &s);
	}
	cli_hexdump(hash,SHA1_HASH_BITS/8);

}


void testrun_sha1_2(void){
	sha1_ctx_t ctx;
	sha1_hash_t hash;
	sha1(&hash,"",0);
	cli_putstr_P(PSTR("\r\nsha1(NULL) = \r\n\t"));
	cli_hexdump(hash,SHA1_HASH_BYTES);

	memset(hash, 0, SHA1_HASH_BYTES);

	sha1_init(&ctx);
	sha1_lastBlock(&ctx, "", 0);
	sha1_ctx2hash(&hash, &ctx);
	cli_putstr_P(PSTR("\r\nsha1(NULL) = \r\n\t"));
	cli_hexdump(hash,SHA1_HASH_BYTES);
}

/*
Msg = a38b899cae4edb191d88d861c842b6e32b9b67db66bdbdde8911d2b30fafa765a8190b963c28bf162c46d7b5dbde63556d114f43ceab88c7f65560f96c0c34c0
MD = 722246b014af03ef3ba31364fc732a4ab8f38587
*/

void testrun_sha1_506(void){
	uint8_t data[] = {
		0xa3, 0x8b, 0x89, 0x9c, 0xae, 0x4e, 0xdb, 0x19,
		0x1d, 0x88, 0xd8, 0x61, 0xc8, 0x42, 0xb6, 0xe3,
		0x2b, 0x9b, 0x67, 0xdb, 0x66, 0xbd, 0xbd, 0xde,
		0x89, 0x11, 0xd2, 0xb3, 0x0f, 0xaf, 0xa7, 0x65,
		0xa8, 0x19, 0x0b, 0x96, 0x3c, 0x28, 0xbf, 0x16,
		0x2c, 0x46, 0xd7, 0xb5, 0xdb, 0xde, 0x63, 0x55,
		0x6d, 0x11, 0x4f, 0x43, 0xce, 0xab, 0x88, 0xc7,
		0xf6, 0x55, 0x60, 0xf9, 0x6c, 0x0c, 0x34, 0xc0 };
	uint8_t ref[] = {
		0x72, 0x22, 0x46, 0xb0, 0x14, 0xaf, 0x03, 0xef,
		0x3b, 0xa3, 0x13, 0x64, 0xfc, 0x73, 0x2a, 0x4a,
		0xb8, 0xf3, 0x85, 0x87 };
	sha1_hash_t hash;
	sha1(&hash,data,506);
	cli_putstr_P(PSTR("\r\nsha1(<tv506>) = \r\n\t"));
	cli_hexdump(hash,SHA1_HASH_BYTES);
	cli_putstr_P(PSTR("\r\nshould        = \r\n\t"));
	cli_hexdump(ref,SHA1_HASH_BYTES);
	if(memcmp(ref, hash, SHA1_HASH_BYTES)==0){
		cli_putstr_P(PSTR("\r\n[ok]"));
	} else {
		cli_putstr_P(PSTR("\r\n[fail]"));
	}
}

void testrun_performance_sha1(void){
	hfal_performance_multiple(algolist);
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char test2_str[]       PROGMEM = "test2";
const char test506_str[]     PROGMEM = "test506";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test2_str[] PROGMEM = "shavs_test2";
const char dump_str[]        PROGMEM = "dump";


const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_sha1},
	{ test_str,            NULL, testrun_sha1},
	{ test2_str,           NULL, testrun_sha1_2},
	{ test506_str,         NULL, testrun_sha1_506},
	{ performance_str,     NULL, testrun_performance_sha1},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ shavs_test1_str,     NULL, shavs_test1},
	{ shavs_test2_str,     NULL, shavs_test2},
	{ dump_str,        (void*)1, (void_fpt)dump},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&sha1_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
