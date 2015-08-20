/* present128.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
#ifndef PRESENT128_H_
#define PRESENT128_H_

#include <stdint.h>

typedef struct __attribute__((packed)) present128_ctx_st {
	uint8_t fwd_key[16];
	uint8_t rev_key[16];
} present128_ctx_t;


void present128_init(const uint8_t* key, uint8_t keysize_b, present128_ctx_t* ctx);
void present128_enc(void* buffer, present128_ctx_t* ctx);
void present128_dec(void* buffer, present128_ctx_t* ctx);


#endif /*PRESENT128_H_*/
