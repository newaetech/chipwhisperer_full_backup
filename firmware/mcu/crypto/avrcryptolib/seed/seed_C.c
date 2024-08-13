/* seed_C.c */
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
 * \file	seed_C.c
 * \author	Daniel Otte 
 * \date	2007-06-1
 * \brief	SEED parts in C for AVR
 * \par License	
 * GPL
 * 
 */
#include <stdint.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "seed_sbox.h"
#include "cli.h"
#include "debug.h"


static
uint32_t g_function(uint32_t x);
/******************************************************************************/

static
void changeendian32(uint32_t * a){
	*a = (*a & 0x000000FF) << 24 |
		 (*a & 0x0000FF00) <<  8 |
		 (*a & 0x00FF0000) >>  8 |
		 (*a & 0xFF000000) >> 24;
}

/******************************************************************************/
static
uint32_t bigendian_sum32(uint32_t a, uint32_t b){
	changeendian32(&a);
	changeendian32(&b);
	a += b;
	changeendian32(&a);
	return a;
}

/******************************************************************************/
static 
uint32_t bigendian_sub32(uint32_t a, uint32_t b){
	changeendian32(&a);
	changeendian32(&b);
	a -= b;
	changeendian32(&a);
	return a;
}

/******************************************************************************/
static inline
uint64_t bigendian_rotl8_64(uint64_t a){
	/*
	changeendian64(&a);
	a = (a<<8) | (a>>(64-8));
	changeendian64(&a);
	*/
	a = (a>>8) | (a<<(64-8));
	return a;
}

/******************************************************************************/
static inline
uint64_t bigendian_rotr8_64(uint64_t a){
	/*
	changeendian64(&a);
	a = (a>>8) | (a<<(64-8));
	changeendian64(&a);
	*/
	a = (a<<8) | (a>>(64-8));
	return a;
}

/******************************************************************************/
static
uint64_t f_function(const uint64_t* a, uint32_t k0, uint32_t k1){
	uint32_t c,d;

	c = *a & 0x00000000FFFFFFFFLL;
	d = (*a>>32) & 0x00000000FFFFFFFFLL;
	
	c ^= k0; d ^= k1;
	d ^= c;
	d = g_function(d);
	c = bigendian_sum32(c,d);
	c = g_function(c);
	d = bigendian_sum32(c,d);
	d = g_function(d);
	c = bigendian_sum32(c,d);
	return ((uint64_t)d << 32) | c;
}

/******************************************************************************/
#define M0 0xfc
#define M1 0xf3
#define M2 0xcf
#define M3 0x3f

#define X3 (((uint8_t*)(&x))[0])
#define X2 (((uint8_t*)(&x))[1])
#define X1 (((uint8_t*)(&x))[2])
#define X0 (((uint8_t*)(&x))[3])

#define Z3 (((uint8_t*)(&z))[0])
#define Z2 (((uint8_t*)(&z))[1])
#define Z1 (((uint8_t*)(&z))[2])
#define Z0 (((uint8_t*)(&z))[3])

static
uint32_t g_function(uint32_t x){
	uint32_t z;
	/* sbox substitution */
	X3 = pgm_read_byte(&(seed_sbox2[X3]));
	X2 = pgm_read_byte(&(seed_sbox1[X2]));
	X1 = pgm_read_byte(&(seed_sbox2[X1]));
	X0 = pgm_read_byte(&(seed_sbox1[X0]));
	/* now the permutation */
	Z0 = (X0 & M0) ^ (X1 & M1) ^ (X2 & M2) ^ (X3 & M3);
	Z1 = (X0 & M1) ^ (X1 & M2) ^ (X2 & M3) ^ (X3 & M0);
	Z2 = (X0 & M2) ^ (X1 & M3) ^ (X2 & M0) ^ (X3 & M1);
	Z3 = (X0 & M3) ^ (X1 & M0) ^ (X2 & M1) ^ (X3 & M2);
	return z;
}
/******************************************************************************/
typedef struct {
	uint32_t k0, k1;
} keypair_t;

