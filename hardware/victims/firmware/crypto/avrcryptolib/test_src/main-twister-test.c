/* main-twister-test.c */
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
 * twister test suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "twister-small.h"
#include "twister-large.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal_twister224.h"
#include "hfal_twister256.h"
#include "hfal_twister384.h"
#include "hfal_twister512.h"
#include "hfal-nessie.h"
#include "hfal-performance.h"
#include "hfal-test.h"
#include "shavs.h"


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"

char* algo_name = "TWISTER";

const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&twister224_desc,
	(hfdesc_t*)&twister256_desc,
	(hfdesc_t*)&twister384_desc,
	(hfdesc_t*)&twister512_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_nessie_twister(void){
	hfal_nessie_multiple(algolist);
}

/*****************************************************************************
 * selftests
 *
 *****************************************************************************/

void test_twister224( void* msg, uint32_t length_b){
	hfal_test(&twister224_desc, msg, length_b);
}

void test_twister256( void* msg, uint32_t length_b){
	hfal_test(&twister256_desc, msg, length_b);
}

void test_twister384( void* msg, uint32_t length_b){
	hfal_test(&twister384_desc, msg, length_b);
}

void test_twister512( void* msg, uint32_t length_b){
	hfal_test(&twister512_desc, msg, length_b);
}

void testrun_twister(void){
	const hfdesc_t* desc[4] = { &twister224_desc, &twister256_desc,
	                            &twister384_desc, &twister512_desc };
	uint8_t i,j; 
	char* testv[]={
		"", 
		"a", 
		"abc", 
		"message digest", 
		"abcdefghijklmnopqrstuvwxyz", 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 
		"12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
	uint8_t stestv[]= {0x00, 0x00, 0xC0, 0xC0, 0x80, 0x48, 0x50};
	uint8_t stestl[]= {   0,    1,    2,    3,    4,    5,    6};	
	
	for(i=0; i<4; ++i){
		for(j=0; j<7; ++j){
			hfal_test(desc[i], testv[j], strlen(testv[j])*8);
		}
		hfal_test(desc[i], stestv, 7*8);
		hfal_test(desc[i], stestl, 7*8);
		
	}
}


void testrun_performance_twister(void){
	hfal_performance_multiple(algolist);
}


/*****************************************************************************
 * main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";
const char shavs_list_str[]  PROGMEM = "shavs_list";
const char shavs_set_str[]   PROGMEM = "shavs_set";
const char shavs_test1_str[] PROGMEM = "shavs_test1";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,          NULL, testrun_nessie_twister},
	{ test_str,            NULL, testrun_twister},
	{ performance_str,     NULL, testrun_performance_twister},
	{ echo_str,        (void*)1, (void_fpt)echo_ctrl},
	{ shavs_list_str,      NULL, shavs_listalgos},
	{ shavs_set_str,   (void*)1, (void_fpt)shavs_setalgo},
	{ shavs_test1_str,     NULL, shavs_test1},
	{ NULL,                NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	shavs_algolist=(hfdesc_t**)algolist;
	shavs_algo=(hfdesc_t*)&twister256_desc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
