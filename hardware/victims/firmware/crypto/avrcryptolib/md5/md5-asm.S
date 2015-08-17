/* md5-asm.S */
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
 * Author:  Daniel Otte
 * License: GPLv3 or later
 * Date:    2008-11-15
*/


#include "avr-asm-macros.S"

;###########################################################	
; S-BOX

T_table:
.hword	0xa478, 0xd76a, 0xb756, 0xe8c7, 0x70db, 0x2420, 0xceee, 0xc1bd, 0x0faf, 0xf57c 
.hword	0xc62a, 0x4787, 0x4613, 0xa830, 0x9501, 0xfd46, 0x98d8, 0x6980, 0xf7af, 0x8b44 
.hword	0x5bb1, 0xffff, 0xd7be, 0x895c, 0x1122, 0x6b90, 0x7193, 0xfd98, 0x438e, 0xa679 
.hword	0x0821, 0x49b4, 0x2562, 0xf61e, 0xb340, 0xc040, 0x5a51, 0x265e, 0xc7aa, 0xe9b6 
.hword	0x105d, 0xd62f, 0x1453, 0x0244, 0xe681, 0xd8a1, 0xfbc8, 0xe7d3, 0xcde6, 0x21e1 
.hword	0x07d6, 0xc337, 0x0d87, 0xf4d5, 0x14ed, 0x455a, 0xe905, 0xa9e3, 0xa3f8, 0xfcef 
.hword	0x02d9, 0x676f, 0x4c8a, 0x8d2a, 0x3942, 0xfffa, 0xf681, 0x8771, 0x6122, 0x6d9d 
.hword	0x380c, 0xfde5, 0xea44, 0xa4be, 0xcfa9, 0x4bde, 0x4b60, 0xf6bb, 0xbc70, 0xbebf 
.hword	0x7ec6, 0x289b, 0x27fa, 0xeaa1, 0x3085, 0xd4ef, 0x1d05, 0x0488, 0xd039, 0xd9d4 
.hword	0x99e5, 0xe6db, 0x7cf8, 0x1fa2, 0x5665, 0xc4ac, 0x2244, 0xf429, 0xff97, 0x432a 
.hword	0x23a7, 0xab94, 0xa039, 0xfc93, 0x59c3, 0x655b, 0xcc92, 0x8f0c, 0xf47d, 0xffef 
.hword	0x5dd1, 0x8584, 0x7e4f, 0x6fa8, 0xe6e0, 0xfe2c, 0x4314, 0xa301, 0x11a1, 0x4e08 
.hword	0x7e82, 0xf753, 0xf235, 0xbd3a, 0xd2bb, 0x2ad7, 0xd391, 0xeb86


#define MD5_init_fast

.global md5_init 
#ifndef MD5_init_fast
;###########################################################	
;void md5_init(md5_ctx_t *state)
; param1: (r24,r25) 16-bit pointer to sha256_ctx_t struct in ram
; modifys: Z(r30,r31), X(r25,r26)
; size = 9+5*4 WORDS = 29 WORDS = 58 Bytes
md5_init:
	movw r26, r24 ; (24,25) --> (26,27) load X with param1
	ldi r30, lo8(md5_init_vector)
	ldi r31, hi8(md5_init_vector)
	ldi r24, 16+4
md5_init_vloop:	
	lpm r0, Z+ 
	st X+, r0
	dec r24
	brne md5_init_vloop
	ret
	
md5_init_vector:
.hword 0x2301, 0x6745
.hword 0xAB89, 0xEFCD 
.hword 0xDCFE, 0x98BA 
.hword 0x5476, 0x1032 
.hword 0x0000, 0x0000

#else
;###########################################################	
.global md5_init_fast 
;void md5_init(md5_ctx_t *state)
; param1: (r24,r25) 16-bit pointer to sha256_ctx_t struct in ram
; modifys: r23, r22
; cycles = 1+16*3+4*2+4 = 1+48+12 = 61
; size = 1+16*2+4+1 WORDS = 38 WORDS = 76 Bytes
md5_init:
md5_init_fast:
	movw r26, r24
	ldi r24, 0x01
	st X+, r24
	ldi r24, 0x23
	st X+, r24
	ldi r24, 0x45
	st X+, r24
	ldi r24, 0x67
	st X+, r24
	ldi r24, 0x89
	st X+, r24
	ldi r24, 0xAB
	st X+, r24
	ldi r24, 0xCD
	st X+, r24
	ldi r24, 0xEF
	st X+, r24
	ldi r24, 0xFE
	st X+, r24
	ldi r24, 0xDC
	st X+, r24
	ldi r24, 0xBA
	st X+, r24
	ldi r24, 0x98
	st X+, r24
	ldi r24, 0x76
	st X+, r24
	ldi r24, 0x54
	st X+, r24
	ldi r24, 0x32
	st X+, r24
	ldi r24, 0x10
	st X+, r24
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	ret
#endif
;###########################################################	

