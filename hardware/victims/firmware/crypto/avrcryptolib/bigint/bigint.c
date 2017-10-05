/* bigint.c */
/*
    This file is part of the ARM-Crypto-Lib.
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
 * \file		bigint.c
 * \author		Daniel Otte
 * \date		2010-02-22
 * 
 * \license	    GPLv3 or later
 * 
 */
 

#define STRING2(x) #x
#define STRING(x) STRING2(x)
#define STR_LINE STRING(__LINE__)

#include "bigint.h"
#include <string.h>

#ifndef PRINT_STATUS
#define PRINT_STATUS 0
#endif

#if PRINT_STATUS
#include <avr/io.h>
#include "hal.h"
#endif

#define DEBUG 0

#if DEBUG
#include "cli.h"
#include "bigint_io.h"
#endif

#ifndef MAX
 #define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
 #define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define SET_FBS(a, v) do{(a)->info &=~BIGINT_FBS_MASK; (a)->info |= (v);}while(0)
#define GET_FBS(a)   ((a)->info&BIGINT_FBS_MASK)
#define SET_NEG(a)   (a)->info |= BIGINT_NEG_MASK
#define SET_POS(a)   (a)->info &= ~BIGINT_NEG_MASK
#define XCHG(a,b)    do{(a)^=(b); (b)^=(a); (a)^=(b);}while(0)
#define XCHG_PTR(a,b)    do{ a = (void*)(((intptr_t)(a)) ^ ((intptr_t)(b))); \
	                         b = (void*)(((intptr_t)(a)) ^ ((intptr_t)(b))); \
	                         a = (void*)(((intptr_t)(a)) ^ ((intptr_t)(b)));}while(0)

#define GET_SIGN(a) ((a)->info&BIGINT_NEG_MASK)

/******************************************************************************/
void bigint_adjust(bigint_t* a){
	while(a->length_B!=0 && a->wordv[a->length_B-1]==0){
		a->length_B--;
	}
	if(a->length_B==0){
		a->info=0;
		return;
	}
	bigint_word_t t;
	uint8_t i = BIGINT_WORD_SIZE-1;
	t = a->wordv[a->length_B-1];
	while((t&(1L<<(BIGINT_WORD_SIZE-1)))==0 && i){
		t<<=1;
		i--;
	}
	SET_FBS(a, i);
}

/******************************************************************************/

uint16_t bigint_length_b(const bigint_t* a){
	if(!a->length_B || a->length_B==0){
		return 0;
	}
	return (a->length_B-1) * BIGINT_WORD_SIZE + GET_FBS(a);
}

/******************************************************************************/

uint16_t bigint_length_B(const bigint_t* a){
	return a->length_B * sizeof(bigint_word_t);
}

/******************************************************************************/

uint32_t bigint_get_first_set_bit(const bigint_t* a){
	if(a->length_B==0){
		return (uint32_t)(-1);
	}
	return (a->length_B-1)*sizeof(bigint_word_t)*8+GET_FBS(a);
}


/******************************************************************************/

uint32_t bigint_get_last_set_bit(const bigint_t* a){
	uint32_t r=0;
	uint8_t b=0;
	bigint_word_t x=1;
	if(a->length_B==0){
		return (uint32_t)(-1);
	}
	while(a->wordv[r]==0 && r<a->length_B){
		++r;
	}
	if(a->wordv[r] == 0){
		return (uint32_t)(-1);
	}
	while((x&a->wordv[r])==0){
		++b;
		x <<= 1;
	}
	return r*BIGINT_WORD_SIZE+b;
}

/******************************************************************************/

void bigint_copy(bigint_t* dest, const bigint_t* src){
	memcpy(dest->wordv, src->wordv, src->length_B*sizeof(bigint_word_t));
	dest->length_B = src->length_B;
	dest->info = src->info;
}

/******************************************************************************/

/* this should be implemented in assembly */
void bigint_add_u(bigint_t* dest, const bigint_t* a, const bigint_t* b){
	uint16_t i;
	bigint_wordplus_t t = 0LL;
	if(a->length_B < b->length_B){
		XCHG_PTR(a,b);
	}
	for(i = 0; i < b->length_B; ++i){
		t += a->wordv[i];
		t += b->wordv[i];
		dest->wordv[i] = (bigint_word_t)t;
		t >>= BIGINT_WORD_SIZE;
	}
	for(; i < a->length_B; ++i){
		t += a->wordv[i];
		dest->wordv[i] = (bigint_word_t)t;
		t >>= BIGINT_WORD_SIZE;
	}
	if(t){
		dest->wordv[i++] = (bigint_word_t)t;
	}
	dest->length_B = i;
	bigint_adjust(dest);
}

/******************************************************************************/

