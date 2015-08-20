/* cast5.c */
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
 * \file	cast5.c
 * \author	Daniel Otte
 * \email       daniel.otte@rub.de
 * \date 	2006-07-26
 * \par License:
 *  GPLv3 or later
 * \brief Implementation of the CAST5 (aka CAST-128) cipher algorithm as described in RFC 2144
 * 
 */
 
#include <stdint.h>
#include <string.h>
#include "cast5.h"
#include <avr/pgmspace.h> 

#undef DEBUG
 
#ifdef DEBUG
 #include "cli.h"
#endif
 
#include "cast5-sbox.h"


 
#define S5(x) pgm_read_dword(&s5[(x)])
#define S6(x) pgm_read_dword(&s6[(x)])
#define S7(x) pgm_read_dword(&s7[(x)])
#define S8(x) pgm_read_dword(&s8[(x)])

static 
void cast5_init_A(uint8_t *dest, uint8_t *src, bool bmode){
	uint8_t mask = bmode?0x8:0;
	*((uint32_t*)(&dest[0x0])) = *((uint32_t*)(&src[0x0^mask]))
                                     ^ S5(src[0xD^mask]) ^ S6(src[0xF^mask]) 
                                     ^ S7(src[0xC^mask]) ^ S8(src[0xE^mask]) 
                                     ^ S7(src[0x8^mask]);
	*((uint32_t*)(&dest[0x4])) = *((uint32_t*)(&src[0x8^mask])) 
                                     ^ S5(dest[0x0]) ^ S6(dest[0x2]) 
                                     ^ S7(dest[0x1]) ^ S8(dest[0x3]) 
                                     ^ S8(src[0xA^mask]);
	*((uint32_t*)(&dest[0x8])) = *((uint32_t*)(&src[0xC^mask])) 
                                     ^ S5(dest[0x7]) ^ S6(dest[0x6]) 
                                     ^ S7(dest[0x5]) ^ S8(dest[0x4]) 
                                     ^ S5(src[0x9^mask]);
	*((uint32_t*)(&dest[0xC])) = *((uint32_t*)(&src[0x4^mask])) 
                                     ^ S5(dest[0xA]) 
                                     ^ S6(dest[0x9]) 
                                     ^ S7(dest[0xB]) 
                                     ^ S8(dest[0x8]) 
                                     ^ S6(src[0xB^mask]);
}

static
void cast5_init_M(uint8_t *dest, uint8_t *src, bool nmode, bool xmode){
	uint8_t nmt[] = {0xB, 0xA, 0x9, 0x8, 
                         0xF, 0xE, 0xD, 0xC, 
                         0x3, 0x2, 0x1, 0x0, 
                         0x7, 0x6, 0x5, 0x4}; /* nmode table */
	uint8_t xmt[4][4] = {{0x2, 0x6, 0x9, 0xC}, 
                             {0x8, 0xD, 0x3, 0x7}, 
                             {0x3, 0x7, 0x8, 0xD}, 
                             {0x9, 0xC, 0x2, 0x6}};
	#define NMT(x) (src[nmode?nmt[(x)]:(x)])
	#define XMT(x) (src[xmt[(xmode<<1) + nmode][(x)]])
	*((uint32_t*)(&dest[0x0])) = S5(NMT(0x8)) ^ S6(NMT(0x9)) ^ S7(NMT(0x7)) ^ S8(NMT(0x6)) ^ S5(XMT(0));
	*((uint32_t*)(&dest[0x4])) = S5(NMT(0xA)) ^ S6(NMT(0xB)) ^ S7(NMT(0x5)) ^ S8(NMT(0x4)) ^ S6(XMT(1));
	*((uint32_t*)(&dest[0x8])) = S5(NMT(0xC)) ^ S6(NMT(0xD)) ^ S7(NMT(0x3)) ^ S8(NMT(0x2)) ^ S7(XMT(2));
	*((uint32_t*)(&dest[0xC])) = S5(NMT(0xE)) ^ S6(NMT(0xF)) ^ S7(NMT(0x1)) ^ S8(NMT(0x0)) ^ S8(XMT(3));
}

#define S5B(x) pgm_read_byte(3+(uint8_t*)(&s5[(x)]))
#define S6B(x) pgm_read_byte(3+(uint8_t*)(&s6[(x)]))
#define S7B(x) pgm_read_byte(3+(uint8_t*)(&s7[(x)]))
#define S8B(x) pgm_read_byte(3+(uint8_t*)(&s8[(x)]))

