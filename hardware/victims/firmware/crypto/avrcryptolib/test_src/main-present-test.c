/* main-present-test.c */
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
 * present test-suit
 * 
*/

#include "main-test-common.h"

#include <present80.h>
#include <present128.h>
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_present80.h"
#include "bcal_present128.h"

char* algo_name = "Present";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&present80_desc,
	(bcdesc_t*)&present128_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void testrun_nessie_present(void){
	bcal_nessie_multiple(algolist);
}

void testrun_selfenc(uint8_t* key, uint8_t* buffer){

	present80_ctx_t ctx;
	cli_putstr_P(PSTR("\r\nkey   : "));
	cli_hexdump(key, 10);
	cli_putstr_P(PSTR("\r\nplain : "));
	cli_hexdump(buffer, 8);
	present80_init(key, 80, &ctx);
	present80_enc(buffer, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(buffer, 8);
	present80_dec(buffer, &ctx);
	cli_putstr_P(PSTR("\r\nplain : "));
	cli_hexdump(buffer, 8);
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_selfenc_128(uint8_t* key, uint8_t* buffer){

	present128_ctx_t ctx;
	cli_putstr_P(PSTR("\r\nkey   : "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\nplain : "));
	cli_hexdump(buffer, 8);
	present128_init(key, 128, &ctx);
	present128_enc(buffer, &ctx);
	cli_putstr_P(PSTR("\r\ncipher: "));
	cli_hexdump(buffer, 8);
	present128_dec(buffer, &ctx);
	cli_putstr_P(PSTR("\r\nplain : "));
	cli_hexdump(buffer, 8);
	cli_putstr_P(PSTR("\r\n"));
}
// void present_key_test(const uint8_t* key);


void testrun_self_present(void){
	uint8_t buffer[8], key[10], i;
	cli_putstr_P(PSTR("\r\n\r\n=== Testvectors from the paper ===\r\n"));
	for(i=0; i<4; ++i){
		memset(buffer, (i&2)?0xff:0x00,  8);
		memset(key,    (i&1)?0xff:0x00, 10);
		testrun_selfenc(key, buffer);
	}
	memset(buffer, 0x00,  8);
	memset(key,    0x00, 10);
	key[0] = 0x80;
	testrun_selfenc(key, buffer);

//	present_key_test(key);

}

void testrun_self_present_128(void){
	uint8_t buffer[8], key[16], i;
	cli_putstr_P(PSTR("\r\n\r\n=== Testvectors from the paper ===\r\n"));
	for(i=0; i<4; ++i){
		memset(buffer, (i&2)?0xff:0x00,  8);
		memset(key,    (i&1)?0xff:0x00, 16);
		testrun_selfenc_128(key, buffer);
	}
	memset(buffer, 0x00,  8);
	memset(key,    0x00, 16);
	key[0] = 0x80;
	testrun_selfenc_128(key, buffer);

//	present_key_test(key);

}

void testrun_performance_present(void){
	bcal_performance_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char test_128_str[]    PROGMEM = "test-128";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_present},
	{ test_str,        NULL, testrun_self_present},
	{ test_128_str,    NULL, testrun_self_present_128},
	{ performance_str, NULL, testrun_performance_present},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main(void) {
	main_setup();

	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&sha256_desc;

	for(;;){
		welcome_msg(algo_name);
		cmd_interface(cmdlist);
    }

}