/* this should be implemented in assembly */
void bigint_add_scale_u(bigint_t* dest, const bigint_t* a, uint16_t scale){
	if(a->length_B == 0){
		return;
	}
	if(scale == 0){
		bigint_add_u(dest, dest, a);
		return;
	}
#if DEBUG_ADD_SCALE
	cli_putstr_P(PSTR("\r\nDBG: bigint_add_scale("));
	bigint_print_hex(dest);
	cli_putc(',');
	bigint_print_hex(dest);
	cli_putc(',');
	cli_hexdump_rev(&scale, 2);
	cli_putc(')');
#endif
#if BIGINT_WORD_SIZE == 8
	if(scale >= dest->length_B){
#if DEBUG_ADD_SCALE
		cli_putstr_P(PSTR("\r\n\tpath one"));
#endif
		memset(dest->wordv + dest->length_B, 0, scale - dest->length_B);
		memcpy(dest->wordv + scale, a->wordv, a->length_B);
		dest->info = a->info;
		dest->length_B = a->length_B + scale;
		return;
	}
	bigint_t x;
#if DEBUG_ADD_SCALE
	cli_putstr_P(PSTR("\r\n\tpath two"));
#endif
	x.length_B = dest->length_B - scale;
	x.info = dest->info;
	x.wordv = dest->wordv + scale;
	bigint_add_u(&x, &x, a);
	dest->length_B = x.length_B + scale;
	dest->info = 0;
	bigint_adjust(dest);
#else
	bigint_t s;
	uint16_t word_shift = scale / sizeof(bigint_word_t), byte_shift = scale % sizeof(bigint_word_t);
	bigint_word_t bv[a->length_B + 1];
	s.wordv = bv;
	bv[0] = bv[a->length_B] = 0;
	memcpy((uint8_t*)bv + byte_shift, a->wordv, a->length_B * sizeof(bigint_word_t));
	s.length_B = a->length_B + 1;
	bigint_adjust(&s);
	memset(dest->wordv + dest->length_B, 0, (MAX(dest->length_B, s.length_B + word_shift) - dest->length_B) * sizeof(bigint_word_t));
	x.wordv = dest->wordv + word_shift;
	x.length_B = dest->length_B - word_shift;
	if((int16_t)x.length_B < 0){
		x.length_B = 0;
		x.info = 0;
	}else{
		x.info = dest->info;
	}
	bigint_add_u(&x, &x, &s);
	dest->length_B = x.length_B + word_shift;
	dest->info = 0;
	bigint_adjust(dest);
#endif


/*	uint16_t i,j=0;
	uint16_t scale_w;
	bigint_word_t *dst;
	bigint_wordplus_t t=0;
	scale_w = (scale+sizeof(bigint_word_t)-1)/sizeof(bigint_word_t);
	if(scale>dest->length_B*sizeof(bigint_word_t)){
		memset(((uint8_t*)dest->wordv)+dest->length_B*sizeof(bigint_word_t), 0, scale-dest->length_B*sizeof(bigint_word_t));
	}
	// a->wordv = (const uint32_t*)(((uint8_t*)a->wordv)+(scale&3));
	dst  = dest->wordv + (scale&(sizeof(bigint_word_t)-1));
	for(i=scale/sizeof(bigint_word_t); i<a->length_B+scale_w; ++i,++j){
		t += a->wordv[j];
		if(dest->length_B>i){
			t += dst[i];
		}
		dst[i] = (bigint_word_t)t;
		t>>=BIGINT_WORD_SIZE;
	}
	while(t){
		if(dest->length_B>i){
			t += dst[i];
		}
		dst[i] = (bigint_word_t)t;
		t>>=BIGINT_WORD_SIZE;
		++i;
	}
	if(dest->length_B < i){
		dest->length_B = i;
	}
	bigint_adjust(dest);
	*/
}

/******************************************************************************/

/* this should be implemented in assembly */
void bigint_sub_u(bigint_t* dest, const bigint_t* a, const bigint_t* b){
	int8_t borrow=0;
	int8_t  r;
	bigint_wordplus_signed_t t=0LL;
	uint16_t i;
	r = bigint_cmp_u(a,b);
	if(r==0){
		bigint_set_zero(dest);
		return;
	}
	if(b->length_B==0){
		bigint_copy(dest, a);
		SET_POS(dest);
		return;
	}
	if(a->length_B==0){
		bigint_copy(dest, b);
		SET_NEG(dest);
		return;
	}
	if(r<0){
		bigint_sub_u(dest, b, a);
		SET_NEG(dest);
		return;
	}
	for(i=0; i < a->length_B; ++i){
		t = a->wordv[i];
		if(i < b->length_B){
			t -= b->wordv[i];
		}
		t -= borrow;
		dest->wordv[i]=(bigint_word_t)t;
		if(t<0){
			borrow = 1;
		}else{
			borrow = 0;
		}
	}
	SET_POS(dest);
	dest->length_B = i;
	bigint_adjust(dest);
}

