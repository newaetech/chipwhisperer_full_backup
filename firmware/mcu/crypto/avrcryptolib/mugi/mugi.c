/* mugi.c */
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
/**
 * \file	mugi.c
 * \author	Daniel Otte 
 * \email   daniel.otte@rub.de
 * \date	2009-02-15
 * \brief 	implementation of the MUGI key stream generator
 * \license	GPLv3 or later
 */
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "aes_sbox.h"
#include "mugi.h"
#include "gf256mul.h"

/*
#include "cli.h" / * only for debugging * /

void dump_mugi_ctx(mugi_ctx_t* ctx){
	uint8_t i;
	cli_putstr_P(PSTR("\r\n== MUGI CTX DUMP==\r\n a:"));
	cli_hexdump(&(ctx->a[0]), 8);
	cli_putc(' ');
	cli_hexdump(&(ctx->a[1]), 8);
	cli_putc(' ');
	cli_hexdump(&(ctx->a[2]), 8);
	cli_putstr_P(PSTR("\r\n b: "));
	for(i=0; i<4; ++i){
		cli_putstr_P(PSTR("\r\n    "));
		cli_hexdump(&(ctx->b[i*4+0]), 8);
		cli_putc(' ');
		cli_hexdump(&(ctx->b[i*4+1]), 8);
		cli_putc(' ');
		cli_hexdump(&(ctx->b[i*4+2]), 8);
		cli_putc(' ');
		cli_hexdump(&(ctx->b[i*4+3]), 8);
	}
}
*/

#define C0 0x08c9bcf367e6096all
#define C1 0x3ba7ca8485ae67bbll
#define C2 0x2bf894fe72f36e3cll

#define GF256MUL_2(a) (gf256mul(2, (a), 0x1b))

uint64_t changeendian64(uint64_t a){
	union {
		uint8_t v8[8];
		uint64_t v64;
	} r;
	r.v8[0] = ((uint8_t*)&a)[7];
	r.v8[1] = ((uint8_t*)&a)[6];
	r.v8[2] = ((uint8_t*)&a)[5];
	r.v8[3] = ((uint8_t*)&a)[4];
	r.v8[4] = ((uint8_t*)&a)[3];
	r.v8[5] = ((uint8_t*)&a)[2];
	r.v8[6] = ((uint8_t*)&a)[1];
	r.v8[7] = ((uint8_t*)&a)[0];
	return r.v64;
}

static
uint64_t rotl64(uint64_t a, uint8_t i){
	uint64_t r;
	r=changeendian64(a);
	r=(r<<i | r>>(64-i));
	r=changeendian64(r);
	return r;
}

static
uint64_t rotr64(uint64_t a, uint8_t i){
	uint64_t r;
	r=changeendian64(a);
	r=(r>>i | r<<(64-i));
	r=changeendian64(r);
	return r;
}


#define T(x) (((uint8_t*)&t)[(x)])
#define D(y) (((uint8_t*)dest)[(y)])
static void mugi_f(uint64_t* dest, uint64_t* a, uint64_t* b){
	uint64_t t;
	uint8_t i,x;
	t = (*a); 
	if(b)
		t ^= (*b);
	for(i=0; i<8; ++i)
		T(i) = pgm_read_byte(aes_sbox+T(i));
	
	x = T(0) ^ T(1) ^ T(2) ^ T(3);
	D(4) =
		  GF256MUL_2(T(0)^T(1))
		^ T(0)
		^ x;
	D(5) =
		  GF256MUL_2(T(1)^T(2))
		^ T(1)
		^ x;
	D(2) =
		  GF256MUL_2(T(2)^T(3))
		^ T(2)
		^ x;
	D(3) =
		  GF256MUL_2(T(3)^T(0))
		^ T(3)
		^ x;
	x = T(4) ^ T(5) ^ T(6) ^ T(7);
	D(0) =
		  GF256MUL_2(T(4)^T(5))
		^ T(4)
		^ x;
	D(1) =
		  GF256MUL_2(T(5)^T(6))
		^ T(5)
		^ x;
	D(6) =
		  GF256MUL_2(T(6)^T(7))
		^ T(6)
		^ x;
	D(7) =
		  GF256MUL_2(T(7)^T(4))
		^ T(7)
		^ x;
} 

static
void mugi_rho(mugi_ctx_t* ctx){
	uint64_t t,bx;
	t = ctx->a[1];
	ctx->a[1] = ctx->a[2];
	ctx->a[2] = ctx->a[0];
	ctx->a[0] = t;
	mugi_f(&t, &(ctx->a[0]), &(ctx->b[4]));
	ctx->a[1] ^= t ^ C1;
	bx = rotl64(ctx->b[10], 17);
	mugi_f(&t, &(ctx->a[0]), &bx);
	ctx->a[2] ^= t ^ C2;
} 

static
void mugi_rho_init(uint64_t* a){
	uint64_t t;
	t = a[1];
	a[1] = a[2];
	a[2] = a[0];
	a[0] = t;
	mugi_f(&t, &(a[0]), NULL);
	a[1] ^= t ^ C1;
	mugi_f(&t, &(a[0]), NULL);
	a[2] ^= t ^ C2;
} 

static
void mugi_lambda(uint64_t* b, uint64_t *a){
	uint8_t i;
	uint64_t t;
	t=b[15];
	for(i=15; i!=0; --i){
		b[i]=b[i-1];
	}
	b[0]   = t ^ *a;
	b[4]  ^= b[8];
	b[10] ^= rotl64(b[14], 32);
}

void mugi_init(const void* key, const void* iv, mugi_ctx_t* ctx){
	uint8_t i;
	uint64_t a0;
	memcpy(ctx->a, key, 128/8);
	ctx->a[2] = rotl64(ctx->a[0], 7) ^ rotr64(ctx->a[1], 7) ^ C0;
	for(i=0; i<16;i++){
		mugi_rho_init(ctx->a);
		ctx->b[15-i] = ctx->a[0];
	}
	ctx->a[0] ^= ((uint64_t*)iv)[0];
	ctx->a[1] ^= ((uint64_t*)iv)[1];
	ctx->a[2] ^= rotl64(((uint64_t*)iv)[0], 7) ^ rotr64(((uint64_t*)iv)[1], 7) ^ C0;
	for(i=0; i<16;i++){
		mugi_rho_init(ctx->a);
	}
	for(i=0; i<15;i++){
		a0 = ctx->a[0];
		mugi_rho(ctx);
		mugi_lambda(ctx->b, &a0);
	}
	a0=0x00;
}

uint64_t mugi_gen(mugi_ctx_t* ctx){
	uint64_t r;
	r=ctx->a[0];
	mugi_rho(ctx);
	mugi_lambda(ctx->b, &r);
	r=ctx->a[2];
	return r;
}
