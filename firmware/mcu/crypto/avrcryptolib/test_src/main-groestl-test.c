/* main-groestl-test.c */
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
 * groestl test-suit
 *
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "groestl_small.h"
#include "groestl_large.h"
#include "hfal_groestl_small.h"
#include "hfal_groestl_large.h"
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

char* algo_name = "Groestl";


const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&groestl224_desc,
	(hfdesc_t*)&groestl256_desc,
	(hfdesc_t*)&groestl384_desc,
	(hfdesc_t*)&groestl512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_groestl(void){
	hfal_nessie_multiple(algolist);
}
void groestl224_test(void* msg, uint32_t length_b){
	hfal_test(&groestl224_desc, msg, length_b);
}

void groestl256_test(void* msg, uint32_t length_b){
	hfal_test(&groestl256_desc, msg, length_b);
}

void groestl384_test(void* msg, uint32_t length_b){
	hfal_test(&groestl384_desc, msg, length_b);
}

void groestl512_test(void* msg, uint32_t length_b){
	hfal_test(&groestl512_desc, msg, length_b);
}


void testrun_stdtest_groestl(void){
	uint8_t msg1[144];
	memset(msg1, 0, 144);
	groestl224_test("", 8);
	groestl224_test(msg1, 576);
	groestl256_test("", 8);
	groestl256_test(msg1, 576);
	groestl384_test("", 8);
	groestl384_test(msg1, 1152);
	groestl512_test("", 8);
	groestl512_test(msg1, 1152);
}

void testshort(void){
	groestl256_test("abc", 24);
}

void testlshort(void){
	groestl512_test("abc", 24);
}

void test505(void){
	uint8_t data[] = {
		0x84, 0x73, 0xDC, 0x53, 0x82, 0xDE, 0x32, 0x95,
		0x7E, 0x3A, 0x15, 0xCA, 0x3D, 0x79, 0x1C, 0x67,
		0xD2, 0x0C, 0xF9, 0xEF, 0xBE, 0x3E, 0x46, 0x40,
		0x7D, 0xCA, 0x5D, 0x02, 0x63, 0x5A, 0xC8, 0x6D,
		0x2E, 0x0B, 0x22, 0xC7, 0x6D, 0x7D, 0x08, 0x0D,
		0x36, 0x2E, 0x82, 0x75, 0x89, 0x14, 0xCC, 0x0A,
		0xE2, 0xB8, 0x9B, 0xD3, 0x5F, 0x71, 0xD8, 0x44,
		0x92, 0xD9, 0x43, 0x07, 0x42, 0x78, 0x9C, 0x80 };
	groestl224_test(data, 505);
}


void performance_groestl(void){
	hfal_performance_multiple(algolist);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/


const char nessie_str[]       PROGMEM = "nessie";
const char test_str[]         PROGMEM = "test";
const char testshort_str[]    PROGMEM = "short";
const char testlshort_str[]   PROGMEM = "lshort";
const char test505_str[]      PROGMEM = "test505";
const char performance_str[]  PROGMEM = "performance";
const char echo_str[]         PROGMEM = "echo";
const char shavs_list_str[]   PROGMEM = "shavs_list";
const char shavs_set_str[]    PROGMEM = "shavs_set";
const char shavs_test1_str[]  PROGMEM = "shavs_test1";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_groestl},
	{ test_str,            NULL, testrun_stdtest_groestl},
	{ testshort_str,       NULL, testshort},
	{ testlshort_str,      NULL, testlshort},
	{ test505_str,         NULL, test505},
	{ performance_str,     NULL, performance_groestl},
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
	shavs_algo=(hfdesc_t*)&groestl256_desc;
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
