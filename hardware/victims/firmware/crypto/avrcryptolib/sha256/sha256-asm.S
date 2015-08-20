/* sha256-asm.S */
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
 * Author:	Daniel Otte
 *
 * License: GPLv3 or later
*/
; sha-256 implementation in assembler
SHA256_BLOCK_BITS = 512
SHA256_HASH_BITS = 256


.macro precall
	/* push r18 - r27, r30 - r31*/
	push r0
	push r1
	push r18
	push r19
	push r20
	push r21
	push r22
	push r23
	push r24
	push r25
	push r26
	push r27
	push r30
	push r31
	clr r1
.endm

.macro postcall
	pop r31
	pop r30
	pop r27
	pop r26
	pop r25
	pop r24
	pop r23
	pop r22
	pop r21
	pop r20
	pop r19
	pop r18
	pop r1
	pop r0
.endm


.macro hexdump length
	push r27
	push r26
	ldi r25, '\r'
	mov r24, r25
	call uart_putc
	ldi r25, '\n'
	mov r24, r25
	call uart_putc
	pop r26
	pop r27
	movw r24, r26
.if \length > 16
	ldi r22, lo8(16)
	ldi r23, hi8(16)
	push r27
	push r26
	call uart_hexdump
	pop r26
	pop r27
	adiw r26, 16
	hexdump \length-16
.else
	ldi r22, lo8(\length)
	ldi r23, hi8(\length)
	call uart_hexdump
.endif
.endm

/* X points to Block */
.macro dbg_hexdump length
	precall
	hexdump \length
	postcall
.endm

.section .text

SPL = 0x3D
SPH = 0x3E
SREG = 0x3F


;
;sha256_ctx_t is:
;
; [h0][h1][h2][h3][h4][h5][h6][h7][length]
; hn is 32 bit large, length is 64 bit large

;###########################################################

.global sha256_ctx2hash
; === sha256_ctx2hash ===
; this function converts a state into a normal hash (bytestring)
;  param1: the 16-bit destination pointer
;	given in r25,r24 (r25 is most significant)
;  param2: the 16-bit pointer to sha256_ctx structure
;	given in r23,r22
sha256_ctx2hash:
	movw r26, r22
	movw r30, r24
	ldi r21, 8
	sbiw r26, 4
1:
	ldi r20, 4
	adiw r26, 8
2:
		ld r0, -X
		st Z+, r0
	dec r20
	brne 2b

	dec r21
	brne 1b

	ret

;###########################################################

.global sha256
; === sha256 ===
; this function calculates SHA-256 hashes from messages in RAM
;  param1: the 16-bit hash destination pointer
;	given in r25,r24 (r25 is most significant)
;  param2: the 16-bit pointer to message
;	given in r23,r22
;  param3: 32-bit length value (length of message in bits)
;   given in r21,r20,r19,r18
sha256:
sha256_prolog:
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r16
	push r17
	in r30, SPL
	in r31, SPH
	sbiw r30, 8*4+8
	in r0, SREG
	cli
	out SPL, r30
	out SREG, r0
	out SPH, r31

	push r25
	push r24
	adiw r30, 1
	movw r16, r30
	movw r8, r18		/* backup of length*/
	movw r10, r20

	movw r12, r22	/* backup pf msg-ptr */

	movw r24, r16
	rcall sha256_init
	/* if length > 0xffff */
1:
	tst r11
	brne 2f
	tst r10
	breq 4f
2:
	movw r24, r16
	movw r22, r12
	rcall sha256_nextBlock
	ldi r19, 64
	add r12, r19
	adc r13, r1
	/* length -= 512 */
	ldi r19, 0x02
	sub r9, r19
	sbc r10, r1
	sbc r11, r1
	rjmp 1b

4:
	movw r24, r16
	movw r22, r12
	movw r20, r8
	rcall sha256_lastBlock

	pop r24
	pop r25
	movw r22, r16
	rcall sha256_ctx2hash

