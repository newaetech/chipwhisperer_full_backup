/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2016 NewAE Technology Inc.

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

#include "des.h"
#define KEY_LENGTH 8
#define DEFAULT_KEY {0,1,2,3,4,5,6,7}

#define IDLE 0
#define KEY 1
#define PLAIN 2

char hex_lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

uint8_t* hex_decode(const char *in, int len,uint8_t *out)
{
        unsigned int i, t, hn, ln;

        for (t = 0,i = 0; i < len; i+=2,++t) {

                hn = in[i] > '9' ? (in[i]|32) - 'a' + 10 : in[i] - '0';
                ln = in[i+1] > '9' ? (in[i+1]|32) - 'a' + 10 : in[i+1] - '0';
                out[t] = (hn << 4 ) | ln;
        }

        return out;
}

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

#define BUFLEN KEY_LENGTH*4

uint8_t memory[BUFLEN];
char asciibuf[BUFLEN];
uint8_t pt[8];
uint8_t ct[8];
uint8_t key[KEY_LENGTH] = {DEFAULT_KEY};

int main
	(
	void
	)
	{
    platform_init();
	init_uart();	
	trigger_setup();
	
 	/* Uncomment this to get a HELLO message for debug */
	/*
	putch('h');
	putch('e');
	putch('l');
	putch('l');
	putch('o');
	putch('\n');
	*/
			
	/* Super-Crappy Protocol works like this:
	
	Send kKEY
	Send pPLAINTEXT
	*** Encryption Occurs ***
	receive rRESPONSE
	
	e.g.:
	
    kE8E9EAEBEDEEEFF0F2F3F4F5F7F8F9FA\n
	p014BAF2278A69D331D5180103643E99A\n
	r6743C3D1519AB4F2CD9A78AB09A511BD\n
    */
		
	char c;
	int ptr = 0;
    
	char state = 0;
    
    //des_enc(ct, key, key);
	 
	while(1){
	
		c = getch();
		
		if (c == 'x') {
			ptr = 0;
			state = IDLE;
			continue;		
		}
		
		if (c == 'k') {
			ptr = 0;
			state = KEY;			
			continue;
		}
		
		else if (c == 'p') {
			ptr = 0;
			state = PLAIN;
			continue;
		}
		
		
		else if (state == KEY) {
			if ((c == '\n') || (c == '\r')) {
				asciibuf[ptr] = 0;
				hex_decode(asciibuf, ptr, key);
				state = IDLE;
			} else {
				asciibuf[ptr++] = c;
			}
		}
		
		else if (state == PLAIN) {
			if ((c == '\n') || (c == '\r')) {
				asciibuf[ptr] = 0;
				hex_decode(asciibuf, ptr, pt);

				/* Do Encryption */					
                //NOTE: Setup for DES is huge - about 15000(!) clock cycles. This results in a large
                //      and annoying offset. Instead trigger_high() is called within the DES algorithm.
				//trigger_high();
				des_enc(ct, pt, key); /* encrypting the data block */
				trigger_low();
				               
				/* Print Results */
				hex_print(ct, 8, asciibuf);
				
				putch('r');
				for(int i = 0; i < 16; i++){
					putch(asciibuf[i]);
				}
				putch('\n');
				
				state = IDLE;
			} else {
                if (ptr >= BUFLEN){
                    state = IDLE;
                } else {
                    asciibuf[ptr++] = c;
                }
			}
		}
	}
		
	return 1;
	}
	
	