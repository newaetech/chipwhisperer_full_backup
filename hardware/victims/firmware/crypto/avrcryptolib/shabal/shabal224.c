/* shabal224.c */
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
 * \file    shabal224.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 * 
 */

#include <stdint.h>
#include "shabal.h"
#include <avr/pgmspace.h>
#include <string.h>

const uint32_t shabal224_iv[] PROGMEM = {
	/* A */
	0xA5201467, 0xA9B8D94A, 0xD4CED997, 0x68379D7B, 
	0xA7FC73BA, 0xF1A2546B, 0x606782BF, 0xE0BCFD0F,
	0x2F25374E, 0x069A149F, 0x5E2DFF25, 0xFAECF061,
	/* B */ 
	0xEC9905D8, 0xF21850CF, 0xC0A746C8, 0x21DAD498,
	0x35156EEB, 0x088C97F2, 0x26303E40, 0x8A2D4FB5,
    0xFEEE44B6, 0x8A1E9573, 0x7B81111A, 0xCBC139F0, 
	0xA3513861, 0x1D2C362E, 0x918C580E, 0xB58E1B9C,
	/* C */
	0xE4B573A1, 0x4C1A0880, 0x1E907C51, 0x04807EFD, 
	0x3AD8CDE5, 0x16B21302, 0x02512C53, 0x2204CB18,
    0x99405F2D, 0xE5B648A1, 0x70AB1D43, 0xA10C25C2, 
	0x16F1AC05, 0x38BBEB56, 0x9B01DC60, 0xB1096D83
};

void shabal224_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal224_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal224_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal224_iv[SHABAL_R+16+i]));
	}
}

void shabal224_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 224);
}

void shabal224(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal224_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal224_ctx2hash(dest, &ctx);
}
