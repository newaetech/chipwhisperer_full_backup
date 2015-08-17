/**
 * 
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 * 
 * */

#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "mickey128.h"
#include "cli.h"
/*
RTAPS = { 0,4,5,  8,10,11,14  ,16,20,  25,30,  32,35,36,38,
         42,43,46,  50,51,53,54,55,  56,57,60,61,62,63,  
         65,66,69,  73,74,76,79,  80,81,82,85,86,  90,91,92,95,  
         97,100,101,  105,106,107,108,109,111,  112,113,115,116,117,
         127,  128,129,130,131,133,135,  136,137,140,142,  145,148,150,  
         152,153,154,156,157 }
         
         0011.0001  0100.1101  0001.0001  0100.0010  0101.1001
         0100.1100  1110.1100  1111.0011 
         0010.0110  1001.0110  0110.0111  1001.1100 
         0011.0010  1011.1110  0011.1011
         1000.0000  1010.1111  0101.0011  0101.0010
         0011.0111
          
         1000110010110010100010000100001010011010001100100011011111001111011001
         0001101001111001100011100101001100011111011101110000000001111101011100
         101001001010111011
          
         1000.1100 1011.0010 1000.1000 0100.0010 1001.1010 0011.0010 
         0011.0111 1100.1111 0110.0100 0110.1001 1110.0110 0011.1001
         0100.1100 0111.1101 1101.1100 0000.0001 1111.0101 1100.1010
         0100.1010 1110.11
*/

const uint8_t rtaps[] PROGMEM = {
	0x31, 0x4D, 0x11, 0x42,
	0x59, 0x4C, 0xEC, 0xF3,
	0x26, 0x96, 0x67, 0x9C,
	0x32, 0xBE, 0x3B, 0x80, 
	0xAF, 0x53, 0x52, 0x37
};
	
static 
void memxor_P(void* d, PGM_VOID_P s, uint8_t length_B){
	while(length_B--){
		*((uint8_t*)d) ^= pgm_read_byte(s);
		d = (uint8_t*)d +1;
		s = (uint8_t*)s +1;
	}
}

#define SHL0(a) c1=((a)>>7); (a)=(((a)<<1)|c0)
#define SHL1(a) c0=((a)>>7); (a)=(((a)<<1)|c1)

#define SHLX0(a) c1=((a)>>7); (a)^=(((a)<<1)|c0)
#define SHLX1(a) c0=((a)>>7); (a)^=(((a)<<1)|c1)

static
void clock_r(uint8_t* r, uint8_t ibit, uint8_t cbit){
	uint8_t i,c0=0,c1=0; /* carry */
	ibit ^= ((r[159/8])>>(159%8))&1; /* ibit is now the same as feedback_bit */
	if(cbit){
		for(i=0; i<10; ++i){
			SHLX0(r[2*i+0]);
			SHLX1(r[2*i+1]);
		}
	} else {
		for(i=0; i<10; ++i){
			SHL0(r[2*i+0]);
			SHL1(r[2*i+1]);
		}
	}
	if(ibit){
		memxor_P(r, rtaps, 20);
	}
}

/* comp0 (filling spaces with zero) (one at each side)
 * 0101.1110 1111.0010 1101.0110 0101.1101 
 * 0101.0101 0000.1001 0010.0110 0111.1001
 * 0110.0010 0111.0000 0000.0000 0111.1001
 * 0011.0001 1101.1001 1010.1111 0011.0111
 * 1011.1110 0000.0110 1011.1110 0000.1111
 * --
 * 5E F2 D6 5D
 * 55 09 26 79
 * 62 70 00 79
 * 31 D9 AF 37
 * BE 06 BE 0F
 */ 
const uint8_t comp0[] PROGMEM = {
	0x5E, 0xF2, 0xD6, 0x5D,
	0x55, 0x09, 0x26, 0x79,
	0x62, 0x70, 0x00, 0x79, 
	0x31, 0xD9, 0xAF, 0x37, 
	0xBE, 0x06, 0xBE, 0x0F
};


/* comp1 (shifting one bit right to make calculation easier, so inserting two zeros)
 * 0110.0000 0011.1110 0011.0010 1111.1010 
 * 0011.0000 0111.1001 0110.1100 1111.1101
 * 1100.0001 1000.0111 0000.0001 1111.1000
 * 1000.1010 1100.0110 1100.0001 1100.1100
 * 0110.1010 1011.0111 1110.1000 1111.1111
 * --
 * 60 3E 32 FA
 * 30 79 6C FD
 * C1 87 01 F8
 * 8A C6 C1 CC
 * 6A B7 E8 FF
*/
/*
const uint8_t comp1[] PROGMEM = {
	0x60, 0x3E, 0x32, 0xFA, 0x30, 0x79, 0x6C, 0xFD, 0xC1, 0x87, 
	0x01, 0xF8, 0x8A, 0xC6, 0xC1, 0xCC, 0x6A, 0xB7, 0xE8, 0xFF
};
*/
/* comp1
 * 0000.1100 1111.1000 1001.1000 1011.1110
 * 0001.1001 0011.1100 0110.1101 0111.1111
 * 0000.0111 1100.0011 0000.0000 0011.1110
 * 1010.0010 1100.0111 0000.0110 0110.0110
 * 1010.1101 1101.1010 0010.1111 1111.1110
 * --
 * 0C F8 98 BE
 * 19 3C 6D 7F
 * 07 C3 00 3E
 * A2 C7 06 66
 * AD DA 2F FE
*/
/*
const uint8_t comp1[] PROGMEM = {
	0x0C, 0xF8, 0x98, 0xBE, 0x19, 0x3C, 0x6D, 0x7F, 0x07, 0xC3,
	0x00, 0x3E, 0xA2, 0xC7, 0x06, 0x66, 0xAD, 0xDA, 0x2F, 0xFE
};
*/
/* comp1
 * 0011.0000 0001.1111 0001.1001 0111.1101
 * 1001.1000 0011.1100 1011.0110 1111.1110
 * 1110.0000 1100.0011 0000.0000 0111.1100
 * 0100.0101 1110.0011 0110.0000 0110.0110
 * 1011.0101 0101.1011 1111.0100 0111.1111
 * --
 * 30 1F 19 7D
 * 98 3C B6 FE
 * E0 C3 00 7C
 * 45 E3 60 66
 * B5 5B F4 7F
*/

