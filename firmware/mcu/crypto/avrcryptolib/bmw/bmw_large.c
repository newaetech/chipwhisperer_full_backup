/* bmw_large.c */
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
 * \file    bmw_large.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 *
 */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "bmw_large.h"

#define SHL64(a,n)  shiftl64(a,n)
#define SHR64(a,n)  shiftr64(a,n)
#define ROTL64(a,n) rotl64(a,n)
#define ROTR64(a,n) rotr64(a,n)

#define TWEAK   1
#define BUG24   0
#define F0_HACK 2
#define DEBUG   0

#if DEBUG
 #include "cli.h"

 void ctx_dump(const bmw_large_ctx_t* ctx){
 	uint8_t i;
	cli_putstr_P(PSTR("\r\n==== ctx dump ===="));
	for(i=0; i<16;++i){
		cli_putstr_P(PSTR("\r\n h["));
		cli_hexdump(&i, 1);
		cli_putstr_P(PSTR("] = "));
		cli_hexdump_rev(&(ctx->h[i]), 8);
	}
	cli_putstr_P(PSTR("\r\n counter = "));
	cli_hexdump(&(ctx->counter), 4);
 }

 void dump_x(const uint64_t* q, uint8_t elements, char x){
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
		cli_hexdump_rev(&(q[i]), 8);
	}
 }
#else
 #define ctx_dump(x)
 #define dump_x(a,b,c)
#endif

static
uint64_t rotl64(uint64_t a, uint8_t r){
	return (a<<r)|(a>>(64-r));
}

static
uint64_t rotr64(uint64_t a, uint8_t r){
	return (a>>r)|(a<<(64-r));
}

static
uint64_t shiftl64(uint64_t a, uint8_t r){
	return (a<<r);
}

static
uint64_t shiftr64(uint64_t a, uint8_t r){
	return (a>>r);
}

static
uint64_t bmw_large_s0(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 1)
		^ SHL64(x, 3)
		^ ROTL64(x, 4)
		^ ROTR64(x, 64-37);
	return r;
}

static
uint64_t bmw_large_s1(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 1)
		^ SHL64(x, 2)
		^ ROTL64(x,13)
		^ ROTR64(x,64-43);
	return r;
}

static
uint64_t bmw_large_s2(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 2)
		^ SHL64(x, 1)
		^ ROTL64(x, 19)
		^ ROTR64(x, 64-53);
	return r;
}

static
uint64_t bmw_large_s3(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 2)
		^ SHL64(x, 2)
		^ ROTL64(x, 28)
		^ ROTR64(x, 64-59);
	return r;
}

static
uint64_t bmw_large_s4(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 1)
		^ x;
	return r;
}

static
uint64_t bmw_large_s5(uint64_t x){
	uint64_t r;
	r =   SHR64(x, 2)
		^ x;
	return r;
}

static
uint64_t bmw_large_r1(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 5);
	return r;
}

static
uint64_t bmw_large_r2(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 11);
	return r;
}

static
uint64_t bmw_large_r3(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 27);
	return r;
}

static
uint64_t bmw_large_r4(uint64_t x){
	uint64_t r;
	r =   ROTL64(x, 32);
	return r;
}

static
uint64_t bmw_large_r5(uint64_t x){
	uint64_t r;
	r =   ROTR64(x, 64-37);
	return r;
}

static
uint64_t bmw_large_r6(uint64_t x){
	uint64_t r;
	r =   ROTR64(x, 64-43);
	return r;
}

static
uint64_t bmw_large_r7(uint64_t x){
	uint64_t r;
	r =   ROTR64(x, 64-53);
	return r;
}
/*
#define K    0x0555555555555555LL
#define MASK 0xFFFFFFFFFFFFFFFFLL
static
const uint64_t k_lut[] PROGMEM = {
	16LL*K, 17LL*K, 18LL*K, 19LL*K,
	20LL*K, 21LL*K, 22LL*K, 23LL*K,
	24LL*K, 25LL*K, 26LL*K, 27LL*K,
	28LL*K, 29LL*K, 30LL*K, 31LL*K };
*/
/* the same as above but precomputed to avoid compiler warnings */
static
const uint64_t k_lut[] PROGMEM = {
	0x5555555555555550LL, 0x5aaaaaaaaaaaaaa5LL, 0x5ffffffffffffffaLL,
	0x655555555555554fLL, 0x6aaaaaaaaaaaaaa4LL, 0x6ffffffffffffff9LL,
	0x755555555555554eLL, 0x7aaaaaaaaaaaaaa3LL, 0x7ffffffffffffff8LL,
	0x855555555555554dLL, 0x8aaaaaaaaaaaaaa2LL, 0x8ffffffffffffff7LL,
	0x955555555555554cLL, 0x9aaaaaaaaaaaaaa1LL, 0x9ffffffffffffff6LL,
	0xa55555555555554bLL };

