/* sha224.c */
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
 * \file		sha224.c
 * \author		Daniel Otte
 * \date		16.05.2006
 *
 * \par License:
 * 	GPL
 *
 * \brief SHA-224 implementation.
 *
 *
 */

#include <stdint.h>
#include <string.h> /* for memcpy, memmove, memset */
#include <avr/pgmspace.h>
#include "sha2_small_common.h"
#include "sha224.h"

#define LITTLE_ENDIAN

#if defined LITTLE_ENDIAN
#elif defined BIG_ENDIAN
#else
	#error specify endianess!!!
#endif


/*************************************************************************/

const
uint32_t sha224_init_vector[] PROGMEM = {
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
};


/*************************************************************************/

/**
 * \brief \c sh224_init initialises a sha224 context for hashing.
 * \c sh224_init c initialises the given sha224 context for hashing
 * @param state pointer to a sha224 context
 * @return none
 */
void sha224_init(sha224_ctx_t *state){
	state->length=0;
	memcpy_P(state->h, sha224_init_vector, 8*4);
}

/*************************************************************************/
void sha224_nextBlock (sha224_ctx_t *state, const void* block){
	sha2_small_common_nextBlock(state, block);
}

/*************************************************************************/
void sha224_lastBlock (sha224_ctx_t *state, const void* block, uint16_t length_b){
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
void sha224(void* dest, const void* msg, uint32_t length_b){ /* length could be choosen longer but this is for µC */
	sha224_ctx_t s;
	sha224_init(&s);
	while(length_b >= SHA224_BLOCK_BITS){
		sha224_nextBlock(&s, msg);
		msg = (uint8_t*)msg + SHA224_BLOCK_BITS/8;
		length_b -= SHA224_BLOCK_BITS;
	}
	sha224_lastBlock(&s, msg, length_b);
	sha224_ctx2hash(dest,&s);
}



/*************************************************************************/

void sha224_ctx2hash(void* dest, const sha224_ctx_t *state){
#if defined LITTLE_ENDIAN
	uint8_t i, j, *s=(uint8_t*)(state->h);
	i=7;
	do{
		j=3;
		do{
			*((uint8_t*)dest) = s[j];
			dest = (uint8_t*)dest + 1;
		}while(j--);
		s += 4;
	}while(--i);
#elif BIG_ENDIAN
	memcpy(dest, state->h, 28);
#else
# error unsupported endian type!
#endif
}


