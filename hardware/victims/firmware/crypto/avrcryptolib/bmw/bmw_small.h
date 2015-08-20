/* bmw_small.h */
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
 * \file    bmw_small.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 * 
 */
#ifndef BMW_SMALL_H_
#define BMW_SMALL_H_

#include <stdint.h>

#define BMW_SMALL_BLOCKSIZE   512
#define BMW_SMALL_BLOCKSIZE_B ((BMW_SMALL_BLOCKSIZE+7)/8)
#define BMW224_BLOCKSIZE      BMW_SMALL_BLOCKSIZE
#define BMW224_BLOCKSIZE_B    BMW_SMALL_BLOCKSIZE_B
#define BMW256_BLOCKSIZE      BMW_SMALL_BLOCKSIZE
#define BMW256_BLOCKSIZE_B    BMW_SMALL_BLOCKSIZE_B

typedef struct {
	uint32_t h[16];
	uint32_t counter;
} bmw_small_ctx_t;

typedef bmw_small_ctx_t bmw224_ctx_t;
typedef bmw_small_ctx_t bmw256_ctx_t;

void bmw224_init(bmw224_ctx_t* ctx);
void bmw256_init(bmw256_ctx_t* ctx);

void bmw_small_nextBlock(bmw_small_ctx_t* ctx, const void* block);
void bmw_small_lastBlock(bmw_small_ctx_t* ctx, const void* block, uint16_t length_b);

void bmw224_nextBlock(bmw224_ctx_t* ctx, const void* block);
void bmw224_lastBlock(bmw224_ctx_t* ctx, const void* block, uint16_t length_b);

void bmw256_nextBlock(bmw256_ctx_t* ctx, const void* block);
void bmw256_lastBlock(bmw256_ctx_t* ctx, const void* block, uint16_t length_b);

void bmw224_ctx2hash(void* dest, const bmw224_ctx_t* ctx);
void bmw256_ctx2hash(void* dest, const bmw256_ctx_t* ctx);

void bmw224(void* dest, const void* msg, uint32_t length_b);
void bmw256(void* dest, const void* msg, uint32_t length_b);

#endif /* BMW_SMALL_H_ */
