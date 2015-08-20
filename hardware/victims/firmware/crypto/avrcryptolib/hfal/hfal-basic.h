/* hfal-basic.h */
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

#ifndef HFAL_BASIC_H_
#define HFAL_BASIC_H_

#include <avr/pgmspace.h>
#include "hashfunction_descriptor.h"

uint8_t hfal_hash_init(const hfdesc_t* hash_descriptor, hfgen_ctx_t* ctx);
void hfal_hash_nextBlock(hfgen_ctx_t* ctx, const void* block);
void hfal_hash_lastBlock(hfgen_ctx_t* ctx, const void* block, uint16_t length_b);
void hfal_hash_ctx2hash(void* dest, hfgen_ctx_t* ctx);
void hfal_hash_free(hfgen_ctx_t* ctx);
void hfal_hash_mem(const hfdesc_t* hash_descriptor, void* dest, const void* msg, uint32_t length_b);
uint16_t hfal_hash_getBlocksize(const hfdesc_t* hash_descriptor);
uint16_t hfal_hash_getHashsize(const hfdesc_t* hash_descriptor);
uint16_t hfal_hash_getCtxsize_B(const hfdesc_t* hash_descriptor);

#endif /* HFAL_BASIC_H_ */
