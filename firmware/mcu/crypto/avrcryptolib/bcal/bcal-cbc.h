/* bcal-cbc.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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

#ifndef BCALCBC_H_
#define BCALCBC_H_

#include <stdint.h>
#include "blockcipher_descriptor.h"
#include "bcal-basic.h"

typedef struct{
	bcdesc_t*    desc;
	bcgen_ctx_t  cctx;
	uint8_t*     prev_block;
	uint8_t      blocksize_B;
} bcal_cbc_ctx_t;

uint8_t bcal_cbc_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, bcal_cbc_ctx_t* ctx);
void bcal_cbc_free(bcal_cbc_ctx_t* ctx);
void bcal_cbc_loadIV(const void* iv, bcal_cbc_ctx_t* ctx);
void bcal_cbc_encNext(void* block, bcal_cbc_ctx_t* ctx);
void bcal_cbc_decNext(void* block, bcal_cbc_ctx_t* ctx);
void bcal_cbc_decRand(void* block, const void* prev_block, bcal_cbc_ctx_t* ctx);
void bcal_cbc_encMsg(const void* iv, void* msg, uint16_t msg_blocks, bcal_cbc_ctx_t* ctx);
void bcal_cbc_decMsg(const void* iv, void* msg, uint16_t msg_blocks, bcal_cbc_ctx_t* ctx);


#endif /* BCALCBC_H_ */
