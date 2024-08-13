/* groestl_large.c */
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
 * \file    groestl_large.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-06-11
 * \license GPLv3 or later
 * 
 */

#include "groestl_large.h"
#include "aes_sbox.h"
#include "gf256mul.h"
#include "memxor.h"
#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>

#define ROUNDS 14
#define POLYNOM 0x1b

#define DEBUG 0

#if DEBUG
 #include "cli.h"
 void dump_m(const uint8_t* m){
	 uint8_t i,j;
	 for(i=0; i<16; ++i){
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

static
void shift_columns(uint8_t *a, PGM_VOID_P shifts){
	uint8_t s;
	uint8_t tmp[16];
	uint8_t i,j;
	for(i=0; i<8; ++i){
		s = pgm_read_byte(shifts);
		shifts = (uint8_t*)shifts + 1;
		if(s==0){
			continue;
		}
		for(j=0;j<16;++j){
			tmp[j] = a[i+j*8];
		}
		for(j=0; j<16; ++j){
			a[i+((j-s+16)%16)*8] = tmp[j];
		}
	}
}

static const uint8_t p_shifts[] PROGMEM = {
		0, 1, 2, 3, 4, 5, 6, 11
};

static const uint8_t q_shifts[] PROGMEM = {
		1, 3, 5, 11, 0, 2, 4, 6
};

void groestl_large_rounds(uint8_t *m, uint8_t q){
	uint8_t r,i,j;
	uint8_t tmp[16];
	for(r=0; r<ROUNDS; ++r){
		if(q){
			for(i=0; i<(8*16); ++i){
				m[i] ^= 0xff;
			}
			for(i=0; i<16; ++i){
				m[7+i*8] ^= r ^ (i<<4);
			}
		}else{
			for(i=0; i<16; ++i){
				m[i*8] ^= r ^ (i<<4);
			}
		}	
#if DEBUG		
		if(r<2){
			cli_putstr_P(PSTR("\r\npost add-const"));
			dump_m(m);
		}
#endif
		for(i=0;i<16*8; ++i){
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
		for(i=0; i<16; ++i){
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

void groestl384_init(groestl384_ctx_t* ctx){
	memset(ctx->h, 0, 16*8);
	ctx->h[8*16-1] = (uint8_t)384;
	ctx->h[8*16-2] = (uint8_t)(384>>8);
	ctx->counter = 0;
}

void groestl512_init(groestl512_ctx_t* ctx){
	memset(ctx->h, 0, 16*8);
	ctx->h[8*16-2] = 2;
	ctx->counter = 0;
}

void groestl_large_nextBlock(groestl_large_ctx_t* ctx, const void* block){
	uint8_t tmp1[128], tmp2[128];
/*
	for(i=0; i<8; ++i){
		for(j=0; j<8; ++j){
			tmp1[j*8+i] = ((uint8_t*)block)[i*8+j];
		}
	}
*/ 
	memcpy(tmp1, block, 128);
	memcpy(tmp2, tmp1, 128);
	memxor(tmp1, ctx->h, 128);
	groestl_large_rounds(tmp1, 0);
	groestl_large_rounds(tmp2, 1);
	memxor(ctx->h, tmp1, 128);
	memxor(ctx->h, tmp2, 128);
	ctx->counter++;
}

void groestl_large_lastBlock(groestl_large_ctx_t* ctx, const void* block, uint16_t length_b){
	uint8_t buffer[128];
	while(length_b>=GROESTL_LARGE_BLOCKSIZE){
		groestl_large_nextBlock(ctx, block);
		length_b -= GROESTL_LARGE_BLOCKSIZE;
		block = (uint8_t*)block + GROESTL_LARGE_BLOCKSIZE_B;
	}
	memset(buffer, 0, 128);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b%8);
	if(length_b>1024-65){
		groestl_large_nextBlock(ctx, buffer);
		memset(buffer, 0, 128-4);
	}
	ctx->counter++;
	buffer[128-1]  = (uint8_t)(ctx->counter);
	buffer[128-2]  = (uint8_t)((ctx->counter)>>8);
	buffer[128-3]  = (uint8_t)((ctx->counter)>>16);
	buffer[128-4]  = (uint8_t)((ctx->counter)>>24);
	groestl_large_nextBlock(ctx, buffer);
}

void groestl_large_ctx2hash(void* dest, const groestl_large_ctx_t* ctx, uint16_t outlength_b){
	uint8_t tmp[128];
	memcpy(tmp, ctx->h, 128);
	groestl_large_rounds(tmp, 0);
	memxor(tmp, ctx->h, 128);
#if DEBUG
	cli_putstr_P(PSTR("\r\npost finalisation"));
	dump_m(tmp);
#endif		
	memcpy(dest, tmp+128-outlength_b/8, outlength_b/8);
}

void groestl384_ctx2hash(void* dest, const groestl384_ctx_t* ctx){
	groestl_large_ctx2hash(dest, ctx, 384);
}

void groestl512_ctx2hash(void* dest, const groestl512_ctx_t* ctx){
	groestl_large_ctx2hash(dest, ctx, 512);
}

void groestl384_nextBlock(groestl384_ctx_t* ctx, const void* block){
	groestl_large_nextBlock(ctx, block);
}

void groestl512_nextBlock(groestl512_ctx_t* ctx, const void* block){
	groestl_large_nextBlock(ctx, block);
}

void groestl384_lastBlock(groestl384_ctx_t* ctx, const void* block, uint16_t length_b){
	groestl_large_lastBlock(ctx, block, length_b);
}

void groestl512_lastBlock(groestl512_ctx_t* ctx, const void* block, uint16_t length_b){
	groestl_large_lastBlock(ctx, block, length_b);
}

void groestl384(void* dest, const void* msg, uint32_t length_b){
	groestl_large_ctx_t ctx;
	groestl384_init(&ctx);
	while(length_b>=GROESTL_LARGE_BLOCKSIZE){
		groestl_large_nextBlock(&ctx, msg);
		length_b -= GROESTL_LARGE_BLOCKSIZE;
		msg = (uint8_t*)msg + GROESTL_LARGE_BLOCKSIZE_B;
	}
	groestl_large_lastBlock(&ctx, msg, length_b);
	groestl_large_ctx2hash(dest, &ctx, 384);
}

void groestl512(void* dest, const void* msg, uint32_t length_b){
	groestl_large_ctx_t ctx;
	groestl512_init(&ctx);
	while(length_b>=GROESTL_LARGE_BLOCKSIZE){
		groestl_large_nextBlock(&ctx, msg);
		length_b -= GROESTL_LARGE_BLOCKSIZE;
		msg = (uint8_t*)msg + GROESTL_LARGE_BLOCKSIZE_B;
	}
	groestl_large_lastBlock(&ctx, msg, length_b);
	groestl_large_ctx2hash(dest, &ctx, 512);
}



