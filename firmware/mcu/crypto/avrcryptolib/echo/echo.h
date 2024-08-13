/* echo.h */
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

#ifndef ECHO_H_
#define ECHO_H_

#include <stdint.h>

#define ECHO_SMALL_BLOCKSIZE 1536
#define ECHO_SMALL_BLOCKSIZE_B ((ECHO_SMALL_BLOCKSIZE+7)/8)
#define ECHO_LARGE_BLOCKSIZE 1024
#define ECHO_LARGE_BLOCKSIZE_B ((ECHO_LARGE_BLOCKSIZE+7)/8)

#define ECHO224_BLOCKSIZE ECHO_SMALL_BLOCKSIZE
#define ECHO224_BLOCKSIZE_B ((ECHO224_BLOCKSIZE+7)/8)
#define ECHO256_BLOCKSIZE ECHO_SMALL_BLOCKSIZE
#define ECHO256_BLOCKSIZE_B ((ECHO256_BLOCKSIZE+7)/8)
#define ECHO384_BLOCKSIZE ECHO_LARGE_BLOCKSIZE
#define ECHO384_BLOCKSIZE_B ((ECHO384_BLOCKSIZE+7)/8)
#define ECHO512_BLOCKSIZE ECHO_LARGE_BLOCKSIZE
#define ECHO512_BLOCKSIZE_B ((ECHO512_BLOCKSIZE+7)/8)

typedef struct{
	uint8_t v[4*16];
	uint8_t salt[16];
	uint64_t counter;
	uint16_t id;
}echo_small_ctx_t;

typedef struct{
	uint8_t v[8*16];
	uint8_t salt[16];
	uint64_t counter;
	uint16_t id;
}echo_large_ctx_t;

void echo_small_nextBlock(echo_small_ctx_t* ctx, void* block);
void echo_small_lastBlock(echo_small_ctx_t* ctx, void* block, uint16_t length_b);
void echo_small_ctx2hash(void* dest, uint16_t length_b, echo_small_ctx_t* ctx);
void echo224_ctx2hash(void* dest, echo_small_ctx_t* ctx);
void echo256_ctx2hash(void* dest, echo_small_ctx_t* ctx);
void echo224_init(echo_small_ctx_t* ctx);
void echo256_init(echo_small_ctx_t* ctx);

void echo_large_nextBlock(echo_large_ctx_t* ctx, void* block);
void echo_large_lastBlock(echo_large_ctx_t* ctx, void* block, uint16_t length_b);
void echo_large_ctx2hash(void* dest, uint16_t length_b, echo_large_ctx_t* ctx);
void echo384_ctx2hash(void* dest, echo_large_ctx_t* ctx);
void echo512_ctx2hash(void* dest, echo_large_ctx_t* ctx);
void echo384_init(echo_large_ctx_t* ctx);
void echo512_init(echo_large_ctx_t* ctx);

#endif /* ECHO_H_ */
