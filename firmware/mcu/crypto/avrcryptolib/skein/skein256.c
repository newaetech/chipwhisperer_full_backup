/* skein256.c */
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

#include "cli.h"

void skein256_init(skein256_ctx_t* ctx, uint16_t outsize_b){
	skein_config_t conf;
	uint8_t null[UBI256_BLOCKSIZE_B];
	memset(null, 0, UBI256_BLOCKSIZE_B);
	memset(&conf, 0, sizeof(skein_config_t));
	conf.schema[0] = 'S';
	conf.schema[1] = 'H';
	conf.schema[2] = 'A';
	conf.schema[3] = '3';
	conf.version = 1;
	conf.out_length = outsize_b;
	ctx->outsize_b = outsize_b;
	ubi256_init(&(ctx->ubictx), null, UBI_TYPE_CFG);
	ubi256_lastBlock(&(ctx->ubictx), &conf, 256);
	ubi256_init(&(ctx->ubictx), ctx->ubictx.g, UBI_TYPE_MSG);
}

void skein256_nextBlock(skein256_ctx_t* ctx, const void* block){
	ubi256_nextBlock(&(ctx->ubictx), block);
}

void skein256_lastBlock(skein256_ctx_t* ctx, const void* block, uint16_t length_b){
	ubi256_lastBlock(&(ctx->ubictx), block, length_b);
}

void skein256_ctx2hash(void* dest, skein256_ctx_t* ctx){
	ubi256_ctx_t uctx;
	uint16_t outsize_b;
	
	uint64_t counter=0;
	uint8_t outbuffer[UBI256_BLOCKSIZE_B];
	ubi256_init(&(ctx->ubictx), ctx->ubictx.g, UBI_TYPE_OUT);
	
	outsize_b = ctx->outsize_b;
	while(1){
		memcpy(&uctx, &(ctx->ubictx), sizeof(ubi256_ctx_t));
		ubi256_lastBlock(&uctx, &counter, 64);
		ubi256_ctx2hash(outbuffer, &uctx);
		if(outsize_b<=UBI256_BLOCKSIZE){
			memcpy(dest, outbuffer, (outsize_b+7)/8);
			break;
		}else{
			memcpy(dest, outbuffer, UBI256_BLOCKSIZE_B);
			dest = (uint8_t*)dest + UBI256_BLOCKSIZE_B;
			outsize_b -= UBI256_BLOCKSIZE;
			counter++;
		}
	}
}

void skein256(void* dest, uint16_t outlength_b,const void* msg, uint32_t length_b){
	skein256_ctx_t ctx;
	skein256_init(&ctx, outlength_b);
	while(length_b>SKEIN256_BLOCKSIZE){
		skein256_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + SKEIN256_BLOCKSIZE_B;
		length_b -= SKEIN256_BLOCKSIZE;
	}
	skein256_lastBlock(&ctx, msg, length_b);
	skein256_ctx2hash(dest, &ctx);
}
