/* sha512.c */
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

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "sha2_large_common.h"
#include "sha512.h"


void sha512_nextBlock (sha512_ctx_t* ctx, const void* block){
	sha2_large_common_nextBlock(ctx, block);
}

void sha512_lastBlock(sha512_ctx_t* ctx, const void* block, uint16_t length_b){
	sha2_large_common_lastBlock(ctx, block, length_b);
}

static const
uint64_t sha512_init_values[8] PROGMEM = {
0x6a09e667f3bcc908LL, 0xbb67ae8584caa73bLL, 0x3c6ef372fe94f82bLL, 0xa54ff53a5f1d36f1LL,
0x510e527fade682d1LL, 0x9b05688c2b3e6c1fLL, 0x1f83d9abfb41bd6bLL, 0x5be0cd19137e2179LL
};


void sha512_init(sha512_ctx_t* ctx){
	ctx->length = 0;
	memcpy_P(ctx->h, sha512_init_values, 8*8);
}

void sha512_ctx2hash(void* dest, const sha512_ctx_t* ctx){
	uint8_t i=8, j, *s = (uint8_t*)(ctx->h);
	do{
		j=7;
		do{
			*((uint8_t*)dest) = s[j];
			dest = (uint8_t*)dest + 1;
		}while(j--);
		s += 8;
	}while(--i);
}


void sha512(void* dest, const void* msg, uint32_t length_b){
	sha512_ctx_t ctx;
	sha512_init(&ctx);
	while(length_b >= 1024){
		sha512_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 1024/8;
		length_b -= 1024;
	}
	sha512_lastBlock(&ctx, msg, length_b);
	sha512_ctx2hash(dest, &ctx);
}
