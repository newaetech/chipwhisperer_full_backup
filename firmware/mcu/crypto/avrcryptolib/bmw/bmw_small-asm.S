/* bmw_small-asm.S */
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
 * File:        bmw_small-asm.S
 * Author:      Daniel Otte
 * Date:        2009-11-13
 * License:     GPLv3 or later
 * Description: implementation of BlueMidnightWish
 *
 */

#include "avr-asm-macros.S"

shiftcodetable:
;	.byte 0x00 ;  0
shiftcodetable_1:
	.byte 0x01 ;  1
	.byte 0x02 ;  2
	.byte 0x03 ;  3
	.byte 0x04 ;  4
	.byte 0x1B ;  5
	.byte 0x1A ;  6
	.byte 0x19 ;  7
	.byte 0x10 ;  8
shiftcodetable_9:
	.byte 0x11 ;  9
	.byte 0x12 ; 10
	.byte 0x13 ; 11
	.byte 0x2C ; 12
	.byte 0x2B ; 13
	.byte 0x2A ; 14
	.byte 0x29 ; 15
	.byte 0x20 ; 16
;	.byte 0x21 ; 17 unused but necesseray for padding



/*******************************************************************************
 * shiftl32
 *   value: r25:r22
 *   shift: r20
 */
shiftl32:
1:
;	clc
	lsl r22
	rol r23
	rol r24
	rol r25
	dec r20
	brne 1b
	ret

/*******************************************************************************
 * shiftr32
 *   value: r25:r22
 *   shift: r20
 */
shiftr32:
1:
;	clc
	lsr r25
	ror r24
	ror r23
	ror r22
	dec r20
	brne 1b
	ret

/*******************************************************************************
 * rotl32
 *   value: r25:r22
 *   shift: r20
 */
rotl32:
	mov r21, r25
1:
	lsl r21
	rol r22
	rol r23
	rol r24
	rol r25
	dec r20
	brne 1b
	ret

/*******************************************************************************
 * rotr32
 *   value: r25:r22
 *   shift: r20
 */
rotr32:
	mov r21, r22
1:
	lsr r21
	ror r25
	ror r24
	ror r23
	ror r22
	dec r20
	brne 1b
some_ret:
	ret

/*******************************************************************************
 * rotl32p9
 *   value: r25:r22
 *   shift: r20
 */
rotl32p9:
	push_range 30, 31
	ldi r30, lo8(shiftcodetable_9)
	ldi r31, hi8(shiftcodetable_9)
	add r30, r20
	adc r31, r1
	lpm r20, Z
	pop_range 30, 31
	sbrs r20, 4
	rjmp 2f
	mov r0, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r0
2:	sbrs r20, 5
	rjmp 3f
	movw r0, r24
	movw r24, r22
	movw r22, r0
	clr r1
3:  bst r20, 3
	andi r20, 0x07
	breq some_ret
	brts rotr32
	rjmp rotl32


/*******************************************************************************
* uint32_t rotl_addel(uint32_t x, uint8_t v){
* 	uint32_t r;
*	r =  ROTL32(x, (v&0xf)+1);
*	return r;
* }
* param x: r25:r22
* param v: r20
*/
.global rotl_addel
rotl_addel:
	andi r20, 0x0f
	ldi r30, lo8(shiftcodetable_1)
	ldi r31, hi8(shiftcodetable_1)
	add r30, r20
	adc r31, r1
	lpm r20, Z
	sbrs r20, 4
	rjmp 1f
	mov r21, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r21
1:  sbrs r20, 5
	rjmp 2f
	movw r30, r24
	movw r24, r22
	movw r22, r30
2:  bst  r20, 3
	andi r20, 0x07
	breq some_ret
3:
	brts rotr32; 4f
	rjmp rotl32
;4:	rjmp rotr32

/******************************************************************************/

preg0 = 22 /* preg for processing register */
preg1 = 23
preg2 = 24
preg3 = 25
breg0 = 26 /* breg for backup register */
breg1 = 27
breg2 = 18
breg3 = 19
areg0 =  0 /* areg for accumulator register */
areg1 =  1
areg2 = 30
areg3 = 31

/*******************************************************************************
* uint32_t bmw_small_s0(uint32_t x){
* 	uint32_t r;
*	r =   SHR32(x, 1)
*		^ SHL32(x, 3)
*		^ ROTL32(x, 4)
*		^ ROTR32(x, 13);
*	return r;
* }
*/
.global bmw_small_s0
bmw_small_s0:
	movw breg0, preg0
	movw breg2, preg2
	ldi r20, 1
	rcall shiftr32
	movw areg2, preg2
	movw areg0, preg0
	movw preg2, breg2
	movw preg0, breg0
	ldi r20, 3
	rcall shiftl32
	eor areg0, preg0
	eor areg1, preg1
	eor areg2, preg2
	eor areg3, preg3
	movw preg2, breg2
	movw preg0, breg0
	ldi r20, 4
	rcall rotl32
	eor areg0, preg0
	eor areg1, preg1
	eor areg2, preg2
	eor areg3, preg3
	/* now the trick, we simply can rotate the old value to the right by 17 */
	movw breg0, preg0 /* first rotate by 16 */
	movw preg0, preg2
	movw preg2, breg0
outro_1:
	ldi r20, 1
	rcall rotr32
outro_2:
	eor preg0, areg0
	eor preg1, areg1
	eor preg2, areg2
	eor preg3, areg3
	clr r1
	ret

/*******************************************************************************
* uint32_t bmw_small_s1(uint32_t x){
* 	uint32_t r;
*	r =   SHR32(x, 1)
*		^ SHL32(x, 2)
*		^ ROTL32(x, 8)
*		^ ROTR32(x, 9);
*	return r;
* }
*/
.global bmw_small_s1
bmw_small_s1:
	movw breg0, preg0
	movw breg2, preg2
	ldi r20, 1
	rcall shiftr32
	movw areg2, preg2
	movw areg0, preg0
	movw preg2, breg2
	movw preg0, breg0
	ldi r20, 2
	rcall shiftl32
	eor areg0, preg0
	eor areg1, preg1
	eor areg2, preg2
	eor areg3, preg3
	eor areg0, breg3
	eor areg1, breg0
	eor areg2, breg1
	eor areg3, breg2
	mov preg0, breg1
	mov preg1, breg2
	mov preg2, breg3
	mov preg3, breg0
	rjmp outro_1

/*******************************************************************************
* uint32_t bmw_small_s2(uint32_t x){
* 	uint32_t r;
*	r =   SHR32(x, 2)
*		^ SHL32(x, 1)
*		^ ROTL32(x, 12)
*		^ ROTR32(x, 7);
*	return r;
* }
*/
.global bmw_small_s2
bmw_small_s2:
	movw breg0, preg0
	movw breg2, preg2
	ldi r20, 2
	rcall shiftr32
	movw areg2, preg2
	movw areg0, preg0
	movw preg2, breg2
	movw preg0, breg0
	ldi r20, 1
	rcall shiftl32
	eor areg0, preg0
	eor areg1, preg1
	eor areg2, preg2
	eor areg3, preg3
	movw preg0, breg2
	movw preg2, breg0
	ldi r20, 4
	rcall rotr32
	eor areg0, preg0
	eor areg1, preg1
	eor areg2, preg2
	eor areg3, preg3
	mov preg0, breg1
	mov preg1, breg2
	mov preg2, breg3
	mov preg3, breg0
	ldi r20, 1
	rcall rotl32
	rjmp outro_2

