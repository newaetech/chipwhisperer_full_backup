/* trivium.c */
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
 */

 
#include <stdint.h>
#include <string.h>
#include "trivium.h"
#include <avr/pgmspace.h>

#define G(i) ((((*ctx)[(i)/8])>>(((i)%8)))&1)
#define S(i,v) ((*ctx)[(i)/8] = (((*ctx)[(i)/8]) & (uint8_t)~(1<<((i)%8))) | ((v)<<((i)%8)))
uint8_t trivium_enc(trivium_ctx_t* ctx){
	uint8_t t1,t2,t3,z;
	
	t1 = G(65)  ^ G(92);
	t2 = G(161) ^ G(176);
	t3 = G(242) ^ G(287);
	z  = t1^t2^t3;
	t1 ^= (G(90)  & G(91))  ^ G(170);
	t2 ^= (G(174) & G(175)) ^ G(263);
	t3 ^= (G(285) & G(286)) ^ G(68);
	
	/* shift whole state and insert ts later */
	uint8_t i,c1=0,c2;
	for(i=0; i<36; ++i){
		c2=(((*ctx)[i])>>7);
		(*ctx)[i] = (((*ctx)[i])<<1)|c1;
		c1=c2;
	}
	/* insert ts */
	S(0, t3);
	S(93, t1);
	S(177, t2);
	
	return z?0x080:0x00;
}

uint8_t trivium_getbyte(trivium_ctx_t *ctx){
	uint8_t r=0, i=0;
	do{
		r>>=1;
		r |= trivium_enc(ctx);
	}while(++i<8);
	return r;
}

#define KEYSIZE_B ((keysize_b+7)/8)
#define IVSIZE_B  ((ivsize_b +7)/8)

static const uint8_t rev_table[16] PROGMEM = {
	0x00, 0x08, 0x04, 0x0C,   /* 0000 1000 0100 1100 */
	0x02, 0x0A, 0x06, 0x0E,   /* 0010 1010 0110 1110 */
	0x01, 0x09, 0x05, 0x0D,   /* 0001 1001 0101 1101 */
	0x03, 0x0B, 0x07, 0x0F    /* 0011 1011 0111 1111 */
};

void trivium_init(const void* key, uint16_t keysize_b,
                  const void* iv,  uint16_t ivsize_b,
                  trivium_ctx_t* ctx){
	uint16_t i;
	uint8_t c1,c2;
	uint8_t t1,t2;
	memset((*ctx)+KEYSIZE_B, 0, 35-KEYSIZE_B);
	c2=0;
	c1=KEYSIZE_B;
	do{
		t1 = ((uint8_t*)key)[--c1];
		t2 = (pgm_read_byte(&(rev_table[t1&0x0f]))<<4)|(pgm_read_byte(&(rev_table[t1>>4])));
		(*ctx)[c2++] = t2;
	}while(c1!=0);

	c2=12;
	c1=IVSIZE_B;
	do{
		t1 = ((uint8_t*)iv)[--c1];
		t2 = (pgm_read_byte(&(rev_table[t1&0x0f]))<<4)|(pgm_read_byte(&(rev_table[t1>>4])));
		(*ctx)[c2++] = t2;
	}while(c1!=0);

	for(i=12+IVSIZE_B; i>10; --i){
		c2=(((*ctx)[i])<<5);
		(*ctx)[i] = (((*ctx)[i])>>3)|c1;
		c1=c2;
	}

	(*ctx)[35] = 0xE0;
	
	for(i=0; i<4*288; ++i){
		trivium_enc(ctx);
	}
}


