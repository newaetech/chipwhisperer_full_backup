// main.c

// Only include serial_IO.h in one place
#include <stdio.h>
#include <mcs51/serial_IO.h>

#include "glitch_loop.h"

void main (void) 
{
	// Set up things for the code underneath
	// Set up serial port for 4800 baud (7.37 MHz crystal)
	inituart(248);

	// Print one helpful character to make the simulator happy
	putchar('\n');	
	
	// Run the password check forever
	glitch_loop();
}
