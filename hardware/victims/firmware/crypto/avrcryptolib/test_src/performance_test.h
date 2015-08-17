/* performance_test.h */
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
#ifndef PERFORMANCE_TEST_H_
#define PERFORMANCE_TEST_H_

#include <stdint.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#define START_TIMER  TCCR1B=1
#define STOP_TIMER   TCCR1B=0

void calibrateTimer(void);
void startTimer(uint8_t granularity);
uint64_t stopTimer(void);
void getOverhead(uint16_t* constoh, uint16_t* intoh);

void print_time_P(PGM_P s, uint64_t t);
void print_overhead(void);

#endif /*PERFORMANCE_TEST_H_*/
