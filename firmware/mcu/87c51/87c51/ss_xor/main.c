// main.c
// ss_xor: use SimpleSerial protocol and encrypt 16 bytes with XOR

// test:
// k00112233445566778899AABBCCDDEEFF  \r
// p00112233445566778899AABBCCDDEEFF  \r
// p0123456789ABCDEF0123456789ABCDEF  \r

#include <stdio.h>
#include <stdint.h>
#include <mcs51/serial_IO.h>

#include "simpleserial.h"
#include "xor.h"

void main(void) 
{
	// 16 byte arrays for input (plaintext) and key
	uint8_t input[16];
	uint8_t key  [16];
	
	// Set up things for the code underneath
	// Set up serial port for 4800 baud (7.37 MHz crystal)
	inituart(248);

	// Print one helpful character to make the simulator happy
	putchar('\n');
	
	// Let the SimpleSerial module fill in the input and key arrays
	while(1)
	{
		// Read data from SimpleSerial
		if(simpleserial_get(input, key))
		{
			// We're done reading an input: encrypt in place and send back
			xor_encrypt(input, key);
			simpleserial_put(input);
		}
	}
}
