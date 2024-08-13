/* main-rabbit-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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


#include "config.h"

#include "uart_i.h"
#include "debug.h"

#include "rabbit.h"
#include "cli.h"
#include "performance_test.h"

#include "scal_rabbit.h"
#include "scal-basic.h"
#include "scal-nessie.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <avr/pgmspace.h>

const char* algo_name = "Rabbit";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void test_vector(void* key, void* iv, const void* ref){
	rabbit_ctx_t ctx;
	uint8_t fail=0;
	cli_putstr_P(PSTR("\r\n testing with\r\n key:  "));
	cli_hexdump(key, 16);
	cli_putstr_P(PSTR("\r\n iv:   "));
	if(iv){
		cli_hexdump(iv, 8);
	}else{
		cli_putstr_P(PSTR("[no iv]"));
	}
	rabbit_init(key, 128, iv, &ctx);
	rabbit_gen(&ctx);
	if(!ref || (memcmp_P(ctx.buffer, ref, 16))!=0){
		fail = 1;
		cli_putstr_P(PSTR("\r\n S[0]: "));
		cli_hexdump(ctx.buffer, 16);
	}
	ctx.buffer_idx=16;
	rabbit_gen(&ctx);
	if(!ref || (memcmp_P(ctx.buffer, ref, 16))!=0){
		fail = 1;
		cli_putstr_P(PSTR("\r\n S[0]: "));
		cli_hexdump(ctx.buffer, 16);
	}
	ctx.buffer_idx=16;
	rabbit_gen(&ctx);
	if(!ref || (memcmp_P(ctx.buffer, ref, 16))!=0){
		fail = 1;
		cli_putstr_P(PSTR("\r\n S[0]: "));
		cli_hexdump(ctx.buffer, 16);
	}
	if(ref){
		cli_putstr_P(fail?PSTR("\r\n [fail]"):PSTR("\r\n [ok]"));
	}
	cli_putstr_P(PSTR("\r\n"));
}

void nessie_first(void){
	uint8_t key[16];
    uint8_t iv[8];

	memset(iv, 0, 8);
	memset(key, 0, 16);
	key[0] = 0x80;
	test_vector(key, iv, NULL);
	key[0] = 0x00;

	key[15] = 0x80;
	test_vector(key, iv, NULL);

}


const uint8_t spec_key1[] PROGMEM = {
	0x91, 0x28, 0x13, 0x29, 0x2E, /* 0xED */ 0x3D, 0x36, 0xFE,
	0x3B, 0xFC, 0x62, 0xF1, 0xDC, 0x51, 0xC3, 0xAC
};

const uint8_t spec_key2[] PROGMEM = {
	0x83, 0x95, 0x74, 0x15, 0x87, 0xE0, 0xC7, 0x33,
	0xE9, 0xE9, 0xAB, 0x01, 0xC0, 0x9B, 0x00, 0x43
};

const uint8_t spec_iv1[] PROGMEM = {
	0xC3, 0x73, 0xF5, 0x75, 0xC1, 0x26, 0x7E, 0x59
};

const uint8_t spec_iv2[] PROGMEM = {
	0xA6, 0xEB, 0x56, 0x1A, 0xD2, 0xF4, 0x17, 0x27
};