sha256_epilog:
	in r30, SPL
	in r31, SPH
	adiw r30, 8*4+8
	in r0, SREG
	cli
	out SPL, r30
	out SREG, r0
	out SPH, r31
	pop r17
	pop r16
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	ret

;###########################################################


; block MUST NOT be larger than 64 bytes

.global sha256_lastBlock
; === sha256_lastBlock ===
; this function does padding & Co. for calculating SHA-256 hashes
;  param1: the 16-bit pointer to sha256_ctx structure
;	given in r25,r24 (r25 is most significant)
;  param2: an 16-bit pointer to 64 byte block to hash
;	given in r23,r22
;  param3: an 16-bit integer specifing length of block in bits
;	given in r21,r20
sha256_lastBlock_localSpace = (SHA256_BLOCK_BITS/8+1)


sha256_lastBlock:
	cpi r21, 0x02
	brlo sha256_lastBlock_prolog
	push r25
	push r24
	push r23
	push r22
	push r21
	push r20
	rcall sha256_nextBlock
	pop r20
	pop r21
	pop r22
	pop r23
	pop r24
	pop r25
	subi r21, 0x02
	ldi r19, 64
	add r22, r19
	adc r23, r1
	rjmp sha256_lastBlock
sha256_lastBlock_prolog:
	/* allocate space on stack */
	in r30, SPL
	in r31, SPH
	in r0, SREG
	subi r30, lo8(64)
	sbci r31, hi8(64)
	cli
	out SPL, r30
	out SREG,r0
	out SPH, r31

	adiw r30, 1 /* SP points to next free byte on stack */
	mov r18, r20 /* r20 = LSB(length) */
	lsr r18
	lsr r18
	lsr r18
	bst r21, 0	/* may be we should explain this ... */
	bld r18, 5  /* now: r18 == length/8 (aka. length in bytes) */


	movw r26, r22 /* X points to begin of msg */
	tst r18
	breq sha256_lastBlock_post_copy
	mov r1, r18
sha256_lastBlock_copy_loop:
	ld r0, X+
	st Z+, r0
	dec r1
	brne sha256_lastBlock_copy_loop
sha256_lastBlock_post_copy:
sha256_lastBlock_insert_stuffing_bit:
	ldi r19, 0x80
	mov r0,r19
	ldi r19, 0x07
	and r19, r20 /* if we are in bitmode */
	breq 2f	/* no bitmode */
1:
	lsr r0
	dec r19
	brne 1b
	ld r19, X
/* maybe we should do some ANDing here, just for safety */
	or r0, r19
2:
	st Z+, r0
	inc r18

/* checking stuff here */
	cpi r18, 64-8+1
	brsh 0f
	rjmp sha256_lastBlock_insert_zeros
0:
	/* oh shit, we landed here */
	/* first we have to fill it up with zeros */
	ldi r19, 64
	sub r19, r18
	breq 2f
1:
	st Z+, r1
	dec r19
	brne 1b
2:
	sbiw r30, 63
	sbiw r30,  1
	movw r22, r30

	push r31
	push r30
	push r25
	push r24
	push r21
	push r20
	rcall sha256_nextBlock
	pop r20
	pop r21
	pop r24
	pop r25
	pop r30
	pop r31

	/* now we should subtract 512 from length */
	movw r26, r24
	adiw r26, 4*8+1 /* we can skip the lowest byte */
	ld r19, X
	subi r19, hi8(512)
	st X+, r19
	ldi r18, 6
1:
	ld r19, X
	sbci r19, 0
	st X+, r19
	dec r18
	brne 1b

;	clr r18 /* not neccessary ;-) */
	/* reset Z pointer to begin of block */

sha256_lastBlock_insert_zeros:
	ldi r19, 64-8
	sub r19, r18
	breq sha256_lastBlock_insert_length
	clr r1
1:
	st Z+, r1	/* r1 is still zero */
	dec r19
	brne 1b

