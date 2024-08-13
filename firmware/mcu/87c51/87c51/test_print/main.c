// main.c
// Prints "Testing #" with increasing #s to a serial port
// Assumes 7.37 MHz clock speed

#include <8052.h>
#include <mcs51/serial_IO.h>
#include <stdio.h> 

void func()
{
	int x = 1;
	x += 2;
	int y = x;
}

void main (void) 
{
	// SDCC doesn't support mixed code and variable declarations!
	// Make sure we define our variables first
	int x = 0;

	// Set up serial port for 4800 baud (7.37 MHz crystal)
	inituart(248);

	// Print one helpful character to make the simulator happy
	putchar('\n');
	
	// Print our messages
	while(1)
	{
		printf_tiny("Testing %d\n", x);
		x += 1;
	}

	// We'll never get here, so don't bother returning
}
