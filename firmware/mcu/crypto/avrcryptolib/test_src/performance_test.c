/* performance_test.c */
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
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 *
 *
 **/

#include "config.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "cli.h"
#include "performance_test.h"


#ifdef ATMEGA644
	#define TIMSK TIMSK1
#endif



static volatile uint32_t ovfcounter;

static uint16_t const_overhead=0;
static uint16_t int_overhead=0;

ISR(TIMER1_OVF_vect){
	ovfcounter++;
}

void calibrateTimer(void){
	volatile uint8_t i=0;
	startTimer(1);
	stopTimer();
	const_overhead = TCNT1;
	startTimer(1);
	TCNT1=0xFFFE;
	i++;
	stopTimer();
	int_overhead = TCNT1;
}

void startTimer(uint8_t granularity){
	TCCR1B = 0; /* stop timer */
	TCNT1  = 0;
	ovfcounter = 0;
	TCCR1A = 0x00;
	TIMSK  = _BV(TOIE1); /* enable TOIE1 */
	TCCR1B = granularity & 0x7;	/* start timer */
}

uint64_t stopTimer(void){
	TCCR1B = 0; /* stop timer */
	uint64_t ret;
	ret = (((uint64_t)ovfcounter)<<16) | TCNT1;
	ret -= const_overhead;
	ret -= ovfcounter * int_overhead;
	return ret;
}

void getOverhead(uint16_t* constoh, uint16_t* intoh){
	*constoh = const_overhead;
	*intoh   = int_overhead;
}

void print_time_P(PGM_P s, uint64_t t){
	char sv[16];
	uint8_t c;
	cli_putstr_P(PSTR("\r\n"));
	cli_putstr_P(s);
	ultoa((unsigned long)t, sv, 10);
	for(c=strlen(sv); c<11; ++c){
		cli_putc(' ');
	}
	cli_putstr(sv);
}

void print_overhead(void){
	char str[16];
	cli_putstr_P(PSTR("\r\n\r\n=== benchmark ==="));
	utoa(const_overhead, str, 10);
	cli_putstr_P(PSTR("\r\n\tconst overhead:     "));
	cli_putstr(str);
	utoa(int_overhead, str, 10);
	cli_putstr_P(PSTR("\r\n\tinterrupt overhead: "));
	cli_putstr(str);
}


