/* bigint.h */
/*
    This file is part of the ARM-Crypto-Lib.
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
 * \file		bigint.h
 * \author		Daniel Otte
 * \date		2010-02-22
 * 
 * \license	    GPLv3 or later
 * 
 */

#ifndef BIGINT_H_
#define BIGINT_H_

#include <stdint.h>

typedef uint8_t bigint_word_t;
typedef uint16_t bigint_wordplus_t;
typedef int16_t  bigint_wordplus_signed_t;
#define BIGINT_WORD_SIZE 8

#define BIGINT_FBS_MASK (BIGINT_WORD_SIZE-1) /* the last five bits indicate which is the first bit set */
#define BIGINT_NEG_MASK 0x80 /* this bit indicates a negative value */
typedef struct{
	uint16_t length_B;
	uint8_t info;
	bigint_word_t *wordv; /* word vector, pointing to the LSB */
}bigint_t;



/******************************************************************************/

void   bigint_adjust(bigint_t* a);
uint32_t bigint_get_first_set_bit(const bigint_t* a);
uint32_t bigint_get_last_set_bit(const bigint_t* a);
uint16_t bigint_length_b(const bigint_t* a);
uint16_t bigint_length_B(const bigint_t* a);
void   bigint_copy(bigint_t* dest, const bigint_t* src);
void   bigint_add_u(bigint_t* dest, const bigint_t* a, const bigint_t* b);
void   bigint_add_scale_u(bigint_t* dest, const bigint_t* a, uint16_t scale);
void   bigint_sub_u(bigint_t* dest, const bigint_t* a, const bigint_t* b);
int8_t bigint_cmp_u(const bigint_t * a, const bigint_t * b);
void   bigint_add_s(bigint_t* dest, const bigint_t* a, const bigint_t* b);
void   bigint_sub_s(bigint_t* dest, const bigint_t* a, const bigint_t* b);
int8_t bigint_cmp_s(const bigint_t* a, const bigint_t* b);
void   bigint_shiftleft(bigint_t* a, uint16_t shift);
void   bigint_shiftright(bigint_t* a, uint16_t shift);
void   bigint_xor(bigint_t* dest, const bigint_t* a);
void   bigint_set_zero(bigint_t* a);
void   bigint_mul_u(bigint_t* dest, const bigint_t* a, const bigint_t* b);
void   bigint_mul_s(bigint_t* dest, const bigint_t* a, const bigint_t* b);
void   bigint_square(bigint_t* dest, const bigint_t* a);
void   bigint_sub_u_bitscale(bigint_t* a, const bigint_t* b, uint16_t bitscale);
void   bigint_reduce(bigint_t* a, const bigint_t* r);
void   bigint_expmod_u(bigint_t* dest, const bigint_t* a, const bigint_t* exp, const bigint_t* r);
void   bigint_gcdext(bigint_t* gcd, bigint_t* a, bigint_t* b, const bigint_t* x, const bigint_t* y);
void   bigint_inverse(bigint_t* dest, const bigint_t* a, const bigint_t* m);
void   bigint_changeendianess(bigint_t* a);
/******************************************************************************/

#endif /*BIGINT_H_*/
