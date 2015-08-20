/* entropium.c */
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
 * \file    entropium.c
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2006-05-17
 * \license	GPLv3 or later
 * \brief	This file contains an implementaition of a pseudo-random-number generator.
 * 
 * Extension 1:
 * 	rndCore is expanded to 512 bits for more security.
 *
  \verbatim
                       ################################################################################################
                       #                                                                                              #
                       #         +---------------------------+                                                        #
                       #         |                           |                                                        #
                       #         V                           |                                                        #
                       #      (concat)                       |                                                        #
   +---------------+   #    o---------o             (xor)+---------+      o---------o        o----o     o---------o   #    +--------------+
   | entropy Block | -----> | sha-256 | --(offset)-<     | rndCore | ---> | sha-256 | --+----| +1 |---> | sha-256 | -----> | random Block |
   +---------------+   #    o---------o             (xor)+---------+      o---------o   |    o----o     o---------o   #    +--------------+
                       #                                 (xor) (xor)                    |                             #
                       #                                   ^     ^                      |                             #
                       #                                    \   /                       |                             #
                       #                                   (offset)---------------------+                             #
                       #                                                                                              #
                       ################################################################################################
  \endverbatim
 */

#include <stdint.h>
#include <string.h>
#include "sha256.h"
#include "entropium.h"

/**
 * \brief secret entropy pool. 
 * This is the core of the random which is generated
 */
uint32_t rndCore[16]; 

/*************************************************************************/

/* idea is: hash the message and add it via xor to rndCore
 *
 * length in bits 
 * 
 * we simply first "hash" rndCore, then entropy.
 */
void entropium_addEntropy(unsigned length_b, const void* data){
	sha256_ctx_t s;
	static uint8_t offset=0; /* selects if higher or lower half gets updated */
	sha256_init(&s);
	sha256_nextBlock(&s, rndCore);
	while (length_b>=512){
		sha256_nextBlock(&s, data);
		data = (uint8_t*)data+ 512/8;
		length_b -= 512;	
	}
	sha256_lastBlock(&s, data, length_b);
	uint8_t i;
	for (i=0; i<8; ++i){
		rndCore[i+offset] ^= s.h[i];
	}
	offset ^= 8; /* hehe */
}

/*************************************************************************/

void entropium_getRandomBlock(void *b){
	sha256_ctx_t s;
	static uint8_t offset=8;
	
	sha256_init(&s);
	sha256_lastBlock(&s, rndCore, 512); /* remeber the byte order! */
	uint8_t i;
	for (i=0; i<8; ++i){
		rndCore[i+offset] ^= s.h[i];
	}
	offset ^= 8; /* hehe */
	memcpy(b, s.h, 32); /* back up first hash in b */
	((uint8_t*)b)[*((uint8_t*)b)&31]++; 	/* the important increment step */
	sha256_init(&s);
	sha256_lastBlock(&s, b, 256);
	memcpy(b, s.h, 32);
}

/*************************************************************************/

uint8_t entropium_getRandomByte(void){
	static uint8_t block[32];
	static uint8_t i=32;
	
	if (i==32){
		entropium_getRandomBlock((void*)block);
		i=0;
	}	
	return block[i++];
}

void entropium_fillBlockRandom(void* block, unsigned length_B){
	while(length_B>ENTROPIUM_RANDOMBLOCK_SIZE){
		entropium_getRandomBlock(block);
		block = (uint8_t*)block + ENTROPIUM_RANDOMBLOCK_SIZE;
		length_B -= ENTROPIUM_RANDOMBLOCK_SIZE;
	}
	while(length_B){
		*((uint8_t*)block) = entropium_getRandomByte();
		block= (uint8_t*)block +1; --length_B;
	}
}
 
 
