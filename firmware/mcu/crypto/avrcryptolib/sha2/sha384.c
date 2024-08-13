/* sha384.c */
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
#include "sha384.h"


void sha384_nextBlock (sha384_ctx_t* ctx, const void* block){
	sha2_large_common_nextBlock(ctx, block);
}

void sha384_lastBlock(sha384_ctx_t* ctx, const void* block, uint16_t length_b){
	sha2_large_common_lastBlock(ctx, block, length_b);
}

static const
uint64_t sha384_init_values[8] PROGMEM = {
0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4
};


void sha384_init(sha384_ctx_t* ctx){
	ctx->length = 0;
	memcpy_P(ctx->h, sha384_init_values, 8*8);
}

void sha384_ctx2hash(void* dest, const sha384_ctx_t* ctx){
	uint8_t i=6, j, *s = (uint8_t*)(ctx->h);
	do{
		j=7;
		do{
			*((uint8_t*)dest) = s[j];
			dest = (uint8_t*)dest + 1;
		}while(j--);
		s += 8;
	}while(--i);
}


void sha384(void* dest, const void* msg, uint32_t length_b){
	sha384_ctx_t ctx;
	sha384_init(&ctx);
	while(length_b >= 1024){
		sha384_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + 1024/8;
		length_b -= 1024;
	}
	sha384_lastBlock(&ctx, msg, length_b);
	sha384_ctx2hash(dest, &ctx);
}