/*******************************************************************************
* uint32_t bmw_small_s3(uint32_t x){
* 	uint32_t r;
*	r =   SHR32(x, 2)
*		^ SHL32(x, 2)
*		^ ROTL32(x, 15)
*		^ ROTR32(x, 3);
*	return r;
* }
*/
.global bmw_small_s3
bmw_small_s3:
	movw breg0, preg0
	movw breg2, preg2
	ldi r20, 2
	rcall shiftr32
	movw areg2, preg2
	movw areg0, preg0
	movw preg2, breg2
	movw preg0, breg0
	ldi r20, 2
	rcall shiftl32
	eor areg0, preg0
	eor areg1, preg1
	eor areg2, preg2
	eor areg3, preg3
	movw preg0, breg2
	movw preg2, breg0
	ldi r20, 1
	rcall rotr32
	eor areg0, preg0
	eor areg1, preg1
	eor areg2, preg2
	eor areg3, preg3
	movw preg0, breg0
	movw preg2, breg2
	ldi r20, 3
	rcall rotr32
	rjmp outro_2

/*******************************************************************************
* uint32_t bmw_small_s4(uint32_t x){
* 	uint32_t r;
*	r =  SHR32(x, 1)
*		 ^ x;
*	return r;
* }
*/
.global bmw_small_s4
bmw_small_s4:
	movw areg0, preg0
	movw areg2, preg2
	ldi r20, 1
	rcall shiftr32
	rjmp outro_2

/*******************************************************************************
* uint32_t bmw_small_s5(uint32_t x){
* 	uint32_t r;
*	r =  SHR32(x, 2)
*		 ^ x;
*	return r;
* }
*/
.global bmw_small_s5
bmw_small_s5:
	movw areg0, preg0
	movw areg2, preg2
	ldi r20, 2
	rcall shiftr32
	rjmp outro_2

/*******************************************************************************
* uint32_t bmw_small_r1(uint32_t x){
* 	uint32_t r;
*	r =  ROTL32(x, 3);
*	return r;
* }
*/
.global bmw_small_r1
bmw_small_r1:
	ldi r20, 3
	rjmp rotl32

/*******************************************************************************
* uint32_t bmw_small_r2(uint32_t x){
* 	uint32_t r;
*	r =  ROTL32(x, 7);
*	return r;
* }
*/
.global bmw_small_r2
bmw_small_r2:
	ldi r20, 7
	rjmp rotl32

/*******************************************************************************
* uint32_t bmw_small_r3(uint32_t x){
* 	uint32_t r;
*	r =  ROTL32(x, 13);
*	return r;
* }
*/
.global bmw_small_r3
bmw_small_r3:
	movw r18, r24
	movw r24, r22
	movw r22, r18
	ldi r20, 3
	rjmp rotr32


/*******************************************************************************
* uint32_t bmw_small_r4(uint32_t x){
* 	uint32_t r;
*	r =  ROTL32(x, 16);
*	return r;
* }
*/
.global bmw_small_r4
bmw_small_r4:
	movw r18, r24
	movw r24, r22
	movw r22, r18
	ret

/*******************************************************************************
* uint32_t bmw_small_r5(uint32_t x){
* 	uint32_t r;
*	r =  ROTR32(x, 13);
*	return r;
* }
*/
.global bmw_small_r5
bmw_small_r5:
	movw r18, r24
	movw r24, r22
	movw r22, r18
	ldi r20, 3
	rjmp rotl32

/*******************************************************************************
* uint32_t bmw_small_r6(uint32_t x){
* 	uint32_t r;
*	r =  ROTR32(x, 9);
*	return r;
* }
*/
.global bmw_small_r6
bmw_small_r6:
	mov r18, r22
	mov r22, r23
	mov r23, r24
	mov r24, r25
	mov r25, r18
	ldi r20, 1
	rjmp rotr32

/*******************************************************************************
* uint32_t bmw_small_r7(uint32_t x){
* 	uint32_t r;
*	r =  ROTR32(x, 5);
*	return r;
* }
*/
.global bmw_small_r7
bmw_small_r7:
	ldi r20, 5
	rjmp rotr32

/******************************************************************************/

const_lut:
	.long 0x55555550, 0x5aaaaaa5, 0x5ffffffa, 0x6555554f
	.long 0x6aaaaaa4, 0x6ffffff9, 0x7555554e, 0x7aaaaaa3
	.long 0x7ffffff8, 0x8555554d, 0x8aaaaaa2, 0x8ffffff7
	.long 0x9555554c, 0x9aaaaaa1, 0x9ffffff6, 0xa555554b

/*******************************************************************************
* uint32_t addelment(uint8_t j, const uint32_t* m, const uint32_t* h){
* 	uint32_t r;
*	r  = pgm_read_dword(k_lut+j);
*	r += rotl_addel(((uint32_t*)m)[j&0xf], j+0);
*	r += rotl_addel(((uint32_t*)m)[(j+3)&0xf], j+3);
*	r -= rotl_addel(((uint32_t*)m)[(j+10)&0xf], j+10);
*	r ^= ((uint32_t*)h)[(j+7)&0xf];
*	return r;
* }
* param j: r24
* param m: r22:r23
* param h: r20:r21
*/
j    = 16
acc2 =  8
acc3 =  9
h0   = 10
h1   = 11
m0   = 12
m1   = 13
acc0 = 14
acc1 = 15
.global addelement
addelement:
	push_range 8, 16
	mov j, r24
	movw h0, r20
	movw m0, r22
	lsl r24
	lsl r24
	ldi r30, lo8(const_lut)
	ldi r31, hi8(const_lut)
	add r30, r24
	adc r31, r1
	lpm acc0, Z+
	lpm acc1, Z+
	lpm acc2, Z+
	lpm acc3, Z+

	mov r20, j
	andi r20, 0x0f
	lsl r20
	lsl r20
	movw r26, m0
	add r26, r20
	adc r27, r1
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	mov r20, j
	rcall rotl_addel
	add acc0, r22
	adc acc1, r23
	adc acc2, r24
	adc acc3, r25

	subi j, -3
	mov r20, j
	andi r20, 0x0f
	lsl r20
	lsl r20
	movw r26, m0
	add r26, r20
	adc r27, r1
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	mov r20, j
	rcall rotl_addel
	add acc0, r22
	adc acc1, r23
	adc acc2, r24
	adc acc3, r25

	subi j, -7
	mov r20, j
	andi r20, 0x0f
	lsl r20
	lsl r20
	movw r26, m0
	add r26, r20
	adc r27, r1
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	mov r20, j
	rcall rotl_addel
	sub acc0, r22
	sbc acc1, r23
	sbc acc2, r24
	sbc acc3, r25

	subi j, 3
	mov r20, j
	andi r20, 0x0f
	lsl r20
	lsl r20
	movw r26, h0
	add r26, r20
	adc r27, r1
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	eor r22, acc0
	eor r23, acc1
	eor r24, acc2
	eor r25, acc3
	pop_range 8, 16
	ret

