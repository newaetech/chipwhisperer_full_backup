// main.c
// ss_aes: use SimpleSerial protocol and encrypt with AES-128 (ECB)

// test:
// k2B7E151628AED2A6ABF7158809CF4F3C  \r
// p3243F6A8885A308D313198A2E0370734  \r
// r3902DC1925DC116A8409850B1DFB9732  \r

#include <stdio.h>
#include <stdint.h>
#include <mcs51/serial_IO.h>
#include <8052.h>

#include "simpleserial.h"
#include "aes.h"

#define trigger_high() P1 |= 0x01
#define trigger_low()  P1 &= ~(0x01)

void main(void) 
{
	// 16 byte arrays for input (plaintext) and key
	uint8_t input[16];
	uint8_t key  [16];
	
	// Set up things for the code underneath (serial + trigger)
	inituart(248);	// 4800 baud
	trigger_low();

	// Print one helpful character to make the simulator happy
	putchar('\n');
	
	// Let the SimpleSerial module fill in the input and key arrays
	while(1)
	{
		// Read data from SimpleSerial
		if(simpleserial_get(input, key))
		{
			// We're done reading an input: encrypt in place and send back
			trigger_high();
			aes_encrypt(input, key);
			trigger_low();
			simpleserial_put(input);
		}
	}
}
