/* sha1-asm.S */
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
; SHA1 implementation in assembler for AVR
SHA1_BLOCK_BITS = 512
SHA1_HASH_BITS = 160

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

.macro delay
/*
	push r0
	push r1
	clr r0
1:	clr r1
2:	dec r1
	brne 2b
	dec r0
	brne 1b
	pop r1
	pop r0  // */
.endm

/* X points to Block */
.macro dbg_hexdump length
/*
	precall
	hexdump \length
	postcall
	// */
.endm



.section .text

SPL = 0x3D
SPH = 0x3E
SREG = 0x3F


;
;sha1_ctx_t is:
;
; [h0][h1][h2][h3][h4][length]
; hn is 32 bit large, length is 64 bit large

;###########################################################

.global sha1_ctx2hash
; === sha1_ctx2hash ===
; this function converts a state into a normal hash (bytestring)
;  param1: the 16-bit destination pointer
;	given in r25,r24 (r25 is most significant)
;  param2: the 16-bit pointer to sha1_ctx structure
;	given in r23,r22
sha1_ctx2hash:
	movw r26, r22
	movw r30, r24
	ldi r21, 5
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

.global sha1
; === sha1 ===
; this function calculates SHA-1 hashes from messages in RAM
;  param1: the 16-bit hash destination pointer
;	given in r25,r24 (r25 is most significant)
;  param2: the 16-bit pointer to message
;	given in r23,r22
;  param3: 32-bit length value (length of message in bits)
;   given in r21,r20,r19,r18
sha1:
sha1_prolog:
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
	sbiw r30, 5*4+8
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
	rcall sha1_init
	/* if length >= 512 */
1:
	tst r11
	brne 2f
	tst r10
	breq 4f
2:
	movw r24, r16
	movw r22, r12
	rcall sha1_nextBlock
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
	rcall sha1_lastBlock

	pop r24
	pop r25
	movw r22, r16
	rcall sha1_ctx2hash

sha1_epilog:
	in r30, SPL
	in r31, SPH
	adiw r30, 5*4+8
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

.global sha1_lastBlock
; === sha1_lastBlock ===
; this function does padding & Co. for calculating SHA-1 hashes
;  param1: the 16-bit pointer to sha1_ctx structure
;	given in r25,r24 (r25 is most significant)
;  param2: an 16-bit pointer to 64 byte block to hash
;	given in r23,r22
;  param3: an 16-bit integer specifing length of block in bits
;	given in r21,r20
sha1_lastBlock_localSpace = (SHA1_BLOCK_BITS/8+1)


sha1_lastBlock:
	cpi r21, 0x02
	brlo sha1_lastBlock_prolog
	push r25
	push r24
	push r23
	push r22
	push r21
	push r20
	rcall sha1_nextBlock
	pop r20
	pop r21
	pop r22
	pop r23
	pop r24
	pop r25
	subi r21, 2
	ldi r19, 64
	sub r22, r19
	sbc r23, r1
	rjmp sha1_lastBlock
sha1_lastBlock_prolog:
	/* allocate space on stack */
	in r30, SPL
	in r31, SPH
	in r0, SREG
	subi r30, lo8(64)
	sbci r31, hi8(64) /* ??? */
	cli
	out SPL, r30
	out SREG, r0
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
	breq sha1_lastBlock_post_copy
	mov r1, r18
sha1_lastBlock_copy_loop:
	ld r0, X+
	st Z+, r0
	dec r1
	brne sha1_lastBlock_copy_loop
sha1_lastBlock_post_copy:
sha1_lastBlock_insert_stuffing_bit:
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
	rjmp sha1_lastBlock_insert_zeros
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
	rcall sha1_nextBlock
	pop r20
	pop r21
	pop r24
	pop r25
	pop r30
	pop r31

	/* now we should subtract 512 from length */
	movw r26, r24
	adiw r26, 4*5+1 /* we can skip the lowest byte */
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

sha1_lastBlock_insert_zeros:
	ldi r19, 64-8
	sub r19, r18
	breq sha1_lastBlock_insert_length
	clr r1
1:
	st Z+, r1	/* r1 is still zero */
	dec r19
	brne 1b

