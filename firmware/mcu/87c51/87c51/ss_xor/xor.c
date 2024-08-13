// xor.c

#include <stdint.h>

#include "xor.h"

#define BLOCK_LEN 16

void xor_encrypt(uint8_t* state, uint8_t* key)
{
	unsigned char i;
	for(i = 0; i < BLOCK_LEN; i++)
	{
		state[i] ^= key[i];
	}
}