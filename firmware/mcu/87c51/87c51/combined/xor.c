// xor.c

// test:
// k00112233445566778899AABBCCDDEEFF  \r
// p00112233445566778899AABBCCDDEEFF  \r
// p0123456789ABCDEF0123456789ABCDEF  \r

#include "modules.h"

#include <stdint.h>
#include <stdio.h>

#define XOR_BLOCK_SIZE 16
#define XOR_KEY_SIZE   16

void xor_encrypt(uint8_t* state, uint8_t* key)
{
	unsigned char i;
	for(i = 0; i < XOR_BLOCK_SIZE; i++)
	{
		state[i] ^= key[i];
	}
}

void xor_auth(uint8_t* v, uint8_t* k, uint8_t* e)
{
	uint8_t res = 0x10;
	uint8_t i;
	xor_encrypt(v, k);
	for(i = 0; i < 16; i++)
	{
		if(v[i] != e[i])
			res = 0;
		v[i] = 0;
	}
	v[0] = res;
}