/* main-mugi-test.c */
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
 * MUGI test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "mugi.h"
#include "nessie_stream_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* algo_name = "MUGI";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_performance_mugi(void){
	uint64_t t;
	char str[16];
	uint8_t key[16];
	uint8_t iv[16];
	mugi_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();	
	
	memset(key, 0, 16);
	memset(iv,  0, 16);
	
	startTimer(1);
	mugi_init(key, iv, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	mugi_gen(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	cli_putstr_P(PSTR("\r\n"));	
}


void testrun_mugi(void){
	uint8_t key[]={  0x00, 0x01, 0x02, 0x03,
	                 0x04, 0x05, 0x06, 0x07,
					 0x08, 0x09, 0x0a, 0x0b,
					 0x0c, 0x0d, 0x0e, 0x0f };
	uint8_t iv[]= {  0xf0, 0xe0, 0xd0, 0xc0,
	                 0xb0, 0xa0, 0x90, 0x80,
					 0x70, 0x60, 0x50, 0x40,
					 0x30, 0x20, 0x10, 0x00 };
	mugi_ctx_t ctx;
	uint64_t output;
	uint8_t i;
	mugi_init(key, iv, &ctx);
	cli_putstr_P(PSTR("\r\nkey = "));
	cli_hexdump2(key, 16);
	cli_putstr_P(PSTR("\r\niv  = "));
	cli_hexdump2(iv, 16);
	for(i=0; i<8; ++i){
		output = mugi_gen(&ctx);
		cli_putstr_P(PSTR("\r\nMUGI output: "));
		cli_hexdump(&output, 8);				 
	}                
	
	memset(key, 0, 16);
	memset(iv,  0, 16);
	mugi_init(key, iv, &ctx);
	cli_putstr_P(PSTR("\r\nkey = "));
	cli_hexdump2(key, 16);
	cli_putstr_P(PSTR("\r\niv  = "));
	cli_hexdump2(iv, 16);
	for(i=0; i<8; ++i){
		output = mugi_gen(&ctx);
		cli_putstr_P(PSTR("\r\nMUGI output: "));
		cli_hexdump(&output, 8);				 
	}                
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
//	{ nessie_str,      NULL, testrun_nessie_arcfour },
	{ test_str,        NULL, testrun_mugi},
	{ performance_str, NULL, testrun_performance_mugi},
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

