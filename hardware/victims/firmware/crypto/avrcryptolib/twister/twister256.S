/* twister256.S */
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
 * \file     twister256.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2008-12-28
 * \license  GPLv3 or later
 * 
 */

#include "avr-asm-macros.S"
/*********************************************************************/
/* void twister256(void* dest, void* msg, uint32_t msg_length_b) */
/*
 * param dest:         r24:r25
 * param msg:          r22:r23
 * param msg_length_b: r18:r21
 */
 
CTX_SAVE0 = 10
CTX_SAVE1 = 11
DST_SAVE0 = 14
DST_SAVE1 = 15
MSG_LEN0  = 16
MSG_LEN1  = 17
MSG_LEN2  = 12
MSG_LEN3  = 13
MSG_SAVE0 = 28
MSG_SAVE1 = 29
 
.global twister256
twister256:
	push_range 10, 17
	push r28
	push r29
	stack_alloc_large 64+2*8
	adiw r30, 1
	movw CTX_SAVE0, r30
	movw DST_SAVE0, r24
	movw MSG_SAVE0, r22
	movw MSG_LEN0, r18
	movw MSG_LEN2, r20
	movw r24, CTX_SAVE0
	ldi r22, lo8(256)
	ldi r23, hi8(256)
	rcall twister_small_init
1:
	tst MSG_LEN3
	brne 2f
	tst MSG_LEN2
	brne 2f
	cpi MSG_LEN1, 2
	brmi 3f
2:
	movw r24, CTX_SAVE0
	movw r22, MSG_SAVE0
	rcall twister_small_nextBlock
	adiw MSG_SAVE0, 63
	adiw MSG_SAVE0,  1	
	subi MSG_LEN1, 2
	sbc MSG_LEN2, r1
	sbc MSG_LEN3, r1
	rjmp 1b
3:
	movw r24, CTX_SAVE0
	movw r22, MSG_SAVE0
	movw r20, MSG_LEN0
	rcall twister_small_lastBlock
	
	movw r24, DST_SAVE0
	movw r22, CTX_SAVE0
	ldi r20, lo8(256)
	ldi r21, hi8(256)
	rcall twister_small_ctx2hash

	stack_free_large 64+2*8
	pop r29
	pop r28
	pop_range 10, 17
	ret