/******************************************************************************/

int8_t bigint_cmp_u(const bigint_t* a, const bigint_t* b){
	if(a->length_B > b->length_B){
		return 1;
	}
	if(a->length_B < b->length_B){
		return -1;
	}
	if(a->length_B==0){
		return 0;
	}
	uint16_t i;
	i = a->length_B-1;
	do{
		if(a->wordv[i] != b->wordv[i]){
			if(a->wordv[i] > b->wordv[i]){
				return 1;
			}else{
				return -1;
			}
		}
	}while(i--);
	return 0;
}

/******************************************************************************/

void bigint_add_s(bigint_t* dest, const bigint_t* a, const bigint_t* b){
	uint8_t s;
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			bigint_add_u(dest, a,b);
			SET_POS(dest);
			break;
		case 1: /* a positive, b negative */
			bigint_sub_u(dest, a, b);
			break;
		case 2: /* a negative, b positive */
			bigint_sub_u(dest, b, a);
			break;
		case 3: /* both negative */
			bigint_add_u(dest, a, b);
			SET_NEG(dest);
			break;
		default: /* how can this happen?*/
			break;
	}
}

/******************************************************************************/

void bigint_sub_s(bigint_t* dest, const bigint_t* a, const bigint_t* b){
	uint8_t s;
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			bigint_sub_u(dest, a,b);
			break;
		case 1: /* a positive, b negative */
			bigint_add_u(dest, a, b);
			SET_POS(dest);
			break;
		case 2: /* a negative, b positive */
			bigint_add_u(dest, a, b);
			SET_NEG(dest);
			break;
		case 3: /* both negative */
			bigint_sub_u(dest, b, a);
			break;
		default: /* how can this happen?*/
					break;
	}

}

/******************************************************************************/

int8_t bigint_cmp_s(const bigint_t* a, const bigint_t* b){
	uint8_t s;
	if(a->length_B==0 && b->length_B==0){
		return 0;
	}
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			return bigint_cmp_u(a, b);
			break;
		case 1: /* a positive, b negative */
			return 1;
			break;
		case 2: /* a negative, b positive */
			return -1;
			break;
		case 3: /* both negative */
			return bigint_cmp_u(b, a);
			break;
		default: /* how can this happen?*/
					break;
	}
	return 0; /* just to satisfy the compiler */
}

/******************************************************************************/

void bigint_shiftleft(bigint_t* a, uint16_t shift){
	uint16_t byteshift, words_to_shift;
	int16_t i;
	uint8_t bitshift;
	bigint_word_t *p;
	bigint_wordplus_t t=0;
	if(shift == 0){
		return;
	}
	byteshift = shift / 8;
	bitshift = shift & 7;
	memset(&a->wordv[a->length_B], 0x00, byteshift);
	if(byteshift){
		memmove(((uint8_t*)a->wordv)+byteshift, a->wordv, a->length_B * sizeof(bigint_word_t));
		memset(a->wordv, 0, byteshift);
	}
	if(bitshift != 0){
		p = (bigint_word_t*)((uint8_t*)a->wordv + byteshift / sizeof(bigint_word_t));
		words_to_shift = a->length_B + ((byteshift % sizeof(bigint_word_t))?1:0);
		/* XXX */
		for(i = 0; i < words_to_shift; ++i){
			t |= ((bigint_wordplus_t)p[i]) << bitshift;
			p[i] = (bigint_word_t)t;
			t >>= BIGINT_WORD_SIZE;
		}
		p[i] = (bigint_word_t)t;
	}
	a->length_B += (shift + BIGINT_WORD_SIZE - 1) / BIGINT_WORD_SIZE;
	bigint_adjust(a);
}

/******************************************************************************/