const uint8_t comp1[] PROGMEM = {
	0x30, 0x1F, 0x19, 0x7D,
	0x98, 0x3C, 0xB6, 0xFE, 
	0xE0, 0xC3, 0x00, 0x7C, 
	0x45, 0xE3, 0x60, 0x66, 
	0xB5, 0x5B, 0xF4, 0x7F
};

/* fb0
 * 1010.1111 0001.1111 0011.1100 1100.0100
 * 0010.0010 1010.0011 0010.1111 0000.1110
 * 1000.0001 0100.1101 1110.0101 0110.0110
 * 1001.0001 0100.1011 0101.0100 1101.0100
 * 1100.0001 0000.1011 0110.0011 1000.0011
 * --
 * AF 1F 3C C4
 * 22 A3 2F 0E
 * 81 4D E5 66
 * 91 4B 54 D4
 * C1 0B 63 83
 */ 
const uint8_t fb0[] PROGMEM = {	
	0xAF, 0x1F, 0x3C, 0xC4, 
	0x22, 0xA3, 0x2F, 0x0E, 
	0x81, 0x4D, 0xE5, 0x66, 
	0x91, 0x4B, 0x54, 0xD4, 
	0xC1, 0x0B, 0x63, 0x83
};

/* fb1
 * 1010.1011 0111.0111 1111.0100 1001.1011 
 * 1001.0000 1000.1100 0111.1001 0111.0000
 * 1011.0110 0001.1000 1001.1010 0110.1111
 * 1110.0111 0111.1110 0100.1011 0110.1100 
 * 1110.1111 1000.0000 1010.0111 0001.0001
 * --
 * AB 77 F4 9B
 * 90 8C 79 70
 * B6 18 9A 6F
 * E7 7E 4B 6C
 * EF 80 A7 11
 */ 
const uint8_t fb1[] PROGMEM = {
	0xAB, 0x77, 0xF4, 0x9B, 
	0x90, 0x8C, 0x79, 0x70, 
	0xB6, 0x18, 0x9A, 0x6F, 
	0xE7, 0x7E, 0x4B, 0x6C, 
	0xEF, 0x80, 0xA7, 0x11
};

static
void clock_s(uint8_t* s, uint8_t ibit, uint8_t cbit){
	uint8_t s0[20], s1[20];
	uint8_t i,c=0, c2=0;
	ibit ^= (s[19])>>7;
	memcpy(s0,s,20);
	memxor_P(s0, comp0, 20);
	for(i=0; i<20; ++i){
		s1[19-i]= c|((s[19-i])>>1);
		c = (s[19-i])<<7;
	}
	memxor_P(s1, comp1, 20);
	c=0;
	for(i=0; i<20; ++i){
		c2=(s[i])>>7;
		s[i]=((s[i])<<1) ^ ((s0[i])&(s1[i])) ^ c;
		c=c2;
	}
	s[0] &= 0xFE;
	if(ibit){
		memxor_P(s, cbit?fb1:fb0, 20);
	}
}

static
void clock_kg(uint8_t* r, uint8_t* s, uint8_t mixing, uint8_t input){
	uint8_t rb, sb;
	rb = ((s[ 54/8])>>(( 54%8))) ^ ((r[106/8])>>(((106%8))));
	sb = ((s[106/8])>>((106%8))) ^ ((r[ 53/8])>>((( 53%8))));
	rb &= 1;
	sb &= 1;
	mixing = input ^ (mixing & ((s[80/8]>>((80%8))) & 1)); 
	clock_r(r, mixing, rb);
	clock_s(s, input, sb);
}

void mickey128_init(void* key, uint16_t keysize_b, 
                    void* iv,  uint16_t ivsize_b, 
                    mickey128_ctx_t* ctx){
	uint16_t i;
	memset(ctx->r, 0, 20);
	memset(ctx->s, 0, 20);
	for(i=0; i<ivsize_b; ++i){
		clock_kg(ctx->r, ctx->s, 1, 1&((((uint8_t*)iv)[i/8])>>(7-(i%8))));
	}
	for(i=0; i<keysize_b; ++i){
		clock_kg(ctx->r, ctx->s, 1, 1&((((uint8_t*)key)[i/8])>>(7-(i%8))));
	}                  
	for(i=0; i<160; ++i){
		clock_kg(ctx->r, ctx->s, 1, 0);
	}                  
} 

uint8_t mickey128_getbit(mickey128_ctx_t* ctx){
	uint8_t ret;
	ret = 1&(*(ctx->r) ^ *(ctx->s));
	clock_kg(ctx->r, ctx->s, 0, 0);
	return ret;
}

uint8_t mickey128_getbyte(mickey128_ctx_t* ctx){
	uint8_t i,ret=0;
	for(i=0; i<8; ++i){
		ret<<=1;
		ret |= 1&(((ctx->r)[0]) ^ ((ctx->s)[0]));
		clock_kg(ctx->r, ctx->s, 0, 0);
	}
	return ret;
}

		
