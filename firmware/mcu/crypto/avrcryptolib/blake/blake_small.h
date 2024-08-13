/* blake_small.h */
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
 * \file    blake_small.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 * 
 */
#ifndef BLAKE_SMALL_H_
#define BLAKE_SMALL_H_

#include <stdint.h>

#define BLAKE_SMALL_BLOCKSIZE   512
#define BLAKE_SMALL_BLOCKSIZE_B ((BLAKE_SMALL_BLOCKSIZE+7)/8)
#define BLAKE224_BLOCKSIZE      BLAKE_SMALL_BLOCKSIZE
#define BLAKE224_BLOCKSIZE_B    BLAKE_SMALL_BLOCKSIZE_B
#define BLAKE256_BLOCKSIZE      BLAKE_SMALL_BLOCKSIZE
#define BLAKE256_BLOCKSIZE_B    BLAKE_SMALL_BLOCKSIZE_B

typedef struct {
	uint32_t h[8];
	uint32_t s[4];
	uint32_t counter;
	uint8_t  appendone;
} blake_small_ctx_t;

typedef blake_small_ctx_t blake224_ctx_t;
typedef blake_small_ctx_t blake256_ctx_t;

void blake224_init(blake224_ctx_t* ctx);
void blake256_init(blake256_ctx_t* ctx);

void blake_small_nextBlock(blake_small_ctx_t* ctx, const void* block);
void blake_small_lastBlock(blake_small_ctx_t* ctx, const void* block, uint16_t length_b);

void blake224_nextBlock(blake224_ctx_t* ctx, const void* block);
void blake224_lastBlock(blake224_ctx_t* ctx, const void* block, uint16_t length_b);

void blake256_nextBlock(blake256_ctx_t* ctx, const void* block);
void blake256_lastBlock(blake256_ctx_t* ctx, const void* block, uint16_t length_b);

void blake224_ctx2hash(void* dest, const blake224_ctx_t* ctx);
void blake256_ctx2hash(void* dest, const blake256_ctx_t* ctx);

void blake224(void* dest, const void* msg, uint32_t length_b);
void blake256(void* dest, const void* msg, uint32_t length_b);

#endif /* BLAKE_SMALL_H_ */
