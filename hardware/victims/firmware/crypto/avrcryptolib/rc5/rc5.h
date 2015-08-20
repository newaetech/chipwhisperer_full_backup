/* rc5.h */
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
/* rc5.h a C implementation of RC5 for AVR microcontrollers
 * 
 * author: Daniel Otte 
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * this implementation is limited to 64bit blocks and a maximum of 255 rounds
 * 
 */
#ifndef RC5_H_
#define RC5_H_


#include <stdint.h>
#include <stdlib.h> /* malloc() & free() */
#include <string.h> /* memset() & memcpy() */
 
typedef struct rc5_ctx_st {
	uint8_t rounds;
	uint32_t *s;
}rc5_ctx_t; 

void rc5_enc(void* buffer, const rc5_ctx_t* ctx);
void rc5_dec(void* buffer, const rc5_ctx_t* ctx);
void rc5_init(void* key, uint16_t keysize_b, uint8_t rounds, rc5_ctx_t* ctx);
void rc5_free(rc5_ctx_t* ctx);

#endif /*RC5_H_*/
