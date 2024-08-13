/* threefish256_enc_asm.S */
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
 * \date    2009-03-16
 * \license GPLv3 or later
 */

#include "avr-asm-macros.S"

/******************************************************************************/
/*
void permute_4(void* data){
	uint64_t t;
	t = X(1);
	X(1) = X(3);
	X(3) = t;
}
void add_key_4(void* data, const threefish256_ctx_t* ctx, uint8_t s){
	X(0) -= ctx->k[(s+0)%5];
	X(1) -= ctx->k[(s+1)%5] + ctx->t[s%3];
	X(2) -= ctx->k[(s+2)%5] + ctx->t[(s+1)%3];
	X(3) -= ctx->k[(s+3)%5] + s;
}
void threefish256_dec(void* data, const threefish256_ctx_t* ctx){
	uint8_t i=0,s=18;
	uint8_t r0[8] = {0x73, 0x13, 0x7b, 0x32, 0x72, 0x2b, 0x44, 0x1b};
	uint8_t r1[8] = {0x62, 0x52, 0x43, 0x24, 0x54, 0x6a, 0x34, 0x70};
	do{
		if(i%4==0){
			add_key_4(data, ctx, s);
			--s;
		}
		permute_4(data);
		threefish_invmix(data, r0[i%8]);
		threefish_invmix((uint8_t*)data + 16, r1[i%8]);
		++i;
	}while(i!=72);
	add_key_4(data, ctx, s);
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
/*
 * param data:  r24:r25
 * param ctx:   r22:r23
 */
.global threefish256_dec
threefish256_dec:
	push r28
	push r29
	push_range 2, 17
	movw DATA0, r24
	movw CTX0, r22
	clr I
	ldi r26, 18
	mov S, r26
1:
	mov r30,  I
	andi r30, 0x03
	breq 2f
	rjmp 4f
2:
	ldi r30, lo8(threefish256_slut5)
	ldi r31, hi8(threefish256_slut5)
	add r30, S
	adc r31, r1
	lpm IDX0, Z+
	lpm IDX1, Z+
	lpm IDX2, Z+
	lpm IDX3, Z
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

	/* now the remaining key */
	sbiw r26, 3*8
	ldi r30, lo8(threefish256_slut3)
	ldi r31, hi8(threefish256_slut3)
	add r30, S
	adc r31, r1
	lpm IDX0, Z+
	lpm IDX1, Z
	movw r30, CTX0
	adiw r30, 5*8
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
	movw r26, DATA0
	adiw r26, 8
	movw r30, r26
	adiw r30, 16
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	ld IDX0, X
	ld IDX1, Z
	st X+, IDX1
	st Z+, IDX0
	/* call mix */
	ldi r30, lo8(threefish256_rc0)
	ldi r31, hi8(threefish256_rc0)
	mov r26, I
	andi r26, 0x07
	add r30, r26
	adc r31, r1
	lpm r22, Z
	adiw r30, 8
	lpm IDX0, Z
	movw r24, DATA0
	call threefish_invmix_asm /* no rcall? */
	movw r24, DATA0
	adiw r24, 16
	mov r22, IDX0
	call threefish_invmix_asm /* no rcall? */
	inc I
	rjmp 1b

threefish256_slut5:
    .byte 0x00, 0x08, 0x10, 0x18, 0x20, 0x00, 0x08, 0x10
	.byte 0x18, 0x20, 0x00, 0x08, 0x10, 0x18, 0x20, 0x00
	.byte 0x08, 0x10, 0x18, 0x20, 0x00, 0x08, 0x10
threefish256_slut3:
	.byte 0x00, 0x08, 0x10, 0x00, 0x08, 0x10, 0x00, 0x08
	.byte 0x10, 0x00, 0x08, 0x10, 0x00, 0x08, 0x10, 0x00
	.byte 0x08, 0x10, 0x00, 0x08, 0x10, 0x00, 0x08
/* old round constants
threefish256_rc0: .byte 0x73, 0x13, 0x7b, 0x32, 0x72, 0x2b, 0x44, 0x1b
threefish256_rc1: .byte	0x62, 0x52, 0x43, 0x24, 0x54, 0x6a, 0x34, 0x70
*/
threefish256_rc0:  .byte 0x40, 0x72, 0x6a, 0x31, 0x1b, 0x39, 0x64, 0x2a
threefish256_rc1:  .byte 0x40, 0x3a, 0x14, 0x41, 0x5b, 0x50, 0x71, 0x20


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










