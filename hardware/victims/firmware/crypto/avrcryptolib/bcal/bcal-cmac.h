/* bcal-cmac.h */
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

#ifndef BCALCMAC_H_
#define BCALCMAC_H_

#include <stdint.h>
#include "bcal-basic.h"
#include "blockcipher_descriptor.h"

typedef struct{
	bcdesc_t*    desc;
	bcgen_ctx_t  cctx;
	uint8_t*     accu;
	uint8_t*     k1;
	uint8_t*     k2;
	uint8_t*     lastblock;
	uint8_t      last_set;
	uint8_t      blocksize_B;
} bcal_cmac_ctx_t;

uint8_t bcal_cmac_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, bcal_cmac_ctx_t* ctx);
void bcal_cmac_free(bcal_cmac_ctx_t* ctx);
void bcal_cmac_nextBlock(bcal_cmac_ctx_t* ctx, const void* block);
void bcal_cmac_lastBlock(bcal_cmac_ctx_t* ctx, const void* block, uint16_t length_b);
void bcal_cmac_ctx2mac(void* dest, uint16_t length_b, const bcal_cmac_ctx_t* ctx);
void bcal_cmac(void* dest, uint16_t out_length_b, const void* block, uint32_t length_b, bcal_cmac_ctx_t* ctx);

#endif /* BCALCMAC_H_ */
