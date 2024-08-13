/* salsa20.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "salsa20.h"


#define ROTL32(a,n) (((a)<<(n))|((a)>>(32-(n))))

static
void quaterround(uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d){
	*b ^= ROTL32(*a + *d,  7);
	*c ^= ROTL32(*b + *a,  9);
	*d ^= ROTL32(*c + *b, 13);
	*a ^= ROTL32(*d + *c, 18);
}

static
void rowround(uint32_t* a){
	quaterround(a+ 0, a+ 1, a+ 2, a+ 3);
	quaterround(a+ 5, a+ 6, a+ 7, a+ 4);
	quaterround(a+10, a+11, a+ 8, a+ 9);
	quaterround(a+15, a+12, a+13, a+14);
}

static
void columnround(uint32_t* a){
	quaterround(a+ 0, a+ 4, a+ 8, a+12);
	quaterround(a+ 5, a+ 9, a+13, a+ 1);
	quaterround(a+10, a+14, a+ 2, a+ 6);
	quaterround(a+15, a+ 3, a+ 7, a+11);
}

static
void doubleround(uint32_t* a){
	columnround(a);
	rowround(a);

}


void salsa20_hash(uint32_t* a){
	uint8_t i;
	uint32_t b[16];
	memcpy(b, a, 64);
	for(i=0; i<10; ++i){
		doubleround(a);
	}
	for(i=0; i<16; ++i){
		a[i] += b[i];
	}
}

const uint8_t sigma[] PROGMEM = {'e','x','p','a','n','d',' ','3','2','-','b','y','t','e',' ','k'};
const uint8_t theta[] PROGMEM = {'e','x','p','a','n','d',' ','1','6','-','b','y','t','e',' ','k'};

void salsa_k32(uint32_t* dest, const uint32_t* k, const uint32_t* n){
	memcpy_P(dest+ 0, sigma+ 0, 4);
	memcpy(  dest+ 4, k+ 0, 16);
	memcpy_P(dest+20, sigma+ 4, 4);
	memcpy(  dest+24, n+ 0, 16);
	memcpy_P(dest+40, sigma+ 8, 4);
	memcpy(  dest+44, k+16, 16);
	memcpy_P(dest+60, sigma+12, 4);
	salsa20_hash(dest);
}

void salsa_k16(uint32_t* dest, const uint32_t* k, const uint32_t* n){
	memcpy_P(dest+ 0, theta+ 0, 4);
	memcpy(  dest+ 4, k+ 0, 16);
	memcpy_P(dest+20, theta+ 4, 4);
	memcpy(  dest+24, n+ 0, 16);
	memcpy_P(dest+40, theta+ 8, 4);
	memcpy(  dest+44, k+ 0, 16);
	memcpy_P(dest+60, theta+12, 4);
	salsa20_hash(dest);
}

void salsa20_genBlock256(void* dest, const void* k, const void* iv, uint64_t i){
	uint32_t n[4];
	memcpy(n, iv, 8);
	memcpy(n+8, &i, 8);
	salsa_k32((uint32_t*)dest, (uint32_t*)k, n);
}

void salsa20_genBlock128(void* dest, const void* k, const void* iv, uint64_t i){
	uint32_t n[4];
	memcpy(n, iv, 8);
	memcpy(n+8, &i, 8);
	salsa_k16((uint32_t*)dest, (uint32_t*)k, n);
}


void salsa20_init(void* key, uint16_t keylength_b, void* iv, salsa20_ctx_t* ctx){
	if(keylength_b==256){
		memcpy_P((ctx->a.v8+ 0), sigma+ 0, 4);
		memcpy_P((ctx->a.v8+20), sigma+ 4, 4);
		memcpy_P((ctx->a.v8+40), sigma+ 8, 4);
		memcpy(  (ctx->a.v8+44), (uint8_t*)key+16, 16);
		memcpy_P((ctx->a.v8+60), sigma+12, 4);
	}else{
		memcpy_P((ctx->a.v8+ 0), theta+ 0, 4);
		memcpy_P((ctx->a.v8+20), theta+ 4, 4);
		memcpy_P((ctx->a.v8+40), theta+ 8, 4);
		memcpy(  (ctx->a.v8+44), (uint8_t*)key+ 0, 16);
		memcpy_P((ctx->a.v8+60), theta+12, 4);
	}
	memcpy(  (ctx->a.v8+ 4), key, 16);
	memset(  (ctx->a.v8+24), 0, 16);
	if(iv){
		memcpy(  (ctx->a.v8+24), iv, 8);
	}
	ctx->buffer_idx=64;
}

uint8_t salsa20_gen(salsa20_ctx_t* ctx){
	if(ctx->buffer_idx==64){
		memcpy(ctx->buffer, ctx->a.v8, 64);
		salsa20_hash((uint32_t*)(ctx->buffer));
		ctx->a.v64[4] += 1;
		ctx->buffer_idx = 0;
	}
	return ctx->buffer[ctx->buffer_idx++];
}

