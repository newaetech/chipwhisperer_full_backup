/* main-serpent-test.c */
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
 * serpent test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "serpent.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_serpent.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Serpent";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&serpent_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void serpent_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	serpent_init(key, keysize&0xff, ctx);
}

void testrun_nessie_serpent(void){
	bcal_nessie_multiple(algolist);
}

void testrun_test_serpent(void){
	uint8_t key[32];
	serpent_ctx_t ctx;
	uint8_t i;
	memset(key, 0, 16);
	serpent_init(key, 128, &ctx);
	for(i=0; i<33; ++i){
		cli_putstr_P(PSTR("\r\n subkekey "));	
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR(" : "));	
		cli_hexdump(ctx.k[i], 16);
	}
}

void testrun_performance_serpent(void){
	bcal_performance_multiple(algolist);
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_serpent},
	{ test_str,        NULL, testrun_test_serpent},
	{ performance_str, NULL, testrun_performance_serpent},
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