/*******************************************************************************
* uint32_t bmw_small_expand1(uint8_t j, const void* m, const void* h, const uint32_t* q){
*	uint32_t(*s[])(uint32_t) = {bmw_small_s1, bmw_small_s2, bmw_small_s3, bmw_small_s0};
*	uint32_t r;
*	uint8_t i;
*	r = addelement(j, m, h);
*	i=15;
*	do{
*		r += s[i%4](q[j+i]);
*	}while(i--!=0);
*	return r;
*
* param j: r24
* param m: r22:r23
* param h: r20:r21
* param q: r18:r19
*/
acc0 =  2
acc1 =  3
acc2 =  4
acc3 =  5
.global bmw_small_expand1
bmw_small_expand1:
	push_range 28, 29
	movw r28, r18
	mov r18, r24
	lsl r18
	lsl r18
	add r28, r18
	adc r29, r1
	rcall addelement
	push_range 2, 5
	push r16
	ldi r16, 4
	movw acc0, r22
	movw acc2, r24
1:
	ld r22, Y+
	ld r23, Y+
	ld r24, Y+
	ld r25, Y+
	rcall bmw_small_s1
	add acc0, r22
	adc acc1, r23
	adc acc2, r24
	adc acc3, r25
	ld r22, Y+
	ld r23, Y+
	ld r24, Y+
	ld r25, Y+
	rcall bmw_small_s2
	add acc0, r22
	adc acc1, r23
	adc acc2, r24
	adc acc3, r25
	ld r22, Y+
	ld r23, Y+
	ld r24, Y+
	ld r25, Y+
	rcall bmw_small_s3
	add acc0, r22
	adc acc1, r23
	adc acc2, r24
	adc acc3, r25
	ld r22, Y+
	ld r23, Y+
	ld r24, Y+
	ld r25, Y+
	rcall bmw_small_s0
	add acc0, r22
	adc acc1, r23
	adc acc2, r24
	adc acc3, r25
	dec r16
	brne 1b
expand1_exit:
	movw r22, acc0
	movw r24, acc2
	pop r16
	pop_range 2, 5
	pop_range 28, 29
	ret

/*******************************************************************************
* uint32_t bmw_small_expand2(uint8_t j, const void* m, const void* h, const uint32_t* q){
*	uint32_t(*rf[])(uint32_t) = {bmw_small_r1, bmw_small_r2, bmw_small_r3,
*	                             bmw_small_r4, bmw_small_r5, bmw_small_r6,
*							     bmw_small_r7};
*	uint32_t r;
*	uint8_t i;
*	r = addelement(j, m, h);
*	for(i=0; i<14; i+=2){
*		r += q[j+i];
*	}
*	for(i=0; i<14; i+=2){
*		r += rf[i/2](q[j+i+1]);
*	}
*	r += bmw_small_s4(q[j+14]);
*	r += bmw_small_s5(q[j+15]);
*	return r;
* }
*/
expand2_jumptable:
	ret
	rjmp bmw_small_r1
	ret
	rjmp bmw_small_r2
	ret
	rjmp bmw_small_r3
	ret
	rjmp bmw_small_r4
	ret
	rjmp bmw_small_r5
	ret
	rjmp bmw_small_r6
	ret
	rjmp bmw_small_r7
	rjmp bmw_small_s4
	rjmp bmw_small_s5

.global bmw_small_expand2
bmw_small_expand2:
	push_range 28, 29
	movw r28, r18
	mov r18, r24
	lsl r18
	lsl r18
	add r28, r18
	adc r29, r1
	rcall addelement
	push_range 2, 5
	push r16
	ldi r16, 16
	movw acc0, r22
	movw acc2, r24
	ldi r30, pm_lo8(expand2_jumptable)
	ldi r31, pm_hi8(expand2_jumptable)
1:
	ld r22, Y+
	ld r23, Y+
	ld r24, Y+
	ld r25, Y+
	push r30
	push r31
	icall
	pop r31
	pop r30
	adiw r30, 1
	add acc0, r22
	adc acc1, r23
	adc acc2, r24
	adc acc3, r25
	dec r16
	brne 1b
	rjmp expand1_exit

/*******************************************************************************
* void bmw_small_f1(uint32_t* q, const void* m, const void* h){
*	uint8_t i;
*	q[16] = bmw_small_expand1(0, m, h, q);
*	q[17] = bmw_small_expand1(1, m, h, q);
*	for(i=2; i<16; ++i){
*		q[16+i] = bmw_small_expand2(i, m, h, q);
*	}
* }
*/
m0 =  2
m1 =  3
h0 =  4
h1 =  5
q0 =  6
q1 =  7
.global bmw_small_f1
bmw_small_f1:
;	push_range 2, 7
;	push_range 28, 29
	push r16
	movw q0, r24
	movw m0, r22
	movw h0, r20
	movw r28, q0
	adiw r28, 63
	adiw r28, 1
	clr r24
	clr r25 /* not required */
	movw r18, q0
	rcall bmw_small_expand1
	st Y+, r22
	st Y+, r23
	st Y+, r24
	st Y+, r25
	ldi r16, 1
	mov r24, r16
	clr r25 /* not required */
	movw r22, m0
	movw r20, h0
	movw r18, q0
	rcall bmw_small_expand1
	st Y+, r22
	st Y+, r23
	st Y+, r24
	st Y+, r25
	inc r16
1:
	mov r24, r16
	movw r22, m0
	movw r20, h0
	movw r18, q0
	rcall bmw_small_expand2
	st Y+, r22
	st Y+, r23
	st Y+, r24
	st Y+, r25
	inc r16
	cpi r16, 16
	brne 1b
	pop r16
;	pop_range 28, 29
;	pop_range 2, 7
	ret

/*******************************************************************************
* uint16_t hack_table[5]   PROGMEM = { 0x0311, 0xDDB3, 0x2A79, 0x07AA, 0x51C2 };
* uint8_t  offset_table[5] PROGMEM = { 4+16, 6+16, 9+16, 12+16, 13+16 };
*
* void bmw_small_f0(uint32_t* h, const void* m, uint32_t* q){
*	uint16_t hack_reg;
*	uint8_t c,i,j;
*	uint32_t(*s[])(uint32_t)={ bmw_small_s0, bmw_small_s1, bmw_small_s2,
*	                           bmw_small_s3, bmw_small_s4 };
*	for(i=0; i<16; ++i){
*		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
*	}
*	dump_x(h, 16, 'T');
*	memset(q, 0, 4*16);
*	c=4;
*	do{
*		i=15;
*		j=pgm_read_byte(offset_table+c);
*		hack_reg=pgm_read_word(&(hack_table[c]));
*		do{
*			if(hack_reg&1){
*				q[i]-= h[j&15];
*			}else{
*				q[i]+= h[j&15];
*			}
*			--j;
*			hack_reg>>= 1;
*		}while(i--!=0);
*	}while(c--!=0);
*	dump_x(q, 16, 'W');
*	for(i=0; i<16; ++i){
*		q[i] = s[i%5](q[i]);
*	}
*	for(i=0; i<16; ++i){
*		((uint32_t*)h)[i] ^= ((uint32_t*)m)[i];
*	}
*	for(i=0; i<16; ++i){
*		q[i] += h[(i+1)&0xf];
*	}
* }
*
* param h: r24:r25
* param m: r22:r23
* param q: r20:r21
*/
h0   =  24
h1   =  25
m0   =  22
m1   =  23
q0   =  20
q1   =  21
acc0 =  4
acc1 =  5
acc2 =  6
acc3 =  7
bcc0 =  8
bcc1 =  9
bcc2 = 10
bcc3 = 11
hack = 16

