/* main-shacal1_enc-test.c */
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
 * Shacal1 encryption only test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "shacal1_enc.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* algo_name = "Shacal1 encryption only";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void shacal1_genctx_dummy(uint8_t* key, uint16_t keysize_b, void* ctx){
	memcpy(ctx, key, (keysize_b+7)/8);
}

void shacal1_enc_dummy(void* buffer, void* ctx){
	shacal1_enc(buffer, ctx, 512);
}

void testrun_nessie_shacal1enc(void){
	nessie_bc_ctx.blocksize_B = SHACAL1_BLOCKSIZE_B;
	nessie_bc_ctx.keysize_b   = SHACAL1_KEYSIZE;
	nessie_bc_ctx.name        = algo_name;
	nessie_bc_ctx.ctx_size_B  = SHACAL1_KEYSIZE_B;
	nessie_bc_ctx.cipher_enc  = (nessie_bc_enc_fpt)shacal1_enc_dummy;
	nessie_bc_ctx.cipher_dec  = (nessie_bc_dec_fpt)NULL;
	nessie_bc_ctx.cipher_genctx  = (nessie_bc_gen_fpt)shacal1_genctx_dummy;
	
	nessie_bc_run();	
}

void testrun_performance_shacal1enc(void){
	uint64_t t;
	uint8_t key[SHACAL1_KEYSIZE_B], data[SHACAL1_BLOCKSIZE_B];
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, SHACAL1_KEYSIZE_B);
	memset(data, 0, SHACAL1_BLOCKSIZE_B);
	
	
	startTimer(1);
	shacal1_enc(data, key, SHACAL1_KEYSIZE);
	t = stopTimer();
	print_time_P(PSTR("\tencrypt time: "), t);
	
	cli_putstr_P(PSTR("\r\n"));
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_shacal1enc},
	{ test_str,        NULL, testrun_nessie_shacal1enc},
	{ performance_str, NULL, testrun_performance_shacal1enc},
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
