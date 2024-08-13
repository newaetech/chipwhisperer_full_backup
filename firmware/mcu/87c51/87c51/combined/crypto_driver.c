// crpyto_driver.c

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
#include "crypto.h"

#define MAX_BLOCK_LEN 16
#define MAX_KEY_LEN   16

#define AES_BLOCK_LEN 16
#define AES_KEY_LEN   16

#define TEA_BLOCK_LEN 8
#define TEA_KEY_LEN   16

#define XOR_BLOCK_LEN 16
#define XOR_KEY_LEN   16


void main_ss(uint8_t mode) 
{
	// 16 byte arrays for input (plaintext) and key
	uint8_t input[MAX_BLOCK_LEN];
	uint8_t key  [MAX_KEY_LEN];
	uint8_t exp  [MAX_BLOCK_LEN];
	uint8_t status;
	
	uint8_t block_len;
	uint8_t key_len;
	void (*fp)(uint8_t*, uint8_t*, uint8_t*);
	
	switch(mode)
	{
		case SS_AES:
			block_len = AES_BLOCK_LEN;
			key_len = AES_KEY_LEN;
			fp = aes_auth;
			break;
			
		case SS_TEA:
			block_len = TEA_BLOCK_LEN;
			key_len = TEA_KEY_LEN;
			fp = tea_auth;
			break;
			
		case SS_XOR:
			block_len = XOR_BLOCK_LEN;
			key_len = XOR_KEY_LEN;
			fp = xor_auth;
			break;
	}
	
	// Let the SimpleSerial module fill in the input and key arrays
	while(1)
	{
		// Read data from SimpleSerial
		status = simpleserial_get(input, key, exp, block_len, key_len);
		if(status == ss_get_bad)
			continue;
		
		if(status == ss_get_enc)
		{
			// We're done reading an input: encrypt in place and send back
			trigger_high();
			fp(input, key, exp);
			trigger_low();
			simpleserial_put(input, block_len);
		}
		
		simpleserial_ack();
	}
}