static
void cast5_init_rM(uint8_t *klo, uint8_t *khi, uint8_t offset, uint8_t *src, bool nmode, bool xmode){
	uint8_t nmt[] = {0xB, 0xA, 0x9, 0x8, 0xF, 0xE, 0xD, 0xC, 0x3, 0x2, 0x1, 0x0, 0x7, 0x6, 0x5, 0x4}; /* nmode table */
	uint8_t xmt[4][4] = {{0x2, 0x6, 0x9, 0xC}, {0x8, 0xD, 0x3, 0x7}, {0x3, 0x7, 0x8, 0xD}, {0x9, 0xC, 0x2, 0x6}};
	uint8_t t, h=0; 
	t = S5B(NMT(0x8)) ^ S6B(NMT(0x9)) ^ S7B(NMT(0x7)) ^ S8B(NMT(0x6)) ^ S5B(XMT(0));
		klo[offset*2] |= (t & 0x0f);
		h |= (t&0x10); h>>=1;
	t = S5B(NMT(0xA)) ^ S6B(NMT(0xB)) ^ S7B(NMT(0x5)) ^ S8B(NMT(0x4)) ^ S6B(XMT(1));
		klo[offset*2] |= (t<<4) & 0xf0;
		h |= t&0x10; h>>=1;
	t = S5B(NMT(0xC)) ^ S6B(NMT(0xD)) ^ S7B(NMT(0x3)) ^ S8B(NMT(0x2)) ^ S7B(XMT(2));
		klo[offset*2+1] |= t&0xf;
		h |= t&0x10; h>>=1;
	t = S5B(NMT(0xE)) ^ S6B(NMT(0xF)) ^ S7B(NMT(0x1)) ^ S8B(NMT(0x0)) ^ S8B(XMT(3));
		klo[offset*2+1] |= t<<4;
		h |= t&0x10; h >>=1;
	#ifdef DEBUG
		cli_putstr("\r\n\t h="); cli_hexdump(&h,1);
	#endif
	khi[offset>>1] |= h<<((offset&0x1)?4:0);
}

#define S_5X(s) pgm_read_dword(&s5[BPX[(s)]])
#define S_6X(s) pgm_read_dword(&s6[BPX[(s)]])
#define S_7X(s) pgm_read_dword(&s7[BPX[(s)]])
#define S_8X(s) pgm_read_dword(&s8[BPX[(s)]])

#define S_5Z(s) pgm_read_dword(&s5[BPZ[(s)]])
#define S_6Z(s) pgm_read_dword(&s6[BPZ[(s)]])
#define S_7Z(s) pgm_read_dword(&s7[BPZ[(s)]])
#define S_8Z(s) pgm_read_dword(&s8[BPZ[(s)]])




void cast5_init(const void* key, uint16_t keylength_b, cast5_ctx_t* s){
 	 /* we migth return if the key is valid and if setup was successful */
	uint32_t x[4], z[4];
	#define BPX ((uint8_t*)&(x[0]))
	#define BPZ ((uint8_t*)&(z[0]))
	s->shortkey = (keylength_b<=80);
	/* littel endian only! */
	memset(&(x[0]), 0 ,16); /* set x to zero */
	if(keylength_b > 128)
		keylength_b=128;
	memcpy(&(x[0]), key, (keylength_b+7)/8);
	

	/* todo: merge a and b and compress the whole stuff */
	/***** A *****/
	cast5_init_A((uint8_t*)(&z[0]), (uint8_t*)(&x[0]), false);	
	/***** M *****/
	cast5_init_M((uint8_t*)(&(s->mask[0])), (uint8_t*)(&z[0]), false, false);
	/***** B *****/
	cast5_init_A((uint8_t*)(&x[0]), (uint8_t*)(&z[0]), true);
	/***** N *****/
	cast5_init_M((uint8_t*)(&(s->mask[4])), (uint8_t*)(&x[0]), true, false);
	/***** A *****/
	cast5_init_A((uint8_t*)(&z[0]), (uint8_t*)(&x[0]), false);
	/***** N' *****/
	cast5_init_M((uint8_t*)(&(s->mask[8])), (uint8_t*)(&z[0]), true, true);
	/***** B *****/
	cast5_init_A((uint8_t*)(&x[0]), (uint8_t*)(&z[0]), true);
	/***** M' *****/
	cast5_init_M((uint8_t*)(&(s->mask[12])), (uint8_t*)(&x[0]), false, true);
	
	/* that were the masking keys, now the rotation keys */
	/* set the keys to zero */
	memset(&(s->rotl[0]),0,8);
	s->roth[0]=s->roth[1]=0;
	/***** A *****/
	cast5_init_A((uint8_t*)(&z[0]), (uint8_t*)(&x[0]), false);
	/***** M *****/
	cast5_init_rM(&(s->rotl[0]), &(s->roth[0]), 0, (uint8_t*)(&z[0]), false, false);
	/***** B *****/
	cast5_init_A((uint8_t*)(&x[0]), (uint8_t*)(&z[0]), true);
	/***** N *****/
	cast5_init_rM(&(s->rotl[0]), &(s->roth[0]), 1, (uint8_t*)(&x[0]), true, false);
	/***** A *****/
	cast5_init_A((uint8_t*)(&z[0]), (uint8_t*)(&x[0]), false);
	/***** N' *****/
	cast5_init_rM(&(s->rotl[0]), &(s->roth[0]), 2, (uint8_t*)(&z[0]), true, true);
	/***** B *****/
	cast5_init_A((uint8_t*)(&x[0]), (uint8_t*)(&z[0]), true);
	/***** M' *****/
	cast5_init_rM(&(s->rotl[0]), &(s->roth[0]), 3, (uint8_t*)(&x[0]), false, true);
	/* done ;-) */
}



