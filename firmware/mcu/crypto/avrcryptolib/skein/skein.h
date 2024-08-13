/* skein.h */
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
 * \file    skein.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-12
 * \license GPLv3 or later
 * 
 */

#ifndef SKEIN_H_
#define SKEIN_H_

#include <stdint.h>
#include "ubi.h"

#define SKEIN256_BLOCKSIZE    UBI256_BLOCKSIZE
#define SKEIN256_BLOCKSIZE_B  UBI256_BLOCKSIZE_B

#define SKEIN512_BLOCKSIZE    UBI512_BLOCKSIZE
#define SKEIN512_BLOCKSIZE_B  UBI512_BLOCKSIZE_B

#define SKEIN1024_BLOCKSIZE   UBI1024_BLOCKSIZE
#define SKEIN1024_BLOCKSIZE_B UBI1024_BLOCKSIZE_B

typedef struct{
	 uint16_t outsize_b;
	 ubi256_ctx_t ubictx;
}skein256_ctx_t;

void skein256_init(skein256_ctx_t* ctx, uint16_t outsize_b);
void skein256_nextBlock(skein256_ctx_t* ctx, const void* block);
void skein256_lastBlock(skein256_ctx_t* ctx, const void* block, uint16_t length_b);
void skein256_ctx2hash(void* dest, skein256_ctx_t* ctx);
void skein256(void* dest, uint16_t outlength_b, const void* msg, uint32_t length_b);

typedef struct{
	 uint16_t outsize_b;
	 ubi512_ctx_t ubictx;
}skein512_ctx_t;

void skein512_init(skein512_ctx_t* ctx, uint16_t outsize_b);
void skein512_nextBlock(skein512_ctx_t* ctx, const void* block);
void skein512_lastBlock(skein512_ctx_t* ctx, const void* block, uint16_t length_b);
void skein512_ctx2hash(void* dest, skein512_ctx_t* ctx);
void skein512(void* dest, uint16_t outlength_b, const void* msg, uint32_t length_b);

typedef struct{
	 uint16_t outsize_b;
	 ubi1024_ctx_t ubictx;
}skein1024_ctx_t;

void skein1024_init(skein1024_ctx_t* ctx, uint16_t outsize_b);
void skein1024_nextBlock(skein1024_ctx_t* ctx, const void* block);
void skein1024_lastBlock(skein1024_ctx_t* ctx, const void* block, uint16_t length_b);
void skein1024_ctx2hash(void* dest, skein1024_ctx_t* ctx);
void skein1024(void* dest, uint16_t outlength_b, const void* msg, uint32_t length_b);

#endif /* SKEIN_H_ */
