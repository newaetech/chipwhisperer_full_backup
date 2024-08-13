/* grain.c */
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
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3 or later
 * 
 */


#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "grain.h"


#define GRAIN_REVERSEKEY

/* s0, s1, s2, ..., s78, s79 */
#define S(i) ((ctx->lfsr[9-((i)/8)])>>(7-((i)%8)))
/* b0, b1, b2, ..., b78, b79 */
#define B(i) ((ctx->nfsr[9-((i)/8)])>>(7-((i)%8)))
#define _B(i) (((ctx->nfsr[9-((i)/8)])>>(7-((i)%8)))&1)


const uint8_t h_lut[4] PROGMEM = {0x4C, 0xB6, 0xD3, 0x26};

#ifdef GRAIN_BADOPTIMISATION
const uint8_t g_lut[128] PROGMEM = {
        0xF0, 0xA5, 0x0F, 0x5A, 0x0F, 0x5A, 0xF0, 0xA5, 0x0F, 0x5A, 0xF0, 0xA5, 0xF0, 0x5A, 0x0F, 0x0F, 
        0xC3, 0x96, 0x3C, 0x69, 0x3C, 0x69, 0xC3, 0x96, 0x9C, 0xC9, 0x63, 0x36, 0x63, 0xC9, 0x9C, 0x9C, 
        0x0F, 0x5A, 0x0F, 0x5A, 0xF0, 0xA5, 0xF0, 0x5A, 0xF0, 0xA5, 0xF0, 0xA5, 0x0F, 0xA5, 0x0F, 0xF0, 
        0x3C, 0x69, 0x3C, 0x69, 0xC3, 0x96, 0xC3, 0x69, 0x63, 0x36, 0x63, 0x36, 0x9C, 0x36, 0x9C, 0x63, 
        0x0F, 0xD2, 0xF0, 0x2D, 0xF0, 0x2D, 0x0F, 0xD2, 0xF0, 0x2D, 0x0F, 0xD2, 0x0F, 0x2D, 0xF0, 0x78, 
        0x3C, 0xE1, 0xC3, 0x1E, 0xC3, 0x1E, 0x3C, 0xE1, 0x63, 0xBE, 0x9C, 0x41, 0x9C, 0xBE, 0x63, 0xEB, 
        0x00, 0xDD, 0x00, 0xDD, 0xFF, 0x22, 0xFF, 0xDD, 0xFF, 0x22, 0xFF, 0x22, 0x00, 0x22, 0xF0, 0x87, 
        0xF3, 0x2E, 0xF3, 0x2E, 0x0C, 0xD1, 0x0C, 0x2E, 0xAC, 0x71, 0xAC, 0x71, 0x53, 0x71, 0xA3, 0xD4  };
#endif

