/* groestl_small.c */
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
 * \file    groestl_small.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-19
 * \license GPLv3 or later
 *
 */

#include "groestl_small.h"
#include "aes_sbox.h"
#include "gf256mul.h"
#include "memxor.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>

#define ROUNDS 10
#define POLYNOM 0x1b

#define DEBUG 0

#if DEBUG
 #include "cli.h"
 void dump_m(const uint8_t* m){
	 uint8_t i,j;
	 for(i=0; i<8; ++i){
		cli_putstr_P(PSTR("\r\n"));
		for(j=0; j<8; ++j){
			cli_putc(' ');
			cli_hexdump(m+8*i+j, 1);
		}
	 }
 }
#else
 #define dump_m(m)
#endif

static const uint8_t matrix[] PROGMEM = {
 2, 2, 3, 4, 5, 3, 5, 7,
 7, 2, 2, 3, 4, 5, 3, 5,
 5, 7, 2, 2, 3, 4, 5, 3,
 3, 5, 7, 2, 2, 3, 4, 5,
 5, 3, 5, 7, 2, 2, 3, 4,
 4, 5, 3, 5, 7, 2, 2, 3,
 3, 4, 5, 3, 5, 7, 2, 2,
 2, 3, 4, 5, 3, 5, 7, 2
};


static const uint8_t p_shifts[] PROGMEM = {
		0, 1, 2, 3, 4, 5, 6, 7
};

static const uint8_t q_shifts[] PROGMEM = {
		1, 3, 5, 7, 0, 2, 4, 6
};


static
void shift_columns(uint8_t *a, PGM_VOID_P shifts){
	uint8_t s;
	uint8_t tmp[8];
	uint8_t i,j;
	for(i=0; i<8; ++i){
		s = pgm_read_byte(shifts);
		shifts = (uint8_t*)shifts + 1;
		if(s==0){
			continue;
		}
		for(j=0;j<8;++j){
			tmp[j] = a[i+j*8];
		}
		for(j=0; j<8; ++j){
			a[i+((j-s+8)%8)*8] = tmp[j];
		}
	}
}



void groestl_small_rounds(uint8_t *m, uint8_t q){
	uint8_t r,i,j;
	uint8_t tmp[8];
#if DEBUG
	cli_putstr_P(PSTR("\r\n:: BEGIN "));
	cli_putc(q?'Q':'P');
#endif

	for(r=0; r<ROUNDS; ++r){
		if(q){
			for(i=0; i<8*8; ++i){
				m[i] ^= 0xff;
			}
			for(i=0; i<8; ++i){
				m[7+i*8] ^=  r ^ (i<<4);
			}
		}else{
			for(i=0; i<8; ++i){
				m[i*8] ^= r ^ (i<<4);
			}
		}
#if DEBUG
	//	if(r<2){
			cli_putstr_P(PSTR("\r\npost add-const"));
			dump_m(m);
	//	}
#endif
		for(i=0;i<8*8; ++i){
			m[i] = pgm_read_byte(aes_sbox+m[i]);
		}
		if(!q){
			shift_columns(m, p_shifts);
		}else{
			shift_columns(m, q_shifts);
		}
#if DEBUG
		if(r<2){
			cli_putstr_P(PSTR("\r\npost shift-bytes"));
			dump_m(m);
		}
#endif
		for(i=0; i<8; ++i){
			memcpy(tmp, m+8*i, 8);
			for(j=0; j<8; ++j){
				m[j+i*8] = gf256mul(pgm_read_byte(matrix+8*j+0),tmp[0], POLYNOM)
				        ^ gf256mul(pgm_read_byte(matrix+8*j+1),tmp[1], POLYNOM)
				        ^ gf256mul(pgm_read_byte(matrix+8*j+2),tmp[2], POLYNOM)
				        ^ gf256mul(pgm_read_byte(matrix+8*j+3),tmp[3], POLYNOM)
				        ^ gf256mul(pgm_read_byte(matrix+8*j+4),tmp[4], POLYNOM)
				        ^ gf256mul(pgm_read_byte(matrix+8*j+5),tmp[5], POLYNOM)
				        ^ gf256mul(pgm_read_byte(matrix+8*j+6),tmp[6], POLYNOM)
				        ^ gf256mul(pgm_read_byte(matrix+8*j+7),tmp[7], POLYNOM);
			}
		}
#if DEBUG
		if(r<2){
			cli_putstr_P(PSTR("\r\npost mix-bytes"));
			dump_m(m);
		}
#endif
	}
}

void groestl224_init(groestl224_ctx_t* ctx){
	memset(ctx->h, 0, 8*8);
	ctx->h[8*8-1] = 224;
	ctx->counter = 1;
}

