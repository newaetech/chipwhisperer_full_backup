/* mqq160-sign_P-asm.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2010 Daniel Otte (daniel.otte@rub.de)

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
 * \file     mqq160-sign_P-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2010-03-21
 * \license  GPLv3 or later
 *
 */

#include "avr-asm-macros.S"

#if 0
static void mqq_inv_affine_transformation(uint8_t* input_bytes, uint8_t* result, const mqq160_sign_key_t* key){
	/* The matrix SInv is given as two permutations of 160 elements. */
	uint8_t j, byteindex, bitindex, bitindex_d, byteindex_d, rp1, rp5;
	uint8_t *r1_ptr, *r5_ptr;
	uint8_t h1[20];

	/* Initialize H1 and H2 = 0 */
	memset(h1, 0, 20);
	memset(result, 0, 20);

	/*
	   Fill H1 with bits of InputBytes accordingly to RP1 permutation
	   and fill H2 with bits of InputBytes accordingly to RP5 permutation
	*/
	bitindex_d = 0x80;
	byteindex_d = 0;
	j=160;
	r1_ptr = key->rp1;
	r5_ptr = key->rp5;
	do{
		rp1 = pgm_read_byte(r1_ptr++);
		rp5 = pgm_read_byte(r5_ptr++);
		byteindex = rp1>>3;
		bitindex = 0x80 >> (rp1&0x07);
		if (input_bytes[byteindex] & bitindex){
			h1[byteindex_d] ^= bitindex_d;
		}

		byteindex = rp5>>3;
		bitindex = 0x80 >> (rp5&0x07);
		if (input_bytes[byteindex] & bitindex){
			result[byteindex_d] ^= bitindex_d;
		}
		bitindex_d >>= 1;
		if(bitindex_d==0){
			++byteindex_d;
			bitindex_d = 0x80;
		}
	}while(--j);

	for (j=0; j<20; j++){
		result[j] ^= h1[j] ^ h1[pgm_read_byte(j+mod20_table)]
		                   ^ h1[pgm_read_byte(8+j+mod20_table)]
		                   ^ h1[pgm_read_byte(12+j+mod20_table)];
	}
}
#endif

fetch_bit:
	ld r0, Z+
	mov r28, r0
	ldi r29, 0x80
	andi r28, 7
	breq 3f
2:	lsr r29
	dec r28
	brne 2b
3:	mov r28, r0
	lsr r28
	lsr r28
	lsr r28
	mov r0, r29
	clr r29
	add r28, r24
	adc r29, r25
	ld r28, Y
	clt
	and r28, r0
	breq 4f
	set
4:  ret

xres_0 = 18
xres_1 = 19
h_0 = 20
h_1 = 21
xrp5_0 = 22
xrp5_1 = 23
inp_0 =  24
inp_1 =  25
tmp_0 =  22
tmp_1 =  23
tmp_2 =  24
tmp_3 =  25
tmp_4 =  18

/*
 param input_bytes: r24:r25
 param result:      r22:r23
 param key:         r20:r21
*/
;.global mqq_inv_affine_transformation
mqq_inv_affine_transformation:
	push r17
;	push r28
;	push r29
	stack_alloc 20
	adiw r30, 1   /* Z points to stack space for h1 */
	movw r28, r20 /* Y points to the key struct in RAM */
	movw xres_0, r22
	movw r26, r30 /* X points to h1[0] */
	ldd xrp5_0, Y+8 /* load pointer rp5 to xrp5 */
	ldd xrp5_1, Y+9
	movw h_0, r30
	ldd r30, Y+6 /* load pointer to rp1 in Z */
	ldd r31, Y+7
	ldi r17, 20
20:	rcall fetch_bit
	bld r1, 7
	rcall fetch_bit
	bld r1, 6
	rcall fetch_bit
	bld r1, 5
	rcall fetch_bit
	bld r1, 4
	rcall fetch_bit
	bld r1, 3
	rcall fetch_bit
	bld r1, 2
	rcall fetch_bit
	bld r1, 1
	rcall fetch_bit
	bld r1, 0
	st X+, r1
	dec r17
	brne 20b
;----
	movw r26, xres_0 /* X points to result */
	movw r30, xrp5_0
	ldi r17, 20
20:	rcall fetch_bit
	bld r1, 7
	rcall fetch_bit
	bld r1, 6
	rcall fetch_bit
	bld r1, 5
	rcall fetch_bit
	bld r1, 4
	rcall fetch_bit
	bld r1, 3
	rcall fetch_bit
	bld r1, 2
	rcall fetch_bit
	bld r1, 1
	rcall fetch_bit
	bld r1, 0
	st X+, r1
	dec r17
	brne 20b
	clr r1
