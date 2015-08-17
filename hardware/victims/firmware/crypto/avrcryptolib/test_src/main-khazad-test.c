/* main-khazad-test.c */
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
 * khazad test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "khazad.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_khazad.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Khazad";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&khazad_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_khazad(void){
	bcal_nessie(&khazad_desc);
}

void testrun_performance_khazad(void){
	bcal_performance_multiple(algolist);
}

void test_khazad(void){
	uint8_t key[16];
	uint8_t data[8];
	khazad_ctx_t ctx;

	memset(key, 0, 16);
	memset(data, 0, 8);
	key[0] = 0x80;
	cli_putstr_P(PSTR("\r\nkey:   "));
	cli_hexdump(key, 16);
	khazad_init(key, &ctx);
	cli_putstr_P(PSTR("\r\nround keys:"));
	cli_hexdump_block(&ctx, 8*8, 4, 8);
	cli_putstr_P(PSTR("\r\nplain:  "));
	cli_hexdump(data, 8);
	khazad_enc(data, &ctx);
	cli_putstr_P(PSTR("\r\nencrypt:"));
	cli_hexdump(data, 8);
	khazad_dec(data, &ctx);
	cli_putstr_P(PSTR("\r\ndecrypt:"));
	cli_hexdump(data, 8);
}

void test_sbox(void){
	uint8_t i=0,x;
	cli_putstr_P(PSTR("\r\nKhazad Sbox:\r\n\t"));
	do{
		x = khazad_sbox(i);
		cli_hexdump_byte(x);
		cli_putc(' ');
		if(i%16==15){
			cli_putstr_P(PSTR("\r\n\t"));
		}
		++i;
	}while(i);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char test_sbox_str[]   PROGMEM = "test_sbox";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_khazad},
	{ test_str,        NULL, test_khazad},
	{ test_sbox_str,   NULL, test_sbox},
	{ performance_str, NULL, testrun_performance_khazad},
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
