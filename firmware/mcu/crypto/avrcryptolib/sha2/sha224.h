/* sha224.h */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2011  Daniel Otte (daniel.otte@rub.de)

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
 * \file	sha224.h
 * \author  Daniel Otte 
 * \date    2011-10-11
 * \license	GPLv3 or later
 * 
 */

#ifndef SHA224_H_
#define SHA224_H_

#include <stdint.h>
#include "sha2_small_common.h"
/** \def SHA224_HASH_BITS
 * defines the size of a SHA-224 hash value in bits
 */

/** \def SHA224_HASH_BYTES
 * defines the size of a SHA-224 hash value in bytes
 */

/** \def SHA224_BLOCK_BITS
 * defines the size of a SHA-224 input block in bits
 */

/** \def SHA224_BLOCK_BYTES
 * defines the size of a SHA-224 input block in bytes
 */

#define SHA224_HASH_BITS  224
#define SHA224_HASH_BYTES (SHA224_HASH_BITS/8)
#define SHA224_BLOCK_BITS 512
#define SHA224_BLOCK_BYTES (SHA224_BLOCK_BITS/8)

/** \typedef sha224_ctx_t
 * \brief SHA-224 context type
 * 
 * A variable of this type may hold the state of a SHA-224 hashing process
 */
typedef sha2_small_common_ctx_t sha224_ctx_t;

/** \fn void sha224_init(sha224_ctx_t *state)
 * \brief initialize a SHA-224 context
 * 
 * This function sets a ::sha224_ctx_t to the initial values for hashing.
 * \param state pointer to the SHA-224 hashing context
 */
void sha224_init(sha224_ctx_t *state);

/** \fn void sha224_nextBlock (sha224_ctx_t* state, const void* block)
 * \brief update the context with a given block
 * 
 * This function updates the SHA-224 hash context by processing the given block
 * of fixed length.
 * \param state pointer to the SHA-224 hash context
 * \param block pointer to the block of fixed length (512 bit = 64 byte)
 */
void sha224_nextBlock (sha224_ctx_t* state, const void* block);

/** \fn void sha224_lastBlock(sha224_ctx_t* state, const void* block, uint16_t length_b)
 * \brief finalize the context with the given block 
 * 
 * This function finalizes the SHA-224 hash context by processing the given block
 * of variable length.
 * \param state pointer to the SHA-224 hash context
 * \param block pointer to the block of fixed length (512 bit = 64 byte)
 * \param length_b the length of the block in bits
 */
void sha224_lastBlock(sha224_ctx_t* state, const void* block, uint16_t length_b);

/** \fn void sha224_ctx2hash(sha224_hash_t* dest, const sha224_ctx_t* state)
 * \brief convert the hash state into the hash value
 * This function reads the context and writes the hash value to the destination
 * \param dest pointer to the location where the hash value should be written
 * \param state pointer to the SHA-224 hash context
 */
void sha224_ctx2hash(void* dest, const sha224_ctx_t* state);

/** \fn void sha224(sha224_hash_t* dest, const void* msg, uint32_t length_b)
 * \brief simple SHA-224 hashing function for direct hashing
 * 
 * This function automatically hashes a given message of arbitary length with
 * the SHA-224 hashing algorithm.
 * \param dest pointer to the location where the hash value is going to be written to
 * \param msg pointer to the message thats going to be hashed
 * \param length_b length of the message in bits
 */
void sha224(void* dest, const void* msg, uint32_t length_b);

#endif /*SHA224_H_*/
