/* nessie_common.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * common function for nessie-tests
 * 
 * */

#include <string.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <stdlib.h> /* utoa() */
#include "nessie_common.h"
#include "hexdigit_tab.h"

#ifdef NESSIE_ALIVE
void nessie_send_alive(void){
	NESSIE_PUTC(NESSIE_ALIVE_CHAR);
}

void nessie_send_alive_a(uint16_t i){
	if((i&31)==0)
		NESSIE_PUTC(NESSIE_ALIVE_CHAR);
}
#endif

void nessie_print_block(uint8_t* block, uint16_t blocksize_bit){
	uint16_t i;
	for(i=0; i<(blocksize_bit+7)/8; ++i){
		NESSIE_PUTC(pgm_read_byte(hexdigit_tab_uc_P+((block[i])>>4)));
		NESSIE_PUTC(pgm_read_byte(hexdigit_tab_uc_P+((block[i])&0xf)));
	}				   
}

#define SPACES 31
#define BYTESPERLINE 16

void nessie_print_item(char* name, uint8_t* buffer, uint16_t size_B){
	uint8_t name_len;
	uint8_t i;
	name_len=strlen(name);
	if(name_len>SPACES-1){
		NESSIE_PUTSTR_P(PSTR("\r\n!!! formatting error !!!\r\n"));
		return;
	}
	NESSIE_PUTSTR_P(PSTR("\r\n"));
	for(i=0; i<SPACES-name_len-1; ++i){
		NESSIE_PUTC(' ');
	}
	NESSIE_PUTSTR(name);
	NESSIE_PUTC('=');
	/* now the data printing begins */
	if(size_B<=BYTESPERLINE){
		/* one line seems sufficient */
		nessie_print_block(buffer, size_B*8);
	} else {
		/* we need more lines */
		nessie_print_block(buffer, BYTESPERLINE*8); /* first line */
		int16_t toprint = size_B - BYTESPERLINE;
		buffer += BYTESPERLINE;
		while(toprint > 0){
			NESSIE_PUTSTR_P(PSTR("\r\n"));
			for(i=0; i<SPACES; ++i){
				NESSIE_PUTC(' ');
			}
			nessie_print_block(buffer, ((toprint>BYTESPERLINE)?BYTESPERLINE:toprint)*8);
			buffer  += BYTESPERLINE;
			toprint -= BYTESPERLINE;
		}
	}
} 


void nessie_print_set_vector(uint8_t set, uint16_t vector){
	NESSIE_PUTSTR_P(PSTR("\r\n\r\nSet "));
	NESSIE_PUTC('0'+set%10);
	NESSIE_PUTSTR_P(PSTR(", vector#"));
	NESSIE_PUTC((vector<1000)?' ':'0'+vector/1000);
	NESSIE_PUTC((vector<100)?' ':'0'+(vector/100)%10);
	NESSIE_PUTC((vector<10 )?' ':'0'+(vector/10)%10);
	NESSIE_PUTC('0'+vector%10);
	NESSIE_PUTC(':');
}

/* example:
Test vectors -- set 3
=====================
 */ 
void nessie_print_setheader(uint8_t set){
	NESSIE_PUTSTR_P(PSTR("\r\n\r\nTest vectors -- set "));
	NESSIE_PUTC('0'+set%10);
	NESSIE_PUTSTR_P(PSTR("\r\n====================="));
}

/* example:
********************************************************************************
*Project NESSIE - New European Schemes for Signature, Integrity, and Encryption*
********************************************************************************

Primitive Name: Serpent
=======================
Key size: 256 bits
Block size: 128 bits
*/

void nessie_print_header(char* name,
                         uint16_t keysize_b, 
                         uint16_t blocksize_b,
                         uint16_t hashsize_b, 
                         uint16_t macsize_b,
                         uint16_t ivsize_b ){
	uint16_t i;
	NESSIE_PUTSTR_P(PSTR("\r\n\r\n"
	"********************************************************************************\r\n"
	"* AVR-Crypto-Lib - crypto primitives for AVR microcontrolles by Daniel Otte    *\r\n"
	"********************************************************************************\r\n"
	"\r\n"));
	NESSIE_PUTSTR_P(PSTR("Primitive Name: "));
	NESSIE_PUTSTR(name);
	NESSIE_PUTSTR_P(PSTR("\r\n"));
	/* underline */	
	for(i=0; i<16+strlen(name); ++i){
		NESSIE_PUTC('=');
	}
	char str[6]; /* must catch numbers up to 65535 + terminatin \0 */
	if(keysize_b){
		NESSIE_PUTSTR_P(PSTR("\r\nKey size: "));
		utoa(keysize_b, str, 10);
		NESSIE_PUTSTR(str);
		NESSIE_PUTSTR_P(PSTR(" bits"));
	}
	if(blocksize_b){
		NESSIE_PUTSTR_P(PSTR("\r\nBlock size: "));
		utoa(blocksize_b, str, 10);
		NESSIE_PUTSTR(str);
		NESSIE_PUTSTR_P(PSTR(" bits"));
	}
	if(hashsize_b){
		NESSIE_PUTSTR_P(PSTR("\r\nHash size: "));
		utoa(hashsize_b, str, 10);
		NESSIE_PUTSTR(str);
		NESSIE_PUTSTR_P(PSTR(" bits"));
	}
	if(macsize_b){
		NESSIE_PUTSTR_P(PSTR("\r\nMac size: "));
		utoa(macsize_b, str, 10);
		NESSIE_PUTSTR(str);
		NESSIE_PUTSTR_P(PSTR(" bits"));
	}
	if(ivsize_b){
		if(ivsize_b==(uint16_t)-1){
			NESSIE_PUTSTR_P(PSTR("\r\nNo initial value (IV) mode"));
		}else{
			NESSIE_PUTSTR_P(PSTR("\r\nIV size: "));
			utoa(ivsize_b, str, 10);
			NESSIE_PUTSTR(str);
			NESSIE_PUTSTR_P(PSTR(" bits"));
		}
	}
	NESSIE_PUTSTR_P(PSTR("\r\n"));
}

void nessie_print_footer(void){
	NESSIE_PUTSTR_P(PSTR("\r\n\r\n\r\n\r\nEnd of test vectors\r\n\r\n"));
}

