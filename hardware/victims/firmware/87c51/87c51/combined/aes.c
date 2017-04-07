// aes.c

// test:
// k2B7E151628AED2A6ABF7158809CF4F3C  \r
// p3243F6A8885A308D313198A2E0370734  \r
// r3925841d02dc09fbdc118597196a0b32  \r

#include "modules.h"

#include <stdio.h>
#include <stdint.h>
#include <8052.h>

#include "simpleserial.h"
#include "utility.h"
#include "aes_crypto.h"

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE   16

void main_aes(void) 
{
	// 16 byte arrays for input (plaintext) and key
	uint8_t input[16];
	uint8_t key  [16];
	
	// Let the SimpleSerial module fill in the input and key arrays
	while(1)
	{
		// Read data from SimpleSerial
		if(simpleserial_get(input, key, AES_BLOCK_SIZE, AES_KEY_SIZE))
		{
			// We're done reading an input: encrypt in place and send back
			trigger_high();
			aes_encrypt(input, key);
			trigger_low();
			simpleserial_put(input, AES_BLOCK_SIZE);
		}
	}
}
