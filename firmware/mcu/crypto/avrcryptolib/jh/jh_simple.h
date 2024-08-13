/* jh_simple.h */
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

#ifndef JH_SIMPLE_H_
#define JH_SIMPLE_H_

#include <stdint.h>

#define JH224_BLOCKSIZE 512
#define JH224_BLOCKSIZE_B ((JH224_BLOCKSIZE+7)/8)
#define JH256_BLOCKSIZE 512
#define JH256_BLOCKSIZE_B ((JH256_BLOCKSIZE+7)/8)
#define JH384_BLOCKSIZE 512
#define JH384_BLOCKSIZE_B ((JH284_BLOCKSIZE+7)/8)
#define JH512_BLOCKSIZE 512
#define JH512_BLOCKSIZE_B ((JH512_BLOCKSIZE+7)/8)

typedef struct {
	uint8_t a[128];
	uint32_t block_hashed;
} jh_ctx_t;


void jh_init(uint16_t hashlen_b, jh_ctx_t* ctx);
void jh_nextBlock(jh_ctx_t* ctx, void* block);
void jh_lastBlock(jh_ctx_t* ctx, void* block, uint16_t length_b);
void jh_ctx2hash(void* dest, uint16_t length_b, jh_ctx_t* ctx);

void jh224_init(jh_ctx_t* ctx);
void jh224_ctx2hash(void* dest, jh_ctx_t* ctx);

void jh256_init(jh_ctx_t* ctx);
void jh256_ctx2hash(void* dest, jh_ctx_t* ctx);

void jh384_init(jh_ctx_t* ctx);
void jh384_ctx2hash(void* dest, jh_ctx_t* ctx);

void jh512_init(jh_ctx_t* ctx);
void jh512_ctx2hash(void* dest, jh_ctx_t* ctx);


#endif /* JH_SIMPLE_H_ */
