/* jh_simple_speed.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2010 Daniel Otte (daniel.otte@rub.de)

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
#include <stdlib.h>
#include <string.h>
#include "memxor.h"
#include "jh_simple.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

const static uint8_t sbox0[] PROGMEM =
	{  9,  0,  4, 11, 13, 12,  3, 15,  1, 10,  2,  6,  7,  5,  8, 14 };
const static uint8_t sbox1[] PROGMEM =
	{  3, 12,  6, 13,  5,  7,  1,  9, 15,  2,  0,  4, 11, 10, 14,  8 };

const static uint8_t round_const_0[] PROGMEM = {
  0x6a, 0x09, 0xe6, 0x67, 0xf3, 0xbc, 0xc9, 0x08,
  0xb2, 0xfb, 0x13, 0x66, 0xea, 0x95, 0x7d, 0x3e,
  0x3a, 0xde, 0xc1, 0x75, 0x12, 0x77, 0x50, 0x99,
  0xda, 0x2f, 0x59, 0x0b, 0x06, 0x67, 0x32, 0x2a,
};

static
uint8_t jh_l(uint8_t v, uint8_t w){
	v ^= ((w<<1)^(w>>3)^((w>>2)&2))&0xf;
	w ^= ((v<<1)^(v>>3)^((v>>2)&2))&0xf;
	return v|(w<<4);
}

static
void jh_round(uint8_t* a, const uint8_t* rc){
	uint8_t b[128];
	uint8_t i,r=0,x,y;
	for(i=0; i<128; ++i){
		if(i%4==0){
			r = rc[i/4];
		}
		x = pgm_read_byte(((r&0x80)?sbox1:sbox0)+(a[i]>>4));
		y = pgm_read_byte(((r&0x40)?sbox1:sbox0)+(a[i]&0xf));
		a[i]=jh_l(y,x);
		r<<=2;
	}
	/* pi permutation */
	for(i=1; i<128; i+=2){
		a[i] = (a[i]<<4)|(a[i]>>4);
	}
	/* P' permutation */
	for(i=0; i<64; ++i){
		b[i] = (a[i*2]&0xF0) | (a[i*2+1]>>4);
		b[64+i] = (a[i*2]<<4) | (a[i*2+1]&0x0F);
	}
	memcpy(a,b,64);
	/* phi permutation */
	for(i=64; i<128; i+=1){
		a[i] = (b[i]<<4)|(b[i]>>4);
	}
}

static
void jh_next_round_const(uint8_t* a){
	uint8_t b[32];
	uint8_t i,x,y;
	for(i=0; i<32; ++i){
		x = pgm_read_byte(sbox0+(a[i]>>4));
		y = pgm_read_byte(sbox0+(a[i]&0xf));
		a[i]=jh_l(y,x);
	}
	/* pi permutation */
	for(i=1; i<32; i+=2){
		a[i] = (a[i]<<4)|(a[i]>>4);
	}
	/* P' permutation */
	for(i=0; i<16; ++i){
		b[i] = (a[i*2]&0xF0) | (a[i*2+1]>>4);
		b[16+i] = (a[i*2]<<4) | (a[i*2+1]&0x0F);
	}
	memcpy(a,b,16);
	/* phi permutation */
	for(i=16; i<32; i+=1){
		a[i] = (b[i]<<4)|(b[i]>>4);
	}
}

static const uint8_t idx[]={112,80,48,16,96,64,32,0};


static inline
void group(uint8_t *a){
	uint8_t b[128];
	uint8_t i,j,k,x=0;
	for(i=0; i<128; ++i){
		j=i/8;
		for(k=0;k<8;++k){
			x>>=1;
			x |= a[j+idx[k]]&0x80;
			a[j+idx[k]] <<= 1;
		}
		b[i]= x;
	}
	memcpy(a,b,128);
}

static inline
void degroup(uint8_t *a){
	uint8_t b[128];
	uint8_t i,j,k,t;
	for(i=0;i<128;++i){
		j=i/8;
		t = a[i];
		for(k=0; k<8; ++k){
			b[j+idx[k]]<<=1;
			b[j+idx[k]] |= t&1;
			t>>=1;
		}
	}
	memcpy(a,b,128);
}

void jh_encrypt(uint8_t* a){
	uint8_t i;
	uint8_t rc[32];
	/* grouping */
#if DEBUG
	cli_putstr_P(PSTR("\r\n== pre group ==\r\n"));
	cli_hexdump_block(a, 128, 4, 16);
#endif
	group(a);
	for(i=0;i<32;++i){
		rc[i] = pgm_read_byte(&(round_const_0[i]));
	}
	for(i=0;i<42;++i){
		jh_round(a, rc);
		jh_next_round_const(rc);
	}

	/* degrouping */
#if DEBUG
	cli_putstr_P(PSTR("\r\n== pre degroup ==\r\n"));
	cli_hexdump_block(a, 128, 4, 16);
#endif
	degroup(a);
#if DEBUG
	cli_putstr_P(PSTR("\r\n== post degroup ==\r\n"));
	cli_hexdump_block(a, 128, 4, 16);
#endif
}


