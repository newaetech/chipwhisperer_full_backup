/* threefish1024_enc_asm.S */
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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-24
 * \license GPLv3 or later
 */

#include "avr-asm-macros.S"

/******************************************************************************/
A0 = 14
A1 = 15
A2 = 16
A3 = 17
A4 = 18
A5 = 19
A6 = 20
A7 = 21
/*
#define THREEFISH_KEY_CONST 0x5555.5555.5555.5555.LL / * 2**64/3 * /

#define K(s) (((uint64_t*)key)[(s)])
#define T(s) (((uint64_t*)tweak)[(s)])
void threefish1024_init(const void* key, const void* tweak, threefish512_ctx_t* ctx){
	memcpy(ctx->k, key, 16*8);
	memcpy(ctx->t, tweak, 2*8);
	uint8_t i;
	ctx->k[16] = THREEFISH_KEY_CONST;
	for(i=0; i<8; ++i){
		ctx->k[16] ^= K(i);
	}
	ctx->t[2] = T(0) ^ T(1);
}
*/
/*
 * param key:   r24:r25
 * param tweak: r22:r23
 * param ctx:   r20:r21
 */
.global threefish1024_init
threefish1024_init:
	push_range 14, 17
	movw r30, r20
	movw r26, r24
	ldi r24, 16
;	ldi A7, 0x55
;	mov A6, A7
;	movw A4, A6
;	movw A2, A6
;	movw A0, A6
	ldi A6, 0x22 ; 0x1BD1.1BDA.A9FC.1A22
	ldi A7, 0x1A
	movw A0, A6
	ldi A2, 0xFC
	ldi A3, 0xA9
	ldi A4, 0xDA
	ldi A5, 0x1B
	ldi A6, 0xD1
	ldi A7, 0x1B
1:
	ld r0, X+
	st Z+, r0
	eor A0, r0
	ld r0, X+
	st Z+, r0
	eor A1, r0
	ld r0, X+
	st Z+, r0
	eor A2, r0
	ld r0, X+
	st Z+, r0
	eor A3, r0
	ld r0, X+
	st Z+, r0
	eor A4, r0
	ld r0, X+
	st Z+, r0
	eor A5, r0
	ld r0, X+
	st Z+, r0
	eor A6, r0
	ld r0, X+
	st Z+, r0
	eor A7, r0
	dec r24
	brne 1b
	st Z+, A0
	st Z+, A1
	st Z+, A2
	st Z+, A3
	st Z+, A4
	st Z+, A5
	st Z+, A6
	st Z+, A7
	/* now the tweak */
	movw r26, r22
	tst r27
	brne 3f
	tst r26
	brne 3f
	ldi r26, 3*8
1:
	st Z+, r1
	dec r26
	brne 1b
	rjmp 9f
3:
	ld A0, X+
	ld A1, X+
	ld A2, X+
	ld A3, X+
	ld A4, X+
	ld A5, X+
	ld A6, X+
	ld A7, X+
	st Z+, A0
	st Z+, A1
	st Z+, A2
	st Z+, A3
	st Z+, A4
	st Z+, A5
	st Z+, A6
	st Z+, A7
	ld r0, X+
	eor A0, r0
	st Z+, r0
	ld r0, X+
	eor A1, r0
	st Z+, r0
	ld r0, X+
	eor A2, r0
	st Z+, r0
	ld r0, X+
	eor A3, r0
	st Z+, r0
	ld r0, X+
	eor A4, r0
	st Z+, r0
	ld r0, X+
	eor A5, r0
	st Z+, r0
	ld r0, X+
	eor A6, r0
	st Z+, r0
	ld r0, X+
	eor A7, r0
	st Z+, r0
	st Z+, A0
	st Z+, A1
	st Z+, A2
	st Z+, A3
	st Z+, A4
	st Z+, A5
	st Z+, A6
	st Z+, A7
9:
	pop_range 14, 17
	ret

