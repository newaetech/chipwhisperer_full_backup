/* main-jh-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2010 Daniel Otte (daniel.otte@rub.de)

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
 * JH test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "jh_simple.h"
#include "hfal_jh.h"
#include "hfal-nessie.h"
#include "hfal-test.h"
#include "hfal-performance.h"
#include "shavs.h"
#include "cli.h"
#include "nessie_hash_test.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "JH";


const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&jh224_desc,
	(hfdesc_t*)&jh256_desc,
	(hfdesc_t*)&jh384_desc,
	(hfdesc_t*)&jh512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void performance_jh(void){
	hfal_performance_multiple(algolist);
}

void testrun_nessie_jh(void){
	hfal_nessie_multiple(algolist);
}

void test256Null(void){
	jh_ctx_t ctx;
	uint8_t hash[32];
	jh256_init(&ctx);
	jh_lastBlock(&ctx, NULL, 0);
	jh256_ctx2hash(hash, &ctx);
	cli_putstr_P(PSTR("\r\nresult:\r\n"));
	cli_hexdump_block(hash, 32, 4, 8);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/


const char nessie_str[]       PROGMEM = "nessie";
const char test_str[]         PROGMEM = "test";
const char test256_str[]      PROGMEM = "test256";
const char performance_str[]  PROGMEM = "performance";
const char singleround_str[]  PROGMEM = "singleround";
const char echo_str[]         PROGMEM = "echo";
const char shavs_list_str[]   PROGMEM = "shavs_list";
const char shavs_set_str[]    PROGMEM = "shavs_set";
const char shavs_test1_str[]  PROGMEM = "shavs_test1";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_jh},
	{ performance_str,     NULL, performance_jh},
	{ test256_str,         NULL, test256Null},
//	{ singleround_str,     NULL, singleround_jh},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ shavs_test1_str,     NULL, shavs_test1},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&jh256_desc;
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
