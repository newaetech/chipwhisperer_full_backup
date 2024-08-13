/* main-aes128-test.c */
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
 * AES-128 test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "aes.h"
#include "cli.h"
#include "performance_test.h"
#include "blockcipher_descriptor.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_aes128.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

char* algo_name = "AES-128";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&aes128_desc,
	NULL
};


/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_aes(void){
	bcal_nessie_multiple(algolist);
}

void testrun_test_aes(void){
	uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16,
	                    0x28, 0xae, 0xd2, 0xa6,
	                    0xab, 0xf7, 0x15, 0x88,
	                    0x09, 0xcf, 0x4f, 0x3c };
	uint8_t data[16] = { 0x32, 0x43, 0xf6, 0xa8,
	                     0x88, 0x5a, 0x30, 0x8d,
	                     0x31, 0x31, 0x98, 0xa2,
	                     0xe0, 0x37, 0x07, 0x34 };
	aes128_ctx_t ctx;
	aes128_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n cipher test (FIPS 197):\r\n key:        "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);
	aes128_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\n ciphertext: "));
	cli_hexdump(data, 16);
	aes128_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\n plaintext:  "));
	cli_hexdump(data, 16);


}

void testrun_testkey_aes128(void){
	uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16,
	                    0x28, 0xae, 0xd2, 0xa6,
	                    0xab, 0xf7, 0x15, 0x88,
	                    0x09, 0xcf, 0x4f, 0x3c};
	aes128_ctx_t ctx;
	uint8_t i;
	aes128_init(key, &ctx);
	cli_putstr_P(PSTR("\r\n\r\n keyschedule test (FIPS 197):\r\n key:   "));
	cli_hexdump(key, 16);
	for(i=0; i<11; ++i){
		cli_putstr_P(PSTR("\r\n index: "));
		cli_putc('0'+i/10);
		cli_putc('0'+i%10);
		cli_putstr_P(PSTR(" roundkey "));
		cli_hexdump(ctx.key[i].ks, 16);
	}
}

void testrun_testkey_aes(void){
	testrun_testkey_aes128();
}

/*****************************************************************************/

void testrun_performance_aes(void){
	bcal_performance_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/
const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testkey_str[]     PROGMEM = "testkey";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_aes },
	{ test_str,        NULL, testrun_test_aes},
	{ testkey_str,     NULL, testrun_testkey_aes},
	{ performance_str, NULL, testrun_performance_aes},
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

