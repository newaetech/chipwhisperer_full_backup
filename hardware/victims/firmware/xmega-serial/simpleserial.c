/*
    This file is part of the AESExplorer Example Targets
    Copyright (C) 2012 Colin O'Flynn <coflynn@newae.com>

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
#include "uart.h"
#include "AES_driver.h"
#include "aes-independant.h"
#include <stdint.h>

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
uint8_t key[16];
uint8_t pt[16];
uint8_t tmp[16] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};

//USE_PLL doesn't require an external clock. This has issues with noise though so isn't recommended
//#define USE_PLL

int main
	(
	void
	)
	{
    
#ifdef USE_PLL    
	OSC.PLLCTRL = 4;
	OSC.CTRL |= OSC_PLLEN_bm;
	
	while((OSC.STATUS & OSC_PLLRDY_bm) == 0);
	    
	// Select system clock source: External Osc. or Clock
	uint8_t n=(CLK.CTRL & (~CLK_SCLKSEL_gm)) | CLK_SCLKSEL_PLL_gc;
	CCP=CCP_IOREG_gc;
	CLK.CTRL=n;
    
    PORTD.DIRSET = PIN7_bm;                   /* To output clock, port must be set as output */
	PORTCFG.CLKEVOUT = PORTCFG_CLKOUT_PD7_gc; /* Output the clock frequency on PD7 to measure on Counter/Scope */     
#else
    OSC.XOSCCTRL = 0x00;
    OSC.PLLCTRL = 0x00;
    OSC.CTRL |= OSC_XOSCEN_bm;

    //wait for clock
    while((OSC.STATUS & OSC_XOSCRDY_bm) == 0);
    
    //Switch clock source
    CCP = CCP_IOREG_gc;
    CLK.CTRL = CLK_SCLKSEL_XOSC_gc;    
    
    //Turn off other sources besides external    
    OSC.CTRL = OSC_XOSCEN_bm;
#endif
   	
	init_uart0();
	
	AES_software_reset();
	
	PORTA.DIRSET = PIN0_bm;
	
    //Test Trigger Pin
    //PORTA.OUT = PIN0_bm;
    //PORTA.OUT = 0;
    
    //Test UART 
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
	
	h014BAF	
    */
	
	char c;
	int ptr = 0;
	
	//Initial key
	for(uint8_t i = 0; i < 16; i++){
		key[i] = tmp[i];
	}
	aes_indep_init();
	aes_indep_key(tmp);

	char state = 0;
	
	static uint8_t useHwEnc = 0;
	 
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
			useHwEnc = 0;
			continue;
		}
		
		else if (c == 'h') {
			ptr = 0;
			state = PLAIN;
			useHwEnc = 1;
			continue;
		}		
		
		else if (state == KEY) {
			if ((c == '\n') || (c == '\r')) {
				asciibuf[ptr] = 0;
				hex_decode(asciibuf, ptr, tmp);
				aes_indep_key(tmp);
				for(uint8_t i = 0; i < 16; i++){
					key[i] = tmp[i];
				}                
				state = IDLE;
			} else {
				asciibuf[ptr++] = c;
			}
		}
		
		else if (state == PLAIN) {
			if ((c == '\n') || (c == '\r')) {
				asciibuf[ptr] = 0;
				hex_decode(asciibuf, ptr, pt);

				_delay_ms(25);
				
				/* Do Encryption */
				if(useHwEnc){
					PORTA.OUT = PIN0_bm;
					AES_encrypt(pt, pt, key);
					PORTA.OUT = 0;
				} else {
					PORTA.OUT = PIN0_bm;
					aes_indep_enc(pt);
					PORTA.OUT = 0;
				}
				
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
	
	