;	rjmp sha256_lastBlock_epilog
sha256_lastBlock_insert_length:
	movw r26, r24	/* X points to state */
	adiw r26, 8*4	/* X points to (state.length) */
	adiw r30, 8		/* Z points one after the last byte of block */
	ld r0, X+
	add r0, r20
	st -Z, r0
	ld r0, X+
	adc r0, r21
	st -Z, r0
	ldi r19, 6
1:
	ld r0, X+
	adc r0, r1
	st -Z, r0
	dec r19
	brne 1b

	sbiw r30, 64-8
	movw r22, r30
	rcall sha256_nextBlock

sha256_lastBlock_epilog:
	in r30, SPL
	in r31, SPH
	in r0, SREG
	adiw r30, 63 ; lo8(64)
	adiw r30,  1  ; hi8(64)
	cli
	out SPL, r30
	out SREG,r0
	out SPH, r31
	clr r1
	ret

/**/
;###########################################################

.global sha256_nextBlock
; === sha256_nextBlock ===
; this is the core function for calculating SHA-256 hashes
;  param1: the 16-bit pointer to sha256_ctx structure
;	given in r25,r24 (r25 is most significant)
;  param2: an 16-bit pointer to 64 byte block to hash
;	given in r23,r22
sha256_nextBlock_localSpace = (64+8)*4 ; 64 32-bit values for w array and 8 32-bit values for a array (total 288 byte)

Bck1 = 12
Bck2 = 13
Bck3 = 14
Bck4 = 15
Func1 = 22
Func2 = 23
Func3 = 24
Func4 = 25
Accu1 = 16
Accu2 = 17
Accu3 = 18
Accu4 = 19
XAccu1 = 8
XAccu2 = 9
XAccu3 = 10
XAccu4 = 11
T1	= 4
T2	= 5
T3	= 6
T4	= 7
LoopC = 1
/* byteorder: high number <--> high significance */
sha256_nextBlock:
 ; initial, let's make some space ready for local vars
	push r4 /* replace push & pop by mem ops? */
	push r5
	push r6
	push r7
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push r16
	push r17
	push r28
	push r29
	in r20, SPL
	in r21, SPH
	movw r18, r20			;backup SP
;	movw r26, r20			; X points to free space on stack
	movw r30, r22			; Z points to message
	subi r20, lo8(sha256_nextBlock_localSpace) ;sbiw can do only up to 63
	sbci r21, hi8(sha256_nextBlock_localSpace)
	movw r26, r20			; X points to free space on stack
	in r0, SREG
	cli ; we want to be uninterrupted while updating SP
	out SPL, r20
	out SREG, r0
	out SPH, r21
	push r18
	push r19
	push r24
	push r25 /* param1 will be needed later */
 ; now we fill the w array with message (think about endianess)
 	adiw r26, 1 ; X++
 	ldi r20, 16
sha256_nextBlock_wcpyloop:
 	ld r23, Z+
 	ld r22, Z+
 	ld r19, Z+
 	ld r18, Z+
 	st X+, r18
 	st X+, r19
 	st X+, r22
	st X+, r23
	dec r20
	brne sha256_nextBlock_wcpyloop
/*	for (i=16; i<64; ++i){
		w[i] = SIGMA_b(w[i-2]) + w[i-7] + SIGMA_a(w[i-15]) + w[i-16];
	} */
	/* r25,r24,r23,r24 (r21,r20) are function values
	   r19,r18,r17,r16 are the accumulator
	   r15,r14,r13,rBck1 are backup1
	   r11,r10,r9 ,r8  are xor accu
	   r1 is round counter 								*/

	ldi r20, 64-16
	mov LoopC, r20
