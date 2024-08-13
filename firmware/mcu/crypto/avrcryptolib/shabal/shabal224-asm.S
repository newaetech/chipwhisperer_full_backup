/* shabal224-asm.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file    shabal224-asm.S
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

/******************************************************************************/	

shabal224_iv:
	/* A */
.long	0xA5201467, 0xA9B8D94A, 0xD4CED997, 0x68379D7B 
.long	0xA7FC73BA, 0xF1A2546B, 0x606782BF, 0xE0BCFD0F
.long	0x2F25374E, 0x069A149F, 0x5E2DFF25, 0xFAECF061
	/* B */ 
.long	0xEC9905D8, 0xF21850CF, 0xC0A746C8, 0x21DAD498
.long	0x35156EEB, 0x088C97F2, 0x26303E40, 0x8A2D4FB5
.long	0xFEEE44B6, 0x8A1E9573, 0x7B81111A, 0xCBC139F0 
.long	0xA3513861, 0x1D2C362E, 0x918C580E, 0xB58E1B9C
	/* C */
.long	0xE4B573A1, 0x4C1A0880, 0x1E907C51, 0x04807EFD 
.long	0x3AD8CDE5, 0x16B21302, 0x02512C53, 0x2204CB18
.long	0x99405F2D, 0xE5B648A1, 0x70AB1D43, 0xA10C25C2 
.long	0x16F1AC05, 0x38BBEB56, 0x9B01DC60, 0xB1096D83


/******************************************************************************/	
/*
 * param ctx: r24,r25
 */
.global shabal224_init
shabal224_init:
	movw r26, r24
	ldi r24, 1
	st X+, r24
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	movw r24, r26
	adiw r24, 12*4+4
	st X+, r24
	st X+, r25
	adiw r24, 4*16-1
	adiw r24, 1
	st X+, r24
	st X+, r25 
	ldi r24, (12+16+16)
	ldi r30, lo8(shabal224_iv)
	ldi r31, hi8(shabal224_iv)
1:	lpm r0, Z+
	st X+, r0
	lpm r0, Z+
	st X+, r0
	lpm r0, Z+
	st X+, r0
	lpm r0, Z+
	st X+, r0
	dec r24
	brne 1b
	ret	

/******************************************************************************/	
	
.global shabal224_ctx2hash
shabal224_ctx2hash:
	movw r26, r24
	movw r30, r22
	ldd r24, Z+(8+2)
	ldd r25, Z+(8+2+1)
	movw r30, r24
	adiw r30, (16-224/32)*4
	ldi r24, 224/8
1:  ld r0, Z+
	st X+, r0
	dec r24
	brne 1b
	ret		
	
/******************************************************************************/	
/*
 * param dest:      r24:r25
 * param msg:       r22:r23
 * param length_b:  r18:r21
 */
DST0 =  8
DST1 =  9
CTX0 = 10
CTX1 = 11
MSG0 = 12
MSG1 = 13
LEN2 = 14
LEN3 = 15
LEN0 = 16
LEN1 = 17
.global shabal224
shabal224: 
	push_range 8, 17
	stack_alloc_large 188
	adiw r30, 1 /* Z points to stack space (ctx) */
	movw CTX0, r30
	movw DST0, r24
	movw MSG0, r22
	movw LEN0, r18
	movw LEN2, r20
	movw r24, r30
	rcall shabal224_init
2:
	tst LEN2
	brne 3f
	tst LEN3
	breq 4f
3:
	movw r24, CTX0
	movw r22, MSG0
	rcall shabal_nextBlock
	subi LEN1, 0x02
	sbc LEN2, r1
	sbc LEN3, r1
	ldi r18, 64
	add MSG0, r18
	adc MSG1, r1	
	rjmp 2b
4:	
	movw r24, CTX0
	movw r22, MSG0
	movw r20, LEN0
	rcall shabal_lastBlock
	movw r24, DST0
	movw r22, CTX0
	rcall shabal224_ctx2hash
	stack_free_large2 188
	pop_range 8, 17
	ret
	

