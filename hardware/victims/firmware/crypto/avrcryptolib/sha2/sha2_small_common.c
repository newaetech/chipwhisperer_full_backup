/* sha2_small_common.c */
/*
    This file is part of the ARM-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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
#include <avr/pgmspace.h>
#include "sha2_small_common.h"


#define LITTLE_ENDIAN

/**
 * rotate x right by n positions
 */
static
uint32_t rotr32( uint32_t x, uint8_t n){
	return ((x>>n) | (x<<(32-n)));
}

static
uint32_t rotl32( uint32_t x, uint8_t n){
	return ((x<<n) | (x>>(32-n)));
}


/*************************************************************************/

// #define CHANGE_ENDIAN32(x) (((x)<<24) | ((x)>>24) | (((x)& 0x0000ff00)<<8) | (((x)& 0x00ff0000)>>8))
static
uint32_t change_endian32(uint32_t x){
	return (((x)<<24) | ((x)>>24) | (((x)& 0x0000ff00)<<8) | (((x)& 0x00ff0000)>>8));
}


/* sha256 functions as macros for speed and size, cause they are called only once */

#define CH(x,y,z)  (((x)&(y)) ^ ((~(x))&(z)))
#define MAJ(x,y,z) (((x)&(y)) ^ ((x)&(z)) ^ ((y)&(z)))

#define SIGMA_0(x) (rotr32((x), 2) ^ rotr32((x),13) ^ rotl32((x),10))
#define SIGMA_1(x) (rotr32((x), 6) ^ rotr32((x),11) ^ rotl32((x),7))
#define SIGMA_a(x) (rotr32((x), 7) ^ rotl32((x),14) ^ ((x)>>3))
#define SIGMA_b(x) (rotl32((x),15) ^ rotl32((x),13) ^ ((x)>>10))

const
uint32_t k[] PROGMEM = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};



/**
 * block must be, 512, Bit = 64, Byte, long !!!
 */
void sha2_small_common_nextBlock (sha2_small_common_ctx_t *state, const void* block){
	uint32_t w[16], wx;
	uint8_t  i;
	uint32_t a[8],t1,t2;

	/* init w */
#if defined LITTLE_ENDIAN
	for (i=0; i<16; ++i){
		w[i]= change_endian32(((uint32_t*)block)[i]);
	}
#elif defined BIG_ENDIAN
		memcpy((void*)w, block, 64);
#endif
/*
	for (i=16; i<64; ++i){
		w[i] = SIGMA_b(w[i-2]) + w[i-7] + SIGMA_a(w[i-15]) + w[i-16];
	}
*/
/* init working variables */
	memcpy((void*)a,(void*)(state->h), 8*4);

/* do the, fun stuff, */
	for (i=0; i<64; ++i){
		if(i<16){
			wx = w[i];
		}else{
			wx = SIGMA_b(w[14]) + w[9] + SIGMA_a(w[1]) + w[0];
			memmove(&(w[0]), &(w[1]), 15*4);
			w[15] = wx;
		}
		t1 = a[7] + SIGMA_1(a[4]) + CH(a[4],a[5],a[6]) + pgm_read_dword(&k[i]) + wx;
		t2 = SIGMA_0(a[0]) + MAJ(a[0],a[1],a[2]);
		memmove(&(a[1]), &(a[0]), 7*4); 	/* a[7]=a[6]; a[6]=a[5]; a[5]=a[4]; a[4]=a[3]; a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; */
		a[4] += t1;
		a[0] = t1 + t2;
	}

/* update, the, state, */
	for (i=0; i<8; ++i){
		state->h[i] += a[i];
	}
	state->length += 1;
}


void sha2_small_common_lastBlock(sha2_small_common_ctx_t *state, const void* block, uint16_t length_b){
	uint8_t lb[512/8]; /* local block */
	uint64_t len;
	while(length_b>=512){
		sha2_small_common_nextBlock(state, block);
		length_b -= 512;
		block = (uint8_t*)block+64;
	}
	len = state->length*512 + length_b;
	memset(lb, 0, 64);
	memcpy(lb, block, (length_b+7)/8);

	/* set the final one bit */
	lb[length_b/8] |= 0x80>>(length_b & 0x7);
	/* pad with zeros */
	if (length_b>=512-64){ /* not enouth space for 64bit length value */
		sha2_small_common_nextBlock(state, lb);
		memset(lb, 0, 64);
	}
	/* store the 64bit length value */
#if defined LITTLE_ENDIAN
	 	/* this is now rolled up */
	uint8_t i;
	i=7;
	do{
		lb[63-i] = ((uint8_t*)&len)[i];
	}while(i--);
#elif defined BIG_ENDIAN
	*((uint64_t)&(lb[56])) = len;
#endif
	sha2_small_common_nextBlock(state, lb);
}

