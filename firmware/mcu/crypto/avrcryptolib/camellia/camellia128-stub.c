/* camellia128-stub.c */
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
 * 
 * 
 * 
 */
 
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "camellia.h"
#if 0
 #include "cli.h"
 #include "debug.h"
 #include <util/delay.h>
#endif
/*****************************************************************************/
uint64_t camellia_f(uint64_t x, uint64_t k);
/*****************************************************************************/
uint64_t camellia_fl(uint64_t x, uint64_t k);
/*****************************************************************************/
uint64_t camellia_fl_inv(uint64_t y, uint64_t k);
/*****************************************************************************/
void change_endian(void* data, uint8_t length);
/*
uint64_t PROGMEM camellia_sigma[6]={ / * 64 byte table * /
	0xA09E667F3BCC908BLL,
	0xB67AE8584CAA73B2LL,
	0xC6EF372FE94F82BELL,
	0x54FF53A5F1D36F1CLL,
	0x10E527FADE682D1DLL,
	0xB05688C2B3E6C1FDLL
};	
*/
const uint32_t camellia_sigma[12] PROGMEM = { /* 48 byte table */
         0x3BCC908BL, 0xA09E667FL,
         0x4CAA73B2L, 0xB67AE858L,
         0xE94F82BEL, 0xC6EF372FL,
         0xF1D36F1CL, 0x54FF53A5L,
         0xDE682D1DL, 0x10E527FAL,
         0xB3E6C1FDL, 0xB05688C2L
};

/* an ugly macro to load an entry form the table above */
/*
#define SIGMA(p) (( ((uint64_t)(pgm_read_dword((uint32_t*)camellia_sigma+2*(p)+1)))<<32) | \
                    ((uint64_t)(pgm_read_dword((uint32_t*)camellia_sigma+2*(p)+0))) )
*/
#define SIGMA(p) (( ((uint64_t)(pgm_read_dword(&(((uint32_t*)camellia_sigma)[2*(p)+1]))))<<32) | \
                    ((uint64_t)(pgm_read_dword(&(((uint32_t*)camellia_sigma)[2*(p)+0])))) )



/*****************************************************************************/
/*
void camellia128_ctx_dump(camellia128_ctx_t *s){
	cli_putstr_P(PSTR("\r\n==State Dump=="));
	cli_putstr_P(PSTR("\n\rKAl: ")); cli_hexdump(&(s->kal), 8);
	cli_putstr_P(PSTR("\n\rKAr: ")); cli_hexdump(&(s->kar), 8);
	cli_putstr_P(PSTR("\n\rKLl: ")); cli_hexdump(&(s->kll), 8);
	cli_putstr_P(PSTR("\n\rKLr: ")); cli_hexdump(&(s->klr), 8);	
	return;
}
*/
/*****************************************************************************/
/* extern uint64_t camellia_sigma[6]; */

void camellia128_init(const void* key, camellia128_ctx_t* s){
	uint8_t i;
	s->kll = 0; /* ((uint64_t*)key)[0]; */
	
	/* load the key, endian-adjusted, to kll,klr */
	for(i=0; i<8; ++i){
		s->kll <<= 8;
		s->kll |= *((uint8_t*)key);
		key = (uint8_t*)key+1;
	}
	for(i=0; i<8; ++i){
		s->klr <<= 8;
		s->klr |= *((uint8_t*)key);
		key = (uint8_t*)key+1;
	}

	s->kal = s->kll;
	s->kar = s->klr;
	
	s->kar ^= camellia_f(s->kal, SIGMA(0));
	s->kal ^= camellia_f(s->kar, SIGMA(1));
	
	s->kal ^= s->kll;
	s->kar ^= s->klr;
	
	s->kar ^= camellia_f(s->kal, SIGMA(2));
	s->kal ^= camellia_f(s->kar, SIGMA(3));
}

/*****************************************************************************/
void camellia128_keyop(camellia128_ctx_t* s, int8_t q);
/*****************************************************************************/
void camellia128_keyop_inv(camellia128_ctx_t* s, int8_t q); 
/*****************************************************************************/

#define SEL_KA 1
#define SEL_KL 0

#define KEY_POSTC1		0x00
#define KEY_POSTC2		0x01
#define KEY_INC2		0x02

#define KEY_DIR			0x04
#define KEY_DIR_NORM	0x00
#define KEY_DIR_INV		0x04

