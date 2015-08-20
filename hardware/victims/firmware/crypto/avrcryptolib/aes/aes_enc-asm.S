/* aes_enc-asm.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file     aes_enc-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-10
 * \license  GPLv3 or later
 * 
 */

#include "avr-asm-macros.S"


/*
 * param a: r24
 * param b: r22
 * param reducer: r0
 */
A = 28
B = 29
P = 0
xREDUCER = 25

.global aes256_enc
aes256_enc:
	ldi r20, 14
	rjmp aes_encrypt_core

.global aes192_enc
aes192_enc:
	ldi r20, 12
	rjmp aes_encrypt_core

.global aes128_enc
aes128_enc:
	ldi r20, 10


/*
  void aes_encrypt_core(aes_cipher_state_t* state, const aes_genctx_t* ks, uint8_t rounds)
*/
T0= 2
T1= 3
T2= 4
T3= 5
SBOX_SAVE0 = 6
SBOX_SAVE1 = 7
ST00 =  8
ST01 =  9
ST02 = 10
ST03 = 11
ST10 = 12
ST11 = 13
ST12 = 14
ST13 = 15
ST20 = 16
ST21 = 17
ST22 = 18
ST23 = 19
ST30 = 20
ST31 = 21
ST32 = 22
ST33 = 23
CTR = 24
/*
 * param state:  r24:r25
 * param ks:     r22:r23
 * param rounds: r20   
 */
.global aes_encrypt_core
aes_encrypt_core:
	push_range 2, 17
	push r28
	push r29
	push r24
	push r25
	movw r26, r22
	movw r30, r24
	mov  CTR, r20
	clt
	
	.irp param,ST00, ST01, ST02, ST03, ST10, ST11, ST12, ST13, ST20, ST21, ST22, ST23, ST30, ST31, ST32, ST33
		ld \param, Z+
	.endr
	
	ldi xREDUCER, 0x1b /* load reducer */
	ldi r31, hi8(aes_sbox)
	
	/* key whitening */
1:
	.irp param,ST00, ST01, ST02, ST03, ST10, ST11, ST12, ST13, ST20, ST21, ST22, ST23, ST30, ST31, ST32, ST33
		ld r0, X+
		eor \param, r0
	.endr
	
	brtc 2f
exit:	
	pop r31
	pop r30
	st Z+, ST00
	st Z+, ST01
	st Z+, ST02
	st Z+, ST03
	st Z+, ST10
	st Z+, ST11
	st Z+, ST12
	st Z+, ST13
	st Z+, ST20
	st Z+, ST21
	st Z+, ST22
	st Z+, ST23
	st Z+, ST30
	st Z+, ST31
	st Z+, ST32
	st Z+, ST33
	pop r29
	pop r28
	pop_range 2, 17
	ret

2:	dec CTR
	brne 3f
	set
3:

	/* encryption loop */ 

	/* SBOX substitution and shifting */
	mov r30, ST00
	lpm ST00, Z
	mov r30, ST10
	lpm ST10, Z
	mov r30, ST20
	lpm ST20, Z
	mov r30, ST30
	lpm ST30, Z

	mov r30, ST01
	lpm T0, Z
	mov r30, ST11
	lpm ST01, Z
	mov r30, ST21
	lpm ST11, Z
	mov r30, ST31
	lpm ST21, Z
	mov ST31, T0

	mov r30, ST02
	lpm T0, Z
	mov r30, ST12
	lpm T1, Z
	mov r30, ST22
	lpm ST02, Z
	mov r30, ST32
	lpm ST12, Z
	mov ST22, T0
	mov ST32, T1

	mov r30, ST03
	lpm T0, Z
	mov r30, ST33
	lpm ST03, Z
	mov r30, ST23
	lpm ST33, Z
	mov r30, ST13
	lpm ST23, Z
	mov ST13, T0
		
	/* mixcols (or rows in our case) */
	brtc 2f
	rjmp 1b
2:	
 /* mixrow 1 */
	mov r0, ST02
	eor r0, ST03
	mov T2, r0
		
	mov T0, ST00
	eor ST00, ST01
	eor r0, ST00
	lsl ST00
	brcc 3f
	eor ST00, xREDUCER
3:	eor ST00, r0
	eor ST00, T0
	
	mov T1, ST01
	eor T1, ST02
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:	eor T1, r0
	eor ST01, T1
	
	lsl T2
	brcc 3f
	eor T2, xREDUCER
3:  eor T2, r0
	eor ST02, T2
	
	eor T0, ST03
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:	eor T0, r0
	eor ST03, T0
		
 /* mixrow 2 */
	mov r0, ST12
	eor r0, ST13
	mov T2, r0
		
	mov T0, ST10
	eor ST10, ST11
	eor r0, ST10
	lsl ST10
	brcc 3f
	eor ST10, xREDUCER
3:	eor ST10, r0
	eor ST10, T0
	
	mov T1, ST11
	eor T1, ST12
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:	eor T1, r0
	eor ST11, T1
	
	lsl T2
	brcc 3f
	eor T2, xREDUCER
3:  eor T2, r0
	eor ST12, T2
	
	eor T0, ST13
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:	eor T0, r0
	eor ST13, T0
		
 /* mixrow 3 */
	mov r0, ST22
	eor r0, ST23
	mov T2, r0
		
	mov T0, ST20
	eor ST20, ST21
	eor r0, ST20
	lsl ST20
	brcc 3f
	eor ST20, xREDUCER
3:	eor ST20, r0
	eor ST20, T0
	
	mov T1, ST21
	eor T1, ST22
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:	eor T1, r0
	eor ST21, T1
	
	lsl T2
	brcc 3f
	eor T2, xREDUCER
3:  eor T2, r0
	eor ST22, T2
	
	eor T0, ST23
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:	eor T0, r0
	eor ST23, T0
		
 /* mixrow 4 */
	mov r0, ST32
	eor r0, ST33
	mov T2, r0
		
	mov T0, ST30
	eor ST30, ST31
	eor r0, ST30
	lsl ST30
	brcc 3f
	eor ST30, xREDUCER
3:	eor ST30, r0
	eor ST30, T0
	
	mov T1, ST31
	eor T1, ST32
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:	eor T1, r0
	eor ST31, T1
	
	lsl T2
	brcc 3f
	eor T2, xREDUCER
3:  eor T2, r0
	eor ST32, T2
	
	eor T0, ST33
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:	eor T0, r0
	eor ST33, T0
	/* mix colums (rows) done */

	/* add key*/
	rjmp 1b
	








