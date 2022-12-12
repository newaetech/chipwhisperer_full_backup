/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2015 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
//#include <util/delay.h>
#include "decryption.h"

char hex_lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void hex_print(const uint8_t * in, int len, char *out)
{
	unsigned int i,j;
	j=0;
	for (i=0; i < len; i++) {
		out[j++] = hex_lookup[in[i] >> 4];
		out[j++] = hex_lookup[in[i] & 0x0F];			
	}
	
	out[j] = 0;
}

#define DATA_BUFLEN 40
#define ASCII_BUFLEN (2 * DATA_BUFLEN)

// uint8_t buffer[ASCII_BUFLEN + DATA_BUFLEN];
// uint8_t *ascii_buffer = buffer;
// uint8_t *data_buffer = buffer + ASCII_BUFLEN;
uint8_t ascii_buffer[ASCII_BUFLEN];
uint8_t data_buffer[DATA_BUFLEN];

#define IDLE 0
#define INPUT 1
#define RESPOND 2

int main(void)
{
    platform_init();
	init_uart();	
	trigger_setup();
	
 	/* Uncomment this to get a HELLO message for debug */
	// putch('h');
	// putch('e');
	// putch('l');
	// putch('l');
	// putch('o');
	// putch('\n');
		
	char c;
	char state = 0;
	uint8_t ascii_idx = 0;
	 
	while(1){
		c = getch();
		
		if (c == 'x' || c == 'k') 
		{
			ascii_idx = 0;
			state = IDLE;
			continue;		
		}	
		
		else if (c == 'p') 
		{
			ascii_idx = 0;
			state = INPUT;
			continue;
		}
		
		else if (state == INPUT) {
			if ((c == '\n') || (c == '\r')) {
				// We received the final character - decode our string
				hex_decode((char*)ascii_buffer, ascii_idx, data_buffer);

				// Decrypt data in-place
				decrypt_data(data_buffer, DATA_BUFLEN);
				
				// This is where we would write the image into memory
				
				// Send back a positive response
				ascii_idx = 0;
				ascii_buffer[ascii_idx++] = 'r';
				ascii_buffer[ascii_idx++] = '0';
				ascii_buffer[ascii_idx++] = '\n';
				ascii_buffer[ascii_idx++] = '\n';
				ascii_buffer[ascii_idx++] = '\n';
				ascii_buffer[ascii_idx++] = '\n';
				ascii_buffer[ascii_idx++] = '\n';
				ascii_buffer[ascii_idx++] = '\n';
				state = RESPOND;
			} 
			else if (ascii_idx >= ASCII_BUFLEN)
			{
				// We have nowhere to put this character - give up!
				state = IDLE;
			} 
			else 
			{
				// Store the character in the buffer so we can use it later
				ascii_buffer[ascii_idx++] = c;
			}
		}
		
		
		if(state == RESPOND)
		{
			// Send the ascii buffer back 
			trigger_high();
			
			int i;
			for(i = 0; i < ascii_idx; i++)
			{
				putch(ascii_buffer[i]);
			}
			trigger_low();
			state = IDLE;
		}
	}
		
	return 1;
}
