// simpleserial.c

#include <stdio.h>
#include <stdint.h>

#include "simpleserial.h"

#define BLOCK_LENGTH 16
#define ASCII_LENGTH (2*BLOCK_LENGTH + 2)
#define STATE_IDLE   0
#define STATE_PT     1
#define STATE_KEY    2

static const char hex_lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// Encode this 16 byte array into the 34 byte ASCII buffer
// ASCII format: "r0123456789ABCDEF\n"
void simpleserial_encode(uint8_t* ascii, uint8_t* data)
{
	int i;
	
	ascii[0] = 'r';
	for(i = 0; i < BLOCK_LENGTH; i++)
	{
		unsigned char n_hi = (data[i] >> 4) & 0x0f;
		unsigned char n_lo =  data[i]       & 0x0f;
		ascii[2*i+1] = hex_lookup[n_hi];
		ascii[2*i+2] = hex_lookup[n_lo];
	}
	ascii[ASCII_LENGTH-1] = '\n';
}

// Decode the ASCII buffer into one of the 16 byte arrays
void simpleserial_decode(uint8_t* ascii, uint8_t* data)
{
	int i;
	
	for(i = 0; i < BLOCK_LENGTH; i++)
	{
		unsigned char n_hi = ascii[2*i + 1];
		unsigned char n_lo = ascii[2*i + 2];
		
		if(n_lo <= '9')
			data[i] = n_lo - '0';
		else if(n_lo <= 'F')
			data[i] = n_lo - 'A' + 10;
		else
			data[i] = n_lo - 'a' + 10;
		
		if(n_hi <= '9')
			data[i] |= (n_hi - '0') << 4;
		else if(n_hi <= 'F')
			data[i] |= (n_hi - 'A' + 10) << 4;
		else
			data[i] |= (n_hi - 'a' + 10) << 4;
	}
}

int simpleserial_get(uint8_t* input, uint8_t* key)
{
	// Make a buffer for the incoming ASCII chars
	char ascii_buffer[ASCII_LENGTH];
	unsigned char ascii_pos = 0;
	
	// Keep track of which state we're in
	unsigned char state = STATE_IDLE;
	
	// Repeat reading loop until done
	while(1)
	{
		// Get character
		ascii_buffer[ascii_pos] = getchar();

		if(state == STATE_IDLE)
		{
			// If we're in idle mode, check for 'k' or 'p' to break us out
			// Any other character will fall through and return
			if(ascii_buffer[ascii_pos] == 'k')
			{
				state = STATE_KEY;
				ascii_pos++;
				continue;
			}
			else if(ascii_buffer[ascii_pos] == 'p')
			{
				state = STATE_PT;
				ascii_pos++;
				continue;
			}
		}
		else // state == STATE_PT or state == STATE_KEY
		{
			// If we're in the message body, check that characters are [0-9A-F]
			if(ascii_pos < (ASCII_LENGTH - 1))
			{
				if((ascii_buffer[ascii_pos] >= '0' && ascii_buffer[ascii_pos] <= '9')
				|| (ascii_buffer[ascii_pos] >= 'A' && ascii_buffer[ascii_pos] <= 'F')
				|| (ascii_buffer[ascii_pos] >= 'a' && ascii_buffer[ascii_pos] <= 'f')
			)
				{
					ascii_pos++;
					continue;
				}
			}
			
			// Otherwise, for the last character, check for '\n' or '\r'
			else if((ascii_pos == ASCII_LENGTH-1)
			&& ((ascii_buffer[ascii_pos] == '\n') || (ascii_buffer[ascii_pos] == '\r')))
			{
				// Decide where to decode our ASCII buffer
				if(state == STATE_PT)
				{
					simpleserial_decode(ascii_buffer, input);
					return 1;
				}
				else // state == STATE_KEY
				{
					simpleserial_decode(ascii_buffer, key);
					return 0;
				}
			}
		}
	
		// If we get here, something was wrong with the message
		// (Could be a message like "x\n")
		return 0;
	}
}

void simpleserial_put(uint8_t* output)
{
	// Make a buffer for the outgoing ASCII chars
	char ascii_buffer[ASCII_LENGTH];
	unsigned char ascii_pos = 0;
	
	simpleserial_encode(ascii_buffer, output);
	for(ascii_pos = 0; ascii_pos < ASCII_LENGTH; ascii_pos++)
	{
		putchar(ascii_buffer[ascii_pos]);
	}
}