; --- now we mix result with h1
	sbiw r26, 20 /* adjusting X to point at result[0] */
	movw tmp_2, h_0
	ldi r30, lo8(affine_mix_lut)
	ldi r31, hi8(affine_mix_lut)
	ldi r17, 20
30:
	ld tmp_0, X
	movw r28, tmp_2
	ld tmp_1, Y+
	movw tmp_2, r28
	eor tmp_0, tmp_1
	movw r28, h_0
	lpm r0, Z+
	mov tmp_4, r0
	andi tmp_4, 0x0f
	add r28, tmp_4
	adc r29, r1
	ld tmp_1, Y
	eor tmp_0, tmp_1
	adiw r28, 4
	sbrc r0, 7
	adiw r28, 4
	ld tmp_1, Y
	eor tmp_0, tmp_1
	adiw r28, 4
	sbrc r0, 6
	adiw r28, 4
	ld tmp_1, Y
	eor tmp_0, tmp_1
	st X+, tmp_0
	dec r17
	brne 30b

	stack_free 20
;	pop r29
;	pop r28
	pop r17
	ret

affine_mix_lut:
	.byte 0x84, 0x85, 0x86, 0x87
	.byte 0xC0, 0xC1, 0xC2, 0xC3
	.byte 0x40, 0x41, 0x42, 0x43
	.byte 0x44, 0x45, 0x46, 0x47
	.byte 0x80, 0x81, 0x82, 0x83

/******************************************************************************/

xres  = 20
tmp_0 = 23
tmp_1 = 22
tmp_2 = 21
tmp_3 = 19
/*
  param i:    r24
  param b1:   r22
  param b2:   r20
  param key:  r18:r19
*/
;.global mqq_q
mqq_q:
;	push r28
;	push r29
;	stack_alloc 25, r26, r27
;	adiw r26, 1  /* X points to e[0] */
	movw r28, r18
	sbrs r24, 0
	adiw r28, 2
	ldd r30, Y+2
	ldd r31, Y+3
	ldi r28, 9
10:	ld r0, Z+
	st X+, r0
	dec r28
	brne 10b
	sbiw r26, 9 /* adjust X to point at e[0] */
;---
	movw r28, r18
	ld r30, Y+ /* Z points to a[0] in progmem */
	ld r31, Y
	sbrs r24, 0
	rjmp 40f
20:
	sbrs r22, 7
	rjmp 30f
	ldi r25, 9
	movw r28, r30
25:	ld r0, Z
    adiw r30, 9
    ld r24, X
	eor r24, r0
	st X+, r24
	dec r25
	brne 25b
	movw r30, r28
	sbiw r26, 9
30:
	adiw r30, 1
	lsl r22
	breq 60f
	rjmp 20b
40:
	sbrs r22, 7
	rjmp 50f
	ldi r25, 9
	movw r28, r30
45:	ld r0, Z+
    ld r24, X
	eor r24, r0
	st X+, r24
	dec r25
	brne 45b
	movw r30, r28
	sbiw r26, 9
50:
	adiw r30, 9
	lsl r22
	breq 60f
	rjmp 40b
60:
;------ all inputs are consumed, X points at e[0]
;------ So we finished with obtaining e0 .. e7 and e8
	movw r28, r26
	ldd r0, Y+8
	eor xres, r0
;---

/*
   We can look at the bits of e0 .. e7 as a columns of a given matrix. We want to define 8 variables that have the rows
   of that matrix. The variables need to be 16-bit because we will put into the upper 8 bits the bits of e0 .. e7,
   and the bits of the variable result will be the Least Significant Bits of a[0] ... a[7].
*/
	adiw r28, 9 /* Y points at a[0] */
	ldi r25, 8
63:
	ldi r24, 8
	clr tmp_0
65:	ld tmp_1, X
	lsl tmp_1
	st X+, tmp_1
	rol tmp_0
	dec r24
	brne 65b
;---
	clr tmp_1
	lsl xres
	rol tmp_1
	st Y+, tmp_1
	st Y+, tmp_0
	sbiw r26, 8
	dec r25
	brne 63b
;------- First we apply upper triangular transformation
	sbiw r28, 16  /* Y points at a[0] */
	movw r30, r28 /* Z points at a[0] */

col = 25
	ldi r24, 8
	clr col
70:
	mov r1, col
	ldi tmp_3, 0x80
	tst r1
	breq 72f
71:	lsr tmp_3
	dec r1
	brne 71b
