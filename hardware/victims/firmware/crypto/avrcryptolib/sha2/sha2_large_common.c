/* sha2_large_common.c */
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
#include "sha2_large_common.h"
#include <avr/pgmspace.h>

static const
uint64_t sha2_large_common_const[80] PROGMEM = {
0x428a2f98d728ae22LL, 0x7137449123ef65cdLL, 0xb5c0fbcfec4d3b2fLL, 0xe9b5dba58189dbbcLL,
0x3956c25bf348b538LL, 0x59f111f1b605d019LL, 0x923f82a4af194f9bLL, 0xab1c5ed5da6d8118LL,
0xd807aa98a3030242LL, 0x12835b0145706fbeLL, 0x243185be4ee4b28cLL, 0x550c7dc3d5ffb4e2LL,
0x72be5d74f27b896fLL, 0x80deb1fe3b1696b1LL, 0x9bdc06a725c71235LL, 0xc19bf174cf692694LL,
0xe49b69c19ef14ad2LL, 0xefbe4786384f25e3LL, 0x0fc19dc68b8cd5b5LL, 0x240ca1cc77ac9c65LL,
0x2de92c6f592b0275LL, 0x4a7484aa6ea6e483LL, 0x5cb0a9dcbd41fbd4LL, 0x76f988da831153b5LL,
0x983e5152ee66dfabLL, 0xa831c66d2db43210LL, 0xb00327c898fb213fLL, 0xbf597fc7beef0ee4LL,
0xc6e00bf33da88fc2LL, 0xd5a79147930aa725LL, 0x06ca6351e003826fLL, 0x142929670a0e6e70LL,
0x27b70a8546d22ffcLL, 0x2e1b21385c26c926LL, 0x4d2c6dfc5ac42aedLL, 0x53380d139d95b3dfLL,
0x650a73548baf63deLL, 0x766a0abb3c77b2a8LL, 0x81c2c92e47edaee6LL, 0x92722c851482353bLL,
0xa2bfe8a14cf10364LL, 0xa81a664bbc423001LL, 0xc24b8b70d0f89791LL, 0xc76c51a30654be30LL,
0xd192e819d6ef5218LL, 0xd69906245565a910LL, 0xf40e35855771202aLL, 0x106aa07032bbd1b8LL,
0x19a4c116b8d2d0c8LL, 0x1e376c085141ab53LL, 0x2748774cdf8eeb99LL, 0x34b0bcb5e19b48a8LL,
0x391c0cb3c5c95a63LL, 0x4ed8aa4ae3418acbLL, 0x5b9cca4f7763e373LL, 0x682e6ff3d6b2b8a3LL,
0x748f82ee5defb2fcLL, 0x78a5636f43172f60LL, 0x84c87814a1f0ab72LL, 0x8cc702081a6439ecLL,
0x90befffa23631e28LL, 0xa4506cebde82bde9LL, 0xbef9a3f7b2c67915LL, 0xc67178f2e372532bLL,
0xca273eceea26619cLL, 0xd186b8c721c0c207LL, 0xeada7dd6cde0eb1eLL, 0xf57d4f7fee6ed178LL,
0x06f067aa72176fbaLL, 0x0a637dc5a2c898a6LL, 0x113f9804bef90daeLL, 0x1b710b35131c471bLL,
0x28db77f523047d84LL, 0x32caab7b40c72493LL, 0x3c9ebe0a15c9bebcLL, 0x431d67c49c100d4cLL,
0x4cc5d4becb3e42b6LL, 0x597f299cfc657e2aLL, 0x5fcb6fab3ad6faecLL, 0x6c44198c4a475817LL
};


static const
uint64_t change_endian64(uint64_t x){
	uint64_t r=0;
	uint8_t i=8;
	do{
		r <<= 8;
		r |= 0xff&x;
		x >>=8;
	}while(--i);
	return r;
}



static const
uint64_t rotr64(uint64_t x, uint8_t n){
	return (x>>n)|(x<<(64-n));
}

static const
uint64_t rotl64(uint64_t x, uint8_t n){
	return (x<<n)|(x>>(64-n));
}

static const
uint64_t pgm_read_uint64_t_P(const uint64_t * p){
	union {
		uint64_t v64;
		uint32_t v32[2];
	} ret;
	ret.v32[0] = pgm_read_dword(p);
	ret.v32[1] = pgm_read_dword((uint8_t*)p + 4);
	return ret.v64;
}

#define CH(x,y,z)  (((x)&(y))^((~(x))&(z)))
#define MAJ(x,y,z) (((x)&(y))^((x)&(z))^((y)&(z)))
#define SIGMA_0(x) (rotr64((x), 28) ^ rotl64((x), 30) ^ rotl64((x), 25))
#define SIGMA_1(x) (rotr64((x), 14) ^ rotr64((x), 18) ^ rotl64((x), 23))
#define SIGMA_a(x) (rotr64((x),  1) ^ rotr64((x),  8) ^ ((x)>>7))
#define SIGMA_b(x) (rotr64((x), 19) ^ rotl64((x),  3) ^ ((x)>>6))

void sha2_large_common_nextBlock(sha2_large_common_ctx_t* ctx, const void* block){
	uint64_t w[16], wx;
	uint64_t a[8];
	uint64_t t1, t2;
	const uint64_t *k=sha2_large_common_const;
	uint8_t i;
	i=16;
	do{
		w[16-i] = change_endian64(*((const uint64_t*)block));
		block = (uint8_t*)block + 8;
	}while(--i);
	memcpy(a, ctx->h, 8*8);
	for(i=0; i<80; ++i){
		if(i<16){
			wx=w[i];
		}else{
			wx = SIGMA_b(w[14]) + w[9] + SIGMA_a(w[1]) + w[0];
			memmove(&(w[0]), &(w[1]), 15*8);
			w[15] = wx;
		}
		t1 = a[7] + SIGMA_1(a[4]) + CH(a[4], a[5], a[6]) + pgm_read_uint64_t_P(k++) + wx;
		t2 = SIGMA_0(a[0]) + MAJ(a[0], a[1], a[2]);
		memmove(&(a[1]), &(a[0]), 7*8);
		a[0] = t1 + t2;
		a[4] += t1;
	}
	i=7;
	do{
		ctx->h[i] += a[i];
	}while(i--);
	ctx->length += 1;
}

void sha2_large_common_lastBlock(sha2_large_common_ctx_t* ctx, const void* block, uint16_t length_b){
	while(length_b >= 1024){
		sha2_large_common_nextBlock(ctx, block);
		block = (uint8_t*)block + 1024/8;
		length_b -= 1024;
	}
	uint8_t buffer[1024/8];
	uint64_t len;
	len = ((uint64_t)ctx->length)*1024LL + length_b;
	len = change_endian64(len);
	memset(buffer, 0, 1024/8);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b%8);
	if(length_b>1024-128-1){
		/* length goes into the next block */
		sha2_large_common_nextBlock(ctx, buffer);
		memset(buffer, 0, 120);
	}
	memcpy(&(buffer[128-8]), &len, 8);
	sha2_large_common_nextBlock(ctx, buffer);
}