static
uint64_t bmw_large_expand1(uint8_t j, const uint64_t* q, const void* m, const void* h){
	uint64_t(*s[])(uint64_t) = {bmw_large_s1, bmw_large_s2, bmw_large_s3, bmw_large_s0};
	uint64_t a = 0;
	union{
		uint64_t v64;
		uint32_t v32[2];
	} r;
	uint8_t i;
	/* r = 0x0555555555555555LL*(j+16); */
	r.v32[0] = pgm_read_dword(((uint8_t*)k_lut+8*j));
	r.v32[1] = pgm_read_dword(((uint8_t*)k_lut+8*j+4));
	for(i=0; i<16; ++i){
		a += s[i%4](q[j+i]);
	}
#if TWEAK
	a += (   ROTL64(((uint64_t*)m)[(j)&0xf],   ((j+ 0)&0xf)+1)
	       + ROTL64(((uint64_t*)m)[(j+3)&0xf], ((j+ 3)&0xf)+1)
	       + r.v64
	       - ROTL64(((uint64_t*)m)[(j+10)&0xf],((j+10)&0xf)+1)
	     ) ^ ((uint64_t*)h)[(j+7)&0xf];
#else
	a += ((uint64_t*)m)[j&0xf];
	a += ((uint64_t*)m)[(j+3)&0xf];
	a -= ((uint64_t*)m)[(j+10)&0xf];
	a += r.v64;
#endif
	return a;
}

static
uint64_t bmw_large_expand2(uint8_t j, const uint64_t* q, const void* m, const void* h){
	uint64_t(*rf[])(uint64_t) = {bmw_large_r1, bmw_large_r2, bmw_large_r3,
	                             bmw_large_r4, bmw_large_r5, bmw_large_r6,
							     bmw_large_r7};
	uint64_t a=0;
	union{
		uint64_t v64;
		uint32_t v32[2];
	} r;
	uint8_t i;
	/* r = 0x0555555555555555LL*(j+16); */
	r.v32[0] = pgm_read_dword(((uint8_t*)k_lut+8*j));
	r.v32[1] = pgm_read_dword(((uint8_t*)k_lut+8*j+4));
	for(i=0; i<14; i+=2){
		a += q[j+i];
	}
	for(i=0; i<14; i+=2){
		a += rf[i/2](q[j+i+1]);
	}
#if TWEAK
	a += bmw_large_s4(q[j+14]);
	a += bmw_large_s5(q[j+15]);
#else
	a += bmw_large_s5(q[j+14]);
	a += bmw_large_s4(q[j+15]);
#endif
#if TWEAK
	/*
	if(j==(22-16)){
		uint64_t t;
		cli_putstr_P(PSTR("\n+++++++++ expand_2 ++++++++++++"));
		dump_x(&a, 1, 'a');
		dump_x(&r, 1, 'r');
		t=ROTL64(((uint64_t*)m)[j],   ((j+ 0)&0xf)+1);
		dump_x(&t, 1, '0');
		t=ROTL64(((uint64_t*)m)[j],   ((j+ 3)&0xf)+1);
		dump_x(&t, 1, '0');
		t=ROTL64(((uint64_t*)m)[j],   ((j+ 0)&0xf)+1);
		dump_x(&t, 1, '0');

	}
	*/
	a += (   ROTL64(((uint64_t*)m)[(j)&0xf],   ((j+ 0)&0xf)+1)
	       + ROTL64(((uint64_t*)m)[(j+3)&0xf], ((j+ 3)&0xf)+1)
	       + r.v64
	       - ROTL64(((uint64_t*)m)[(j+10)&0xf],((j+10)&0xf)+1)
	     ) ^ ((uint64_t*)h)[(j+7)&0xf];
#else
	a += ((uint64_t*)m)[j&0xf];
	a += ((uint64_t*)m)[(j+3)&0xf];
	a -= ((uint64_t*)m)[(j+10)&0xf];
	a += r.v64;
#endif
	return a;
}

#if F0_HACK==2
/* to understand this implementation take a look at f0-opt-table.txt */
static const uint16_t hack_table[5] PROGMEM = { 0x0311, 0xDDB3, 0x2A79, 0x07AA, 0x51C2 };
static const uint8_t  offset_table[5] PROGMEM = { 4+16, 6+16, 9+16, 12+16, 13+16 };


