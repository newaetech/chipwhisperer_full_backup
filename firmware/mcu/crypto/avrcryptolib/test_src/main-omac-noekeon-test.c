/* main-omac-noekeon-test.c */
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
 * OMAC-Noekeon test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "noekeon.h"
#include "omac_noekeon.h"

#include "cli.h"
#include "nessie_mac_test.h"
#include "performance_test.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* algo_name = "OMAC-Noekeon";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void test_mac(void* key, void* data, uint16_t datalength_b){
	uint8_t mac[16];
	cli_putstr_P(PSTR("\r\n-----------\r\n msg length (bit): 0x"));
	cli_hexdump(((uint8_t*)&datalength_b)+1, 1);
	cli_hexdump(((uint8_t*)&datalength_b)+0, 1);	
	cli_putstr_P(PSTR("\r\n msg: "));
	cli_hexdump(data, (datalength_b+7)/8);
	cli_putstr_P(PSTR("\r\n key: "));
	cli_hexdump(key, 16);
	omac_noekeon(mac, data, datalength_b, key, (uint8_t)-1);
	cli_putstr_P(PSTR("\r\n mac: "));
	cli_hexdump(mac, 16);
	
}

void testrun_test_omac_noekeon(void){
	uint8_t key[16], data[64];
	uint16_t i;
	memset(key,  0xAA, 16);
	memset(data, 0x00, 64);
	for(i=1;i<64*8; ++i){
		test_mac(key, data, i);
	}
}

/******************************************************************************/

uint8_t stat_key[16];

void omac_noekeon_next_dummy(void* ctx, const void* buffer){
	omac_noekeon_next(buffer, stat_key, ctx);
}

void omac_noekeon_init_dummy(void* ctx, const void* key, uint16_t keysize_b){
	omac_noekeon_init(ctx);
	memcpy(stat_key, key, 16);
}

void omac_noekeon_last_dummy(void* ctx, const void* buffer, uint16_t size_b){
	while(size_b>128){
		omac_noekeon_next(buffer, stat_key, ctx);
		size_b -= 128;
		buffer = (uint8_t*)buffer +16;
	}
	omac_noekeon_last(buffer, size_b, stat_key, ctx);
}

void omac_noekeon_conv_dummy(void* buffer, void* ctx){
	memcpy(buffer, ctx, 16);
}

void testrun_nessie_omac_noekeon(void){
	nessie_mac_ctx.macsize_b   = 128;
	nessie_mac_ctx.keysize_b   = 128;
	nessie_mac_ctx.blocksize_B = 16;
	nessie_mac_ctx.ctx_size_B  = sizeof(omac_noekeon_ctx_t);
	nessie_mac_ctx.name = algo_name;
	nessie_mac_ctx.mac_init = omac_noekeon_init_dummy;
	nessie_mac_ctx.mac_next = omac_noekeon_next_dummy;
	nessie_mac_ctx.mac_last = omac_noekeon_last_dummy;
	nessie_mac_ctx.mac_conv = omac_noekeon_conv_dummy;
	
	nessie_mac_run();
}

/******************************************************************************/

void testrun_performance_omac_noekeon(void){
	uint64_t t;
	char str[16];
	uint8_t data[16], key[16];
	omac_noekeon_ctx_t ctx;
	
	calibrateTimer();
	print_overhead();
	
	memset(data, 0, 16);
	memset(key,  0, 16);
	
	startTimer(1);
	omac_noekeon_init(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	omac_noekeon_next(data, key, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tone-block time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);
	
	
	startTimer(1);
	omac_noekeon_last(data, 128, key, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tlast block time: "));
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
	{ nessie_str,      NULL, testrun_nessie_omac_noekeon },
	{ test_str,        NULL, testrun_test_omac_noekeon},
	{ performance_str, NULL, testrun_performance_omac_noekeon},
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
