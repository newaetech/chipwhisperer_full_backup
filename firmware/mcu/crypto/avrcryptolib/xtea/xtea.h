/* xtea.h */
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
 * Author:	Daniel Otte
 * Date:		06.06.2006
 * License:	GPL
 */

#ifndef XTEA_H_
#define XTEA_H_

#include <stdint.h> 
 
/*
 * this fits for xtea.c and xtea-asm.S
 * 
 */
#define XTEA_BLOCKSIZE 64
#define XTEA_BLOCKSIZEB ((XTEA_BLOCKSIZE+7)/8)
#define XTEA_KEYSIZE 128
#define XTEA_KEYSIZEB ((XTEA_KEYSIZE+7)/8)


/*
 * dest: the destination where result of operation will be placed (64 bit)
 * v:	 the block to operate on (64 bit)
 * k:	 the key for en/decryption (128 bit)
 */
void xtea_enc(void* dest, const void* v, const void* k);
void xtea_dec(void* dest, const void* v, const void* k);


#endif /*XTEA_H_*/
