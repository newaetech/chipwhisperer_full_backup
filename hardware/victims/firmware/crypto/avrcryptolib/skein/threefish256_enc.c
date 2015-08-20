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

//#define THREEFISH_KEY_CONST 0x5555555555555555LL /* 2**64/3 */
#define THREEFISH_KEY_CONST 0x1BD11BDAA9FC1A22LL

#define K(s) (((uint64_t*)key)[(s)])
#define T(s) (((uint64_t*)tweak)[(s)])

void threefish256_init(const void* key, const void* tweak, threefish256_ctx_t* ctx){
	memcpy(ctx->k, key, 4*8);
	if(tweak){
		memcpy(ctx->t, tweak, 2*8);
		ctx->t[2] = T(0) ^ T(1);
	}else{
		memset(ctx->t, 0, 3*8);
	}
	uint8_t i;
	ctx->k[4] = THREEFISH_KEY_CONST;
	for(i=0; i<4; ++i){
		ctx->k[4] ^= K(i);
	}
}

static
void add_key_4(void* data, const threefish256_ctx_t* ctx, uint8_t s){
	X(0) += ctx->k[(s+0)%5];
	X(1) += ctx->k[(s+1)%5] + ctx->t[s%3];
	X(2) += ctx->k[(s+2)%5] + ctx->t[(s+1)%3];
	X(3) += ctx->k[(s+3)%5] + s;
}

void threefish256_enc(void* data, const threefish256_ctx_t* ctx){
	uint8_t i=0,s=0;
/* old constans, changed at round 2 of the SHA-3 contest
	uint8_t r0[8] = { 5, 36, 13, 58, 26, 53, 11, 59};
	uint8_t r1[8] = {56, 28, 46, 44, 20, 35, 42, 50};
*/
	uint8_t r0[8] = {14, 52, 23,  5, 25, 46, 58, 32};
	uint8_t r1[8] = {16, 57, 40, 37, 33, 12, 22, 32};
	do{
		if(i%4==0){
			add_key_4(data, ctx, s);
			++s;
		}
		threefish_mix(data, r0[i%8]);
		threefish_mix((uint8_t*)data + 16, r1[i%8]);
		permute_4(data);
		++i;
	}while(i!=72);
	add_key_4(data, ctx, s);
}

