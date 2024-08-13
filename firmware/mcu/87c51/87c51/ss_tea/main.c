// main.c
// ss_tea: use SimpleSerial protocol and encrypt with TEA (ECB)

// test:
// k000102030405060708090A0B0C0D0E0F  \r
// p0102030405060708  \r
// rB1A1AB198C45FA5B  \r

#include <stdio.h>
#include <stdint.h>
#include <mcs51/serial_IO.h>
#include <8052.h>

#include "simpleserial.h"
#include "tea.h"

#define trigger_high() P1 |= 0x01
#define trigger_low()  P1 &= ~(0x01)

void main(void) 
{
	// Arrays for input (plaintext) and key
	uint8_t input[8];
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
		if(simpleserial_get(input, key, 8, 16))
		{
			// We're done reading an input: encrypt in place and send back
			trigger_high();
			tea_encrypt(input, key);
			trigger_low();
			simpleserial_put(input, 8);
		}
	}
}
