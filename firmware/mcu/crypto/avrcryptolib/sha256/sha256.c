/* sha256.c */
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
 * \file		sha256.c
 * \author		Daniel Otte
 * \date		16.05.2006
 *
 * \par License:
 * 	GPL
 *
 * \brief SHA-256 implementation.
 *
 *
 */

#include <stdint.h>
#include <string.h> /* for memcpy, memmove, memset */
#include "sha256.h"

#define LITTLE_ENDIAN

#if defined LITTLE_ENDIAN
#elif defined BIG_ENDIAN
#else
	#error specify endianess!!!
#endif


/*************************************************************************/

uint32_t sha256_init_vector[]={
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };


/*************************************************************************/

/**
 * \brief \c sh256_init initialises a sha256 context for hashing.
 * \c sh256_init c initialises the given sha256 context for hashing
 * @param state pointer to a sha256 context
 * @return none
 */
void sha256_init(sha256_ctx_t *state){
	state->length=0;
	memcpy(state->h, sha256_init_vector, 8*4);
}

/*************************************************************************/

/**
 * rotate x right by n positions
 */
uint32_t rotr32( uint32_t x, uint8_t n){
	return ((x>>n) | (x<<(32-n)));
}


/*************************************************************************/

// #define CHANGE_ENDIAN32(x) (((x)<<24) | ((x)>>24) | (((x)& 0x0000ff00)<<8) | (((x)& 0x00ff0000)>>8))

uint32_t change_endian32(uint32_t x){
	return (((x)<<24) | ((x)>>24) | (((x)& 0x0000ff00)<<8) | (((x)& 0x00ff0000)>>8));
}


/*************************************************************************/

/* sha256 functions as macros for speed and size, cause they are called only once */

#define CH(x,y,z)  (((x)&(y)) ^ ((~(x))&(z)))
#define MAJ(x,y,z) (((x)&(y)) ^ ((x)&(z)) ^ ((y)&(z)))

#define SIGMA0(x) (rotr32((x),2) ^ rotr32((x),13) ^ rotr32((x),22))
#define SIGMA1(x) (rotr32((x),6) ^ rotr32((x),11) ^ rotr32((x),25))
#define SIGMA_a(x) (rotr32((x),7)  ^ rotr32((x),18) ^ ((x)>>3))
#define SIGMA_b(x) (rotr32((x),17) ^ rotr32((x),19) ^ ((x)>>10))


uint32_t k[]={
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


/*************************************************************************/

/**
 * block must be, 512, Bit = 64, Byte, long !!!
 */
void sha256_nextBlock (sha256_ctx_t *state, const void* block){
	uint32_t w[64];	/* this is 256, byte, large, */
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
		for (i=16; i<64; ++i){
			w[i] = SIGMA_b(w[i-2]) + w[i-7] + SIGMA_a(w[i-15]) + w[i-16];
		}

	/* init working variables */
		memcpy((void*)a,(void*)(state->h), 8*4);

	/* do the, fun stuff, */
		for (i=0; i<64; ++i){
			t1 = a[7] + SIGMA1(a[4]) + CH(a[4],a[5],a[6]) + k[i] + w[i];
			t2 = SIGMA0(a[0]) + MAJ(a[0],a[1],a[2]);
			memmove(&(a[1]), &(a[0]), 7*4); 	/* a[7]=a[6]; a[6]=a[5]; a[5]=a[4]; a[4]=a[3]; a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; */
			a[4] += t1;
			a[0] = t1 + t2;
		}

	/* update, the, state, */
		for (i=0; i<8; ++i){
			state->h[i] += a[i];
		}
		state->length += 512;
}


/*************************************************************************/

/**
 * \brief function to process the last block being hashed
 * @param state Pointer to the context in which this block should be processed.
 * @param block Pointer to the message wich should be hashed.
 * @param length is the length of only THIS block in BITS not in bytes!
 *  bits are big endian, meaning high bits come first.
 * 	if you have a message with bits at the end, the byte must be padded with zeros
 */
void sha256_lastBlock(sha256_ctx_t *state, const void* block, uint16_t length){
	uint8_t lb[SHA256_BLOCK_BITS/8]; /* local block */
	while(length>=SHA256_BLOCK_BITS){
		sha256_nextBlock(state, block);
		length -= SHA256_BLOCK_BITS;
		block = (uint8_t*)block+SHA256_BLOCK_BYTES;
	}

	state->length += length;
	memcpy (&(lb[0]), block, length/8);

	/* set the final one bit */
	if (length & 0x7){ // if we have single bits at the end
		lb[length/8] = ((uint8_t*)(block))[length/8];
	} else {
		lb[length/8] = 0;
	}
	lb[length/8] |= 0x80>>(length & 0x7);
	length =(length >> 3) + 1; /* from now on length contains the number of BYTES in lb*/
	/* pad with zeros */
	if (length>64-8){ /* not enouth space for 64bit length value */
		memset((void*)(&(lb[length])), 0, 64-length);
		sha256_nextBlock(state, lb);
		state->length -= 512;
		length = 0;
	}
	memset((void*)(&(lb[length])), 0, 56-length);
	/* store the 64bit length value */
#if defined LITTLE_ENDIAN
	 	/* this is now rolled up */
	uint8_t i;
	for (i=1; i<=8; ++i){
		lb[55+i] = (uint8_t)(state->length>>(64- 8*i));
	}
#elif defined BIG_ENDIAN
	*((uint64_t)&(lb[56])) = state->length;
#endif
	sha256_nextBlock(state, lb);
}


/*************************************************************************/

/*
 * length in bits!
 */
void sha256(sha256_hash_t *dest, const void* msg, uint32_t length){ /* length could be choosen longer but this is for µC */
	sha256_ctx_t s;
	sha256_init(&s);
	while(length >= SHA256_BLOCK_BITS){
		sha256_nextBlock(&s, msg);
		msg = (uint8_t*)msg + SHA256_BLOCK_BITS/8;
		length -= SHA256_BLOCK_BITS;
	}
	sha256_lastBlock(&s, msg, length);
	sha256_ctx2hash(dest,&s);
}



/*************************************************************************/

void sha256_ctx2hash(sha256_hash_t *dest, const sha256_ctx_t *state){
#if defined LITTLE_ENDIAN
	uint8_t i;
	for(i=0; i<8; ++i){
		((uint32_t*)dest)[i] = change_endian32(state->h[i]);
	}
#elif BIG_ENDIAN
	if (dest != state->h)
		memcpy(dest, state->h, SHA256_HASH_BITS/8);
#else
# error unsupported endian type!
#endif
}


