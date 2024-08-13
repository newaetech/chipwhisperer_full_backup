/* main-noekeon-test.c */
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
 * noekeon test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "noekeon.h"
#include "bcal-nessie.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal_noekeon.h"
#include "cli.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Noekeon";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&noekeon_direct_desc,
	(bcdesc_t*)&noekeon_indirect_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_noekeon(void){
	bcal_nessie_multiple(algolist);
}


void testrun_stdtest_rundirect(void* data, void* key){
	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("k = "));
	cli_hexdump(key,16);

	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("a = "));
	cli_hexdump(data,16);
	
	noekeon_enc(data, key);
	cli_putstr_P(PSTR("\r\nafter NESSIEencrypt, b = "));
	cli_hexdump(data,16);
	
	noekeon_dec(data, key);
	cli_putstr_P(PSTR("\r\nafter NESSIEdecrypt, a?= "));
	cli_hexdump(data,16);
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_stdtest_runindirect(void* data, void* key){
	noekeon_ctx_t ctx;
	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("k = "));
	cli_hexdump(key,16);
	
	cli_putstr_P(PSTR("\r\n                     "));
	cli_putstr_P(PSTR("a = "));
	cli_hexdump(data,16);
	noekeon_init(key, &ctx);
	noekeon_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\nafter NESSIEencrypt, b = "));
	cli_hexdump(data,16);
	
	noekeon_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\nafter NESSIEdecrypt, a?= "));
	cli_hexdump(data,16);
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_stdtest_noekeon(void){
	uint8_t key[16], data[16];
	uint8_t key3[16];
	noekeon_ctx_t ctx;
	
	cli_putstr_P(PSTR("\r\nTest vectors for block cipher Noekeon in Indirect-Key Mode:\r\n"));
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	testrun_stdtest_runindirect(data, key);
	
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	testrun_stdtest_runindirect(data, key);
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	noekeon_init(key, &ctx);
	noekeon_enc(data, &ctx);
	memcpy(key3, data, 16);
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	noekeon_init(key, &ctx);
	noekeon_enc(data, &ctx);
	testrun_stdtest_runindirect(data, key3);
	
	cli_putstr_P(PSTR("\r\nTest vectors for block cipher Noekeon in Direct-Key Mode:\r\n"));
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	testrun_stdtest_rundirect(data, key);
	
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	testrun_stdtest_rundirect(data, key);
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	noekeon_enc(data, key);
	memcpy(key3, data, 16);
	memset(key,  0xFF, 16);
	memset(data, 0xFF, 16);
	noekeon_enc(data, key);
	testrun_stdtest_rundirect(data, key3);
	
}

void testrun_performance_noekeon(void){
	bcal_performance_multiple(algolist);
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char direct_str[]      PROGMEM = "direct";
const char indirect_str[]    PROGMEM = "indirect";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_noekeon},
	{ test_str,        NULL, testrun_stdtest_noekeon},
//	{ direct_str,      NULL, testrun_nessie_noekeon_direct},
//	{ indirect_str,    NULL, testrun_nessie_noekeon_indirect},
	{ performance_str, NULL, testrun_performance_noekeon},
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
