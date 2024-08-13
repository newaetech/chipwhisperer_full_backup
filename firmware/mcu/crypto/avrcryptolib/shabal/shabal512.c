/* shabal512.c */
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
 * \file    shabal512.c
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

const uint32_t shabal512_iv[] PROGMEM = {
	/* A */ 
	0x20728DFD, 0x46C0BD53, 0xE782B699, 0x55304632, 
	0x71B4EF90, 0x0EA9E82C, 0xDBB930F1, 0xFAD06B8B,
	0xBE0CAE40, 0x8BD14410, 0x76D2ADAC, 0x28ACAB7F,
	/* B */ 
	0xC1099CB7, 0x07B385F3, 0xE7442C26, 0xCC8AD640, 
	0xEB6F56C7, 0x1EA81AA9, 0x73B9D314, 0x1DE85D08,
	0x48910A5A, 0x893B22DB, 0xC5A0DF44, 0xBBC4324E, 
	0x72D2F240, 0x75941D99, 0x6D8BDE82, 0xA1A7502B,
	/* C */
	0xD9BF68D1, 0x58BAD750, 0x56028CB2, 0x8134F359, 
	0xB5D469D8, 0x941A8CC2, 0x418B2A6E, 0x04052780,
	0x7F07D787, 0x5194358F, 0x3C60D665, 0xBE97D79A, 
	0x950C3434, 0xAED9A06D, 0x2537DC8D, 0x7CDB5969,
};

void shabal512_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal512_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal512_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal512_iv[SHABAL_R+16+i]));
	}
}

void shabal512_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 512);
}

void shabal512(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal512_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal512_ctx2hash(dest, &ctx);
}
