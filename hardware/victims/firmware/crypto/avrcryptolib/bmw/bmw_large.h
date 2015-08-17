/* bmw_large.h */
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
 * \file    bmw_large.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 * 
 */
#ifndef BMW_LARGE_H_
#define BMW_LARGE_H_

#include <stdint.h>

#define BMW_LARGE_BLOCKSIZE   1024
#define BMW_LARGE_BLOCKSIZE_B ((BMW_LARGE_BLOCKSIZE+7)/8)
#define BMW384_BLOCKSIZE      BMW_LARGE_BLOCKSIZE
#define BMW384_BLOCKSIZE_B    BMW_LARGE_BLOCKSIZE_B
#define BMW512_BLOCKSIZE      BMW_LARGE_BLOCKSIZE
#define BMW512_BLOCKSIZE_B    BMW_LARGE_BLOCKSIZE_B

typedef struct {
	uint64_t h[16];
	uint32_t counter;
} bmw_large_ctx_t;

typedef bmw_large_ctx_t bmw384_ctx_t;
typedef bmw_large_ctx_t bmw512_ctx_t;

void bmw384_init(bmw384_ctx_t* ctx);
void bmw512_init(bmw512_ctx_t* ctx);

void bmw_large_nextBlock(bmw_large_ctx_t* ctx, const void* block);
void bmw_large_lastBlock(bmw_large_ctx_t* ctx, const void* block, uint16_t length_b);

void bmw384_nextBlock(bmw384_ctx_t* ctx, const void* block);
void bmw384_lastBlock(bmw384_ctx_t* ctx, const void* block, uint16_t length_b);

void bmw512_nextBlock(bmw512_ctx_t* ctx, const void* block);
void bmw512_lastBlock(bmw512_ctx_t* ctx, const void* block, uint16_t length_b);

void bmw384_ctx2hash(void* dest, const bmw384_ctx_t* ctx);
void bmw512_ctx2hash(void* dest, const bmw512_ctx_t* ctx);

void bmw384(void* dest, const void* msg, uint32_t length_b);
void bmw512(void* dest, const void* msg, uint32_t length_b);

#endif /* BMW_LARGE_H_ */
