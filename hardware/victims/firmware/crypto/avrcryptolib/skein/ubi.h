/* ubi.h */
/*
    This file is part of the ARM-Crypto-Lib.
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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-12
 * \license GPLv3 or later
 * 
 */

#ifndef UBI_H_
#define UBI_H_

#include <stdint.h>

#define UBI_TYPE_KEY  0
#define UBI_TYPE_CFG  4
#define UBI_TYPE_PRS  8
#define UBI_TYPE_PK  12
#define UBI_TYPE_KDF 16
#define UBI_TYPE_NON 20
#define UBI_TYPE_MSG 48
#define UBI_TYPE_OUT 63

#define UBI256_BLOCKSIZE 256
#define UBI256_BLOCKSIZE_B ((UBI256_BLOCKSIZE+7)/8)

#define UBI512_BLOCKSIZE 512
#define UBI512_BLOCKSIZE_B ((UBI512_BLOCKSIZE+7)/8)

#define UBI1024_BLOCKSIZE 1024
#define UBI1024_BLOCKSIZE_B ((UBI1024_BLOCKSIZE+7)/8)


typedef struct{
	union {
		uint8_t v8[16];
		uint16_t v16[8];
		uint32_t v32[4];
		uint64_t v64[2];
	} tweak;
	uint8_t g[32];
}ubi256_ctx_t;

typedef struct{
	union {
		uint8_t v8[16];
		uint16_t v16[8];
		uint32_t v32[4];
		uint64_t v64[2];
	} tweak;
	uint8_t g[64];
}ubi512_ctx_t;

typedef struct{
	union {
		uint8_t v8[16];
		uint16_t v16[8];
		uint32_t v32[4];
		uint64_t v64[2];
	} tweak;
	uint8_t g[128];
}ubi1024_ctx_t;

void ubi256_init(ubi256_ctx_t* ctx, const void* g, uint8_t type);
void ubi256_nextBlock(ubi256_ctx_t* ctx, const void* block);
void ubi256_lastBlock(ubi256_ctx_t* ctx, const void* block, uint16_t length_b);
void ubi256_ctx2hash(void* dest, const ubi256_ctx_t* ctx);

void ubi512_init(ubi512_ctx_t* ctx, const void* g, uint8_t type);
void ubi512_nextBlock(ubi512_ctx_t* ctx, const void* block);
void ubi512_lastBlock(ubi512_ctx_t* ctx, const void* block, uint16_t length_b);
void ubi512_ctx2hash(void* dest, const ubi512_ctx_t* ctx);

void ubi1024_init(ubi1024_ctx_t* ctx, const void* g, uint8_t type);
void ubi1024_nextBlock(ubi1024_ctx_t* ctx, const void* block);
void ubi1024_lastBlock(ubi1024_ctx_t* ctx, const void* block, uint16_t length_b);
void ubi1024_ctx2hash(void* dest, const ubi1024_ctx_t* ctx);

typedef struct{
	char     schema[4];
	uint16_t version;
	uint16_t reserved1;
	uint64_t out_length;
	uint8_t  tree_leaf_size;
	uint8_t  tree_fan_out;
	uint8_t  tree_max_height;
	uint8_t  reserved2[13];
}skein_config_t;


#endif /* UBI_H_ */
