/* noekeon_asm.S */
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
 * noekeon assembler implementation for avr
 * author: Daniel Otte
 * email:  daniel.otte@rub.de
 * license: GPLv3
 */

#include <avr/io.h>

.macro push_all
	push r2
	push r3
	push r4
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
.endm

.macro pop_all
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
	pop r3
	pop r2
	clr r1
.endm

push_all_func:
	pop r31
	pop r30
	push_all
	ijmp

pop_all_func:
    pop r31
    pop r30
    pop_all
    ijmp

.macro xchg a b
	eor \a, \b
	eor \b, \a
	eor \a, \b
.endm

.macro op32 op a b
	\op \a\()_0, \b\()_0
	\op \a\()_1, \b\()_1
	\op \a\()_2, \b\()_2
	\op \a\()_3, \b\()_3
.endm


.macro op32_4t op a b c d w x y z
	\op \a, \w
	\op \b, \x
	\op \c, \y
	\op \d, \z
.endm


.macro op32_prefix op p q a b c d w x y z
	\op \p\()\a, \q\()\w
	\op \p\()\b, \q\()\x
	\op \p\()\c, \q\()\y
	\op \p\()\d, \q\()\z
.endm

; === bigendian_rotl32 ===
; this function rotates a 32bit bigendian word n bits to the left
;  param1: the 32-bit value
;	given in r25,r24,r23,r22 (r22 is most significant)
;  param2: the 8-bit parameter giving the number of bits to rotate
;	given in r20
;  return: the rotatet 32-bit word
;   given in r25,r24,r23,r22   

bigendian_rotl32:
	/* copy high bit of r22 to carry */
	mov r1, r22
2:
	rol r1

	rol r25
	rol r24
	rol r23
	rol r22
	
	dec r20
	brne 2b
bigendian_rotl32_exit:
	clr r1
	ret
	
	
/******************************************************************************/

; === bigendian_rotl32 ===
; this function rotates a 32bit bigendian word n bits to the right
;  param1: the 32-bit value
;	given in r25,r24,r23,r22 (r22 is most significant)
;  param2: the 8-bit parameter giving the number of bits to rotate
;	given in r20
;  return: the rotatet 32-bit word
;   given in r25,r24,r23,r22   

bigendian_rotr32:
	/* copy high bit of r25 to carry */

	mov r1, r25
2:
	ror r1
	
	ror r22
	ror r23
	ror r24
	ror r25
	dec r20
	brne 2b
bigendian_rotr32_exit:
	clr r1
	ret

/******************************************************************************/
/*
void theta(uint32_t* k, uint32_t* a){
	uint32_t temp;
	temp = a[0] ^ a[2]; temp ^= ROTR32(temp, 8) ^ ROTL32(temp, 8);
	a[1] ^= temp;
	a[3] ^= temp;
	
	a[0] ^= k[0];
	a[1] ^= k[1];
	a[2] ^= k[2];
	a[3] ^= k[3];

	temp = a[1] ^ a[3]; temp ^= ROTR32(temp, 8) ^ ROTL32(temp, 8);
	a[0] ^= temp;
	a[2] ^= temp;	
}
*/

round_const: .byte    0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D, 0x9A, \
	            0x2F, 0x5E, 0xBC, 0x63, 0xC6, 0x97, 0x35, 0x6A, \
                0xD4

;-- a[0]
state0_0 =  2
state0_1 =  3
state0_2 =  4
state0_3 =  5
;-- a[1]
state1_0 =  6
state1_1 =  7
state1_2 =  8
state1_3 =  9
;-- a[2]
state2_0 = 10
state2_1 = 11
state2_2 = 12
state2_3 = 13
;-- a[3]
state3_0 = 14
state3_1 = 15
state3_2 = 16
state3_3 = 17

; === theta ===
;
;  param1: the state in r2-r17
;  param2: pointer to k in X (r26,r27)
;
temp_a = 18
temp_b = 19
temp_c = 20
temp_d = 21

theta:
	/* temp = a[0] ^ a[2]; temp ^= temp>>>8 ^ temp<<<8 */
	op32_prefix mov, temp_, state0_, a,b,c,d, 0,1,2,3
	op32_prefix eor, temp_, state2_, a,b,c,d, 0,1,2,3

	mov r1, temp_a
	eor r1, temp_b
	eor r1, temp_c
	eor r1, temp_d
	
	op32_prefix eor, temp_, r, a,b,c,d, 1,1,1,1

	/* temp is know a little bit mixed c,d,a,b (if abcd is normal order) */
	/* a[1] ^= temp */
	eor state1_0, temp_c 
	eor state1_1, temp_d 
	eor state1_2, temp_a 
	eor state1_3, temp_b
	/* a[3] ^= temp */
	eor state3_0, temp_c 
	eor state3_1, temp_d 
	eor state3_2, temp_a 
	eor state3_3, temp_b
	
	/* state ^ k (X points to K) */
	ldi r28, 2
	clr r29 /* Y points to r2 aka state0_0 */
	ldi temp_a, 16
