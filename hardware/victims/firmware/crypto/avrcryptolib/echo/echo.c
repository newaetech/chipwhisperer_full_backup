/* echo.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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


#include "echo.h"
#include "gf256mul.h"
#include "memxor.h"
#include "aes_enc_round.h"
#include <stdint.h>
#include <string.h>

#ifdef DEBUG
#undef DEBUG
#endif

#define DEBUG 0

#if DEBUG
#define DEBUG_DEPTH 2
#include "cli.h"
#endif


#define INDEX(c,r) ((c)*16*4+(r)*16)

#define GF256MUL_1(a) (a)
#define GF256MUL_2(a) (gf256mul(2, (a), 0x1b))
#define GF256MUL_3(a) (gf256mul(3, (a), 0x1b))

static void mixcol(uint8_t* s){
	uint8_t t, tmp[4];
	tmp[0] = *(s+16*0);
	tmp[1] = *(s+16*1);
	tmp[2] = *(s+16*2);
	tmp[3] = *(s+16*3);

	t = tmp[0] ^ tmp[1] ^ tmp[2] ^ tmp[3];
	*(s+16*0) =
		  GF256MUL_2(tmp[0]^tmp[1])
		^ tmp[0]
		^ t;
	*(s+16*1) =
		  GF256MUL_2(tmp[1]^tmp[2])
		^ tmp[1]
		^ t;
	*(s+16*2) =
		  GF256MUL_2(tmp[2]^tmp[3])
		^ tmp[2]
		^ t;
	*(s+16*3) =
		  GF256MUL_2(tmp[3]^tmp[0])
		^ tmp[3]
		^ t;
}

#if DEBUG
static void dump_state(void* s){
	uint8_t row, col;
	for(col=0; col<4; col++){
		for(row=0; row<4; row++){
			cli_putstr_P(PSTR("\r\nrow "));
			cli_putc('0'+row);
			cli_putstr_P(PSTR(", col "));
			cli_putc('0'+col);
			cli_putstr_P(PSTR(": "));
			cli_hexdump((uint8_t*)s+col*16*4+row*16, 4);
			cli_putc(' ');
			cli_hexdump((uint8_t*)s+col*16*4+row*16+ 4, 4);
			cli_putc(' ');
			cli_hexdump((uint8_t*)s+col*16*4+row*16+ 8, 4);
			cli_putc(' ');
			cli_hexdump((uint8_t*)s+col*16*4+row*16+12, 4);
		}
	}
}
#endif

static void echo_compress(uint8_t* s, uint8_t iterations, uint64_t* c, void* salt){
	uint8_t i, j;
	union {
		uint8_t v8[16];
		uint64_t v64[2];
	} k;
#if DEBUG
	uint8_t round=0;
#endif
	memcpy(k.v8, c, 8);
	memset(k.v8+8, 0, 8);
	do{
		/* BIG.SubWords */
#if DEBUG
	cli_putstr_P(PSTR("\r\n === ROUND "));
	cli_putc('0'+round);
	cli_putstr_P(PSTR(" ==="));
	if(round<DEBUG_DEPTH){
		dump_state(s);
	}
#endif
		for(i=0; i<16; ++i){
			aes_enc_round((aes_cipher_state_t*)(s+16*i), (aes_roundkey_t*)k.v8);
			aes_enc_round((aes_cipher_state_t*)(s+16*i), (aes_roundkey_t*)salt);
			k.v64[0] += 1;
		}
#if DEBUG
		if(round<DEBUG_DEPTH){
			cli_putstr_P(PSTR("\r\nAfter SubWords"));
			dump_state(s);
		}
