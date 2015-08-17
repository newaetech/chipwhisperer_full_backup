/* main-salsa20-test.c */
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

#include "salsa20.h"
#include "cli.h"
#include "performance_test.h"

#include "scal_salsa20.h"
#include "scal-basic.h"
#include "scal-nessie.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

char* algo_name = "Salsa20";

/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/

void nessie_first(void){
	salsa20_ctx_t ctx;
	uint8_t key[16];

	memset(key, 0, 16);
	key[0] = 0x80;
	cli_putstr_P(PSTR("\r\n testing with key: "));
	cli_hexdump(key, 16);
	salsa20_init(key, 128, NULL, &ctx);
	cli_putstr_P(PSTR("\r\n internal state: "));
	cli_hexdump_block(ctx.a.v8, 64, 4, 16);
	salsa20_gen(&ctx);
	cli_putstr_P(PSTR("\r\n internal state: "));
	cli_hexdump_block(ctx.a.v8, 64, 4, 16);
	cli_putstr_P(PSTR("\r\n data: "));
	cli_hexdump_block(ctx.buffer, 64, 4, 16);


	memset(key, 0, 16);
	key[15] = 0x01;
	cli_putstr_P(PSTR("\r\n testing with key: "));
	cli_hexdump(key, 16);
	cli_hexdump_block(ctx.a.v8, 64, 4, 16);
	salsa20_init(key, 128, NULL, &ctx);
	cli_putstr_P(PSTR("\r\n internal state: "));
	cli_hexdump_block(ctx.a.v8, 64, 4, 16);
	salsa20_gen(&ctx);
	cli_putstr_P(PSTR("\r\n internal state: "));
	cli_hexdump_block(ctx.a.v8, 64, 4, 16);
	cli_putstr_P(PSTR("\r\n data: "));
	cli_hexdump_block(ctx.buffer, 64, 4, 16);
}

/*
Salsa20(
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
=(
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  ).


Salsa20(
	211,159, 13,115, 76, 55, 82,183, 3,117,222, 37,191,187,234,136,
	49,237,179, 48, 1,106,178,219,175,199,166, 48, 86, 16,179,207,
	31,240, 32, 63, 15, 83, 93,161,116,147, 48,113,238, 55,204, 36,
	79,201,235, 79, 3, 81,156, 47,203, 26,244,243, 88,118,104, 54)
= (
	109, 42,178,168,156,240,248,238,168,196,190,203, 26,110,170,154,
	29, 29,150, 26,150, 30,235,249,190,163,251, 48, 69,144, 51, 57,
	118, 40,152,157,180, 57, 27, 94,107, 42,236, 35, 27,111,114,114,
	219,236,232,135,111,155,110, 18, 24,232, 95,158,179, 19, 48,202
  ).

Salsa20(
	88,118,104, 54, 79,201,235, 79, 3, 81,156, 47,203, 26,244,243,
	191,187,234,136,211,159, 13,115, 76, 55, 82,183, 3,117,222, 37,
	86, 16,179,207, 49,237,179, 48, 1,106,178,219,175,199,166, 48,
	238, 55,204, 36, 31,240, 32, 63, 15, 83, 93,161,116,147, 48,113)
= (
	179, 19, 48,202,219,236,232,135,111,155,110, 18, 24,232, 95,158,
	26,110,170,154,109, 42,178,168,156,240,248,238,168,196,190,203,
	69,144, 51, 57, 29, 29,150, 26,150, 30,235,249,190,163,251, 48,
	27,111,114,114,118, 40,152,157,180, 57, 27, 94,107, 42,236, 35
  ).

Salsa20^1000000 (
	6,124, 83,146, 38,191, 9, 50, 4,161, 47,222,122,182,223,185,
	75, 27, 0,216, 16,122, 7, 89,162,104,101,147,213, 21, 54, 95,
	225,253,139,176,105,132, 23,116, 76, 41,176,207,221, 34,157,108,
	94, 94, 99, 52, 90,117, 91,220,146,190,239,143,196,176,130,186)
=(
	8, 18, 38,199,119, 76,215, 67,173,127,144,162,103,212,176,217,
	192, 19,233, 33,159,197,154,160,128,243,219, 65,171,136,135,225,
	123, 11, 68, 86,237, 82, 20,155,133,189, 9, 83,167,116,194, 78,
	122,127,195,185,185,204,188, 90,245, 9,183,248,226, 85,245,104
 ).
 */

const uint8_t Salsa20_spectest0_in[] PROGMEM = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const uint8_t Salsa20_spectest0_ref[] PROGMEM = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


const uint8_t Salsa20_spectest1_in[] PROGMEM = {
	211,159, 13,115, 76, 55, 82,183, 3,117,222, 37,191,187,234,136,
	49,237,179, 48, 1,106,178,219,175,199,166, 48, 86, 16,179,207,
	31,240, 32, 63, 15, 83, 93,161,116,147, 48,113,238, 55,204, 36,
	79,201,235, 79, 3, 81,156, 47,203, 26,244,243, 88,118,104, 54 };

