/* jh_simple_speed.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2010 Daniel Otte (daniel.otte@rub.de)

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

#include <stdint.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include "memxor.h"
#include "jh_simple.h"
#include "jh_tables.h"

#define DEBUG 0

#if DEBUG
#include "cli.h"
#endif

static
void jh_round(uint8_t* a, uint8_t roundno){
	uint8_t b[128];
	uint8_t i,r=0,u,v,x,y;
	uint8_t *pr;
	pr = jh_round_const + 32*roundno;
	for(i=0; i<128; ++i){
		if(i%4==0){
			r = pgm_read_byte(pr++);
		}
		b[i]=pgm_read_byte(&(jh_lutbox[((r&0xC0)<<2)|a[i]]));
		r<<=2;
	}
	for(i=0;i<128;++i){
		u = pgm_read_byte(jh_permutation_table+2*i);
		v = pgm_read_byte(jh_permutation_table+2*i+1);
		x = b[u>>1];
		y = b[v>>1];
		if(u&1){
			x <<= 4;
		}else{
			x &= 0xf0;
		}
		if(v&1){
			y &= 0x0f;
		}else{
			y >>= 4;
		}
		a[i] = x|y;
	}
}

/*
static
uint8_t jh_l_inv(uint8_t a){
	uint8_t v,w;
	v = a>>4;
	w = a&0xf;
	v ^= ((w<<1)^(w>>3)^((w>>2)&2))&0xf;
	w ^= ((v<<1)^(v>>3)^((v>>2)&2))&0xf;
	return w|(v<<4);
}
*/

static inline
void group(uint8_t *a){
	uint8_t b[128];
	uint8_t i,x,y;
	for(i=0; i<128; ++i){
		x =   (((a[i/8+  0])>>4)&0x8)
			| (((a[i/8+ 32])>>5)&0x4)
			| (((a[i/8+ 64])>>6)&0x2)
			| (((a[i/8+ 96])>>7)&0x1);
		a[i/8] <<= 1; a[i/8+32]<<=1; a[i/8+64]<<=1; a[i/8+96]<<=1;
		y =   (((a[i/8+ 16])>>4)&0x8)
		    | (((a[i/8+ 48])>>5)&0x4)
		    | (((a[i/8+ 80])>>6)&0x2)
		    | (((a[i/8+112])>>7)&0x1);
		a[i/8+16] <<= 1; a[i/8+48]<<=1; a[i/8+80]<<=1; a[i/8+112]<<=1;
		b[i]= (x<<4)|y;
	}
	memcpy(a,b,128);
}

static inline
void degroup(uint8_t *a){
	uint8_t b[128];
	uint8_t i,j;
	for(i=0;i<128;++i){
		j=i/8;
	    b[j+  0]<<=1; b[j+  0] |= ((a[i])>>7)&1;
	    b[j+ 32]<<=1; b[j+ 32] |= ((a[i])>>6)&1;
	    b[j+ 64]<<=1; b[j+ 64] |= ((a[i])>>5)&1;
	    b[j+ 96]<<=1; b[j+ 96] |= ((a[i])>>4)&1;
	    b[j+ 16]<<=1; b[j+ 16] |= ((a[i])>>3)&1;
	    b[j+ 48]<<=1; b[j+ 48] |= ((a[i])>>2)&1;
	    b[j+ 80]<<=1; b[j+ 80] |= ((a[i])>>1)&1;
	    b[j+112]<<=1; b[j+112] |= ((a[i])>>0)&1;
	}
	memcpy(a,b,128);
}

void jh_encrypt(uint8_t* a){
	uint8_t i;
	/* grouping */
#if DEBUG
	cli_putstr_P(PSTR("\r\n== pre group ==\r\n"));
	cli_hexdump_block(a, 128, 4, 16);
#endif
	group(a);
	for(i=0;i<42;++i){
		jh_round(a, i);
	}

	/* degrouping */
#if DEBUG
	cli_putstr_P(PSTR("\r\n== pre degroup ==\r\n"));
	cli_hexdump_block(a, 128, 4, 16);
#endif
	degroup(a);
#if DEBUG
	cli_putstr_P(PSTR("\r\n== post degroup ==\r\n"));
	cli_hexdump_block(a, 128, 4, 16);
#endif
}