72:
	clt
	movw r28, r30 /* Y points at a[row]*/
73:	ldd tmp_0, Y+1
	and tmp_0, tmp_3
	brne 74f
	set
	adiw r28, 2
	rjmp 73b
74:
    /* Y points at a[row] */
	/* if T is set we have to permute [Y] and [Z] */
	brtc 75f
	ld tmp_0, Y
	ld tmp_1, Z
	st Y, tmp_1
	st Z, tmp_0
	ldd tmp_0, Y+1
	ldd tmp_1, Z+1
	std Y+1, tmp_1
	std Z+1, tmp_0
75: /* permutation done */
	ldi r26, 7
	sub r26, col
	breq 78f
	movw r28, r30
76:	adiw r28, 2
	ldd tmp_0, Y+1
	and tmp_0, tmp_3
	breq 77f
	ld tmp_0, Y
	ld tmp_1, Z
	eor tmp_0, tmp_1
	st Y, tmp_0
	ldd tmp_0, Y+1
	ldd tmp_1, Z+1
	eor tmp_0, tmp_1
	std Y+1, tmp_0
77:
	dec r26
	brne 76b
78:
	adiw r30, 2
	inc col
	dec r24
	brne 70b
79:
;------ Then we eliminate 1s above the main diagonal

	ldi col, 7
	ldi tmp_3, 1
	sbiw r30, 2
80:
	movw r28, r30
	mov r26, col
81:
	sbiw r28, 2
	ldd tmp_0, Y+1
	and tmp_0, tmp_3
	breq 82f
	ld tmp_0, Y
	ld tmp_1, Z
	eor tmp_0, tmp_1
	st Y, tmp_0
	ldd tmp_0, Y+1
	ldd tmp_1, Z+1
	eor tmp_0, tmp_1
	std Y+1, tmp_0
82:
	dec r26
	brne 81b
	sbiw r30, 2
	lsl tmp_3
	dec col
	brne 80b
89:
;------ The result is in the Least Significant Bits of a[0] ... a[7]
	/* Z should point at a[0] */
	ldi r25, 8
	clr r24
90:
	ld tmp_0, Z
	adiw r30, 2
	lsr tmp_0
	rol r24
	dec r25
	brne 90b
mqq_q_exit:
;	stack_free 25
;	pop r29
;	pop r28
	ret

/******************************************************************************/

/*
  param dest:  r24:r25
  param hash:  r22:r23
  param key:   r20:r21
*/

dest_0 =  2
dest_1 =  3
xr1_0  =  4
xr1_1  =  5
key_0  =  6
key_1  =  7
i      =  8
c      =  9
qstack_0 = 10
qstack_1 = 11

.global mqq160_sign
mqq160_sign:
	push_range 2, 11
	push_range 28, 29
	stack_alloc 20, r26, r27  /* r1[20] + key */
	adiw r26, 1 /* X points to stack memory */
	movw key_0, r20
	movw xr1_0, r26
	movw dest_0, r24
	/* call to mqq_inv_affine_transformation(hash, dest, &key); */
	movw r24, r22
	movw r22, dest_0
	movw r20, key_0
	rcall mqq_inv_affine_transformation

	/* r1[0]=((uint8_t*)dest)[0]; */
	movw r26, dest_0
	movw r30, xr1_0
	ld r0, X
	st Z, r0
;----
	ldi r18, 19
	mov c, r18
	clr i
	inc i
	stack_alloc 25, r28, r29
	adiw r28, 1
	movw qstack_0, r28
20:	mov r24, i
	movw r26, xr1_0
	add r26, i
	adc r27, r1
	sbiw r26, 1
	ld r22, X
	movw r26, dest_0
	add r26, i
	adc r27, r1
	ld r20, X
	movw r18, key_0
	movw r26, qstack_0
	rcall mqq_q
	movw r26, xr1_0
	add r26, i
	adc r27, r1
	st X, r24
	inc i
	dec c
	brne 20b
	stack_free 25
;-----


	movw r28, key_0
	ldd r30, Y+8
	ldd r31, Y+9
	movw r26, xr1_0
	ldi r18, 20
30: ld r20, Z+
	swap r20
	andi r20, 0xF0
	ld r21, Z+
	andi r21, 0x0F
	or r20, r21
	ld r21, X
	eor r21, r20
	st X+, r21
	dec r18
	brne 30b
;----

	movw r24, xr1_0
	movw r22, dest_0
	movw r20, key_0
	rcall mqq_inv_affine_transformation
	stack_free 20
	pop_range 28, 29
	pop_range 2, 11
	ret


