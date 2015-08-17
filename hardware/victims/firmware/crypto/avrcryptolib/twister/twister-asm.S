/* twister-asm.S */
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
 * \file     twister-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2008-12-22
 * \license  GPLv3 or later
 * 
 */

#include "avr-asm-macros.S"

twister_sbox: 
.byte    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5
.byte    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76
.byte    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0
.byte    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0
.byte    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc
.byte    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15
.byte    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a
.byte    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75
.byte    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0
.byte    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84
.byte    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b
.byte    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf
.byte    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85
.byte    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8
.byte    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5
.byte    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2
.byte    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17
.byte    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73
.byte    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88
.byte    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb
.byte    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c
.byte    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79
.byte    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9
.byte    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08
.byte    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6
.byte    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a
.byte    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e
.byte    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e
.byte    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94
.byte    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf
.byte    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68
.byte    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16

/*
 * param ctx: r24:r25
 * param msg: r22:r23
 */ 
.global twister_mini_round
twister_mini_round:
	movw r26, r24
	movw r30, r22
	adiw r26, 8*7
	adiw r30, 8
	ldi r21, 8
1:	
	ld r22, X
	ld r23, -Z
	eor r22, r23
	st X+, r22
	dec r21
	brne 1b
 
/*
 * param ctx: r24:r25
 *
 */
X_SAVE0 = 4
X_SAVE1 = 5 
Y_SAVE0 = 6
Y_SAVE1 = 7
MDS0 =  8
MDS1 =  9
MDS2 = 10
MDS3 = 11
MDS4 = 12
MDS5 = 13
MDS6 = 14
MDS7 = 15


.global twister_blank_round
twister_blank_round:
	push_range 4, 17
	push r28
	push r29
	stack_alloc_large 64, r28, r29
	
	movw X_SAVE0, r24
	movw r30, r24
	adiw r30, 63
	adiw r30, 1+8 /* Z points behind counter */
	movw r26, r24
	adiw r26, 1
	ldi r22, 8
1: /* "add" counter */
	ld r16, -Z
	ld r21, X
	eor r21, r16
	st X, r21
	adiw r26, 8
	dec r22
	brne 1b

	/* decrement counter */
	subi r16, 1
	st Z+, r16
	ldi r17, 7
1:	
	ld r16, Z
	sbci r16, 0
	st Z+, r16
	dec r17
	brne 1b

	movw r26, r24
	adiw r28, 1 /* Y points to stack memory */
	movw Y_SAVE0, r28	
	movw r24, r28
	ldi r20, lo8(twister_sbox)
	ldi r21, hi8(twister_sbox)
	ldi r18, 8
1:
	ldi r19, 0
	
2:	/* sbox substitution */
	ld r0, X+
	movw r30, r20
	add r30, r0
	adc r31, r1	
	lpm r0, Z
	movw r28, r24
	mov r16, r18
	add r16, r19
	andi r16, 0x07
	add r28, r16
	adc r29, r1
	st Y, r0
	inc r19
	cpi r19, 8
	brne 2b
	adiw r24, 8
	dec r18
	brne 1b
		
	/* load MDS-Table to MDS0:MDS7 */
	ldi r18, 1
	mov MDS1, r18
	mov MDS2, r18
	mov MDS7, r18
	ldi r18, 2
	mov MDS0, r18
	ldi r18, 5
	mov MDS3, r18
	ldi r18, 6
	mov MDS6, r18
	ldi r18, 7
	mov MDS4, r18
	ldi r18, 8
	mov MDS5, r18
	
	ldi r20, 0x4D /* reducer for gf256mul*/
	ldi r16, 0

1:
	movw r26, X_SAVE0
	add r26, r16
	adc r27, r1
	ldi r17, 8
2:		
	mov r24, MDS0
	movw r28, Y_SAVE0
	add r28, r16
	adc r29, r1
	ld  r22, Y
	rcall gf256mul
	mov r0, r24
	
	mov r24, MDS1
	ldd r22, Y+8
	rcall gf256mul
	eor r0, r24

	mov r24, MDS2
	ldd r22, Y+8*2
	rcall gf256mul
	eor r0, r24

	mov r24, MDS3
	ldd r22, Y+8*3
	rcall gf256mul
	eor r0, r24

	mov r24, MDS4
	ldd r22, Y+8*4
	rcall gf256mul
	eor r0, r24

	mov r24, MDS5
	ldd r22, Y+8*5
	rcall gf256mul
	eor r0, r24

	mov r24, MDS6
	ldd r22, Y+8*6
	rcall gf256mul
	eor r0, r24

	mov r24, MDS7
	ldd r22, Y+8*7
	rcall gf256mul
	eor r0, r24

	st X, r0
	adiw r26, 8

	mov r0, MDS7
	mov MDS7, MDS6
	mov MDS6, MDS5
	mov MDS5, MDS4
	mov MDS4, MDS3
	mov MDS3, MDS2
	mov MDS2, MDS1
	mov MDS1, MDS0
	mov MDS0, r0	
	
	dec r17
	brne 2b