/*
static 
uint32_t md5_F(uint32_t x, uint32_t y, uint32_t z){
	return ((x&y)|((~x)&z));
}
*/
; x: r22-r25
; y: r18-r21
; z: r14-r17
md5_F:
	and r18, r22
	and r19, r23
	and r20, r24
	and r21, r25
	com r22
	com r23
	com r24
	com r25
	and r22, r14
	and r23, r15
	and r24, r16
	and r25, r17
	or  r22, r18
	or  r23, r19
	or  r24, r20
	or  r25, r21
	rjmp md5_core_F_exit
	
/*
static
uint32_t md5_G(uint32_t x, uint32_t y, uint32_t z){
	return ((x&z)|((~z)&y));
}
*/

; x: r22-r25
; y: r18-r21
; z: r14-r17
md5_G:
	and r22, r14
	and r23, r15
	and r24, r16
	and r25, r17
	com r14
	com r15
	com r16
	com r17
	and r18, r14
	and r19, r15
	and r20, r16
	and r21, r17
	or  r22, r18
	or  r23, r19
	or  r24, r20
	or  r25, r21
	rjmp md5_core_F_exit
/*
static
uint32_t md5_H(uint32_t x, uint32_t y, uint32_t z){
	return (x^y^z);
}
*/
; x: r22-r25
; y: r18-r21
; z: r14-r17
md5_H:
	eor r22, r18
	eor r22, r14
	eor r23, r19
	eor r23, r15
	eor r24, r20
	eor r24, r16
	eor r25, r21
	eor r25, r17
	rjmp md5_core_F_exit
/*
static
uint32_t md5_I(uint32_t x, uint32_t y, uint32_t z){
	return (y ^ (x | (~z)));
}
*/

jump_table:
	rjmp md5_F
	rjmp md5_G
	rjmp md5_H
;	rjmp md5_I

; x: r22-r25
; y: r18-r21
; z: r14-r17
md5_I:
	com r14
	com r15
	com r16
	com r17
	or  r22, r14
	or  r23, r15
	or  r24, r16
	or  r25, r17
	eor r22, r18
	eor r23, r19
	eor r24, r20
	eor r25, r21
	rjmp md5_core_F_exit

as_table:
;     (as+0)&3  (as+3)&3  (as+1)&3  (as+2)&3
;                  Z         X         Y
;     AS_SAVE0  AS_SAVE1  AS_SAVE2  AS_SAVE3 
.byte   1*4,      0*4,      2*4,      3*4    ;as=1
.byte   2*4,      1*4,      3*4,      0*4    ;as=2
.byte   3*4,      2*4,      0*4,      1*4    ;as=3
.byte   0*4,      3*4,      1*4,      2*4    ;as=4

;###########################################################	
.global md5_core
md5_core:
	mov r21, r20
	mov r20, r18
	mov r19, r16
	mov r18, r14
;	rjmp md5_core_asm
/*
void md5_core(uint32_t* a, void* block, uint8_t as, uint8_t s, uint8_t i, uint8_t fi){
	uint32_t t;
	md5_func_t* funcs[]={md5_F, md5_G, md5_H, md5_I};
	as &= 0x3;
	/ * a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). * /
	t = a[as] + funcs[fi](a[(as+1)&3], a[(as+2)&3], a[(as+3)&3]) + *((uint32_t*)block) + md5_T[i] ;
	a[as]=a[(as+1)&3] + ROTL32(t, s);
}
*/
; a:     r24-r25
; block: r22-r23
; as:    r21
; s:     r20
; i:     r19
; fi:    r18
P_A0 = 24
P_A1 = 25
P_B0 = 22
P_B1 = 23
P_AS = 21
P_S  = 20
P_I  = 19
P_FI = 18

; x: r22-r25
; y: r18-r21
; z: r14-r17


