/* bmw_small.c */
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
 * \file    bmw_small.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 *
 */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "memxor.h"
#include "bmw_small.h"


#define SHL32(a,n) ((a)<<(n))
#define SHR32(a,n) ((a)>>(n))
#define ROTL32(a,n) (((a)<<(n))|((a)>>(32-(n))))
#define ROTR32(a,n) (((a)>>(n))|((a)<<(32-(n))))


#define TWEAK   1
#if TWEAK
#  define BUG24   0
#else
#  define BUG24   1
#endif

#define F0_HACK 2

#define DEBUG 0

#ifndef F0_HACK
#  define F0_HACK 0
#endif

#if DEBUG
 #include "cli.h"

 void ctx_dump(const bmw_small_ctx_t* ctx){
 	uint8_t i;
	cli_putstr_P(PSTR("\r\n==== ctx dump ===="));
	for(i=0; i<16;++i){
		cli_putstr_P(PSTR("\r\n h["));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR("] = "));
		cli_hexdump_rev(&(ctx->h[i]), 4);
	}
	cli_putstr_P(PSTR("\r\n counter = "));
	cli_hexdump(&(ctx->counter), 4);
 }

 void dump_x(const uint32_t* q, uint8_t elements, char x){
	uint8_t i;
 	cli_putstr_P(PSTR("\r\n==== "));
	cli_putc(x);
	cli_putstr_P(PSTR(" dump ===="));
	for(i=0; i<elements;++i){
		cli_putstr_P(PSTR("\r\n "));
		cli_putc(x);
		cli_putstr_P(PSTR("["));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR("] = "));
		cli_hexdump_rev(&(q[i]), 4);
	}
 }
#else
 #define ctx_dump(x)
 #define dump_x(a,b,c)
#endif

static
uint32_t bmw_small_s0(uint32_t x){
	uint32_t r;
	r =   SHR32(x, 1)
		^ SHL32(x, 3)
		^ ROTL32(x, 4)
		^ ROTR32(x, 13);
	return r;
}

static
uint32_t bmw_small_s1(uint32_t x){
	uint32_t r;
	r =   SHR32(x, 1)
		^ SHL32(x, 2)
		^ ROTL32(x, 8)
		^ ROTR32(x, 9);
	return r;
}

static
uint32_t bmw_small_s2(uint32_t x){
	uint32_t r;
	r =   SHR32(x, 2)
		^ SHL32(x, 1)
		^ ROTL32(x, 12)
		^ ROTR32(x, 7);
	return r;
}

static
uint32_t bmw_small_s3(uint32_t x){
	uint32_t r;
	r =   SHR32(x, 2)
		^ SHL32(x, 2)
		^ ROTL32(x, 15)
		^ ROTR32(x, 3);
	return r;
}

static
uint32_t bmw_small_s4(uint32_t x){
	uint32_t r;
	r =   SHR32(x, 1)
		^ x;
	return r;
}

static
uint32_t bmw_small_s5(uint32_t x){
	uint32_t r;
	r =   SHR32(x, 2)
		^ x;
	return r;
}

static
uint32_t bmw_small_r1(uint32_t x){
	uint32_t r;
	r =   ROTL32(x, 3);
	return r;
}

static
uint32_t bmw_small_r2(uint32_t x){
	uint32_t r;
	r =   ROTL32(x, 7);
	return r;
}

static
uint32_t bmw_small_r3(uint32_t x){
	uint32_t r;
	r =   ROTL32(x, 13);
	return r;
}

static
uint32_t bmw_small_r4(uint32_t x){
	uint32_t r;
	r =   ROTL32(x, 16);
	return r;
}

static
uint32_t bmw_small_r5(uint32_t x){
	uint32_t r;
	r =   ROTR32(x, 13);
	return r;
}

static
uint32_t bmw_small_r6(uint32_t x){
	uint32_t r;
	r =   ROTR32(x, 9);
	return r;
}

static
uint32_t bmw_small_r7(uint32_t x){
	uint32_t r;
	r =   ROTR32(x, 5);
	return r;
}
/*
#define K 0x05555555L
static
const uint32_t k_lut[] PROGMEM = {
	16L*K, 17L*K, 18L*K, 19L*K, 20L*K, 21L*K, 22L*K, 23L*K,
	24L*K, 25L*K, 26L*K, 27L*K, 28L*K, 29L*K, 30L*K, 31L*K
};
*/
/* same as above but precomputed to avoid compiler warnings */

