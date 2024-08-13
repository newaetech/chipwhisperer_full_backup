/* aes_keyschedule.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
/**
 * \file     aes_keyschedule.h
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2008-12-30
 * \license  GPLv3 or later
 * \ingroup  AES
 */


#ifndef AES_KEYSCHEDULE_H_
#define AES_KEYSCHEDULE_H_

#include "aes_types.h"
/**
 * \brief initialize the keyschedule
 *
 * This function computes the keyschedule from a given key with a given length
 * and stores it in the context variable
 * \param key       pointer to the key material
 * \param keysize_b length of the key in bits (valid are 128, 192 and 256)
 * \param ctx       pointer to the context where the keyschedule should be stored
 */
void aes_init(const void* key, uint16_t keysize_b, aes_genctx_t* ctx);

/**
 * \brief initialize the keyschedule for 128 bit key
 *
 * This function computes the keyschedule from a given 128 bit key
 * and stores it in the context variable
 * \param key       pointer to the key material
 * \param ctx       pointer to the context where the keyschedule should be stored
 */
void aes128_init(const void* key, aes128_ctx_t* ctx);

/**
 * \brief initialize the keyschedule for 192 bit key
 *
 * This function computes the keyschedule from a given 192 bit key
 * and stores it in the context variable
 * \param key       pointer to the key material
 * \param ctx       pointer to the context where the keyschedule should be stored
 */
void aes192_init(const void* key, aes192_ctx_t* ctx);

/**
 * \brief initialize the keyschedule for 256 bit key
 *
 * This function computes the keyschedule from a given 256 bit key
 * and stores it in the context variable
 * \param key       pointer to the key material
 * \param ctx       pointer to the context where the keyschedule should be stored
 */
void aes256_init(const void* key, aes256_ctx_t* ctx);

#endif /* AES_KEYSCHEDULE_H_ */

