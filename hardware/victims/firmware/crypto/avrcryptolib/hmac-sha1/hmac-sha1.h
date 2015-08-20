/* hmac-sha1.h */
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
#ifndef HMACSHA1_H_
#define HMACSHA1_H_

#include "sha1.h"

#define HMAC_SHA1_BITS        SHA1_HASH_BITS
#define HMAC_SHA1_BYTES       SHA1_HASH_BYTES
#define HMAC_SHA1_BLOCK_BITS  SHA1_BLOCK_BITS
#define HMAC_SHA1_BLOCK_BYTES SHA1_BLOCK_BYTES

typedef struct{
	 sha1_ctx_t a, b;
} hmac_sha1_ctx_t;


void hmac_sha1_init(hmac_sha1_ctx_t *s, const void* key, uint16_t keylength_b);
void hmac_sha1_nextBlock(hmac_sha1_ctx_t *s, const void* block);
void hmac_sha1_lastBlock(hmac_sha1_ctx_t *s, const void* block, uint16_t length_b);
void hmac_sha1_final(void* dest, hmac_sha1_ctx_t *s);

void hmac_sha1(void* dest, const void* key, uint16_t keylength_b, const void* msg, uint32_t msglength_b);

#endif /*HMACSHA1_H_*/
