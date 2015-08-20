/* shabal192.c */
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
 * \file    shabal192.c
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

const uint32_t shabal192_iv[] PROGMEM = {
	/* A */
	0xFD749ED4, 0xB798E530, 0x33904B6F, 0x46BDA85E,
	0x076934B4, 0x454B4058, 0x77F74527, 0xFB4CF465,
    0x62931DA9, 0xE778C8DB, 0x22B3998E, 0xAC15CFB9,
	/* B */
	0x58BCBAC4, 0xEC47A08E, 0xAEE933B2, 0xDFCBC824,
	0xA7944804, 0xBF65BDB0, 0x5A9D4502, 0x59979AF7,
    0xC5CEA54E, 0x4B6B8150, 0x16E71909, 0x7D632319,
	0x930573A0, 0xF34C63D1, 0xCAF914B4, 0xFDD6612C,
	/* C */
	0x61550878, 0x89EF2B75, 0xA1660C46, 0x7EF3855B,
	0x7297B58C, 0x1BC67793, 0x7FB1C723, 0xB66FC640,
    0x1A48B71C, 0xF0976D17, 0x088CE80A, 0xA454EDF3,
	0x1C096BF4, 0xAC76224B, 0x5215781C, 0xCD5D2669
};

void shabal192_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal192_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal192_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal192_iv[SHABAL_R+16+i]));
	}
}

void shabal192_ctx2hash(void* dest, const shabal_ctx_t* ctx){
	shabal_ctx2hash(dest, ctx, 192);
}

void shabal192(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal192_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal192_ctx2hash(dest, &ctx);
}