static
void bmw_large_f0(uint64_t* q, const uint64_t* h, const void* m){
	uint16_t hack_reg;
	uint8_t i,j,c;
	uint64_t(*s[])(uint64_t)={ bmw_large_s0, bmw_large_s1, bmw_large_s2,
	                           bmw_large_s3, bmw_large_s4 };
	for(i=0; i<16; ++i){
		((uint64_t*)h)[i] ^= ((uint64_t*)m)[i];
	}
	dump_x(h, 16, 'T');
	memset(q, 0, 8*16);
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
		((uint64_t*)h)[i] ^= ((uint64_t*)m)[i];
	}
	for(i=0; i<16; ++i){
		q[i] += h[(i+1)&0xf];
	}
#endif /* TWEAK */
}
#endif /* F0_HACK==2 */

#if F0_HACK==1
static
const uint8_t f0_lut[] PROGMEM ={
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
void bmw_large_f0(uint64_t* q, const uint64_t* h, const void* m){
	uint8_t i,j=-1,v,sign,l=0;
	uint64_t(*s[])(uint64_t)={ bmw_large_s0, bmw_large_s1, bmw_large_s2,
	                           bmw_large_s3, bmw_large_s4 };
	for(i=0; i<16; ++i){
		((uint64_t*)h)[i] ^= ((uint64_t*)m)[i];
	}
	dump_x(h, 16, 'T');
//	memset(q, 0, 4*16);
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
		((uint64_t*)h)[i] ^= ((uint64_t*)m)[i];
	}
	for(i=0; i<16; ++i){
		q[i] += h[(i+1)&0xf];
	}
#endif /* TWEAK */
}
#endif /* F0_HACK==1 */

#if F0_HACK==0
static
void bmw_large_f0(uint64_t* q, const uint64_t* h, const void* m){
	uint8_t i;
	uint64_t(*s[])(uint64_t)={ bmw_large_s0, bmw_large_s1, bmw_large_s2,
	                           bmw_large_s3, bmw_large_s4 };
	for(i=0; i<16; ++i){
		((uint64_t*)h)[i] ^= ((uint64_t*)m)[i];
	}
//	dump_x(t, 16, 'T');
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
		((uint64_t*)h)[i] ^= ((uint64_t*)m)[i];
	}
	for(i=0; i<16; ++i){
		q[i] += h[(i+1)&0xf];
	}
#endif /* TWEAK */

}
#endif /* F0_HACK==0 */

static
void bmw_large_f1(uint64_t* q, const void* m, const uint64_t* h){
	uint8_t i;
	q[16] = bmw_large_expand1(0, q, m, h);
	q[17] = bmw_large_expand1(1, q, m, h);
	for(i=2; i<16; ++i){
		q[16+i] = bmw_large_expand2(i, q, m, h);
	}
}

static
void bmw_large_f2(uint64_t* h, const uint64_t* q, const void* m){
	uint64_t xl=0, xh;
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
	memcpy(h, m, 16*8);
	h[0] ^= SHL64(xh, 5) ^ SHR64(q[16], 5);
	h[1] ^= SHR64(xh, 7) ^ SHL64(q[17], 8);
	h[2] ^= SHR64(xh, 5) ^ SHL64(q[18], 5);
	h[3] ^= SHR64(xh, 1) ^ SHL64(q[19], 5);
	h[4] ^= SHR64(xh, 3) ^ q[20];
	h[5] ^= SHL64(xh, 6) ^ SHR64(q[21], 6);
	h[6] ^= SHR64(xh, 4) ^ SHL64(q[22], 6);
	h[7] ^= SHR64(xh,11) ^ SHL64(q[23], 2);
	for(i=0; i<8; ++i){
		h[i] += xl ^ q[24+i] ^ q[i];
	}
	for(i=0; i<8; ++i){
		h[8+i] ^= xh ^ q[24+i];
		h[8+i] += ROTL64(h[(4+i)%8],i+9);
	}
	h[ 8] += SHL64(xl, 8) ^ q[23] ^ q[ 8];
	h[ 9] += SHR64(xl, 6) ^ q[16] ^ q[ 9];
	h[10] += SHL64(xl, 6) ^ q[17] ^ q[10];
	h[11] += SHL64(xl, 4) ^ q[18] ^ q[11];
	h[12] += SHR64(xl, 3) ^ q[19] ^ q[12];
	h[13] += SHR64(xl, 4) ^ q[20] ^ q[13];
	h[14] += SHR64(xl, 7) ^ q[21] ^ q[14];
	h[15] += SHR64(xl, 2) ^ q[22] ^ q[15];
}