sha256_nextBlock_wcalcloop:
	movw r30, r26 ; cp X to Z
	sbiw r30, 63
	sbiw r30, 1 		; substract 64 = 16*4
	ld Accu1, Z+
	ld Accu2, Z+
	ld Accu3, Z+
	ld Accu4, Z+ /* w[i] = w[i-16] */
	ld Bck1, Z+
	ld Bck2, Z+
	ld Bck3, Z+
	ld Bck4, Z+ /* backup = w[i-15] */
	/* now sigma 0 */
	mov Func1, Bck2
	mov Func2, Bck3
	mov Func3, Bck4
	mov Func4, Bck1  /* prerotated by 8 */
	ldi r20, 1
	rcall bitrotl
	movw XAccu1, Func1
	movw XAccu3, Func3	 /* store ROTR(w[i-15],7) in xor accu */
	movw Func1, Bck3
	movw Func3, Bck1 /* prerotated by 16 */
	ldi r20, 2
	rcall bitrotr
	eor XAccu1, Func1  /* xor ROTR(w[i-15], 18)*/
	eor XAccu2, Func2
	eor XAccu3, Func3
	eor XAccu4, Func4
	ldi Func2, 3		 /* now shr3 */ /*we can destroy backup now*/
sigma0_shr:
	lsr Bck4
	ror Bck3
	ror Bck2
	ror Bck1
	dec Func2
	brne sigma0_shr
	eor XAccu1, Bck1
	eor XAccu2, Bck2
	eor XAccu3, Bck3
	eor XAccu4, Bck4	/* xor SHR(w[i-15], 3)*/ /* xor accu == sigma1(w[i-15]) */
	add Accu1, XAccu1
	adc Accu2, XAccu2
	adc Accu3, XAccu3
	adc Accu4, XAccu4 /* finished with sigma0 */
	ldd Func1, Z+7*4  /* now accu += w[i-7] */
	ldd Func2, Z+7*4+1
	ldd Func3, Z+7*4+2
	ldd Func4, Z+7*4+3
	add Accu1, Func1
	adc Accu2, Func2
	adc Accu3, Func3
	adc Accu4, Func4
	ldd Bck1, Z+12*4 /* now backup = w[i-2]*/
	ldd Bck2, Z+12*4+1
	ldd Bck3, Z+12*4+2
	ldd Bck4, Z+12*4+3
	/* now sigma 1 */
	movw Func1, Bck3
	movw Func3, Bck1 /* prerotated by 16 */
	ldi r20, 1
	rcall bitrotr
	movw XAccu3, Func3
	movw XAccu1, Func1	 /* store in ROTR(w[i-2], 17) xor accu */
;	movw Func1, Bck3
;	movw Func3, Bck1 /* prerotated by 16 */
	ldi r20, 2
	rcall bitrotr
	eor XAccu1, Func1  /* xor ROTR(w[i-2], 19)*/
	eor XAccu2, Func2
	eor XAccu3, Func3
	eor XAccu4, Func4
	ldi Func2, 2	 /* now shr10 (dirty trick, skipping a byte) */ /*we can destroy backup now*/
sigma1_shr:
	lsr Bck4
	ror Bck3
	ror Bck2
	dec Func2
	brne sigma1_shr
	eor XAccu1, Bck2
	eor XAccu2, Bck3
	eor XAccu3, Bck4  /* xor SHR(w[i-2], 10)*/ /* xor accu == sigma1(w[i-15]) */
	add Accu1, XAccu1
	adc Accu2, XAccu2
	adc Accu3, XAccu3
	adc Accu4, XAccu4 /* finished with sigma0 */
	/* now let's store the shit */
	st X+, Accu1
	st X+, Accu2
	st X+, Accu3
	st X+, Accu4
	dec LoopC
	breq 3f  ; skip if zero
	rjmp sha256_nextBlock_wcalcloop
3:
	/* we are finished with w array X points one byte post w */
/* init a array */
	pop r31
	pop r30
	push r30
	push r31
	ldi r25, 8*4 /* 8 32-bit values to copy from ctx to a array */
init_a_array:
	ld r1, Z+
	st X+, r1
	dec r25
	brne init_a_array

