/* rc5.c */
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
/* rc5.c a C implementation of RC5 for AVR microcontrollers
 * 
 * author: Daniel Otte 
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * this implementation is limited to 64bit blocks and a maximum of 255 rounds
 * 
 */

#include <stdint.h>
#include <stdlib.h> /* malloc() & free() */
#include <string.h> /* memset() & memcpy() */
#include "rc5.h" 
 

#define A (((uint32_t*)buffer)[0])
#define B (((uint32_t*)buffer)[1])
#define ROTL32(v,n) (((v)<<(n))|((v)>>(32-(n))))
#define ROTR32(v,n) (((v)>>(n))|((v)<<(32-(n))))

void rc5_enc(void* buffer, const rc5_ctx_t* ctx){
	uint8_t i;
	A += ctx->s[0];
	B += ctx->s[1];
	for(i=0; i<ctx->rounds; ++i){
		A = ROTL32(A^B, B&31) + ctx->s[(i+1)*2+0];
		B = ROTL32(A^B, A&31) + ctx->s[(i+1)*2+1];
	} 
}

void rc5_dec(void* buffer, const rc5_ctx_t* ctx){
	uint8_t i;
	for(i=ctx->rounds; i>0; --i){
		B = ROTR32(B - ctx->s[i*2+1], A&31) ^ A;
		A = ROTR32(A - ctx->s[i*2+0], B&31) ^ B;
	} 
	B -= ctx->s[1];
	A -= ctx->s[0];
}
/*
P32 = 10110111111000010101000101100011 = b7e15163
Q32 = 10011110001101110111100110111001 = 9e3779b9
*/
#define P32 0xb7e15163
#define Q32 0x9e3779b9


void rc5_init(void* key, uint16_t keysize_b, uint8_t rounds, rc5_ctx_t* ctx){
	uint16_t c,n,m,j,i,t;
	uint32_t a,b,l[(keysize_b+31)/32];
	ctx->rounds = rounds;
	t=2*(rounds+1);
	c=(keysize_b+31)/32;
	ctx->s = malloc(t*sizeof(uint32_t));
	
	memset(l, 0, sizeof(uint32_t)*c);
	memcpy(l, key, (keysize_b+7)/8);
	
	ctx->s[0] = P32;
	for(i=1; i<t; ++i){
		ctx->s[i] = ctx->s[i-1] + Q32;
	}
	
	m = ((t>c)?t:c)*3;
	i=j=0;
	a=b=0;
	for(n=0; n<m; ++n){
		a=ctx->s[i]=ROTL32(ctx->s[i]+a+b, 3);
		b=l[j]=ROTL32(l[j]+a+b, (a+b)&31);
		i=(i+1)%t;
		j=(j+1)%c;
	}
}

void rc5_free(rc5_ctx_t* ctx){
	if(ctx->s)
		free(ctx->s);
}

