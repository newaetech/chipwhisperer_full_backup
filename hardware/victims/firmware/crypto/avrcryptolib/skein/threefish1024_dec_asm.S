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
/*
void permute_inv16(void* data){
	uint64_t t;
	t = X(15);
	X(15) = X(7);
	X(7) = X(9);
	X(9) = X(1);
	X(1) = t;
	t = X(11);
	X(11) = X(5);
	X(5) = X(13);
	X(13) = X(3);
	X(3) = t;
	t = X(4);
	X(4) = X(6);
	X(6) = t;
	t = X(14);
	X(14) = X(12);
	X(12) = X(10);
	X(10) = X(8);
	X(8) = t;
}
void add_key_16(void* data, const threefish1024_ctx_t* ctx, uint8_t s){
	uint8_t i;
	for(i=0; i<13; ++i){
		X(i) -= ctx->k[(s+i)%17];
	}
	X(13) -= ctx->k[(s+13)%17] + ctx->t[s%3];
	X(14) -= ctx->k[(s+14)%17] + ctx->t[(s+1)%3];
	X(15) -= ctx->k[(s+15)%17] + s;
}
void threefish1024_dec(void* data, const threefish1024_ctx_t* ctx){
	uint8_t i=0,s=20;
	uint8_t r0[8] = {0x69, 0x72, 0x21, 0x34, 0x42, 0x41, 0x31, 0x79};
	uint8_t r1[8] = {0x61, 0x19, 0x1a, 0x19, 0x53, 0x10, 0x31, 0x53};
	uint8_t r2[8] = {0x33, 0x40, 0x22, 0x69, 0x31, 0x22, 0x6a, 0x5b};
	uint8_t r3[8] = {0x72, 0x6b, 0x31, 0x60, 0x74, 0x71, 0x2b, 0x50};
	uint8_t r4[8] = {0x5b, 0x23, 0x53, 0x63, 0x54, 0x3b, 0x2a, 0x20};
	uint8_t r5[8] = {0x60, 0x22, 0x52, 0x11, 0x11, 0x14, 0x2b, 0x3a};
	uint8_t r6[8] = {0x7b, 0x02, 0x50, 0x43, 0x73, 0x40, 0x64, 0x5a};
	uint8_t r7[8] = {0x70, 0x70, 0x29, 0x51, 0x42, 0x7a, 0x71, 0x14};

	do{
		if(i%4==0){
			add_key_16(data, ctx, s);
			--s;
		}
		permute_inv16(data);
		threefish_invmix((uint8_t*)data +  0, r0[i%8]);
		threefish_invmix((uint8_t*)data + 16, r1[i%8]);
		threefish_invmix((uint8_t*)data + 32, r2[i%8]);
		threefish_invmix((uint8_t*)data + 48, r3[i%8]);
		threefish_invmix((uint8_t*)data + 64, r4[i%8]);
		threefish_invmix((uint8_t*)data + 80, r5[i%8]);
		threefish_invmix((uint8_t*)data + 96, r6[i%8]);
		threefish_invmix((uint8_t*)data +112, r7[i%8]);
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
.global threefish1024_dec
threefish1024_dec:
	push r28
	push r29
	push_range 2, 17
	movw DATA0, r24
	movw CTX0, r22
	clr I
	ldi r26, 20
	mov S, r26
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
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX1
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX2
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX3
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX4
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX5
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX6
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX7
	adc r31, r1
	rcall sub_z_from_x8
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
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX1
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX2
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX3
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX4
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX5
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX6
	adc r31, r1
	rcall sub_z_from_x8
	movw r30, CTX0
	add r30, IDX7
	adc r31, r1
	rcall sub_z_from_x8
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
	rcall sub_z_from_x8
	movw r30, IDX2
	add r30, IDX1
	adc r31, r1
	rcall sub_z_from_x8
	ld r0, X
	sub r0, S
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
	ld r0, X
	sbc r0, r1
	st X+, r0
	tst S
	brne 3f
exit:
	pop_range 2, 17
	pop r29
	pop r28
	ret
3:
	dec S
4:
	/* now the permutation */
	movw r26, DATA0  /* X1 <-> X15 */
	adiw r26, 1*8
	movw r30, DATA0
	adiw r30, 7*8+4
	adiw r30, 7*8+4
	rcall xchg_zx8
	movw r26, DATA0  /* X15 <-> X7 */
	adiw r26, 7*8+4
	adiw r26, 7*8+4
	movw r30, DATA0
	adiw r30, 7*8
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
	movw r26, DATA0  /* X11 <-> X5 */
	adiw r26, 7*8
	adiw r26, 4*8
	movw r30, DATA0
	adiw r30, 5*8
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
	movw r26, DATA0  /* X14 <-> X12 */
	adiw r26, 7*8
	adiw r26, 7*8
	movw r30, DATA0
	adiw r30, 7*8
	adiw r30, 5*8
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
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 16
	mov r22, IDX0
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 32
	mov r22, IDX1
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 48
	pop r22
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24,  1
	pop r22
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24, 17
	pop r22
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24, 33
	pop r22
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 63
	adiw r24, 49
	pop r22
	call threefish_invmix_asm /* no rcall? */
	inc I
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
threefish1024_rc0: .byte 0x69, 0x72, 0x21, 0x34, 0x42, 0x41, 0x31, 0x79
threefish1024_rc1: .byte 0x61, 0x19, 0x1a, 0x19, 0x53, 0x10, 0x31, 0x53
threefish1024_rc2: .byte 0x33, 0x40, 0x22, 0x69, 0x31, 0x22, 0x6a, 0x5b
threefish1024_rc3: .byte 0x72, 0x6b, 0x31, 0x60, 0x74, 0x71, 0x2b, 0x50
threefish1024_rc4: .byte 0x5b, 0x23, 0x53, 0x63, 0x54, 0x3b, 0x2a, 0x20
threefish1024_rc5: .byte 0x60, 0x22, 0x52, 0x11, 0x11, 0x14, 0x2b, 0x3a
threefish1024_rc6: .byte 0x7b, 0x02, 0x50, 0x43, 0x73, 0x40, 0x64, 0x5a
threefish1024_rc7: .byte 0x70, 0x70, 0x29, 0x51, 0x42, 0x7a, 0x71, 0x14
*/
threefish1024_rc0:  .byte 0x11, 0x49, 0x20, 0x51, 0x1b, 0x41, 0x5a, 0x30
threefish1024_rc1:  .byte 0x60, 0x54, 0x42, 0x11, 0x24, 0x04, 0x23, 0x2b
threefish1024_rc2:  .byte 0x43, 0x69, 0x70, 0x5b, 0x60, 0x63, 0x12, 0x10
threefish1024_rc3:  .byte 0x64, 0x6a, 0x63, 0x49, 0x51, 0x2b, 0x79, 0x69
threefish1024_rc4:  .byte 0x39, 0x23, 0x04, 0x14, 0x69, 0x42, 0x61, 0x10
threefish1024_rc5:  .byte 0x49, 0x52, 0x7b, 0x69, 0x34, 0x51, 0x22, 0x21
threefish1024_rc6:  .byte 0x5b, 0x54, 0x52, 0x54, 0x20, 0x73, 0x39, 0x3a
threefish1024_rc7:  .byte 0x24, 0x31, 0x51, 0x4a, 0x31, 0x21, 0x64, 0x5b

sub_z_from_x8:
	ld r0, Z+
	ld r1, X
	sub r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	sbc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	sbc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	sbc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	sbc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	sbc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	sbc r1, r0
	st X+, r1
	ld r0, Z+
	ld r1, X
	sbc r1, r0
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