/******************************************************************************/
/*
#define X(a) (((uint64_t*)data)[(a)])
void permute_16(void* data){
	uint64_t t;
	t = X(1);
	X(1) = X(9);
	X(9) = X(7);
	X(7) = X(15);
	X(15) = t;
	t = X(3);
	X(3) = X(13);
	X(13) = X(5);
	X(5) = X(11);
	X(11) = t;
	t = X(4);
	X(4) = X(6);
	X(6) = t;
	t = X(8);
	X(8) = X(10);
	X(10) = X(12);
	X(12) = X(14);
	X(14) = t;
}
void add_key_16(void* data, const threefish1024_ctx_t* ctx, uint8_t s){
	uint8_t i;
	for(i=0; i<13; ++i){
		X(i) += ctx->k[(s+i)%17];
	}
	X(13) += ctx->k[(s+13)%17] + ctx->t[s%3];
	X(14) += ctx->k[(s+14)%17] + ctx->t[(s+1)%3];
	X(15) += ctx->k[(s+15)%17] + s;
}
void threefish1024_enc(void* data, const threefish1024_ctx_t* ctx){
	uint8_t i=0,s=0;
	uint8_t r0[8] = {55, 25, 33, 34, 28, 17, 58, 47};
	uint8_t r1[8] = {43, 25,  8, 43,  7,  6,  7, 49};
	uint8_t r2[8] = {37, 46, 18, 25, 47, 18, 32, 27};
	uint8_t r3[8] = {40, 13, 57, 60, 48, 25, 45, 58};
	uint8_t r4[8] = {16, 14, 21, 44, 51, 43, 19, 37};
	uint8_t r5[8] = {22, 13, 12,  9,  9, 42, 18, 48};
	uint8_t r6[8] = {38, 52, 32, 59, 35, 40,  2, 53};
	uint8_t r7[8] = {12, 57, 54, 34, 41, 15, 56, 56};
	do{
		if(i%4==0){
			add_key_16(data, ctx, s);
			++s;
		}
		threefish_mix((uint8_t*)data +  0, r0[i%8]);
		threefish_mix((uint8_t*)data + 16, r1[i%8]);
		threefish_mix((uint8_t*)data + 32, r2[i%8]);
		threefish_mix((uint8_t*)data + 48, r3[i%8]);
		threefish_mix((uint8_t*)data + 64, r4[i%8]);
		threefish_mix((uint8_t*)data + 80, r5[i%8]);
		threefish_mix((uint8_t*)data + 96, r6[i%8]);
		threefish_mix((uint8_t*)data +112, r7[i%8]);
		permute_16(data);
		++i;
	}while(i!=80);
	add_key_16(data, ctx, s);
}
*/
I     =  2
S     =  3
DATA0 =  4
DATA1 =  5
CTX0  =  6
CTX1  =  7
IDX0  =  8
IDX1  =  9
IDX2  = 10
IDX3  = 11
IDX4  = 12
IDX5  = 13
IDX6  = 14
IDX7  = 15

/*
 * param data:  r24:r25
 * param ctx:   r22:r23
 */
.global threefish1024_enc
threefish1024_enc:
	push r28
	push r29
	push_range 2, 17
	movw DATA0, r24
	movw CTX0, r22
	clr I
	clr S
1:
	mov r30,  I
	andi r30, 0x03
	breq 2f
	rjmp 4f
