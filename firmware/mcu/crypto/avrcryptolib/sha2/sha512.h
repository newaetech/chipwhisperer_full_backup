/* sha512.h */
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

#ifndef SHA512_H_
#define SHA512_H_

#include <stdint.h>
#include "sha2_large_common.h"

#define SHA512_HASH_BITS  512
#define SHA512_HASH_BYTES (SHA512_HASH_BITS/8)
#define SHA512_BLOCK_BITS 1024
#define SHA5122_BLOCK_BYTES (SHA512_BLOCK_BITS/8)

typedef sha2_large_common_ctx_t sha512_ctx_t;


void sha512_init(sha512_ctx_t *ctx);
void sha512_nextBlock (sha512_ctx_t* ctx, const void* block);
void sha512_lastBlock(sha512_ctx_t* ctx, const void* block, uint16_t length_b);
void sha512_ctx2hash(void* dest, const sha512_ctx_t* ctx);
void sha512(void* dest, const void* msg, uint32_t length_b);


#endif /* SHA512_H_ */
