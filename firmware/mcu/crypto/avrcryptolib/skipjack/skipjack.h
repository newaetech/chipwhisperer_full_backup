/* skipjack.h */
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
#ifndef SKIPJACK_H_
#define SKIPJACK_H_

/** 
 * \file      skipjack.h
 * \author    Daniel Otte
 * \date      2006-11-01
 * \license   GPLv3 or later
 * \brief     Implementation of the serpent sbox function.
 * 
 */


#include <stdint.h>

/** \fn void skipjack_enc(void* block, void* key)
 * \brief skipjack encryption function
 * 
 * This function encrypts a block of plaintext with the Skipjac encryption
 * algorithm. The block is 64 bit (8 byte) long, the key is 80 bit (10 byte)
 * long.
 * \param block pointer to the 64 bit (8 byte) block to encrypt
 * \param key   pointer to the 80 bit (10 byte) key 
 */
void skipjack_enc(void* block, void* key);

/** \fn void skipjack_dec(void* block, void* key)
 * \brief skipjack decryption function
 * 
 * This function decrypts a block of ciphertext encrypted with the Skipjac
 * encryption algorithm. 
 * The block is 64 bit (8 byte) long, the key is 80 bit (10 byte) long.
 * \param block pointer to the 64 bit (8 byte) block to decrypt
 * \param key   pointer to the 80 bit (10 byte) key 
 */
void skipjack_dec(void* block, void* key);

#endif /*SKIPJACK_H_*/
