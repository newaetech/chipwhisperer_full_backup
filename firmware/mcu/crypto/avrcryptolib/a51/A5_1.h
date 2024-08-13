/* A5_1.h */
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
 * File:    A5_1.h
 * Author:  Daniel Otte
 * Date:    24.06.2006
 * License: GPL
 * Description: Implementation of the A5/1 stream cipher algorithm, as used in GSM.
 * ! Warning, this is weak crypto !
 * 
 */
#ifndef A5_1_H_
#define A5_1_H_

#include <stdint.h>

#define R1_LENGTH 19
#define R2_LENGTH 22
#define R3_LENGTH 23
#define R1_CLK 11
#define R2_CLK 12
#define R3_CLK 13

/* 3-Bit word parity lookup table (Byte) 
 * 0: 0
 * 1: 1
 * 2: 1
 * 3: 0
 * 4: 1
 * 5: 0
 * 6: 0
 * 7: 1
 * => 1001.0110 = 0x96
 * 
 */

#define PARITY_LOOKUP 0x96

typedef struct {
	/* we are wasting one byte here but this allows a much faster implementation */
	uint8_t r1[3], r2[3], r3[3]; /* the three regs, 19,22,23 bit in length  */
} a5_1_ctx_t;
 

void		a5_1_init(a5_1_ctx_t *c, void* key, uint8_t keylength_b, void* iv, uint8_t ivlength_b);
uint8_t		a5_1_clock(a5_1_ctx_t *c);
uint8_t 	a5_1_gen(a5_1_ctx_t *c);

#endif