AS_SAVE0  =  4
AS_SAVE1  =  5
AS_SAVE2  =  6
AS_SAVE3  =  7
FI_SAVE   =  8
S_SAVE    =  9
ACCU0     = 10
ACCU1     = 11
ACCU2     = 12
ACCU3     = 13
ARG_X0    = 22
ARG_X1    = 23
ARG_X2    = 24
ARG_X3    = 25
ARG_Y0    = 18
ARG_Y1    = 19
ARG_Y2    = 20
ARG_Y3    = 21
ARG_Z0    = 14
ARG_Z1    = 15
ARG_Z2    = 16
ARG_Z3    = 17


md5_core_asm:
	push r16
	push r17
	push_range 4, 8
	ldi r30, lo8(T_table)
	ldi r31, hi8(T_table)
	lsl P_I
	rol r1
	lsl P_I
	rol r1
	add r30, P_I
	adc r31, r1
	clr r1
	mov FI_SAVE, r18
	/* loading T[i] into ACCU */	
	lpm ACCU0, Z+	
	lpm ACCU1, Z+	
	lpm ACCU2, Z+	
	lpm ACCU3, Z
	/* add *block to ACCU */
	movw r30, P_B0
	ld r0, Z+
	add ACCU0, r0
	ld r0, Z+
	adc ACCU1, r0
	ld r0, Z+
	adc ACCU2, r0
	ld r0, Z+
	adc ACCU3, r0
	/* add a[as+0&3] to ACCU */
	ldi r30, lo8(as_table)
	ldi r31, hi8(as_table)
	dec P_AS
	andi P_AS, 0x03
	lsl P_AS
	lsl P_AS
	add r30, r21
	adc r31, r1       ; Z points to the correct row in as_table
	lpm AS_SAVE0, Z+
	lpm AS_SAVE1, Z+
	lpm AS_SAVE2, Z+
	lpm AS_SAVE3, Z
	movw r26, r24     ; X points to a[0]
	add r26, AS_SAVE0
	adc r27, r1       ; X points at a[as&3]
	ld r0, X+
	add ACCU0, r0
	ld r0, X+
	adc ACCU1, r0
	ld r0, X+
	adc ACCU2, r0
	ld r0, X+
	adc ACCU3, r0
	mov S_SAVE, r20

	movw r28, r24
	/* loading z value */
	movw r26, r28
	add r26, AS_SAVE1
	adc r27, r1
	ld ARG_Z0, X+
	ld ARG_Z1, X+
	ld ARG_Z2, X+
	ld ARG_Z3, X

	/* loading x value */
	movw r26, r28	
	add r26, AS_SAVE2
	adc r27, r1
	ld ARG_X0, X+
	ld ARG_X1, X+
	ld ARG_X2, X+
	ld ARG_X3, X

	/* loading y value */
	movw r26, r28
	add r26, AS_SAVE3
	adc r27, r1
	ldi r30, pm_lo8(jump_table)
	ldi r31, pm_hi8(jump_table)
	add r30, FI_SAVE
	adc r31, r1    ; Z points to the correct entry in our jump table
	ld ARG_Y0, X+
	ld ARG_Y1, X+
	ld ARG_Y2, X+
	ld ARG_Y3, X

	ijmp /* calls the function pointed by Z */
md5_core_F_exit:		

	/* add ACCU to result of f() */
	add r22, ACCU0
	adc r23, ACCU1
	adc r24, ACCU2
	adc r25, ACCU3

	/* rotate */
	mov r20, S_SAVE
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
	mov r21, r25
bitrotl_loop:	
	tst r20
	breq fixrotl
bitrotl_loop2:	
	lsl r21
	rol r22
	rol r23
	rol r24
	rol r25
	dec r20
	brne bitrotl_loop2
fixrotl:

	/* add a[(as+1)&3]  */
	movw r26, r28
	add r26, AS_SAVE2
	adc r27, r1
	ld r0, X+
	add r22, r0
	ld r0, X+
	adc r23, r0
	ld r0, X+
	adc r24, r0
	ld r0, X
	adc r25, r0

	/* store result */
	movw r26, r28
	add r26, AS_SAVE0
	adc r27, r1
	st X+, r22
	st X+, r23
	st X+, r24
	st X , r25	
md5_core_exit:
	pop_range 4, 8
	pop r17
	pop r16
	ret