/* now the real fun begins */
/* for (i=0; i<64; ++i){
			t1 = a[7] + SIGMA1(a[4]) + CH(a[4],a[5],a[6]) + k[i] + w[i];
			t2 = SIGMA0(a[0]) + MAJ(a[0],a[1],a[2]);
			memmove(&(a[1]), &(a[0]), 7*4); 	// a[7]=a[6]; a[6]=a[5]; a[5]=a[4]; a[4]=a[3]; a[3]=a[2]; a[2]=a[1]; a[1]=a[0];
			a[4] += t1;
			a[0] = t1 + t2;
		} */
	/* Y points to a[0], Z ('cause lpm wants it) points to k[i], X points to w[i] */
	sbiw r26, 8*4  /* X still points at a[7]+1*/
	movw r28, r26
	ldi r30, lo8(sha256_kv)
	ldi r31, hi8(sha256_kv)
	dec r27  /* X - (64*4 == 256) */
	ldi r25, 64
	mov LoopC, r25
sha256_main_loop:
	/* now calculate t1 */
	 /*CH(x,y,z) = (x&y)^((~x)&z)*/
	ldd T1, Y+5*4
	ldd T2, Y+5*4+1
	ldd T3, Y+5*4+2
	ldd T4, Y+5*4+3 /* y in T */
	ldd Func1, Y+4*4
	ldd Func2, Y+4*4+1
	ldd Func3, Y+4*4+2
	ldd Func4, Y+4*4+3  /* x in Func */
	ldd Bck1, Y+6*4
	ldd Bck2, Y+6*4+1
	ldd Bck3, Y+6*4+2
	ldd Bck4, Y+6*4+3 /* z in Bck */
	and T1, Func1
	and T2, Func2
	and T3, Func3
	and T4, Func4
	com Func1
	com Func2
	com Func3
	com Func4
	and Bck1, Func1
	and Bck2, Func2
	and Bck3, Func3
	and Bck4, Func4
	eor T1, Bck1
	eor T2, Bck2
	eor T3, Bck3
	eor T4, Bck4 /* done, CH(x,y,z) is in T */
	/* now SIGMA1(a[4]) */
	ldd Bck4, Y+4*4		/* think about using it from Func reg above*/
	ldd Bck1, Y+4*4+1
	ldd Bck2, Y+4*4+2
	ldd Bck3, Y+4*4+3 /* load prerotate by 8-bit */
	movw Func1, Bck1
	movw Func3, Bck3
	ldi r20, 2
	rcall bitrotl		/* rotr(x,6) */
	movw XAccu1, Func1
	movw XAccu3, Func3
	movw Func1, Bck1
	movw Func3, Bck3
	ldi r20, 3
	rcall bitrotr 	/* rotr(x,11) */
	eor XAccu1, Func1
	eor XAccu2, Func2
	eor XAccu3, Func3
	eor XAccu4, Func4
	movw Func1, Bck3 /* this prerotates furteh 16 bits*/
	movw Func3, Bck1 /* so we have now prerotated by 24 bits*/
	ldi r20, 1
	rcall bitrotr 	/* rotr(x,11) */
	eor XAccu1, Func1
	eor XAccu2, Func2
	eor XAccu3, Func3
	eor XAccu4, Func4 /* finished with SIGMA1, add it to T */
	add T1, XAccu1
	adc T2, XAccu2
	adc T3, XAccu3
	adc T4, XAccu4
	/* now we've to add a[7], w[i] and k[i] */
	ldd XAccu1, Y+4*7
	ldd XAccu2, Y+4*7+1
	ldd XAccu3, Y+4*7+2
	ldd XAccu4, Y+4*7+3
	add T1, XAccu1
	adc T2, XAccu2
	adc T3, XAccu3
	adc T4, XAccu4 /* add a[7] */
	ld XAccu1, X+
	ld XAccu2, X+
	ld XAccu3, X+
	ld XAccu4, X+
	add T1, XAccu1
	adc T2, XAccu2
	adc T3, XAccu3
	adc T4, XAccu4 /* add w[i] */
	lpm XAccu1, Z+
	lpm XAccu2, Z+
	lpm XAccu3, Z+
	lpm XAccu4, Z+
	add T1, XAccu1
	adc T2, XAccu2
	adc T3, XAccu3
	adc T4, XAccu4 /* add k[i] */ /* finished with t1 */
	/*now t2 = SIGMA0(a[0]) + MAJ(a[0],a[1],a[2]) */ /*i did to much x86 asm, i always see 4 32bit regs*/
		/* starting with MAJ(x,y,z) */
	ldd Func1, Y+4*0+0
	ldd Func2, Y+4*0+1
	ldd Func3, Y+4*0+2
	ldd Func4, Y+4*0+3 /* load x=a[0] */
	ldd XAccu1, Y+4*1+0
	ldd XAccu2, Y+4*1+1
	ldd XAccu3, Y+4*1+2
	ldd XAccu4, Y+4*1+3 /* load y=a[1] */
	and XAccu1, Func1
	and XAccu2, Func2
	and XAccu3, Func3
	and XAccu4, Func4	/* XAccu == (x & y) */
	ldd Bck1, Y+4*2+0
	ldd Bck2, Y+4*2+1
	ldd Bck3, Y+4*2+2
	ldd Bck4, Y+4*2+3 /* load z=a[2] */
	and Func1, Bck1
	and Func2, Bck2
	and Func3, Bck3
	and Func4, Bck4
	eor XAccu1, Func1
	eor XAccu2, Func2
	eor XAccu3, Func3
	eor XAccu4, Func4	/* XAccu == (x & y) ^ (x & z) */
	ldd Func1, Y+4*1+0
	ldd Func2, Y+4*1+1
	ldd Func3, Y+4*1+2
	ldd Func4, Y+4*1+3 /* load y=a[1] */
	and Func1, Bck1
	and Func2, Bck2
	and Func3, Bck3
	and Func4, Bck4
	eor XAccu1, Func1
	eor XAccu2, Func2
	eor XAccu3, Func3
	eor XAccu4, Func4	/* XAccu == Maj(x,y,z) == (x & y) ^ (x & z) ^ (y & z) */
   	/* SIGMA0(a[0]) */
	ldd Bck1, Y+4*0+0 /* we should combine this with above */
	ldd Bck2, Y+4*0+1
	ldd Bck3, Y+4*0+2
	ldd Bck4, Y+4*0+3
	movw Func1, Bck1
	movw Func3, Bck3
	ldi r20, 2
	rcall bitrotr
	movw Accu1, Func1
	movw Accu3, Func3 /* Accu = shr(a[0], 2) */
	movw Func1, Bck3
	movw Func3, Bck1 /* prerotate by 16 bits */
	ldi r20, 3
	rcall bitrotl
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4 /* Accu ^= shr(a[0], 13) */
	mov Func1, Bck4
	mov Func2, Bck1
	mov Func3, Bck2
	mov Func4, Bck3  /* prerotate by 24 bits */
	ldi r20, 2
	rcall bitrotl
	eor Accu1, Func1
	eor Accu2, Func2
	eor Accu3, Func3
	eor Accu4, Func4 /* Accu ^= shr(a[0], 22) */
	add Accu1, XAccu1 /* add previous result (MAJ)*/
	adc Accu2, XAccu2
	adc Accu3, XAccu3
	adc Accu4, XAccu4
	/* now we are finished with the computing stuff (t1 in T, t2 in Accu)*/
	/* a[7]=a[6]; a[6]=a[5]; a[5]=a[4]; a[4]=a[3]; a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; */

	ldi r21, 7*4
	adiw r28, 7*4
