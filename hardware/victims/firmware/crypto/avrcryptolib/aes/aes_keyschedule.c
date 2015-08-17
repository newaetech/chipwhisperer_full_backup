/* aes_keyschedule.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file     aes_keyschedule.c
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2008-12-30
 * \license  GPLv3 or later
 *
 */

#include <stdint.h>
#include "aes.h"
#include "aes_keyschedule.h"
#include "aes_sbox.h"
#include <string.h>
#include <avr/pgmspace.h>

static
void aes_rotword(void* a){
	uint8_t t;
	t=((uint8_t*)a)[0];
	((uint8_t*)a)[0] = ((uint8_t*)a)[1];
	((uint8_t*)a)[1] = ((uint8_t*)a)[2];
	((uint8_t*)a)[2] = ((uint8_t*)a)[3];
	((uint8_t*)a)[3] = t;
}

const uint8_t rc_tab[] PROGMEM = { 0x01, 0x02, 0x04, 0x08,
                             0x10, 0x20, 0x40, 0x80,
                             0x1b, 0x36 };

void aes_init(const void* key, uint16_t keysize_b, aes_genctx_t* ctx){
	uint8_t hi,i,nk, next_nk;
	uint8_t rc=0;
	union {
		uint32_t v32;
		uint8_t  v8[4];
	} tmp;
	nk=keysize_b>>5; /* 4, 6, 8 */
	hi=4*(nk+6+1);
	memcpy(ctx, key, keysize_b/8);
	next_nk = nk;
	for(i=nk;i<hi;++i){
		tmp.v32 = ((uint32_t*)(ctx->key[0].ks))[i-1];
		if(i!=next_nk){
			if(nk==8 && i%8==4){
				tmp.v8[0] = pgm_read_byte(aes_sbox+tmp.v8[0]);
				tmp.v8[1] = pgm_read_byte(aes_sbox+tmp.v8[1]);
				tmp.v8[2] = pgm_read_byte(aes_sbox+tmp.v8[2]);
				tmp.v8[3] = pgm_read_byte(aes_sbox+tmp.v8[3]);
			}
		} else {
			next_nk += nk;
			aes_rotword(&(tmp.v32));
			tmp.v8[0] = pgm_read_byte(aes_sbox+tmp.v8[0]);
			tmp.v8[1] = pgm_read_byte(aes_sbox+tmp.v8[1]);
			tmp.v8[2] = pgm_read_byte(aes_sbox+tmp.v8[2]);
			tmp.v8[3] = pgm_read_byte(aes_sbox+tmp.v8[3]);
			tmp.v8[0] ^= pgm_read_byte(rc_tab+rc);
			rc++;
		}
		((uint32_t*)(ctx->key[0].ks))[i] = ((uint32_t*)(ctx->key[0].ks))[i-nk]
		                                   ^ tmp.v32;
	}
}

void aes128_init(const void* key, aes128_ctx_t* ctx){
	aes_init(key, 128, (aes_genctx_t*)ctx);
}

void aes192_init(const void* key, aes192_ctx_t* ctx){
	aes_init(key, 192, (aes_genctx_t*)ctx);
}

void aes256_init(const void* key, aes256_ctx_t* ctx){
	aes_init(key, 256, (aes_genctx_t*)ctx);
}
