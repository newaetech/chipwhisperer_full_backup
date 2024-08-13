/* serpent_asm.S */
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

/*
static void serpent_lt(uint8_t *b){
	X0 = rotl32(X0, 13);
	X2 = rotl32(X2,  3);
	X1 ^= X0 ^ X2;
	X3 ^= X2 ^ (X0 << 3);
	X1 = rotl32(X1, 1);
	X3 = rotl32(X3, 7);
	X0 ^= X1 ^ X3;
	X2 ^= X3 ^ (X1 << 7);
	X0 = rotl32(X0, 5);
	X2 = rotr32(X2, 10);
}
*/

#if 0
A0 =  4
A1 =  5
A2 =  6
A3 =  7
B0 =  8
B1 =  9
B2 = 10
B3 = 11
C0 = 12
C1 = 13
C2 = 14
C3 = 15
D0 = 16
D1 = 17
D2 = 18
D3 = 19
T0 = 20
T1 = 21
T2 = 22
T3 = 23

serpent_lt:
	push_range 4, 17
	movw r26, r24
	ld A2, X+
	ld A3, X+
	ld A0, X+
	ld A1, X+
	ldi r20, 3
	mov r0, A0
1:	
	lsr r0
	ror A3
	ror A2
	ror A1
	ror A0
	dec r20
	brne 1b
	ld B0, X+
	ld B1, X+
	ld B2, X+
	ld B3, X+
	
	ld C2, X+
	ld C3, X+
	ld C0, X+
	ld C1, X+
	ldi r20, 3
	mov r0, C0
1:	
	lsr r0
	ror C3
	ror C2
	ror C1
	ror C0
	dec r20
	brne 1b

	ld D0, X+
	ld D1, X+
	ld D2, X+
	ld D3, X+
	/* X1 ^= X0 ^ X2; */
	eor B0, A0
	eor B0, C0
	eor B1, A1
	eor B1, C1
	eor B2, A2
	eor B2, C2
	eor B3, A3
	eor B3, C3
	/* X3 ^= X2 ^ (X0 << 3); */
	mov T0, A0
	mov T1, A1
	mov T2, A2
	mov T3, A3
	ldi r24, 3
1:
	lsl T0
	rol T1
	rol T2
	rol T3
	dec r24
	brne 1b
	eor C0, B0
	eor C0, T0
	eor C1, B1
	eor C1, T1
	eor C2, B2
	eor C2, T2
	eor C3, B3
	eor C3, T3
	/*	X1 = rotl32(X1, 1); */
	mov r0, B3
	lsl r0
	rol B0
	rol B1
	rol B2
	rol B3
	/* X3 = rotl32(X3, 7); */
	mov r0, D3
	mov D3, D2
	mov D2, D1
	mov D1, D0
	mov D0, r0
	lsr r0
	ror D3
	ror D2
	ror D1
	ror D0
	/* 	X0 ^= X1 ^ X3; */
	eor A0, B0
	eor A0, D0
	eor A1, B1
	eor A1, D1
	eor A2, B2
	eor A2, D2
	eor A3, B3
	eor A3, D3
	/*  X2 ^= X3 ^ (X1 << 7); */
	mov T1, B0
	mov T2, B1
	mov T3, B2
	clr T0
	mov r0, B3
	lsr r0
	ror T2
	ror T1
	ror T0 
	eor C0, D0
	eor C0, T0
	eor C1, D1
	eor C1, T1
	eor C2, D2
	eor C2, T2
	eor C3, D3
	eor C3, T3
	/* 	X0 = rotl32(X0, 5); */
	ldi r24, 5
	mov r0, A3
1:	
	lsl r0
	rol A0
	rol A1
	rol A2
	rol A3
	dec r24
	brne 1b
	/* X2 = rotr32(X2, 10); */
	mov r0, C0
	mov C0, C1
	mov C1, C2
	mov C2, C3	
	mov C3, r0
	ldi r24, 2
1:
	lsr r0
	ror C2
	ror C1
	ror C0
	ror C3	
	dec r24
	brne 1b
	
	clr r31
	ldi r30, D3+1
	ldi r24, 16
