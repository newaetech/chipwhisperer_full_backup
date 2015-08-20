/* main-a5_1-test.c */
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
 * A5/1 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include <A5_1.h>
#include "nessie_stream_test.h"

#include <stdint.h>
#include <string.h>
#include "cli.h"

char* algo_name = "A5_1";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void a51_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	a5_1_init(ctx,key, keysize, NULL, 0);
}



void testrun_nessie_a51(void){
	nessie_stream_ctx.outsize_b =   8; /* actually unused */
	nessie_stream_ctx.keysize_b =  64; 
	nessie_stream_ctx.ivsize_b =   64;
	nessie_stream_ctx.name = algo_name;
	nessie_stream_ctx.ctx_size_B = sizeof(a5_1_ctx_t);
	nessie_stream_ctx.cipher_genctx = (nessie_stream_genctx_fpt)a51_genctx_dummy;
	nessie_stream_ctx.cipher_enc = (nessie_stream_genenc_fpt)a5_1_gen;
	
	nessie_stream_run();	
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char testkey_str[]     PROGMEM = "testkey";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_a51, NULL},
/*	{ performance_str, NULL, testrun_performance_a51, NULL}, */
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl, NULL},
	{ NULL,            NULL, NULL, NULL}
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

