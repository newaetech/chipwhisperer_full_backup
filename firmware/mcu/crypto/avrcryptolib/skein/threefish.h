/* threefish.c */
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
/*
 * \file    threefish.h
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-12
 * \license GPLv3 or later
 * \brief   Implementation of the Threefish block cipher
 * \ingroup Threefish
 */

#ifndef THREEFISH_H_
#define THREEFISH_H_

#include <stdint.h>

#define THREEFISH256_BLOCKSIZE 256
#define THREEFISH256_BLOCKSIZE_B ((THREEFISH256_BLOCKSIZE+7)/8)
#define THREEFISH512_BLOCKSIZE 512
#define THREEFISH512_BLOCKSIZE_B ((THREEFISH512_BLOCKSIZE+7)/8)
#define THREEFISH1024_BLOCKSIZE 1024
#define THREEFISH1024_BLOCKSIZE_B ((THREEFISH1024_BLOCKSIZE+7)/8)

/** \typedef threefish256_ctx_t
 * \brief holds key data for Threefish-256
 *  
 * A variable of this type may hold the key data for Threefish-256 encryption
 * or decryption..
 */
typedef struct{
	uint64_t k[5];
	uint64_t t[3];
} threefish256_ctx_t;

/** \typedef threefish512_ctx_t
 * \brief holds key data for Threefish-512
 *  
 * A variable of this type may hold the key data for Threefish-512 encryption
 * or decryption..
 */
typedef struct{
	uint64_t k[9];
	uint64_t t[3];
} threefish512_ctx_t;

/** \typedef threefish1024_ctx_t
 * \brief holds key data for Threefish-1024
 *  
 * A variable of this type may hold the key data for Threefish-1024 encryption
 * or decryption..
 */
typedef struct{
	uint64_t k[17];
	uint64_t t[3];
} threefish1024_ctx_t;


void threefish_mix(void* data, uint8_t rot);
void threefish_invmix(void* data, uint8_t rot);

void threefish256_init(const void* key, const void* tweak, threefish256_ctx_t* ctx);
void threefish512_init(const void* key, const void* tweak, threefish512_ctx_t* ctx);
void threefish1024_init(const void* key, const void* tweak, threefish1024_ctx_t* ctx);

void threefish256_enc(void* data, const threefish256_ctx_t* ctx);
void threefish512_enc(void* data, const threefish512_ctx_t* ctx);
void threefish1024_enc(void* data, const threefish1024_ctx_t* ctx);

void threefish256_dec(void* data, const threefish256_ctx_t* ctx);
void threefish512_dec(void* data, const threefish512_ctx_t* ctx);
void threefish1024_dec(void* data, const threefish1024_ctx_t* ctx);

#endif /* THREEFISH_H_ */
