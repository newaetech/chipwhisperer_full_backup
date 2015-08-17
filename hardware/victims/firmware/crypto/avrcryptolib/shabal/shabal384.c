/* shabal384.c */
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
 * \file    shabal384.c
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

const uint32_t shabal384_iv[] PROGMEM = {
	/* A */ 
	0xC8FCA331, 0xE55C504E, 0x003EBF26, 0xBB6B8D83, 
	0x7B0448C1, 0x41B82789, 0x0A7C9601, 0x8D659CFF,
	0xB6E2673E, 0xCA54C77B, 0x1460FD7E, 0x3FCB8F2D,
	/* B */ 
	0x527291FC, 0x2A16455F, 0x78E627E5, 0x944F169F, 
	0x1CA6F016, 0xA854EA25, 0x8DB98ABE, 0xF2C62641,
    0x30117DCB, 0xCF5C4309, 0x93711A25, 0xF9F671B8, 
	0xB01D2116, 0x333F4B89, 0xB285D165, 0x86829B36,
	/* C */ 
	0xF764B11A, 0x76172146, 0xCEF6934D, 0xC6D28399, 
	0xFE095F61, 0x5E6018B4, 0x5048ECF5, 0x51353261,
    0x6E6E36DC, 0x63130DAD, 0xA9C69BD6, 0x1E90EA0C, 
	0x7C35073B, 0x28D95E6D, 0xAA340E0D, 0xCB3DEE70
};

void shabal384_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal384_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal384_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal384_iv[SHABAL_R+16+i]));
	}
}

void shabal384_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 384);
}

void shabal384(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal384_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal384_ctx2hash(dest, &ctx);
}
