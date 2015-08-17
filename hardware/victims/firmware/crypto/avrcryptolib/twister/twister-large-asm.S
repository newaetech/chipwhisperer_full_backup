/* twister-large-asm.S */
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
 * \file     twister-large-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2008-12-27
 * \license  GPLv3 or later
 * 
 */
 
 #include "avr-asm-macros.S"

/* void checksum_update(twister_large_ctx_t* ctx, uint8_t col) */
/*
 * param ctx: r24:r25
 * param col: r22
 */
checksum_update:
	push r16
	push r28
	push r29
	lsl r22
	lsl r22
	lsl r22
	mov r16, r22
	movw r30, r24 /* X points to ctx->state.s */
	
	ldd r18, Z+7*8
	ldd r19, Z+6*8
	ldd r20, Z+5*8
	ldd r21, Z+4*8
	ldd r22, Z+3*8
	ldd r23, Z+2*8
	ldd r24, Z+1*8
	ldd r25, Z+0*8
	
	adiw r30, 63
	adiw r30, 1+3*8	/* Z points at ctx->checksum[0][8] */
	movw r28, r30	/* Y points at ctx->checksum[0][8] */
	andi r16, 63
	add r30, r16   /* Z points at ctx->checksum[col][8]*/
	adc r31, r1
	ldi r26, 8
	add r16, r26
	andi r16, 63
	add r28, r16
	adc r29, r1    /* Y points at ctx->checksum[(col+1)%8][8]*/
	
	ld r0, -Y
	add r18, r0
	ld r0, -Z
	eor r0, r18
	st Z, r0
	
	ld r0, -Y
	adc r19, r0
	ld r0, -Z
	eor r0, r19
	st Z, r0
	
	ld r0, -Y
	adc r20, r0
	ld r0, -Z
	eor r0, r20
	st Z, r0
	
	ld r0, -Y
	adc r21, r0
	ld r0, -Z
	eor r0, r21
	st Z, r0
	
	ld r0, -Y
	adc r22, r0
	ld r0, -Z
	eor r0, r22
	st Z, r0
	
	ld r0, -Y
	adc r23, r0
	ld r0, -Z
	eor r0, r23
	st Z, r0
	
	ld r0, -Y
	adc r24, r0
	ld r0, -Z
	eor r0, r24
	st Z, r0
	
	ld r0, -Y
	adc r25, r0
	ld r0, -Z
	eor r0, r25
	st Z, r0
	
	pop r29
	pop r28
	pop r16
	ret

/*********************************************************************/
/* void twister_large_init(twister_large_ctx_t* ctx, uint16_t hashsize_b)*/
/* 
 * param ctx:        r24:r25
 * param hashsize_b: r22:r23
 */
.global twister384_init
twister384_init:
	ldi r22, lo8(384)
	ldi r23, hi8(384)
	rjmp twister_large_init
	
.global twister512_init
twister512_init:
	ldi r22, lo8(512)
	ldi r23, hi8(512)
	
.global twister_large_init
twister_large_init:
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
	ldi r24, 8+64
1:
	st Z+, r1
	dec r24
	brne 1b

	subi r30, lo8(1+8+8+8*7+64)
	sbci r31, hi8(1+8+8+8*7+64)
	st Z, r23
	std Z+8, r22
	ret

/*********************************************************************/
/* void twister_large_nextBlock(twister_state_t* ctx, void* msg) */   
/*
 * param ctx: r24:r25
 * param msg: r22:r23
 */
CTX_SAVE0 = 14
CTX_SAVE1 = 15
TMP_SAVE0 = 12
TMP_SAVE1 = 13
MSG_SAVE0 = 28
MSG_SAVE1 = 29
.global twister_large_nextBlock
.global twister384_nextBlock
.global twister512_nextBlock

twister384_nextBlock:
twister512_nextBlock:
twister_large_nextBlock:
	push_range 12, 15
	push r28
	push r29
	stack_alloc_large 64
	adiw r30, 1
	movw TMP_SAVE0, r30
	movw CTX_SAVE0, r24
	movw MSG_SAVE0, r22
	movw r26, CTX_SAVE0
	ldi r18, 64/8