uint8_t grain_enc(grain_ctx_t* ctx){
	uint8_t s80, s0, c1, c2;
	uint8_t i;
	/* clock the LFSR */
	s0=S(0);
	s80 =S(62) ^ S(51) ^ S(38) ^ S(23) ^ S(13) ^ s0;
	s80 &= 1;
	c1 = s80;
	for(i=0; i<10; ++i){
		c2 = (ctx->lfsr[i])>>7;
		ctx->lfsr[i] = ((ctx->lfsr[i])<<1) | c1;
		c1 = c2;
	}
	/* clock the NFSR */
	uint8_t b80;
/*	778 Byte in this variant / 617 clks enc_time */
#ifndef GRAIN_BADOPTIMISATION
    uint8_t a,b,d,e;
	b80 = B(62) ^ B(60) ^ B(52) ^ B(45) ^ 
	      B(37) ^ B(33) ^ B(28) ^ B(21) ^ 
	      B(14) ^ B( 9) ^ B( 0) ^ s0;
	b80 ^= (a = B(63) & B(60));
	b80 ^= (b = B(37) & B(33));
	b80 ^= B(15) & B( 9); // c 
	b80 ^= (d = B(60) & B(52) & B(45));
	b80 ^= (e = B(33) & B(28) & B(21));
	b80 ^= B(63) & B(45) & B(28) & B(9); // f 
	/* -- */
	b80 ^= b & B(60) & B(52); // g 
	b80 ^= a & B(21) & B(15); // h 
	b80 ^= d & B(63) & B(37); // i 
	b80 ^= e & B(15) & B( 9); // j 
	b80 ^= e & B(52) & B(45) & B(37); // k
#else
	/* let's reorder the bits */
	uint16_t x; 

/*
	x  = _B(21); x<<=1;
	x |= _B(33); x<<=1;
	x |= _B(9) ; x<<=1;
	x |= _B(45); x<<=1;
	x |= _B(52); x<<=1;
	x |= _B(37); x<<=1;
	x |= _B(60); x<<=1;
	x |= _B(28); x<<=1;
	x |= _B(15); x<<=1;
	x |= _B(63);
*/
	x  = ((ctx->nfsr[8])&0x41)<<1; // B15 & B09
	x |= ((ctx->nfsr[2])&0x09);    // B63 & B60 
//	x |= ((ctx->nfsr[4])&0x04)<<4; // B45
	x |= (((ctx->nfsr[5])&0x44) | 
	      ((ctx->nfsr[3])&0x08) | 
	      (((((ctx->nfsr[7])&0x04)<<3) |((ctx->nfsr[4])&0x04))<<2) )<<2; // B37 & B33
//	x |= ((ctx->nfsr[3])&0x08)<<2; // B52
	x |= ((ctx->nfsr[6])&0x08)>>1; // B28
//	x |= ((ctx->nfsr[7])&0x04)<<7; // B21 


	b80 = pgm_read_byte(g_lut+(x/8))>>(x%8);
	b80 ^= s0 ^ B(62) ^ B(14) ^ B(0);
#endif
	c1 = b80 & 1;
	for(i=0; i<10; ++i){
		c2 = (ctx->nfsr[i])>>7;
		ctx->nfsr[i] = ((ctx->nfsr[i])<<1) | c1;
		c1 = c2;
	}
	/* now the h function */
	uint8_t h;
	i = (S(2)&1) | 
	    ((S(24)&1) << 1) |
	    ((S(45)&1) << 2) |
	    ((S(63)&1) << 3) |
	    ((B(62)&1) << 4);
	
	h = (pgm_read_byte(h_lut+(i/8)))>>(i%8);
	
	h ^= B(0) ^ B(1) ^ B(3) ^ B(9) ^ B(30) ^ B(42) ^ B(55);
	return (h&1);
}

uint8_t grain_getbyte(grain_ctx_t* ctx){
	uint8_t i=0;
	uint8_t r=0;
	do{
		r >>= 1;
		r |= grain_enc(ctx)?0x80:0x00;
	}while(++i<8);
	return r;
}

#ifdef GRAIN_REVERSEKEY

static
uint8_t reverse_bits(uint8_t a){
	uint8_t lut[16] = {
		0x0, 0x8, 0x4, 0xC,   /* 0000 1000 0100 1100 */
		0x2, 0xA, 0x6, 0xE,   /* 0010 1010 0110 1110 */
		0x1, 0x9, 0x5, 0xD,   /* 0001 1001 0101 1101 */
		0x3, 0xB, 0x7, 0xF }; /* 0011 1011 0111 1111 */
	uint8_t x;
	x = ((lut[a&0xf]) << 4) | lut[a>>4];
	return x;
}
#else

#define reverse_bits(a) (a)

#endif

void grain_init(const void* key, const void* iv, grain_ctx_t* ctx){
	uint8_t i,t;
	
	/* load the 80bit key */
	for(i=0; i<10; ++i){
		ctx->nfsr[9-i] = reverse_bits(((uint8_t*)key)[i]);
	}
	/* load the 64bit iv */
	for(i=0; i<8; ++i){
		ctx->lfsr[9-i] = reverse_bits(((uint8_t*)iv)[i]);
	}
	/* set the other bits of iv to 1 */
	ctx->lfsr[0] = ctx->lfsr[1] = 0xFF;
	
	/* run it 160 times */
	for(i=0; i<160; ++i){
		t = grain_enc(ctx);
		(ctx->lfsr[0]) ^= t;
		(ctx->nfsr[0]) ^= t;
	}
}






