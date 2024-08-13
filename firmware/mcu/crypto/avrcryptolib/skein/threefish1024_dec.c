/* threefish1024_enc.c */
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
void permute_inv16(void* data){
	uint64_t t;
	t = X(15);
	X(15) = X(7);
	X(7) = X(9);
	X(9) = X(1);
	X(1) = t;
	t = X(11);
	X(11) = X(5);
	X(5) = X(13);
	X(13) = X(3);
	X(3) = t;
	t = X(4);
	X(4) = X(6);
	X(6) = t;
	t = X(14);
	X(14) = X(12);
	X(12) = X(10);
	X(10) = X(8);
	X(8) = t;
}

static
void add_key_16(void* data, const threefish1024_ctx_t* ctx, uint8_t s){
	uint8_t i;
	for(i=0; i<13; ++i){
		X(i) -= ctx->k[(s+i)%17];
	}
	X(13) -= ctx->k[(s+13)%17] + ctx->t[s%3];
	X(14) -= ctx->k[(s+14)%17] + ctx->t[(s+1)%3];
	X(15) -= ctx->k[(s+15)%17] + s;
}

void threefish1024_dec(void* data, const threefish1024_ctx_t* ctx){
	uint8_t i=0,s=20;
	/* old round constants
	uint8_t r0[8] = {47, 58, 17, 28, 34, 33, 25, 55};
	uint8_t r1[8] = {49,  7,  6,  7, 43,  8, 25, 43};
	uint8_t r2[8] = {27, 32, 18, 47, 25, 18, 46, 37};
	uint8_t r3[8] = {58, 45, 25, 48, 60, 57, 13, 40};
	uint8_t r4[8] = {37, 19, 43, 51, 44, 21, 14, 16};
	uint8_t r5[8] = {48, 18, 42,  9,  9, 12, 13, 22};
	uint8_t r6[8] = {53,  2, 40, 35, 59, 32, 52, 38};
	uint8_t r7[8] = {56, 56, 15, 41, 34, 54, 57, 12};
	*/
	uint8_t r0[8] = {  9, 31, 16, 41,  5, 33, 38, 24};
	uint8_t r1[8] = { 48, 44, 34,  9, 20,  4, 19, 13};
	uint8_t r2[8] = { 35, 47, 56, 37, 48, 51, 10,  8};
	uint8_t r3[8] = { 52, 46, 51, 31, 41, 13, 55, 47};
	uint8_t r4[8] = { 23, 19,  4, 12, 47, 34, 49,  8};
	uint8_t r5[8] = { 31, 42, 53, 47, 28, 41, 18, 17};
	uint8_t r6[8] = { 37, 44, 42, 44, 16, 59, 23, 22};
	uint8_t r7[8] = { 20, 25, 41, 30, 25, 17, 52, 37};
	do{
		if(i%4==0){
			add_key_16(data, ctx, s);
			--s;
		}
		permute_inv16(data);
		threefish_invmix((uint8_t*)data +  0, r0[i%8]);
		threefish_invmix((uint8_t*)data + 16, r1[i%8]);
		threefish_invmix((uint8_t*)data + 32, r2[i%8]);
		threefish_invmix((uint8_t*)data + 48, r3[i%8]);
		threefish_invmix((uint8_t*)data + 64, r4[i%8]);
		threefish_invmix((uint8_t*)data + 80, r5[i%8]);
		threefish_invmix((uint8_t*)data + 96, r6[i%8]);
		threefish_invmix((uint8_t*)data +112, r7[i%8]);
		++i;
	}while(i!=80);
	add_key_16(data, ctx, s);
}
