/* skein512.c */
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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-12
 * \license GPLv3 or later
 * 
 */

#include <stdint.h>
#include <string.h>
#include "ubi.h"
#include "skein.h"


void skein512_init(skein512_ctx_t* ctx, uint16_t outsize_b){
	skein_config_t conf;
	uint8_t null[UBI512_BLOCKSIZE_B];
	memset(null, 0, UBI512_BLOCKSIZE_B);
	memset(&conf, 0, sizeof(skein_config_t));
	conf.schema[0] = 'S';
	conf.schema[1] = 'H';
	conf.schema[2] = 'A';
	conf.schema[3] = '3';
	conf.version = 1;
	conf.out_length = outsize_b;
	ctx->outsize_b = outsize_b;
	ubi512_init(&(ctx->ubictx), null, UBI_TYPE_CFG);
	ubi512_lastBlock(&(ctx->ubictx), &conf, 256);
	ubi512_init(&(ctx->ubictx), ctx->ubictx.g, UBI_TYPE_MSG);
}

void skein512_nextBlock(skein512_ctx_t* ctx, const void* block){
	ubi512_nextBlock(&(ctx->ubictx), block);
}

void skein512_lastBlock(skein512_ctx_t* ctx, const void* block, uint16_t length_b){
	ubi512_lastBlock(&(ctx->ubictx), block, length_b);
}

void skein512_ctx2hash(void* dest, skein512_ctx_t* ctx){
	ubi512_ctx_t uctx;
	uint16_t outsize_b;
	
	uint64_t counter=0;
	uint8_t outbuffer[UBI512_BLOCKSIZE_B];
	ubi512_init(&(ctx->ubictx), ctx->ubictx.g, UBI_TYPE_OUT);
	
	outsize_b = ctx->outsize_b;
	while(1){
		memcpy(&uctx, &(ctx->ubictx), sizeof(ubi512_ctx_t));
		ubi512_lastBlock(&uctx, &counter, 64);
		ubi512_ctx2hash(outbuffer, &uctx);
		if(outsize_b<=UBI512_BLOCKSIZE){
			memcpy(dest, outbuffer, (ctx->outsize_b+7)/8);
			break;
		}else{
			memcpy(dest, outbuffer, UBI512_BLOCKSIZE_B);
			dest = (uint8_t*)dest + UBI512_BLOCKSIZE_B;
			outsize_b -= UBI512_BLOCKSIZE;
			counter++;
		}
	}
}

void skein512(void* dest, uint16_t outlength_b,const void* msg, uint32_t length_b){
	skein512_ctx_t ctx;
	skein512_init(&ctx, outlength_b);
	while(length_b>SKEIN512_BLOCKSIZE){
		skein512_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + SKEIN512_BLOCKSIZE_B;
		length_b -= SKEIN512_BLOCKSIZE;
	}
	skein512_lastBlock(&ctx, msg, length_b);
	skein512_ctx2hash(dest, &ctx);
}