;	rjmp sha1_lastBlock_epilog
sha1_lastBlock_insert_length:
	movw r26, r24	/* X points to state */
	adiw r26, 5*4	/* X points to (state.length) */
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
	rcall sha1_nextBlock

sha1_lastBlock_epilog:
	in r30, SPL
	in r31, SPH
	in r0, SREG
	adiw r30, 63 ; lo8(64)
	adiw r30,  1  ; hi8(64)
	cli
	out SPL, r30
	out SREG, r0
	out SPH, r31
	clr r1
	ret

/**/
;###########################################################

.global sha1_nextBlock
; === sha1_nextBlock ===
; this is the core function for calculating SHA-1 hashes
;  param1: the 16-bit pointer to sha1_ctx structure
;	given in r25,r24 (r25 is most significant)
;  param2: an 16-bit pointer to 64 byte block to hash
;	given in r23,r22
sha1_nextBlock_localSpace = (16+5+1)*4 ; 16 32-bit values for w array and 5 32-bit values for a array (total 84 byte)

xtmp = 0
xNULL = 1
W1 = 10
W2 = 11
T1	= 12
T2	= 13
T3	= 14
T4	= 15
LoopC = 16
S	  = 17
tmp1 = 18
tmp2 = 19
tmp3 = 20
tmp4 = 21
F1 = 22
F2 = 23
F3 = 24
F4 = 25

/* byteorder: high number <--> high significance */
sha1_nextBlock:
 ; initial, let's make some space ready for local vars
 			 /* replace push & pop by mem ops? */
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
;	movw r26, r20			; X points to free space on stack /* maybe removeable? */
	movw r30, r22			; Z points to message
	subi r20, lo8(sha1_nextBlock_localSpace) ;sbiw can do only up to 63
	sbci r21, hi8(sha1_nextBlock_localSpace)
	movw r26, r20			; X points to free space on stack
	in r0, SREG
	cli ; we want to be uninterrupted while updating SP
	out SPL, r20
	out SREG, r0
	out SPH, r21

	push r18
	push r19 /* push old SP on new stack */
	push r24
	push r25 /* param1 will be needed later */

	/* load a[] with state */
	movw 28, r24 /* load pointer to state in Y */
	adiw r26, 1 ; X++

	ldi LoopC, 5*4
1:	ld tmp1, Y+
	st X+, tmp1
	dec LoopC
	brne 1b

	movw W1, r26 /* save pointer to w[0] */
	/* load w[] with endian fixed message */
		/* we might also use the changeendian32() function at bottom */
	movw r30, r22 /* mv param2 (ponter to msg) to Z */
	ldi LoopC, 16
1:
	ldd tmp1, Z+3
	st X+, tmp1
	ldd tmp1, Z+2
	st X+, tmp1
	ldd tmp1, Z+1
	st X+, tmp1
	ld tmp1, Z
	st X+, tmp1
	adiw r30, 4
	dec LoopC
	brne 1b

	;clr LoopC /* LoopC is named t in FIPS 180-2 */
	clr xtmp
sha1_nextBlock_mainloop:
	mov S, LoopC
	lsl S
	lsl S
	andi S, 0x3C /* S is a bytepointer so *4 */
	/* load w[s] */
	movw r26, W1
	add r26, S /* X points at w[s] */
	adc r27, xNULL
	ld T1, X+
	ld T2, X+
	ld T3, X+
	ld T4, X+

/*
	push r26
	push r27
	push T4
	push T3
	push T2
	push T1
	in r26, SPL
	in r27, SPH
	adiw r26, 1
	dbg_hexdump 4
	pop T1
	pop T2
	pop T3
	pop T4
	pop r27
	pop r26
*/

	cpi LoopC, 16
	brlt sha1_nextBlock_mainloop_core
	/* update w[s] */
	ldi tmp1, 2*4
	rcall 1f
	ldi tmp1, 8*4
	rcall 1f
	ldi tmp1, 13*4
	rcall 1f
	rjmp 2f
