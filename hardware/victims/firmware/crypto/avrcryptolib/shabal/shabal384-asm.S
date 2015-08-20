/* shabal384-asm.S */
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
 * \file    shabal384-asm.S
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

/******************************************************************************/	

shabal384_iv:
	/* A */ 
.long	0xC8FCA331, 0xE55C504E, 0x003EBF26, 0xBB6B8D83 
.long	0x7B0448C1, 0x41B82789, 0x0A7C9601, 0x8D659CFF
.long	0xB6E2673E, 0xCA54C77B, 0x1460FD7E, 0x3FCB8F2D
	/* B */ 
.long	0x527291FC, 0x2A16455F, 0x78E627E5, 0x944F169F 
.long	0x1CA6F016, 0xA854EA25, 0x8DB98ABE, 0xF2C62641
.long	0x30117DCB, 0xCF5C4309, 0x93711A25, 0xF9F671B8 
.long	0xB01D2116, 0x333F4B89, 0xB285D165, 0x86829B36
	/* C */ 
.long	0xF764B11A, 0x76172146, 0xCEF6934D, 0xC6D28399 
.long	0xFE095F61, 0x5E6018B4, 0x5048ECF5, 0x51353261
.long	0x6E6E36DC, 0x63130DAD, 0xA9C69BD6, 0x1E90EA0C 
.long	0x7C35073B, 0x28D95E6D, 0xAA340E0D, 0xCB3DEE70

/******************************************************************************/	
/*
 * param ctx: r24,r25
 */
.global shabal384_init
shabal384_init:
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
	ldi r30, lo8(shabal384_iv)
	ldi r31, hi8(shabal384_iv)
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

.global shabal384_ctx2hash
shabal384_ctx2hash:
	movw r26, r24
	movw r30, r22
	ldd r24, Z+(8+2)
	ldd r25, Z+(8+2+1)
	movw r30, r24
	adiw r30, (16-384/32)*4
	ldi r24, 384/8
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
.global shabal384
shabal384: 
	push_range 8, 17
	stack_alloc_large 188
	adiw r30, 1 /* Z points to stack space (ctx) */
	movw CTX0, r30
	movw DST0, r24
	movw MSG0, r22
	movw LEN0, r18
	movw LEN2, r20
	movw r24, r30
	rcall shabal384_init
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
	rcall shabal384_ctx2hash
	stack_free_large2 188
	pop_range 8, 17
	ret
	
