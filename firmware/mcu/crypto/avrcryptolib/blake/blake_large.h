/* blake_large.h */
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
 * \file    blake_large.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-08
 * \license GPLv3 or later
 * 
 */
#ifndef BLAKE_LARGE_H_
#define BLAKE_LARGE_H_

#include <stdint.h>

#define BLAKE_LARGE_BLOCKSIZE   1024
#define BLAKE_LARGE_BLOCKSIZE_B ((BLAKE_LARGE_BLOCKSIZE+7)/8)
#define BLAKE384_BLOCKSIZE      BLAKE_LARGE_BLOCKSIZE
#define BLAKE384_BLOCKSIZE_B    BLAKE_LARGE_BLOCKSIZE_B
#define BLAKE512_BLOCKSIZE      BLAKE_LARGE_BLOCKSIZE
#define BLAKE512_BLOCKSIZE_B    BLAKE_LARGE_BLOCKSIZE_B

typedef struct {
	uint64_t h[8];
	uint64_t s[4];
	uint32_t counter;
	uint8_t  appendone;
} blake_large_ctx_t;

typedef blake_large_ctx_t blake384_ctx_t;
typedef blake_large_ctx_t blake512_ctx_t;

void blake384_init(blake384_ctx_t* ctx);
void blake512_init(blake512_ctx_t* ctx);

void blake_large_nextBlock(blake_large_ctx_t* ctx, const void* block);
void blake_large_lastBlock(blake_large_ctx_t* ctx, const void* block, uint16_t length_b);

void blake384_nextBlock(blake384_ctx_t* ctx, const void* block);
void blake384_lastBlock(blake384_ctx_t* ctx, const void* block, uint16_t length_b);

void blake512_nextBlock(blake512_ctx_t* ctx, const void* block);
void blake512_lastBlock(blake512_ctx_t* ctx, const void* block, uint16_t length_b);

void blake384_ctx2hash(void* dest, const blake384_ctx_t* ctx);
void blake512_ctx2hash(void* dest, const blake512_ctx_t* ctx);

void blake384(void* dest, const void* msg, uint32_t length_b);
void blake512(void* dest, const void* msg, uint32_t length_b);

#endif /* BLAKE_LARGE_H_ */