1:		/* this might be "outsourced" to save the jump above */
	add tmp1, S
	andi tmp1, 0x3f
	movw r26, W1
	add r26, tmp1
	adc r27, xNULL
	ld tmp2, X+
	eor T1, tmp2
	ld tmp2, X+
	eor T2, tmp2
	ld tmp2, X+
	eor T3, tmp2
	ld tmp2, X+
	eor T4, tmp2
	ret
2:	/* now we just hav to do a ROTL(T) and save T back */
	mov tmp2, T4
	rol tmp2
	rol T1
	rol T2
	rol T3
	rol T4
	movw r26, W1
	add r26, S
	adc r27, xNULL
	st X+, T1
	st X+, T2
	st X+, T3
	st X+, T4

sha1_nextBlock_mainloop_core:	/* ther core function; T=ROTL5(a) ....*/
								/* T already contains w[s] */
	movw r26, W1
	sbiw r26, 4*1		/* X points at a[4] aka e */
	ld tmp1, X+
	add T1, tmp1
	ld tmp1, X+
	adc T2, tmp1
	ld tmp1, X+
	adc T3, tmp1
	ld tmp1, X+
	adc T4, tmp1		/* T = w[s]+e */
	sbiw r26, 4*5		/* X points at a[0] aka a */
	ld F1, X+
	ld F2, X+
	ld F3, X+
	ld F4, X+
	mov tmp1, F4		/* X points at a[1] aka b */
	ldi tmp2, 5
1:
	rol tmp1
	rol F1
	rol F2
	rol F3
	rol F4
	dec tmp2
	brne 1b

	add T1, F1
	adc T2, F2
	adc T3, F3
	adc T4, F4 /* T = ROTL(a,5) + e + w[s] */

	/* now we have to do this fucking conditional stuff */
	ldi r30, lo8(sha1_nextBlock_xTable)
	ldi r31, hi8(sha1_nextBlock_xTable)
	add r30, xtmp
	adc r31, xNULL
	lpm tmp1, Z
	cp tmp1, LoopC
	brne 1f
	inc xtmp
1:	ldi r30, lo8(sha1_nextBlock_KTable)
	ldi r31, hi8(sha1_nextBlock_KTable)
	lsl xtmp
	lsl xtmp
	add r30, xtmp
	adc r31, xNULL
	lsr xtmp
	lsr xtmp

	lpm tmp1, Z+
	add T1, tmp1
	lpm tmp1, Z+
	adc T2, tmp1
	lpm tmp1, Z+
	adc T3, tmp1
	lpm tmp1, Z+
	adc T4, tmp1
			/* T = ROTL(a,5) + e + kt + w[s] */

	/* Z-4 is just pointing to kt ... */
	movw r28, r26 /* copy X in Y */
	adiw r30, 3*4 /* now Z points to the rigth locatin in our jump-vector-table */
	lsr r31
	ror r30

	icall
	mov F1, tmp1
	icall
	mov F2, tmp1
	icall
	mov F3, tmp1
	icall

	add T1, F1
	adc T2, F2
	adc T3, F3
	adc T4, tmp1 /* T = ROTL5(a) + f_t(b,c,d) + e + k_t + w[s] */
				 /* X points still at a[1] aka b, Y points at a[2] aka c */
	/* update a[] */
sha1_nextBlock_update_a:
	/*first we move all vars in a[] "one up" e=d, d=c, c=b, b=a*/
	//adiw r28, 3*4  /* Y should point at a[4] aka e */
	movw r28, W1
	sbiw r28, 4

	ldi tmp2, 4*4
1:
	ld tmp1, -Y
	std Y+4, tmp1
	dec tmp2
	brne 1b
	/* Y points at a[0] aka a*/

	movw r28, W1
	sbiw r28, 5*4
	/* store T in a[0] aka a */
	st Y+, T1
	st Y+, T2
	st Y+, T3
	st Y+, T4
	/* Y points at a[1] aka b*/

	/* rotate c */
	ldd T1, Y+1*4
	ldd T2, Y+1*4+1
	ldd T3, Y+1*4+2
	ldd T4, Y+1*4+3
	mov tmp1, T1
	ldi tmp2, 2
1:	ror tmp1
	ror T4
	ror T3
	ror T2
	ror T1
	dec tmp2
	brne 1b
	std Y+1*4+0, T1
	std Y+1*4+1, T2
	std Y+1*4+2, T3
	std Y+1*4+3, T4