void groestl256_init(groestl256_ctx_t* ctx){
	memset(ctx->h, 0, 8*8);
	ctx->h[8*8-2] = 1;
	ctx->counter = 1;
}

void groestl_small_nextBlock(groestl_small_ctx_t* ctx, const void* block){
	uint8_t tmp1[64], tmp2[64];
/*	for(i=0; i<8; ++i){
		for(j=0; j<8; ++j){
			tmp1[j*8+i] = ((uint8_t*)block)[i*8+j];
		}
	}
*/
	memcpy(tmp1, block, 64);
	memcpy(tmp2, tmp1, 64);
	memxor(tmp1, ctx->h, 64);
	groestl_small_rounds(tmp1, 0);
	groestl_small_rounds(tmp2, 1);
	memxor(ctx->h, tmp1, 64);
	memxor(ctx->h, tmp2, 64);
	ctx->counter++;
}

void groestl_small_lastBlock(groestl_small_ctx_t* ctx, const void* block, uint16_t length_b){
	uint8_t buffer[64];
	while(length_b>=GROESTL_SMALL_BLOCKSIZE){
		groestl_small_nextBlock(ctx, block);
		length_b -= GROESTL_SMALL_BLOCKSIZE;
		block = (uint8_t*)block + GROESTL_SMALL_BLOCKSIZE_B;
	}
	memset(buffer, 0, 64);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b&0x7);
	if(length_b>512-65){
		groestl_small_nextBlock(ctx, buffer);
		memset(buffer, 0, 64-4);
	}
//	ctx->counter++;
	buffer[64-1]  = (uint8_t)(ctx->counter);
	buffer[64-2]  = (uint8_t)((ctx->counter)>>8);
	buffer[64-3]  = (uint8_t)((ctx->counter)>>16);
	buffer[64-4]  = (uint8_t)((ctx->counter)>>24);
	groestl_small_nextBlock(ctx, buffer);
}

void groestl_small_ctx2hash(void* dest, const groestl_small_ctx_t* ctx, uint16_t outlength_b){
	uint8_t tmp[64];
	memcpy(tmp, ctx->h, 64);
	groestl_small_rounds(tmp, 0);
	memxor(tmp, ctx->h, 64);
#if DEBUG
	cli_putstr_P(PSTR("\r\npost finalisation"));
	dump_m(tmp);
#endif
	memcpy(dest, tmp+64-outlength_b/8, outlength_b/8);
}

void groestl224_ctx2hash(void* dest, const groestl224_ctx_t* ctx){
	groestl_small_ctx2hash(dest, ctx, 224);
}

void groestl256_ctx2hash(void* dest, const groestl256_ctx_t* ctx){
	groestl_small_ctx2hash(dest, ctx, 256);
}

void groestl224_nextBlock(groestl224_ctx_t* ctx, const void* block){
	groestl_small_nextBlock(ctx, block);
}

void groestl256_nextBlock(groestl256_ctx_t* ctx, const void* block){
	groestl_small_nextBlock(ctx, block);
}

void groestl224_lastBlock(groestl224_ctx_t* ctx, const void* block, uint16_t length_b){
	groestl_small_lastBlock(ctx, block, length_b);
}

void groestl256_lastBlock(groestl256_ctx_t* ctx, const void* block, uint16_t length_b){
	groestl_small_lastBlock(ctx, block, length_b);
}

void groestl224(void* dest, const void* msg, uint32_t length_b){
	groestl_small_ctx_t ctx;
	groestl224_init(&ctx);
	while(length_b>=GROESTL_SMALL_BLOCKSIZE){
		groestl_small_nextBlock(&ctx, msg);
		length_b -= GROESTL_SMALL_BLOCKSIZE;
		msg = (uint8_t*)msg + GROESTL_SMALL_BLOCKSIZE_B;
	}
	groestl_small_lastBlock(&ctx, msg, length_b);
	groestl_small_ctx2hash(dest, &ctx, 224);
}

void groestl256(void* dest, const void* msg, uint32_t length_b){
	groestl_small_ctx_t ctx;
	groestl256_init(&ctx);
	while(length_b>=GROESTL_SMALL_BLOCKSIZE){
		groestl_small_nextBlock(&ctx, msg);
		length_b -= GROESTL_SMALL_BLOCKSIZE;
		msg = (uint8_t*)msg + GROESTL_SMALL_BLOCKSIZE_B;
	}
	groestl_small_lastBlock(&ctx, msg, length_b);
	groestl_small_ctx2hash(dest, &ctx, 256);
}