f0_helper:
20:
	ldd acc0, Z+0
	ldd acc1, Z+1
	ldd acc2, Z+2
	ldd acc3, Z+3
	ld bcc0, X+
	ld bcc1, X+
	ld bcc2, X+
	ld bcc3, X+
	lsr r17
	ror r16
	brcs l20_sub
	add acc0, bcc0
	adc acc1, bcc1
	adc acc2, bcc2
	adc acc3, bcc3
	rjmp l20_post
l20_sub:
	sub acc0, bcc0
	sbc acc1, bcc1
	sbc acc2, bcc2
	sbc acc3, bcc3
l20_post:
	st Z+, acc0
	st Z+, acc1
	st Z+, acc2
	st Z+, acc3
	dec r18
	brne 20b
	ret

f0_jumptable:
	rjmp bmw_small_s0
	rjmp bmw_small_s1
	rjmp bmw_small_s2
	rjmp bmw_small_s3
	rjmp bmw_small_s4
	rjmp bmw_small_s0
	rjmp bmw_small_s1
	rjmp bmw_small_s2
	rjmp bmw_small_s3
	rjmp bmw_small_s4
	rjmp bmw_small_s0
	rjmp bmw_small_s1
	rjmp bmw_small_s2
	rjmp bmw_small_s3
	rjmp bmw_small_s4
	rjmp bmw_small_s0

.global bmw_small_f0
bmw_small_f0:
;	push_range 28, 29
;    push_range 4, 11
;    push_range 16, 17
    /* h[i] ^= m[i]; q[i]= 0 */
	movw r26, h0 ; h
	movw r30, m0 ; m
	movw r28, q0 ; q
	ldi r18, 64
1:  ld r0, X
    ld r19, Z+
    eor r0, r19
    st X+, r0
    st Y+, r1
    dec r18
    brne 1b
;------
    ldi r17, 0x88
    ldi r16, 0xC0
    movw r26, h0 ; X = h
    adiw r26, 5*4
    ldi r18, 16-5
    movw r30, q0 ; Z = q
    rcall f0_helper
    movw r26, h0 ; X = h
    ldi r18,    5
    rcall f0_helper
;---
    ldi r17, 0xCD
    ldi r16, 0xBB
    movw r26, h0 ; X = h
    adiw r26, 7*4
    ldi r18, 16-7
    movw r30, q0 ; Z = q
    rcall f0_helper
    movw r26, h0 ; X = h
    ldi r18,    7
    rcall f0_helper
;---
    ldi r17, 0x9E
    ldi r16, 0x54
    movw r26, h0 ; X = h
    adiw r26, 10*4
    ldi r18, 16-10
    movw r30, q0 ; Z = q
    rcall f0_helper
    movw r26, h0 ; X = h
    ldi r18,   10
    rcall f0_helper
;---
    ldi r17, 0x55
    ldi r16, 0xE0
    movw r26, h0 ; X = h
    adiw r26, 13*4
    ldi r18, 16-13
    movw r30, q0 ; Z = q
    rcall f0_helper
    movw r26, h0 ; X = h
    ldi r18,  13
    rcall f0_helper
;---
    ldi r17, 0x43
    ldi r16, 0x8A
    movw r26, h0 ; X = h
    adiw r26, 14*4
    ldi r18, 16-14
    movw r30, q0 ; Z = q
    rcall f0_helper
    movw r26, h0 ; X = h
    ldi r18,  14
    rcall f0_helper
;--------------- h[i] ^= m[i]
	movw r26, h0 ; h
	movw r30, m0 ; m
	ldi r18, 64
25: ld r0, X
    ld r19, Z+
    eor r0, r19
    st X+, r0
    dec r18
    brne 25b
;--------------- q[i] = s[i%5](q[i])
	ldi r16, 16
	ldi r30, pm_lo8(f0_jumptable)
	ldi r31, pm_hi8(f0_jumptable)
    movw bcc0, r30
    movw bcc2, h0 ; h
    movw acc0, q0 ; q
    movw r28,  q0 ; Y = q
30:
	ldd r22, Y+0
	ldd r23, Y+1
	ldd r24, Y+2
	ldd r25, Y+3
	icall
	st Y+, r22
	st Y+, r23
	st Y+, r24
	st Y+, r25
	movw r30, bcc0
	adiw r30, 1
	movw bcc0, r30
	dec r16
	brne 30b
;--------------- q[i] += h[(i+1)%16]
	movw r30, acc0 ; q
	movw r26, bcc2 ; h
	adiw r26, 4
	ldi r18, 15
40:
	ld acc0, Z
	ld acc1, X+
	add acc0, acc1
	st Z+, acc0
	ld acc0, Z
	ld acc1, X+
	adc acc0, acc1
	st Z+, acc0
	ld acc0, Z
	ld acc1, X+
	adc acc0, acc1
	st Z+, acc0
	ld acc0, Z
	ld acc1, X+
	adc acc0, acc1
	st Z+, acc0
	dec r18
	brne 40b
	movw r26, bcc2 ; h
	ld acc0, Z
	ld acc1, X+
	add acc0, acc1
	st Z+, acc0
	ld acc0, Z
	ld acc1, X+
	adc acc0, acc1
	st Z+, acc0
	ld acc0, Z
	ld acc1, X+
	adc acc0, acc1
	st Z+, acc0
	ld acc0, Z
	ld acc1, X+
	adc acc0, acc1
	st Z+, acc0

;   pop_range 16, 17
;   pop_range 4, 11
;	pop_range 28, 29
    ret

