/* main-skipjack-test.c */
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
 * skipjack test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "skipjack.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_skipjack.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


char* algo_name = "Skipjack";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&skipjack_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void skipjack_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	memcpy(ctx, key, 10);
}

void testrun_nessie_skipjack(void){
	bcal_nessie_multiple(algolist);
}


void testrun_performance_skipjack(void){
	bcal_performance_multiple(algolist);
}

int test_enc(const void* buffer, void* key){
	uint8_t data[8];
	int r;
	memcpy(data, buffer, 8);
	skipjack_enc(data, key);
	cli_putstr_P(PSTR("  key = "));
	cli_hexdump(key, 10);
	cli_putstr_P(PSTR(" plaintext = "));
	cli_hexdump(buffer, 8);
	cli_putstr_P(PSTR(" ciphertext = "));
	cli_hexdump(data, 8);
	skipjack_dec(data, key);
	r = memcmp(data, buffer, 8);
	cli_putstr_P(PSTR(" decrypt: "));
	if(r){
		cli_putstr_P(PSTR("fail"));
	}else{
		cli_putstr_P(PSTR("ok"));
	}
	return r;
}

void testrun_nist_vectors(void){
	uint8_t key[10];
	uint8_t data[8];
	uint8_t i;

	cli_putstr_P(PSTR("\r\n\r\n=== NIST vectors run 1 ==="));
	memset(key, 0, 10);
	for(i=0; i<64; ++i){
		memset(data, 0, 8);
		data[i>>3] |= 0x80 >> (i & 7);
		cli_putstr_P(PSTR("\r\n round: 0x"));
		cli_hexdump_byte(i);
		test_enc(data, key);
	}

	cli_putstr_P(PSTR("\r\n\r\n=== NIST vectors run 2 ==="));
	memset(data, 0, 8);
	for(i=0; i<80; ++i){
                memset(key, 0, 10);
                key[i>>3] |= 0x80 >> (i & 7);
                cli_putstr_P(PSTR("\r\n round: 0x"));
                cli_hexdump_byte(i);
                test_enc(data, key);
        }
}

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

void testencrypt(uint8_t* block, uint8_t* key){
	cli_putstr("\r\n==testy-encrypt==\r\n key: ");
	cli_hexdump(key,10);
	cli_putstr("\r\n plain: ");
	cli_hexdump(block,8);
	skipjack_enc(block,key);
	cli_putstr("\r\n crypt: ");
	cli_hexdump(block,8);
}

void testdecrypt(uint8_t* block, uint8_t* key){
	cli_putstr("\r\n==testy-decrypt==\r\n key: ");
	cli_hexdump(key,10);
	cli_putstr("\r\n crypt: ");
	cli_hexdump(block,8);
	skipjack_dec(block,key);
	cli_putstr("\r\n plain: ");
	cli_hexdump(block,8);
}

void testrun_skipjack(void){
	uint8_t key[2][10]={
	                     { 0x00, 0x99, 0x88, 0x77, 0x66,
	                       0x55, 0x44, 0x33, 0x22, 0x11 },
	                     { 0x11, 0x22, 0x33, 0x44, 0x55,
	                       0x66, 0x77, 0x88, 0x99, 0x00 }
					   };   
	                 
	uint8_t data[2][8]={
		                 { 0x33, 0x22, 0x11, 0x00, 
		                   0xdd, 0xcc, 0xbb, 0xaa },
		                 { 0xaa, 0xbb, 0xcc, 0xdd,
		                   0x00, 0x11, 0x22, 0x33 }
					   };
	testencrypt(data[0],key[0]);
	testdecrypt(data[0],key[0]);
	testencrypt(data[1],key[1]);
	testdecrypt(data[1],key[1]);	
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char nist_str[]        PROGMEM = "nist";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_skipjack},
	{ test_str,        NULL, testrun_skipjack},
	{ nist_str,        NULL, testrun_nist_vectors},
	{ performance_str, NULL, testrun_performance_skipjack},
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
