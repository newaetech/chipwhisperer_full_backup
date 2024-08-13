/* rabbit.h */
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

#ifndef RABBIT_H_
#define RABBIT_H_

#include <stdint.h>

typedef struct {
	uint32_t x[8];
	uint32_t c[8];
	uint8_t  buffer[16];
	uint8_t  carry;
	uint8_t  buffer_idx;
} rabbit_ctx_t;

void dump_ctx(rabbit_ctx_t* ctx);

uint8_t rabbit_gen(rabbit_ctx_t* ctx);
void rabbit_init(const void* key, uint16_t keysize_b,
                  const void* iv,
                  rabbit_ctx_t* ctx);


#endif /* RABBIT_H_ */
