/* grain.h */
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

/** \file grain.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \license GPLv3 or later
 * \brief implementation of the Grain streamcipher
 */

#ifndef GRAIN_H_
#define GRAIN_H_


#include <stdint.h>

typedef struct gain_ctx_st{
	uint8_t lfsr[10];
	uint8_t nfsr[10];
} grain_ctx_t;

uint8_t grain_getbyte(grain_ctx_t* ctx);
uint8_t grain_enc(grain_ctx_t* ctx);
void grain_init(const void* key, const void* iv, grain_ctx_t* ctx);

#endif /*GRAIN_H_*/
