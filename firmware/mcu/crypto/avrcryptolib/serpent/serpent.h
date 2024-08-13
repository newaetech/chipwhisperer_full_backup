/* serpent.h */
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
/** \file   serpent.h
 * \author  Daniel Otte
 * \license GPLv3
 * \brief a implementation of the serpent cipher for avr microcontrollers
 */

#ifndef SERPENT_H_
#define SERPENT_H_

#include <stdint.h>

typedef uint32_t serpent_subkey_t[4];

typedef struct serpent_ctx_st {
	serpent_subkey_t k[33];
}  serpent_ctx_t;

#define SERPENT_KEY128 128
#define SERPENT_KEY192 192
#define SERPENT_KEY256 256


/* key must be 256bit (32 byte) large! */
void serpent_init(const void* key, uint16_t keysize_b, serpent_ctx_t* ctx);
void serpent_enc(void* buffer, const serpent_ctx_t* ctx);
void serpent_dec(void* buffer, const serpent_ctx_t* ctx);


#endif /*SERPENT_H_*/
