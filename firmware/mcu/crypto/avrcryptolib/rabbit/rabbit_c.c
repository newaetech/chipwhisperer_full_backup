/* rabbit_c.c */
/*
    This file is part of the ARM-Crypto-Lib.
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

#include <avr/pgmspace.h>
#include <string.h>
#include <stdint.h>
#include "rabbit.h"

#ifndef ESTREAM
#define ESTREAM 0
#endif

/*
void dump_ctx(rabbit_ctx_t* ctx){
	uint8_t i=0;
	cli_putstr_P(PSTR("\r\n --- ctx dump ---\r\n  b = "));
	cli_hexdump_byte(ctx->carry);
	do{
		if((i&3)==0){
			cli_putstr_P(PSTR("\r\n"));
		}
		cli_putstr_P(PSTR("  X"));
		cli_hexdump_byte(i);
		cli_putstr_P(PSTR(" = 0x"));
		cli_hexdump_rev(&(ctx->x[i]), 4);
	}while(++i<8);
	i=0;
	do{
		if((i&3)==0){
			cli_putstr_P(PSTR("\r\n"));
		}
		cli_putstr_P(PSTR("  C"));
		cli_hexdump_byte(i);
		cli_putstr_P(PSTR(" = 0x"));
		cli_hexdump_rev(&(ctx->c[i]), 4);
	}while(++i<8);
}
*/

static
const uint32_t c_const[8] PROGMEM = {
		0x4D34D34D, 0xD34D34D3,
		0x34D34D34, 0x4D34D34D,
		0xD34D34D3, 0x34D34D34,
		0x4D34D34D, 0xD34D34D3
};

static
void gen_g(uint32_t* dest, rabbit_ctx_t* ctx){
	uint8_t i=0;
	uint64_t a;
	uint32_t t, *x, *c;
	x = ctx->x;
	c = ctx->c;
	do{
		t = *x++ + *c++;
		a = ((uint64_t)t)*((uint64_t)t);
		dest[i] = (uint32_t)(a^(a>>32));
	}while(++i<8);
}

static
void update_c(rabbit_ctx_t* ctx){
	uint8_t i=0;
	uint64_t a;
	uint32_t *c;
	const uint32_t *con;
	c = ctx->c;
	con = c_const;
	a = ctx->carry;
	do{
		a += *c;
		a += pgm_read_dword(con++);
		*c++ = (uint32_t)a;
		a >>= 32;
	}while(++i<8);
	ctx->carry = a?1:0;
}

#define ROT16(a) (((a)<<16) | ((a)>>16))
#define ROT8(a)  (((a)<< 8) | ((a)>>24))

static
void step(rabbit_ctx_t* ctx){
	uint32_t g[8];
	uint8_t i=0;
	update_c(ctx);
	gen_g(g, ctx);
	memcpy(ctx->x, g, 8*4);
	do{
		ctx->x[i] += ROT16(g[(i+8-1)%8]) + ROT16(g[(i+8-2)%8]);
		++i;
		ctx->x[i] += ROT8(g[(i+8-1)%8]) + g[(i+8-2)%8];
	}while(++i<8);
}

static
void keysetup(rabbit_ctx_t* ctx, const void* key){
	uint16_t *x, *c;
	uint8_t i=0;
	x = (uint16_t*)(ctx->x);
	c = (uint16_t*)(ctx->c);
	ctx->carry = 0;
	do{
		*x++ = ((uint16_t*)key)[i];
		*x++ = ((uint16_t*)key)[(i+1)%8];
		*c++ = ((uint16_t*)key)[(i+5)%8];
		*c++ = ((uint16_t*)key)[(i+4)%8];
		++i;
		*x++ = ((uint16_t*)key)[(i+4)%8];
		*x++ = ((uint16_t*)key)[(i+5)%8];
		*c++ = ((uint16_t*)key)[(i+1)%8];
		*c++ = ((uint16_t*)key)[i];
	}while(++i<8);
	i=0;
	do{
		step(ctx);
	}while(++i<4);
	i=0;
	do{
		ctx->c[i] ^= ctx->x[(i+4)%8];
	}while(++i<8);
}

static
void ivsetup(rabbit_ctx_t* ctx, const void* iv){
	uint8_t i;
	uint32_t t;
	union __attribute__((packed)){
		uint8_t v8[8];
		uint16_t v16[4];
		uint32_t v32[2];
	}t_iv;
	i=0;
#if ESTREAM
	memcpy(t_iv.v8, iv, 8);
#else
	do{
		t_iv.v8[i] = ((uint8_t*)iv)[7-i];
		t_iv.v8[7-i] = ((uint8_t*)iv)[i];
	}while(++i<4);
#endif
	ctx->c[0] ^= t_iv.v32[0];
	ctx->c[4] ^= t_iv.v32[0];
	ctx->c[2] ^= t_iv.v32[1];
	ctx->c[6] ^= t_iv.v32[1];
	t = ( ((uint32_t)(t_iv.v16[3]))<<16) | (t_iv.v16[1]);
	ctx->c[1] ^= t;
	ctx->c[5] ^= t;
	t = ( ((uint32_t)(t_iv.v16[2]))<<16) | (t_iv.v16[0]);
	ctx->c[3] ^= t;
	ctx->c[7] ^= t;
	i=4;
	do{
		step(ctx);
	}while(--i);
}

static
void extract(rabbit_ctx_t* ctx){
	int8_t i=0;
	uint8_t *t;
	uint16_t v;
	t = ctx->buffer;
	i=6;
	do{
		v =   ((uint16_t*)(ctx->x))[(2*(i+ 8)+1)%16]
		    ^ ((uint16_t*)(ctx->x))[(2*(i+11)+0)%16];
		*t++ = v>>8;
		*t++ = (uint8_t)v;
		v =   ((uint16_t*)(ctx->x))[(2*(i+ 8)+0)%16]
		    ^ ((uint16_t*)(ctx->x))[(2*(i+13)+1)%16];
		*t++ = v>>8;
		*t++ = (uint8_t)v;

		i-=2;
	}while(i>=0);
#if ESTREAM
	uint8_t x;
	i=0;
	do{
		x = ctx->buffer[i];
		ctx->buffer[i] = ctx->buffer[15-i];
		ctx->buffer[15-i] = x;
	}while(++i<8);
#endif
}

static const uint8_t key80_pad[] PROGMEM = { 0xDE, 0x05, 0x6E, 0xAC, 0x8A, 0x11 };

void rabbit_init(const void* key, uint16_t keysize_b,
                  const void* iv,
                  rabbit_ctx_t* ctx){
	uint8_t t_key[16];
	if(keysize_b==80){
		memcpy(t_key, key, 10);
		memcpy_P(t_key+10, key80_pad, 6);
	}else{
		memcpy(t_key, key, 16);
	}
#if !ESTREAM
	uint8_t i=0, t;
	do{
		t = t_key[i];
		t_key[i] = t_key[15-i];
		t_key[15-i] = t;
	}while(++i<8);
#endif
	keysetup(ctx, t_key);


	if(iv){
		ivsetup(ctx, iv);
	}
	extract(ctx);
	ctx->buffer_idx = 16;
}

uint8_t rabbit_gen(rabbit_ctx_t* ctx){
	if(ctx->buffer_idx==16){
		step(ctx);
		extract(ctx);
		ctx->buffer_idx = 0;
	}
	return ctx->buffer[ctx->buffer_idx++];
}
