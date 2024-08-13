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
void permute_16(void* data){
	uint64_t t;
	t = X(1);
	X(1) = X(9);
	X(9) = X(7);
	X(7) = X(15);
	X(15) = t;
	t = X(3);
	X(3) = X(13);
	X(13) = X(5);
	X(5) = X(11);
	X(11) = t;
	t = X(4);
	X(4) = X(6);
	X(6) = t;
	t = X(8);
	X(8) = X(10);
	X(10) = X(12);
	X(12) = X(14);
	X(14) = t;
}

//#define THREEFISH_KEY_CONST 0x5555555555555555LL /* 2**64/3 */
#define THREEFISH_KEY_CONST 0x1BD11BDAA9FC1A22LL

#define K(s) (((uint64_t*)key)[(s)])
#define T(s) (((uint64_t*)tweak)[(s)])

void threefish1024_init(const void* key, const void* tweak, threefish1024_ctx_t* ctx){
	memcpy(ctx->k, key, 16*8);
	if(tweak){
		memcpy(ctx->t, tweak, 2*8);
		ctx->t[2] = T(0) ^ T(1);
	}else{
		memset(ctx, 0, 3*8);
	}
	uint8_t i;
	ctx->k[16] = THREEFISH_KEY_CONST;
	for(i=0; i<16; ++i){
		ctx->k[16] ^= K(i);
	}
}

static
void add_key_16(void* data, const threefish1024_ctx_t* ctx, uint8_t s){
	uint8_t i;
	for(i=0; i<13; ++i){
		X(i) += ctx->k[(s+i)%17];
	}
	X(13) += ctx->k[(s+13)%17] + ctx->t[s%3];
	X(14) += ctx->k[(s+14)%17] + ctx->t[(s+1)%3];
	X(15) += ctx->k[(s+15)%17] + s;
}

void threefish1024_enc(void* data, const threefish1024_ctx_t* ctx){
	uint8_t i=0,s=0;
	/* old constans, changed at round 2 of the SHA-3 contest
	uint8_t r0[8] = {55, 25, 33, 34, 28, 17, 58, 47};
	uint8_t r1[8] = {43, 25,  8, 43,  7,  6,  7, 49};
	uint8_t r2[8] = {37, 46, 18, 25, 47, 18, 32, 27};
	uint8_t r3[8] = {40, 13, 57, 60, 48, 25, 45, 58};
	uint8_t r4[8] = {16, 14, 21, 44, 51, 43, 19, 37};
	uint8_t r5[8] = {22, 13, 12,  9,  9, 42, 18, 48};
	uint8_t r6[8] = {38, 52, 32, 59, 35, 40,  2, 53};
	uint8_t r7[8] = {12, 57, 54, 34, 41, 15, 56, 56};
	*/
	uint8_t r0[8] = {24, 38, 33,  5, 41, 16, 31,  9};
	uint8_t r1[8] = {13, 19,  4, 20,  9, 34, 44, 48};
	uint8_t r2[8] = { 8, 10, 51, 48, 37, 56, 47, 35};
	uint8_t r3[8] = {47, 55, 13, 41, 31, 51, 46, 52};
	uint8_t r4[8] = { 8, 49, 34, 47, 12,  4, 19, 23};
	uint8_t r5[8] = {17, 18, 41, 28, 47, 53, 42, 31};
	uint8_t r6[8] = {22, 23, 59, 16, 44, 42, 44, 37};
	uint8_t r7[8] = {37, 52, 17, 25, 30, 41, 25, 20};
	do{
		if(i%4==0){
			add_key_16(data, ctx, s);
			++s;
		}
		threefish_mix((uint8_t*)data +  0, r0[i%8]);
		threefish_mix((uint8_t*)data + 16, r1[i%8]);
		threefish_mix((uint8_t*)data + 32, r2[i%8]);
		threefish_mix((uint8_t*)data + 48, r3[i%8]);
		threefish_mix((uint8_t*)data + 64, r4[i%8]);
		threefish_mix((uint8_t*)data + 80, r5[i%8]);
		threefish_mix((uint8_t*)data + 96, r6[i%8]);
		threefish_mix((uint8_t*)data +112, r7[i%8]);
		permute_16(data);
		++i;
	}while(i!=80);
	add_key_16(data, ctx, s);
}
