// main.c
// combined: One project that combines all of the smaller modules
// Assumes 7.37 MHz clock speed

// Different project modes:
// - print: prints "Testing #" for debug
// - passcheck: password check susceptible to timing attacks
// - glitchloop: looped calculations susceptible to glitching
// - xor: SimpleSerial XOR encryption     (128 bit key, 128 bit block)
// - aes: SimpleSerial AES-128 encryption (128 bit key, 128 bit block)
// - tea: SimpleSerial TEA encryption     (128 bit key, 64 bit block)

// Include this once for 8051-specific IO
#include <mcs51/serial_IO.h>

#include <stdio.h>
#include <8052.h>
#include <stdint.h>
#include <setjmp.h>

// Useful things
#include "utility.h"

// Modules
#include "modules.h"

// Project modes
#define MODE_PRINT      0
#define MODE_PASSCHECK  1
#define MODE_GLITCHLOOP 2
#define MODE_XOR        3
#define MODE_AES        4
#define MODE_TEA        5

void main(void) 
{	
	// Which mode we're in - read this from the headers
	uint8_t mode = 0;
	
	// Set up things for the code underneath (serial + trigger)
	inituart(248);	// 4800 baud with 7.37 MHz clock
	trigger_low();

	// Print one helpful character to make the simulator happy
	putchar('\n');
	
	mode = get_mode();
	
	// All of the functions in here are infinite loops
	switch(mode)
	{
		case MODE_PRINT:
			main_print();
			
		case MODE_PASSCHECK:
			main_passcheck();
			
		case MODE_GLITCHLOOP:
			main_glitchloop();
			
		case MODE_XOR:
			main_ss(SS_XOR);
			
		case MODE_AES:
			main_ss(SS_AES);
			
		case MODE_TEA:
			main_ss(SS_TEA);
			
		// Default:
		// Fall through to the end
	}
	
	// This is the worst hack ever.
	// We have to call the getchar() function once here
	// ...so that the compiler keeps it around
	// ...so that other modules can use it
	getchar();
	
	// wait forever
	while(1);
}