#endif
		/* BIG.ShiftRows */
		uint8_t t[16];
		/* "Row" 1 */
		memcpy(t,             s+INDEX(0, 1), 16);
		memcpy(s+INDEX(0, 1), s+INDEX(1, 1), 16);
		memcpy(s+INDEX(1, 1), s+INDEX(2, 1), 16);
		memcpy(s+INDEX(2, 1), s+INDEX(3, 1), 16);
		memcpy(s+INDEX(3, 1), t,             16);
		/* "Row" 2 */
		memcpy(t,             s+INDEX(0, 2), 16);
		memcpy(s+INDEX(0, 2), s+INDEX(2, 2), 16);
		memcpy(s+INDEX(2, 2), t,             16);
		memcpy(t,             s+INDEX(1, 2), 16);
		memcpy(s+INDEX(1, 2), s+INDEX(3, 2), 16);
		memcpy(s+INDEX(3, 2), t,             16);
		/* "Row" 3 */
		memcpy(t,             s+INDEX(0, 3), 16);
		memcpy(s+INDEX(0, 3), s+INDEX(3, 3), 16);
		memcpy(s+INDEX(3, 3), s+INDEX(2, 3), 16);
		memcpy(s+INDEX(2, 3), s+INDEX(1, 3), 16);
		memcpy(s+INDEX(1, 3), t,             16);
#if DEBUG
		if(round<DEBUG_DEPTH){
			cli_putstr_P(PSTR("\r\nAfter ShiftRows"));
			dump_state(s);
		}
#endif
		/* BIG.MixColumns */
		for(i=0; i<4; i+=1){
			for(j=0; j<16; ++j){
				mixcol(s+i*64+j);
			}
		}
#if DEBUG
		if(round<DEBUG_DEPTH){
			cli_putstr_P(PSTR("\r\nAfter MixColumns"));
			dump_state(s);
		}
		round++;
#endif
	}while(--iterations);

}

/******************************************************************************/

static void compress512(void* v, void* m, uint64_t* c, void* salt){
	uint8_t s[16*16];
	uint8_t i;
	memcpy(s, v, 16*4);           /* load v into state */
	memcpy(s+16*4, m, 16*12);     /* load m into state */

	echo_compress(s, 8, c, salt);

	/* BIG.Final */
	for(i=0; i<3; ++i){
		memxor(v, (uint8_t*)m+4*16*i, 4*16);
	}
	for(i=0; i<4; ++i){
		memxor(v, s+4*16*i, 4*16);
	}
}

static void compress1024(void* v, void* m, uint64_t* c, void* salt){
	uint8_t s[16*16];
	memcpy(s, v, 16*8);           /* load v into state */
	memcpy(s+16*8, m, 16*8);      /* load m into state */

	echo_compress(s, 10, c, salt);

	/* BIG.Final */
	memxor(v, m, 16*8);
	memxor(v, s, 16*8);
	memxor(v, s+16*8, 16*8);
}

/******************************************************************************/

void echo_small_nextBlock(echo_small_ctx_t* ctx, void* block){
	ctx->counter += ECHO_SMALL_BLOCKSIZE;
	compress512(ctx->v, block, &(ctx->counter), ctx->salt);
}

void echo_small_lastBlock(echo_small_ctx_t* ctx, void* block, uint16_t length_b){
	while(length_b>=ECHO_SMALL_BLOCKSIZE){
		echo_small_nextBlock(ctx, block);
		block = (uint8_t*)block + ECHO_SMALL_BLOCKSIZE_B;
		length_b -= ECHO_SMALL_BLOCKSIZE;
	}
	uint8_t buffer[ECHO_SMALL_BLOCKSIZE_B];
	uint64_t total_len;
	memset(buffer, 0, ECHO_SMALL_BLOCKSIZE_B);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80 >> (length_b&7);
	total_len = (ctx->counter += length_b);
	if(length_b>=ECHO_SMALL_BLOCKSIZE-144){
		compress512(ctx->v, buffer, &total_len, ctx->salt);
		memset(buffer, 0, ECHO_SMALL_BLOCKSIZE_B);
		ctx->counter = 0;
	}
	if(length_b==0){
		ctx->counter = 0;
	}
	memcpy(buffer+ECHO_SMALL_BLOCKSIZE_B-18, &(ctx->id), 2);
	memcpy(buffer+ECHO_SMALL_BLOCKSIZE_B-16, &total_len, 8);
	compress512(ctx->v, buffer, &(ctx->counter), ctx->salt);
}