static
const uint32_t k_lut[] PROGMEM = {
	0x55555550L, 0x5aaaaaa5L, 0x5ffffffaL,
	0x6555554fL, 0x6aaaaaa4L, 0x6ffffff9L,
	0x7555554eL, 0x7aaaaaa3L, 0x7ffffff8L,
	0x8555554dL, 0x8aaaaaa2L, 0x8ffffff7L,
	0x9555554cL, 0x9aaaaaa1L, 0x9ffffff6L,
	0xa555554bL };

static
uint32_t bmw_small_expand1(uint8_t j, const uint32_t* q, const void* m, const void* h){
	uint32_t(*s[])(uint32_t) = {bmw_small_s1, bmw_small_s2, bmw_small_s3, bmw_small_s0};
	uint32_t r;
	uint8_t i;
	/* r = 0x05555555*(j+16); */

#if TWEAK
	r = (   ROTL32(((uint32_t*)m)[j&0xf],      ((j+0)&0xf)+1  )
	       + ROTL32(((uint32_t*)m)[(j+3)&0xf],  ((j+3)&0xf)+1  )
	       - ROTL32(((uint32_t*)m)[(j+10)&0xf], ((j+10)&0xf)+1 )
	       + pgm_read_dword(k_lut+j)
	     ) ^ ((uint32_t*)h)[(j+7)&0xf];
#else
	r = pgm_read_dword(k_lut+j);
	r += ((uint32_t*)m)[j&0xf];
	r += ((uint32_t*)m)[(j+3)&0xf];
	r -= ((uint32_t*)m)[(j+10)&0xf];
#endif
	for(i=0; i<16; ++i){
		r += s[i%4](q[j+i]);
	}
	return r;
}

static
uint32_t bmw_small_expand2(uint8_t j, const uint32_t* q, const void* m, const void* h){
	uint32_t(*rf[])(uint32_t) = {bmw_small_r1, bmw_small_r2, bmw_small_r3,
	                             bmw_small_r4, bmw_small_r5, bmw_small_r6,
							     bmw_small_r7};
	uint32_t r=0;
	uint8_t i;
	for(i=0; i<14; i+=2){
		r += q[j+i];
	}
	for(i=0; i<14; i+=2){
		r += rf[i/2](q[j+i+1]);
	}
#if TWEAK
	r += bmw_small_s4(q[j+14]);
	r += bmw_small_s5(q[j+15]);
#else
	r += bmw_small_s5(q[j+14]);
	r += bmw_small_s4(q[j+15]);
#endif
#if TWEAK
	r += (   ROTL32(((uint32_t*)m)[j&0xf],      ((j+0)&0xf)+1  )
	       + ROTL32(((uint32_t*)m)[(j+3)&0xf],  ((j+3)&0xf)+1  )
	       - ROTL32(((uint32_t*)m)[(j+10)&0xf], ((j+10)&0xf)+1 )
	       + pgm_read_dword(k_lut+j)
	     ) ^ ((uint32_t*)h)[(j+7)&0xf];
#else
	r += pgm_read_dword(k_lut+j);
	r += ((uint32_t*)m)[j&0xf];
	r += ((uint32_t*)m)[(j+3)&0xf];
	r -= ((uint32_t*)m)[(j+10)&0xf];
#endif
	return r;
}

#if F0_HACK==2
/* to understand this implementation take a look at f0-opt-table.txt */
static const uint16_t hack_table[5] PROGMEM = { 0x0311, 0xDDB3, 0x2A79, 0x07AA, 0x51C2 };
static const uint8_t  offset_table[5] PROGMEM = { 4+16, 6+16, 9+16, 12+16, 13+16 };

static
void bmw_small_f0(uint32_t* q, uint32_t* h, const void* m){
	uint16_t hack_reg;
	uint8_t c,i,j;
	uint32_t(*s[])(uint32_t)={ bmw_small_s0, bmw_small_s1, bmw_small_s2,
	                           bmw_small_s3, bmw_small_s4 };
	for(i=0; i<16; ++i){
		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
	}
	dump_x(h, 16, 'T');
	memset(q, 0, 4*16);
	c=4;
	do{
		i=15;
		j=pgm_read_byte(offset_table+c);
		hack_reg=pgm_read_word(&(hack_table[c]));
		do{
			if(hack_reg&1){
				q[i]-= h[j&15];
			}else{
				q[i]+= h[j&15];
			}
			--j;
			hack_reg>>= 1;
		}while(i--!=0);
	}while(c--!=0);
	dump_x(q, 16, 'W');
	for(i=0; i<16; ++i){
		q[i] = s[i%5](q[i]);
	}
#if TWEAK
	for(i=0; i<16; ++i){
		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
	}
	for(i=0; i<16; ++i){
		q[i] += h[(i+1)&0xf];
	}
#endif
}
#endif /* F0_HACK==2*/

