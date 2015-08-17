/* bcal-performance.c */
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
 * \file    bcal-performance.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2010-02-16
 * \license GPLv3 or later
 *
 */

#include "bcal-performance.h"
#include "keysize_descriptor.h"
#include "blockcipher_descriptor.h"
#include "performance_test.h"
#include "stack_measuring.h"
#include "cli.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#define PATTERN_A 0xAA
#define PATTERN_B 0x55


static
void printvalue(unsigned long v){
	char str[20];
	int i;
	ultoa(v, str, 10);
	for(i=0; i<10-strlen(str); ++i){
		cli_putc(' ');
	}
	cli_putstr(str);
}

void bcal_performance(const bcdesc_t* bcd){
	bcdesc_t bc;
	memcpy_P(&bc, bcd, sizeof(bcdesc_t));
	uint8_t ctx[bc.ctxsize_B];
	uint8_t data[(bc.blocksize_b+7)/8];
	uint16_t keysize = get_keysize(bc.valid_keysize_desc);
	uint8_t key[(keysize+7)/8];
	uint64_t t;
	uint8_t i;

	if(bc.type!=BCDESC_TYPE_BLOCKCIPHER)
		return;
	calibrateTimer();
	print_overhead();
	cli_putstr_P(PSTR("\r\n\r\n === "));
	cli_putstr_P(bc.name);
	cli_putstr_P(PSTR(" performance === "
	                  "\r\n    type:             blockcipher"
	                  "\r\n    keysize (bits):     "));
	printvalue(keysize);

	cli_putstr_P(PSTR("\r\n    ctxsize (bytes):    "));
	printvalue(bc.ctxsize_B);

	cli_putstr_P(PSTR("\r\n    blocksize (bits):   "));
	printvalue(bc.blocksize_b);



	t=0;
	if(bc.init.init1){
		if((bc.flags&BC_INIT_TYPE)==BC_INIT_TYPE_1){
			for(i=0; i<32; ++i){
				startTimer(0);
				START_TIMER;
				(bc.init.init1)(key, &ctx);
				STOP_TIMER;
				t += stopTimer();
				if(i!=31 && bc.free){
					bc.free(&ctx);
				}
			}
		} else {
			for(i=0; i<32; ++i){
				startTimer(0);
				START_TIMER;
				(bc.init.init2)(key, keysize, &ctx);
				STOP_TIMER;
				t += stopTimer();
				if(i!=31 && bc.free){
					bc.free(&ctx);
				}
			}
		}
		t>>=5;
		cli_putstr_P(PSTR("\r\n    init (cycles):      "));
		printvalue(t);
	}
	t=0;
	for(i=0; i<32; ++i){
		startTimer(0);
		START_TIMER;
		bc.enc.enc1(data, &ctx);
		STOP_TIMER;
		t += stopTimer();
	}
	t>>=5;
	cli_putstr_P(PSTR("\r\n    encrypt (cycles):   "));
	printvalue(t);

	t=0;
	for(i=0; i<32; ++i){
		startTimer(0);
		START_TIMER;
		bc.dec.dec1(data, &ctx);
		STOP_TIMER;
		t += stopTimer();
	}
	t>>=5;
	cli_putstr_P(PSTR("\r\n    decrypt (cycles):   "));
	printvalue(t);

	if(bc.free){
		bc.free(&ctx);
	}
}

void bcal_stacksize(const bcdesc_t* bcd){
	bcdesc_t bc;
	stack_measuring_ctx_t smctx;
	memcpy_P(&bc, bcd, sizeof(bcdesc_t));
	uint8_t ctx[bc.ctxsize_B];
	uint8_t data[(bc.blocksize_b+7)/8];
	uint16_t keysize = get_keysize(bc.valid_keysize_desc);
	uint8_t key[(keysize+7)/8];
	uint16_t t1, t2;

	if(bc.type!=BCDESC_TYPE_BLOCKCIPHER)
		return;
	cli_putstr_P(PSTR("\r\n\r\n === "));
	cli_putstr_P(bc.name);
	cli_putstr_P(PSTR(" stack-usage === "));

	if(bc.init.init1){
		if((bc.flags&BC_INIT_TYPE)==BC_INIT_TYPE_1){
			cli();
			stack_measure_init(&smctx, PATTERN_A);
			bc.init.init1(&ctx, key);
			t1 = stack_measure_final(&smctx);
			stack_measure_init(&smctx, PATTERN_B);
			bc.init.init1(&ctx, key);
			t2 = stack_measure_final(&smctx);
			sei();
		} else {
			cli();
			stack_measure_init(&smctx, PATTERN_A);
			bc.init.init2(&ctx, keysize, key);
			t1 = stack_measure_final(&smctx);
			stack_measure_init(&smctx, PATTERN_B);
			bc.init.init2(&ctx, keysize, key);
			t2 = stack_measure_final(&smctx);
			sei();
		}
		t1 = (t1>t2)?t1:t2;
		cli_putstr_P(PSTR("\r\n    init (bytes):       "));
		printvalue((unsigned long)t1);
	}
	cli();
	stack_measure_init(&smctx, PATTERN_A);
	bc.enc.enc1(data, &ctx);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	bc.enc.enc1(data, &ctx);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1>t2)?t1:t2;
	cli_putstr_P(PSTR("\r\n    encBlock (bytes):   "));
	printvalue((unsigned long)t1);

	cli();
	stack_measure_init(&smctx, PATTERN_A);
	bc.dec.dec1(data, &ctx);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	bc.dec.dec1(data, &ctx);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1>t2)?t1:t2;
	cli_putstr_P(PSTR("\r\n    decBlock (bytes):   "));
	printvalue((unsigned long)t1);

	if(bc.free){
		bc.free(&ctx);
	}
}

void bcal_performance_multiple(const bcdesc_t* const* bcd_list){
	const bcdesc_t* bcd;
	for(;;){
		bcd = (void*)pgm_read_word(bcd_list);
		if(!bcd){
			cli_putstr_P(PSTR("\r\n\r\n End of performance figures\r\n"));
			return;
		}
		bcal_performance(bcd);
		bcal_stacksize(bcd);
		bcd_list = (void*)((uint8_t*)bcd_list + 2);
	}
}