/*******************************************************************************
* void bmw_small_f2(uint32_t* h, const uint32_t* q, const void* m){
*	uint32_t xl=0, xh;
*	uint8_t i;
*	for(i=16;i<24;++i){
*		xl ^= q[i];
*	}
*	xh = xl;
*	for(i=24;i<32;++i){
*		xh ^= q[i];
*	}
*	memcpy(h, m, 16*4);
*	h[0] ^= SHL32(xh, 5) ^ SHR32(q[16], 5);
*	h[5] ^= SHL32(xh, 6) ^ SHR32(q[21], 6);
*	h[3] ^= SHR32(xh, 1) ^ SHL32(q[19], 5);
*	h[4] ^= SHR32(xh, 3) ^ q[20];
*	h[6] ^= SHR32(xh, 4) ^ SHL32(q[22], 6);
*	h[2] ^= SHR32(xh, 5) ^ SHL32(q[18], 5);
*	h[1] ^= SHR32(xh, 7) ^ SHL32(q[17], 8);
*	h[7] ^= SHR32(xh,11) ^ SHL32(q[23], 2);
*	for(i=0; i<8; ++i){
*		h[i] += xl ^ q[24+i] ^ q[i];
*	}
*	for(i=0; i<8; ++i){
*		h[8+i] ^= xh ^ q[24+i];
*		h[8+i] += ROTL32(h[(4+i)%8],i+9);
*	}
*	h[11] += SHL32(xl, 4) ^ q[18] ^ q[11];
*	h[10] += SHL32(xl, 6) ^ q[17] ^ q[10];
*	h[ 8] += SHL32(xl, 8) ^ q[23] ^ q[ 8];
*	h[15] += SHR32(xl, 2) ^ q[22] ^ q[15];
*	h[12] += SHR32(xl, 3) ^ q[19] ^ q[12];
*	h[13] += SHR32(xl, 4) ^ q[20] ^ q[13];
*	h[ 9] += SHR32(xl, 6) ^ q[16] ^ q[ 9];
*	h[14] += SHR32(xl, 7) ^ q[21] ^ q[14];
* }
*
* param h: r24:r25
* param q: r22:r23
* param m: r20:r21
*/
xl0 =  2
xl1 =  3
xl2 =  4
xl3 =  5
xh0 =  6
xh1 =  7
xh2 =  8
xh3 =  9
q0  = 10
q1  = 11
h0  = 12
h1  = 13
t0  = 14
t1  = 15
t2  = 16
t3  = 17


.macro modify_h_2 addr:req
	ldd r22, Y+\addr*4+0
	ldd r23, Y+\addr*4+1
	ldd r24, Y+\addr*4+2
	ldd r25, Y+\addr*4+3
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+\addr*4+0
	add r0, r22
	std Z+\addr*4+0, r0
	ldd r0, Z+\addr*4+1
	adc r0, r23
	std Z+\addr*4+1, r0
	ldd r0, Z+\addr*4+2
	adc r0, r24
	std Z+\addr*4+2, r0
	ldd r0, Z+\addr*4+3
	adc r0, r25
	std Z+\addr*4+3, r0
.endm

tshiftr:
	lsr t3
	ror t2
	ror t1
	ror t0
	dec r20
	brne tshiftr
	ret

tshiftl:
	lsl t0
	rol t1
	rol t2
	rol t3
	dec r20
	brne tshiftl
	ret

.global bmw_small_f2
bmw_small_f2:
    /* memcpy(h, m, 64) */
	movw r26, r24
	movw r30, r20
	ldi r18, 64
1:	ld r0, Z+
	st X+, r0
	dec r18
	brne 1b
;	push_range 28, 29
;	push_range  2, 17
	movw q0, r22
	movw h0, r24
	/* calc xl */
/*	for(i=16;i<24;++i){
		xl ^= q[i];
	}
*/
	movw r26, q0
	adiw r26, 63
	adiw r26, 1 ; X points at q[16]
	ld xl0, X+
	ld xl1, X+
	ld xl2, X+
	ld xl3, X+
	ldi r18, 8-1
20: ld r0, X+
	eor xl0, r0
	ld r0, X+
	eor xl1, r0
	ld r0, X+
	eor xl2, r0
	ld r0, X+
	eor xl3, r0
	dec r18
	brne 20b
	/* calc xh */
/*  xh = xl
	for(i=24;i<32;++i){
		xh ^= q[i];
	}
*/
	movw xh0, xl0
	movw xh2, xl2
	ldi r18, 8
25: ld r0, X+
	eor xh0, r0
	ld r0, X+
	eor xh1, r0
	ld r0, X+
	eor xh2, r0
	ld r0, X+
	eor xh3, r0
	dec r18
	brne 25b
/* h[0]..h[7] */
	movw r30, h0
	movw r28, q0
	adiw r28, 60 ; Y points at q[15]
/*	h[0] ^= SHL32(xh, 5) ^ SHR32(q[16], 5); */
	movw t0, xh0
	movw t2, xh2
	ldi r20, 5
	rcall tshiftl
	ldd r22, Y+4
	ldd r23, Y+5
	ldd r24, Y+6
	ldd r25, Y+7
	ldi r20, 5
	rcall shiftr32
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+0
	eor r22, r0
	ldd r0, Z+1
	eor r23, r0
	ldd r0, Z+2
	eor r24, r0
	ldd r0, Z+3
	eor r25, r0
	std Z+0, r22
	std Z+1, r23
	std Z+2, r24
	std Z+3, r25
/*	h[5] ^= SHL32(xh, 6) ^ SHR32(q[21], 6); */
	lsl t0
	rol t1
	rol t2
	rol t3
	ldd r22, Y+24
	ldd r23, Y+25
	ldd r24, Y+26
	ldd r25, Y+27
	ldi r20, 6
	rcall shiftr32
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+20
	eor r22, r0
	ldd r0, Z+21
	eor r23, r0
	ldd r0, Z+22
	eor r24, r0
	ldd r0, Z+23
	eor r25, r0
	std Z+20, r22
	std Z+21, r23
	std Z+22, r24
	std Z+23, r25
/*	h[3] ^= SHR32(xh, 1) ^ SHL32(q[19], 5); */
	movw t0, xh0
	movw t2, xh2
	lsr t3
	ror t2
	ror t1
	ror t0
	ldd r22, Y+16
	ldd r23, Y+17
	ldd r24, Y+18
	ldd r25, Y+19
	ldi r20, 5
	rcall shiftl32
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+12
	eor r22, r0
	ldd r0, Z+13
	eor r23, r0
	ldd r0, Z+14
	eor r24, r0
	ldd r0, Z+15
	eor r25, r0
	std Z+12, r22
	std Z+13, r23
	std Z+14, r24
	std Z+15, r25
/*	h[4] ^= SHR32(xh, 3) ^ q[20]; */
	ldi r20, 2
	rcall tshiftr
	ldd r22, Y+20
	ldd r23, Y+21
	ldd r24, Y+22
	ldd r25, Y+23
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+16
	eor r22, r0
	ldd r0, Z+17
	eor r23, r0
	ldd r0, Z+18
	eor r24, r0
	ldd r0, Z+19
	eor r25, r0
	std Z+16, r22
	std Z+17, r23
	std Z+18, r24
	std Z+19, r25
/*	h[6] ^= SHR32(xh, 4) ^ SHL32(q[22], 6); */
	lsr t3
	ror t2
	ror t1
	ror t0
	ldd r22, Y+28
	ldd r23, Y+29
	ldd r24, Y+30
	ldd r25, Y+31
	ldi r20, 6
	rcall shiftl32
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+24
	eor r22, r0
	ldd r0, Z+25
	eor r23, r0
	ldd r0, Z+26
	eor r24, r0
	ldd r0, Z+27
	eor r25, r0
	std Z+24, r22
	std Z+25, r23
	std Z+26, r24
	std Z+27, r25
