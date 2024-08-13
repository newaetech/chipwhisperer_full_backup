/* shacal1_enc.c */
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
 * \file	shacal1_enc.c
 * \author	Daniel Otte
 * \date	2008-05-06
 * \par License:
 * GPL
 * \brief SHACAL1 encryption only implementation.
 * 
 */

#include <stdint.h>
#include <string.h>
#include "sha1.h"
#include "shacal1_enc.h"

void shacal1_enc(void* buffer, void* key, uint16_t keysize_b){
	sha1_ctx_t ctx, t_ctx;
	uint8_t i;
	memcpy(t_ctx.h, buffer, SHA1_HASH_BITS/8);
	
	uint8_t keybuffer[SHA1_BLOCK_BITS/8];
	memset(keybuffer, 0, SHA1_BLOCK_BITS/8);
	if(keysize_b>SHA1_BLOCK_BITS)
		keysize_b=SHA1_BLOCK_BITS;
	memcpy(keybuffer, key, (keysize_b+7)/8);
	
	memcpy(t_ctx.h, buffer, SHA1_HASH_BITS/8);
	sha1_ctx2hash(&ctx.h[0], &t_ctx);
	memcpy(t_ctx.h, ctx.h, SHA1_HASH_BITS/8);
	sha1_nextBlock(&ctx, keybuffer);
	for(i=0; i<5; ++i)
		ctx.h[i] -= t_ctx.h[i];
	sha1_ctx2hash(buffer, &ctx);
}


