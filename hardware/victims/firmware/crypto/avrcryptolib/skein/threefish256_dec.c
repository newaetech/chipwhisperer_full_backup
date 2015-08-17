/* threefish256_enc.c */
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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-12
 * \license GPLv3 or later
 *
 *
 *
 */

#include <stdint.h>
#include <string.h>
#include "threefish.h"

#define X(a) (((uint64_t*)data)[(a)])
static
void permute_4(void* data){
	uint64_t t;
	t = X(1);
	X(1) = X(3);
	X(3) = t;
}

#define K(s) (((uint64_t*)key)[(s)])
#define T(s) (((uint64_t*)tweak)[(s)])

static
void add_key_4(void* data, const threefish256_ctx_t* ctx, uint8_t s){
	X(0) -= ctx->k[(s+0)%5];
	X(1) -= ctx->k[(s+1)%5] + ctx->t[s%3];
	X(2) -= ctx->k[(s+2)%5] + ctx->t[(s+1)%3];
	X(3) -= ctx->k[(s+3)%5] + s;
}

void threefish256_dec(void* data, const threefish256_ctx_t* ctx){
	uint8_t i=0,s=18;
/* old constans, changed at round 2 of the SHA-3 contest
	uint8_t r0[8] = {59, 11, 53, 26, 58, 13, 36,  5};
	uint8_t r1[8] = {50, 42, 35, 20, 44, 46, 28, 56};
*/
	uint8_t r0[8] = { 32, 58, 46, 25,  5, 23, 52, 14};
	uint8_t r1[8] = { 32, 22, 12, 33, 37, 40, 57, 16};

	do{
		if(i%4==0){
			add_key_4(data, ctx, s);
			--s;
		}
		permute_4(data);
		threefish_invmix(data, r0[i%8]);
		threefish_invmix((uint8_t*)data + 16, r1[i%8]);
		++i;
	}while(i!=72);
	add_key_4(data, ctx, s);
}