void bigint_shiftright(bigint_t* a, uint16_t shift){
	uint16_t byteshift;
	uint16_t i;
	uint8_t bitshift;
	bigint_wordplus_t t=0;
	byteshift = shift/8;
	bitshift = shift&7;
	if(byteshift >= a->length_B * sizeof(bigint_word_t)){ /* we would shift out more than we have */
		bigint_set_zero(a);
		return;
	}
	if(byteshift == a->length_B * sizeof(bigint_word_t) - 1 && bitshift > GET_FBS(a)){
		bigint_set_zero(a);
		return;
	}
	if(byteshift){
		memmove(a->wordv, (uint8_t*)a->wordv + byteshift, a->length_B * sizeof(bigint_word_t) - byteshift);
		memset((uint8_t*)a->wordv + a->length_B * sizeof(bigint_word_t) - byteshift, 0,  byteshift);
	}
	byteshift /= sizeof(bigint_word_t);
    a->length_B -= (byteshift  + sizeof(bigint_word_t) - 1) / sizeof(bigint_word_t);
	if(bitshift != 0 && a->length_B){
	 /* shift to the right */
		i = a->length_B - 1;
		do{
			t |= ((bigint_wordplus_t)(a->wordv[i])) << (BIGINT_WORD_SIZE - bitshift);
			a->wordv[i] = (bigint_word_t)(t >> BIGINT_WORD_SIZE);
			t <<= BIGINT_WORD_SIZE;
		}while(i--);
	}
	bigint_adjust(a);
}

/******************************************************************************/

void bigint_xor(bigint_t* dest, const bigint_t* a){
	uint16_t i;
	for(i=0; i<a->length_B; ++i){
		dest->wordv[i] ^= a->wordv[i];
	}
	bigint_adjust(dest);
}

/******************************************************************************/

void bigint_set_zero(bigint_t* a){
	a->length_B=0;
}

/******************************************************************************/

/* using the Karatsuba-Algorithm */
/* x*y = (xh*yh)*b**2n + ((xh+xl)*(yh+yl) - xh*yh - xl*yl)*b**n + yh*yl */
void bigint_mul_u(bigint_t* dest, const bigint_t* a, const bigint_t* b){
	if(a->length_B==0 || b->length_B==0){
		bigint_set_zero(dest);
		return;
	}
	if(dest==a || dest==b){
		bigint_t d;
		bigint_word_t d_b[a->length_B + b->length_B];
		d.wordv = d_b;
		bigint_mul_u(&d, a, b);
		bigint_copy(dest, &d);
		return;
	}
	if(a->length_B==1 || b->length_B==1){
		if(a->length_B != 1){
			XCHG_PTR(a,b);
		}
		bigint_wordplus_t t=0;
		uint16_t i;
		bigint_word_t x = a->wordv[0];
		for(i=0; i < b->length_B; ++i){
			t += ((bigint_wordplus_t)b->wordv[i]) * ((bigint_wordplus_t)x);
			dest->wordv[i] = (bigint_word_t)t;
			t>>=BIGINT_WORD_SIZE;
		}
		dest->wordv[i] = (bigint_word_t)t;
		dest->length_B = i+1;
		dest->info = 0;
		bigint_adjust(dest);
		return;
	}
	if(a->length_B * sizeof(bigint_word_t) <= 4 && b->length_B * sizeof(bigint_word_t) <= 4){
		uint32_t p=0, q=0;
		uint64_t r;
		memcpy(&p, a->wordv, a->length_B*sizeof(bigint_word_t));
		memcpy(&q, b->wordv, b->length_B*sizeof(bigint_word_t));
		r = (uint64_t)p * (uint64_t)q;
		memcpy(dest->wordv, &r, (dest->length_B = a->length_B + b->length_B)*sizeof(bigint_word_t));
		bigint_adjust(dest);
		return;
	}
#if BIGINT_WORD_SIZE == 8
	if(a->length_B <= 4 || b->length_B <= 4){
		if(a->length_B > 4){
			XCHG_PTR(a,b);
		}
		uint32_t x = 0, y = 0;
		uint16_t j = b->length_B / 4, idx = 0;
		uint64_t r = 0;
		memcpy(&x, a->wordv, a->length_B);
		while(j){
			r += (uint64_t)((uint32_t*)b->wordv)[idx] * (uint64_t)x;
			((uint32_t*)dest->wordv)[idx] = (uint32_t)r;
			r >>= 32;
			++idx;
			--j;
		}
		idx *= 4;
		memcpy(&y, b->wordv + idx, b->length_B - idx);
		r += (uint64_t)y * (uint64_t)x;
		while(r){
			dest->wordv[idx++] = (uint8_t)r;
			r >>= 8;
		}
		dest->length_B = idx;
		bigint_adjust(dest);
		return;
	}
#endif
	/* split a in xh & xl; split b in yh & yl */
	const uint16_t n = (MAX(a->length_B, b->length_B)+1)/2;
	bigint_t xl, xh, yl, yh;
	xl.wordv = a->wordv;
	yl.wordv = b->wordv;
	if(a->length_B <= n){
		bigint_set_zero(&xh);
		xl.length_B = a->length_B;
		xl.info = a->info;
	}else{
		xl.length_B = n;
		xl.info = 0;
		bigint_adjust(&xl);
		xh.wordv = &(a->wordv[n]);
		xh.length_B = a->length_B-n;
		xh.info = a->info;
	}
	if(b->length_B <= n){
		bigint_set_zero(&yh);
		yl.length_B = b->length_B;
		yl.info = b->info;
	}else{
		yl.length_B = n;
		yl.info = 0;
		bigint_adjust(&yl);
		yh.wordv = &(b->wordv[n]);
		yh.length_B = b->length_B-n;
		yh.info = b->info;
	}
	/* now we have split up a and b */
	/* remember we want to do:
	 * x*y = (xh * b ** n + xl) * (yh * b ** n + yl)
	 * x*y = (xh*yh)*b**2n + ((xh+xl)*(yh+yl) - xh*yh - xl*yl)*b**n + xl*yl
	 *          5          9     2   4   3    7   5   6   1         8   1
	 */
	bigint_word_t  tmp1_b[n*2+2];
	bigint_t tmp1, tmp2;
	tmp1.wordv = tmp1_b;
    tmp2.wordv = &tmp1_b[n+1];

	bigint_add_u(&tmp2,  &xh,   &xl);                            /* 2: tmp2 <= xh + xl     */
	bigint_add_u(&tmp1, &yh,   &yl);                            /* 3: tmp1 <= yh + yl     */
	bigint_mul_u(dest,  &tmp2, &tmp1);                          /* 4: dest <= tmp2 * tmp1  */
	bigint_mul_u(&tmp1, &xh,   &yh);                            /* 5: tmp1 <= xh * yh     */
    bigint_sub_u(dest,  dest,  &tmp1);                          /* 7: dest <= dest - tmp1       */
    bigint_word_t tmp3_b[2*n];
    tmp2.wordv = tmp3_b;
    bigint_mul_u(&tmp2, &xl,   &yl);                            /* 1: tmp3 <= xl * yl     */
	bigint_sub_u(dest,  dest,  &tmp2);                          /* 6: dest <= dest - tmp3    */
    bigint_shiftleft(dest, n * sizeof(bigint_word_t) * 8);
	bigint_add_u(dest, dest, &tmp2);                            /* 8: dest <= tmp3 + dest ** n       */
	bigint_add_scale_u(dest, &tmp1, 2*n*sizeof(bigint_word_t)); /* 9: dest <= dest + tmp1 ** (2 * n) */

#if 0
	bigint_mul_u(dest,  &xl,   &yl);                            /* 1: dest <= xl * yl     */
	bigint_add_u(&tmp2, &xh,   &xl);                            /* 2: tmp2 <= xh + xl     */
	bigint_add_u(&tmp1, &yh,   &yl);                            /* 3: tmp1 <= yh + yl     */
	bigint_mul_u(&tmp3, &tmp2, &tmp1);                          /* 4: tmp3 <= tmp2 * tmp1  */
	bigint_mul_u(&tmp1, &xh,   &yh);                            /* 5: h    <= xh * yh     */
	bigint_sub_u(&tmp3, &tmp3, dest);                           /* 6: tmp3 <= tmp3 - dest    */
    bigint_sub_u(&tmp3, &tmp3, &tmp1);                          /* 7: tmp3 <= tmp3 - h       */
	bigint_add_scale_u(dest, &tmp3, n*sizeof(bigint_word_t));   /* 8: dest <= dest + tmp3 ** n       */
	bigint_add_scale_u(dest, &tmp1, 2*n*sizeof(bigint_word_t)); /* 9: dest <= dest + tmp1 ** (2 * n) */
#endif
}