1:
	ld r0, -Z
	st -X, r0	
	dec r24
	brne 1b
	
	pop_range 4, 17
	ret
#endif

T0 = 22
T1 = 23
T2 = 24
T3 = 25
TT = 21
/* rotate the data word (4 byte) pointed to by X by r20 bits to the right */
memrotr32:
	ld T0, X+
	ld T1, X+
	ld T2, X+
	ld T3, X+
	mov TT, T0
1:
	lsr TT
	ror T3
	ror T2
	ror T1
	ror T0
	dec r20
	brne 1b
	st -X, T3
	st -X, T2
	st -X, T1
	st -X, T0
	ret
	
/* rotate the data word (4 byte) pointed to by X by r20 bits to the left */
memrotl32:
	ld T0, X+
	ld T1, X+
	ld T2, X+
	ld T3, X+
	mov TT, T3
1:
	lsl TT
	rol T0
	rol T1
	rol T2
	rol T3
	dec r20
	brne 1b
	st -X, T3
	st -X, T2
	st -X, T1
	st -X, T0
	ret

/* xor the dataword (4 byte) pointed by Z into X */	
memeor32:
  ldi T2, 4
1:  
  ld T0, X		
  ld T1, Z+
  eor T0, T1
  st X+, T0
  dec T2
  brne 1b
  ret

serpent_lt:
	 /* X0 := X0 <<< 13 */
	movw r26, r24
	ldi r20, 7
	rcall memrotl32
	ldi r20, 6
	rcall memrotl32
	/* X2 := X2 <<< 3 */
	adiw r26, 8
	ldi r20, 3
	rcall memrotl32
	/* X1 ^= X2 */
	movw r30, r26
	sbiw r26, 4
	rcall memeor32
	/* X1 ^= X0 */
	sbiw r26, 4
	sbiw r30, 12
	rcall memeor32
	/* X3 ^= X2 */
	movw r30, r26
	adiw r26, 4
	rcall memeor32
	/* T := X0 */
	sbiw r26, 16
	ld r18, X+
	ld r19, X+
	ld r20, X+
	ld r21, X+
	/* T := T<<3 */
	ldi r22, 3
1:
	lsl r18
	rol r19
	rol r20
	rol r21
	dec r22
	brne 1b
	clr r31 
	/* X3 ^= T */
	adiw r26, 8
	ldi r30, 18
	rcall memeor32
	/* X1 := X1<<<1 */
	sbiw r26, 12
	ldi r20, 1
	rcall memrotl32
	/* X3 := X3<<<7 */
	adiw r26, 8
	ldi r20, 7
	rcall memrotl32
	/* X0 ^= X3 */
	movw r30, r26
	sbiw r26, 12
	rcall memeor32
	/* X0 ^= X1 */
	movw r30, r26
	sbiw r26, 4
	rcall memeor32
	/* X2 ^= X3 */
	adiw r26, 4
	adiw r30, 4
	rcall memeor32
	/* T := X1<<<8 */
	sbiw r26, 8
	ld r19, X+
	ld r20, X+
	ld r21, X+
	ld r18, X+
	/* T := T>>>1; T&=0xfffffff8 */
	lsr r18
	ror r21
	ror r20
	ror r19
	clr r18
	ror r18
	clr r31
	ldi r30, 18
	/* X2 ^= T */
	rcall memeor32
	/* X0 := X0 <<< 5 */
	sbiw r26, 12
	ldi r20, 5
	rcall memrotl32
	/* X3 := X3 >>> 10 */
	adiw r26, 8
	ldi r20, 7
	rcall memrotr32
	ldi r20, 3
	rcall memrotr32
	ret