/*	h[2] ^= SHR32(xh, 5) ^ SHL32(q[18], 5); */
	lsr t3
	ror t2
	ror t1
	ror t0
	ldd r22, Y+12
	ldd r23, Y+13
	ldd r24, Y+14
	ldd r25, Y+15
	ldi r20, 5
	rcall shiftl32
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+8
	eor r22, r0
	ldd r0, Z+9
	eor r23, r0
	ldd r0, Z+10
	eor r24, r0
	ldd r0, Z+11
	eor r25, r0
	std Z+8 , r22
	std Z+9 , r23
	std Z+10, r24
	std Z+11, r25
/*	h[1] ^= SHR32(xh, 7) ^ SHL32(q[17], 8); */
	ldi r20, 2
	rcall tshiftr
	ldd r23, Y+8
	ldd r24, Y+9
	ldd r25, Y+10
	mov r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+4
	eor r22, r0
	ldd r0, Z+5
	eor r23, r0
	ldd r0, Z+6
	eor r24, r0
	ldd r0, Z+7
	eor r25, r0
	std Z+4 , r22
	std Z+5 , r23
	std Z+6 , r24
	std Z+7 , r25
/*	h[7] ^= SHR32(xh,11) ^ SHL32(q[23], 2); */
	ldi r20, 4
	rcall tshiftr
	ldd r22, Y+32
	ldd r23, Y+33
	ldd r24, Y+34
	ldd r25, Y+35
	ldi r20, 2
	rcall shiftl32
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ldd r0, Z+28
	eor r22, r0
	ldd r0, Z+29
	eor r23, r0
	ldd r0, Z+30
	eor r24, r0
	ldd r0, Z+31
	eor r25, r0
	std Z+28, r22
	std Z+29, r23
	std Z+30, r24
	std Z+31, r25
/*	for(i=0; i<8; ++i){
*		h[i] += xl ^ q[24+i] ^ q[i];
*	}
*/
	movw r26, q0
	movw r28, q0
	adiw r28, 63
	adiw r28, 24*4-63
	ldi r18, 8
10:
	movw t0, xl0
	movw t2, xl2
	ld r0, X+
	eor t0, r0
	ld r0, X+
	eor t1, r0
	ld r0, X+
	eor t2, r0
	ld r0, X+
	eor t3, r0
	ld r0, Y+
	eor t0, r0
	ld r0, Y+
	eor t1, r0
	ld r0, Y+
	eor t2, r0
	ld r0, Y+
	eor t3, r0
	ldd r22, Z+0
	ldd r23, Z+1
	ldd r24, Z+2
	ldd r25, Z+3
	add r22, t0
	adc r23, t1
	adc r24, t2
	adc r25, t3
	st Z+, r22
	st Z+, r23
	st Z+, r24
	st Z+, r25
	dec r18
	brne 10b
	; Z points to h[8]
/*	for(i=0; i<8; ++i){
		h[8+i] ^= xh ^ q[24+i];
		h[8+i] += ROTL32(h[(4+i)%8],i+9);
	}
*/
	; Z points at h[8]
;	clr r18
	sbiw r28, 8*4 ; Y points at q[24]
	movw r26, r30
	sbiw r26, 4*4 ; X points at h[4]
15:
	ldd t0, Z+0
	ldd t1, Z+1
	ldd t2, Z+2
	ldd t3, Z+3
	eor t0, xh0
	eor t1, xh1
	eor t2, xh2
	eor t3, xh3
	ld r0, Y+
	eor t0, r0
	ld r0, Y+
	eor t1, r0
	ld r0, Y+
	eor t2, r0
	ld r0, Y+
	eor t3, r0
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	mov r20, r18
	rcall rotl32p9
	add t0, r22
	adc t1, r23
	adc t2, r24
	adc t3, r25
	st Z+, t0
	st Z+, t1
	st Z+, t2
	st Z+, t3
	inc r18
	cpi r18, 4
	brne 16f
	movw r26, h0
16:
	sbrs r18, 3
	rjmp 15b
	sbiw r30, 4*8 ; adjust Z to point at h[8]
	sbiw r28, 16*4-1
	sbiw r28, 1   ; adjust Y to point at q[16]
	movw r26, r28
	sbiw r26, 7*4 ; adjust X to point at q[9]
	ldi r18, 7*4
20: /* now we do the memxor stuff */
	ld t0, X
	ld t1, Y+
	eor t0, t1
	st X+, t0
	dec r18
	brne 20b
	; X points at q[16]
	; Y points at q[23]
	sbiw r26, 4*8 ; X points at q[8]

	clr t0
	mov t1, xl0
	mov t2, xl1
	mov t3, xl2
/*	h[ 8] += SHL32(xl, 8) ^ q[23] ^ q[ 8]; */
	ld r22, X+
	ld r23, X+
	ld r24, X+
	ld r25, X+
	ld r0, Y+
	eor r22, r0
	ld r0, Y+
	eor r23, r0
	ld r0, Y+
	eor r24, r0
	ld r0, Y+
	eor r25, r0
	eor r22, t0
	eor r23, t1
	eor r24, t2
	eor r25, t3
	ld r0, Z
	add r0, r22
	st Z+, r0
	ld r0, Z
	adc r0, r23
	st Z+, r0
	ld r0, Z
	adc r0, r24
	st Z+, r0
	ld r0, Z
	adc r0, r25
	st Z+, r0
	movw r28, r26
	; Z points at h[9]
	; X points at q[9] but we won't need it anymore
	; Y points at q[9]
/*	h[11] += SHL32(xl, 4) ^ q[11]; */
	movw t0, xl0
	movw t2, xl2
	ldi r20, 4
	rcall tshiftl
	modify_h_2 2
/*	h[10] += SHL32(xl, 6) ^ q[10]; */
	ldi r20, 2
	rcall tshiftl
	modify_h_2 1
/*	h[15] += SHR32(xl, 2) ^ q[15]; */
	movw t0, xl0
	movw t2, xl2
	ldi r20, 2
	rcall tshiftr
	modify_h_2 6
/*	h[12] += SHR32(xl, 3) ^ q[12]; */
	ldi r20, 1
	rcall tshiftr
	modify_h_2 3
/*	h[13] += SHR32(xl, 4) ^ q[13]; */
	ldi r20, 1
	rcall tshiftr
	modify_h_2 4
/*	h[ 9] += SHR32(xl, 6) ^ q[ 9]; */
	ldi r20, 2
	rcall tshiftr
	modify_h_2 0
/*	h[14] += SHR32(xl, 7) ^ q[14]; */
	ldi r20, 1
	rcall tshiftr
	modify_h_2 5
bmw_small_f2_exit:
;	pop_range  2, 17
;	pop_range 28, 29
	ret

#if DEBUG_FUNCTIONS

cli_putb:
	push r2
	push_range 18, 26
	push_range 30, 31
	mov r2, r24
	swap r24
	andi r24, 0xf
	ldi r30, lo8(hextable)
	ldi r31, hi8(hextable)
	add r30, r24
	adc r31, r1
	lpm r24, Z
	clr r25
	call cli_putc
	mov r24, r2
	andi r24, 0xf
	ldi r30, lo8(hextable)
	ldi r31, hi8(hextable)
	add r30, r24
	adc r31, r1
	lpm r24, Z
	clr r25
	call cli_putc
	pop_range 30, 31
	pop_range 18, 26
	pop r2
	ret
