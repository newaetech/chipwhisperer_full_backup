/* twister.h */
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
 * \file     twister.h
 * \author   Daniel Otte 
 * \date     2008-12-20
 * \license  GPLv3 or later
 * 
 */

#ifndef TWISTER_H_
#define TWISTER_H_

#include <stdint.h>

typedef struct {
	uint8_t  s[8][8];
	uint64_t counter;
	uint64_t length_counter_b;
} twister_state_t;

typedef uint8_t twister_checksum_t[8][8];

void twister_blank_round(twister_state_t* ctx);
void twister_mini_round(twister_state_t* ctx, const void* msg);	
void twister_ctx2hash(void* dest, twister_state_t* ctx, uint16_t hashsize_b);

#endif /* TWISTER_H_ */