serpent_inv_lt:
	 /* X0 := X0 >>> 5 */
	movw r26, r24
	ldi r20, 5
	rcall memrotr32
	/* X2 := X2 <<< 10 */
	adiw r26, 8
	ldi r20, 7
	rcall memrotl32
	ldi r20, 3
	rcall memrotl32
	/* X2 ^= X3 */
	movw r30, r26
	adiw r30, 4
	rcall memeor32
	sbiw r26, 4
	sbiw r30, 12
	/* T := X1<<7 */
	ld r19, Z+
	ld r20, Z+
	ld r21, Z+
	ld r18, Z+
	lsr r18
	ror r21
	ror r20
	ror r19
	clr r18
	ror r18
    clr r31
    /* X2 ^= T */
    ldi r30, 18
    rcall memeor32
    /* X0 ^= X1 */
    sbiw r26, 12
    movw r30, r26
    adiw r30, 4
    rcall memeor32
    /* X0 ^= X3 */
    sbiw r26, 4
    adiw r30, 4
    rcall memeor32
    /* X1 := X1>>>1 */
    ldi r20, 1
	rcall memrotr32
	/* X3 := X3>>>7 */
	adiw r26, 8
	ldi r20, 7
	rcall memrotr32
	/* X3 ^= X2 */
	sbiw r30, 8
	rcall memeor32
	sbiw r26, 4
	/* T:= X0<<3 */
	sbiw r30, 12
	ld r18, Z+
	ld r19, Z+
	ld r20, Z+
	ld r21, Z+
	ldi r24, 3
1:
	lsl r18
	rol r19
	rol r20
	rol r21
	dec r24
	brne 1b
	/* X3 ^= T */
	clr r31
	ldi r30, 18
	rcall memeor32
	/* X1 ^= X0 */
	sbiw r26, 12
	movw r30, r26
	sbiw r30, 4
	rcall memeor32
	/* X1 ^= X2 */
	movw r26, r30
	adiw r30, 4
	rcall memeor32
	/* X2 := X2 >>> 3 */
	ldi r20, 3
	rcall memrotr32
	/* X0 := X0 >>> 13 */
	sbiw r26, 8
	ldi r20, 7
	rcall memrotr32
	ldi r20, 6
	rcall memrotr32
	ret

/*
#define GOLDEN_RATIO 0x9e3779b9l

static uint32_t serpent_gen_w(uint32_t * b, uint8_t i){
	uint32_t ret;
	ret = b[0] ^ b[3] ^ b[5] ^ b[7] ^ GOLDEN_RATIO ^ (uint32_t)i;
	ret = rotl32(ret, 11);
	return ret;
}
*/
/*
 * param b is passed in r24:r25
 * param i is passed in r22
 * return value is returned in r22.r23.r24.r25
 */
 /* trashes:
  *  r20-r25, r30-r31
  */
serpent_gen_w:
	movw r30, r24
	/* ^i^b[0]*/
    ld r21, Z+
    eor r22, r21
    ld r23, Z+
    ld r24, Z+
    ld r25, Z+
    /* ^b[3]^b[5]^[b7] */
    adiw r30, 4
    ldi r20, 3
1:    
    adiw r30, 4
    ld r21, Z+
    eor r22, r21
    ld r21, Z+
    eor r23, r21
    ld r21, Z+
    eor r24, r21
    ld r21, Z+
    eor r25, r21
	dec r20
	brne 1b
	/* ^0x9e3779b9l */
	ldi r21, 0xb9
	eor r22, r21
	ldi r21, 0x79
	eor r23, r21
	ldi r21, 0x37
	eor r24, r21
	ldi r21, 0x9e
	eor r25, r21
	/* <<<11 */
	mov r21, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r21
	mov r21, r25
	ldi r20, 3
1:
	lsl r21
	rol r22
	rol r23
	rol r24
	rol r25
	dec r20
	brne 1b
	ret

/*
 * void serpent_init(const void* key, uint16_t keysize_b, serpent_ctx_t* ctx)
 */
/*
 * param key     is passed in r24:r25
 * param keysize is passed in r22:r23
 * param ctx     is passed in r20:r21
 */
.global serpent_init
serpent_init:
    stack_alloc 32
    adiw r30, 1
	push_ r30, r31
    movw r26, r22
    adiw r26, 7
    tst r27
    breq 1f
	ldi r26, 32
	rjmp 2f
1:
	lsr r26
	lsr r26
	lsr r26
2:	
	mov r22, r26
	bst r22, 5 /* store in T if we have to do the "append 1 thing"*/
	ldi r27, 32
