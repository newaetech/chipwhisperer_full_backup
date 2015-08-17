/* main-threefish-test.c */
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
 * threefish test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "threefish.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_threefish256.h"
#include "bcal_threefish512.h"
#include "bcal_threefish1024.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Threefish";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&threefish256_desc,
	(bcdesc_t*)&threefish512_desc,
	(bcdesc_t*)&threefish1024_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void threefish256_dump(threefish256_ctx_t* ctx){
	uint8_t i;
	cli_putstr_P(PSTR("\r\n=== ctx dump (256) === \r\n k: "));
	for(i=0; i<5; ++i){
		cli_hexdump(&(ctx->k[i]), 8);
		cli_putc(' ');
	}
	cli_putstr_P(PSTR("\r\n t: "));
	for(i=0; i<3; ++i){
		cli_hexdump(&(ctx->t[i]), 8);
		cli_putc(' ');
	}
}

void testrun_nessie_threefish(void){
	bcal_nessie_multiple(algolist);
}

void testrun_stdtest_threefish256(void){
	uint8_t key[32], data[32];
	uint8_t tweak[16];
	uint8_t i;
	threefish256_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for block cipher Threefish (256 bits):"));
	memset(key,  0, 32);
	memset(data, 0, 32);
	memset(tweak, 0, 16);
	
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump_block(key, 32, 4, 16);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump_block(tweak, 16, 4, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump_block(data, 32, 4, 16);
	threefish256_init(key, tweak, &ctx);
	threefish256_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump_block(data, 32, 4, 16);
	cli_putstr_P(PSTR("\r\ndecipher: "));
	threefish256_dec(data, &ctx);
	cli_hexdump_block(data, 32, 4, 16);
	
	/* second test */
	for(i=0; i<32; ++i){
		key[i] = 0x10+i;
		data[i] = 0xFF-i;
	}
	for(i=0; i<16; ++i){
		tweak[i] = i;
	}
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump_block(key, 32, 4, 16);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump_block(tweak, 16, 4, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump_block(data, 32, 4, 16);
	threefish256_init(key, tweak, &ctx);
	threefish256_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump_block(data, 32, 4, 16);
	cli_putstr_P(PSTR("\r\ndecipher: "));
	threefish256_dec(data, &ctx);
	cli_hexdump_block(data, 32, 4, 16);
}

void testrun_stdtest_threefish512(void){
	uint8_t key[64], data[64];
	uint8_t tweak[16];
	uint8_t i;
	threefish512_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for block cipher Threefish (512 bits) :"));
	memset(key,  0, 64);
	memset(data, 0, 64);
	memset(tweak, 0, 16);
	
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump_block(key, 32, 4, 16);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump_block(key+32, 32, 4, 16);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump_block(tweak, 16, 4, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump_block(data, 64, 4, 16);
	threefish512_init(key, tweak, &ctx);
	threefish512_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump_block(data, 64, 4, 16);
	threefish512_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\ndecipher: "));
	cli_hexdump_block(data, 64, 4, 16);
	
	
	for(i=0; i<64; ++i){
		key[i] = 0x10+i;
		data[i] = 0xFF-i;
	}
	for(i=0; i<16; ++i){
		tweak[i] = i;
	}
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump_block(key, 32, 4, 16);
	cli_putstr_P(PSTR("\r\n        "));
	cli_hexdump_block(key+32, 32, 4, 16);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump_block(tweak, 16, 4, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump_block(data, 64, 4, 16);
	threefish512_init(key, tweak, &ctx);
	threefish512_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump_block(data, 64, 4, 16);
	threefish512_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\ndecipher: "));
	cli_hexdump_block(data, 64, 4, 16);
	
}

void testrun_stdtest_threefish1024(void){
	uint8_t key[128], data[128];
	uint8_t tweak[16];
	uint8_t i;
	threefish1024_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for block cipher Threefish (1024 bits) :"));
	memset(key,  0, 128);
	memset(data, 0, 128);
	memset(tweak, 0, 16);
	
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump_block(key, 128, 4, 16);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump_block(tweak, 16, 4, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump_block(data, 128, 4, 16);
	threefish1024_init(key, tweak, &ctx);
	threefish1024_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump_block(data, 128, 4, 16);
	threefish1024_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\ndecipher: "));
	cli_hexdump_block(data, 128, 4, 16);

	for(i=0; i<128; ++i){
		key[i] = 0x10+i;
		data[i] = 0xFF-i;
	}
	for(i=0; i<16; ++i){
		tweak[i] = i;
	}
	cli_putstr_P(PSTR("\r\nkey:    "));
	cli_hexdump_block(key, 128, 4, 16);
	cli_putstr_P(PSTR("\r\ntweak:  "));
	cli_hexdump_block(tweak, 16, 4, 16);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump_block(data, 128, 4, 16);
	threefish1024_init(key, tweak, &ctx);
	threefish1024_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump_block(data, 128, 4, 16);
	threefish1024_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\ndecipher: "));
	cli_hexdump_block(data, 128, 4, 16);
}


void testrun_stdtest_threefish(void){
	testrun_stdtest_threefish256();
	testrun_stdtest_threefish512();
	testrun_stdtest_threefish1024();
}

void testrun_performance_threefish(void){
	bcal_performance_multiple(algolist);
}

void init_test(void){
	threefish256_ctx_t ctx;
	uint8_t key[32], tweak[16];
	memset(key, 0,32);
	memset(tweak, 0,16);
	threefish256_init(key, tweak, &ctx);
	cli_putstr_P(PSTR("\r\n ctx: \r\n\tk:"));
	cli_hexdump(ctx.k, 5*8);
	cli_putstr_P(PSTR("\r\n\tt:"));
	cli_hexdump(ctx.t, 3*8);
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char test256_str[]     PROGMEM = "test256";
const char test512_str[]     PROGMEM = "test512";
const char test1024_str[]    PROGMEM = "test1024";
const char inittest_str[]    PROGMEM = "inittest";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_threefish},
	{ test_str,        NULL, testrun_stdtest_threefish},
    { test256_str,     NULL, testrun_stdtest_threefish256},
    { test512_str,     NULL, testrun_stdtest_threefish512},
    { test1024_str,    NULL, testrun_stdtest_threefish1024},
	{ inittest_str,    NULL, init_test},
	{ performance_str, NULL, testrun_performance_threefish},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