8:	
	inc r16
	cpi r16, 8
	brne 1b
	
9:
	stack_free_large 64
	pop r29
	pop r28
	pop_range 4, 17
	ret

/*********************************************************************/
A = 23
B = 22
P = 24

gf256mul:
	mov A, r24
	clr P
1:	
	lsr A
	breq 4f
	brcc 2f
	eor P, B
2:
	lsl B
	brcc 3f
	eor B, r20	
3:
	rjmp 1b
4:
	brcc 2f
	eor P, B
2:
	ret

/*********************************************************************/
/* twister_ctx2hash */
/*
 * param dest:       r24:r25
 * param ctx:        r22:r23
 * param hashsize_b: r20:r21
 */
DEST_SAVE0 = 10
DEST_SAVE1 = 11
CTX_SAVE0  = 12
CTX_SAVE1  = 13
LEN_SAVE  = 14
LEN32_SAVE  = 15
TMP_SAVE0  = 16
TMP_SAVE1  = 17


.global twister_ctx2hash
.global twister_small_ctx2hash
.global twister_large_ctx2hash
.global twister224_ctx2hash
.global twister256_ctx2hash
.global twister384_ctx2hash
.global twister512_ctx2hash

twister224_ctx2hash:
	ldi r20, lo8(224)
	ldi r21, hi8(224)
	rjmp twister_ctx2hash

twister256_ctx2hash:
	ldi r20, lo8(256)
	ldi r21, hi8(256)
	rjmp twister_ctx2hash

twister384_ctx2hash:
	ldi r20, lo8(384)
	ldi r21, hi8(384)
	rjmp twister_ctx2hash

twister512_ctx2hash:
	ldi r20, lo8(512)
	ldi r21, hi8(512)
;	rjmp twister_ctx2hash

twister_large_ctx2hash:
twister_small_ctx2hash:
twister_ctx2hash:
	push_range 10, 17
	push r28
	push r29
	stack_alloc_large 64
	movw DEST_SAVE0, r24
	movw CTX_SAVE0, r22
	clr LEN32_SAVE
	sbrc r20, 5
	inc LEN32_SAVE
	lsr r21
	ror r20
	lsr r21
	ror r20 /* length is max 512 so we now only have to shift r20 */
	swap r20 /* this is faster than 4 shifts */
	andi r20, 0x0f
	add r20, LEN32_SAVE
	mov LEN_SAVE, r20
	
	adiw r30, 1
	movw TMP_SAVE0, r30
1:	
	dec LEN_SAVE
	brmi 9f
	/* tmp <- ctx-s */
	movw r30, TMP_SAVE0
	movw r26, CTX_SAVE0
	ldi r20, 64/4
3:	
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	ld r0, X+
	st Z+, r0
	dec r20
	brne 3b
	
	movw r24, CTX_SAVE0
	rcall twister_blank_round
	/* ctx-s ^= tmp */
	movw r30, TMP_SAVE0
	movw r26, CTX_SAVE0
	ldi r20, 64
3:
	ld r0, X
	ld r21, Z+
	eor r0, r21
	st X+, r0
	dec r20
	brne 3b

	movw r24, CTX_SAVE0
	rcall twister_blank_round


	movw r26, CTX_SAVE0
	
	tst LEN_SAVE
	brne 2f
	tst LEN32_SAVE
	brne 5f
2:	
	adiw r26, 8*7
	movw r30, TMP_SAVE0
	adiw r30, 8*7
	movw r28, DEST_SAVE0
	ldi r20, 8
3:
	ld r0, Z
	ld r21, X
	eor r0, r21
	st Y+, r0
	sbiw r26, 8
	sbiw r30, 8
	dec r20
	brne 3b 
	movw DEST_SAVE0, r28
7:
	rjmp 1b

5:
	adiw r26, 8*3
	movw r30, TMP_SAVE0
	adiw r30, 8*3
	movw r28, DEST_SAVE0
	ldi r20, 4
3:
	ld r0, Z
	ld r21, X
	eor r0, r21
	st Y+, r0
	sbiw r26, 8
	sbiw r30, 8
	dec r20
	brne 3b 
	
9:	
	stack_free_large 64
	pop r29
	pop r28
	pop_range 10, 17
	ret



/*********************************************************************/
/* void twister_small_nextBlock(twister_state_t* ctx, void* msg) */   
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
.global twister_small_nextBlock
.global twister224_nextBlock
.global twister256_nextBlock

twister224_nextBlock:
twister256_nextBlock:
twister_small_nextBlock:
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

	rcall twister_mini_round
	
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
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

	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
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

	adiw MSG_SAVE0, 8
	movw r22, MSG_SAVE0
	movw r24, CTX_SAVE0 
	rcall twister_mini_round
	
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








