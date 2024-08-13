/* shabal256.c */
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
 * \file    shabal256.c
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

const uint32_t shabal256_iv[] PROGMEM = {
	/* A */ 
	0x52F84552, 0xE54B7999, 0x2D8EE3EC, 0xB9645191, 
	0xE0078B86, 0xBB7C44C9, 0xD2B5C1CA, 0xB0D2EB8C,
	0x14CE5A45, 0x22AF50DC, 0xEFFDBC6B, 0xEB21B74A,
	/* B */ 
	0xB555C6EE, 0x3E710596, 0xA72A652F, 0x9301515F, 
	0xDA28C1FA, 0x696FD868, 0x9CB6BF72, 0x0AFE4002,
    0xA6E03615, 0x5138C1D4, 0xBE216306, 0xB38B8890, 
	0x3EA8B96B, 0x3299ACE4, 0x30924DD4, 0x55CB34A5,
	/* C */ 
	0xB405F031, 0xC4233EBA, 0xB3733979, 0xC0DD9D55,
	0xC51C28AE, 0xA327B8E1, 0x56C56167, 0xED614433,
	0x88B59D60, 0x60E2CEBA, 0x758B4B8B, 0x83E82A7F, 
	0xBC968828, 0xE6E00BF7, 0xBA839E55, 0x9B491C60
};

void shabal256_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal256_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal256_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal256_iv[SHABAL_R+16+i]));
	}
}

void shabal256_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 256);
}

void shabal256(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal256_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal256_ctx2hash(dest, &ctx);
}