2:
	ldi r30, lo8(threefish1024_slut17)
	ldi r31, hi8(threefish1024_slut17)
	add r30, S
	adc r31, r1
	lpm IDX0, Z+
	lpm IDX1, Z+
	lpm IDX2, Z+
	lpm IDX3, Z+
	lpm IDX4, Z+
	lpm IDX5, Z+
	lpm IDX6, Z+
	lpm IDX7, Z
	movw r30, CTX0
	movw r26, DATA0
	add r30, IDX0
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX1
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX2
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX3
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX4
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX5
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX6
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX7
	adc r31, r1
	rcall add_z_to_x8
	/* second half */
	ldi r30, lo8(threefish1024_slut17)
	ldi r31, hi8(threefish1024_slut17)
	add r30, S
	adc r31, r1
	adiw r30, 8
	lpm IDX0, Z+
	lpm IDX1, Z+
	lpm IDX2, Z+
	lpm IDX3, Z+
	lpm IDX4, Z+
	lpm IDX5, Z+
	lpm IDX6, Z+
	lpm IDX7, Z
	movw r30, CTX0
	add r30, IDX0
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX1
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX2
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX3
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX4
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX5
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX6
	adc r31, r1
	rcall add_z_to_x8
	movw r30, CTX0
	add r30, IDX7
	adc r31, r1
	rcall add_z_to_x8
	/* now the remaining key */
	sbiw r26, 3*8
	ldi r30, lo8(threefish1024_slut3)
	ldi r31, hi8(threefish1024_slut3)
	add r30, S
	adc r31, r1
	lpm IDX0, Z+
	lpm IDX1, Z
	movw r30, CTX0
	adiw r30, 7*8 /* make Z pointing to (extended tweak) */
	adiw r30, 7*8
	adiw r30, 3*8
	movw IDX2, r30
	add r30, IDX0
	adc r31, r1
	rcall add_z_to_x8
	movw r30, IDX2
	add r30, IDX1
	adc r31, r1
	rcall add_z_to_x8
	ld r0, X
	add r0, S
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
	inc S
	mov r26, S
	cpi r26, 21
	brmi 4f
exit:
	pop_range 2, 17
	pop r29
	pop r28
	ret
4:
	/* call mix */
	ldi r30, lo8(threefish1024_rc0)
	ldi r31, hi8(threefish1024_rc0)
	mov r26, I
	andi r26, 0x07
	add r30, r26
	adc r31, r1
	lpm r22, Z
	adiw r30, 8
	lpm IDX0, Z
	adiw r30, 8
	lpm IDX1, Z
	adiw r30, 8
	lpm IDX2, Z
	adiw r30, 8
	lpm IDX3, Z
	adiw r30, 8
	lpm IDX4, Z
	adiw r30, 8
	lpm IDX5, Z
	adiw r30, 8
	lpm IDX6, Z
	push IDX6
	push IDX5
	push IDX4
	push IDX3
	push IDX2

	movw r24, DATA0
	call threefish_mix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 16
	mov r22, IDX0
	call threefish_mix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 32
	mov r22, IDX1
	call threefish_mix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 48
	pop r22
	call threefish_mix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24,  1
	pop r22
	call threefish_mix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24, 17
	pop r22
	call threefish_mix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24, 33
	pop r22
	call threefish_mix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24, 49
	pop r22
	call threefish_mix_asm /* no rcall? */
	/* now the permutation */
	movw r26, DATA0  /* X1 <-> X15 */
	adiw r26, 1*8
	movw r30, DATA0
	adiw r30, 7*8+4
	adiw r30, 7*8+4
	rcall xchg_zx8
	movw r26, DATA0  /* X1 <-> X9 */
	adiw r26, 1*8
	movw r30, DATA0
	adiw r30, 7*8
	adiw r30, 2*8
	rcall xchg_zx8
	movw r26, DATA0  /* X9 <-> X7 */
	adiw r26, 7*8
	adiw r26, 2*8
	movw r30, DATA0
	adiw r30, 7*8
	rcall xchg_zx8
	/* --- */
	movw r26, DATA0  /* X3 <-> X11 */
	adiw r26, 3*8
	movw r30, DATA0
	adiw r30, 7*8
	adiw r30, 4*8
	rcall xchg_zx8
	movw r26, DATA0  /* X3 <-> X13 */
	adiw r26, 3*8
	movw r30, DATA0
	adiw r30, 7*8
	adiw r30, 6*8
	rcall xchg_zx8
	movw r26, DATA0  /* X13 <-> X5 */
	adiw r26, 7*8
	adiw r26, 6*8
	movw r30, DATA0
	adiw r30, 5*8
	rcall xchg_zx8
	/* --- */
	movw r26, DATA0  /* X8 <-> X14 */
	adiw r26, 7*8
	adiw r26, 1*8
	movw r30, DATA0
	adiw r30, 7*8
	adiw r30, 7*8
	rcall xchg_zx8
	movw r26, DATA0  /* X8 <-> X10 */
	adiw r26, 7*8
	adiw r26, 1*8
	movw r30, DATA0
	adiw r30, 7*8
	adiw r30, 3*8
	rcall xchg_zx8
	movw r26, DATA0  /* X10 <-> X12 */
	adiw r26, 7*8
	adiw r26, 3*8
	movw r30, DATA0
	adiw r30, 7*8
	adiw r30, 5*8
	rcall xchg_zx8
	/* --- */
	movw r26, DATA0  /* X4 <-> X6 */
	adiw r26, 4*8
	movw r30, DATA0
	adiw r30, 6*8
	rcall xchg_zx8

	inc I
