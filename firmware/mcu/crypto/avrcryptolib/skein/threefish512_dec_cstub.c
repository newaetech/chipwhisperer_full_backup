/* threefish512_dec.c */
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
 * \date    2009-03-22
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

static
void add_key_8(void* data, const threefish512_ctx_t* ctx, uint8_t s){
	uint8_t i;
	for(i=0; i<5; ++i){
		X(i) -= ctx->k[(s+i)%9];
	}
	X(5) -= ctx->k[(s+5)%9] + ctx->t[s%3];
	X(6) -= ctx->k[(s+6)%9] + ctx->t[(s+1)%3];
	X(7) -= ctx->k[(s+7)%9] + s;
}

void threefish512_dec(void* data, const threefish512_ctx_t* ctx){
	uint8_t i=0,s=18;
	uint8_t r0[8] = {0x41, 0x4b, 0x59, 0x41, 0x32, 0x42, 0x60, 0x5a};
	uint8_t r1[8] = {0x63, 0x32, 0x33, 0x61, 0x14, 0x2a, 0x24, 0x4a};
	uint8_t r2[8] = {0x59, 0x13, 0x51, 0x10, 0x72, 0x29, 0x53, 0x62};
	uint8_t r3[8] = {0x43, 0x11, 0x2a, 0x52, 0x19, 0x33, 0x49, 0x7b};
	do{
		if(i%4==0){
			add_key_8(data, ctx, s);
			--s;
		}
		permute_inv8(data);
		threefish_invmix((uint8_t*)data +  0, r0[i%8]);
		threefish_invmix((uint8_t*)data + 16, r1[i%8]);
		threefish_invmix((uint8_t*)data + 32, r2[i%8]);
		threefish_invmix((uint8_t*)data + 48, r3[i%8]);
		++i;
	}while(i!=72);
	add_key_8(data, ctx, s);
}