3:	/* set buffer to zero */
	st Z+, r1
	dec r27
	brne 3b
	
	movw r26, r24 /* X points to the key */
	sbiw r30, 32
	tst r22
	breq 5f /* if keylength_b==0 */
4:	/* copy keybytes to buffer */
	ld r19, X+
	st Z+, r19
	dec r22
	brne 4b
5:
	brts 7f /* if keylength_b == 256 */
	ldi r18, 0x01
	andi r22, 0x07
	brne 6f
	st Z, r18
	rjmp 7f
6:	/* shift the one to the right position */
	lsl r18
	dec r22
	brne 6b
	or r18, r19
	st -Z, r18
7: /* post "appending 1 thing" buffer is ready for subkey generation */
	movw r26, r20  /* X points to the context */
	
	pop_ r19, r18 /* r18:r19 points to the buffer */
	push r16
	clr r16
8:
	movw r24, r18
	mov  r22, r16
	rcall serpent_gen_w
	movw r30, r18
	ldi r20, 7*4
1: /* the memmove */
	ldd r0, Z+4
	st Z+, r0
	dec r20
	brne 1b
  /* store new word in buffer and context */	
	st Z+, r22
	st Z+, r23
	st Z+, r24
	st Z+, r25
	st X+, r22
	st X+, r23
	st X+, r24
	st X+, r25
	
	inc r16
	cpi r16, 132
	brne 8b	
	
	push_ r28, r29
	movw r28, r26
	subi r28, lo8(132*4)
	sbci r29, hi8(132*4)
	ldi r16, 33
2:
	movw r24, r28
	adiw r28, 16
	ldi r22, 2
	add r22, r16
	rcall sbox128
	dec r16
	brne 2b
	pop_ r29, r28, r16
	stack_free 32
	ret

/*
 * void serpent_enc(void* buffer, const serpent_ctx_t* ctx){
 */
/*
 * param buffer is passed in r24:r25
 * param ctx    is passed in r22:r23
 */
.global serpent_enc
serpent_enc:

	push_ r12, r13, r14, r15, r16 
	clr r16
	movw r14, r24
	movw r12, r22
1:
	movw r24, r14
	movw r22, r12
	ldi r20, 16
	add r12, r20
	adc r13, r1
	clr r21
	rcall memxor
	movw r24, r14
	mov r22, r16
	rcall sbox128
	movw r24, r14
	rcall serpent_lt
	
	inc r16
	cpi r16, 31
	brne 1b
	
	movw r24, r14
	movw r22, r12
	ldi r20, 16
	add r12, r20
	adc r13, r1
	clr r21
	rcall memxor
	movw r24, r14
	mov r22, r16
	rcall sbox128
	
	inc r16
	movw r24, r14
	movw r22, r12
	ldi r20, 16
	clr r21
	pop_ r16, r15, r14, r13, r12
	rjmp memxor

/*
 * void serpent_dec(void* buffer, const serpent_ctx_t* ctx){
 */
/*
 * param buffer is passed in r24:r25
 * param ctx    is passed in r22:r23
 */
.global serpent_dec
serpent_dec:
	push_ r12, r13, r14, r15, r16 
	movw r14, r24
//	ldi r16, lo8(32*16)
//	add r22, r16
	ldi r16, hi8(32*16)
	add r23, r16
	movw r12, r22
	ldi r20, 16
	clr r21
	rcall memxor
	
	movw r24, r14
	ldi r22, 31
	call inv_sbox128
	
	movw r24, r14
	ldi r20, 16
	sub r12, r20
	sbc r13, r1
	movw r22, r12
	clr r21
	rcall memxor
	ldi r16, 31
1:
	dec r16
	movw r24, r14
	rcall serpent_inv_lt
	movw r24, r14
	mov r22, r16
	rcall inv_sbox128
	movw r24, r14
	ldi r20, 16
	sub r12, r20
	sbc r13, r1
	movw r22, r12
	clr r21
	rcall memxor
	
	tst r16
	brne 1b
	pop_ r16, r15, r14, r13, r12
	ret	
	
	
	














