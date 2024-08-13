/* sha256.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2010  Daniel Otte (daniel.otte@rub.de)

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
 * \file		sha256.c
 * \author		Daniel Otte
 * \date		16.05.2006
 *
 * \par License:
 * 	GPL
 *
 * \brief SHA-256 implementation.
 *
 *
 */

#include <stdint.h>
#include <string.h> /* for memcpy, memmove, memset */
#include <avr/pgmspace.h>
#include "sha2_small_common.h"
#include "sha256.h"

#define LITTLE_ENDIAN

#if defined LITTLE_ENDIAN
#elif defined BIG_ENDIAN
#else
	#error specify endianess!!!
#endif


/*************************************************************************/

const
uint32_t sha256_init_vector[] PROGMEM = {
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };


/*************************************************************************/

/**
 * \brief \c sh256_init initialises a sha256 context for hashing.
 * \c sh256_init c initialises the given sha256 context for hashing
 * @param state pointer to a sha256 context
 * @return none
 */
void sha256_init(sha256_ctx_t *state){
	state->length=0;
	memcpy_P(state->h, sha256_init_vector, 8*4);
}

/*************************************************************************/
void sha256_nextBlock (sha256_ctx_t *state, const void* block){
	sha2_small_common_nextBlock(state, block);
}

/*************************************************************************/
void sha256_lastBlock (sha256_ctx_t *state, const void* block, uint16_t length_b){
	sha2_small_common_lastBlock(state, block, length_b);
}
/*************************************************************************/

/**
 * \brief function to process the last block being hashed
 * @param state Pointer to the context in which this block should be processed.
 * @param block Pointer to the message wich should be hashed.
 * @param length is the length of only THIS block in BITS not in bytes!
 *  bits are big endian, meaning high bits come first.
 * 	if you have a message with bits at the end, the byte must be padded with zeros
 */

/*************************************************************************/

/*
 * length in bits!
 */
void sha256(void* dest, const void* msg, uint32_t length_b){ /* length could be choosen longer but this is for µC */
	sha256_ctx_t s;
	sha256_init(&s);
	while(length_b >= SHA256_BLOCK_BITS){
		sha256_nextBlock(&s, msg);
		msg = (uint8_t*)msg + SHA256_BLOCK_BITS/8;
		length_b -= SHA256_BLOCK_BITS;
	}
	sha256_lastBlock(&s, msg, length_b);
	sha256_ctx2hash(dest,&s);
}



/*************************************************************************/

void sha256_ctx2hash(void* dest, const sha256_ctx_t *state){
#if defined LITTLE_ENDIAN
	uint8_t i, j, *s=(uint8_t*)(state->h);
	i=8;
	do{
		j=3;
		do{
			*((uint8_t*)dest) = s[j];
			dest = (uint8_t*)dest + 1;
		}while(j--);
		s += 4;
	}while(--i);
#elif BIG_ENDIAN
	memcpy(dest, state->h, 32);
#else
# error unsupported endian type!
#endif
}