#if F0_HACK==1
static
const uint8_t f0_lut[] PROGMEM = {
	 5<<1, ( 7<<1)+1, (10<<1)+0, (13<<1)+0, (14<<1)+0,
	 6<<1, ( 8<<1)+1, (11<<1)+0, (14<<1)+0, (15<<1)+1,
	 0<<1, ( 7<<1)+0, ( 9<<1)+0, (12<<1)+1, (15<<1)+0,
	 0<<1, ( 1<<1)+1, ( 8<<1)+0, (10<<1)+1, (13<<1)+0,
	 1<<1, ( 2<<1)+0, ( 9<<1)+0, (11<<1)+1, (14<<1)+1,
	 3<<1, ( 2<<1)+1, (10<<1)+0, (12<<1)+1, (15<<1)+0,
	 4<<1, ( 0<<1)+1, ( 3<<1)+1, (11<<1)+1, (13<<1)+0,
	 1<<1, ( 4<<1)+1, ( 5<<1)+1, (12<<1)+1, (14<<1)+1,
	 2<<1, ( 5<<1)+1, ( 6<<1)+1, (13<<1)+0, (15<<1)+1,
	 0<<1, ( 3<<1)+1, ( 6<<1)+0, ( 7<<1)+1, (14<<1)+0,
	 8<<1, ( 1<<1)+1, ( 4<<1)+1, ( 7<<1)+1, (15<<1)+0,
	 8<<1, ( 0<<1)+1, ( 2<<1)+1, ( 5<<1)+1, ( 9<<1)+0,
	 1<<1, ( 3<<1)+0, ( 6<<1)+1, ( 9<<1)+1, (10<<1)+0,
	 2<<1, ( 4<<1)+0, ( 7<<1)+0, (10<<1)+0, (11<<1)+0,
	 3<<1, ( 5<<1)+1, ( 8<<1)+0, (11<<1)+1, (12<<1)+1,
	12<<1, ( 4<<1)+1, ( 6<<1)+1, ( 9<<1)+1, (13<<1)+0
};

static
void bmw_small_f0(uint32_t* q, uint32_t* h, const void* m){
	uint8_t i,j=-1,v,sign,l=0;
	uint32_t(*s[])(uint32_t)={ bmw_small_s0, bmw_small_s1, bmw_small_s2,
	                           bmw_small_s3, bmw_small_s4 };
	for(i=0; i<16; ++i){
		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
	}
	dump_x(h, 16, 'T');
	// memset(q, 0, 4*16);
	for(i=0; i<5*16; ++i){
		v = pgm_read_byte(f0_lut+i);
		sign = v&1;
		v >>=1;
		if(i==l){
			j++;
			l+=5;
			q[j] = h[v];
			continue;
		}
		if(sign){
			q[j] -= h[v];
		}else{
			q[j] += h[v];
		}
	}
	dump_x(q, 16, 'W');
	for(i=0; i<16; ++i){
		q[i] = s[i%5](q[i]);
	}
#if TWEAK
	for(i=0; i<16; ++i){
		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
	}
	for(i=0; i<16; ++i){
		q[i] += h[(i+1)&0xf];
	}
#endif /* TWEAK */
}
#endif /* F0_HACK==1 */

