/* aes.c */
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

#include <stdint.h>
#include <string.h>
#include "gf256mul.h"
#include "aes.h"
#include "aes_invsbox.h"
#include "aes_dec.h"
#include <avr/pgmspace.h>

void aes_invshiftrow(void* data, uint8_t shift){
	uint8_t tmp[4];
	tmp[0] = ((uint8_t*)data)[(4+0-shift)&3];
	tmp[1] = ((uint8_t*)data)[(4+1-shift)&3];
	tmp[2] = ((uint8_t*)data)[(4+2-shift)&3];
	tmp[3] = ((uint8_t*)data)[(4+3-shift)&3];
	memcpy(data, tmp, 4);
}

void aes_invshiftcol(void* data, uint8_t shift){
	uint8_t tmp[4];
	tmp[0] = ((uint8_t*)data)[ 0];
	tmp[1] = ((uint8_t*)data)[ 4];
	tmp[2] = ((uint8_t*)data)[ 8];
	tmp[3] = ((uint8_t*)data)[12];
	((uint8_t*)data)[ 0] = tmp[(4-shift+0)&3];
	((uint8_t*)data)[ 4] = tmp[(4-shift+1)&3];
	((uint8_t*)data)[ 8] = tmp[(4-shift+2)&3];
	((uint8_t*)data)[12] = tmp[(4-shift+3)&3];
}
static
void aes_dec_round(aes_cipher_state_t* state, const aes_roundkey_t* k){
	uint8_t tmp[16];
	uint8_t i;
	uint8_t t,u,v,w;
	/* keyAdd */
	for(i=0; i<16; ++i){
		tmp[i] = state->s[i] ^ k->ks[i];
	}
	/* mixColums */
	for(i=0; i<4; ++i){
		t = tmp[4*i+3] ^ tmp[4*i+2];
		u = tmp[4*i+1] ^ tmp[4*i+0];
		v = t ^ u;
		v = gf256mul(0x09, v, 0x1b);
		w = v ^ gf256mul(0x04, tmp[4*i+2] ^ tmp[4*i+0], 0x1b);
		v = v ^ gf256mul(0x04, tmp[4*i+3] ^ tmp[4*i+1], 0x1b);
		state->s[4*i+3] = tmp[4*i+3] ^ v ^ gf256mul(0x02, tmp[4*i+0] ^ tmp[4*i+3], 0x1b);
		state->s[4*i+2] = tmp[4*i+2] ^ w ^ gf256mul(0x02, t, 0x1b);
		state->s[4*i+1] = tmp[4*i+1] ^ v ^ gf256mul(0x02, tmp[4*i+2] ^ tmp[4*i+1], 0x1b);
		state->s[4*i+0] = tmp[4*i+0] ^ w ^ gf256mul(0x02, u, 0x1b);
		
		/*
		state->s[4*i+0] =
			  gf256mul(0xe, tmp[4*i+0], 0x1b)
			^ gf256mul(0xb, tmp[4*i+1], 0x1b)
			^ gf256mul(0xd, tmp[4*i+2], 0x1b)
			^ gf256mul(0x9, tmp[4*i+3], 0x1b);
		state->s[4*i+1] =
			  gf256mul(0x9, tmp[4*i+0], 0x1b)
			^ gf256mul(0xe, tmp[4*i+1], 0x1b)
			^ gf256mul(0xb, tmp[4*i+2], 0x1b)
			^ gf256mul(0xd, tmp[4*i+3], 0x1b);
		state->s[4*i+2] =
			  gf256mul(0xd, tmp[4*i+0], 0x1b)
			^ gf256mul(0x9, tmp[4*i+1], 0x1b)
			^ gf256mul(0xe, tmp[4*i+2], 0x1b)
			^ gf256mul(0xb, tmp[4*i+3], 0x1b);
		state->s[4*i+3] =
			  gf256mul(0xb, tmp[4*i+0], 0x1b)
			^ gf256mul(0xd, tmp[4*i+1], 0x1b)
			^ gf256mul(0x9, tmp[4*i+2], 0x1b)
			^ gf256mul(0xe, tmp[4*i+3], 0x1b);
		*/
	}	
	/* shiftRows */
	aes_invshiftcol(state->s+1, 1);
	aes_invshiftcol(state->s+2, 2);
	aes_invshiftcol(state->s+3, 3);		
	/* subBytes */
	for(i=0; i<16; ++i){
		state->s[i] = pgm_read_byte(aes_invsbox+state->s[i]);
	}
}


static
void aes_dec_firstround(aes_cipher_state_t* state, const aes_roundkey_t* k){
	uint8_t i;
	/* keyAdd */
	for(i=0; i<16; ++i){
		state->s[i] ^= k->ks[i];
	}
	/* shiftRows */
	aes_invshiftcol(state->s+1, 1);
	aes_invshiftcol(state->s+2, 2);
	aes_invshiftcol(state->s+3, 3);		
	/* subBytes */
	for(i=0; i<16; ++i){
		state->s[i] = pgm_read_byte(aes_invsbox+state->s[i]);
	}
}

void aes_decrypt_core(aes_cipher_state_t* state, const aes_genctx_t* ks, uint8_t rounds){
	uint8_t i;
	aes_dec_firstround(state, &(ks->key[i=rounds]));
	for(;rounds>1;--rounds){
		--i;
		aes_dec_round(state, &(ks->key[i]));
	}
	for(i=0; i<16; ++i){
		state->s[i] ^= ks->key[0].ks[i];
	}
}
