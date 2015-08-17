/* shabea.c */
/*
 *   This file is part of AnonAccess, an access system which can be used
 *    to open door or doing other things with an anonymity featured
 *    account managment.
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
 * \file	shabea.c
 * \author	Daniel Otte 
 * \date	2007-06-07
 * \brief	SHABEA - a SHA Based Encryption Algorithm implementation
 * \par License	
 * GPL
 * 
 * SHABEAn-r where n is the blocksize and r the number of round used
 * 
 * 
 */
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#include "config.h"
#include "memxor.h"


/*
 * SHABEA256-n
 */ 
 
#define SHABEA_BLOCKSIZE 256
#define SHABEA_BLOCKSIZEB (SHABEA_BLOCKSIZE/8)
#define SHABEA_HALFSIZEB  (SHABEA_BLOCKSIZEB/2)
#define SHABEA_HALFSIZE (SHABEA_BLOCKSIZE/2)

#define L ((uint8_t*)block+ 0)
#define R ((uint8_t*)block+16)
void shabea256(void * block, void * key, uint16_t keysize_b, uint8_t enc, uint8_t rounds){
	uint8_t r;		/**/
	uint8_t tb[SHABEA_HALFSIZEB+2+(keysize_b+7)/8];	/**/
	uint16_t kbs;	/* bytes used for the key / temporary block */
	sha256_hash_t hash;
	uint8_t termcond; 
	int8_t dir;
	if(enc){
		r = 0;
		termcond = rounds-1;
		dir = 1;
	} else {
		r = rounds-1;
		termcond = 0;
		dir = -1;
	}
	kbs = (keysize_b+7)/8;
	memcpy(tb+SHABEA_HALFSIZEB+2, key, kbs); /* copy key to temporary block */
	tb[SHABEA_HALFSIZEB+0] = 0;	/* set round counter high value to zero */
	
	for(;;r+=dir){ /* enc: 0..(rounds-1) ; !enc: (rounds-1)..0 */
		memcpy(tb, R, SHABEA_HALFSIZEB); /* copy right half into tb */
		tb[SHABEA_HALFSIZEB+1] = r;
		sha256(&hash, tb, SHABEA_HALFSIZE+16+keysize_b);
		if(r!=termcond){	
			/* swap */
			memxor(hash, L, SHABEA_HALFSIZEB);
			memcpy(L, R, SHABEA_HALFSIZEB);
			memcpy(R, hash, SHABEA_HALFSIZEB);
		} else {
			/* last round */
			/* no swap */
			memxor(L, hash, SHABEA_HALFSIZEB);
			return;	
		}
	}
}


