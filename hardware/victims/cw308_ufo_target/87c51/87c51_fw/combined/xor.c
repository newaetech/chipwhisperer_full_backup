// xor.c

// test:
// k00112233445566778899AABBCCDDEEFF  \r
// p00112233445566778899AABBCCDDEEFF  \r
// p0123456789ABCDEF0123456789ABCDEF  \r

#include "modules.h"

#include <stdint.h>
#include <stdio.h>

#include "simpleserial.h"
#include "utility.h"

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

void main_xor(void) 
{
	// 16 byte arrays for input (plaintext) and key
	uint8_t input[16];
	uint8_t key  [16];
	
	// Let the SimpleSerial module fill in the input and key arrays
	while(1)
	{
		// Read data from SimpleSerial
		if(simpleserial_get(input, key, XOR_BLOCK_SIZE, XOR_KEY_SIZE))
		{
			// We're done reading an input: encrypt in place and send back
			trigger_high();
			xor_encrypt(input, key);
			trigger_low();
			simpleserial_put(input, XOR_BLOCK_SIZE);
		}
	}
}
