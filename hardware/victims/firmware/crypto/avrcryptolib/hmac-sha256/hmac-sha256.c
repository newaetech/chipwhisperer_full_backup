/* hmac-sha256.c */
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
/**
 * 
 * implementation of HMAC as described in RFC2104
 * Author:      Daniel Otte
 * email:       daniel.otte@rub.de
 * License:     GPLv3 or later
 **/

/* 
 * hmac = hash ( k^opad , hash( k^ipad  , msg))
 */

#include <stdint.h>
#include <string.h>
#include "config.h"
#include "sha256.h"
#include "hmac-sha256.h"

#define IPAD 0x36
#define OPAD 0x5C

#ifndef HMAC_SHA256_SHORTONLY

void hmac_sha256_init(hmac_sha256_ctx_t *s, const void* key, uint16_t keylength_b){
	uint8_t buffer[HMAC_SHA256_BLOCK_BYTES];
	uint8_t i;
	
	memset(buffer, 0, HMAC_SHA256_BLOCK_BYTES);
	if (keylength_b > HMAC_SHA256_BLOCK_BITS){
		sha256((void*)buffer, key, keylength_b);
	} else {
		memcpy(buffer, key, (keylength_b+7)/8);
	}
	
	for (i=0; i<HMAC_SHA256_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD;
	}
	
	sha256_init(&(s->a));
	sha256_nextBlock(&(s->a), buffer);
	
	for (i=0; i<HMAC_SHA256_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD^OPAD;
	}
	sha256_init(&(s->b));
	sha256_nextBlock(&(s->b), buffer);
	
#if defined SECURE_WIPE_BUFFER
	memset(buffer, 0, SHA256_BLOCK_BYTES);
#endif
}

void hmac_sha256_nextBlock(hmac_sha256_ctx_t *s, const void* block){
	sha256_nextBlock(&(s->a), block);
}

void hmac_sha256_lastBlock(hmac_sha256_ctx_t *s, const void* block, uint16_t length_b){
/*	while(length_b>=SHA256_BLOCK_BITS){
		sha256_nextBlock(&(s->a), block);
		block = (uint8_t*)block + SHA256_BLOCK_BYTES;
		length_b -= SHA256_BLOCK_BITS;
	}
*/	sha256_lastBlock(&(s->a), block, length_b);
}

void hmac_sha256_final(void* dest, hmac_sha256_ctx_t *s){
	sha256_ctx2hash((sha256_hash_t*)dest, &(s->a));
	sha256_lastBlock(&(s->b), dest, SHA256_HASH_BITS);
	sha256_ctx2hash((sha256_hash_t*)dest, &(s->b));			
}

#endif

/*
 * keylength in bits!
 * message length in bits!
 */
void hmac_sha256(void* dest, const void* key, uint16_t keylength_b, const void* msg, uint32_t msglength_b){ /* a one-shot*/
	sha256_ctx_t s;
	uint8_t i;
	uint8_t buffer[HMAC_SHA256_BLOCK_BYTES];
	
	memset(buffer, 0, HMAC_SHA256_BLOCK_BYTES);
	
	/* if key is larger than a block we have to hash it*/
	if (keylength_b > SHA256_BLOCK_BITS){
		sha256((void*)buffer, key, keylength_b);
	} else {
		memcpy(buffer, key, (keylength_b+7)/8);
	}
	
	for (i=0; i<SHA256_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD;
	}
	sha256_init(&s);
	sha256_nextBlock(&s, buffer);
	while (msglength_b >= HMAC_SHA256_BLOCK_BITS){
		sha256_nextBlock(&s, msg);
		msg = (uint8_t*)msg + HMAC_SHA256_BLOCK_BYTES;
		msglength_b -=  HMAC_SHA256_BLOCK_BITS;
	}
	sha256_lastBlock(&s, msg, msglength_b);
	/* since buffer still contains key xor ipad we can do ... */
	for (i=0; i<HMAC_SHA256_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD ^ OPAD;
	}
	sha256_ctx2hash(dest, &s); /* save inner hash temporary to dest */
	sha256_init(&s);
	sha256_nextBlock(&s, buffer);
	sha256_lastBlock(&s, dest, SHA256_HASH_BITS);
	sha256_ctx2hash(dest, &s);
}
