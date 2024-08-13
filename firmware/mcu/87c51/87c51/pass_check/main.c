// main.c

// !! Dangerous note! !!
// There are function implementations in this header file, so we can't include
// it multiple times in one project (multiple definition errors)
// However, stdio.h has extern void prototypes for putchar() and getchar(), so
// as long as we include it once (here), we can happily use these functions
// Moral of the story: don't include this file anywhere else!
#include <mcs51/serial_IO.h>

#include "pass_check.h"

void main (void) 
{
	// Set up things for the code underneath
	// Set up serial port for 4800 baud (7.37 MHz crystal)
	inituart(248);

	// Print one helpful character to make the simulator happy
	putchar('\n');
	
	
	// Run the password check
	check_password();

	// Show's over - wait here until they reset
	while(1);
}
