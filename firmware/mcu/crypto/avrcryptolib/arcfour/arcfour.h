/* arcfour.h */
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
/*
 * File:	arcfour.h
 * Author:	Daniel Otte
 * Date: 	2006-06-07
 * License: GPLv3+
 * Description: Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm.
 */

/**
 * \file	arcfour.h
 * \author	Daniel Otte
 * \date 	2006-06-07
 * \license GPLv3+
 * \brief Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm.
 *
 * This header file defines the interface of the ARCFOUR cipher implementation.
 *
 * This implementation aims to be compatible with the ARCFOUR description
 * available at
 * http://www.mozilla.org/projects/security/pki/nss/draft-kaukonen-cipher-arcfour-03.txt
 */


#ifndef ARCFOUR_H_
#define ARCFOUR_H_

#include <stdint.h>

/** \typedef arcfour_ctx_t
 * \brief type for arcfour context
 *
 * A variable of this type may contain a complete ARCFOUR context.
 * The context is used to store the state of the cipher and gets
 * created by the arcfour_init(arcfour_ctx_t *c, uint8_t *key, uint8_t length_B)
 * function. The context is of the fixed size of 258 bytes
 */

/** \struct arcfour_ctx_st
 * \brief base for ::arcfour_ctx_t
 *
 * The struct holds the two indices and the S-Box
 */
typedef struct arcfour_ctx_st {
	uint8_t i,j;
	uint8_t s[256];
} arcfour_ctx_t;


/** \fn void arcfour_init(const void *key, uint8_t length_B, arcfour_ctx_t *ctx)
 * \brief setup a context with a key
 *
 * This function sets up a ::arcfour_ctx_t context using
 * the supplied key of the given length.
 * \param ctx pointer to the context
 * \param key pointer to the key
 * \param length_b length of the key in bits (between 8 and 2048)
 */

void arcfour_init(const void *key, uint16_t length_b, arcfour_ctx_t *ctx);

/** \fn uint8_t arcfour_gen(arcfour_ctx_t *ctx)
 * \brief generates a byte of keystream
 *
 * This function generates the next byte of keystream
 * from the supplied ::arcfour_ctx_t context which is updated accordingly
 *
 * \param ctx pointer to the context
 * \return byte of keystream
 */

uint8_t arcfour_gen(arcfour_ctx_t *ctx);

#endif
