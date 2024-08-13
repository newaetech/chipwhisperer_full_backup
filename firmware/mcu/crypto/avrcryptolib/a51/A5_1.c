/* A5_1.c */
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
/* 
 * File:        A5_1.c
 * Author:      Daniel Otte
 * email:       daniel.otte@rub.de
 * Date:        2006-06-24
 * License:     GPLv3 or later
 * Description: Implementation of the A5/1 stream cipher algorithm, as used in GSM.
 * ! Warning, this is weak crypto !
 * 
 */
 
#include <stdint.h>
#include <string.h>
#include "A5_1.h"
#include <avr/pgmspace.h>

uint8_t a5_1_clock_core(a5_1_ctx_t *c, uint8_t clockoverride);


/*
 * length is length of key in bits!
 */

void a5_1_init(a5_1_ctx_t *c, void* key, uint8_t keylength_b, void* iv, uint8_t ivlength_b){
	uint8_t i,t;
	memset(c->r1, 0, 3);
	memset(c->r2, 0, 3);
	memset(c->r3, 0, 3);
	for(i=0; i<keylength_b; ++i){
		t=((uint8_t*)key)[i/8];
		t=1&(t>>i);
		c->r1[0] ^= t;
		c->r2[0] ^= t;
		c->r3[0] ^= t;
		a5_1_clock_core(c, 0x7);
	}
	for(i=0; i<ivlength_b; ++i){
		t=((uint8_t*)iv)[i/8];
		t=1&(t>>i);
		c->r1[0] ^= t;
		c->r2[0] ^= t;
		c->r3[0] ^= t;
		a5_1_clock_core(c, 0x7);
	}
	for(i=0; i<100; ++i)
		a5_1_clock_core(c,0);
}

static
void shiftreg(uint8_t* d){
	uint8_t c, c2;
	c=d[0]>>7;
	d[0] <<= 1;
	c2=d[1]>>7;
	d[1] = (d[1]<<1) | c;
	d[2] = (d[2]<<1) | c2;
} 

const uint8_t parity3_lut[] PROGMEM = {0, 1, 1, 0,
		                             1, 0, 0, 1}; 	
const uint8_t clock_lut[] PROGMEM =  {0x7, 0x6, 0x5, 0x3,
		                            0x3, 0x5, 0x6, 0x7}; 
		                            
uint8_t a5_1_clock_core(a5_1_ctx_t *c, uint8_t clockoverride){
	uint8_t ret,clk,fb;
	ret = (0x04&c->r1[2]) | (0x20&c->r2[2]) | (0x40&c->r3[2]);
	ret = ret^(ret>>6);
	ret &= 0x7;
	ret = pgm_read_byte(parity3_lut+ret);
	clk = (0x08&c->r1[1]) | (0x10&c->r2[1]) | (0x20&c->r3[1]);
	clk >>= 3;
	clk = pgm_read_byte(clock_lut+clk);
	clk |= clockoverride;
	
	if(clk&1){
		fb = c->r1[2] ^ (1&((c->r1[1])>>5));
		fb &= 0x7;
		fb = pgm_read_byte(parity3_lut+fb);
		shiftreg(c->r1);
		c->r1[0] |= fb;
		c->r1[2] &= 0x07;
	}
	clk>>=1;
	if(clk&1){
		fb = c->r2[2]>>4 ;
		fb &= 0x7;
		fb = pgm_read_byte(parity3_lut+fb);
		shiftreg(c->r2);
		c->r2[0] |= fb;
		c->r2[2] &= 0x3F;
	
	}
	clk>>=1;
	if(clk&1){
		fb = (c->r3[2]>>4) ^ (1&((c->r3[0])>>7));
		fb &= 0x7;
		fb = pgm_read_byte(parity3_lut+fb);
		shiftreg(c->r3);
		c->r3[0] |= fb;
		c->r3[2] &= 0x7F;
	}
	return ret;
}

uint8_t a5_1_clock(a5_1_ctx_t *c){
	return a5_1_clock_core(c, 0);
}


uint8_t a5_1_gen(a5_1_ctx_t *c){
	uint8_t ret=0;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	ret <<= 1;
	ret = a5_1_clock(c);
	return ret;
}




