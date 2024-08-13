/* threefish512_enc.c */
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
void permute_8(void* data){
	uint64_t t;
	t = X(0);
	X(0) = X(2);
	X(2) = X(4);
	X(4) = X(6);
	X(6) = t;
	t = X(3);
	X(3) = X(7);
	X(7) = t;
}
/*
static
void permute_inv8(void* data){
	uint64_t t;
	t = X(6);
	X(6) = X(4);
	X(4) = X(2);
	X(2) = X(0);
	X(0) = t;
	t = X(7);
	X(7) = X(3);
	X(3) = t;
}
*/

//#define THREEFISH_KEY_CONST 0x5555555555555555LL /* 2**64/3 */
#define THREEFISH_KEY_CONST 0x1BD11BDAA9FC1A22LL

#define K(s) (((uint64_t*)key)[(s)])
#define T(s) (((uint64_t*)tweak)[(s)])

void threefish512_init(const void* key, const void* tweak, threefish512_ctx_t* ctx){
	memcpy(ctx->k, key, 8*8);
	if(tweak){
		memcpy(ctx->t, tweak, 2*8);
		ctx->t[2] = T(0) ^ T(1);
	}else{
		memset(ctx->t, 0, 3*8);
	}
	uint8_t i;
	ctx->k[8] = THREEFISH_KEY_CONST;
	for(i=0; i<8; ++i){
		ctx->k[8] ^= K(i);
	}
}

static
void add_key_8(void* data, const threefish512_ctx_t* ctx, uint8_t s){
	uint8_t i;
	for(i=0; i<5; ++i){
		X(i) += ctx->k[(s+i)%9];
	}
	X(5) += ctx->k[(s+5)%9] + ctx->t[s%3];
	X(6) += ctx->k[(s+6)%9] + ctx->t[(s+1)%3];
	X(7) += ctx->k[(s+7)%9] + s;
}

void threefish512_enc(void* data, const threefish512_ctx_t* ctx){
	uint8_t i=0,s=0;
	/* old constans, changed at round 2 of the SHA-3 contest
	uint8_t r0[8] = {38, 48, 34, 26, 33, 39, 29, 33};
	uint8_t r1[8] = {30, 20, 14, 12, 49, 27, 26, 51};
	uint8_t r2[8] = {50, 43, 15, 58,  8, 41, 11, 39};
	uint8_t r3[8] = {53, 31, 27,  7, 42, 14,  9, 35};
	*/
	uint8_t r0[8] = {46, 33, 17, 44, 39, 13, 25,  8};
	uint8_t r1[8] = {36, 27, 49,  9, 30, 50, 29, 35};
	uint8_t r2[8] = {19, 14, 36, 54, 34, 10, 39, 56};
	uint8_t r3[8] = {37, 42, 39, 56, 24, 17, 43, 22};
	do{
		if(i%4==0){
			add_key_8(data, ctx, s);
			++s;
		}
		threefish_mix((uint8_t*)data +  0, r0[i%8]);
		threefish_mix((uint8_t*)data + 16, r1[i%8]);
		threefish_mix((uint8_t*)data + 32, r2[i%8]);
		threefish_mix((uint8_t*)data + 48, r3[i%8]);
		permute_8(data);
		++i;
	}while(i!=72);
	add_key_8(data, ctx, s);
}

