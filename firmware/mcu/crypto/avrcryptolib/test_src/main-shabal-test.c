/* main-shabal-test.c */
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
 * shabal test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "shabal.h"
#include "cli.h"
#include "hfal_shabal.h"
#include "hfal-test.h"
#include "hfal-nessie.h"
#include "hfal-performance.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Shabal";


const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&shabal192_desc,
	(hfdesc_t*)&shabal224_desc,
	(hfdesc_t*)&shabal256_desc,
	(hfdesc_t*)&shabal384_desc,
	(hfdesc_t*)&shabal512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void testrun_stdtest_shabal192(void* msg, uint16_t size_b){
	hfal_test(&shabal192_desc, msg, size_b);
}

void testrun_stdtest_shabal224(void* msg, uint16_t size_b){
	hfal_test(&shabal224_desc, msg, size_b);
}

void testrun_stdtest_shabal256(void* msg, uint16_t size_b){
	hfal_test(&shabal256_desc, msg, size_b);
}

void testrun_stdtest_shabal384(void* msg, uint16_t size_b){
	hfal_test(&shabal384_desc, msg, size_b);
}

void testrun_stdtest_shabal512(void* msg, uint16_t size_b){
	hfal_test(&shabal512_desc, msg, size_b);
}

void testrun_stdtest_shabal(void){
	uint8_t ma[64];
	char*   mb= "abcdefghijklmnopqrstuvwxyz-"
	            "0123456789-"
			    "ABCDEFGHIJKLMNOPQRSTUVWXYZ-"
                "0123456789-"
				"abcdefghijklmnopqrstuvwxyz";

	memset(ma, 0, 64);
	testrun_stdtest_shabal192(ma, 64*8);
	testrun_stdtest_shabal192(mb, strlen(mb)*8);
	testrun_stdtest_shabal224(ma, 64*8);
	testrun_stdtest_shabal224(mb, strlen(mb)*8);
	testrun_stdtest_shabal256(ma, 64*8);
	testrun_stdtest_shabal256(mb, strlen(mb)*8);
	testrun_stdtest_shabal384(ma, 64*8);
	testrun_stdtest_shabal384(mb, strlen(mb)*8);
	testrun_stdtest_shabal512(ma, 64*8);
	testrun_stdtest_shabal512(mb, strlen(mb)*8);
}

void testshort(void){
	uint8_t ma[64];
	memset(ma, 0, 64);
	testrun_stdtest_shabal192(ma, 64*8);
}

void shabal_ctx_dump(shabal_ctx_t* ctx){
	uint8_t i;
	void* p;
	cli_putstr_P(PSTR("\r\n=== shabal ctx dump ===\r\n  size = "));
	i=sizeof(shabal_ctx_t);
	if(i>=100)
		cli_putc('0'+i/100);
	if(i>=10)
		cli_putc('0'+(i/10)%10);
	cli_putc('0'+i%10);
	cli_putstr_P(PSTR("\r\n  a = "));
	cli_hexdump_block(ctx->a, 12*4, 5, 4*8);
	cli_putstr_P(PSTR("\r\n  b_buffer = "));
	cli_hexdump_block(ctx->b_buffer, 12*4, 5, 4*8);
	cli_putstr_P(PSTR("\r\n  c_buffer = "));
	cli_hexdump_block(ctx->c_buffer, 12*4, 5, 4*8);
	if(ctx->b == &(ctx->b_buffer[0]))
		cli_putstr_P(PSTR("\r\nb --> b_buffer"));
	if(ctx->b == &(ctx->c_buffer[0]))
		cli_putstr_P(PSTR("\r\nb --> c_buffer"));
	if(ctx->c == &(ctx->b_buffer[0]))
		cli_putstr_P(PSTR("\r\nc --> b_buffer"));
	if(ctx->c == &(ctx->c_buffer[0]))
		cli_putstr_P(PSTR("\r\nc --> c_buffer"));
	cli_putstr_P(PSTR("\r\n b = "));
	cli_hexdump(&(ctx->b), 2);
	p = ctx->b_buffer;
	cli_putstr_P(PSTR("\r\n b (should) = "));
	cli_hexdump(&p, 2);
	cli_putstr_P(PSTR("\r\n c = "));
	cli_hexdump(&(ctx->c), 2);
	p = ctx->c_buffer;
	cli_putstr_P(PSTR("\r\n c (should) = "));
	cli_hexdump(&p, 2);
}


void testinit_192(void){
	shabal_ctx_t ctx;
	shabal192_init(&ctx);
	shabal_ctx_dump(&ctx);
}

void testinit_224(void){
	shabal_ctx_t ctx;
	shabal224_init(&ctx);
	shabal_ctx_dump(&ctx);
}

void testinit_256(void){
	shabal_ctx_t ctx;
	shabal256_init(&ctx);
	shabal_ctx_dump(&ctx);
}

void testinit_384(void){
	shabal_ctx_t ctx;
	shabal384_init(&ctx);
	shabal_ctx_dump(&ctx);
}

void testinit_512(void){
	shabal_ctx_t ctx;
	shabal512_init(&ctx);
	shabal_ctx_dump(&ctx);
}
void testinit(void){
	testinit_192();
	testinit_224();
	testinit_256();
	testinit_384();
	testinit_512();
}

void performance_shabal(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie_shabal(void){
	hfal_nessie_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/


const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testinit192_str[] PROGMEM = "testinit192";
const char testinit_str[]    PROGMEM = "testinit";
const char testshort_str[]   PROGMEM = "short";
const char ztest_str[]       PROGMEM = "zerotest";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test2_str[] PROGMEM = "shavs_test2";
const char shavs_test3_str[] PROGMEM = "shavs_test3";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_shabal          },
	{ test_str,            NULL, testrun_stdtest_shabal         },
	{ testinit192_str,     NULL, testinit_192                   },
	{ testinit_str,        NULL, testinit                       },
	{ testshort_str,       NULL, testshort                      },
	{ performance_str,     NULL, performance_shabal             },
	{ shavs_list_str,      NULL, shavs_listalgos                },
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo        },
	{ shavs_test1_str,     NULL, shavs_test1                    },
	{ shavs_test2_str,     NULL, shavs_test2                    },
	{ shavs_test3_str,     NULL, shavs_test3                    },
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl            },
	{ NULL,                NULL, NULL                           }
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&shabal256_desc;
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
