/* shabal512-asm.S */
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
 * \file    shabal512-asm.S
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

/******************************************************************************/	

shabal512_iv:
	/* A */ 
.long	0x20728DFD, 0x46C0BD53, 0xE782B699, 0x55304632 
.long	0x71B4EF90, 0x0EA9E82C, 0xDBB930F1, 0xFAD06B8B
.long	0xBE0CAE40, 0x8BD14410, 0x76D2ADAC, 0x28ACAB7F
	/* B */ 
.long	0xC1099CB7, 0x07B385F3, 0xE7442C26, 0xCC8AD640 
.long	0xEB6F56C7, 0x1EA81AA9, 0x73B9D314, 0x1DE85D08
.long	0x48910A5A, 0x893B22DB, 0xC5A0DF44, 0xBBC4324E 
.long	0x72D2F240, 0x75941D99, 0x6D8BDE82, 0xA1A7502B
	/* C */
.long	0xD9BF68D1, 0x58BAD750, 0x56028CB2, 0x8134F359 
.long	0xB5D469D8, 0x941A8CC2, 0x418B2A6E, 0x04052780
.long	0x7F07D787, 0x5194358F, 0x3C60D665, 0xBE97D79A 
.long	0x950C3434, 0xAED9A06D, 0x2537DC8D, 0x7CDB5969

/******************************************************************************/	
/*
 * param ctx: r24,r25
 */
.global shabal512_init
shabal512_init:
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
	ldi r30, lo8(shabal512_iv)
	ldi r31, hi8(shabal512_iv)
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

.global shabal512_ctx2hash
shabal512_ctx2hash:
	movw r26, r24
	movw r30, r22
	ldd r24, Z+(8+2)
	ldd r25, Z+(8+2+1)
	movw r30, r24
	; adiw r30, (16-512/32)*4
	ldi r24, 512/8
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
.global shabal512
shabal512: 
	push_range 8, 17
	stack_alloc_large 188
	adiw r30, 1 /* Z points to stack space (ctx) */
	movw CTX0, r30
	movw DST0, r24
	movw MSG0, r22
	movw LEN0, r18
	movw LEN2, r20
	movw r24, r30
	rcall shabal512_init
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
	rcall shabal512_ctx2hash
	stack_free_large2 188
	pop_range 8, 17
	ret
		
