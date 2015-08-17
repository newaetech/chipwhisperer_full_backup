/* cscipher_small_core.c */
/*
    This file is part of the ARM-Crypto-Lib.
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
#include "cscipher.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

#define ROTL(a) (((a)<<1)|((a)>>7))

#ifndef SBOX_PROG
#define SBOX_PROG 0
#endif

#if SBOX_PROG
static const uint8_t fg_table[] PROGMEM = {
		0xfa, 0xd6, 0xb0, 0xb2, 0x7b, 0x5e, 0x71, 0x78,
		0xed, 0xd4, 0xa5, 0xb3, 0xef, 0xdc, 0xe7, 0xf9
};

static
uint8_t p(uint8_t a){
	a ^= pgm_read_byte(fg_table+(a&0xf))&0xf0;
	a ^= pgm_read_byte(fg_table+(a>>4)) &0x0f;
	a ^= pgm_read_byte(fg_table+(a&0xf))&0xf0;
	return a;
}
#define P(a) p(a)

#else

#include "cscipher_sbox.h"
#define P(a) pgm_read_byte(cscipher_sbox+(a))
#endif

static const uint8_t round_const[] PROGMEM = {
	0xb7, 0xe1, 0x51, 0x62, 0x8a, 0xed, 0x2a, 0x6a,
	0xbf, 0x71, 0x58, 0x80, 0x9c, 0xf4, 0xf3, 0xc7 };

static const uint8_t ks_const[] PROGMEM = {
	0x29,0x0d,0x61,0x40,0x9c,0xeb,0x9e,0x8f,
	0x1f,0x85,0x5f,0x58,0x5b,0x01,0x39,0x86,
	0x97,0x2e,0xd7,0xd6,0x35,0xae,0x17,0x16,
	0x21,0xb6,0x69,0x4e,0xa5,0x72,0x87,0x08,
	0x3c,0x18,0xe6,0xe7,0xfa,0xad,0xb8,0x89,
	0xb7,0x00,0xf7,0x6f,0x73,0x84,0x11,0x63,
	0x3f,0x96,0x7f,0x6e,0xbf,0x14,0x9d,0xac,
	0xa4,0x0e,0x7e,0xf6,0x20,0x4a,0x62,0x30,
	0x03,0xc5,0x4b,0x5a,0x46,0xa3,0x44,0x65
};

static uint16_t m(uint16_t a){
	uint8_t xl, xr, yl, yr;
	uint16_t ret;
	xr = a>>8;
	xl = a&0xff;
	yl = (ROTL(xl)&0x55)^xl^xr;
	yr = ROTL(xl)^xr;
	ret = (P(yr)<<8)|P(yl);
	return ret;
}

static uint16_t m_inv(uint16_t a){
	uint8_t xl, xr;
	xr = P(a>>8);
	xl = P(a&0xff);
	xl ^= xr;
	xl ^= (ROTL(xl)&0xaa);
	xr ^= ROTL(xl);
	return (xr<<8)|xl;
}


void cscipher_enc(void* buffer, const cscipher_ctx_t* ctx){
	uint8_t i,j,k;
	uint8_t tmp[8];
	for(i=0; i<8; ++i){
#if DEBUG
		cli_putstr_P(PSTR("\r\nDBG: round "));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR(" buffer:"));
		cli_hexdump(buffer, 8);
#endif
		for(j=0; j<3; ++j){
			if(j==0){
				memxor(buffer, ctx->keys[i], 8);
			}else{
				memxor_P(buffer, round_const+((j==1)?0:8), 8);
			}
			for(k=0; k<4; ++k){
				((uint16_t*)tmp)[k] = m(((uint16_t*)buffer)[k]);
			}
			for(k=0; k<4; ++k){
				((uint8_t*)buffer)[k]   = tmp[2*k];
				((uint8_t*)buffer)[k+4] = tmp[2*k+1];
			}
		}
	}
	memxor(buffer, ctx->keys[8], 8);
}

void cscipher_dec(void* buffer, const cscipher_ctx_t* ctx){
	uint8_t i=7,j,k;
	uint8_t tmp[8];
	memxor(buffer, ctx->keys[8], 8);
	do{
		for(j=0; j<3; ++j){
			for(k=0; k<4; ++k){
				tmp[2*k]   = ((uint8_t*)buffer)[k];
				tmp[2*k+1] = ((uint8_t*)buffer)[4+k];
			}
			for(k=0; k<4; ++k){
				((uint16_t*)buffer)[k] = m_inv(((uint16_t*)tmp)[k]);
			}
			if(j==2){
				memxor(buffer, ctx->keys[i], 8);
			}else{
				memxor_P(buffer, round_const+((j==1)?0:8), 8);
			}

		}
	}while(i--);
}

void cscipher_init(const void* key, cscipher_ctx_t* ctx){
	uint8_t tmp_key[16], tmp[8];
	uint8_t i,j,k,t=0;
	memcpy(tmp_key, key, 16);
	for(i=0; i<9; ++i){
#if DEBUG
		cli_putstr_P(PSTR("\r\nDBG: round "));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR(" key state:"));
		cli_hexdump(tmp_key, 16);
#endif
		memcpy(tmp, tmp_key+(((i&1)==0)?0:8), 8);
		memxor_P(tmp, ks_const+8*i, 8);
		for(j=0; j<8; ++j){
			tmp[j] = P(tmp[j]);
		}
		for(j=0; j<8; ++j){
			for(k=0; k<8; ++k){
				t<<=1;
				t |= tmp[k]>>7;
				tmp[k]<<=1;
			}
			tmp_key[j+(((i&1)==0)?8:0)] ^= t;
		}
		memcpy(ctx->keys[i], tmp_key+(((i&1)==0)?8:0), 8);
	}
}
