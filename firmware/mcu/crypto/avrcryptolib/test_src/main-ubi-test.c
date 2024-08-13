/* main-threefish-test.c */
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
 * threefish test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "ubi.h"
#include "cli.h"
#include "performance_test.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "UBI-Threefish";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
void testrun_stdtest_ubi256(uint16_t outsize){
	ubi256_ctx_t ctx;
	skein_config_t conf;
	uint64_t iv[4];
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for UBI (256 bits):"));
	memset(&conf, 0, sizeof(skein_config_t));

	ubi256_init(&ctx, &conf, UBI_TYPE_CFG);
	conf.schema[0] = 'S';
	conf.schema[1] = 'H';
	conf.schema[2] = 'A';
	conf.schema[3] = '3';
	conf.version = 1;
	conf.out_length = outsize;
	ubi256_lastBlock(&ctx, &conf, 256);
	ubi256_ctx2hash(iv, &ctx);
		
	cli_putstr_P(PSTR("\r\nIV: "));
	cli_hexdump_rev(&(iv[0]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[1]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[2]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[3]), 8);
}

void testrun_stdtest_ubi512(uint16_t outsize){
	ubi512_ctx_t ctx;
	skein_config_t conf;
	uint64_t iv[8];
	uint8_t null[UBI512_BLOCKSIZE_B];
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for UBI (512 bits):"));
	memset(&conf, 0, sizeof(skein_config_t));
	memset(null, 0, UBI512_BLOCKSIZE_B);
	ubi512_init(&ctx, null, UBI_TYPE_CFG);
	conf.schema[0] = 'S';
	conf.schema[1] = 'H';
	conf.schema[2] = 'A';
	conf.schema[3] = '3';
	conf.version = 1;
	conf.out_length = outsize;
	ubi512_lastBlock(&ctx, &conf, 256);
	ubi512_ctx2hash(iv, &ctx);
		
	cli_putstr_P(PSTR("\r\nIV: "));
	cli_hexdump_rev(&(iv[0]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[1]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[2]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[3]), 8);
	cli_putstr_P(PSTR("\r\n    "));
	cli_hexdump_rev(&(iv[4]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[5]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[6]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[7]), 8);
}

void testrun_stdtest_ubi1024(uint16_t outsize){
	ubi1024_ctx_t ctx;
	skein_config_t conf;
	uint64_t iv[16];
	uint8_t null[UBI1024_BLOCKSIZE_B];
	
	cli_putstr_P(PSTR("\r\n\r\nTest vectors for UBI (1024 bits):"));
	memset(&conf, 0, sizeof(skein_config_t));

	memset(null, 0, UBI1024_BLOCKSIZE_B);
	ubi1024_init(&ctx, null, UBI_TYPE_CFG);
	conf.schema[0] = 'S';
	conf.schema[1] = 'H';
	conf.schema[2] = 'A';
	conf.schema[3] = '3';
	conf.version = 1;
	conf.out_length = outsize;
	ubi1024_lastBlock(&ctx, &conf, 256);
	ubi1024_ctx2hash(iv, &ctx);
		
	cli_putstr_P(PSTR("\r\nIV: "));
	cli_hexdump_rev(&(iv[0]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[1]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[2]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[3]), 8);
	cli_putstr_P(PSTR("\r\n    "));
	cli_hexdump_rev(&(iv[4]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[5]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[6]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[7]), 8);
	cli_putstr_P(PSTR("\r\n    "));
	cli_hexdump_rev(&(iv[8]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[9]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[10]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[11]), 8);
	cli_putstr_P(PSTR("\r\n    "));
	cli_hexdump_rev(&(iv[12]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[13]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[14]), 8);
	cli_putstr_P(PSTR("    "));
	cli_hexdump_rev(&(iv[15]), 8);
}

void testrun_stdtest_ubi(void){
	testrun_stdtest_ubi256(128);
	testrun_stdtest_ubi256(160);
	testrun_stdtest_ubi256(224);
	testrun_stdtest_ubi256(256);
	
	testrun_stdtest_ubi512(128);
	testrun_stdtest_ubi512(160);
	testrun_stdtest_ubi512(224);
	testrun_stdtest_ubi512(256);
	testrun_stdtest_ubi512(384);
	testrun_stdtest_ubi512(512);
	
	testrun_stdtest_ubi1024(384);
	testrun_stdtest_ubi1024(512);
	testrun_stdtest_ubi1024(1024);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
//	{ nessie_str,      NULL, testrun_nessie_noekeon},
	{ test_str,        NULL, testrun_stdtest_ubi},
//	{ performance_str, NULL, testrun_performance_noekeon},
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