/******************************************************************************/

void bigint_mul_s(bigint_t* dest, const bigint_t* a, const bigint_t* b){
	uint8_t s;
	s  = GET_SIGN(a)?2:0;
	s |= GET_SIGN(b)?1:0;
	switch(s){
		case 0: /* both positive */
			bigint_mul_u(dest, a,b);
			SET_POS(dest);
			break;
		case 1: /* a positive, b negative */
			bigint_mul_u(dest, a,b);
			SET_NEG(dest);
			break;
		case 2: /* a negative, b positive */
			bigint_mul_u(dest, a,b);
			SET_NEG(dest);
			break;
		case 3: /* both negative */
			bigint_mul_u(dest, a,b);
			SET_POS(dest);
			break;
		default: /* how can this happen?*/
			break;
	}
}

/******************************************************************************/

/* square */
/* (xh*b^n+xl)^2 = xh^2*b^2n + 2*xh*xl*b^n + xl^2 */
void bigint_square(bigint_t* dest, const bigint_t* a){
	if(a->length_B * sizeof(bigint_word_t) <= 4){
		uint64_t r = 0;
		memcpy(&r, a->wordv, a->length_B * sizeof(bigint_word_t));
		r = r * r;
		memcpy(dest->wordv, &r, 2 * a->length_B*sizeof(bigint_word_t));
		SET_POS(dest);
		dest->length_B = 2 * a->length_B;
		bigint_adjust(dest);
		return;
	}
	if(dest==a){
		bigint_t d;
		bigint_word_t d_b[a->length_B * 2];
		d.wordv = d_b;
		bigint_square(&d, a);
		bigint_copy(dest, &d);
		return;
	}
	uint16_t n;
	n = (a->length_B + 1) / 2;
	bigint_t xh, xl, tmp; /* x-high, x-low, temp */
	bigint_word_t buffer[2 * n + 1];
	xl.wordv = a->wordv;
	xl.length_B = n;
	xl.info = 0;
	xh.wordv = &(a->wordv[n]);
	xh.length_B = a->length_B - n;
	xh.info = a->info;
	bigint_adjust(&xl);
	tmp.wordv = buffer;
/* (xh * b**n + xl)**2 = xh**2 * b**2n + 2 * xh * xl * b**n + xl**2 */

//	cli_putstr("\r\nDBG (a): xl: "); bigint_print_hex(&xl);
//	cli_putstr("\r\nDBG (b): xh: "); bigint_print_hex(&xh);
	bigint_square(dest, &xl);
//	cli_putstr("\r\nDBG (1): xl**2: "); bigint_print_hex(dest);
	bigint_square(&tmp, &xh);
//	cli_putstr("\r\nDBG (2): xh**2: "); bigint_print_hex(&tmp);
	bigint_add_scale_u(dest, &tmp, 2 * n * sizeof(bigint_word_t));
//	cli_putstr("\r\nDBG (3): xl**2 + xh**2*n**2: "); bigint_print_hex(dest);
	bigint_mul_u(&tmp, &xl, &xh);
//	cli_putstr("\r\nDBG (4): xl*xh: "); bigint_print_hex(&tmp);
	bigint_shiftleft(&tmp, 1);
//	cli_putstr("\r\nDBG (5): xl*xh*2: "); bigint_print_hex(&tmp);
	bigint_add_scale_u(dest, &tmp, n*sizeof(bigint_word_t));
//	cli_putstr("\r\nDBG (6): x**2: "); bigint_print_hex(dest);
//	cli_putstr("\r\n");
}