;###################################################################
/*
void md5_nextBlock(md5_ctx_t *state, void* block){
	uint32_t	a[4];
	uint8_t		m,n,i=0;

	a[0]=state->a[0];
	a[1]=state->a[1];
	a[2]=state->a[2];
	a[3]=state->a[3];
	
	/ * round 1 * /
	uint8_t s1t[]={7,12,17,22}; // 1,-1   1,4   2,-1   3,-2
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, &(((uint32_t*)block)[m*4+n]), 4-n, s1t[n],i++,0);
		}
	}
	/ * round 2 * /
	uint8_t s2t[]={5,9,14,20}; // 1,-3   1,1   2,-2   2,4
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, &(((uint32_t*)block)[(1+m*4+n*5)&0xf]), 4-n, s2t[n],i++,1);
		}
	}
	/ * round 3 * /
	uint8_t s3t[]={4,11,16,23}; // 0,4   1,3   2,0   3,-1
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, &(((uint32_t*)block)[(5-m*4+n*3)&0xf]), 4-n, s3t[n],i++,2);
		}
	}
	/ * round 4 * /
	uint8_t s4t[]={6,10,15,21}; // 1,-2   1,2   2,-1   3,-3
	for(m=0;m<4;++m){
		for(n=0;n<4;++n){
			md5_core(a, &(((uint32_t*)block)[(0-m*4+n*7)&0xf]), 4-n, s4t[n],i++,3);
		}
	}
	state->a[0] += a[0];
	state->a[1] += a[1];
	state->a[2] += a[2];
	state->a[3] += a[3];
	state->counter++;
}
*/

shift_table_1:  .byte  7,12,17,22
shift_table_2:  .byte  5, 9,14,20
shift_table_3:  .byte  4,11,16,23
shift_table_4:  .byte  6,10,15,21

index_table_r2:
;(1+m*4+n*5)&0xf:
        .byte 0x04, 0x18, 0x2c, 0x00 
        .byte 0x14, 0x28, 0x3c, 0x10 
        .byte 0x24, 0x38, 0x0c, 0x20 
        .byte 0x34, 0x08, 0x1c, 0x30 

index_table_r3:
;(5-m*4+n*3)&0xf:
        .byte 0x14, 0x20, 0x2c, 0x38 
        .byte 0x04, 0x10, 0x1c, 0x28 
        .byte 0x34, 0x00, 0x0c, 0x18 
        .byte 0x24, 0x30, 0x3c, 0x08 

index_table_r4:
;(0-m*4+n*7)&0xf:
        .byte 0x00, 0x1c, 0x38, 0x14 
        .byte 0x30, 0x0c, 0x28, 0x04 
        .byte 0x20, 0x3c, 0x18, 0x34 
        .byte 0x10, 0x2c, 0x08, 0x24

APTR_REG = 2
BPTR_REG = 4
N_REG = 6
M_REG = 7
I_REG = 8
.global md5_nextBlock
md5_nextBlock:
	stack_alloc 16
	push_range 2, 17
	push r28
	push r29
	push r24
	push r25
	adiw r30, 1 /* Z now points to the beginning of the allocated memory */
	movw r2, r30
	movw r4, r22
	movw r26, r24
	ldi r20, 16
1:
	ld r0, X+
	st Z+, r0
	dec r20
	brne 1b
	/* state now copied to stack memory */
	clr I_REG	
	/* Round 1 */
	clr M_REG
	ldi r17, 4
1:
	clr N_REG	
	ldi r16, 4
2:
	movw r24, APTR_REG
	movw r22, BPTR_REG 
	mov r0, M_REG
	lsl r0
	lsl r0
	add r0, N_REG
	lsl r0
	lsl r0
	add r22, r0
	adc r23, r1
	mov r21, r16	
	ldi r30, lo8(shift_table_1)
	ldi r31, hi8(shift_table_1)
	add r30, N_REG
	adc r31, r1
	lpm r20, Z
	mov r19, I_REG
	ldi r18, 0
	rcall md5_core_asm
	inc I_REG
	inc N_REG
	dec r16
	brne 2b
	inc M_REG
	dec r17
	brne 1b
	
	/* Round 2 */
	clr M_REG
	ldi r17, 4
1:
	clr N_REG	
	ldi r16, 4
