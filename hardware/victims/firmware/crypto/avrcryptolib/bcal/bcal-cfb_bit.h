/* bcal-cfb_bit.h */
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


#ifndef BCALCFB_BIT_H_
#define BCALCFB_BIT_H_

#include <stdint.h>
#include "bcal-basic.h"
#include "blockcipher_descriptor.h"


typedef struct{
	bcdesc_t*    desc;
	bcgen_ctx_t  cctx;
	uint8_t*     in_block;
	uint8_t      blocksize_B;
	uint16_t     size_b;
} bcal_cfb_b_ctx_t;


uint8_t bcal_cfb_b_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, uint16_t size_b, bcal_cfb_b_ctx_t* ctx);
void bcal_cfb_b_free(bcal_cfb_b_ctx_t* ctx);
void bcal_cfb_b_loadIV(const void* iv, bcal_cfb_b_ctx_t* ctx);
void bcal_cfb_b_encNext(void* block, uint8_t offset, bcal_cfb_b_ctx_t* ctx);
void bcal_cfb_b_decNext(void* block, uint8_t offset, bcal_cfb_b_ctx_t* ctx);
void bcal_cfb_b_encMsg(const void* iv, void* msg, uint8_t offset, uint32_t msg_blocks, bcal_cfb_b_ctx_t* ctx);
void bcal_cfb_b_decMsg(const void* iv, void* msg, uint8_t offset, uint32_t msg_blocks, bcal_cfb_b_ctx_t* ctx);


#endif /* BCALCFB_BIT_H_ */