a_shift_loop:
	ld  r25, -Y /* warning: this is PREdecrement */
	std Y+4, r25
	dec r21
	brne a_shift_loop

	ldd Bck1, Y+4*4+0
	ldd Bck2, Y+4*4+1
	ldd Bck3, Y+4*4+2
	ldd Bck4, Y+4*4+3
	add Bck1, T1
	adc Bck2, T2
	adc Bck3, T3
	adc Bck4, T4
	std Y+4*4+0, Bck1
	std Y+4*4+1, Bck2
	std Y+4*4+2, Bck3
	std Y+4*4+3, Bck4
	add Accu1, T1
	adc Accu2, T2
	adc Accu3, T3
	adc Accu4, T4
	std Y+4*0+0, Accu1
	std Y+4*0+1, Accu2
	std Y+4*0+2, Accu3
	std Y+4*0+3, Accu4 /* a array updated */


	dec LoopC
	breq update_state
	rjmp sha256_main_loop ;brne sha256_main_loop
update_state:
	/* update state */
	/* pointers to state should still exist on the stack ;-) */
	pop r31
	pop r30
	ldi r21, 8
update_state_loop:
	ldd Accu1, Z+0
	ldd Accu2, Z+1
	ldd Accu3, Z+2
	ldd Accu4, Z+3
	ld Func1, Y+
	ld Func2, Y+
	ld Func3, Y+
	ld Func4, Y+
	add Accu1, Func1
	adc Accu2, Func2
	adc Accu3, Func3
	adc Accu4, Func4
	st Z+, Accu1
	st Z+, Accu2
	st Z+, Accu3
	st Z+, Accu4
	dec r21
	brne update_state_loop
	/* now we just have to update the length */
	adiw r30, 1 /* since we add 512, we can simply skip the LSB */
	ldi r21, 2
	ldi r22, 6
	ld r20, Z
	add r20, r21
	st Z+, r20
	clr r21