;	mov r26, I
;	cpi r26, 5
;	brne 9f
;	rjmp exit
9:
	rjmp 1b

threefish1024_slut17:
	.byte 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38
	.byte 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78
	.byte 0x80, 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30
	.byte 0x38, 0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70
	.byte 0x78, 0x80, 0x00, 0x08, 0x10
threefish1024_slut3:
    .byte 0x00, 0x08, 0x10, 0x00, 0x08, 0x10, 0x00, 0x08
	.byte 0x10, 0x00, 0x08, 0x10, 0x00, 0x08, 0x10, 0x00
	.byte 0x08, 0x10, 0x00, 0x08, 0x10, 0x00, 0x08, 0x10
	.byte 0x00
/* old round constants
threefish1024_rc0: .byte 0x79, 0x31, 0x41, 0x42, 0x34, 0x21, 0x72, 0x69
threefish1024_rc1: .byte 0x53, 0x31, 0x10, 0x53, 0x19, 0x1a, 0x19, 0x61
threefish1024_rc2: .byte 0x5b, 0x6a, 0x22, 0x31, 0x69, 0x22, 0x40, 0x33
threefish1024_rc3: .byte 0x50, 0x2b, 0x71, 0x74, 0x60, 0x31, 0x6b, 0x72
threefish1024_rc4: .byte 0x20, 0x2a, 0x3b, 0x54, 0x63, 0x53, 0x23, 0x5b
threefish1024_rc5: .byte 0x3a, 0x2b, 0x14, 0x11, 0x11, 0x52, 0x22, 0x60
threefish1024_rc6: .byte 0x5a, 0x64, 0x40, 0x73, 0x43, 0x50, 0x02, 0x7b
threefish1024_rc7: .byte 0x14, 0x71, 0x7a, 0x42, 0x51, 0x29, 0x70, 0x70
*/
threefish1024_rc0:  .byte 0x30, 0x5a, 0x41, 0x1b, 0x51, 0x20, 0x49, 0x11
threefish1024_rc1:  .byte 0x2b, 0x23, 0x04, 0x24, 0x11, 0x42, 0x54, 0x60
threefish1024_rc2:  .byte 0x10, 0x12, 0x63, 0x60, 0x5b, 0x70, 0x69, 0x43
threefish1024_rc3:  .byte 0x69, 0x79, 0x2b, 0x51, 0x49, 0x63, 0x6a, 0x64
threefish1024_rc4:  .byte 0x10, 0x61, 0x42, 0x69, 0x14, 0x04, 0x23, 0x39
threefish1024_rc5:  .byte 0x21, 0x22, 0x51, 0x34, 0x69, 0x7b, 0x52, 0x49
threefish1024_rc6:  .byte 0x3a, 0x39, 0x73, 0x20, 0x54, 0x52, 0x54, 0x5b
threefish1024_rc7:  .byte 0x5b, 0x64, 0x21, 0x31, 0x4a, 0x51, 0x31, 0x24

add_z_to_x8:
	ld r0, Z+
	ld r1, X
	add r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	adc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	adc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	adc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	adc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	adc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	adc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	adc r1, r0
	st X+, r1
	clr r1
	ret

T0 = IDX0
T1 = 0
CNT = 24
xchg_zx8:
	ldi CNT, 8
1:	ld T0, X
	ld T1, Z
	st X+, T1
	st Z+, T0
	dec CNT
	brne 1b
	ret



