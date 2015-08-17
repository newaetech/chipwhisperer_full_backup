/* hfal-hmac.h */
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

#ifndef HFAL_HMAC_H_
#define HFAL_HMAC_H_

#include <avr/pgmspace.h>
#include "hashfunction_descriptor.h"

typedef struct {
	hfdesc_t* desc;
	void*     ctx;
	void*     finctx;
} hfhmacgen_ctx_t;

uint8_t hfal_hmac_init(const hfdesc_t* hash_descriptor, hfhmacgen_ctx_t* ctx, const void* key, uint16_t keylength_b);
void hfal_hmac_nextBlock(hfhmacgen_ctx_t* ctx, const void* block);
void hfal_hmac_lastBlock(hfhmacgen_ctx_t* ctx, const void* block, uint16_t length_b);
void hfal_hmac_ctx2mac(void* dest, hfhmacgen_ctx_t* ctx);
void hfal_hmac_free(hfhmacgen_ctx_t* ctx);
void hfal_hmac_mem(const hfdesc_t* hash_descriptor, const void* key, uint16_t keylength_b, void* dest, const void* msg, uint32_t length_b);
uint16_t hfal_hmac_getBlocksize(const hfdesc_t* hash_descriptor);
uint16_t hfal_hmac_getMACsize(const hfdesc_t* hash_descriptor);

#endif /* HFAL_HMAC_H_ */
