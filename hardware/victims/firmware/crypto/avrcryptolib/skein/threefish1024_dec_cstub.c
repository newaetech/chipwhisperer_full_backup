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
	uint8_t r0[8] = {0x69, 0x72, 0x21, 0x34, 0x42, 0x41, 0x31, 0x79};
	uint8_t r1[8] = {0x61, 0x19, 0x1a, 0x19, 0x53, 0x10, 0x31, 0x53};
	uint8_t r2[8] = {0x33, 0x40, 0x22, 0x69, 0x31, 0x22, 0x6a, 0x5b};
	uint8_t r3[8] = {0x72, 0x6b, 0x31, 0x60, 0x74, 0x71, 0x2b, 0x50};
	uint8_t r4[8] = {0x5b, 0x23, 0x53, 0x63, 0x54, 0x3b, 0x2a, 0x20};
	uint8_t r5[8] = {0x60, 0x22, 0x52, 0x11, 0x11, 0x14, 0x2b, 0x3a};
	uint8_t r6[8] = {0x7b, 0x02, 0x50, 0x43, 0x73, 0x40, 0x64, 0x5a};
	uint8_t r7[8] = {0x70, 0x70, 0x29, 0x51, 0x42, 0x7a, 0x71, 0x14}; 
	
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