1:	
	ld r1, X+
	ld r0, Y
	eor r1, r0
	st Y+, r1
	dec temp_a
	brne 1b
	sbiw r26, 16 /* set X back to key */ 
	
	mov temp_a, state1_0
	mov temp_b, state1_1
	mov temp_c, state1_2
	mov temp_d, state1_3
	eor temp_a, state3_0
	eor temp_b, state3_1
	eor temp_c, state3_2
	eor temp_d, state3_3
	mov r1, temp_a
	eor r1, temp_b
	eor r1, temp_c
	eor r1, temp_d
	eor temp_a, r1
	eor temp_b, r1
	eor temp_c, r1
	eor temp_d, r1
	/* temp is know a little bit mixed c,d,a,b (if abcd is normal order) */
	/* a[0] ^= temp */
	eor state0_0, temp_c 
	eor state0_1, temp_d 
	eor state0_2, temp_a 
	eor state0_3, temp_b
	/* a[2] ^= temp */
	eor state2_0, temp_c 
	eor state2_1, temp_d 
	eor state2_2, temp_a 
	eor state2_3, temp_b
	
	clr r1
	ret

/******************************************************************************/
#ifndef NOEKEON_NO_ENC
; === noekeon_enc ===
;
;  param1: pointer to buffer (r24,r25)
;  param2: pointer to k (r22,r23) 
;
.global noekeon_enc
noekeon_enc:
	rcall push_all_func
	/* load state */
	movw r26, r22
	ldi r28, 2
	clr r29	/* Y points at r2 aka state0_0 */
	movw r30, r24 /* Z points at state */
	push r30
	push r31
	ldi r22, 16
	push r22 /* 16 is also the number of rounds and gets pushed here */
1:	
	ld r0, Z+
	st Y+, r0
	dec r22
	brne 1b
	/* state loaded */
	push r1 /* push round constan2 (0x00) */
	ldi r20, 0x80
	push r20 /* push round constan2 (0x00) */
	rjmp 3f
2:
	ldi r30, lo8(round_const+15)
	ldi r31, hi8(round_const+15)
	sub r30, r22
	sbci r31, 0
	clr r1
	push r1
	lpm r0, Z
	push r0
3:
	rcall round /* pops rc2 & rc1 */
	pop r22
	dec r22
	push r22
	brne 2b

	pop r22

	ldi r22, 0xD4
	eor state0_3, r22
	rcall theta

	pop r31
	pop r30
	clr r29
	ldi r28, 2
	ldi r22, 16
1:	
	ld r0, Y+
	st Z+, r0
	dec r22
	brne 1b
	
	rcall pop_all_func
	ret
#endif
/******************************************************************************/
/******************************************************************************/
#ifndef NOEKEON_NO_DEC

; === noekeon_dec ===
;
;  param1: pointer to buffer/state (r24,r25)
;  param2: pointer to k  (r22,r23) 
;
.global noekeon_dec
noekeon_dec:
	rcall push_all_func
	/* allocate 16 bytes on the stack */
	in r30, _SFR_IO_ADDR(SPL)
	in r31, _SFR_IO_ADDR(SPH)
	sbiw r30, 16 
	out  _SFR_IO_ADDR(SPH), r31
	out  _SFR_IO_ADDR(SPL), r30

	adiw r30, 1
	/* push state pointer */
	push r24
	push r25
	movw r26, r22 /* move key ptr to X */

	/* set stackkey to zero */
	ldi r22, 16
1:	st Z+, r1
	dec r22
	brne 1b
		
	/* copy key to state */
	clr r29
	ldi r28, 2
	ldi r22, 16
1:  ld r0, X+
	st Y+, r0
	dec r22
	brne 1b	

	movw r26, r30
	sbiw r26, 16 /* set X back to begining of stack key */
	rcall theta
	
	/* mov state to stackkey */
	clr r29
	ldi r28,  2
	ldi r22, 16
1:	ld r0, Y+
	st X+, r0	
	dec r22
	brne 1b
	sbiw r26, 16 /* set X back to begining of stack key */
	
	/* move data from stateptr to state */
	pop r31
	pop r30
	push r30
	push r31
	clr r29
	ldi r28,  2
	ldi r22, 16
	push r22
1:	ld r0, Z+
	st Y+, r0
	dec r22
	brne 1b	
	
;--- snip 8< ----
	
	ldi r20, 0xD4
	push r20 /* push round constant2 (0xD4) */
	push r22 /* push round constan1 (0x00) */
	rjmp 3f
2:
	ldi r30, lo8(round_const-1)
	ldi r31, hi8(round_const-1)
	clr r1
	add r30, r22
	adc r31, r1
	lpm r0, Z
	push r0
	push r1
3:
	rcall round /* pops rc2 & rc1 */
	pop r22
	dec r22
	push r22
	brne 2b