hextable:
	.byte '0', '1', '2', '3', '4', '5', '6', '7'
	.byte '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'

cli_putchar:
	push_range 18, 31
	call cli_putc
	pop_range 18, 31
	ret

#endif

/*******************************************************************************
* void bmw_small_nextBlock(bmw_small_ctx_t* ctx, const void* block){
*	uint32_t q[32];
*	dump_x(block, 16, 'M');
*	bmw_small_f0(ctx->h, block, q);
*	dump_x(q, 16, 'Q');
*	bmw_small_f1(q, block, ctx->h);
*	dump_x(q, 32, 'Q');
*	bmw_small_f2(ctx->h, q, block);
*	ctx->counter += 1;
*	ctx_dump(ctx);
* }
*
* param ctx:   r24:r25
* param block: r22:r23
*/
h0 =  2
h1 =  3
b0 =  4
b1 =  5
q0 =  6
q1 =  7
.global bmw_small_nextBlock
.global bmw224_nextBlock
.global bmw256_nextBlock
bmw_small_nextBlock:
bmw224_nextBlock:
bmw256_nextBlock:
	push_range 28, 29
	push_range  2, 17
	stack_alloc_large 32*4, 30, 31
	adiw r30, 1
	movw q0, r30
	movw h0, r24
	movw b0, r22
	/* increment counter */
	movw r30, r24
	adiw r30, 60
	ldd r22, Z+4
	ldd r23, Z+5
	ldd r24, Z+6
	ldd r25, Z+7
	ldi r21, 1
	add r22, r21
	adc r23, r1
	adc r24, r1
	adc r25, r1
	std Z+4, r22
	std Z+5, r23
	std Z+6, r24
	std Z+7, r25
	/* call bmw_small_f0(ctx->h, block, q) */
	movw r24, h0
	movw r22, b0
	movw r20, q0
	push_ q1, q0, b1, b0, h1, h0
	rcall bmw_small_f0
	/*	call bmw_small_f1(q, block, ctx->h) */
	pop_ 20, 21, 22, 23, 24, 25,
	push_ 21, 20, 25, 24, 23, 22
	rcall bmw_small_f1
	/*	call bmw_small_f2(ctx->h, q, block) */
	pop_ 20, 21, 22, 23, 24, 25,
	rcall bmw_small_f2
	stack_free_large3 32*4
	pop_range  2, 17
	pop_range 28, 29
	ret


/*******************************************************************************
* void bmw224_init(bmw224_ctx_t* ctx){
*	uint8_t i;
*	ctx->h[0] = 0x00010203;
*	for(i=1; i<16; ++i){
*		ctx->h[i] = ctx->h[i-1]+ 0x04040404;
*	}
*	ctx->counter=0;
* }
*
* param ctx:  r24:r25
*/
.global bmw224_init
bmw224_init:
	movw r26, r24
	ldi r22, 0x03
	ldi r23, 0x02
	ldi r24, 0x01
	ldi r25, 0x00
bmw_small_init:
	st X+, r22
	st X+, r23
	st X+, r24
	st X+, r25
	ldi r18, 16-1
	ldi r20, 0x04
1:
	add r22, r20
	adc r23, r20
	adc r24, r20
	adc r25, r20
	st X+, r22
	st X+, r23
	st X+, r24
	st X+, r25
	dec r18
	brne 1b
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	ret

.global bmw256_init
bmw256_init:
	movw r26, r24
	ldi r22, 0x43
	ldi r23, 0x42
	ldi r24, 0x41
	ldi r25, 0x40
	rjmp bmw_small_init

/*******************************************************************************
* void bmw_small_lastBlock(bmw_small_ctx_t* ctx, const void* block, uint16_t length_b){
*	struct {
*		uint8_t  buffer[64];
*		uint32_t ctr;
*	} pctx;
*	while(length_b >= BMW_SMALL_BLOCKSIZE){
*		bmw_small_nextBlock(ctx, block);
*		length_b -= BMW_SMALL_BLOCKSIZE;
*		block = (uint8_t*)block + BMW_SMALL_BLOCKSIZE_B;
*	}
*	memset(pctx.buffer, 0, 64);
*	memcpy(pctx.buffer, block, (length_b+7)/8);
*	pctx.buffer[length_b>>3] |= 0x80 >> (length_b&0x07);
*	if(length_b+1>64*8-64){
*		bmw_small_nextBlock(ctx, pctx.buffer);
*		memset(pctx.buffer, 0, 64-8);
*		ctx->counter -= 1;
*	}
*	*((uint64_t*)&(pctx.buffer[64-8])) = (uint64_t)(ctx->counter*512LL)+(uint64_t)length_b;
*	bmw_small_nextBlock(ctx, pctx.buffer);
*	uint8_t i;
*	memset(pctx.buffer, 0xaa, 64);
*	for(i=0; i<16;++i){
*		pctx.buffer[i*4] = i+0xa0;
*	}
*	bmw_small_nextBlock((bmw_small_ctx_t*)&pctx, ctx->h);
*	memcpy(ctx->h, pctx.buffer, 64);
* }
*
* param ctx:      r24:r25
* param block:    r22:r23
* param length_b: r20:r21
*/
ctx0 =  2
ctx1 =  3
blc0 =  4
blc1 =  5
len0 = 28
len1 = 29
buf0 =  6
buf1 =  7

.global bmw_small_lastBlock
.global bmw224_lastBlock
.global bmw256_lastBlock
bmw_small_lastBlock:
bmw224_lastBlock:
bmw256_lastBlock:
/*	while(length_b >= BMW_SMALL_BLOCKSIZE){
		bmw_small_nextBlock(ctx, block);
		length_b -= BMW_SMALL_BLOCKSIZE;
		block = (uint8_t*)block + BMW_SMALL_BLOCKSIZE_B;
	}
*/
	push_range 2, 7
	push_range 28, 29
	movw ctx0, r24
	movw blc0, r22
	movw len0, r20
1:
	cpi len1, hi8(512)
	brlo 2f
	movw r24, ctx0
	movw r22, blc0
	rcall bmw_small_nextBlock
	ldi r24, 64
	add blc0, r24
	adc blc1, r1
	subi len1, hi8(512)
	rjmp 1b
2:
/*	struct {
		uint8_t  buffer[64];
		uint32_t ctr;
	} pctx;
*/
	stack_alloc_large 68
	adiw r30, 1
	movw buf0, r30
/*	memset(pctx.buffer, 0, 64);
	memcpy(pctx.buffer, block, (length_b+7)/8);
	pctx.buffer[length_b>>3] |= 0x80 >> (length_b&0x07);
*/	movw r24, len0
	lsr r25
	ror r24
	lsr r24
	lsr r24
;	inc r24
	ldi r23, 63
	sub r23, r24
	movw r26, blc0
	tst r24
	breq 301f
30: ld r20, X+
	st Z+, r20
	dec r24
	brne 30b
301:
	clr r20
	mov r21, len0
	ldi r24, 0x80
	andi r21, 0x07
	breq 305f
	ld r20, X+
303:
	lsr r24
	dec r21
	brne 303b