keypair_t getnextkeys(uint32_t *keystate, uint8_t curround){
	keypair_t ret;
	if (curround>15){
		/* ERROR */
		ret.k0 = ret.k1 = 0;
	} else {
	/*	ret.k0 = g_function(keystate[0] + keystate[2] - pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = g_function(keystate[1] - keystate[3] + pgm_read_dword(&(seed_kc[curround]))); */
		ret.k0 = bigendian_sum32(keystate[0], keystate[2]);
		ret.k0 = bigendian_sub32(ret.k0, pgm_read_dword(&(seed_kc[curround])));
		ret.k0 = g_function(ret.k0);
		ret.k1 = bigendian_sub32(keystate[1], keystate[3]);
		ret.k1 = bigendian_sum32(ret.k1, pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = g_function(ret.k1);
		
		if (curround & 1){
			/* odd round (1,3,5, ...) */
			((uint64_t*)keystate)[1] = bigendian_rotl8_64( ((uint64_t*)keystate)[1] );
		} else {
			/* even round (0,2,4, ...) */
			((uint64_t*)keystate)[0] = bigendian_rotr8_64(((uint64_t*)keystate)[0]);
		}
	}
	return ret;
}


/******************************************************************************/

keypair_t getprevkeys(uint32_t *keystate, uint8_t curround){
	keypair_t ret;
	if (curround>15){
		/* ERROR */
		ret.k0 = ret.k1 = 0;
	} else {
		if (curround & 1){
			/* odd round (1,3,5, ..., 15) */
			((uint64_t*)keystate)[1] = bigendian_rotr8_64( ((uint64_t*)keystate)[1] );
		} else {
			/* even round (0,2,4, ..., 14) */
			((uint64_t*)keystate)[0] = bigendian_rotl8_64(((uint64_t*)keystate)[0]);
		}
	/*	ret.k0 = g_function(keystate[0] + keystate[2] - pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = g_function(keystate[1] - keystate[3] + pgm_read_dword(&(seed_kc[curround]))); */
		ret.k0 = bigendian_sum32(keystate[0], keystate[2]);
		ret.k0 = bigendian_sub32(ret.k0, pgm_read_dword(&(seed_kc[curround])));
		ret.k0 = g_function(ret.k0);
		ret.k1 = bigendian_sub32(keystate[1], keystate[3]);
		ret.k1 = bigendian_sum32(ret.k1, pgm_read_dword(&(seed_kc[curround])));
		ret.k1 = g_function(ret.k1);
		}
	return ret;
}

/******************************************************************************/

typedef struct{
	uint32_t k[4];
} seed_ctx_t;

/******************************************************************************/

void seed_init(const void * key, seed_ctx_t * ctx){
	memcpy(ctx->k, key, 128/8);
}

/******************************************************************************/

#define L (((uint64_t*)buffer)[0])
#define R (((uint64_t*)buffer)[1])

void seed_enc(void * buffer, const seed_ctx_t * ctx){
	uint8_t r;
	keypair_t k;
	for(r=0; r<8; ++r){
			k = getnextkeys(((seed_ctx_t*)ctx)->k, 2*r);
/*
	DEBUG_S("\r\n\tDBG ka,0: "); cli_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG ka,1: "); cli_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); cli_hexdump((uint8_t*)buffer+0, 8);
	DEBUG_S("\r\n\t DBG R: "); cli_hexdump((uint8_t*)buffer+8, 8);
*/
			L ^= f_function(&R,k.k0,k.k1);
			
			k = getnextkeys(((seed_ctx_t*)ctx)->k, 2*r+1);
/*
	DEBUG_S("\r\n\tDBG kb,0: "); cli_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG kb,1: "); cli_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); cli_hexdump((uint8_t*)buffer+8, 8);
	DEBUG_S("\r\n\t DBG R: "); cli_hexdump((uint8_t*)buffer+0, 8);
*/
			R ^= f_function(&L,k.k0,k.k1);
	}
	/* just an exchange without temp. variable */
	L ^= R;
	R ^= L;
	L ^= R;
}

/******************************************************************************/

#define L (((uint64_t*)buffer)[0])
#define R (((uint64_t*)buffer)[1])

void seed_dec(void * buffer, seed_ctx_t * ctx){
	int8_t r;
	keypair_t k;
	for(r=7; r>=0; --r){
			k = getprevkeys(((seed_ctx_t*)ctx)->k, 2*r+1);
/*
	DEBUG_S("\r\n\tDBG ka,0: "); cli_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG ka,1: "); cli_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); cli_hexdump((uint8_t*)buffer+0, 8);
	DEBUG_S("\r\n\t DBG R: "); cli_hexdump((uint8_t*)buffer+8, 8);
*/
			L ^= f_function(&R,k.k0,k.k1);
			
			k = getprevkeys(((seed_ctx_t*)ctx)->k, 2*r+0);
/*
	DEBUG_S("\r\n\tDBG kb,0: "); cli_hexdump(&k.k0, 4);
	DEBUG_S("\r\n\tDBG kb,1: "); cli_hexdump(&k.k1, 4);
	DEBUG_S("\r\n\t DBG L: "); cli_hexdump((uint8_t*)buffer+8, 8);
	DEBUG_S("\r\n\t DBG R: "); cli_hexdump((uint8_t*)buffer+0, 8);
*/
			R ^= f_function(&L,k.k0,k.k1);
	}
	/* just an exchange without temp. variable */
	L ^= R;
	R ^= L;
	L ^= R;
}











