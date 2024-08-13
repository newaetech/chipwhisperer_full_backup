/* rc6.h */
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
 * File:	rc6.h
 * Author:	Daniel Otte
 * Date: 	06.08.2006
 * License: GPL
 * Description: Implementation of the RC6 cipher algorithm.
 * 	This implementation is restricted to 32-bit words, but free in the choice of number of rounds (0 to 255).
 * 	so it is RC6-32/r/b
 */

#ifndef RC6_H_
#define RC6_H_

 
#include <stdint.h>
 
typedef struct rc6_ctx_st{
	uint8_t		rounds;		/* specifys the number of rounds; default: 20 */
 	uint32_t*	S;			/* the round-keys */
} rc6_ctx_t;
 
 
uint8_t rc6_init(void* key, uint16_t keylength_b, rc6_ctx_t *s);
 
uint8_t rc6_initl(void* key, uint16_t keylength_b, uint8_t rounds, rc6_ctx_t *s);
 
void rc6_enc(void* block, rc6_ctx_t *s);

void rc6_dec(void* block, rc6_ctx_t *s);
 
void rc6_free(rc6_ctx_t *s); 

#endif /* RC6_H_ */