/********************************************************************************************************/

#define ROTL32(a,n) ((a)<<(n) | (a)>>(32-(n)))
#define CHANGE_ENDIAN32(x) ((x)<<24 | (x)>>24 | ((x)&0xff00)<<8 | ((x)&0xff0000)>>8 )

typedef uint32_t cast5_f_t(uint32_t,uint32_t,uint8_t);

#define IA 3
#define IB 2
#define IC 1
#define ID 0

static
uint32_t cast5_f1(uint32_t d, uint32_t m, uint8_t r){
	uint32_t t;
	t = ROTL32((d + m),r);
#ifdef DEBUG
	uint32_t ia,ib,ic,id;
	cli_putstr("\r\n f1("); cli_hexdump(&d, 4); cli_putc(',');
		cli_hexdump(&m , 4); cli_putc(','); cli_hexdump(&r, 1);cli_putstr("): I=");
		cli_hexdump(&t, 4);
	ia = pgm_read_dword(&s1[((uint8_t*)&t)[IA]] );
	ib = pgm_read_dword(&s2[((uint8_t*)&t)[IB]] );
	ic = pgm_read_dword(&s3[((uint8_t*)&t)[IC]] );
	id = pgm_read_dword(&s4[((uint8_t*)&t)[ID]] );
	cli_putstr("\r\n\tIA="); cli_hexdump(&ia, 4);
	cli_putstr("\r\n\tIB="); cli_hexdump(&ib, 4);
	cli_putstr("\r\n\tIC="); cli_hexdump(&ic, 4);
	cli_putstr("\r\n\tID="); cli_hexdump(&id, 4);

	return (((ia ^ ib) - ic) + id);

#else
	
	return (((  pgm_read_dword(&s1[((uint8_t*)&t)[IA]]) 
                  ^ pgm_read_dword(&s2[((uint8_t*)&t)[IB]]) ) 
		  - pgm_read_dword(&s3[((uint8_t*)&t)[IC]]) ) 
                  + pgm_read_dword(&s4[((uint8_t*)&t)[ID]]) );

#endif
}

static
uint32_t cast5_f2(uint32_t d, uint32_t m, uint8_t r){
	uint32_t t;
	t = ROTL32((d ^ m),r);
#ifdef DEBUG
	uint32_t ia,ib,ic,id;
	cli_putstr("\r\n f2("); cli_hexdump(&d, 4); cli_putc(',');
		cli_hexdump(&m , 4); cli_putc(','); cli_hexdump(&r, 1);cli_putstr("): I=");
		cli_hexdump(&t, 4);

	ia = pgm_read_dword(&s1[((uint8_t*)&t)[IA]] );
	ib = pgm_read_dword(&s2[((uint8_t*)&t)[IB]] );
	ic = pgm_read_dword(&s3[((uint8_t*)&t)[IC]] );
	id = pgm_read_dword(&s4[((uint8_t*)&t)[ID]] );
	
	cli_putstr("\r\n\tIA="); cli_hexdump(&ia, 4);
	cli_putstr("\r\n\tIB="); cli_hexdump(&ib, 4);
	cli_putstr("\r\n\tIC="); cli_hexdump(&ic, 4);
	cli_putstr("\r\n\tID="); cli_hexdump(&id, 4);

	return (((ia - ib) + ic) ^ id);
#else
	
	return (((    pgm_read_dword(&s1[((uint8_t*)&t)[IA]]) 
	            - pgm_read_dword(&s2[((uint8_t*)&t)[IB]]) ) 
		    + pgm_read_dword(&s3[((uint8_t*)&t)[IC]]) ) 
		    ^ pgm_read_dword(&s4[((uint8_t*)&t)[ID]]) );

#endif
}

