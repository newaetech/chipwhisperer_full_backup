/* shabal-asm.S */
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
 * \file    shabal-asm.S
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

/******************************************************************************/
/*
void shabal_p(shabal_ctx_t* ctx, const void* m){
	uint8_t i,j;
	for(i=0;i<16;++i){
		ctx->b[i] = ROTL32(ctx->b[i],17);
	}
	for(j=0;j<SHABAL_P;++j){
		for(i=0;i<16;++i){
			ctx->a[(i+16*j)%SHABAL_R] =
				shabal_u(ctx->a[(i+16*j)%SHABAL_R] 
					^ shabal_v(ROTL32(ctx->a[(i+16*j+SHABAL_R-1)%SHABAL_R],15))
				    ^ ctx->c[(8-i+16)%16])
				^ ctx->b[(i+SHABAL_O1)%16]
				^ ((ctx->b[(i+SHABAL_O2)%16]) & ~(ctx->b[(i+SHABAL_O3)%16]))
				^ ((uint32_t*)m)[i];
			ctx->b[i] = ROTL32(ctx->b[i], 1) ^ ~(ctx->a[(i+16*j)%SHABAL_R]);
		}
	}
	
	for(j=0;j<36;++j){
		ctx->a[j%SHABAL_R] += ctx->c[(j+3)%16];
	}
}
*/
MB0 =  2
MB1 =  3
AB0 =  4
AB1 =  5
BB0 =  6
BB1 =  7
CB0 =  8
CB1 =  9
AL0 = 10
AL1 = 11
AL2 = 12
AL3 = 13
A0  = 14
A1  = 15
A2  = 16
A3  = 17
B0  = 18
B1  = 19
B2  = 20
B3  = 21
I   = 22
J   = 23
T0  = 26
T1  = 27
T2  = 28
T3  = 29
/*
 * param ctx: r24:r25
 * param m:   r22:r23 
 */
; .global shabal_p
shabal_p:
	push_range 2, 17
	push r28
	push r29
	movw MB0, r22
	movw r30, r24
	adiw r30, 8
	ld BB0, Z+
	ld BB1, Z+
	ld CB0, Z+
	ld CB1, Z+
	movw AB0, r30
	movw r30, BB0
	adiw r30, 16*4-1
	adiw r30, 1
	ldi r24, 16
1:
	ld A3, -Z
	ld A2, -Z
	ld A1, -Z
	ld A0, -Z	
	mov r0, A3
	rol r0
	rol A0
	rol A1
	rol A2
	rol A3
	std Z+0, A2
	std Z+1, A3
	std Z+2, A0
	std Z+3, A1
	dec r24
	brne 1b
	movw B0, A2
	movw B2, A0
	/* load ctx->a[(i+16*j-1)%12]*/
	movw r26, AB0
	adiw r26, 4*11
	ld AL0, X+
	ld AL1, X+
	ld AL2, X+
	ld AL3, X+
	clr I
	clr J
