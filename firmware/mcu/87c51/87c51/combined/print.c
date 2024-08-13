// print.c
// Prints "Testing #" with increasing #s to a serial port

#include "modules.h"

#include <stdio.h> 
//#include <8052.h>

void main_print(void) 
{
	// SDCC doesn't support mixed code and variable declarations!
	// Make sure we define our variables first
	int x = 0;
	
	// Print our messages
	while(1)
	{
		printf_tiny("Testing %d\n", x);
		x += 1;
	}

	// We'll never get here, so don't bother returning
}