1:
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	dec r18
	brne 1b
	/* maxi round 1 */
	movw r24, CTX_SAVE0 
	ldi r22, 0
	rcall checksum_update
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	movw r24, CTX_SAVE0 
	ldi r22, 1
	rcall checksum_update
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	movw r24, CTX_SAVE0 
	ldi r22, 2
	rcall checksum_update
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	movw r30, TMP_SAVE0
	movw r26, CTX_SAVE0
	ldi r18, 64
1:
	ld r0, X
	ld r23, Z
	eor r0, r23
	st X+, r0
	st Z+, r0
	dec r18
	brne 1b
	/* maxi round 2 */
	movw r24, CTX_SAVE0 
	ldi r22, 3
	rcall checksum_update
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	movw r24, CTX_SAVE0 
	rcall twister_blank_round
	
	movw r24, CTX_SAVE0 
	ldi r22, 4
	rcall checksum_update
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round

	movw r30, TMP_SAVE0
	movw r26, CTX_SAVE0
	ldi r18, 64
1:
	ld r0, X
	ld r23, Z
	eor r0, r23
	st X+, r0
	st Z+, r0
	dec r18
	brne 1b
	/* maxi round 3 */
	movw r24, CTX_SAVE0 
	ldi r22, 5
	rcall checksum_update
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	movw r24, CTX_SAVE0 
	ldi r22, 6
	rcall checksum_update
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	movw r24, CTX_SAVE0 
	ldi r22, 7
	rcall checksum_update
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	movw r24, CTX_SAVE0 
	rcall twister_blank_round

	movw r30, TMP_SAVE0
	movw r26, CTX_SAVE0
	ldi r18, 64
1:
	ld r0, X
	ld r23, Z+
	eor r0, r23
	st X+, r0
	dec r18
	brne 1b
	
	adiw r26, 9
	ldi r19, 2
	ld r0, X
	add r0, r19
	st X+, r0
		
	ld r0, X
	adc r0, r1
	st X+, r0
	ld r0, X
	adc r0, r1
	st X+, r0
	ld r0, X
	adc r0, r1
	st X+, r0
	ld r0, X
	adc r0, r1
	st X+, r0
	ld r0, X
	adc r0, r1
	st X+, r0
	ld r0, X
	adc r0, r1
	st X+, r0
	ld r0, X
	adc r0, r1
	st X+, r0
	
	stack_free_large 64
	pop r29
	pop r28
	pop_range 12, 15
	ret
	
/*********************************************************************/
/* void twister_large_lastBlock(twister_state_t* ctx, void* msg, uint16_t length_b) */   
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
.global twister_large_lastBlock
.global twister384_lastBlock
.global twister512_lastBlock

twister384_lastBlock:
twister512_lastBlock:
twister_large_lastBlock:
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
	rcall twister_large_nextBlock
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
3:
	tst r18
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
	rcall twister_large_nextBlock
	
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
	movw r22, CTX_SAVE0
	ldi r16, 64+8+8
	add r22, r16
	adc r23, r1
	movw r30, r22
	ldi r26, 8
1:	
	ld r12, Z+
	ld r13, Z+
	ld r16, Z+
	ld r17, Z+
	ld r18, Z+
	ld r19, Z+
	ld r20, Z+
	ld r21, Z+
	st -Z, r12
	st -Z, r13
	st -Z, r16
	st -Z, r17
	st -Z, r18
	st -Z, r19
	st -Z, r20
	st -Z, r21
	adiw r30, 8
	dec r26
	brne 1b
	
	movw r24, CTX_SAVE0
	movw r22, CTX_SAVE0
	ldi r26, 64+2*8
	add r22, r26
	adc r23, r1
	rcall twister_small_nextBlock
	
	stack_free_large 64
	pop r29
	pop r28
	pop_range 12, 17
	ret
