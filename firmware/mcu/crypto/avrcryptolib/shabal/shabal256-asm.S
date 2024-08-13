/* shabal256-asm.S */
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
 * \file    shabal256-asm.S
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

/******************************************************************************/	

shabal256_iv:
	/* A */ 
.long	0x52F84552, 0xE54B7999, 0x2D8EE3EC, 0xB9645191 
.long	0xE0078B86, 0xBB7C44C9, 0xD2B5C1CA, 0xB0D2EB8C
.long	0x14CE5A45, 0x22AF50DC, 0xEFFDBC6B, 0xEB21B74A
	/* B */ 
.long	0xB555C6EE, 0x3E710596, 0xA72A652F, 0x9301515F 
.long	0xDA28C1FA, 0x696FD868, 0x9CB6BF72, 0x0AFE4002
.long	0xA6E03615, 0x5138C1D4, 0xBE216306, 0xB38B8890 
.long	0x3EA8B96B, 0x3299ACE4, 0x30924DD4, 0x55CB34A5
	/* C */ 
.long	0xB405F031, 0xC4233EBA, 0xB3733979, 0xC0DD9D55
.long	0xC51C28AE, 0xA327B8E1, 0x56C56167, 0xED614433
.long	0x88B59D60, 0x60E2CEBA, 0x758B4B8B, 0x83E82A7F 
.long	0xBC968828, 0xE6E00BF7, 0xBA839E55, 0x9B491C60

/******************************************************************************/	
/*
 * param ctx: r24,r25
 */
.global shabal256_init
shabal256_init:
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
	ldi r30, lo8(shabal256_iv)
	ldi r31, hi8(shabal256_iv)
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

.global shabal256_ctx2hash
shabal256_ctx2hash:
	movw r26, r24
	movw r30, r22
	ldd r24, Z+(8+2)
	ldd r25, Z+(8+2+1)
	movw r30, r24
	adiw r30, (16-256/32)*4
	ldi r24, 256/8
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
.global shabal256
shabal256: 
	push_range 8, 17
	stack_alloc_large 188
	adiw r30, 1 /* Z points to stack space (ctx) */
	movw CTX0, r30
	movw DST0, r24
	movw MSG0, r22
	movw LEN0, r18
	movw LEN2, r20
	movw r24, r30
	rcall shabal256_init
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
	rcall shabal256_ctx2hash
	stack_free_large2 188
	pop_range 8, 17
	ret
	

