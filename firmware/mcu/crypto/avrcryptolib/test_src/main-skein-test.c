/* main-skein-test.c */
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
 * skein test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "skein.h"
#include "hfal_skein256.h"
#include "hfal_skein512.h"
#include "hfal_skein1024.h"
#include "cli.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-performance.h"
#include "hfal-nessie.h"
#include "hfal-basic.h"


#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Skein";

const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&skein256_128_desc,
	(hfdesc_t*)&skein256_160_desc,
	(hfdesc_t*)&skein256_224_desc,
	(hfdesc_t*)&skein256_256_desc,
	(hfdesc_t*)&skein256_384_desc,
	(hfdesc_t*)&skein256_512_desc,

	(hfdesc_t*)&skein512_128_desc,
	(hfdesc_t*)&skein512_160_desc,
	(hfdesc_t*)&skein512_224_desc,
	(hfdesc_t*)&skein512_256_desc,
	(hfdesc_t*)&skein512_384_desc,
	(hfdesc_t*)&skein512_512_desc,
	(hfdesc_t*)&skein512_1024_desc,

	(hfdesc_t*)&skein1024_128_desc,
	(hfdesc_t*)&skein1024_160_desc,
	(hfdesc_t*)&skein1024_224_desc,
	(hfdesc_t*)&skein1024_256_desc,
	(hfdesc_t*)&skein1024_384_desc,
	(hfdesc_t*)&skein1024_512_desc,
	(hfdesc_t*)&skein1024_1024_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void testrun_stdtest_skein256(uint16_t outsize_b){
	uint8_t message[64];
	uint8_t hash[(outsize_b+7)/8];
	uint8_t i;

	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Skein (256 bits):"));
	for(i=0; i<64; ++i)
		message[i] = 0xFF-i;

	cli_putstr_P(PSTR("\r\nmessage:    "));
	cli_hexdump(message, 1);
	skein256(hash, outsize_b, message, 8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 32, 4, 16);
	skein256(hash, outsize_b, message, 32*8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 64, 4, 16);
	skein256(hash, outsize_b, message, 64*8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
}

void testrun_stdtest_skein512(uint16_t outsize_b){
	uint8_t message[128];
	uint8_t hash[(outsize_b+7)/8];
	uint8_t i;

	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Skein (512 bits):"));
	for(i=0; i<128; ++i)
		message[i] = 0xFF-i;

	cli_putstr_P(PSTR("\r\nmessage:    "));
	cli_hexdump(message, 1);
	skein512(hash, outsize_b, message, 8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 64, 4, 16);
	skein512(hash, outsize_b, message, 64*8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 128, 4, 16);
	skein512(hash, outsize_b, message, 128*8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
}

void testrun_stdtest_skein1024(uint16_t outsize_b){
	uint8_t message[256];
	uint8_t hash[(outsize_b+7)/8];
	uint16_t i;

	cli_putstr_P(PSTR("\r\n\r\nTest vectors for Skein (1024 bits):"));
	for(i=0; i<256; ++i)
		message[i] = 0xFF-i;

	cli_putstr_P(PSTR("\r\nmessage:    "));
	cli_hexdump(message, 1);
	skein1024(hash, outsize_b, message, 8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 128, 4, 16);
	skein1024(hash, outsize_b, message, 128*8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	cli_putstr_P(PSTR("\r\nmessage:"));
	cli_hexdump_block(message, 256, 4, 16);
	skein1024(hash, outsize_b, message, 256*8);
	cli_putstr_P(PSTR("\r\nhash:"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
}

void testrun_stdtest_skein(void){
	testrun_stdtest_skein256(256);
	testrun_stdtest_skein512(512);
	testrun_stdtest_skein1024(1024);
}

void zeromsg_test_skein(uint16_t outsize_b){
	char str[8];
	uint8_t hash[(outsize_b+7)/8];

	skein256(hash, outsize_b, NULL, 0);
	cli_putstr_P(PSTR("\r\nskein256-"));
	utoa(outsize_b, str, 10);
	cli_putstr(str);
	cli_putstr_P(PSTR(" :"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	skein512(hash, outsize_b, NULL, 0);
	cli_putstr_P(PSTR("\r\nskein512-"));
	utoa(outsize_b, str, 10);
	cli_putstr(str);
	cli_putstr_P(PSTR(" :"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);

	skein1024(hash, outsize_b, NULL, 0);
	cli_putstr_P(PSTR("\r\nskein1024-"));
	utoa(outsize_b, str, 10);
	cli_putstr(str);
	cli_putstr_P(PSTR(" :"));
	cli_hexdump_block(hash, (outsize_b+7)/8, 4, 16);
}

void zeromsg_test_common(char* p){
	uint8_t i;
	uint16_t s=0;
	uint16_t sizes[]={128, 160, 224, 256, 384, 512, 1024};
	if(p){
		s = strtoul(p, NULL, 0);
	}
	if(s){
		zeromsg_test_skein(s);
	}else{
		for(i=0; i<7; ++i)
			zeromsg_test_skein(sizes[i]);
	}
}

void performance_skein(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie_skein(void){
	nessie_hash_quick = 1;
	hfal_nessie_multiple(algolist);
}

void test_monte2(void){
	uint8_t data[] = {
	0x6c, 0xd4, 0xc0, 0xc5, 0xcb, 0x2c, 0xa2, 0xa0,
	0xf1, 0xd1, 0xae, 0xce, 0xba, 0xc0, 0x3b, 0x52,
	0xe6, 0x4e, 0xa0, 0x3d, 0x1a, 0x16, 0x54, 0x37,
	0x29, 0x36, 0x54, 0x5b, 0x92, 0xbb, 0xc5, 0x48,
	0x4a, 0x59, 0xdb, 0x74, 0xbb, 0x60, 0xf9, 0xc4,
	0x0c, 0xeb, 0x1a, 0x5a, 0xa3, 0x5a, 0x6f, 0xaf,
	0xe8, 0x03, 0x49, 0xe1, 0x4c, 0x25, 0x3a, 0x4e,
	0x8b, 0x1d, 0x77, 0x61, 0x2d, 0xdd, 0x81, 0xac,
	0xe9, 0x26, 0xae, 0x8b, 0x0a, 0xf6, 0xe5, 0x31,
	0x76, 0xdb, 0xff, 0xcc, 0x2a, 0x6b, 0x88, 0xc6,
	0xbd, 0x76, 0x5f, 0x93, 0x9d, 0x3d, 0x17, 0x8a,
	0x9b, 0xde, 0x9e, 0xf3, 0xaa, 0x13, 0x1c, 0x61,
	0xe3, 0x1c, 0x1e, 0x42, 0xcd, 0xfa, 0xf4, 0xb4,
	0xdc, 0xde, 0x57, 0x9a, 0x37, 0xe1, 0x50, 0xef,
	0xbe, 0xf5, 0x55, 0x5b, 0x4c, 0x1c, 0xb4, 0x04,
	0x39, 0xd8, 0x35, 0xa7, 0x24, 0xe2, 0xfa, 0xe7 };

   uint8_t hash[256/8];
   skein256(hash, 256, data, 1024);
   cli_putstr_P(PSTR("\r\n hash(data) = "));
   cli_hexdump(hash, 32);
   hfal_hash_mem(&skein256_256_desc,hash, data, 1024);
   cli_putstr_P(PSTR("\r\n hash(data) = "));
   cli_hexdump(hash, 32);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/


const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char ztest_str[]       PROGMEM = "zerotest";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char monte2_str[]      PROGMEM = "monte2";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test2_str[] PROGMEM = "shavs_test2";
const char shavs_test3_str[] PROGMEM = "shavs_test3";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_skein            },
	{ performance_str,     NULL, performance_skein               },
	{ test_str,            NULL, testrun_stdtest_skein           },
	{ ztest_str,       (void*)1, (void_fpt)zeromsg_test_common   },
	{ shavs_list_str,      NULL, shavs_listalgos                 },
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo         },
	{ monte2_str,          NULL, test_monte2                     },
	{ shavs_test1_str,     NULL, shavs_test1                     },
	{ shavs_test2_str,     NULL, shavs_test2                     },
	{ shavs_test3_str,     NULL, shavs_test3                     },
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl             },
	{ NULL,                NULL, NULL                            }
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&skein256_256_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR("; "));
		cli_putstr(__DATE__);
		cli_putstr_P(PSTR(" "));
		cli_putstr(__TIME__);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));

		cmd_interface(cmdlist);
	}
}
