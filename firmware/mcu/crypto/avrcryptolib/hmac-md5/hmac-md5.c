/* hmac-md5.c */
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
#include "md5.h"
#include "hmac-md5.h"

#define IPAD 0x36
#define OPAD 0x5C

#ifndef HMAC_SHORTONLY

void hmac_md5_init(hmac_md5_ctx_t *s, void* key, uint16_t keylength_b){
	uint8_t buffer[MD5_BLOCK_BYTES];
	uint8_t i;
	
	memset(buffer, 0, MD5_BLOCK_BYTES);
	if (keylength_b > MD5_BLOCK_BITS){
		md5((void*)buffer, key, keylength_b);
	} else {
		memcpy(buffer, key, (keylength_b+7)/8);
	}
	
	for (i=0; i<MD5_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD;
	}
	md5_init(&(s->a));
	md5_nextBlock(&(s->a), buffer);
	
	for (i=0; i<MD5_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD^OPAD;
	}
	md5_init(&(s->b));
	md5_nextBlock(&(s->b), buffer);
	
#if defined SECURE_WIPE_BUFFER
	memset(buffer, 0, MD5_BLOCK_BYTES);
#endif
}

void hmac_md5_nextBlock(hmac_md5_ctx_t *s, const void* block){
	md5_nextBlock(&(s->a), block);
}

void hmac_md5_lastBlock(hmac_md5_ctx_t *s, const void* block, uint16_t length_b){
	md5_lastBlock(&(s->a), block, length_b);
}

void hmac_md5_final(void* dest, hmac_md5_ctx_t *s){
	md5_ctx2hash((md5_hash_t*)dest, &(s->a));
	md5_lastBlock(&(s->b), dest, MD5_HASH_BITS);
	md5_ctx2hash((md5_hash_t*)dest, &(s->b));
}

#endif

/*
void hmac_md5_nextBlock()
void hmac_md5_lastBlock()
*/

/*
 * keylength in bits!
 * message length in bits!
 */
void hmac_md5(void* dest, void* key, uint16_t keylength_b, void* msg, uint32_t msglength_b){ /* a one-shot*/
	md5_ctx_t s;
	uint8_t i;
	uint8_t buffer[MD5_BLOCK_BYTES];
	
	memset(buffer, 0, MD5_BLOCK_BYTES);
	
	/* if key is larger than a block we have to hash it*/
	if (keylength_b > MD5_BLOCK_BITS){
		md5((void*)buffer, key, keylength_b);
	} else {
		memcpy(buffer, key, (keylength_b+7)/8);
	}
	
	for (i=0; i<MD5_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD;
	}
	md5_init(&s);
	md5_nextBlock(&s, buffer);
	while (msglength_b >= MD5_BLOCK_BITS){
		md5_nextBlock(&s, msg);
		msg = (uint8_t*)msg + MD5_BLOCK_BYTES;
		msglength_b -=  MD5_BLOCK_BITS;
	}
	md5_lastBlock(&s, msg, msglength_b);
	/* since buffer still contains key xor ipad we can do ... */
	for (i=0; i<MD5_BLOCK_BYTES; ++i){
		buffer[i] ^= IPAD ^ OPAD;
	}
	md5_ctx2hash(dest, &s); /* save inner hash temporary to dest */
	md5_init(&s);
	md5_nextBlock(&s, buffer);
	md5_lastBlock(&s, dest, MD5_HASH_BITS);
	md5_ctx2hash(dest, &s);
}

