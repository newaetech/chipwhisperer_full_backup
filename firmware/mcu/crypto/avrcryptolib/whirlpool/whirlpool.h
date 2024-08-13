/* wirlpool.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2011 Daniel Otte (daniel.otte@rub.de)

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

#ifndef WIRLPOOL_H_
#define WIRLPOOL_H_

#include <stdint.h>

typedef struct {
	uint8_t s[8][8];
	uint32_t blocks;
} whirlpool_ctx_t;


void whirlpool_init(whirlpool_ctx_t* ctx);
void whirlpool_nextBlock(whirlpool_ctx_t* ctx,const void* block);
void whirlpool_lastBlock(whirlpool_ctx_t* ctx, const void* block, uint16_t length_b);
void whirlpool_ctx2hash(void* dest, const whirlpool_ctx_t* ctx);

#endif /* WIRLPOOL_H_ */
