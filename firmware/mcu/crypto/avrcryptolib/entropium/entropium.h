/* entropium.h */
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
 * File:		entropium.h
 * Author:		Daniel Otte
 * Date:		23.07.2006
 * License:		GPL
 * Description:	This file contains the declarations for the pseudo-random-number generator.
 **/

/**
 * \file    entropium.h
 * \author  Daniel Otte
 * \date    23.07.2006
 * \license	GPLv3 or later
 * \brief	This file contains the declarations for the pseudo-random-number generator.
 **/


#ifndef ENTROPIUM_H_
#define ENTROPIUM_H_

#include <stdint.h>
/*
 * length in bits 
 */
#define ENTROPIUM_RANDOMBLOCK_SIZE 32 /* bytes */
 
/** \fn void entropium_addEntropy(unsigned length_b, const void* data)
 * \brief add entropy to the prng
 * 
 * This function adds data to the internal entropy pool
 * \param length_b length of the data block in bits
 * \param data pointer to the data
 */
void entropium_addEntropy(unsigned length_b, const void* data); 

/** \fn void entropium_getRandomBlock(void* b)
 * \brief generate a fixed size block of random data 
 * 
 * This function writes 32 bytes of random extracted from the entropy pool
 * in the supplied buffer.
 * \param b buffer where the random data gets written
 */
void entropium_getRandomBlock(void* b);

/** \fn uint8_t entropium_getRandomByte(void)
 * \brief get a single byte of random data
 * 
 * This function utilizes a internal buffer which gets automatically filled
 * again. 
 * \return a byte of random data
 */ 
uint8_t entropium_getRandomByte(void);

/** \fn void entropium_fillBlockRandom(void* block, unsigned length_B)
 * \brief get a block of random data
 * 
 * This function writes random data extracted from the entropy pool in the 
 * supplied buffer. It shares a internal buffer with the 
 * entropium_getRandomByte() function.
 * \param block pointer to the buffer where the random data goes
 * \param length_B number of bytes to be written to the buffer
 */
void entropium_fillBlockRandom(void* block, unsigned length_B);

#endif /*PRNG_H_*/
