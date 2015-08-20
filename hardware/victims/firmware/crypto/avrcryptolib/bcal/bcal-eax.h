/* bcal-eax.h */
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

#ifndef BCALEAX_H_
#define BCALEAX_H_

#include <stdint.h>
#include <string.h>
#include "bcal-basic.h"
#include "blockcipher_descriptor.h"
#include "bcal-cmac.h"
#include "bcal-ctr.h"

typedef struct{
	uint8_t*        nonce;
	bcal_cmac_ctx_t ntag;
	bcal_cmac_ctx_t ctag;
	bcal_cmac_ctx_t htag;
	bcal_ctr_ctx_t  cipher;
	uint8_t         blocksize_B;
	uint8_t         header_set;
} bcal_eax_ctx_t;

uint8_t bcal_eax_init(const bcdesc_t* desc, const void* key, uint16_t keysize_b, bcal_eax_ctx_t* ctx);
void bcal_eax_free(bcal_eax_ctx_t* ctx);
void bcal_eax_loadNonce(const void* nonce, uint16_t length_b, bcal_eax_ctx_t* ctx);
void bcal_eax_addNextHeader(const void* header, bcal_eax_ctx_t* ctx);
void bcal_eax_addLastHeader(const void* header, uint16_t length_b, bcal_eax_ctx_t* ctx);
void bcal_eax_encNextBlock(void* block, bcal_eax_ctx_t* ctx);
void bcal_eax_encLastBlock(void* block, uint16_t length_b, bcal_eax_ctx_t* ctx);
void bcal_eax_decNextBlock(void* block, bcal_eax_ctx_t* ctx);
void bcal_eax_decLastBlock(void* block, uint16_t length_b, bcal_eax_ctx_t* ctx);
void bcal_eax_ctx2tag(void* dest, uint16_t length_b, bcal_eax_ctx_t* ctx);
//void bcal_eax(void* dest, uint16_t out_length_b, const void* block, uint32_t length_b, bcal_eax_ctx_t* ctx);

#endif /* BCALEAX_H_ */