;----
	pop r22

	rcall theta
	ldi r22, 0x80
	eor state0_3, r22
	
write_state_back:	
	/* write state back */
	pop r31 /* pop state pointer */
	pop r30
	clr r29
	ldi r28, 2
	ldi r22, 16
1:	
	ld r0, Y+
	st Z+, r0
	dec r22
	brne 1b
	
	/* remove key from stack */
	in r30, _SFR_IO_ADDR(SPL)
	in r31, _SFR_IO_ADDR(SPH)
	adiw r30, 16 
	out  _SFR_IO_ADDR(SPH), r31
	out  _SFR_IO_ADDR(SPL), r30
	rcall pop_all_func
	ret
#endif
/******************************************************************************/
	
	
round:	
	pop r24
	pop r25
	pop r1
	eor state0_3, r1
	rcall theta
	pop r1
	eor state0_3, r1
	push r25
	push r24
pi_gamma_pi:
	ldi r30, pm_lo8(bigendian_rotl32)
	ldi r31, pm_hi8(bigendian_rotl32)
	rcall pi
	/* pi1 done; now gamma */
	rcall gamma_1
	/* a[0] <-> a[3] */
	xchg state0_0, state3_0
	xchg state0_1, state3_1
	xchg state0_2, state3_2
	xchg state0_3, state3_3
	/* a[2] ^= a[0] ^ a[1] ^ a[3] */
	op32 eor, state2, state0
	op32 eor, state2, state1
	op32 eor, state2, state3

	rcall gamma_1
	ldi r30, pm_lo8(bigendian_rotr32)
	ldi r31, pm_hi8(bigendian_rotr32)
	rcall pi
	ret
	
gamma_1:
	/* a[1] ^= ~(a[3]|a[2])*/
	mov r1, state3_0
	or  r1, state2_0
	com r1
	eor state1_0, r1

	mov r1, state3_1
	or  r1, state2_1
	com r1
	eor state1_1, r1

	mov r1, state3_2
	or  r1, state2_2
	com r1
	eor state1_2, r1
	
	mov r1, state3_3
	or  r1, state2_3
	com r1
	eor state1_3, r1
	
	/* a[0] ^= a[2]&a[1] */
	mov r1, state2_0
	and r1, state1_0
	eor state0_0, r1
	
	mov r1, state2_1
	and r1, state1_1
	eor state0_1, r1
	
	mov r1, state2_2
	and r1, state1_2
	eor state0_2, r1
	
	mov r1, state2_3
	and r1, state1_3
	eor state0_3, r1
	ret
	
pi:	
	/* a[1] <<<= 1*/
	mov r22, state1_0
	mov r23, state1_1
	mov r24, state1_2
	mov r25, state1_3
	ldi r20, 1
	icall
	mov state1_0, r22
	mov state1_1, r23
	mov state1_2, r24
	mov state1_3, r25
	/* a[2] <<<= 5*/
	mov r22, state2_0
	mov r23, state2_1
	mov r24, state2_2
	mov r25, state2_3
	ldi r20, 5
	icall
	mov state2_0, r22
	mov state2_1, r23
	mov state2_2, r24
	mov state2_3, r25
	/* a[3] <<<= 2*/
	mov r22, state3_0
	mov r23, state3_1
	mov r24, state3_2
	mov r25, state3_3
	ldi r20, 2
	icall
	mov state3_0, r22
	mov state3_1, r23
	mov state3_2, r24
	mov state3_3, r25
	ret

/******************************************************************************/
	
/*
void noekeon_init(void* key, noekeon_ctx_t* ctx){
	uint8_t nullv[16];
	
	memset(nullv, 0, 16);
	memcpy(ctx, key, 16);
	noekeon_enc(ctx, nullv);
}
*/

#ifndef NOEKEON_NO_INIT

.global noekeon_init
noekeon_init:
; === noekeon_init ===
;
;  param1: pointer to key (r24,r25)
;  param2: pointer to context  (r22,r23) 
;
	in r30, _SFR_IO_ADDR(SPL)
	in r31, _SFR_IO_ADDR(SPH)
	sbiw r30, 16 
	out  _SFR_IO_ADDR(SPH), r31
	out  _SFR_IO_ADDR(SPL), r30

	movw r26, r22
	adiw r30, 1
	movw r22, r30
	/* set nullv(stack) to zero */
	ldi r20, 16
1:	st Z+, r1
	dec r20
	brne 1b

	/* copy key data to ctx */
	movw r30, r24
	ldi r20, 16
1:	ld r1, Z+
	st X+, r1
	dec r20
	brne 1b
	clr r1
	
	sbiw r26, 16
	movw r24, r26
	rcall noekeon_enc
	
	in r30, _SFR_IO_ADDR(SPL)
	in r31, _SFR_IO_ADDR(SPH)
	adiw r30, 16 
	out  _SFR_IO_ADDR(SPH), r31
	out  _SFR_IO_ADDR(SPL), r30	
	ret
	
#endif


