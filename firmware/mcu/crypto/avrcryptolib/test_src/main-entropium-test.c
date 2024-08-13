/* main-entropium-test.c */
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
 * entropium test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "entropium.h"
#include "nessie_bc_test.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Entropium";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_entropium(void){
	char c, str[16];
	uint8_t data[32];
	uint32_t i=0;
	while('q'!=cli_getc()){
		entropium_getRandomBlock(data);
		cli_putstr_P(PSTR("\r\n "));
		ultoa(i, str, 10);
		for(c=strlen(str); c<11; ++c){
			cli_putc(' ');
		}
		cli_putstr(str);
		++i;
		cli_putstr_P(PSTR(" : "));
		cli_hexdump(data, 32);
	}
	cli_putstr_P(PSTR("\r\n\r\n"));
}


void testrun_performance_entropium(void){
	uint64_t t;
	char str[16];
	uint8_t data[32];
	
	calibrateTimer();
	print_overhead();
	
	startTimer(1);
	entropium_addEntropy(128, data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tadd entropy time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	entropium_getRandomBlock(data);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tget random time:  "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
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
	{ nessie_str,      NULL, testrun_entropium},
	{ test_str,        NULL, testrun_entropium},
	{ performance_str, NULL, testrun_performance_entropium},
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
