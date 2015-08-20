/* keccak.h */
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

#ifndef KECCAK_H_
#define KECCAK_H_

#include <stdint.h>

#define KECCAK224_BLOCKSIZE 1152
#define KECCAK224_BLOCKSIZE_B (KECCAK224_BLOCKSIZE/8)
#define KECCAK256_BLOCKSIZE 1088
#define KECCAK256_BLOCKSIZE_B (KECCAK256_BLOCKSIZE/8)
#define KECCAK384_BLOCKSIZE  832
#define KECCAK384_BLOCKSIZE_B (KECCAK384_BLOCKSIZE/8)
#define KECCAK512_BLOCKSIZE  576
#define KECCAK512_BLOCKSIZE_B (KECCAK512_BLOCKSIZE/8)

typedef struct{
	uint64_t a[5][5];
	uint16_t r, c;
	uint8_t  d, bs;
} keccak_ctx_t;


void keccak_init(uint16_t r, uint16_t c, uint8_t d, keccak_ctx_t* ctx);
void keccak224_init(keccak_ctx_t* ctx);
void keccak256_init(keccak_ctx_t* ctx);
void keccak384_init(keccak_ctx_t* ctx);
void keccak512_init(keccak_ctx_t* ctx);

void keccak_nextBlock(keccak_ctx_t* ctx, const void* block);
void keccak_lastBlock(keccak_ctx_t* ctx, const void* block, uint16_t length_b);

void keccak_ctx2hash(void* dest, uint16_t length_b, keccak_ctx_t* ctx);
void keccak224_ctx2hash(void* dest, keccak_ctx_t* ctx);
void keccak256_ctx2hash(void* dest, keccak_ctx_t* ctx);
void keccak384_ctx2hash(void* dest, keccak_ctx_t* ctx);
void keccak512_ctx2hash(void* dest, keccak_ctx_t* ctx);

#endif /* KECCAK_H_ */