/******************************************************************************/

void echo_large_nextBlock(echo_large_ctx_t* ctx, void* block){
	ctx->counter += ECHO_LARGE_BLOCKSIZE;
	compress1024(ctx->v, block, &(ctx->counter), ctx->salt);
}

void echo_large_lastBlock(echo_large_ctx_t* ctx, void* block, uint16_t length_b){
	while(length_b>=ECHO_LARGE_BLOCKSIZE){
		echo_large_nextBlock(ctx, block);
		block = (uint8_t*)block + ECHO_LARGE_BLOCKSIZE_B;
		length_b -= ECHO_LARGE_BLOCKSIZE;
	}
	uint8_t buffer[ECHO_LARGE_BLOCKSIZE_B];
	uint64_t total_len;
	memset(buffer, 0, ECHO_LARGE_BLOCKSIZE_B);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80 >> (length_b&7);
	total_len = (ctx->counter += length_b);
	if(length_b>=ECHO_LARGE_BLOCKSIZE-144){
		compress1024(ctx->v, buffer, &total_len, ctx->salt);
		memset(buffer, 0, ECHO_LARGE_BLOCKSIZE_B);
		ctx->counter = 0;
	}
	if(length_b==0){
		ctx->counter = 0;
	}
	memcpy(buffer+ECHO_LARGE_BLOCKSIZE_B-18, &(ctx->id), 2);
	memcpy(buffer+ECHO_LARGE_BLOCKSIZE_B-16, &total_len, 8);
	compress1024(ctx->v, buffer, &(ctx->counter), ctx->salt);
}
/******************************************************************************/

void echo_ctx2hash(void* dest, uint16_t length_b, echo_small_ctx_t* ctx){
	memcpy(dest, ctx->v, (length_b+7)/8);
}

void echo224_ctx2hash(void* dest, echo_small_ctx_t* ctx){
	memcpy(dest, ctx->v, 224/8);
}

void echo256_ctx2hash(void* dest, echo_small_ctx_t* ctx){
	memcpy(dest, ctx->v, 256/8);
}

/******************************************************************************/

void echo384_ctx2hash(void* dest, echo_large_ctx_t* ctx){
	memcpy(dest, ctx->v, 384/8);
}

void echo512_ctx2hash(void* dest, echo_large_ctx_t* ctx){
	memcpy(dest, ctx->v, 512/8);
}

/******************************************************************************/

void echo224_init(echo_small_ctx_t* ctx){
	memset(ctx->v, 0, 4*16);
	ctx->counter = 0;
	memset(ctx->salt, 0, 16);
	ctx->id = 0x00E0;
	ctx->v[0+16*0] = 0xE0;
	ctx->v[0+16*1] = 0xE0;
	ctx->v[0+16*2] = 0xE0;
	ctx->v[0+16*3] = 0xE0;
}

void echo256_init(echo_small_ctx_t* ctx){
	memset(ctx->v, 0, 4*16);
	ctx->counter = 0;
	memset(ctx->salt, 0, 16);
	ctx->id = 0x0100;
	ctx->v[1+16*0] = 0x01;
	ctx->v[1+16*1] = 0x01;
	ctx->v[1+16*2] = 0x01;
	ctx->v[1+16*3] = 0x01;
}

/******************************************************************************/

void echo384_init(echo_large_ctx_t* ctx){
	uint8_t i;
	memset(ctx->v, 0, 8*16);
	ctx->counter = 0;
	memset(ctx->salt, 0, 16);
	ctx->id = 0x0180;
	for(i=0; i<8; ++i){
		ctx->v[0+16*i] = 0x80;
		ctx->v[1+16*i] = 0x01;
	}
}

void echo512_init(echo_large_ctx_t* ctx){
	uint8_t i;
	memset(ctx->v, 0, 8*16);
	ctx->counter = 0;
	memset(ctx->salt, 0, 16);
	ctx->id = 0x0200;
	for(i=0; i<8; ++i){
		ctx->v[1+16*i] = 0x02;
	}
}

/******************************************************************************/
