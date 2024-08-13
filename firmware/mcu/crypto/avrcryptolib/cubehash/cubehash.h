/* cubehash.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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

#ifndef CUBEHASH_H_
#define CUBEHASH_H_

#include <stdint.h>

#define CUBEHASH224_BLOCKSIZE 256
#define CUBEHASH224_BLOCKSIZE_B ((CUBEHASH224_BLOCKSIZE+7)/8)
#define CUBEHASH256_BLOCKSIZE 256
#define CUBEHASH256_BLOCKSIZE_B ((CUBEHASH256_BLOCKSIZE+7)/8)
#define CUBEHASH384_BLOCKSIZE 256
#define CUBEHASH384_BLOCKSIZE_B ((CUBEHASH284_BLOCKSIZE+7)/8)
#define CUBEHASH512_BLOCKSIZE 256
#define CUBEHASH512_BLOCKSIZE_B ((CUBEHASH512_BLOCKSIZE+7)/8)

typedef struct {
	uint32_t a[32];
	uint8_t rounds;
	uint8_t blocksize_B;
} cubehash_ctx_t;

void cubehash_init(uint8_t r, uint8_t b, uint16_t h, cubehash_ctx_t* ctx);
void cubehash_nextBlock(cubehash_ctx_t* ctx, void* block);
void cubehash_lastBlock(cubehash_ctx_t* ctx, void* block, uint16_t length_b);
void cubehash_ctx2hash(void* dest, uint16_t length_b, cubehash_ctx_t* ctx);

void cubehash224_init(cubehash_ctx_t* ctx);
void cubehash224_ctx2hash(void* dest, cubehash_ctx_t* ctx);

void cubehash256_init(cubehash_ctx_t* ctx);
void cubehash256_ctx2hash(void* dest, cubehash_ctx_t* ctx);

void cubehash384_init(cubehash_ctx_t* ctx);
void cubehash384_ctx2hash(void* dest, cubehash_ctx_t* ctx);

void cubehash512_init(cubehash_ctx_t* ctx);
void cubehash512_ctx2hash(void* dest, cubehash_ctx_t* ctx);

#endif /* CUBEHASH_H_ */
