/* bcal-basic.h */
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

#ifndef BCAL_BASIC_H_
#define BCAL_BASIC_H_

#include <stdlib.h>
#include <stdint.h>
#include "blockcipher_descriptor.h"
#include "keysize_descriptor.h"
#include <avr/pgmspace.h>

uint8_t bcal_cipher_init(const bcdesc_t* cipher_descriptor,
                         const void* key, uint16_t keysize_b, bcgen_ctx_t* ctx);
void bcal_cipher_free(bcgen_ctx_t* ctx);
void bcal_cipher_enc(void* block, const bcgen_ctx_t* ctx);
void bcal_cipher_dec(void* block, const bcgen_ctx_t* ctx);
uint16_t bcal_cipher_getBlocksize_b(const bcdesc_t* desc);
PGM_VOID_P bcal_cipher_getKeysizeDesc(const bcdesc_t* desc);
#endif /* BCAL_BASIC_H_ */
