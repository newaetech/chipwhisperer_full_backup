/* md5.h */
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
/* 
 * File:	md5.h
 * Author:	Daniel Otte
 * Date: 	31.07.2006
 * License: GPL
 * Description: Implementation of the MD5 hash algorithm as described in RFC 1321
 * 
 */


#ifndef MD5_H_
#define MD5_H_

#include <stdint.h>


#define MD5_HASH_BITS  128
#define MD5_HASH_BYTES (MD5_HASH_BITS/8)
#define MD5_BLOCK_BITS 512
#define MD5_BLOCK_BYTES (MD5_BLOCK_BITS/8)


typedef struct md5_ctx_st {
	uint32_t a[4];
	uint32_t counter;
} md5_ctx_t;

typedef uint8_t md5_hash_t[MD5_HASH_BYTES];

 
void md5_init(md5_ctx_t *s);
void md5_nextBlock(md5_ctx_t *state, const void* block);
void md5_lastBlock(md5_ctx_t *state, const void* block, uint16_t length);
void md5_ctx2hash(md5_hash_t* dest, const md5_ctx_t* state);
void md5(md5_hash_t* dest, const void* msg, uint32_t length_b);

#endif /*MD5_H_*/
