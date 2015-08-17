/* sha384.h */
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

#ifndef SHA384_H_
#define SHA384_H_


#include <stdint.h>
#include "sha2_large_common.h"

#define SHA384_HASH_BITS  384
#define SHA384_HASH_BYTES (SHA384_HASH_BITS/8)
#define SHA384_BLOCK_BITS 1024
#define SHA3842_BLOCK_BYTES (SHA384_BLOCK_BITS/8)

typedef sha2_large_common_ctx_t sha384_ctx_t;


void sha384_init(sha384_ctx_t *ctx);
void sha384_nextBlock (sha384_ctx_t* ctx, const void* block);
void sha384_lastBlock(sha384_ctx_t* ctx, const void* block, uint16_t length_b);
void sha384_ctx2hash(void* dest, const sha384_ctx_t* ctx);
void sha384(void* dest, const void* msg, uint32_t length_b);


#endif /* SHA384_H_ */
