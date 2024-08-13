/* sha256.h */
/*
    This file is part of the ARM-Crypto-Lib.
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
 * \file	sha256.h
 * \author  Daniel Otte 
 * \date    2006-05-16
 * \license	GPLv3 or later
 * 
 */

#ifndef SHA256_H_
#define SHA256_H_

#include <stdint.h>
#include "sha2_small_common.h"
/** \def SHA256_HASH_BITS
 * defines the size of a SHA-256 hash value in bits
 */

/** \def SHA256_HASH_BYTES
 * defines the size of a SHA-256 hash value in bytes
 */

/** \def SHA256_BLOCK_BITS
 * defines the size of a SHA-256 input block in bits
 */

/** \def SHA256_BLOCK_BYTES
 * defines the size of a SHA-256 input block in bytes
 */

#define SHA256_HASH_BITS  256
#define SHA256_HASH_BYTES (SHA256_HASH_BITS/8)
#define SHA256_BLOCK_BITS 512
#define SHA256_BLOCK_BYTES (SHA256_BLOCK_BITS/8)

/** \typedef sha256_ctx_t
 * \brief SHA-256 context type
 * 
 * A variable of this type may hold the state of a SHA-256 hashing process
 */
typedef sha2_small_common_ctx_t sha256_ctx_t;

/** \fn void sha256_init(sha256_ctx_t *state)
 * \brief initialize a SHA-256 context
 * 
 * This function sets a ::sha256_ctx_t to the initial values for hashing.
 * \param state pointer to the SHA-256 hashing context
 */
void sha256_init(sha256_ctx_t *state);

/** \fn void sha256_nextBlock (sha256_ctx_t* state, const void* block)
 * \brief update the context with a given block
 * 
 * This function updates the SHA-256 hash context by processing the given block
 * of fixed length.
 * \param state pointer to the SHA-256 hash context
 * \param block pointer to the block of fixed length (512 bit = 64 byte)
 */
void sha256_nextBlock (sha256_ctx_t* state, const void* block);

/** \fn void sha256_lastBlock(sha256_ctx_t* state, const void* block, uint16_t length_b)
 * \brief finalize the context with the given block 
 * 
 * This function finalizes the SHA-256 hash context by processing the given block
 * of variable length.
 * \param state pointer to the SHA-256 hash context
 * \param block pointer to the block of fixed length (512 bit = 64 byte)
 * \param length_b the length of the block in bits
 */
void sha256_lastBlock(sha256_ctx_t* state, const void* block, uint16_t length_b);

/** \fn void sha256_ctx2hash(sha256_hash_t* dest, const sha256_ctx_t* state)
 * \brief convert the hash state into the hash value
 * This function reads the context and writes the hash value to the destination
 * \param dest pointer to the location where the hash value should be written
 * \param state pointer to the SHA-256 hash context
 */
void sha256_ctx2hash(void* dest, const sha256_ctx_t* state);

/** \fn void sha256(sha256_hash_t* dest, const void* msg, uint32_t length_b)
 * \brief simple SHA-256 hashing function for direct hashing
 * 
 * This function automatically hashes a given message of arbitary length with
 * the SHA-256 hashing algorithm.
 * \param dest pointer to the location where the hash value is going to be written to
 * \param msg pointer to the message thats going to be hashed
 * \param length_b length of the message in bits
 */
void sha256(void* dest, const void* msg, uint32_t length_b);

#endif /*SHA256_H_*/