#if F0_HACK==0
static
void bmw_small_f0(uint32_t* q, uint32_t* h, const void* m){
	uint8_t i;
	uint32_t(*s[])(uint32_t)={ bmw_small_s0, bmw_small_s1, bmw_small_s2,
	                           bmw_small_s3, bmw_small_s4 };
	for(i=0; i<16; ++i){
		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
	}
	dump_x(h, 16, 'T');
	q[ 0] = (h[ 5] - h[ 7] + h[10] + h[13] + h[14]);
	q[ 1] = (h[ 6] - h[ 8] + h[11] + h[14] - h[15]);
	q[ 2] = (h[ 0] + h[ 7] + h[ 9] - h[12] + h[15]);
	q[ 3] = (h[ 0] - h[ 1] + h[ 8] - h[10] + h[13]);
	q[ 4] = (h[ 1] + h[ 2] + h[ 9] - h[11] - h[14]);
	q[ 5] = (h[ 3] - h[ 2] + h[10] - h[12] + h[15]);
	q[ 6] = (h[ 4] - h[ 0] - h[ 3] - h[11] + h[13]);
	q[ 7] = (h[ 1] - h[ 4] - h[ 5] - h[12] - h[14]);
	q[ 8] = (h[ 2] - h[ 5] - h[ 6] + h[13] - h[15]);
	q[ 9] = (h[ 0] - h[ 3] + h[ 6] - h[ 7] + h[14]);
	q[10] = (h[ 8] - h[ 1] - h[ 4] - h[ 7] + h[15]);
	q[11] = (h[ 8] - h[ 0] - h[ 2] - h[ 5] + h[ 9]);
	q[12] = (h[ 1] + h[ 3] - h[ 6] - h[ 9] + h[10]);
	q[13] = (h[ 2] + h[ 4] + h[ 7] + h[10] + h[11]);
	q[14] = (h[ 3] - h[ 5] + h[ 8] - h[11] - h[12]);
	q[15] = (h[12] - h[ 4] - h[ 6] - h[ 9] + h[13]);
	dump_x(q, 16, 'W');
	for(i=0; i<16; ++i){
		q[i] = s[i%5](q[i]);
	}
#if TWEAK
	for(i=0; i<16; ++i){
		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
	}
	for(i=0; i<16; ++i){
		q[i] += h[(i+1)&0xf];
	}
#endif /* TWEAK */
}
#endif /* F0_HACK==0 */

static
void bmw_small_f1(uint32_t* q, const void* m, const void* h){
	uint8_t i;
	q[16] = bmw_small_expand1(0, q, m, h);
	q[17] = bmw_small_expand1(1, q, m, h);
	for(i=2; i<16; ++i){
		q[16+i] = bmw_small_expand2(i, q, m, h);
	}
}

static
void bmw_small_f2(uint32_t* h, uint32_t* q, const void* m){
	uint32_t xl=0, xh;
	uint8_t i;
	for(i=16;i<24;++i){
		xl ^= q[i];
	}
	xh = xl;
	for(i=24;i<32;++i){
		xh ^= q[i];
	}
#if DEBUG
	cli_putstr_P(PSTR("\r\n XL = "));
	cli_hexdump_rev(&xl, 4);
	cli_putstr_P(PSTR("\r\n XH = "));
	cli_hexdump_rev(&xh, 4);
#endif
	memcpy(h, m, 16*4);
	h[0] ^= SHL32(xh, 5) ^ SHR32(q[16], 5);
	h[1] ^= SHR32(xh, 7) ^ SHL32(q[17], 8);
	h[2] ^= SHR32(xh, 5) ^ SHL32(q[18], 5);
	h[3] ^= SHR32(xh, 1) ^ SHL32(q[19], 5);
	h[4] ^= SHR32(xh, 3) ^ q[20];
	h[5] ^= SHL32(xh, 6) ^ SHR32(q[21], 6);
	h[6] ^= SHR32(xh, 4) ^ SHL32(q[22], 6);
	h[7] ^= SHR32(xh,11) ^ SHL32(q[23], 2);
	for(i=0; i<8; ++i){
		h[i] += xl ^ q[24+i] ^ q[i];
	}
	for(i=0; i<8; ++i){
		h[8+i] ^= xh ^ q[24+i];
		h[8+i] += ROTL32(h[(4+i)%8],i+9);
	}
/*
	h[ 8] += SHL32(xl, 8) ^ q[23] ^ q[ 8];
	h[ 9] += SHR32(xl, 6) ^ q[16] ^ q[ 9];
	h[10] += SHL32(xl, 6) ^ q[17] ^ q[10];
	h[11] += SHL32(xl, 4) ^ q[18] ^ q[11];
	h[12] += SHR32(xl, 3) ^ q[19] ^ q[12];
	h[13] += SHR32(xl, 4) ^ q[20] ^ q[13];
	h[14] += SHR32(xl, 7) ^ q[21] ^ q[14];
	h[15] += SHR32(xl, 2) ^ q[22] ^ q[15];
*/
	memxor(q+9, q+16, 7*4);
	q[8] ^= q[23];
	h[ 8] += SHL32(xl, 8) ^ q[ 8];
	h[ 9] += SHR32(xl, 6) ^ q[ 9];
	h[10] += SHL32(xl, 6) ^ q[10];
	h[11] += SHL32(xl, 4) ^ q[11];
	h[12] += SHR32(xl, 3) ^ q[12];
	h[13] += SHR32(xl, 4) ^ q[13];
	h[14] += SHR32(xl, 7) ^ q[14];
	h[15] += SHR32(xl, 2) ^ q[15];

}

