/* shabal.h */
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
 * \file    shabal.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-17
 * \license GPLv3 or later
 * 
 */

#ifndef SHABAL_H_
#define SHABAL_H_

#include <stdint.h>

#define SHABAL_BLOCKSIZE 512
#define SHABAL_BLOCKSIZE_B ((SHABAL_BLOCKSIZE+7)/8)

#define SHABAL_R 12
#define SHABAL_P  3

typedef struct{
	union{
		uint64_t w64;
		uint32_t w32[2];
	} w; /* the counter */
	uint32_t *b;
	uint32_t *c;
	uint32_t a[SHABAL_R];
	uint32_t b_buffer[16];
	uint32_t c_buffer[16];
}shabal_ctx_t;


void shabal192_init(shabal_ctx_t* ctx);
void shabal224_init(shabal_ctx_t* ctx);
void shabal256_init(shabal_ctx_t* ctx);
void shabal384_init(shabal_ctx_t* ctx);
void shabal512_init(shabal_ctx_t* ctx);

void shabal_nextBlock(shabal_ctx_t* ctx, const void* block);
void shabal_lastBlock(shabal_ctx_t* ctx, const void* block, uint16_t length_b);

void shabal192_ctx2hash(void* dest, const shabal_ctx_t* ctx);
void shabal224_ctx2hash(void* dest, const shabal_ctx_t* ctx);
void shabal256_ctx2hash(void* dest, const shabal_ctx_t* ctx);
void shabal384_ctx2hash(void* dest, const shabal_ctx_t* ctx);
void shabal512_ctx2hash(void* dest, const shabal_ctx_t* ctx);

void shabal192(void* dest, void* msg, uint32_t length_b);
void shabal224(void* dest, void* msg, uint32_t length_b);
void shabal256(void* dest, void* msg, uint32_t length_b);
void shabal384(void* dest, void* msg, uint32_t length_b);
void shabal512(void* dest, void* msg, uint32_t length_b);

void shabal_ctx2hash(void* dest, const shabal_ctx_t* ctx, uint16_t outlength_b);

#endif /* SHABAL_H_ */
