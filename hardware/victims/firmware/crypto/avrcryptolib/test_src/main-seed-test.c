/* main-seed-test.c */
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
/**
 * \file	main-seed-test.c
 * \author	Daniel Otte
 * \email	daniel.otte@rub.de
 * \date	2007-06-01
 * \brief	test suit for SEED
 * \par License	
 * GPLv3 or later
 * 
 */
#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "seed.h"
#include "cli.h"
#include "performance_test.h"
#include "bcal-performance.h"
#include "bcal-nessie.h"
#include "bcal_seed.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* algo_name = "Seed";

const bcdesc_t* const algolist[] PROGMEM = {
	(bcdesc_t*)&seed_desc,
	NULL
};
/*****************************************************************************
 *  additional validation-functions                                          *
 *****************************************************************************/
void seed_genctx_dummy(uint8_t* key, uint16_t keysize, void* ctx){
	seed_init(key, ctx);
}

void testrun_nessie_seed(void){
	bcal_nessie_multiple(algolist);
}


void testrun_performance_seed(void){
	bcal_performance_multiple(algolist);
}

/*****************************************************************************
 *  self tests                                                               *
 *****************************************************************************/

void testencrypt(uint8_t* block, uint8_t* key){
	seed_ctx_t ctx;
	cli_putstr("\r\n==testy-encrypt==\r\n key:   ");
	cli_hexdump(key,16);
	seed_init(key, &ctx);
	cli_putstr("\r\n plain: ");
	cli_hexdump(block,16);
	seed_enc(block, &ctx);
	cli_putstr("\r\n crypt: ");
	cli_hexdump(block,16);
}

void testdecrypt(uint8_t* block, uint8_t* key){
	seed_ctx_t ctx;
	cli_putstr("\r\n==testy-decrypt==\r\n key:   ");
	cli_hexdump(key,16);
	seed_init(key, &ctx);
	cli_putstr("\r\n crypt: ");
	cli_hexdump(block,16);
	seed_dec(block, &ctx);
	cli_putstr("\r\n plain: ");
	cli_hexdump(block,16);
}

void testrun_seed(void){
	uint8_t keys[4][16]=
                { {     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                  {     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
                  {     0x47, 0x06, 0x48, 0x08, 0x51, 0xE6, 0x1B, 0xE8,
                        0x5D, 0x74, 0xBF, 0xB3, 0xFD, 0x95, 0x61, 0x85 },
                  {     0x28, 0xDB, 0xC3, 0xBC, 0x49, 0xFF, 0xD8, 0x7D,
                        0xCF, 0xA5, 0x09, 0xB1, 0x1D, 0x42, 0x2B, 0xE7,}
                };
	uint8_t datas[4][16]=
                { {     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
                  {     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                  {     0x83, 0xA2, 0xF8, 0xA2, 0x88, 0x64, 0x1F, 0xB9, 
                        0xA4, 0xE9, 0xA5, 0xCC, 0x2F, 0x13, 0x1C, 0x7D },
                  {     0xB4, 0x1E, 0x6B, 0xE2, 0xEB, 0xA8, 0x4A, 0x14, 
                        0x8E, 0x2E, 0xED, 0x84, 0x59, 0x3C, 0x5E, 0xC7 }
                };
	uint8_t i=0;
	for(i=0; i<4; ++i){
		testencrypt(datas[i],keys[i]);
		testdecrypt(datas[i],keys[i]);	
	}
}



/*****************************************************************************
 *  main                                                                     *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_seed},
	{ test_str,        NULL, testrun_seed},
	{ performance_str, NULL, testrun_performance_seed},
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