const uint8_t spec_ref[] PROGMEM = {
	0xB1, 0x57, 0x54, 0xF0, 0x36, 0xA5, 0xD6, 0xEC, 0xF5, 0x6B, 0x45, 0x26, 0x1C, 0x4A, 0xF7, 0x02,
	0x88, 0xE8, 0xD8, 0x15, 0xC5, 0x9C, 0x0C, 0x39, 0x7B, 0x69, 0x6C, 0x47, 0x89, 0xC6, 0x8A, 0xA7,
	0xF4, 0x16, 0xA1, 0xC3, 0x70, 0x0C, 0xD4, 0x51, 0xDA, 0x68, 0xD1, 0x88, 0x16, 0x73, 0xD6, 0x96,

	0x3D, 0x2D, 0xF3, 0xC8, 0x3E, 0xF6, 0x27, 0xA1, 0xE9, 0x7F, 0xC3, 0x84, 0x87, 0xE2, 0x51, 0x9C,
	0xF5, 0x76, 0xCD, 0x61, 0xF4, 0x40, 0x5B, 0x88, 0x96, 0xBF, 0x53, 0xAA, 0x85, 0x54, 0xFC, 0x19,
	0xE5, 0x54, 0x74, 0x73, 0xFB, 0xDB, 0x43, 0x50, 0x8A, 0xE5, 0x3B, 0x20, 0x20, 0x4D, 0x4C, 0x5E,

	0x0C, 0xB1, 0x0D, 0xCD, 0xA0, 0x41, 0xCD, 0xAC, 0x32, 0xEB, 0x5C, 0xFD, 0x02, 0xD0, 0x60, 0x9B,
	0x95, 0xFC, 0x9F, 0xCA, 0x0F, 0x17, 0x01, 0x5A, 0x7B, 0x70, 0x92, 0x11, 0x4C, 0xFF, 0x3E, 0xAD,
	0x96, 0x49, 0xE5, 0xDE, 0x8B, 0xFC, 0x7F, 0x3F, 0x92, 0x41, 0x47, 0xAD, 0x3A, 0x94, 0x74, 0x28,

	0xC6, 0xA7, 0x27, 0x5E, 0xF8, 0x54, 0x95, 0xD8, 0x7C, 0xCD, 0x5D, 0x37, 0x67, 0x05, 0xB7, 0xED,
	0x5F, 0x29, 0xA6, 0xAC, 0x04, 0xF5, 0xEF, 0xD4, 0x7B, 0x8F, 0x29, 0x32, 0x70, 0xDC, 0x4A, 0x8D,
	0x2A, 0xDE, 0x82, 0x2B, 0x29, 0xDE, 0x6C, 0x1E, 0xE5, 0x2B, 0xDB, 0x8A, 0x47, 0xBF, 0x8F, 0x66,

	0x1F, 0xCD, 0x4E, 0xB9, 0x58, 0x00, 0x12, 0xE2, 0xE0, 0xDC, 0xCC, 0x92, 0x22, 0x01, 0x7D, 0x6D,
	0xA7, 0x5F, 0x4E, 0x10, 0xD1, 0x21, 0x25, 0x01, 0x7B, 0x24, 0x99, 0xFF, 0xED, 0x93, 0x6F, 0x2E,
	0xEB, 0xC1, 0x12, 0xC3, 0x93, 0xE7, 0x38, 0x39, 0x23, 0x56, 0xBD, 0xD0, 0x12, 0x02, 0x9B, 0xA7,

	0x44, 0x5A, 0xD8, 0xC8, 0x05, 0x85, 0x8D, 0xBF, 0x70, 0xB6, 0xAF, 0x23, 0xA1, 0x51, 0x10, 0x4D,
	0x96, 0xC8, 0xF2, 0x79, 0x47, 0xF4, 0x2C, 0x5B, 0xAE, 0xAE, 0x67, 0xC6, 0xAC, 0xC3, 0x5B, 0x03,
	0x9F, 0xCB, 0xFC, 0x89, 0x5F, 0xA7, 0x1C, 0x17, 0x31, 0x3D, 0xF0, 0x34, 0xF0, 0x15, 0x51, 0xCB
};

void spec_test(void){
	uint8_t key[16];
    uint8_t iv[8];
    const uint8_t *ref;
    ref = spec_ref;

	memset(key, 0, 16);
	test_vector(key, NULL, ref);
	memcpy_P(key, spec_key1, 16);
	ref += 48;
	test_vector(key, NULL, ref);
	memcpy_P(key, spec_key2, 16);
	ref += 48;
	test_vector(key, NULL, ref);


	memset(key, 0, 16);
	memset(iv, 0, 8);
	ref += 48;
	test_vector(key, iv, ref);
	memcpy_P(iv,  spec_iv1,   8);
	ref += 48;
	test_vector(key, iv, ref);
	memcpy_P(iv,  spec_iv2,   8);
	ref += 48;
	test_vector(key, iv, ref);
}


void testrun_nessie_rabbit(void){
	scal_nessie_set_estream(1);
	scal_nessie_run(&rabbit_desc);
}



void testrun_performance_rabbit(void){
	uint64_t t;
	char str[16];
	uint8_t key[16];
	rabbit_ctx_t ctx;

	calibrateTimer();
	print_overhead();

	memset(key,  0, 16);

	startTimer(1);
	rabbit_init(key, 128, NULL, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	startTimer(1);
	rabbit_gen(&ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tencrypt time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	cli_putstr_P(PSTR("\r\n"));
}


/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char nessie_str[]      PROGMEM = "nessie";
const char first_str[]       PROGMEM = "first";
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_rabbit},
	{ performance_str, NULL, testrun_performance_rabbit},
	{ first_str,       NULL, nessie_first},
	{ test_str,        NULL, spec_test},
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

