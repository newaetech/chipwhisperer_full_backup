/* shacal2_enc.h */
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
#ifndef SHACAL2_ENC_H_
#define SHACAL2_ENC_H_

#include <stdint.h>
#include "sha256.h"

#define SHACAL2_BLOCKSIZE  SHA256_HASH_BITS
#define SHACAL2_BLOCKSIZE_B ((SHACAL2_BLOCKSIZE+7)/8)
#define SHACAL2_KEYSIZE SHA256_BLOCK_BITS
#define SHACAL2_KEYSIZE_B ((SHACAL2_KEYSIZE+7)/8)

void shacal2_enc(void* buffer, void* key, uint16_t keysize_b);


#endif /*SHACAL2_ENC_H_*/
