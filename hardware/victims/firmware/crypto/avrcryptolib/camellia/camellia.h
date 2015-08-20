/* camellia.h */
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
#ifndef CAMELLIA_H_
#define CAMELLIA_H_

#include <stdint.h>

typedef struct camellia128_ctx_s{
	uint64_t klr; 
	uint64_t kll;
	uint64_t kar;
	uint64_t kal;
}camellia128_ctx_t;


void camellia128_init(const void* key, camellia128_ctx_t* s);
void camellia128_enc(void* block, const camellia128_ctx_t* s);
void camellia128_dec(void* block, const camellia128_ctx_t* s);


#endif /*CAMELLIA_H_*/