/******************************************************************************/
void bigint_sub_u_bitscale(bigint_t* a, const bigint_t* b, uint16_t bitscale){
	bigint_t tmp, x;
	bigint_word_t tmp_b[b->length_B + 1];
	const uint16_t word_shift = bitscale / BIGINT_WORD_SIZE;

	if(a->length_B < b->length_B + word_shift){
#if DEBUG
		cli_putstr("\r\nDBG: *bang*\r\n");
#endif
		bigint_set_zero(a);
		return;
	}
	tmp.wordv = tmp_b;
	bigint_copy(&tmp, b);
	bigint_shiftleft(&tmp, bitscale % BIGINT_WORD_SIZE);

//	cli_putstr_P(PSTR("\r\nDBG: shifted value: ")); bigint_print_hex(&tmp);

	x.info = a->info;
	x.wordv = &(a->wordv[word_shift]);
	x.length_B = a->length_B - word_shift;

	bigint_sub_u(&x, &x, &tmp);
	a->length_B = x.length_B + word_shift;
	bigint_adjust(a);
	return;
}

/******************************************************************************/

void bigint_reduce(bigint_t* a, const bigint_t* r){
//	bigint_adjust((bigint_t*)r);
	uint8_t rfbs = GET_FBS(r);
#if DEBUG
	cli_putstr("\r\nDBG: (a) = "); bigint_print_hex(a);
#endif
	if(r->length_B==0 || a->length_B==0){
		return;
	}
	if((r->length_B*sizeof(bigint_word_t)<=4) && (a->length_B*sizeof(bigint_word_t)<=4)){
		uint32_t p=0, q=0;
		memcpy(&p, a->wordv, a->length_B*sizeof(bigint_word_t));
		memcpy(&q, r->wordv, r->length_B*sizeof(bigint_word_t));
		p %= q;
		memcpy(a->wordv, &p, a->length_B*sizeof(bigint_word_t));
		bigint_adjust(a);
//		cli_putstr("\r\nDBG: (0) = "); bigint_print_hex(a);
		return;
	}
	uint16_t shift;
	while(a->length_B > r->length_B){
		shift = (a->length_B - r->length_B) * 8 * sizeof(bigint_word_t) + GET_FBS(a) - rfbs - 1;
		/*
		if((a->wordv[a->length_B-1] & ((1LL<<GET_FBS(a)) - 1)) > r->wordv[r->length_B-1]){
			// cli_putc('~');
			cli_putstr("\r\n ~ [a] = ");
			cli_hexdump_rev(&a->wordv[a->length_B-1], 4);
			cli_putstr("  [r] = ");
			cli_hexdump_rev(&r->wordv[r->length_B-1], 4);
			shift += 1;
		}
		*/
//		cli_putstr("\r\nDBG: (p) shift = "); cli_hexdump_rev(&shift, 2);
//		cli_putstr(" a_len = "); cli_hexdump_rev(&a->length_B, 2);
//		cli_putstr(" r_len = "); cli_hexdump_rev(&r->length_B, 2);
//		uart_flush(0);
		bigint_sub_u_bitscale(a, r, shift);
//		cli_putstr("\r\nDBG: (1) = "); bigint_print_hex(a);
	}
	while((GET_FBS(a) > rfbs) && (a->length_B == r->length_B)){
		shift = GET_FBS(a)-rfbs-1;
//		cli_putstr("\r\nDBG: (2a) = "); bigint_print_hex(a);
//		cli_putstr("\r\nDBG: (q) shift = "); cli_hexdump_rev(&shift, 2);
		bigint_sub_u_bitscale(a, r, shift);
//		cli_putstr("\r\nDBG: (2b) = "); bigint_print_hex(a);
	}
	while(bigint_cmp_u(a,r)>=0){
		bigint_sub_u(a,a,r);
//		cli_putstr("\r\nDBG: (3) = "); bigint_print_hex(a);
	}
//	cli_putc(' ');
	bigint_adjust(a);
//	cli_putstr("\r\nDBG: (a) = "); bigint_print_hex(a);
//	cli_putstr("\r\n");
}