2:
	movw r24, APTR_REG
	movw r22, BPTR_REG 
	ldi r30, lo8(index_table_r2)
	ldi r31, hi8(index_table_r2)
	mov r0, M_REG
	lsl r0
	lsl r0
	add r0, N_REG
	add r30, r0
	adc r31, r1
	lpm r0, Z	
	add r22, r0
	adc r23, r1
	mov r21, r16	
	ldi r30, lo8(shift_table_2)
	ldi r31, hi8(shift_table_2)
	add r30, N_REG
	adc r31, r1
	lpm r20, Z
	mov r19, I_REG
	ldi r18, 1
	rcall md5_core_asm
	inc I_REG
	inc N_REG
	dec r16
	brne 2b
	inc M_REG
	dec r17
	brne 1b

	/* Round 3 */
	clr M_REG
	ldi r17, 4
1:
	clr N_REG	
	ldi r16, 4
2:
	movw r24, APTR_REG
	movw r22, BPTR_REG 
	ldi r30, lo8(index_table_r3)
	ldi r31, hi8(index_table_r3)
	mov r0, M_REG
	lsl r0
	lsl r0
	add r0, N_REG
	add r30, r0
	adc r31, r1
	lpm r0, Z	
	add r22, r0
	adc r23, r1
	mov r21, r16	
	ldi r30, lo8(shift_table_3)
	ldi r31, hi8(shift_table_3)
	add r30, N_REG
	adc r31, r1
	lpm r20, Z
	mov r19, I_REG
	ldi r18, 2
	rcall md5_core_asm
	inc I_REG
	inc N_REG
	dec r16
	brne 2b
	inc M_REG
	dec r17
	brne 1b

	/* Round 4 */
	clr M_REG
	ldi r17, 4
1:
	clr N_REG	
	ldi r16, 4
2:
	movw r24, APTR_REG
	movw r22, BPTR_REG 
	ldi r30, lo8(index_table_r4)
	ldi r31, hi8(index_table_r4)
	mov r0, M_REG
	lsl r0
	lsl r0
	add r0, N_REG
	add r30, r0
	adc r31, r1
	lpm r0, Z	
	add r22, r0
	adc r23, r1
	mov r21, r16	
	ldi r30, lo8(shift_table_4)
	ldi r31, hi8(shift_table_4)
	add r30, N_REG
	adc r31, r1
	lpm r20, Z
	mov r19, I_REG
	ldi r18, 3
	rcall md5_core_asm
	inc I_REG
	inc N_REG
	dec r16
	brne 2b
	inc M_REG
	dec r17
	brne 1b


	pop r27
	pop r26 /* X now points to the context */
	movw r30, APTR_REG
	ldi r16, 4
1:
	ld r0, X
	ld r2, Z+
	add r0, r2
	st X+, r0	
	ld r0, X
	ld r2, Z+
	adc r0, r2
	st X+, r0	
	ld r0, X
	ld r2, Z+
	adc r0, r2
	st X+, r0	
	ld r0, X
	ld r2, Z+
	adc r0, r2
	st X+, r0	
	dec r16
	brne 1b

	ld r0, X
	inc r0
	st X+, r0	
	brne 2f
	ld r0, X
	inc r0
	st X+, r0	
	brne 2f
	ld r0, X
	inc r0
	st X+, r0	
	brne 2f	
	ld r0, X
	inc r0
	st X+, r0	
2:			

	pop r29
	pop r28
	pop_range 2, 17
	stack_free 16
	ret

;###############################################################################
/*
void md5_lastBlock(md5_ctx_t *state, const void* block, uint16_t length_b){
	uint16_t l;
	uint8_t b[64];
	while (length_b >= 512){
		md5_nextBlock(state, block);
		length_b -= 512;
		block = ((uint8_t*)block) + 512/8;
	}
	memset(b, 0, 64);
	memcpy(b, block, length_b/8);
	/ * insert padding one * /
	l=length_b/8;
	if(length_b%8){
		uint8_t t;
		t = ((uint8_t*)block)[l];
		t |= (0x80>>(length_b%8));
		b[l]=t;
	}else{
		b[l]=0x80;
	}
	/ * insert length value * /
	if(l+sizeof(uint64_t) >= 512/8){
		md5_nextBlock(state, b);
		state->counter--;
		memset(b, 0, 64-8);
	}
	*((uint64_t*)&b[64-sizeof(uint64_t)]) = (state->counter * 512) + length_b;
	md5_nextBlock(state, b);
}
*/
; state_ptr : r24,r25
; block_ptr : r22,r23
; length_b  : r20,r21
.global md5_lastBlock
md5_lastBlock:
	stack_alloc_large 64
	push_range 12, 17
	push r30
	push r31
	movw r16, r20 /* length_b  */ 
	movw r14, r22 /* block_ptr */
	movw r12, r24 /* state_ptr */
	ldi r18, 64
