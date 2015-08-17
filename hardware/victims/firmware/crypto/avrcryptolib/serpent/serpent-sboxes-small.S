/* serpent_sboxes.S */
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
 * File:        serpent_sboxes.S
 * Author:      Daniel Otte
 * Date:        2008-08-07
 * License:     GPLv3 or later
 * Description: Implementation of the serpent sbox function.
 * 
 */
 
#include <avr/io.h>
#include "avr-asm-macros.S"


serpent_sbox:
 .byte 0x83, 0x1F, 0x6A, 0xB5, 0xDE, 0x24, 0x07, 0xC9
 .byte 0xCF, 0x72, 0x09, 0xA5, 0xB1, 0x8E, 0xD6, 0x43 
 .byte 0x68, 0x97, 0xC3, 0xFA, 0x1D, 0x4E, 0xB0, 0x25
 .byte 0xF0, 0x8B, 0x9C, 0x36, 0x1D, 0x42, 0x7A, 0xE5
 .byte 0xF1, 0x38, 0x0C, 0x6B, 0x52, 0xA4, 0xE9, 0xD7
 .byte 0x5F, 0xB2, 0xA4, 0xC9, 0x30, 0x8E, 0x6D, 0x17
 .byte 0x27, 0x5C, 0x48, 0xB6, 0x9E, 0xF1, 0x3D, 0x0A
 .byte 0xD1, 0x0F, 0x8E, 0xB2, 0x47, 0xAC, 0x39, 0x65

serpent_sbox_inv:
 .byte 0x3D, 0x0B, 0x6A, 0xC5, 0xE1, 0x74, 0x9F, 0x28
 .byte 0x85, 0xE2, 0x6F, 0x3C, 0x4B, 0x97, 0xD1, 0x0A
 .byte 0x9C, 0x4F, 0xEB, 0x21, 0x30, 0xD6, 0x85, 0x7A
 .byte 0x90, 0x7A, 0xEB, 0xD6, 0x53, 0x2C, 0x84, 0x1F
 .byte 0x05, 0x38, 0x9A, 0xE7, 0xC2, 0x6B, 0xF4, 0x1D
 .byte 0xF8, 0x92, 0x14, 0xED, 0x6B, 0x35, 0xC7, 0x0A
 .byte 0xAF, 0xD1, 0x35, 0x06, 0x94, 0x7E, 0xC2, 0xB8
 .byte 0x03, 0xD6, 0xE9, 0x8F, 0xC5, 0x7B, 0x1A, 0x24                  
 
/*
 * void ip(uint32_t *i, uint8_t *o){
 */
/* 
 * param i is given in r24:r25
 * parma o is given in r22:r23
 */
.global serpent_ip
serpent_ip:
	push_range 2, 17	
	movw r26, r24
	ldi r24, 16
	clr r31
	ldi r30, 2
1:
	ld r25, X+
	st Z+, r25
	dec r24
	brne 1b
	/* now the whole input is loaded in r2-r18 */
	movw r26, r22
	ldi r21, 4
4:
	ldi r20, 8	
2:	
	lsr r2
	ror r19
	lsr r6
	ror 19
	lsr r10
	ror r19
	lsr r14
	ror 19
	sbrc r20, 0
	st X+, r19
	dec r20
	brne 2b
	
	ldi r20, 15
	ldi r30, 2
3:
	ldd r19, Z+1
	st Z+, r19
	dec r20
	brne 3b
	
	dec r21
	brne 4b
	pop_range 2, 17 
 	ret

/*
 * void serpent_fp(uint32_t *i, uint8_t *o){
 */
/* 
 * param i is given in r24:r25
 * parma o is given in r22:r23
 */
.global serpent_fp
serpent_fp:
	movw r26, r24
	movw r30, r22
	ldi r18, 4
1:	
	ldi r19, 8 
2:
	sbrs r19, 0
	ld r24, X+
3:
	lsr r24
	ror r20
	lsr r24
	ror r21
	lsr r24
	ror r22
	lsr r24
	ror r23
	dec r19
	brne 2b
	
	st Z+, r20
	std Z+3, r21
	std Z+7, r22
	std Z+11, r23
	
	dec r18
	brne 1b 
	
	ret
/*
 * void inv_sbox128(void * w, uint8_t box)
 */
.global inv_sbox128 
inv_sbox128:
 	andi r22, 0x07
 	ori  r22, 0x08
 	rjmp sbox128x
 
/*	
 * void sbox128(void * w, uint8_t box);
 */
/*
 * param w   is passed in r24:r25
 * param box is passed in r22
 */
.global sbox128 
sbox128:
 	andi r22, 0x07

/*	
 * void sbox128x(void * w, uint8_t box);
 */
/*
 * param w   is passed in r24:r25
 * param box is passed in r22
 */
 
.global sbox128x
sbox128x:
	stack_alloc 16
	adiw r30, 1
	push_ r24, r25, r22, r30, r31
	movw r22, r30	/* Z points to the stack buffer */
	rcall serpent_ip
	pop_ r27, r26, r22
	ldi r25, hi8(serpent_sbox)
	ldi r24, lo8(serpent_sbox)
	swap r22
	lsr  r22 /* r22 *= 8 */
	add r24, r22
	adc r25, r1
	/* now we have X pointing to the buffer and (r24:r25) pointing to the SBox */
	ldi r22, 16
1:	
	movw r30, r24
	ld r18, X
	mov r20, r18
	andi r18, 0x0f
	bst r18, 0
	lsr r18
	add r30, r18
	adc r31, r1
	lpm r19, Z
	brtc 2f
	swap r19
2:	
	swap r20
	andi r20, 0x0f
	bst r20, 0
	lsr r20
	movw r30, r24
	add r30, r20
	adc r31, r1
	lpm r21, Z
	brts 3f
	swap r21
3:	
	andi r19, 0x0F
	andi r21, 0xF0
	or r19, r21
	st X+, r19
	dec r22
	brne 1b
	
	pop_ r23, r22
	movw r24, r26
	sbiw r24, 16

	rcall serpent_fp
	
	stack_free 16	
	ret
 



 


