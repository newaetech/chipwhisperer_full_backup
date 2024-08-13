/* hmac-md5.h */
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
#ifndef HMACMD5_H_
#define HMACMD5_H_

#include "md5.h"

#define HMAC_MD5_BITS        MD5_HASH_BITS
#define HMAC_MD5_BYTES       MD5_HASH_BYTES
#define HMAC_MD5_BLOCK_BITS  MD5_BLOCK_BITS
#define HMAC_MD5_BLOCK_BYTES MD5_BLOCK_BYTES

typedef struct{
	md5_ctx_t a,b;
} hmac_md5_ctx_t;


void hmac_md5_init(hmac_md5_ctx_t *s, void* key, uint16_t keylength_b);
void hmac_md5_nextBlock(hmac_md5_ctx_t *s, const void* block);
void hmac_md5_lastBlock(hmac_md5_ctx_t *s, const void* block, uint16_t length_b);
void hmac_md5_final(void* dest, hmac_md5_ctx_t *s);

void hmac_md5(void* dest, void* key, uint16_t keylength_b, void* msg, uint32_t msglength_b);


#endif /*HMACMD5_H_*/