sha256_nextBlock_fix_length:
	brcc sha256_nextBlock_epilog
	ld r20, Z
	adc r20, r21
	st Z+, r20
	dec r22
	brne sha256_nextBlock_fix_length

; EPILOG
sha256_nextBlock_epilog:
/* now we should clean up the stack */

	pop r21
	pop r20
	in r0, SREG
	cli ; we want to be uninterrupted while updating SP
	out SPL, r20
	out SREG, r0
	out SPH, r21
	clr r1
	pop r29
	pop r28
	pop r17
	pop r16
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop r7
	pop r6
	pop r5
	pop r4
	ret

sha256_kv: ; round-key-vector stored in ProgMem
.word	0x2f98, 0x428a, 0x4491, 0x7137, 0xfbcf, 0xb5c0, 0xdba5, 0xe9b5, 0xc25b, 0x3956, 0x11f1, 0x59f1, 0x82a4, 0x923f, 0x5ed5, 0xab1c
.word	0xaa98, 0xd807, 0x5b01, 0x1283, 0x85be, 0x2431, 0x7dc3, 0x550c, 0x5d74, 0x72be, 0xb1fe, 0x80de, 0x06a7, 0x9bdc, 0xf174, 0xc19b
.word	0x69c1, 0xe49b, 0x4786, 0xefbe, 0x9dc6, 0x0fc1, 0xa1cc, 0x240c, 0x2c6f, 0x2de9, 0x84aa, 0x4a74, 0xa9dc, 0x5cb0, 0x88da, 0x76f9
.word	0x5152, 0x983e, 0xc66d, 0xa831, 0x27c8, 0xb003, 0x7fc7, 0xbf59, 0x0bf3, 0xc6e0, 0x9147, 0xd5a7, 0x6351, 0x06ca, 0x2967, 0x1429
.word	0x0a85, 0x27b7, 0x2138, 0x2e1b, 0x6dfc, 0x4d2c, 0x0d13, 0x5338, 0x7354, 0x650a, 0x0abb, 0x766a, 0xc92e, 0x81c2, 0x2c85, 0x9272
.word	0xe8a1, 0xa2bf, 0x664b, 0xa81a, 0x8b70, 0xc24b, 0x51a3, 0xc76c, 0xe819, 0xd192, 0x0624, 0xd699, 0x3585, 0xf40e, 0xa070, 0x106a
.word	0xc116, 0x19a4, 0x6c08, 0x1e37, 0x774c, 0x2748, 0xbcb5, 0x34b0, 0x0cb3, 0x391c, 0xaa4a, 0x4ed8, 0xca4f, 0x5b9c, 0x6ff3, 0x682e
.word	0x82ee, 0x748f, 0x636f, 0x78a5, 0x7814, 0x84c8, 0x0208, 0x8cc7, 0xfffa, 0x90be, 0x6ceb, 0xa450, 0xa3f7, 0xbef9, 0x78f2, 0xc671