/******************************************************************************/

/* calculate dest = a**exp % r */
/* using square&multiply */
void bigint_expmod_u(bigint_t* dest, const bigint_t* a, const bigint_t* exp, const bigint_t* r){
	if(a->length_B==0 || r->length_B==0){
		return;
	}

	bigint_t res, base;
	bigint_word_t t, base_b[MAX(a->length_B,r->length_B)], res_b[r->length_B*2];
	uint16_t i;
	uint8_t j;
//	uint16_t *xaddr = &i;
//	cli_putstr("\r\npre-alloc (");
//	cli_hexdump_rev(&xaddr, 4);
//	cli_putstr(") ...");
	res.wordv = res_b;
	base.wordv = base_b;
	bigint_copy(&base, a);
//	cli_putstr("\r\npost-copy");
	bigint_reduce(&base, r);
	res.wordv[0]=1;
	res.length_B=1;
	res.info = 0;
	bigint_adjust(&res);
	if(exp->length_B == 0){
		bigint_copy(dest, &res);
		return;
	}
#if PRINT_STATUS
    putch('+');
#endif
	uint8_t flag = 0;
	t=exp->wordv[exp->length_B - 1];
	for(i=exp->length_B; i > 0; --i){
		t = exp->wordv[i - 1];
		for(j=BIGINT_WORD_SIZE; j > 0; --j){
#if PRINT_STATUS
            putch('.');
#endif
			if(!flag){
				if(t & (1<<(BIGINT_WORD_SIZE-1))){
					flag = 1;
				}
			}
			if(flag){            
				bigint_square(&res, &res);
				bigint_reduce(&res, r);
				if(t & (1<<(BIGINT_WORD_SIZE-1))){
                    //PORTA.OUTSET = PIN0_bm;
					bigint_mul_u(&res, &res, &base);
					bigint_reduce(&res, r);
                    //PORTA.OUTCLR = PIN0_bm;
				}
			}
			t<<=1;
		}
	}

#if PRINT_STATUS
    putch('\n');
#endif
//	cli_putc('+');
	SET_POS(&res);
	bigint_copy(dest, &res);
}

/******************************************************************************/

#define cli_putstr(a)
#define bigint_print_hex(a)
#define cli_hexdump_rev(a,b)
#define uart_flush(a)

