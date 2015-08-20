/* twister-small-asm.S */
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
 * \file     twister-small-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2008-12-26
 * \license  GPLv3 or later
 * 
 */

#include "avr-asm-macros.S"

/* void twister_small_init(twister_state_t* ctx, uint16_t hashsize_b)*/
/*
 * param ctx:        r24:r25
 * param hashsize_b: r22:r23
 */
.global twister224_init
twister224_init:
	ldi r22, lo8(224)
	ldi r23, hi8(224)
	rjmp 1f

.global twister256_init
twister256_init:
	ldi r22, lo8(256)
	ldi r23, hi8(256)
	
.global twister_small_init
twister_small_init:
	movw r30, r24
	ldi r24, 64
1:
	st Z+, r1
	dec r24
	brne 1b

	dec r1		
	ldi r24, 8
1:
	st Z+, r1
	dec r24
	brne 1b

	inc r1		
	ldi r24, 8
1:
	st Z+, r1
	dec r24
	brne 1b

	sbiw r30, 1+8+8
	sbiw r30, 8*7
	st Z, r23
	std Z+8, r22
	ret
#if 1 
/*********************************************************************/
/* void twister_small_lastBlock(twister_state_t* ctx, void* msg, uint16_t length_b) */   
/*
 * param ctx:      r24:r25
 * param msg:      r22:r23
 * param length_b: r20:r21
 */
TMP_SAVE0 = 12
TMP_SAVE1 = 13
CTX_SAVE0 = 14
CTX_SAVE1 = 15
LEN_SAVE0 = 16
LEN_SAVE1 = 17
MSG_SAVE0 = 28
MSG_SAVE1 = 29
.global twister_small_lastBlock
.global twister224_lastBlock
.global twister256_lastBlock

twister224_lastBlock:
twister256_lastBlock:
twister_small_lastBlock:
	push_range 12, 17
	push r28
	push r29
	stack_alloc_large 64
	adiw r30, 1
	movw TMP_SAVE0, r30
	movw CTX_SAVE0, r24
	movw MSG_SAVE0, r22
	movw LEN_SAVE0, r20
1:	
	cpi LEN_SAVE1, 2
	brmi 2f
	movw r24, CTX_SAVE0
	movw r22, MSG_SAVE0
	rcall twister_small_nextBlock
	adiw MSG_SAVE0, 8
	subi LEN_SAVE1, 2
	rjmp 1b
2:
	movw r18, LEN_SAVE0
	lsr r19
	ror r18
	lsr r18
	lsr r18
	ldi r19, 63
	movw r26, MSG_SAVE0
	movw r30, TMP_SAVE0	
	ldi r20, 0x80
	sub r19, r18 /* r18: bytes to copy, r19: bytes to clear */

	ld r0, X+	
3:	tst r18
	breq 4f
31:
	st Z+, r0
	ld r0, X+
	dec r18
	brne 31b
4:	
	mov r18, LEN_SAVE0
	andi r18, 0x07
	ldi r20, 0x80
	breq 5f
4:
	lsr r20
	dec r18
	brne 4b
	or r20, r0
	rjmp 5f

5:
	st Z+, r20	

	tst r19
	breq 7f
6:	
	st Z+, r1
	dec r19
	brne 6b
7:	
	movw r24, CTX_SAVE0
	movw r22, TMP_SAVE0
	rcall twister_small_nextBlock
	
	ldi r19, 2
	clr r18
	
	sub r18, LEN_SAVE0
	sbc r19, LEN_SAVE1
	movw r26, CTX_SAVE0
	adiw r26, 63
	adiw r26, 1+8
	
	ld r0, X
	sub r0, r18
	st X+, r0
	ld r0, X
	sbc r0, r19
	st X+, r0
	ld r0, X
	sbc r0, r1
	st X+, r0
	ld r0, X
	sbc r0, r1
	st X+, r0
	ld r0, X
	sbc r0, r1
	st X+, r0
	ld r0, X
	sbc r0, r1
	st X+, r0
	ld r0, X
	sbc r0, r1
	st X+, r0
	ld r0, X
	sbc r0, r1
	st X+, r0
	
	sbiw r26, 8
	movw r24, CTX_SAVE0
	movw r22, r26
	rcall twister_mini_round	

	movw r24, CTX_SAVE0
	rcall twister_blank_round	
	
	stack_free_large 64
	pop r29
	pop r28
	pop_range 12, 17
	ret

#endif
