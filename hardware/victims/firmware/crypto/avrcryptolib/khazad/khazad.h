/* khazad.h */
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

#ifndef KHAZAD_H_
#define KHAZAD_H_

#include <stdint.h>

typedef struct {
	uint8_t k[9][8];
}khazad_ctx_t;

void khazad_enc(void* buffer, const khazad_ctx_t* ctx);
void khazad_dec(void* buffer, const khazad_ctx_t* ctx);
void khazad_init(const void* key, khazad_ctx_t* ctx);
uint8_t khazad_sbox(uint8_t);

#endif /* KHAZAD_H_ */