const uint8_t Salsa20_spectest1_ref[] PROGMEM = {
	109, 42,178,168,156,240,248,238,168,196,190,203, 26,110,170,154,
	29, 29,150, 26,150, 30,235,249,190,163,251, 48, 69,144, 51, 57,
	118, 40,152,157,180, 57, 27, 94,107, 42,236, 35, 27,111,114,114,
	219,236,232,135,111,155,110, 18, 24,232, 95,158,179, 19, 48,202 };

const uint8_t Salsa20_spectest2_in[] PROGMEM = {
	88,118,104, 54, 79,201,235, 79, 3, 81,156, 47,203, 26,244,243,
	191,187,234,136,211,159, 13,115, 76, 55, 82,183, 3,117,222, 37,
	86, 16,179,207, 49,237,179, 48, 1,106,178,219,175,199,166, 48,
	238, 55,204, 36, 31,240, 32, 63, 15, 83, 93,161,116,147, 48,113 };

const uint8_t Salsa20_spectest2_ref[] PROGMEM = {
	179, 19, 48,202,219,236,232,135,111,155,110, 18, 24,232, 95,158,
	26,110,170,154,109, 42,178,168,156,240,248,238,168,196,190,203,
	69,144, 51, 57, 29, 29,150, 26,150, 30,235,249,190,163,251, 48,
	27,111,114,114,118, 40,152,157,180, 57, 27, 94,107, 42,236, 35 };

const uint8_t Salsa20_spectest3_in[] PROGMEM = {
	6,124, 83,146, 38,191, 9, 50, 4,161, 47,222,122,182,223,185,
	75, 27, 0,216, 16,122, 7, 89,162,104,101,147,213, 21, 54, 95,
	225,253,139,176,105,132, 23,116, 76, 41,176,207,221, 34,157,108,
	94, 94, 99, 52, 90,117, 91,220,146,190,239,143,196,176,130,186 };

const uint8_t Salsa20_spectest3_ref[] PROGMEM = {
	8, 18, 38,199,119, 76,215, 67,173,127,144,162,103,212,176,217,
	192, 19,233, 33,159,197,154,160,128,243,219, 65,171,136,135,225,
	123, 11, 68, 86,237, 82, 20,155,133,189, 9, 83,167,116,194, 78,
	122,127,195,185,185,204,188, 90,245, 9,183,248,226, 85,245,104 };


void spec_test(void){
	uint8_t buffer[64];
	nessie_first();
	cli_putstr_P(PSTR("\r\ntesting with vectors from sepcification"));
	cli_putstr_P(PSTR("\r\ntest 0: "));
	memcpy_P(buffer, Salsa20_spectest0_in, 64);
	salsa20_hash((uint32_t*)buffer);
	if(memcmp_P(buffer, Salsa20_spectest0_ref, 64)){
		cli_putstr_P(PSTR(" [fail]"));
	}else{
		cli_putstr_P(PSTR(" [ok]"));
	}

	cli_putstr_P(PSTR("\r\ntest 1: "));
	memcpy_P(buffer, Salsa20_spectest1_in, 64);
	salsa20_hash((uint32_t*)buffer);
	if(memcmp_P(buffer, Salsa20_spectest1_ref, 64)){
		cli_putstr_P(PSTR(" [fail]"));
	}else{
		cli_putstr_P(PSTR(" [ok]"));
	}

	cli_putstr_P(PSTR("\r\ntest 2: "));
	memcpy_P(buffer, Salsa20_spectest2_in, 64);
	salsa20_hash((uint32_t*)buffer);
	if(memcmp_P(buffer, Salsa20_spectest2_ref, 64)){
		cli_putstr_P(PSTR(" [fail]"));
	}else{
		cli_putstr_P(PSTR(" [ok]"));
	}



	uint32_t count=0;
	cli_putstr_P(PSTR("\r\ntest 3: "));
	memcpy_P(buffer, Salsa20_spectest3_in, 64);
	do{
		if((count&0xFFFF)==0){
			cli_putc('.');
			if((count&0x20FFFF)==0){
				cli_putc('\r'); cli_putc('\n');
			}
		}
		salsa20_hash((uint32_t*)buffer);
	}while(++count<1000000);
	if(memcmp_P(buffer, Salsa20_spectest0_ref, 64)){
		cli_putstr_P(PSTR(" [fail]"));
	}else{
		cli_putstr_P(PSTR(" [ok]"));
	}
}

void testrun_nessie_salsa20(void){
	scal_nessie_set_estream(1);
	scal_nessie_run(&salsa20_desc);
}

void testrun_performance_salsa20(void){
	uint64_t t;
	char str[16];
	uint8_t key[32];
	salsa20_ctx_t ctx;

	calibrateTimer();
	print_overhead();

	memset(key,  0, 16);

	startTimer(1);
	salsa20_init(key, 128, NULL, &ctx);
	t = stopTimer();
	cli_putstr_P(PSTR("\r\n\tctx-gen time: "));
	ultoa((unsigned long)t, str, 10);
	cli_putstr(str);

	startTimer(1);
	salsa20_gen(&ctx);
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
const char test_str[]        PROGMEM = "test";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ nessie_str,      NULL, testrun_nessie_salsa20 },
	{ performance_str, NULL, testrun_performance_salsa20},
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

