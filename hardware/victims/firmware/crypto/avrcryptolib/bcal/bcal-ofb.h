/* bcal-ofb.h */
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


#ifndef BCALOFB_H_
#define BCALOFB_H_

#include <stdint.h>
#include "bcal-basic.h"
#include "blockcipher_descriptor.h"


typedef struct{
	bcdesc_t*    desc;
	bcgen_ctx_t  cctx;
	uint8_t*     in_block;
	uint8_t      blocksize_B;
} bcal_ofb_ctx_t;


uint8_t bcal_ofb_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, bcal_ofb_ctx_t* ctx);
void bcal_ofb_free(bcal_ofb_ctx_t* ctx);
void bcal_ofb_loadIV(const void* iv, bcal_ofb_ctx_t* ctx);
void bcal_ofb_encNext(void* block, bcal_ofb_ctx_t* ctx);
void bcal_ofb_decNext(void* block, bcal_ofb_ctx_t* ctx);
void bcal_ofb_encMsg(const void* iv, void* msg, uint32_t msg_len_b, bcal_ofb_ctx_t* ctx);
void bcal_ofb_decMsg(const void* iv, void* msg, uint32_t msg_len_b, bcal_ofb_ctx_t* ctx);


#endif /* BCALOFB_H_ */
