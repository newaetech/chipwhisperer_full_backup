/* main-mqq160-sign-test.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
 * MQQ160-sign test-suit
 *
*/

#include "config.h"
#include "uart_i.h"
#include "debug.h"

#include "cli.h"
#include "mqq160-sign.h"
#include "mqq160-sign_P.h"
#include "mqq160-sign_testkey.h"
#include "performance_test.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "stack_measuring.h"

char* algo_name = "MQQ160-sign";


/*****************************************************************************
 *  additional validation-functions											 *
 *****************************************************************************/
const uint8_t test_hash[20] PROGMEM =
{
	(uint8_t)0x64, (uint8_t)0xFE, (uint8_t)0x2A, (uint8_t)0x85,
	(uint8_t)0xBB, (uint8_t)0x8C, (uint8_t)0x54, (uint8_t)0x5C,
	(uint8_t)0x65, (uint8_t)0x74, (uint8_t)0xA0, (uint8_t)0xF3,
	(uint8_t)0xD0, (uint8_t)0xAF, (uint8_t)0x96, (uint8_t)0xB9,
	(uint8_t)0x0F, (uint8_t)0x17, (uint8_t)0xF3, (uint8_t)0xAD
};

void performance_mqq(void){
	uint8_t hash[20];
	uint8_t signature[20];
	long t;
	char str[3*sizeof(long)+2];
	uint8_t tmp;
	uint16_t s1, s2;
	stack_measuring_ctx_t smctx;
	memcpy_P(hash, test_hash, 20);

	uint8_t key_buffer[MQQ160SIGN_KEY_SIZE];
	mqq160_sign_key_t key;
	mqq_load_pgm_key(key_buffer, &key, &testkey_P);

	cli_putstr_P(PSTR("\r\n=== Performance of MQQ160-SIGN ==="));
	calibrateTimer();
	startTimer(0);
	START_TIMER;
	mqq160_sign_P(signature, hash, &testkey_P);
	STOP_TIMER;
	t = stopTimer();
	ltoa(t, str, 10);
	cli_putstr_P(PSTR("\r\n cycles for mqq160_sign_P: "));
	tmp = 12-strlen(str);
	while(tmp--){
		cli_putc(' ');
	}
	cli_putstr(str);

	calibrateTimer();
	startTimer(0);
	START_TIMER;
	mqq160_sign(signature, hash, &key);
	STOP_TIMER;
	t = stopTimer();
	ltoa(t, str, 10);
	cli_putstr_P(PSTR("\r\n cycles for mqq160_sign:   "));
	tmp = 12-strlen(str);
	while(tmp--){
		cli_putc(' ');
	}
	cli_putstr(str);

	stack_measure_init(&smctx, 0xAA);
	mqq160_sign_P(signature, hash, &testkey_P);
	s1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, 0x55);
	mqq160_sign_P(signature, hash, &testkey_P);
	s2 = stack_measure_final(&smctx);
	s1 = (s1>s2)?s1:s2;
	ltoa((long)s1, str, 10);
	cli_putstr_P(PSTR("\r\n stack for mqq160_sign_P:  "));
	tmp = 12-strlen(str);
	while(tmp--){
		cli_putc(' ');
	}
	cli_putstr(str);
	stack_measure_init(&smctx, 0xAA);
	mqq160_sign(signature, hash, &key);
	s1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, 0x55);
	mqq160_sign_P(signature, hash, &testkey_P);
	s2 = stack_measure_final(&smctx);
	s1 = (s1>s2)?s1:s2;
	ltoa((long)s1, str, 10);
	cli_putstr_P(PSTR("\r\n stack for mqq160_sign:    "));
	tmp = 12-strlen(str);
	while(tmp--){
		cli_putc(' ');
	}
	cli_putstr(str);

	cli_putstr_P(PSTR("\r\n=== End of performance figures ==="));
}

void testrun_mqq_mem(void){
	uint8_t hash[20];
	uint8_t signature[20];
	memcpy_P(hash, test_hash, 20);

	uint8_t key_buffer[MQQ160SIGN_KEY_SIZE];
	mqq160_sign_key_t key;
	mqq_load_pgm_key(key_buffer, &key, &testkey_P);
	mqq160_sign(signature, hash, &key);
	cli_putstr_P(PSTR("\r\ntest signature (RAM):   "));
	cli_hexdump(signature, 20);
}

void testrun_mqq_flash(void){
	uint8_t hash[20];
	uint8_t signature[20];
	memcpy_P(hash, test_hash, 20);

	mqq160_sign_P(signature, hash, &testkey_P);
	cli_putstr_P(PSTR("\r\ntest signature (FLASH): "));
	cli_hexdump(signature, 20);
}

void testrun_mqq(void){
	uint8_t hash[20];
	uint8_t signature[20];
	memcpy_P(hash, test_hash, 20);

	uint8_t key_buffer[MQQ160SIGN_KEY_SIZE];
	mqq160_sign_key_t key;
	mqq_load_pgm_key(key_buffer, &key, &testkey_P);
	mqq160_sign(signature, hash, &key);
	cli_putstr_P(PSTR("\r\ntest signature (RAM):   "));
	cli_hexdump(signature, 20);

	mqq160_sign_P(signature, hash, &testkey_P);
	cli_putstr_P(PSTR("\r\ntest signature (FLASH): "));
	cli_hexdump(signature, 20);
}

/*****************************************************************************
 *  main																	 *
 *****************************************************************************/

const char test_str[]        PROGMEM = "test";
const char test_flash_str[]  PROGMEM = "flash";
const char test_mem_str[]    PROGMEM = "mem";
const char performance_str[] PROGMEM = "performance";
const char echo_str[]        PROGMEM = "echo";

const cmdlist_entry_t cmdlist[] PROGMEM = {
	{ test_str,                    NULL, testrun_mqq                   },
	{ test_flash_str,              NULL, testrun_mqq_flash             },
	{ test_mem_str,                NULL, testrun_mqq_mem               },
	{ performance_str,             NULL, performance_mqq               },
	{ echo_str,                (void*)1, (void_fpt)echo_ctrl           },
	{ NULL,                        NULL, NULL                          }
};

int main (void){
	DEBUG_INIT();

	cli_rx = (cli_rx_fpt)uart0_getc;
	cli_tx = (cli_tx_fpt)uart0_putc;
	for(;;){
		cli_putstr_P(PSTR("\r\n\r\nCrypto-VS ("));
		cli_putstr(algo_name);
		cli_putstr_P(PSTR("; "));
		cli_putstr(__DATE__);
		cli_putstr_P(PSTR(" "));
		cli_putstr(__TIME__);
		cli_putstr_P(PSTR(")\r\nloaded and running\r\n"));

		cmd_interface(cmdlist);
	}
}