void bmw_large_nextBlock(bmw_large_ctx_t* ctx, const void* block){
	uint64_t q[32];
	dump_x(block, 16, 'M');
	bmw_large_f0(q, ctx->h, block);
	dump_x(q, 16, 'Q');
	bmw_large_f1(q, block, ctx->h);
	dump_x(q, 32, 'Q');
	bmw_large_f2(ctx->h, q, block);
	ctx->counter += 1;
	ctx_dump(ctx);
}

void bmw_large_lastBlock(bmw_large_ctx_t* ctx, const void* block, uint16_t length_b){
	union {
		uint8_t   v8[128];
		uint64_t v64[ 16];
	} buffer;
	while(length_b >= BMW_LARGE_BLOCKSIZE){
		bmw_large_nextBlock(ctx, block);
		length_b -= BMW_LARGE_BLOCKSIZE;
		block = (uint8_t*)block + BMW_LARGE_BLOCKSIZE_B;
	}
	memset(buffer.v8, 0, 128);
	memcpy(buffer.v8, block, (length_b+7)/8);
	buffer.v8[length_b>>3] |= 0x80 >> (length_b&0x07);
	if(length_b+1>128*8-64){
		bmw_large_nextBlock(ctx, buffer.v8);
		memset(buffer.v8, 0, 128-8);
		ctx->counter -= 1;
	}
	buffer.v64[15] = (uint64_t)(ctx->counter*1024LL)+(uint64_t)length_b;
	bmw_large_nextBlock(ctx, buffer.v8);
#if TWEAK
	uint8_t i;
	uint64_t q[32];
	memset(buffer.v8, 0xaa, 128);
	for(i=0; i<16; ++i){
		buffer.v8[8*i] = i + 0xa0;
	}
	bmw_large_f0(q, buffer.v64, ctx->h);
	bmw_large_f1(q, ctx->h, buffer.v64);
	bmw_large_f2(buffer.v64, q, ctx->h);
	memcpy(ctx->h, buffer.v8, 128);
#endif
}

void bmw384_init(bmw384_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x0001020304050607LL;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x0808080808080808LL;
	}
#if BUG24
	ctx->h[6] = 0x3031323324353637LL;
#endif
	ctx->counter=0;
	ctx_dump(ctx);
}

void bmw512_init(bmw512_ctx_t* ctx){
	uint8_t i;
	ctx->h[0] = 0x8081828384858687LL;
	for(i=1; i<16; ++i){
		ctx->h[i] = ctx->h[i-1]+ 0x0808080808080808LL;
	}
	ctx->counter=0;
	ctx_dump(ctx);
}

void bmw384_nextBlock(bmw384_ctx_t* ctx, const void* block){
	bmw_large_nextBlock(ctx, block);
}

void bmw512_nextBlock(bmw512_ctx_t* ctx, const void* block){
	bmw_large_nextBlock(ctx, block);
}

void bmw384_lastBlock(bmw384_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_large_lastBlock(ctx, block, length_b);
}

void bmw512_lastBlock(bmw512_ctx_t* ctx, const void* block, uint16_t length_b){
	bmw_large_lastBlock(ctx, block, length_b);
}

void bmw384_ctx2hash(void* dest, const bmw384_ctx_t* ctx){
	memcpy(dest, &(ctx->h[10]), 384/8);
}

void bmw512_ctx2hash(void* dest, const bmw512_ctx_t* ctx){
	memcpy(dest, &(ctx->h[8]), 512/8);
}

void bmw384(void* dest, const void* msg, uint32_t length_b){
	bmw_large_ctx_t ctx;
	bmw384_init(&ctx);
	while(length_b>=BMW_LARGE_BLOCKSIZE){
		bmw_large_nextBlock(&ctx, msg);
		length_b -= BMW_LARGE_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_LARGE_BLOCKSIZE_B;
	}
	bmw_large_lastBlock(&ctx, msg, length_b);
	bmw384_ctx2hash(dest, &ctx);
}

void bmw512(void* dest, const void* msg, uint32_t length_b){
	bmw_large_ctx_t ctx;
	bmw512_init(&ctx);
	while(length_b>=BMW_LARGE_BLOCKSIZE){
		bmw_large_nextBlock(&ctx, msg);
		length_b -= BMW_LARGE_BLOCKSIZE;
		msg = (uint8_t*)msg + BMW_LARGE_BLOCKSIZE_B;
	}
	bmw_large_lastBlock(&ctx, msg, length_b);
	bmw512_ctx2hash(dest, &ctx);
}