305:
	or r20, r24
	st Z+, r20
	tst r23
	breq 32f
31: st Z+, r1
	dec r23
	brne 31b
32:
/*	if(length_b+1>64*8-64){ ; = 64*7-1 = 447 max(length_b)=511
		bmw_small_nextBlock(ctx, pctx.buffer);
		memset(pctx.buffer, 0, 64-8);
		ctx->counter -= 1;
	}
*/
	tst len1
	breq 400f
	cpi len0, 192
	brlo 400f
	movw r24, ctx0
	movw r22, buf0
	rcall bmw_small_nextBlock
	movw r26, buf0
	ldi r20, 64-8
350:
	st X+, r1
	dec r20
	brne 350b
	movw r30, ctx0
	adiw r30, 60
	ldd r21, Z+4
	ldd r22, Z+5
	ldd r23, Z+6
	ldd r24, Z+7
	subi r21, 1
	sbc r22, r1
	sbc r23, r1
	sbc r24, r1
	rjmp 410f
/*	*((uint64_t*)&(pctx.buffer[64-8])) = (uint64_t)(ctx->counter*512LL)+(uint64_t)length_b;
	bmw_small_nextBlock(ctx, pctx.buffer);
*/
400:
	movw r30, ctx0
	adiw r30, 60
	ldd r21, Z+4
	ldd r22, Z+5
	ldd r23, Z+6
	ldd r24, Z+7
410:
	clr r25
	lsl r21
	rol r22
	rol r23
	rol r24
	rol r25
	mov r20, len0
	add r21, len1
	adc r22, r1
	adc r23, r1
	adc r24, r1
	adc r25, r1
	movw r30, buf0
	adiw r30, 64-8
	st Z+, r20
	st Z+, r21
	st Z+, r22
	st Z+, r23
	st Z+, r24
	st Z+, r25
	st Z+, r1
	st Z+, r1
	movw r24, ctx0
	movw r22, buf0
	rcall bmw_small_nextBlock
/*	memset(pctx.buffer, 0xaa, 64);
	for(i=0; i<16;++i){
		pctx.buffer[i*4] = i+0xa0;
	}
*/
	ldi r18, 0xa0
	ldi r19, 0xaa
	movw r26, buf0
500:
	st X+, r18
	st X+, r19
	st X+, r19
	st X+, r19
	inc r18
	sbrs r18, 4
	rjmp 500b
/*	bmw_small_nextBlock((bmw_small_ctx_t*)&pctx, ctx->h);
	memcpy(ctx->h, pctx.buffer, 64);
*/
	movw r24, buf0
	movw r22, ctx0
	rcall bmw_small_nextBlock
	ldi r18, 64
	movw r26, ctx0
	movw r30, buf0
600:
	ld r20, Z+
	st X+, r20
	dec r18
	brne 600b

	stack_free_large 68
	pop_range 28, 29
	pop_range 2, 7
	ret

/*******************************************************************************
* void bmw224_ctx2hash(void* dest, const bmw224_ctx_t* ctx){
*	memcpy(dest, &(ctx->h[9]), 224/8);
* }
*
* param dest:  r24:r25
* param ctx:   r22:r23
*/
.global bmw224_ctx2hash
bmw224_ctx2hash:
	movw r26, r24
	movw r30, r22
	adiw r30, 9*4
	ldi r22, 28
	rjmp 1f

/*******************************************************************************
* void bmw256_ctx2hash(void* dest, const bmw256_ctx_t* ctx){
*	memcpy(dest, &(ctx->h[8]), 256/8);
* }
*
* param dest:  r24:r25
* param ctx:   r22:r23
*/
.global bmw256_ctx2hash
bmw256_ctx2hash:
	movw r26, r24
	movw r30, r22
	adiw r30, 8*4
	ldi r22, 32
1:
	ld r23, Z+
	st X+, r23
	dec r22
	brne 1b
	ret

/*******************************************************************************
* void bmw256(void* dest, const void* msg, uint32_t length_b){
*	bmw_small_ctx_t ctx;
*	bmw256_init(&ctx);
*	while(length_b>=BMW_SMALL_BLOCKSIZE){
*		bmw_small_nextBlock(&ctx, msg);
*		length_b -= BMW_SMALL_BLOCKSIZE;
*		msg = (uint8_t*)msg + BMW_SMALL_BLOCKSIZE_B;
*	}
*	bmw_small_lastBlock(&ctx, msg, length_b);
*	bmw256_ctx2hash(dest, &ctx);
* }
*
* param dest:     r24:r25
* param msg:      r22:r23
* param length_b: r18:r21
*/
ctx0 =   2
ctx1 =   3
msg0 =   4
msg1 =   5
len0 =   6
len1 =   7
len2 =   8
len3 =   9
dst0 =  10
dst1 =  11
.global bmw256
bmw256:
	push r16
	ldi r16, 1
	rjmp bmw_small_all

/*******************************************************************************
* void bmw224(void* dest, const void* msg, uint32_t length_b){
*	bmw_small_ctx_t ctx;
*	bmw224_init(&ctx);
*	while(length_b>=BMW_SMALL_BLOCKSIZE){
*		bmw_small_nextBlock(&ctx, msg);
*		length_b -= BMW_SMALL_BLOCKSIZE;
*		msg = (uint8_t*)msg + BMW_SMALL_BLOCKSIZE_B;
*	}
*	bmw_small_lastBlock(&ctx, msg, length_b);
*	bmw224_ctx2hash(dest, &ctx);
* }
*
* param dest:     r24:r25
* param msg:      r22:r23
* param length_b: r18:r21
*/
ctx0 =   2
ctx1 =   3
msg0 =   4
msg1 =   5
len0 =   6
len1 =   7
len2 =   8
len3 =   9
dst0 =  10
dst1 =  11
.global bmw224
bmw224:
	push r16
	clr r16

bmw_small_all:
	push_range 2, 11
	stack_alloc_large 64+4
	adiw r30, 1
	movw ctx0, r30
	movw dst0, r24
	movw msg0, r22
	movw len0, r18
	movw len2, r20
	movw r24, ctx0
	ldi r30, pm_lo8(init_lut)
	ldi r31, pm_hi8(init_lut)
	add r30, r16
	adc r31, r1
	icall
20:
	mov r18, len2
	or  r18, len3
	breq 50f
	movw r24, ctx0
	movw r22, msg0
	rcall bmw_small_nextBlock
	ldi r20, 2
	sub len1, r20
	sbc len2, r1
	sbc len3, r1
	ldi r20, 64
	add msg0, r20
	adc msg1, r1
	rjmp 20b
50:
	movw r24, ctx0
	movw r22, msg0
	movw r20, len0
	rcall bmw_small_lastBlock
	movw r24, dst0
	movw r22, ctx0
	ldi r30, pm_lo8(c2h_lut)
	ldi r31, pm_hi8(c2h_lut)
	add r30, r16
	adc r31, r1
	icall
	stack_free_large 64+4
	pop_range 2, 11
	pop r16
	ret

init_lut:
	rjmp bmw224_init
	rjmp bmw256_init
c2h_lut:
	rjmp bmw224_ctx2hash
	rjmp bmw256_ctx2hash
