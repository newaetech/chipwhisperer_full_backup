/* main-xtea-test.c */
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
 * XTEA test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "xtea.h"
#include "nessie_bc_test.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_xtea.h"
#include "cli.h"

#include <stdint.h>
#include <string.h>

char* algo_name = "XTEA";

const bcdesc_t* const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const const algolist[] PROGMEM = {
	(bcdesc_t*)&xtea_desc,
	NULL
};

/******************************************************************************/

void testrun_nessie_xtea(void){
	bcal_nessie_multiple(algolist);
}

void testrun_performance_xtea(void){
	bcal_performance_multiple(algolist);
}

void test_xtea(void){
	uint8_t key[16];
	uint8_t data[8];

	memset(key, 0, 16);
	key[0] = 0x80;
	memset(data, 0, 8);
	cli_putstr_P(PSTR("\r\n*** XTEA test ***\r\n key:   "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n plain: "));
	cli_hexdump(data, 8);
	xtea_enc(data, data, key);
	cli_putstr_P(PSTR("\r\n crypt: "));
	cli_hexdump(data, 8);
	xtea_dec(data, data, key);
	cli_putstr_P(PSTR("\r\n plain: "));
	cli_hexdump(data, 8);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_xtea},
	{ test_str,        NULL, test_xtea},
	{ performance_str, NULL, testrun_performance_xtea},
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