void bmw_small_nextBlock(bmw_small_ctx_t* ctx, const void* block){
	uint32_t q[32];
	dump_x(block, 16, 'M');
	bmw_small_f0(q, ctx->h, block);
	dump_x(q, 16, 'Q');
	bmw_small_f1(q, block, ctx->h);
	dump_x(q, 32, 'Q');
	bmw_small_f2(ctx->h, q, block);
	ctx->counter += 1;
	ctx_dump(ctx);
}

void bmw_small_lastBlock(bmw_small_ctx_t* ctx, const void* block, uint16_t length_b){
	union {
		uint8_t   v8[64];
		uint32_t v32[16];
		uint64_t v64[ 8];
	} buffer;
	while(length_b >= BMW_SMALL_BLOCKSIZE){
		bmw_small_nextBlock(ctx, block);
		length_b -= BMW_SMALL_BLOCKSIZE;
		block = (uint8_t*)block + BMW_SMALL_BLOCKSIZE_B;
	}
	memset(buffer.v8, 0, 64);
	memcpy(buffer.v8, block, (length_b+7)/8);
	buffer.v8[length_b>>3] |= 0x80 >> (length_b&0x07);
	if(length_b+1>64*8-64){
		bmw_small_nextBlock(ctx, buffer.v8);
		memset(buffer.v8, 0, 64-8);
		ctx->counter -= 1;
	}
	buffer.v64[7] = (uint64_t)(ctx->counter*512LL)+(uint64_t)length_b;
	bmw_small_nextBlock(ctx, buffer.v8);
#if TWEAK
	uint8_t i;
	uint32_t q[32];
	memset(buffer.v8, 0xaa, 64);
	for(i=0; i<16;++i){
		buffer.v8[i*4] = i+0xa0;
	}
//	dump_x(buffer.v8, 16, 'A');
	dump_x(ctx->h, 16, 'M');
	bmw_small_f0(q, buffer.v32, ctx->h);
	dump_x(buffer.v8, 16, 'a');
	dump_x(q, 16, 'Q');
	bmw_small_f1(q, ctx->h, buffer.v32);
	dump_x(q, 32, 'Q');
	bmw_small_f2(buffer.v32, q, ctx->h);
	memcpy(ctx->h, buffer.v8, 64);
#endif
}

void bmw224_init(bmw224_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x00010203;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x04040404;
	}
#if BUG24
	ctx->h[13] = 0x24353637;
#endif
	ctx->counter=0;
	ctx_dump(ctx);
}

void bmw256_init(bmw256_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x40414243;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x04040404;
	}
	ctx->counter=0;
	ctx_dump(ctx);
}

void bmw224_nextBlock(bmw224_ctx_t* ctx, const void* block){
	bmw_small_nextBlock(ctx, block);
}

void bmw256_nextBlock(bmw256_ctx_t* ctx, const void* block){
	bmw_small_nextBlock(ctx, block);
}

void bmw224_lastBlock(bmw224_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_small_lastBlock(ctx, block, length_b);
}

void bmw256_lastBlock(bmw256_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_small_lastBlock(ctx, block, length_b);
}

void bmw224_ctx2hash(void* dest, const bmw224_ctx_t* ctx){
	memcpy(dest, &(ctx->h[9]), 224/8);
}

void bmw256_ctx2hash(void* dest, const bmw256_ctx_t* ctx){
	memcpy(dest, &(ctx->h[8]), 256/8);
}

void bmw224(void* dest, const void* msg, uint32_t length_b){
	bmw_small_ctx_t ctx;
	bmw224_init(&ctx);
	while(length_b>=BMW_SMALL_BLOCKSIZE){
		bmw_small_nextBlock(&ctx, msg);
		length_b -= BMW_SMALL_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_SMALL_BLOCKSIZE_B;
	}
	bmw_small_lastBlock(&ctx, msg, length_b);
	bmw224_ctx2hash(dest, &ctx);
}

void bmw256(void* dest, const void* msg, uint32_t length_b){
	bmw_small_ctx_t ctx;
	bmw256_init(&ctx);
	while(length_b>=BMW_SMALL_BLOCKSIZE){
		bmw_small_nextBlock(&ctx, msg);
		length_b -= BMW_SMALL_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_SMALL_BLOCKSIZE_B;
	}
	bmw_small_lastBlock(&ctx, msg, length_b);
	bmw256_ctx2hash(dest, &ctx);
}