#define KEY_AMMOUNT		0x08 
#define KEY_ROL17		0x08
#define KEY_ROL15		0x00

void camellia_6rounds(const camellia128_ctx_t* s, uint64_t* bl, uint64_t* br, 
                      uint8_t roundop, uint8_t keychoice);
/*****************************************************************************/


void camellia128_enc(void* block, const camellia128_ctx_t* s){

	#define BL (((uint64_t*)block)[0])
	#define BR (((uint64_t*)block)[1])
	/* endian adjustment */
	 /*BL*/
	 /* 1 2 3 4 5 6 7 8
	  *	8 7 6 5 4 3 2 1
	  */
	 
	uint64_t temp64;
	
	change_endian(&BL, 64/8);	
	change_endian(&BR, 64/8);
	
	/* Prewhitening */
	BL ^= s->kll;
	BR ^= s->klr;
	
	/* the first 6 */
	camellia_6rounds(s, &BL, &BR, KEY_ROL15 | KEY_DIR_NORM | KEY_POSTC1 , 0x33);
	/* FL injection  */
   camellia128_keyop((camellia128_ctx_t*)s, -1);
	BL = camellia_fl(BL, s->kal);
	BR = camellia_fl_inv(BR, s->kar);
   camellia128_keyop((camellia128_ctx_t*)s, -1);
	/* middle 6 */
	camellia_6rounds(s, &BL, &BR, KEY_ROL15 | KEY_DIR_NORM | KEY_INC2 , 0x34);
	/* FL injection  */
   camellia128_keyop((camellia128_ctx_t*)s, 1);
   	BL = camellia_fl(BL, s->kll);
	BR = camellia_fl_inv(BR, s->klr);
   camellia128_keyop((camellia128_ctx_t*)s, 1);
   /* last 6 */
	camellia_6rounds(s, &BL, &BR, KEY_ROL17 | KEY_DIR_NORM | KEY_POSTC2 , 0x0C);
	/* Postwhitening */
	BR ^= s->kal;
	BL ^= s->kar;
	
	temp64 = BR;
	BR = BL;
	BL = temp64;

	camellia128_keyop((camellia128_ctx_t*)s,1);
	
	change_endian(&BL, 64/8);	
	change_endian(&BR, 64/8);
		
	#undef BL
	#undef BR	
}

/*****************************************************************************/

void camellia128_dec(void* block, const camellia128_ctx_t* s){

	#define BL (((uint64_t*)block)[1])
	#define BR (((uint64_t*)block)[0])
	/* endian adjustment */
	 /*BL*/
	 /* 1 2 3 4 5 6 7 8
	  * 8 7 6 5 4 3 2 1
	  */
	 
	uint64_t temp64;
		
	change_endian(&BL, 64/8);	
	change_endian(&BR, 64/8);
		
	camellia128_keyop_inv((camellia128_ctx_t*)s, 1);
	/* Prewhitening */
	BR ^= s->kal; /* kw3 */
	BL ^= s->kar; /* kw4 */
	/* the first 6 */
	camellia_6rounds(s, &BR, &BL, KEY_ROL17 | KEY_DIR_INV | KEY_POSTC1 , 0x0C);
	/* FL injection  */
   camellia128_keyop_inv((camellia128_ctx_t*)s, 1);
	BR = camellia_fl(BR, s->klr);
	BL = camellia_fl_inv(BL, s->kll);
   camellia128_keyop_inv((camellia128_ctx_t*)s, 1);
	/* middle 6 */	
	camellia_6rounds(s, &BR, &BL, KEY_ROL15 | KEY_DIR_INV | KEY_INC2 , 0x0B);
	/* FL injection  */
   camellia128_keyop_inv((camellia128_ctx_t*)s, -1);
   	BR = camellia_fl(BR, s->kar);
	BL = camellia_fl_inv(BL, s->kal);
   camellia128_keyop_inv((camellia128_ctx_t*)s, -1);
   /* last 6 */
	camellia_6rounds(s, &BR, &BL, KEY_ROL15 | KEY_DIR_INV | KEY_POSTC2 , 0x33);
	
	/* Postwhitening */
	BL ^= s->kll; /* kw1 */ 
	BR ^= s->klr; /* kw2 */
	
	temp64 = BR;
	BR = BL;
	BL = temp64;
	
	change_endian(&BL, 64/8);	
	change_endian(&BR, 64/8);
		
}

/*****************************************************************************/
/*****************************************************************************/



/* EOF */
