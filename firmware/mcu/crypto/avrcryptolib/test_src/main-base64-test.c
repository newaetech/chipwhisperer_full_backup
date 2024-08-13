/* main-base64-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
 * base64 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "noekeon.h"
#include "noekeon_prng.h"
#include "base64_enc.h"
#include "base64_dec.h"

#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Base64";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_stdtest_base64(void){
	uint8_t fail=0;
	uint8_t l,i; 
	int     sl;
	char str[10];
	uint8_t bin_buffer[256];
	char    b64_buffer[256*4/3+10];
	uint8_t bin_buffer2[256];
	random_seed(bin_buffer);
	
	for(l=0; l<255; ++l){
		cli_putstr_P(PSTR("\r\nTest "));
		utoa(l, str, 10);
		cli_putstr(str);
		
		for(i=0; i<l; ++i){
			bin_buffer[i] = random8();
		}
		cli_putstr_P(PSTR("\r\n bin: "));
		cli_hexdump(bin_buffer, l);
		
		base64enc(b64_buffer, bin_buffer, l);
		cli_putstr_P(PSTR("\r\n b64: "));
		cli_putstr(b64_buffer);
		
		sl = base64_binlength(b64_buffer, 1);
		
		if(sl!=l){
			cli_putstr_P(PSTR("\r\n ERROR length "));
			cli_putstr(str);
			if(sl!=-1){
				cli_putstr_P(PSTR(" != "));
				utoa(sl, str, 10);
				cli_putstr(str);
			}else{
				cli_putstr_P(PSTR(" != -1"));
			}
			fail=1;
		}else{
			cli_putstr_P(PSTR("\r\n length ok"));
		}
		base64dec(bin_buffer2, b64_buffer, 1);
		if(memcmp(bin_buffer, bin_buffer2, l)){
			cli_putstr_P(PSTR("\r\n ERROR value\r\n out: "));
			cli_hexdump(bin_buffer2, l);
			fail=1;
		}else{
			cli_putstr_P(PSTR("\r\n value ok"));
		}
		if(fail)
			break;
	}	
	cli_putstr_P(fail?PSTR("\r\n [FAIL]\r\n"):PSTR("\r\n [OK]\r\n"));
}

void testrun_performance_base64(void){
/*
	uint64_t t;
	char str[16];
	uint8_t key[16], data[16];
	noekeon_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(key,  0, 16);
	memset(data, 0, 16);
	
	startTimer(1);
	noekeon_init(key, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	noekeon_enc(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);	
	
	startTimer(1);
	noekeon_dec(data, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tdecrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	cli_putstr_P(PSTR("\r\n"));
*/
}
/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ test_str,        NULL, testrun_stdtest_base64},
	{ performance_str, NULL, testrun_performance_base64},
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
