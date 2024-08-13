/* salsa20.h */
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

#ifndef SALSA20_H_
#define SALSA20_H_

#include <stdint.h>

typedef struct{
	union{
		uint8_t   v8[64];
		uint64_t v64[ 8];
	} a;
	uint8_t buffer[64];
	uint8_t buffer_idx;
} salsa20_ctx_t;

void salsa20_hash(uint32_t* a);
void salsa_k32(uint32_t* dest, const uint32_t* k, const uint32_t* n);
void salsa_k16(uint32_t* dest, const uint32_t* k, const uint32_t* n);
void salsa20_genBlock256(void* dest, const void* k, const void* iv, uint64_t i);
void salsa20_genBlock128(void* dest, const void* k, const void* iv, uint64_t i);

void salsa20_init(void* key, uint16_t keylength_b, void* iv, salsa20_ctx_t* ctx);
uint8_t salsa20_gen(salsa20_ctx_t* ctx);

#endif /* SALSA20_H_ */
