/* main-cubehash-test.c */
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
 * CubeHash test-suit
 *
*/

#include "config.h"
#include "uart_i.h"
#include "debug.h"

#include "cubehash.h"
#include "cli.h"
#include "hfal_cubehash.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-nessie.h"
#include "hfal-performance.h"
#include "hfal-test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "CubeHash";


const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&cubehash224_desc,
	(hfdesc_t*)&cubehash256_desc,
	(hfdesc_t*)&cubehash384_desc,
	(hfdesc_t*)&cubehash512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void cubehash256_test0(void){
	cubehash_ctx_t ctx;
	uint8_t hash[32];
	cubehash256_init(&ctx);
	cli_putstr_P(PSTR("\r\ninit done "));
	cubehash_lastBlock(&ctx, NULL, 0);
	cli_putstr_P(PSTR("\r\nlastblock done "));
	cubehash256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nhash = "));
	cli_hexdump(hash, 32);
}

void performance_cubehash(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie_cubehash(void){
	hfal_nessie_multiple(algolist);
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test256_str[]     PROGMEM = "test256";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";
const char shavs_test3_str[] PROGMEM = "shavs_test3";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,                NULL, testrun_nessie_cubehash     },
	{ test256_str,               NULL, cubehash256_test0           },
	{ performance_str,           NULL, performance_cubehash        },
	{ shavs_list_str,            NULL, shavs_listalgos             },
	{ shavs_set_str,         (void*)1, (void_fpt)shavs_setalgo     },
	{ shavs_test1_str,           NULL, shavs_test1                 },
	{ shavs_test3_str,           NULL, shavs_test3                 },
	{ echo_str,              (void*)1, (void_fpt)echo_ctrl         },
	{ NULL,                      NULL, NULL                        }
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&cubehash256_desc;
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
