/* noekeon_prng.c */
/*
 *   This file is part of the AVR-Crypto-Lib.
 *   Copyright (C) 2006, 2007, 2008  Daniel Otte (daniel.otte@rub.de)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \author	Daniel Otte
 * \date	2008-08-24
 * \license GPLv3 or later
 * \brief   random number generator based on noekeon running in CFB-mode
 * 
 */

#include "noekeon.h"
#include "memxor.h"
#include <stdint.h>
#include <string.h>

static uint8_t random_state[16];
static uint8_t random_key[16];
static uint8_t i=0;

uint8_t random8(void){
	static uint8_t sr[16];
	
	if(i==0){
		noekeon_enc(random_state, random_key);
		memcpy(sr, random_state, 16);
		i=15;
		return sr[15];
	}
	--i;
	return sr[i];
}

void random_block(void* dest){
	i=0;
	noekeon_enc(random_state, random_key);
	memcpy(dest, random_state, 16);
}

void srandom32(uint32_t seed){
	memcpy(random_key, &seed, 4);
}

void random_seed(const void* buffer){
	memcpy(random_key, buffer, 16);
}

void random_add(const void* buffer){
	memxor(random_key, buffer, 16);
}


