/* khazad.c */
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
#include <avr/pgmspace.h>
#include <string.h>
#include "gf256mul.h"
#include "memxor.h"
#include "khazad.h"

/*
  | | | |     | | | |
  V V V V     V V V V
 +-------+   +-------+
 |   P   |   |   Q   |
 +-------+   +-------+
  | | \ \     / / | |
  | |  \ \   / /  | |
  | |   \ \ / /   | |
  | |    \ X /    | |
  | |     X X     | |
  | |    / X \    | |
  | |   / / \ \   | |
  | |  / /   \ \  | |
  | | / /     \ \ | |
  | | | |     | | | |
  V V V V     V V V V
 +-------+   +-------+
 |   Q   |   |   P   |
 +-------+   +-------+
  | | \ \     / / | |
  | |  \ \   / /  | |
  | |   \ \ / /   | |
  | |    \ X /    | |
  | |     X X     | |
  | |    / X \    | |
  | |   / / \ \   | |
  | |  / /   \ \  | |
  | | / /     \ \ | |
  | | | |     | | | |
  V V V V     V V V V
 +-------+   +-------+
 |   P   |   |   Q   |
 +-------+   +-------+
  | | | |    | | | |
  V V V V    V V V V


P:
3x Fx Ex 0x 5x 4x Bx Cx Dx Ax 9x 6x 7x 8x 2x 1x
Q:
9x Ex 5x 6x Ax 2x 3x Cx Fx 0x 4x Dx 7x Bx 1x 8x
*/

static const uint8_t pq_lut[16] PROGMEM = {
	0x39, 0xFE, 0xE5, 0x06, 0x5A, 0x42, 0xB3, 0xCC,
	0xDF, 0xA0, 0x94, 0x6D, 0x77, 0x8B, 0x21, 0x18
};

uint8_t khazad_sbox(uint8_t a){
	uint8_t b,c,d,e;
	b = pgm_read_byte(pq_lut+(a>>4))&0xf0;
	c = pgm_read_byte(pq_lut+(a&0xf))&0x0f;
	d = (b>>2)&0x0c;
	e = (c<<2)&0x30;
	b = (b&0xc0)|e;
	c = (c&0x03)|d;
	b = pgm_read_byte(pq_lut+(b>>4))<<4;
	c = pgm_read_byte(pq_lut+(c&0xf))>>4;
	d = (b>>2)&0x0c;
	e = (c<<2)&0x30;
	b = (b&0xc0)|e;
	c = (c&0x03)|d;
	b = pgm_read_byte(pq_lut+(b>>4))&0xf0;
	c = pgm_read_byte(pq_lut+(c&0xf))&0x0f;
	return b|c;
}

static void gamma_1(uint8_t* a){
	uint8_t i;
	for(i=0; i<8; ++i){
		*a = khazad_sbox(*a);
		a++;
	}
}

/******************************************************************************/
/* p8 (x) = x^8 + x^4 + x^3 + x^2 + 1 */
#define POLYNOM 0x1D

/*
 * 01x 03x 04x 05x 06x 08x 0Bx 07x
 * 03x 01x 05x 04x 08x 06x 07x 0Bx
 * 04x 05x 01x 03x 0Bx 07x 06x 08x
 * 05x 04x 03x 01x 07x 0Bx 08x 06x
 * 06x 08x 0Bx 07x 01x 03x 04x 05x
 * 08x 06x 07x 0Bx 03x 01x 05x 04x
 * 0Bx 07x 06x 08x 04x 05x 01x 03x
 * 07x 0Bx 08x 06x 05x 04x 03x 01x
 */

static const uint8_t h[8][4] PROGMEM = {
	{ 0x13, 0x45, 0x68, 0xB7 },
	{ 0x31, 0x54, 0x86, 0x7B },
	{ 0x45, 0x13, 0xB7, 0x68 },
	{ 0x54, 0x31, 0x7B, 0x86 },
	{ 0x68, 0xB7, 0x13, 0x45 },
	{ 0x86, 0x7B, 0x31, 0x54 },
	{ 0xB7, 0x68, 0x45, 0x13 },
	{ 0x7B, 0x86, 0x54, 0x31 }
};

static void theta(uint8_t* a){
	uint8_t i,j,x,accu;
	uint8_t c[8];
	uint8_t *hp;
	hp = (uint8_t*)h;
	for(i=0; i<8; ++i){
		accu = 0;
		for(j=0; j<4; ++j){
			x = pgm_read_byte(hp++);
			accu ^= gf256mul(*a++, x>>4, POLYNOM);
			accu ^= gf256mul(*a++, x&0xf, POLYNOM);
		}
		a -= 8;
		c[i] = accu;
	}
	memcpy(a, c, 8);
}

/******************************************************************************/

static void khazad_round(uint8_t* a, const uint8_t* k){
	gamma_1(a);
	theta(a);
	memxor(a, k, 8);
}

/******************************************************************************/

void khazad_init(const void* key, khazad_ctx_t* ctx){
	uint8_t c[8];
	uint8_t i,r=0;
	for(i=0; i<8; ++i){
		c[i] = khazad_sbox(r*8+i);
	}
	memcpy(ctx->k[r], (uint8_t*)key+8, 8);
	khazad_round(ctx->k[r], c);
	memxor(ctx->k[r], (uint8_t*)key, 8);
	r=1;
	for(i=0; i<8; ++i){
			c[i] = khazad_sbox(r*8+i);
	}
	memcpy(ctx->k[r], ctx->k[r-1], 8);
	khazad_round(ctx->k[r], c);
	memxor(ctx->k[r], (uint8_t*)key+8, 8);
	for(r=2; r<9; ++r){
		for(i=0; i<8; ++i){
				c[i] = khazad_sbox(r*8+i);
		}
		memcpy(ctx->k[r], ctx->k[r-1], 8);
		khazad_round(ctx->k[r], c);
		memxor(ctx->k[r], ctx->k[r-2], 8);
	}
}

/******************************************************************************/

void khazad_enc(void* buffer, const khazad_ctx_t* ctx){
	uint8_t r;
	memxor(buffer, ctx->k[0], 8);
	for(r=1; r<8; ++r){
		khazad_round(buffer, ctx->k[r]);
	}
	gamma_1(buffer);
	memxor(buffer, ctx->k[8], 8);
}

/******************************************************************************/

void khazad_dec(void* buffer, const khazad_ctx_t* ctx){
	uint8_t r=7;
	memxor(buffer, ctx->k[8], 8);
	gamma_1(buffer);
	do{
		memxor(buffer, ctx->k[r--], 8);
		theta(buffer);
		gamma_1(buffer);
	}while(r);
	memxor(buffer, ctx->k[0], 8);
}




