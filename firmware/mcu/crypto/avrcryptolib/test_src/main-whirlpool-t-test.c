/* main-whirlpool-t-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2011  Daniel Otte (daniel.otte@rub.de)

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
 * Whirlpool test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "whirlpool.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-performance.h"
#include "hfal-nessie.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "shavs.h"
#include "hfal_whirlpool_t.h"
#include "dump.h"

char* algo_name = "Whirlpool-T";

const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&whirlpool_t_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_whirlpool(void){
	hfal_nessie_multiple(algolist);
}

void testrun_whirlpool(void){
	whirlpool_ctx_t ctx;
	uint8_t hash[64];
	uint8_t data[64];
	memset(data, 0, 64);
	data[0] = 'a';
	data[1] = 'b';
	data[2] = 'c';
	whirlpool_init(&ctx);
	whirlpool_lastBlock(&ctx, data, 3*8);
	whirlpool_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nEmpty message hash:"));
	cli_hexdump_block(hash, 64, 4, 16);
}

void testrun_performance_whirlpool(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie2(void){
	uint16_t i;
	uint8_t j;
	uint8_t data[128], hash[64];
	whirlpool_ctx_t ctx;
	char str[8];
	memset(data, 0, 128);
	cli_putstr_P(PSTR("\r\nMessage digests of strings of 0-bits and length L:"));
	for(i=0; i<1024; ++i){
		cli_putstr_P(PSTR("\r\n    L = "));
		itoa(i, str, 10);
		j=4;
		j-= strlen(str);
		if(j>3){
			j=0;
		}
		while(j--){
			cli_putc(' ');
		}
		cli_putstr(str);
		cli_putstr_P(PSTR(": "));
		whirlpool_init(&ctx);
		whirlpool_lastBlock(&ctx, data, i);
		whirlpool_ctx2hash(hash, &ctx);
		cli_hexdump(hash, 64);
	}
	cli_putstr_P(PSTR("\r\nMessage digests of all 512-bit strings S containing a single 1-bit:"));
	for(i=0; i<512; ++i){
		cli_putstr_P(PSTR("\r\n    S = "));
		data[i/8] = 0x80>>(i&7);
		cli_hexdump(data, 64);
		cli_putstr_P(PSTR(": "));
		whirlpool_init(&ctx);
		whirlpool_lastBlock(&ctx, data, 512);
		whirlpool_ctx2hash(hash, &ctx);
		data[i/8] = 0x00;
		cli_hexdump(hash, 64);
	}
	cli_putstr_P(PSTR("\r\nIterated message digest computation (100000000 times): "));
	uint32_t c;
	memset(hash, 0, 64);
	for(c=0; c<1000000; ++c){
		whirlpool_init(&ctx);
		whirlpool_lastBlock(&ctx, hash, 512);
		whirlpool_ctx2hash(hash, &ctx);
	}
	cli_hexdump(hash, 64);
	cli_putstr_P(PSTR("\r\n/* finish generating testvectors */\r\n"));
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char nessie2_str[]     PROGMEM = "nessie2";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test2_str[] PROGMEM = "shavs_test2";
const char shavs_test3_str[] PROGMEM = "shavs_test3";
const char dump_str[]        PROGMEM = "dump";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_whirlpool       },
	{ nessie2_str,         NULL, testrun_nessie2                },
	{ test_str,            NULL, testrun_whirlpool              },
	{ performance_str,     NULL, testrun_performance_whirlpool  },
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl            },
	{ shavs_list_str,      NULL, shavs_listalgos                },
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo        },
	{ shavs_test1_str,     NULL, shavs_test1                    },
	{ shavs_test2_str,     NULL, shavs_test2                    },
	{ shavs_test3_str,     NULL, shavs_test3                    },
	{ dump_str,        (void*)1, (void_fpt)dump                 },
	{ NULL,                NULL, NULL                           }
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&whirlpool_t_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
