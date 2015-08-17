/* arcfour-asm.S */
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
 * File:        arcfour-asm.S
 * Author:      Daniel Otte
 * Date:        2006-07-06
 * License:     GPLv3 or later
 * Description: Implementation of the ARCFOUR (RC4 compatible) stream cipher algorithm.
 * 
 */
 
#include <avr/io.h>
#include "avr-asm-macros.S"
 
 /* +---+---+---------------------+
 *  | i | j | ......<256>........ |
 *  +---+---+---------------------+
 */
 
.global arcfour_init
/*
 *== arcfour_init ==
 *  this function initialises the context
 * param1: 16-bit pointer to the key
 *	given in r24:r25
 * param2: 8-bit integer indicating keylength in bits
 *	given in r22:r23
 * param3: 16-bit pointer to a ctx struct
 *	given in r20:r21
 */
arcfour_init:
	push_ r28, r29
	movw r26, r20   /* X points to ctx */
	movw r30, r24   /* Z points to key */
	st X+, r1
	st X+, r1       /* X points to S */
	movw r20, r26   /* store pointer to S in r21:r20 */
	lsr r23
	ror r22
	lsr r23
	ror r22
	lsr r23
	ror r22
1:		
	st X+, r1 
	inc r1
	brne 1b
	
	movw r26, r20
	add r22, r30         /* r18 is keyindex counter */
	clr r0
	clr r19
2:
	ld r23, X
	ld r18, Z+
	add r19, r18
	add r19, r23
	movw r28, r20   /* load pointer to S in Y */
	add r28, r19
	adc r29, r1
	ld r18, Y
	st Y,  r23
	st X+, r18
	cp r30, r22
	brne 3f
	movw r30, r24
3:		
	inc r0
	brne 2b	
	pop_ r29, r28
	ret

/*
uint8_t arcfour_gen(arcfour_ctx_t *c){
	uint8_t t;
	c->i++;
	c->j += c->s[c->i];
	t = c->s[c->j];
	c->s[c->j] = c->s[c->i];
	c->s[c->i] = t;
	return c->s[(c->s[c->j] + c->s[c->i]) & 0xff];
}
*/
.global arcfour_gen

;== arcfour_gen ==
;  this function initialises the context
; param1: 16-bit pointer to a ctx struct
;	given in r25,r24

arcfour_gen:
	movw r26, r24
	ld r18, X
	inc r18
	st X+, r18
	movw r30, r26
	ld r19, X+
	add r26, r18
	adc r27, r1
	ld r20, X
	add r19, r20
	st Z+, r19		/* i,j loaded&saved; X->S[i]; Z->S[0]; r20=S[i] */
	add r30, r19
	adc r31, r1
	ld r21, Z		/* X->S[i]; Z->S[j]; r20=S[i]; r21=S[j]*/
	st Z, r20
	st X, r21
	add r20, r21
	adiw r24, 2
	movw r26, r24 /* X and Z point to S */
	add r26, r20
	adc r27, r1
	ld r24, X
	clr r25
	ret






