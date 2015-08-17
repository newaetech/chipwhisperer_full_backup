/* groestl_small.h */
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
 * \file    groestl_small.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-19
 * \license GPLv3 or later
 * 
 */
#ifndef GROESTL_SMALL_H_
#define GROESTL_SMALL_H_

#include <stdint.h>

#define GROESTL_SMALL_BLOCKSIZE   512
#define GROESTL_SMALL_BLOCKSIZE_B ((GROESTL_SMALL_BLOCKSIZE+7)/8)
#define GROESTL224_BLOCKSIZE      GROESTL_SMALL_BLOCKSIZE
#define GROESTL224_BLOCKSIZE_B    GROESTL_SMALL_BLOCKSIZE_B
#define GROESTL256_BLOCKSIZE      GROESTL_SMALL_BLOCKSIZE
#define GROESTL256_BLOCKSIZE_B    GROESTL_SMALL_BLOCKSIZE_B

typedef struct {
	uint8_t h[8*8];
	uint32_t counter;
} groestl_small_ctx_t;

typedef groestl_small_ctx_t groestl224_ctx_t;
typedef groestl_small_ctx_t groestl256_ctx_t;

void groestl224_init(groestl224_ctx_t* ctx);
void groestl256_init(groestl256_ctx_t* ctx);

void groestl_small_nextBlock(groestl_small_ctx_t* ctx, const void* block);
void groestl_small_lastBlock(groestl_small_ctx_t* ctx, const void* block, uint16_t length_b);

void groestl224_nextBlock(groestl224_ctx_t* ctx, const void* block);
void groestl224_lastBlock(groestl224_ctx_t* ctx, const void* block, uint16_t length_b);

void groestl256_nextBlock(groestl256_ctx_t* ctx, const void* block);
void groestl256_lastBlock(groestl256_ctx_t* ctx, const void* block, uint16_t length_b);

void groestl224_ctx2hash(void* dest, const groestl224_ctx_t* ctx);
void groestl256_ctx2hash(void* dest, const groestl256_ctx_t* ctx);

void groestl224(void* dest, const void* msg, uint32_t length_b);
void groestl256(void* dest, const void* msg, uint32_t length_b);

#endif /* GROESTL_GROESTL_H_ */