2:
	cpi r17, 2 /* hi8(512) */	
	brlo 2f
1:
	movw r24, r12
	movw r22, r14
	rcall md5_nextBlock
	add r14, r18
	adc r15, r1
	subi r17, 2
	rjmp 2b
2:
	pop r31
	pop r30

	adiw r30, 1 /* adjust Z to point to buffer */
	movw r26, r14
	movw r24, r16
	adiw r24, 7

	lsr r25
	ror r24
	lsr r25
	ror r24
	lsr r24 /* r24 now holds how many bytes are to copy */
    ldi r18, 64
	sub r18, r24 /* r18 will hold the amount of used bytes in buffer */
	tst r24
4:
	breq 5f
	ld r0, X+
	st Z+, r0 
	dec r24
	rjmp 4b /* Z points to the byte after msg in buffer */
5:	/* append 1-bit */
	mov r20, r16
	ldi r19, 0x80
	andi r20, 0x07
	brne bit_fucking
	st Z+, r19
	dec r18 /* 'allocate' another byte in buffer */
	rjmp after_bit_fucking
bit_fucking:
1:
	lsr r19
	dec r20
	brne 1b
	or r0, r19
	st -Z, r0
    adiw r30, 1
after_bit_fucking:
	clt	
	cpi r18, 8
	brmi 2f
	set         /* store in t if the counter will also fit in this block (1 if fit)*/
2:
	tst r18
	breq 2f
1: /* fill remaning buffer with zeros */
	st Z+, r1
	dec r18
	brne 1b
2:
	sbiw r30, 63
	sbiw r30,  1
	movw r14, r30 /* r14:r15 now points to buffer */	
	brts load_counter
	/* counter does not fit, finalize this block */
	movw r24, r12
	movw r22, r14
	rcall md5_nextBlock
	movw r30, r14
	ldi r20, 64-8
3:
	st Z+, r1
	dec r20
	brne 3b
	
load_counter:		
	movw r26, r12 /* X points to state */
	adiw r26, 16
	ld r19, X+
	ld r20, X+
	ld r21, X+
	ld r22, X+
	brts post_counter_decrement	/* do not decremen because counter fits */
counter_decrement:
	subi r19, 1
	sbci r20, 0
	sbci r21, 0
	sbci r22, 0
post_counter_decrement:
	clr r18
	clr r23
	lsl r19
	rol r20
	rol r21
	rol r22
	rol r23
	mov r18, r16 /* r16:r17 length_b */
	add r19, r17
	adc r20, r1
	adc r21, r1
	adc r22, r1
	adc r23, r1
	movw r30, r14
	adiw r30, 64-8
	st Z+, r18
	st Z+, r19
	st Z+, r20
	st Z+, r21
	st Z+, r22
	st Z+, r23
	st Z+, r1
	st Z, r1

	sbiw r30, 63
;	sbiw r30, 1
	movw r24, r12
	movw r22, r30
	rcall md5_nextBlock
md5_lastBlock_exit:	
	pop_range 12, 17
	stack_free_large 64
	ret


;###############################################################################


.global md5_ctx2hash
md5_ctx2hash:
	movw r26, r24
	movw r30, r22
	ldi r22, 16
1:
	ld r0, Z+
	st X+, r0
	dec r22
	brne 1b	
	ret


;###############################################################################


.global md5
md5:
	stack_alloc 20
	push_range  8, 17
	adiw r30, 1
	movw  r8, r30 /* ctx           */
	movw r10, r24 /* dest          */
	movw r12, r22 /* msg           */
	movw r14, r18 /* length (low)  */
	movw r16, r20 /* length (high) */
	movw r24, r30
	rcall md5_init
1:
	tst r16
	brne next_round
	tst r17
	breq last_round
next_round:
	movw r24,  r8
	movw r22, r12
	rcall md5_nextBlock
	ldi r22, 64
	add r12, r22
	adc r13, r1
	ldi r22, 2
	sub r15, r22
	sbci r16, 0
	sbci r17, 0
	rjmp 1b
last_round:		
	movw r24, r8
	movw r22, r12
	movw r20, r14
	rcall md5_lastBlock
	movw r24, r10
	movw r22,  r8
	rcall md5_ctx2hash
	pop_range  8, 17
	stack_free 20
	ret



