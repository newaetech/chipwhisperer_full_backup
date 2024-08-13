/* main-cscipher-test.c */
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
 * cscipher test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "cscipher.h"
#include "bcal-nessie.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal_cscipher.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "CS-Cipher";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&cscipher_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void cscipher_init_dummy(const uint8_t* key, uint16_t keysize_b, void* ctx){
	cscipher_init(key, ctx);
}

void testrun_nessie_cscipher(void){
	bcal_nessie(&cscipher_desc);
}

void testrun_cscipher(void){
	uint8_t data[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
	uint8_t key[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
					 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	cscipher_ctx_t ctx;
	cli_putstr_P(PSTR("\r\n== CS-Cipher test==\r\nkey: "));
	cli_hexdump(key, 16);
	memset(&ctx, 0, 8*9);
	cscipher_init(key, &ctx);
	cli_putstr_P(PSTR("\r\nround keys:\r\n"));
	cli_hexdump_block(&ctx, 8*9, 4, 8);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 8);
	cscipher_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 8);
	cscipher_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 8);
}

void testrun_long_cscipher(void){
	uint8_t data[8];
	char str[10];
	uint16_t i;
	uint8_t j;
	uint8_t key[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
					 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	cscipher_ctx_t ctx;
	cli_putstr_P(PSTR("\r\n== CS-Cipher test==\r\nkey: "));
	cli_hexdump(key, 16);
	cscipher_init(key, &ctx);
	memset(data, 0, 8);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 8);
	cli_putstr_P(PSTR("\r\nencrypting 1,000,000 times:\r\n"));
	for(i=0; i<10000;++i){
		for(j=0;j<100;++j){
			cscipher_enc(data, &ctx);
		}
		if(i%128==0){
			itoa(i, str, 10);
			cli_putstr_P(PSTR("\r\n"));
			cli_putstr(str);
			cli_putstr_P(PSTR(": "));
		}
		cli_putc('*');

	}
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(data, 8);
	cli_putstr_P(PSTR("\r\ndecrypting 1,000,000 times:"));
	for(i=0; i<10000;++i){
		for(j=0;j<100;++j){
			cscipher_dec(data, &ctx);
		}
		if(i%128==0){
			itoa(i, str, 10);
			cli_putstr_P(PSTR("\r\n"));
			cli_putstr(str);
			cli_putstr_P(PSTR(": "));
		}
		cli_putc('*');
	}
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 8);
}


void testrun_performance_cscipher(void){
	bcal_performance_multiple(algolist);
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char longtest_str[]    PROGMEM = "longtest";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_cscipher },
	{ test_str,        NULL, testrun_cscipher},
	{ longtest_str,    NULL, testrun_long_cscipher},
	{ performance_str, NULL, testrun_performance_cscipher},
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