static
uint32_t cast5_f3(uint32_t d, uint32_t m, uint8_t r){
	uint32_t t;
	t = ROTL32((m - d),r);

#ifdef DEBUG
	uint32_t ia,ib,ic,id;

	cli_putstr("\r\n f3("); cli_hexdump(&d, 4); cli_putc(',');
		cli_hexdump(&m , 4); cli_putc(','); cli_hexdump(&r, 1);cli_putstr("): I=");
		cli_hexdump(&t, 4);

	ia = pgm_read_dword(&s1[((uint8_t*)&t)[IA]] );
	ib = pgm_read_dword(&s2[((uint8_t*)&t)[IB]] );
	ic = pgm_read_dword(&s3[((uint8_t*)&t)[IC]] );
	id = pgm_read_dword(&s4[((uint8_t*)&t)[ID]] );
	
	cli_putstr("\r\n\tIA="); cli_hexdump(&ia, 4);
	cli_putstr("\r\n\tIB="); cli_hexdump(&ib, 4);
	cli_putstr("\r\n\tIC="); cli_hexdump(&ic, 4);
	cli_putstr("\r\n\tID="); cli_hexdump(&id, 4);
	return (((ia + ib) ^ ic) - id);
#else
	return ((  pgm_read_dword(&s1[((uint8_t*)&t)[IA]] )
                 + pgm_read_dword(&s2[((uint8_t*)&t)[IB]] )) 
		 ^ pgm_read_dword(&s3[((uint8_t*)&t)[IC]] )) 
                 - pgm_read_dword(&s4[((uint8_t*)&t)[ID]] );

#endif
}

/******************************************************************************/

void cast5_enc(void* block, const cast5_ctx_t *s){
	uint32_t l,r, x, y;
	uint8_t i;
	cast5_f_t* f[]={cast5_f1,cast5_f2,cast5_f3};
	l=((uint32_t*)block)[0];
	r=((uint32_t*)block)[1];
//	cli_putstr("\r\n round[-1] = ");
//	cli_hexdump(&r, 4);
	for (i=0;i<(s->shortkey?12:16);++i){
		x = r;
		y = (f[i%3])(CHANGE_ENDIAN32(r), CHANGE_ENDIAN32(s->mask[i]), 
			(((s->roth[i>>3]) & (1<<(i&0x7)))?0x10:0x00) 
			 + ( ((s->rotl[i>>1])>>((i&1)?4:0)) & 0x0f) );
		r = l ^ CHANGE_ENDIAN32(y);
//		cli_putstr("\r\n round["); DEBUG_B(i); cli_putstr("] = ");
//		cli_hexdump(&r, 4);
		l = x;
	}
	((uint32_t*)block)[0]=r;
	((uint32_t*)block)[1]=l;
}

/******************************************************************************/

void cast5_dec(void* block, const cast5_ctx_t *s){
	uint32_t l,r, x, y;
	int8_t i, rounds;
	cast5_f_t* f[]={cast5_f1,cast5_f2,cast5_f3};
	l=((uint32_t*)block)[0];
	r=((uint32_t*)block)[1];
	rounds = (s->shortkey?12:16);
	for (i=rounds-1; i>=0 ;--i){
		x = r;
		y = (f[i%3])(CHANGE_ENDIAN32(r), CHANGE_ENDIAN32(s->mask[i]), 
			(((s->roth[i>>3]) & (1<<(i&0x7)))?0x10:0x00) 
			 + ( ((s->rotl[i>>1])>>((i&1)?4:0)) & 0x0f) );
		r = l ^ CHANGE_ENDIAN32(y);
		l = x;
	}
	((uint32_t*)block)[0]=r;
	((uint32_t*)block)[1]=l;
}


/******************************************************************************/