/*
	push r27
	push r26
	movw r26, W1
	sbiw r26, 4*5
	dbg_hexdump 4*5
	pop r26
	pop r27
*/
	inc LoopC
	cpi LoopC, 80
	brge 1f
	rjmp sha1_nextBlock_mainloop
/**************************************/
1:
   /* littel patch */
	sbiw r28, 4

/* add a[] to state and inc length */
	pop r27
	pop r26		/* now X points to state (and Y still at a[0]) */
	ldi tmp4, 5
1:	clc
	ldi tmp3, 4
2:	ld tmp1, X
	ld tmp2, Y+
	adc tmp1, tmp2
	st X+, tmp1
	dec tmp3
	brne 2b
	dec tmp4
	brne 1b

	/* now length += 512 */
	adiw r26, 1 /* we skip the least significant byte */
	ld tmp1, X
	ldi tmp2, hi8(512) /* 2 */
	add tmp1, tmp2
	st X+, tmp1
	ldi tmp2, 6
1:
	ld tmp1, X
	adc tmp1, xNULL
	st X+, tmp1
	dec tmp2
	brne 1b

; EPILOG
sha1_nextBlock_epilog:
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
	ret

sha1_nextBlock_xTable:
.byte 20,40,60,0
sha1_nextBlock_KTable:
.int	0x5a827999
.int	0x6ed9eba1
.int	0x8f1bbcdc
.int	0xca62c1d6
sha1_nextBlock_JumpTable:
rjmp sha1_nextBlock_Ch
	nop
rjmp sha1_nextBlock_Parity
	nop
rjmp sha1_nextBlock_Maj
	nop
rjmp sha1_nextBlock_Parity

	 /* X and Y still point at a[1] aka b ; return value in tmp1 */
sha1_nextBlock_Ch:
	ld tmp1, Y+
	mov tmp2, tmp1
	com tmp2
	ldd tmp3, Y+3	/* load from c */
	and tmp1, tmp3
	ldd tmp3, Y+7	/* load from d */
	and tmp2, tmp3
	eor tmp1, tmp2
	ret

sha1_nextBlock_Maj:
	ld tmp1, Y+
	mov tmp2, tmp1
	ldd tmp3, Y+3	/* load from c */
	and tmp1, tmp3
	ldd tmp4, Y+7	/* load from d */
	and tmp2, tmp4
	eor tmp1, tmp2
	and tmp3, tmp4
	eor tmp1, tmp3
	ret

sha1_nextBlock_Parity:
	ld tmp1, Y+
	ldd tmp2, Y+3	/* load from c */
	eor tmp1, tmp2
	ldd tmp2, Y+7	/* load from d */
	eor tmp1, tmp2
	ret
/*
ch_str:			.asciz "\r\nCh"
maj_str:		.asciz "\r\nMaj"
parity_str:	.asciz "\r\nParity"
*/
;###########################################################

.global sha1_init
;void sha1_init(sha1_ctx_t *state){
;	DEBUG_S("\r\nSHA1_INIT");
;	state->h[0] = 0x67452301;
;	state->h[1] = 0xefcdab89;
;	state->h[2] = 0x98badcfe;
;	state->h[3] = 0x10325476;
;	state->h[4] = 0xc3d2e1f0;
;	state->length = 0;
;}
; param1: (Func3,r24) 16-bit pointer to sha1_ctx_t struct in ram
; modifys: Z(r30,r31), Func1, r22
sha1_init:
	movw r26, r24 ; (24,25) --> (26,27) load X with param1
	ldi r30, lo8((sha1_init_vector))
	ldi r31, hi8((sha1_init_vector))
	ldi r22, 5*4 /* bytes to copy */
sha1_init_vloop:
	lpm r23, Z+
	st X+, r23
	dec r22
	brne sha1_init_vloop
	ldi r22, 8
sha1_init_lloop:
	st X+, r1
	dec r22
	brne sha1_init_lloop
	ret

sha1_init_vector:
.int 0x67452301;
.int 0xefcdab89;
.int 0x98badcfe;
.int 0x10325476;
.int 0xc3d2e1f0;