1:
	/* ROTL32(AL, 15)*/
	movw T0, AL2
	movw T2, AL0
	mov r0, T0
	ror r0
	ror T3
	ror T2
	ror T1
	ror T0
	movw AL0, T0
	movw AL2, T2
	/* apply V to AL */
	movw A0, AL0
	movw A2, AL2
	lsl A0
	rol A1
	rol A2
	rol A3
	lsl A0
	rol A1
	rol A2
	rol A3
	add A0, AL0
	adc A1, AL1	
	adc A2, AL2
	adc A3, AL3
	/* xor in ctx->c[(8-i+16)%16] */
	ldi T0, 24
	sub T0, I
	andi T0, 0x0f
	lsl T0
	lsl T0
	movw r30, CB0
	add r30, T0
	adc r31, r1
	ld r0, Z+
	eor A0, r0
	ld r0, Z+
	eor A1, r0
	ld r0, Z+
	eor A2, r0
	ld r0, Z+
	eor A3, r0
	/* xor in ctx->a[(i+16*j)%12] */
	mov T0, J
	swap T0 /* *=16 */
	add T0, I
	ldi r30, lo8(mod12table)
	ldi r31, hi8(mod12table)
	add r30, T0
	adc r31, r1
	lpm T0, Z
	movw r30, AB0
	add r30, T0
	adc r31, r1
	movw T2, r30
	ld r0, Z+
	eor A0, r0
	ld r0, Z+
	eor A1, r0
	ld r0, Z+
	eor A2, r0
	ld r0, Z+
	eor A3, r0
	/* AL = 3*A */
	movw AL0, A0
	movw AL2, A2
	lsl AL0
	rol AL1
	rol AL2
	rol AL3
	add AL0, A0
	adc AL1, A1
	adc AL2, A2
	adc AL3, A3
	/* xor in ctx->b[(i+13)%16] */
	ldi T0, 13
	add T0, I
	andi T0, 0x0f
	lsl T0
	lsl T0
	movw r30, BB0
	add r30, T0
	adc r31, r1
	ld r0, Z+
	eor AL0, r0
	ld r0, Z+
	eor AL1, r0
	ld r0, Z+
	eor AL2, r0
	ld r0, Z+
	eor AL3, r0
	/* load ctx->b[(i+9)%16] into A */
	ldi T0, 9
	add T0, I
	andi T0, 0x0f
	lsl T0
	lsl T0
	movw r30, BB0
	add r30, T0
	adc r31, r1
	ld A0, Z+
	ld A1, Z+
	ld A2, Z+
	ld A3, Z+
	/* and in ~(ctx->b[(i+6)%16]) */
	ldi T0, 6
	add T0, I
	andi T0, 0x0f
	lsl T0
	lsl T0
	movw r30, BB0
	add r30, T0
	adc r31, r1
	ld r0, Z+
	com r0
	and A0, r0
	ld r0, Z+
	com r0
	and A1, r0
	ld r0, Z+
	com r0
	and A2, r0
	ld r0, Z+
	com r0
	and A3, r0
	/* xor A into AL */
	eor AL0, A0
	eor AL1, A1
	eor AL2, A2
	eor AL3, A3
	/* xor m[i] into AL */
	mov T0, I
	lsl T0
	lsl T0
	movw r30, MB0
	add r30, T0
	adc r31, r1
	ld r0, Z+
	eor AL0, r0
	ld r0, Z+
	eor AL1, r0
	ld r0, Z+
	eor AL2, r0
	ld r0, Z+
	eor AL3, r0
	/* A (AL) is done, now store it */
	movw r30, T2
	st Z+, AL0
	st Z+, AL1
	st Z+, AL2
	st Z+, AL3
	/* process ctx->b[i] */
	/* ROTL32(b, 1)*/
	mov r0, B3
	rol r0
	rol B0
	rol B1
	rol B2
	rol B3
	/* xor in ~(ctx->a[(i+16*j)%SHABAL_R]) */
	movw A0, AL0
	movw A2, AL2
	com A0
	com A1
	com A2
	com A3
	eor B0, A0
	eor B1, A1
	eor B2, A2
	eor B3, A3
	/* store B */
	movw r30, BB0
	mov T0, I
	lsl T0
	lsl T0
	add r30, T0
	adc r31, r1
	st Z+, B0
	st Z+, B1
	st Z+, B2
	st Z+, B3
	inc I
	cpi I, 16
	brne local_reload
	inc J
	cpi J, 3
	brne global_reload
	rjmp addition
global_reload:
	clr I
local_reload:	
	mov T0, I
	lsl T0
	lsl T0
	movw r30, BB0 
	add r30, T0
	adc r31, r1
	ld B0, Z+
	ld B1, Z+
	ld B2, Z+
	ld B3, Z+
	
	rjmp 1b
addition:
	clr J
	movw r30, AB0
	movw r26, CB0
	adiw r26, 3*4
