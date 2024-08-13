/* shacal2_enc.c */
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
 * \file	shacal2_enc.c
 * \author	Daniel Otte
 * \date	2008-05-07
 * \par License:
 * GPL
 * \brief SHACAL2 encryption only implementation.
 * 
 */

#include <stdint.h>
#include <string.h>
#include "sha256.h"
#include "shacal2_enc.h"


void shacal2_enc(void* buffer, void* key, uint16_t keysize_b){
	uint8_t i;
	sha256_ctx_t ctx, t_ctx;
	memcpy(ctx.h, buffer, SHACAL2_BLOCKSIZE_B);
	
	uint8_t keybuffer[SHACAL2_KEYSIZE_B];
	memset(keybuffer, 0, SHACAL2_KEYSIZE_B);
	if(keysize_b>SHACAL2_KEYSIZE)
		keysize_b=SHACAL2_KEYSIZE;
	memcpy(keybuffer, key, (keysize_b+7)/8);
	
	memcpy(t_ctx.h, buffer, SHACAL2_BLOCKSIZE_B);
	sha256_ctx2hash((sha256_hash_t*)(&(ctx.h[0])), &t_ctx);
	memcpy(t_ctx.h, ctx.h, SHACAL2_BLOCKSIZE_B);
	sha256_nextBlock(&ctx, keybuffer);
	for(i=0; i<SHACAL2_BLOCKSIZE/32; ++i){
		ctx.h[i] -= t_ctx.h[i];
	}
	sha256_ctx2hash(buffer, &ctx);
}



