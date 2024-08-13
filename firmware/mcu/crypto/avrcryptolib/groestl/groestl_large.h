/* groestl_large.h */
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
 * \file    groestl_large.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-05-19
 * \license GPLv3 or later
 * 
 */
#ifndef GROESTL_LARGE_H_
#define GROESTL_LARGE_H_

#include <stdint.h>

#define GROESTL_LARGE_BLOCKSIZE   1024
#define GROESTL_LARGE_BLOCKSIZE_B ((GROESTL_LARGE_BLOCKSIZE+7)/8)
#define GROESTL384_BLOCKSIZE      GROESTL_LARGE_BLOCKSIZE
#define GROESTL384_BLOCKSIZE_B    GROESTL_LARGE_BLOCKSIZE_B
#define GROESTL512_BLOCKSIZE      GROESTL_LARGE_BLOCKSIZE
#define GROESTL512_BLOCKSIZE_B    GROESTL_LARGE_BLOCKSIZE_B

typedef struct {
	uint8_t h[8*16];
	uint32_t counter;
} groestl_large_ctx_t;

typedef groestl_large_ctx_t groestl384_ctx_t;
typedef groestl_large_ctx_t groestl512_ctx_t;

void groestl384_init(groestl384_ctx_t* ctx);
void groestl512_init(groestl512_ctx_t* ctx);

void groestl_large_nextBlock(groestl_large_ctx_t* ctx, const void* block);
void groestl_large_lastBlock(groestl_large_ctx_t* ctx, const void* block, uint16_t length_b);

void groestl384_nextBlock(groestl384_ctx_t* ctx, const void* block);
void groestl384_lastBlock(groestl384_ctx_t* ctx, const void* block, uint16_t length_b);

void groestl512_nextBlock(groestl512_ctx_t* ctx, const void* block);
void groestl512_lastBlock(groestl512_ctx_t* ctx, const void* block, uint16_t length_b);

void groestl384_ctx2hash(void* dest, const groestl384_ctx_t* ctx);
void groestl512_ctx2hash(void* dest, const groestl512_ctx_t* ctx);

void groestl384(void* dest, const void* msg, uint32_t length_b);
void groestl512(void* dest, const void* msg, uint32_t length_b);

#endif /* GROESTL_GROESTL_H_ */
