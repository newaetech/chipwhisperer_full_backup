/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2013 Colin O'Flynn <coflynn@newae.com>

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

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>

#include "uart.h"

#include "aes-independant.h"

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

uint8_t memory[64];
char asciibuf[64];
uint8_t pt[16];
//Default key
uint8_t tmp[16] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};


//We want to use the AVR ADC-pins, since they have a seperate power rail
#define trigger_setup() DDRC |= 0x01
#define trigger_high()  PORTC |= 0x01
#define trigger_low()   PORTC &= ~(0x01)

//Uncomment the following to get a varying clock
//#define VARYING_CLOCK

#define OSCMAX 0x7F
#define OSCMIN 0x00

/* NOTE: The following is determined emperically. You want to calibrate the
   internal oscillator to 7.37 MHz */
#define OSCCAL_UARTGOOD 104

int main
	(
	void
	)
	{
	init_uart0();
	
	/* For 2 MHz crystal use this hack */
	//BAUD0L_REG = 12;
	
	trigger_setup();
	
    //OSCCAL only needed for internal oscillator mode, doesn't hurt anyway
	OSCCAL = OSCCAL_UARTGOOD;	
	_delay_ms(500);

	/* Uncomment this to get a HELLO message for debug */
	/*
	output_ch_0('h');
	output_ch_0('e');
	output_ch_0('l');
	output_ch_0('l');
	output_ch_0('o');
	output_ch_0('\n');
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
	
	//Initial key
	aes_indep_init();
	aes_indep_key(tmp);

	char state = 0;
	 
#ifdef VARYING_CLOCK
	uint8_t newosc;
#endif
	 
	while(1){
	
		c = input_ch_0();
		
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
				hex_decode(asciibuf, ptr, tmp);
				aes_indep_key(tmp);
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

				//The following is used for varying the clock
#ifdef VARYING_CLOCK
				_delay_ms(25);
				
				newosc = (rand() & 10);
				OSCCAL = OSCCAL_UARTGOOD + (int8_t)((int8_t)5 - (int8_t)newosc);
				
				_delay_ms(1);
#endif
				
				trigger_high();
				aes_indep_enc(pt); /* encrypting the data block */
				trigger_low();
				
#ifdef VARYING_CLOCK
				OSCCAL = OSCCAL_UARTGOOD;
				_delay_ms(100);
#endif
                
				/* Print Results */
				hex_print(pt, 16, asciibuf);
				
				output_ch_0('r');
				for(int i = 0; i < 32; i++){
					output_ch_0(asciibuf[i]);
				}
				output_ch_0('\n');
				
				state = IDLE;
			} else {
				asciibuf[ptr++] = c;
			}
		}
	}
		
	return 1;
	}
	
	