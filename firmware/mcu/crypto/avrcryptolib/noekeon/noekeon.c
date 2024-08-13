/* noekeon.c */
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
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3 or later
 * 
 * 
 * 
 */

#include <stdint.h>
#include <string.h>

#ifdef __AVR__
	#include <avr/pgmspace.h>
#endif
#include "noekeon.h"
// #include "cli.h"

#define ROUND_NR 16

#define RC_POS 0

static
void gamma_1(uint32_t* a){
	uint32_t tmp;
	
	a[1] ^= ~((a[3]) | (a[2]));
	a[0] ^=   a[2] & a[1];	
	
	tmp=a[3]; a[3]=a[0]; a[0]=tmp;
	a[2] ^= a[0] ^ a[1] ^ a[3];
	
	a[1] ^= ~((a[3]) | (a[2]));
	a[0] ^=   a[2] & a[1];	
}	

#define ROTL32(a,n) (((a)<<n)|((a)>>(32-n)))
#define ROTR32(a,n) (((a)>>n)|((a)<<(32-n)))

static
void pi1(uint32_t* a){
	a[1] = ROTL32(a[1], 1);
	a[2] = ROTL32(a[2], 5);
	a[3] = ROTL32(a[3], 2);
}

static
void pi2(uint32_t* a){
	a[1] = ROTR32(a[1], 1);
	a[2] = ROTR32(a[2], 5);
	a[3] = ROTR32(a[3], 2);
}

static
void theta(const uint32_t* k, uint32_t* a){
	uint32_t temp;

	temp = a[0] ^ a[2]; temp ^= ROTR32(temp, 8) ^ ROTL32(temp, 8);
	a[1] ^= temp;
	a[3] ^= temp;
	
	a[0] ^= k[0];
	a[1] ^= k[1];
	a[2] ^= k[2];
	a[3] ^= k[3];

	temp = a[1] ^ a[3]; temp ^= ROTR32(temp, 8) ^ ROTL32(temp, 8);
	a[0] ^= temp;
	a[2] ^= temp;	

}

static 
void noekeon_round(uint32_t* key, uint32_t* state, uint8_t const1, uint8_t const2){
	((uint8_t*)state)[RC_POS] ^= const1;
	theta(key, state);
	((uint8_t*)state)[RC_POS] ^= const2;
	pi1(state);
	gamma_1(state);
	pi2(state);
}

const uint8_t rc_tab[]
#ifdef __AVR__
 PROGMEM 
#endif
  = {
/*	0x80, */
	      0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D, 0x9A,
	0x2F, 0x5E, 0xBC, 0x63, 0xC6, 0x97, 0x35, 0x6A,
	0xD4
};
/* for more rounds
 0xD4, 0xB3, 0x7D, 0xFA, 0xEF, 0xC5, 0x91, 0x39,
 0x72, 0xE4, 0xD3, 0xBD, 0x61, 0xC2, 0x9F, 0x25,
*/

static
void changendian32(void* a){
	((uint8_t*)a)[0] ^= ((uint8_t*)a)[3];
	((uint8_t*)a)[3] ^= ((uint8_t*)a)[0];
	((uint8_t*)a)[0] ^= ((uint8_t*)a)[3];
	
	((uint8_t*)a)[1] ^= ((uint8_t*)a)[2];
	((uint8_t*)a)[2] ^= ((uint8_t*)a)[1];
	((uint8_t*)a)[1] ^= ((uint8_t*)a)[2];
}

static
void changendian(void* a){
	changendian32((uint32_t*)(&(((uint32_t*)a)[0])));
	changendian32((uint32_t*)(&(((uint32_t*)a)[1])));
	changendian32((uint32_t*)(&(((uint32_t*)a)[2])));
	changendian32((uint32_t*)(&(((uint32_t*)a)[3])));
}

/******************************************************************************/

void noekeon_enc(void* buffer, const void* key){
	uint8_t rc=0x80;
	uint8_t keyb[16];
	int8_t i;
	
	memcpy(keyb, key, 16);
	changendian(buffer);
	changendian(keyb);

	for(i=0; i<ROUND_NR; ++i){
		noekeon_round((uint32_t*)keyb, (uint32_t*)buffer, rc, 0);
#ifdef __AVR__
		rc = pgm_read_byte(rc_tab+i);
#else
		rc = rc_tab[i];
#endif
	}
	((uint8_t*)buffer)[RC_POS] ^= rc;
	theta((uint32_t*)keyb, (uint32_t*)buffer);

	changendian(buffer);
}


void noekeon_dec(void* buffer, const void* key){
	uint8_t rc;
	int8_t i;
	uint8_t nullv[16];
	uint8_t dkey[16];
	

	changendian(buffer);
	
	memset(nullv, 0, 16);
	memcpy(dkey, key, 16);
	changendian(dkey);
	
	theta((uint32_t*)nullv, (uint32_t*)dkey);
//	cli_putstr_P(PSTR("\r\nTheta: "));
//	cli_hexdump(dkey, 16);
	
	for(i=ROUND_NR-1; i>=0; --i){
#ifdef __AVR__
		rc = pgm_read_byte(rc_tab+i);
#else
		rc = rc_tab[i];
#endif
		noekeon_round((uint32_t*)dkey, (uint32_t*)buffer, 0, rc);
	}
	theta((uint32_t*)dkey, (uint32_t*)buffer);
	((uint8_t*)buffer)[RC_POS] ^= 0x80;

	changendian(buffer);
}

void noekeon_init(const void* key, noekeon_ctx_t* ctx){
	uint8_t nullv[16];
	
	memset(nullv, 0, 16);
	memcpy(ctx, key, 16);
	noekeon_enc(ctx, nullv);
}