1:
	/* J = 0..11 */
	ld AL0, X+
	ld A0, Z
	add A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	inc J
	cpi J, 12
	brne 1b
	/* J = 12 */
	movw r30, AB0
	ld AL0, X+
	ld A0, Z
	add A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	inc J
	/* J= 13..23*/
	movw r26, CB0
1:	
	ld AL0, X+
	ld A0, Z
	add A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	inc J
	cpi J, 24
	brne 1b
	/* J= 24..28*/
	movw r30, AB0
1:	
	ld AL0, X+
	ld A0, Z
	add A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	inc J
	cpi J, 29
	brne 1b

	/* J= 29..35*/
	movw r26, CB0
1:	
	ld AL0, X+
	ld A0, Z
	add A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	ld AL0, X+
	ld A0, Z
	adc A0, AL0
	st Z+, A0
	inc J
	cpi J, 36
	brne 1b	
exit:	
	pop r29
	pop r28
	pop_range 2, 17
	ret

mod12table:
	.byte  0,  4,  8, 12, 16, 20, 24, 28
	.byte 32, 36, 40, 44,  0,  4,  8, 12
	.byte 16, 20, 24, 28, 32, 36, 40, 44
	.byte  0,  4,  8, 12, 16, 20, 24, 28
	.byte 32, 36, 40, 44,  0,  4,  8, 12
	.byte 16, 20, 24, 28, 32, 36, 40, 44
	
/******************************************************************************/
/*
void shabal_nextBlock(shabal_ctx_t* ctx, const void* block){
	uint8_t i;
	uint32_t* t;
	for(i=0;i<16;++i){
		ctx->b[i] += ((uint32_t*)block)[i];
	}
	ctx->a[0] ^= ctx->w.w32[0];
	ctx->a[1] ^= ctx->w.w32[1];
	shabal_p(ctx, block);
	for(i=0;i<16;++i){
		ctx->c[i] -= ((uint32_t*)block)[i];
	}
	ctx->w.w64++;
	t = ctx->c;
	ctx->c = ctx->b;
	ctx->b = t;
}
*/
/*
 * param ctx:    r24:r25
 * param block:  r22:r23
 */
MB0  = 14
MB1  = 15
CTX0 = 16
CTX1 = 17
.global shabal_nextBlock
shabal_nextBlock:
	push_range 14, 17
	movw CTX0, r24
	movw MB0, r22
	/* xor W into A and increment W */
	movw r30, CTX0
	ldi r19, 8
	sec
1:	
	ld r20, Z
	ldd r21, Z+(8+4)
	eor r21, r20
	std Z+(8+4), r21
	adc r20, r1
	st Z+, r20 
	dec r19
	brne 1b
	/* add block to ctx->b */
	ld r26, Z+
	ld r27, Z
	movw r30, MB0
	ldi r19, 16
1:	
	ld r0, X
	ld r18, Z+
	add r0, r18
	st X+, r0
	ld r0, X
	ld r18, Z+
	adc r0, r18
	st X+, r0
	ld r0, X
	ld r18, Z+
	adc r0, r18
	st X+, r0
	ld r0, X
	ld r18, Z+
	adc r0, r18
	st X+, r0
	dec r19
	brne 1b
	/* call shbal_p */
	rcall shabal_p
	/* sub block from ctx->c */
	movw r30, CTX0
	adiw r30, 8+2
	ld r26, Z+
	ld r27, Z
	movw r30, MB0
	ldi r19, 16
1:	
	ld r0, X
	ld r18, Z+
	sub r0, r18
	st X+, r0
	ld r0, X
	ld r18, Z+
	sbc r0, r18
	st X+, r0
	ld r0, X
	ld r18, Z+
	sbc r0, r18
	st X+, r0
	ld r0, X
	ld r18, Z+
	sbc r0, r18
	st X+, r0
	dec r19
	brne 1b
	/* xchange ctx->b with ctx->c*/
	movw r30, CTX0
	ldd r22, Z+8
	ldd r23, Z+9
	ldd r24, Z+10
	ldd r25, Z+11
	std Z+10, r22
	std Z+11, r23
	std Z+8,  r24
	std Z+9,  r25
	pop_range 14, 17
	ret

