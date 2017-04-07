/*
 * simpleserial.c
 *
 * Created: 03/08/2016 4:58:32 PM
 *  Author: greg
 */ 

#include "simpleserial.h"
#include "uart.h"

// Maximum length of an ASCII message
#define ASCII_LENGTH 34

// Maximum length of data array
#define DATA_LENGTH 16

#define STATE_IDLE   0
#define STATE_READ   1

static const char hex_lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

char ascii_buffer[ASCII_LENGTH];
uint8_t address[DATA_LENGTH];

uint8_t* simpleserial_getaddress(void)
{
	return address;
}

// Encode this byte array into the 34 byte ASCII buffer
// ASCII format: "r0123456789ABCDEF\n"
void simpleserial_encode(uint8_t* data, uint8_t size)
{
	int i;
	
	ascii_buffer[0] = 'r';
	for(i = 0; i < size; i++)
	{
		unsigned char n_hi = (data[i] >> 4) & 0x0f;
		unsigned char n_lo =  data[i]       & 0x0f;
		ascii_buffer[2*i+1] = hex_lookup[n_hi];
		ascii_buffer[2*i+2] = hex_lookup[n_lo];
	}
	ascii_buffer[2*size+1] = '\n';
}

// Decode the ASCII buffer into a byte array
void simpleserial_decode(uint8_t* data, uint8_t size)
{
	int i;
	
	for(i = 0; i < size; i++)
	{
		unsigned char n_hi = ascii_buffer[2*i + 1];
		unsigned char n_lo = ascii_buffer[2*i + 2];
		
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

int simpleserial_get(uint8_t size)
{	
	unsigned char state = STATE_IDLE;
	unsigned char ascii_pos = 0;

	// Repeat reading loop until done
	while(1)
	{
		// Get character
		ascii_buffer[ascii_pos] = uart_rx_byte();

		if(state == STATE_IDLE)
		{
			// If we're in idle mode, check for 'a' to break us out
			// Any other character will fall through and return
			if(ascii_buffer[ascii_pos] == 'a')
			{
				state = STATE_READ;
				ascii_pos++;
				continue;
			}
		}
		else // state == STATE_READ
		{
			uint8_t length_limit = 2*size + 1;
			
			// If we're in the message body, check that characters are [0-9A-F]
			if(ascii_pos < length_limit)
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
			else if((ascii_pos == length_limit)
			&& ((ascii_buffer[ascii_pos] == '\n') || (ascii_buffer[ascii_pos] == '\r')))
			{
				simpleserial_decode(address, size);
				return 1;
			}
		}
		
		// If we get here, something was wrong with the message
		// (Could be a message like "x\n")
		return 0;
	}
}

void simpleserial_put(uint8_t* output, uint8_t size)
{
	uint8_t size_ascii = 2*size+2;
	
	simpleserial_encode(output, size);
	for(int i = 0; i < size_ascii; i++)
	{
		uart_tx_byte(ascii_buffer[i]);
	}
}
