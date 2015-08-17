/* main-hmac-sha256-test.c */
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
 * HMAC-SHA256 test-suit
 * 
*/

#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "sha256.h"
#include "hmac-sha256.h"

#include "cli.h"
#include "nessie_mac_test.h"

#include <stdint.h>
#include <string.h>

char* algo_name = "HMAC-SHA256";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void testrun_hmacsha256(void){
	uint8_t key[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	                  0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                      0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 
					  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                      0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 
					  0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
                      0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 
					  0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
	uint8_t msg[] = { 0x00 };
	uint8_t mac[HMAC_SHA256_BYTES];
	hmac_sha256(mac, key, 512, msg, 0);
	cli_putstr_P(PSTR("\r\n quick hmac = "));
	cli_hexdump(mac, HMAC_SHA256_BYTES);
	cli_putstr_P(PSTR("\r\n"));
}

void testrun_nessie_hmacsha256(void){
	nessie_mac_ctx.macsize_b   = HMAC_SHA256_BITS;
	nessie_mac_ctx.keysize_b   = HMAC_SHA256_BLOCK_BITS;
	nessie_mac_ctx.blocksize_B = HMAC_SHA256_BLOCK_BYTES;
	nessie_mac_ctx.ctx_size_B  = sizeof(hmac_sha256_ctx_t);
	nessie_mac_ctx.name = algo_name;
	nessie_mac_ctx.mac_init = (nessie_mac_init_fpt)hmac_sha256_init;
	nessie_mac_ctx.mac_next = (nessie_mac_next_fpt)hmac_sha256_nextBlock;
	nessie_mac_ctx.mac_last = (nessie_mac_last_fpt)hmac_sha256_lastBlock;
	nessie_mac_ctx.mac_conv = (nessie_mac_conv_fpt)hmac_sha256_final;
	
	nessie_mac_run();
}



/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
/* const char performance_str[] PROGMEM = "performance"; */
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_hmacsha256},
	{ test_str,        NULL, testrun_hmacsha256},
/*	{ performance_str, NULL, testrun_performance_hmacsha256}, */
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