/******************************************************************************/
/*
void shabal_lastBlock(shabal_ctx_t* ctx, const void* block, uint16_t length_b){
	uint8_t i,j;
	uint32_t* t;
	uint8_t buffer[64];
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(ctx, block);
		block = (uint8_t*)block + SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	memset(buffer, 0, 64);
	memcpy(buffer, block, (length_b+7)/8);
	buffer[length_b/8] |= 0x80>>(length_b%8);
	for(i=0;i<16;++i){
		ctx->b[i] += ((uint32_t*)buffer)[i];
	}
	for(j=0; j<4;++j){
		ctx->a[0] ^= ctx->w.w32[0];
		ctx->a[1] ^= ctx->w.w32[1];
		shabal_p(ctx, buffer);
		t = ctx->c;
		ctx->c = ctx->b;
		ctx->b = t;
	}	 
}
*/
I    = 16
LEN0 = 16
LEN1 = 17
CTX0 = 14
CTX1 = 15
MB0  = 12
MB1  = 13
/*
 * param ctx:       r24:r25
 * param block:     r22:r23
 * param length_b:  r20:r21
 */
.global shabal_lastBlock
shabal_lastBlock:
	push_range 12, 17
	movw CTX0, r24
	movw MB0,  r22
	movw LEN0, r20
1:
	cpi LEN1, 0x02
	brlo 2f
	movw r24, CTX0
	movw r22, MB0
	rcall shabal_nextBlock
	subi LEN1, 0x02
	ldi r18, 64
	add	MB0, r18
	adc MB1, r1
	rjmp 1b
2:	
	stack_alloc_large 64
	adiw r30, 1 /* Z points at buffer */
	movw r26, MB0
	/* r24 = LEN/8*/
	movw r24, LEN0
	lsr r25
	ror r24
	lsr r24
	lsr r24
	ldi r25, 64-1
	sub r25, r24
	tst r24
	breq 32f
31:
	ld r0, X+
	st Z+, r0
	dec r24
	brne 31b
32:
	ldi r18, 0x80
	andi LEN0, 0x07
	breq append_0x80
	ld r0, X+
33:
	lsr r18
	dec LEN0
	brne 33b
	or r0, r18
	st Z+, r0
	rjmp append_zeros
append_0x80:
	st Z+, r18
append_zeros:
	tst r25
	breq 4f
34: st Z+, r1
	dec r25
	brne 34b
4:		
	sbiw r30, 63
	sbiw r30,  1
	movw MB0, r30
	movw r26, CTX0
	adiw r26, 8
	ld r24, X+
	ld r25, X
	movw r26, r24
	ldi r18, 16
41: 
	ld r24, X
	ld r25, Z+
	add r24, r25
	st X+, r24
	ld r24, X
	ld r25, Z+
	adc r24, r25
	st X+, r24	
	ld r24, X
	ld r25, Z+
	adc r24, r25
	st X+, r24	
	ld r24, X
	ld r25, Z+
	adc r24, r25
	st X+, r24	
	dec r18
	brne 41b
	/* final loop */
	ldi I, 4
5:
	/* xor W into A */
	movw r30, CTX0
	ldi r19, 8
51:	
	ld  r24, Z+
	ldd r25, Z+(8+4-1)
	eor r24, r25
	std Z+(8+4-1), r24	
	dec r19
	brne 51b
	movw r24, CTX0
	movw r22, MB0
	rcall shabal_p
	movw r30, CTX0
	ldd r22, Z+8
	ldd r23, Z+9
	ldd r24, Z+10
	ldd r25, Z+11
	std Z+10, r22
	std Z+11, r23
	std Z+8,  r24
	std Z+9,  r25
	dec I
	brne 5b
	
	stack_free_large 64
	pop_range 12, 17
	ret

	

	
	
	
	
	
	
	
	
	
	
	