;###########################################################

.global sha256_init
;uint32_t sha256_init_vector[]={
;  	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
;	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };
;
;void sha256_init(sha256_ctx_t *state){
;	state->length=0;
;	memcpy(state->h, sha256_init_vector, 8*4);
;}
; param1: (r23,r24) 16-bit pointer to sha256_ctx_t struct in ram
; modifys: Z(r30,r31), Func1, r22
sha256_init:
	movw r26, r24 ; (24,25) --> (26,27) load X with param1
	ldi r30, lo8((sha256_init_vector))
	ldi r31, hi8((sha256_init_vector))
	ldi r22, 32+8
sha256_init_vloop:
	lpm r23, Z+
	st X+, r23
	dec r22
	brne sha256_init_vloop
	ret

sha256_init_vector:
.word 0xE667, 0x6A09
.word 0xAE85, 0xBB67
.word 0xF372, 0x3C6E
.word 0xF53A, 0xA54F
.word 0x527F, 0x510E
.word 0x688C, 0x9B05
.word 0xD9AB, 0x1F83
.word 0xCD19, 0x5BE0
.word 0x0000, 0x0000
.word 0x0000, 0x0000

;###########################################################

.global rotl32
; === ROTL32 ===
; function that rotates a 32 bit word to the left
;  param1: the 32-bit word to rotate
;	given in r25,r24,r23,r22 (r25 is most significant)
;  param2: an 8-bit value telling how often to rotate
;	given in r20
; modifys: r21, r22
rotl32:
	cpi r20, 8
	brlo bitrotl
	mov r21, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r21
	subi r20, 8
	rjmp rotl32
bitrotl:
	clr r21
	clc
bitrotl_loop:
	tst r20
	breq fixrotl
2:
	rol r22
	rol r23
	rol r24
	rol r25
	rol r21
	dec r20
	brne 2b
fixrotl:
	or r22, r21
	ret


;###########################################################

.global rotr32
; === ROTR32 ===
; function that rotates a 32 bit word to the right
;  param1: the 32-bit word to rotate
;	given in r25,r24,r23,22 (r25 is most significant)
;  param2: an 8-bit value telling how often to rotate
;	given in r20
; modifys: r21, r22
rotr32:
	cpi r20, 8
	brlo bitrotr
	mov r21, r22
	mov r22, r23
	mov r23, r24
	mov r24, r25
	mov r25, r21
	subi r20, 8
	rjmp rotr32
bitrotr:
	clr r21
	clc
bitrotr_loop:
	tst r20
	breq fixrotr
2:
	ror r25
	ror r24
	ror r23
	ror r22
	ror r21
	dec r20
	brne 2b
fixrotr:
	or r25, r21
	ret


;###########################################################

.global change_endian32
; === change_endian32 ===
; function that changes the endianess of a 32-bit word
;  param1: the 32-bit word
;	given in r25,r24,r23,22 (r25 is most significant)
;  modifys: r21, r22
change_endian32:
	movw r20,  r22 ; (r22,r23) --> (r20,r21)
	mov r22, r25
	mov r23, r24
	mov r24, r21
	mov r25, r20
	ret

