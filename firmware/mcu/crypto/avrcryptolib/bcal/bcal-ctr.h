/* bcal-ctr.h */
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

#ifndef BCALCTR_H_
#define BCALCTR_H_

#include <stdint.h>
#include "bcal-basic.h"
#include "blockcipher_descriptor.h"

typedef void(*inc_fp_t)(void* block, uint8_t size_B);

typedef struct{
	bcdesc_t*    desc;
	bcgen_ctx_t  cctx;
	uint8_t*     in_block;
	inc_fp_t     inc_func;
	uint8_t      blocksize_B;
} bcal_ctr_ctx_t;

uint8_t bcal_ctr_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, inc_fp_t inc_func, bcal_ctr_ctx_t* ctx);
void bcal_ctr_free(bcal_ctr_ctx_t* ctx);
void bcal_ctr_loadIV(const void* iv, bcal_ctr_ctx_t* ctx);
void bcal_ctr_encNext(void* block, bcal_ctr_ctx_t* ctx);
void bcal_ctr_decNext(void* block, bcal_ctr_ctx_t* ctx);
void bcal_ctr_encMsg(const void* iv, void* msg, uint32_t msg_len_b, bcal_ctr_ctx_t* ctx);
void bcal_ctr_decMsg(const void* iv, void* msg, uint32_t msg_len_b, bcal_ctr_ctx_t* ctx);

#endif /* BCALCTR_H_ */
