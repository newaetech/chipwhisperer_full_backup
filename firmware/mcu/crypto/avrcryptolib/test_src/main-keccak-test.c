/* main-keccak-test.c */
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
 * Keccak test-suit
 *
*/

#include "config.h"
#include "uart_i.h"
#include "debug.h"

#include "keccak.h"
#include "cli.h"
#include "hfal_keccak.h"
#include "shavs.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal-nessie.h"
#include "hfal-performance.h"
#include "hfal-test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Keccak";


const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&keccak224_desc,
	(hfdesc_t*)&keccak256_desc,
	(hfdesc_t*)&keccak384_desc,
	(hfdesc_t*)&keccak512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void test_256(void){
	uint8_t hash[32];
	uint8_t null[KECCAK256_BLOCKSIZE_B];
	memset(null, 0, KECCAK256_BLOCKSIZE_B);
	keccak_ctx_t ctx;
	keccak256_init(&ctx);
	keccak_nextBlock(&ctx, null);
	//	keccak_lastBlock(&ctx, data, 29);
	keccak256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\n testhash: "));
	cli_hexdump(hash, 32);
}


void performance_keccak(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie_keccak(void){
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
	{ nessie_str,                NULL, testrun_nessie_keccak     },
	{ performance_str,           NULL, performance_keccak        },
	{ test256_str,               NULL, test_256                  },
	{ shavs_list_str,            NULL, shavs_listalgos           },
	{ shavs_set_str,         (void*)1, (void_fpt)shavs_setalgo   },
	{ shavs_test1_str,           NULL, shavs_test1               },
	{ shavs_test3_str,           NULL, shavs_test3               },
	{ echo_str,              (void*)1, (void_fpt)echo_ctrl       },
	{ NULL,                      NULL, NULL                      }
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&keccak256_desc;
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