/* gcd <-- gcd(x,y) a*x+b*y=gcd */
void bigint_gcdext(bigint_t* gcd, bigint_t* a, bigint_t* b, const bigint_t* x, const bigint_t* y){
	 bigint_t g, x_, y_, u, v, a_, b_, c_, d_;
	 uint16_t i=0;
	 if(x->length_B==0 || y->length_B==0){
		 return;
	 }
	 if(x->length_B==1 && x->wordv[0]==1){
		 gcd->length_B = 1;
		 gcd->wordv[0] = 1;
		 if(a){
			 a->length_B = 1;
			 a->wordv[0] = 1;
			 SET_POS(a);
			 bigint_adjust(a);
		 }
		 if(b){
			 bigint_set_zero(b);
		 }
		 return;
	 }
	 if(y->length_B==1 && y->wordv[0]==1){
		 gcd->length_B = 1;
		 gcd->wordv[0] = 1;
		 if(b){
			 b->length_B = 1;
			 b->wordv[0] = 1;
			 SET_POS(b);
			 bigint_adjust(b);
		 }
		 if(a){
			 bigint_set_zero(a);
		 }
		 return;
	 }

	 while(x->wordv[i]==0 && y->wordv[i]==0){
		 ++i;
	 }
	 bigint_word_t g_b[i+2], x_b[x->length_B-i], y_b[y->length_B-i];
	 bigint_word_t u_b[x->length_B-i], v_b[y->length_B-i];
	 bigint_word_t a_b[y->length_B+2], c_b[y->length_B+2];
	 bigint_word_t b_b[x->length_B+2], d_b[x->length_B+2];

	 g.wordv = g_b;
	 x_.wordv = x_b;
	 y_.wordv = y_b;
	 memset(g_b, 0, i*sizeof(bigint_word_t));
	 g_b[i]=1;
	 g.length_B = i+1;
	 g.info=0;
	 x_.info = y_.info = 0;
	 x_.length_B = x->length_B-i;
	 y_.length_B = y->length_B-i;
	 memcpy(x_.wordv, x->wordv+i, x_.length_B*sizeof(bigint_word_t));
	 memcpy(y_.wordv, y->wordv+i, y_.length_B*sizeof(bigint_word_t));
	 for(i=0; (x_.wordv[0]&(1<<i))==0 && (y_.wordv[0]&(1<<i))==0; ++i){
	 }

	 bigint_adjust(&x_);
	 bigint_adjust(&y_);

	 if(i){
		 bigint_shiftleft(&g, i);
		 bigint_shiftright(&x_, i);
		 bigint_shiftright(&y_, i);
	 }

	 u.wordv = u_b;
	 v.wordv = v_b;
	 a_.wordv = a_b;
	 b_.wordv = b_b;
	 c_.wordv = c_b;
	 d_.wordv = d_b;

	 bigint_copy(&u, &x_);
	 bigint_copy(&v, &y_);
	 a_.wordv[0] = 1;
	 a_.length_B = 1;
	 a_.info = 0;
	 d_.wordv[0] = 1;
	 d_.length_B = 1;
	 d_.info = 0;
	 bigint_set_zero(&b_);
	 bigint_set_zero(&c_);
	 do{
		 cli_putstr("\r\nDBG (gcdext) 0");
		 while((u.wordv[0]&1)==0){
			 cli_putstr("\r\nDBG (gcdext) 0.1");
			 bigint_shiftright(&u, 1);
			 if((a_.wordv[0]&1) || (b_.wordv[0]&1)){
				 bigint_add_s(&a_, &a_, &y_);
				 bigint_sub_s(&b_, &b_, &x_);
			 }
			 bigint_shiftright(&a_, 1);
			 bigint_shiftright(&b_, 1);
		 }
		 while((v.wordv[0]&1)==0){
			 cli_putstr("\r\nDBG (gcdext) 0.2");
			 bigint_shiftright(&v, 1);
			 if((c_.wordv[0]&1) || (d_.wordv[0]&1)){
				 bigint_add_s(&c_, &c_, &y_);
				 bigint_sub_s(&d_, &d_, &x_);
			 }
			 bigint_shiftright(&c_, 1);
			 bigint_shiftright(&d_, 1);

		 }
		 if(bigint_cmp_u(&u, &v)>=0){
			bigint_sub_u(&u, &u, &v);
			bigint_sub_s(&a_, &a_, &c_);
			bigint_sub_s(&b_, &b_, &d_);
		 }else{
			bigint_sub_u(&v, &v, &u);
			bigint_sub_s(&c_, &c_, &a_);
			bigint_sub_s(&d_, &d_, &b_);
		 }
	 }while(u.length_B);
	 if(gcd){
		 bigint_mul_s(gcd, &v, &g);
	 }
	 if(a){
		bigint_copy(a, &c_);
	 }
	 if(b){
		 bigint_copy(b, &d_);
	 }
}

/******************************************************************************/

void bigint_inverse(bigint_t* dest, const bigint_t* a, const bigint_t* m){
	bigint_gcdext(NULL, dest, NULL, a, m);
	while(dest->info&BIGINT_NEG_MASK){
		bigint_add_s(dest, dest, m);
	}
}

/******************************************************************************/

void bigint_changeendianess(bigint_t* a){
	uint8_t t, *p, *q;
	p = (uint8_t*)(a->wordv);
	q = p + a->length_B * sizeof(bigint_word_t) - 1;
	while(p<q){
		t = *p;
		*p = *q;
		*q = t;
		++p; --q;
	}
}

/******************************************************************************/























