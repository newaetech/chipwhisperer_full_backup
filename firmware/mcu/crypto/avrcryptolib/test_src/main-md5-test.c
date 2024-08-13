/* main-md5-test.c */
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
 * md5 test suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "md5.h"
#include "nessie_hash_test.h"
#include "performance_test.h"
#include "hfal_md5.h"
#include "hfal-performance.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"

char* algo_name = "MD5";

const hfdesc_t* const algolist[] PROGMEM = {
	(hfdesc_t*)&md5_desc,
	NULL
};

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void md5_ctx2hash_dummy(void* buffer, void* ctx){
	memcpy(buffer, ctx, 16);
}


void testrun_nessie_md5(void){
	nessie_hash_ctx.hashsize_b  = 128;
	nessie_hash_ctx.blocksize_B = 512/8;
	nessie_hash_ctx.ctx_size_B  = sizeof(md5_ctx_t);
	nessie_hash_ctx.name = algo_name;
	nessie_hash_ctx.hash_init = (nessie_hash_init_fpt)md5_init;
	nessie_hash_ctx.hash_next = (nessie_hash_next_fpt)md5_nextBlock;
	nessie_hash_ctx.hash_last = (nessie_hash_last_fpt)md5_lastBlock;
	nessie_hash_ctx.hash_conv = (nessie_hash_conv_fpt)md5_ctx2hash_dummy;
	
	nessie_hash_run();
}

/*****************************************************************************
 *  self tests																 *
 *****************************************************************************/

/*
 * MD5 test suite:
 * MD5 ("") = d41d8cd98f00b204e9800998ecf8427e
 * MD5 ("a") = 0cc175b9c0f1b6a831c399e269772661
 * MD5 ("abc") = 900150983cd24fb0d6963f7d28e17f72
 * MD5 ("message digest") = f96b697d7cb7938d525a2f31aaf161d0
 * MD5 ("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
 * MD5 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
 * d174ab98d277d9f5a5611c2c9f419d9f
 * MD5 ("123456789012345678901234567890123456789012345678901234567890123456
 * 78901234567890") = 57edf4a22be3c955ac49da2e2107b67a
 */

void testrun_md5(void){
	md5_hash_t hash;
	char* testv[]={
		"", 
		"a", 
		"abc", 
		"message digest", 
		"abcdefghijklmnopqrstuvwxyz", 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 
		"12345678901234567890123456789012345678901234567890123456789012345678901234567890"};
	uint16_t i;
	
	cli_putstr("\r\n=== MD5 test suit ===");
	for(i=0; i<7; ++i){
		cli_putstr("\r\n MD5 (\"");
		cli_putstr(testv[i]);
		cli_putstr("\") = \r\n\t");
		md5(&hash, testv[i], strlen(testv[i])*8);
		cli_hexdump(hash, 16);
	}
	uint8_t buffer[512/8];
	char str[5];
	for(i=505; i<512; ++i){
		memset(buffer, 0, 512/8);
		cli_putstr_P(PSTR("\r\nMD5("));
		utoa(i, str, 10);
		cli_putstr(str);
		cli_putstr_P(PSTR(" zero bits) = "));
		md5(&hash, buffer, i);
		cli_hexdump(hash, 16);
	}
}


void testrun_performance_md5(void){
	hfal_performance_multiple(algolist);
}


/*****************************************************************************
 * main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_md5},
	{ test_str,        NULL, testrun_md5},
	{ performance_str, NULL, testrun_performance_md5},
	{ echo_str,    (void*)1, (void_fpt)echo_ctrl},
	{ NULL,            NULL, NULL}
};

int main (void){
	DEBUG_INIT();
	
	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;	 	
	testrun_md5();
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));
		cmd_interface(cmdlist);
	}
}
