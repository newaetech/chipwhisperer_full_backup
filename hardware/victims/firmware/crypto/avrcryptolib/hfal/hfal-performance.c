/* hfal-performance.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file    hfal-performance.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-10
 * \license GPLv3 or later
 *
 */

#include "hfal-performance.h"
#include "hashfunction_descriptor.h"
#include "stack_measuring.h"
#include "cli.h"
#include "performance_test.h"
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

void hfal_performance(const hfdesc_t* hd){
	hfdesc_t hf;
	memcpy_P(&hf, hd, sizeof(hfdesc_t));
	uint8_t ctx[hf.ctxsize_B];
	uint8_t data[(hf.blocksize_b+7)/8];
	uint8_t digest[(hf.hashsize_b+7)/8];
	uint64_t t;
	uint8_t i;

	if(hf.type!=HFDESC_TYPE_HASHFUNCTION)
		return;
	calibrateTimer();
	print_overhead();
	cli_putstr_P(PSTR("\r\n\r\n === "));
	cli_putstr_P(hf.name);
	cli_putstr_P(PSTR(" performance === "
	                  "\r\n    type:             hashfunction"
	                  "\r\n    hashsize (bits):    "));
	printvalue(hf.hashsize_b);

	cli_putstr_P(PSTR("\r\n    ctxsize (bytes):    "));
	printvalue(hf.ctxsize_B);

	cli_putstr_P(PSTR("\r\n    blocksize (bits):   "));
	printvalue(hf.blocksize_b);

	t=0;
	for(i=0; i<32; ++i){
		startTimer(0);
		START_TIMER;
		hf.init(&ctx);
		STOP_TIMER;
		t += stopTimer();
		if(i!=31 && hf.free){
			hf.free(&ctx);
		}
	}
	t>>=5;
	cli_putstr_P(PSTR("\r\n    init (cycles):      "));
	printvalue(t);

	t=0;
	for(i=0; i<32; ++i){
		startTimer(0);
		START_TIMER;
		hf.nextBlock(&ctx, data);
		STOP_TIMER;
		t += stopTimer();
	}
	t>>=5;
	cli_putstr_P(PSTR("\r\n    nextBlock (cycles): "));
	printvalue(t);

	t=0;
	for(i=0; i<32; ++i){
		startTimer(0);
		START_TIMER;
		hf.lastBlock(&ctx, data, 0);
		STOP_TIMER;
		t += stopTimer();
	}
	t>>=5;
	cli_putstr_P(PSTR("\r\n    lastBlock (cycles): "));
	printvalue(t);

	t=0;
	for(i=0; i<32; ++i){
		startTimer(0);
		START_TIMER;
		hf.ctx2hash(digest, &ctx);
		STOP_TIMER;
		t += stopTimer();
	}
	t>>=5;
	cli_putstr_P(PSTR("\r\n    ctx2hash (cycles):  "));
	printvalue(t);

	if(hf.free){
		hf.free(&ctx);
	}
}

void hfal_stacksize(const hfdesc_t* hd){
	hfdesc_t hf;
	stack_measuring_ctx_t smctx;
	memcpy_P(&hf, hd, sizeof(hfdesc_t));
	uint8_t ctx[hf.ctxsize_B];
	uint8_t data[(hf.blocksize_b+7)/8];
	uint8_t digest[(hf.hashsize_b+7)/8];
	uint16_t t1, t2;

	if(hf.type!=HFDESC_TYPE_HASHFUNCTION)
		return;
	cli_putstr_P(PSTR("\r\n\r\n === "));
	cli_putstr_P(hf.name);
	cli_putstr_P(PSTR(" stack-usage === "));

	cli();
	stack_measure_init(&smctx, PATTERN_A);
	hf.init(&ctx);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	hf.init(&ctx);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1>t2)?t1:t2;
	cli_putstr_P(PSTR("\r\n    init (bytes):       "));
	printvalue((unsigned long)t1);

	cli();
	stack_measure_init(&smctx, PATTERN_A);
	hf.nextBlock(&ctx, data);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	hf.nextBlock(&ctx, data);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1>t2)?t1:t2;
	cli_putstr_P(PSTR("\r\n    nextBlock (bytes):  "));
	printvalue((unsigned long)t1);

	cli();
	stack_measure_init(&smctx, PATTERN_A);
	hf.lastBlock(&ctx, data, 0);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	hf.lastBlock(&ctx, data, 0);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1>t2)?t1:t2;
	cli_putstr_P(PSTR("\r\n    lastBlock (bytes):  "));
	printvalue((unsigned long)t1);

	cli();
	stack_measure_init(&smctx, PATTERN_A);
	hf.ctx2hash(digest, &ctx);
	t1 = stack_measure_final(&smctx);
	stack_measure_init(&smctx, PATTERN_B);
	hf.ctx2hash(digest, &ctx);
	t2 = stack_measure_final(&smctx);
	sei();

	t1 = (t1>t2)?t1:t2;
	cli_putstr_P(PSTR("\r\n    ctx2hash (bytes):   "));
	printvalue((unsigned long)t1);

	if(hf.free){
		hf.free(&ctx);
	}
}

void hfal_performance_multiple(const hfdesc_t* const* hd_list){
	const hfdesc_t* hd;
	for(;;){
		hd = (void*)pgm_read_word(hd_list);
		if(!hd){
			cli_putstr_P(PSTR("\r\n\r\n End of performance figures\r\n"));
			return;
		}
		hfal_performance(hd);
		hfal_stacksize(hd);
		hd_list = (void*)((uint8_t*)hd_list + 2);
	}
}

