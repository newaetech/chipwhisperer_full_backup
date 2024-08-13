/* main-sha512-test.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2011  Daniel Otte (daniel.otte@rub.de)

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
 * SHA-512 test-suit
 *
*/

#include "main-test-common.h"
#include <avr/pgmspace.h>
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-nessie.h"
#include "hfal-performance.h"
#include "hfal-test.h"


#include "sha224.h"
#include "hfal_sha224.h"
#include "sha256.h"
#include "hfal_sha256.h"
#include "sha384.h"
#include "hfal_sha384.h"
#include "sha512.h"
#include "hfal_sha512.h"

const char* algo_name = "SHA-2";

const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&sha224_desc,
	(hfdesc_t*)&sha256_desc,
	(hfdesc_t*)&sha384_desc,
	(hfdesc_t*)&sha512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_sha2(void){
	hfal_nessie_multiple(algolist);
}

void testrun_performance_sha2(void){
	hfal_performance_multiple(algolist);
}

void simple_test(void){
	const char *msg = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
			          "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";

	uint8_t hash[64];
	sha512_ctx_t ctx;
	cli_putstr_P(PSTR("\r\nDBG: init ..."));
	sha512_init(&ctx);
	cli_putstr_P(PSTR("\r\nDBG: init done"));
	sha512_lastBlock(&ctx, msg, 3*8);
	cli_putstr_P(PSTR("\r\nDBG: lastBlock done"));
	sha512_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\n hash = "));
	cli_hexdump(hash, 64);


	cli_putstr_P(PSTR("\r\nDBG: init ..."));
	sha512_init(&ctx);
	cli_putstr_P(PSTR("\r\nDBG: init done"));
	sha512_lastBlock(&ctx, msg, 896);
	cli_putstr_P(PSTR("\r\nDBG: lastBlock done"));
	sha512_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\n hash = "));
	cli_hexdump(hash, 64);

	uint32_t c=0;
	uint8_t buffer[128];
	memset(buffer, 'a', 128);
	cli_putstr_P(PSTR("\r\nDBG: init ..."));
	sha512_init(&ctx);
	cli_putstr_P(PSTR("\r\nDBG: init done"));
	do{
		sha512_nextBlock(&ctx, buffer);
		c += 128;
	}while(c+128<1000000L);
	sha512_lastBlock(&ctx, buffer, (1000000-c)*8);
	cli_putstr_P(PSTR("\r\nDBG: lastBlock done"));
	sha512_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\n hash = "));
	cli_hexdump(hash, 64);


}

/*
void test_monte(void){
	uint8_t data1[] = {
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7 };

   uint8_t data2[] = {
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xF4, 0x1E, 0xCE, 0x26, 0x13, 0xE4, 0x57, 0x39,
     0x15, 0x69, 0x6B, 0x5A, 0xDC, 0xD5, 0x1C, 0xA3,
	 0x28, 0xBE, 0x3B, 0xF5, 0x66, 0xA9, 0xCA, 0x99,
	 0xC9, 0xCE, 0xB0, 0x27, 0x9C, 0x1C, 0xB0, 0xA7,
     0xFD, 0xDF, 0x1B, 0x37, 0xDD, 0x34, 0xB3, 0xB2,
     0x01, 0xD4, 0x3C, 0x57, 0xBC, 0xDE, 0x11, 0x58,
     0x38, 0xF0, 0xDF, 0x70, 0x1D, 0xA9, 0x3C, 0x3B,
     0xF2, 0xC9, 0xC8, 0x68, 0x96, 0xE7, 0xE6, 0xC7 };
   uint8_t hash[SHA256_HASH_BYTES];
   sha256((sha256_hash_t*)hash, data1, 3*32*8);
   cli_putstr_P(PSTR("\r\n hash(data1) = "));
   cli_hexdump(hash, 32);
   sha256((sha256_hash_t*)hash, data2, 3*32*8);
   cli_putstr_P(PSTR("\r\n hash(data2) = "));
   cli_hexdump(hash, 32);
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

   uint8_t hash[SHA256_HASH_BYTES];
   sha256((sha256_hash_t*)hash, data, 1024);
   cli_putstr_P(PSTR("\r\n hash(data) = "));
   cli_hexdump(hash, 32);
}
*/
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
//const char monte_str[]     PROGMEM   = "monte";
//const char monte2_str[]    PROGMEM   = "monte2";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test2_str[] PROGMEM = "shavs_test2";
const char shavs_test3_str[] PROGMEM = "shavs_test3";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_sha2          },
	{ test_str,            NULL, simple_test                    },
//	{ monte_str,           NULL, test_monte                     },
//	{ monte2_str,          NULL, test_monte2                    },
	{ performance_str,     NULL, testrun_performance_sha2     },
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl            },
	{ shavs_list_str,      NULL, shavs_listalgos                },
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo        },
	{ shavs_test1_str,     NULL, shavs_test1                    },
	{ shavs_test2_str,     NULL, shavs_test2                    },
	{ shavs_test3_str,     NULL, shavs_test3                    },
	{ NULL,                NULL, NULL                           }
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
