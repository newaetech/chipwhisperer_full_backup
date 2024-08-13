/* aes_dec-asm.S */
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
 * \file     aes_dec-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-10
 * \license  GPLv3 or later
 * 
 */

#include "avr-asm-macros.S"
A = 28
B = 29
P = 0
xREDUCER = 25 
gf256mul:
	clr P
1:	
	lsr A
	breq 4f
	brcc 2f
	eor P, B
2:
	lsl B
	brcc 3f
	eor B, xREDUCER	
3:
	rjmp 1b
4:
	brcc 2f
	eor P, B
2:
	ret

.global aes256_dec
aes256_dec:
	ldi r20, 14
	rjmp aes_decrypt_core

.global aes192_dec
aes192_dec:
	ldi r20, 12
	rjmp aes_decrypt_core

.global aes128_dec
aes128_dec:
	ldi r20, 10


/*
  void aes_decrypt_core(aes_cipher_state_t* state, const aes_genctx_t* ks, uint8_t rounds)
*/
T0= 2
T1= 3
T2= 4
T3= 5
T4 = 6
T5 = 7
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
.global aes_decrypt_core
aes_decrypt_core:
	push_range 2, 17
	push r28
	push r29
	push r24
	push r25
	movw r26, r22
	movw r30, r24
	mov  CTR, r20
	inc r20
	swap r20 /* r20*16 */
	add r26, r20
	adc r27, r1
	clt
;	ldi CTR, 2	
	.irp param, ST00, ST01, ST02, ST03, ST10, ST11, ST12, ST13, ST20, ST21, ST22, ST23, ST30, ST31, ST32, ST33
		ld \param, Z+
	.endr
	
	ldi xREDUCER, 0x1b /* load reducer */
	ldi r31, hi8(aes_invsbox)
	

	.irp param, ST33, ST32, ST31, ST30, ST23, ST22, ST21, ST20, ST13, ST12, ST11, ST10, ST03, ST02, ST01, ST00
		ld r0, -X
		eor \param, r0
	.endr
1:
	dec CTR
	brne 2f
	set
2:	
	/* substitute and invShift */
	.irp param, ST00, ST10, ST20, ST30
		mov r30, \param
		lpm \param, Z
	.endr
	mov r30, ST31
	lpm T0, Z
	mov r30, ST21
	lpm ST31, Z
	mov r30, ST11
	lpm ST21, Z
	mov r30, ST01
	lpm ST11, Z
	mov ST01, T0
	
	mov r30, ST32
	lpm T0, Z
	mov r30, ST22
	lpm T1,Z
	mov r30, ST12
	lpm ST32, Z
	mov r30, ST02
	lpm ST22, Z
	mov ST12, T0
	mov ST02, T1
	
	mov r30, ST03
	lpm T0, Z
	mov r30, ST13
	lpm ST03, Z
	mov r30, ST23
	lpm ST13, Z
	mov r30, ST33
	lpm ST23, Z
	mov ST33, T0
	
	/* key addition */
	.irp param, ST33, ST32, ST31, ST30, ST23, ST22, ST21, ST20, ST13, ST12, ST11, ST10, ST03, ST02, ST01, ST00
		ld r0, -X
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
2:	
	/* inv column (row) mixing*/
	/* invMixCol (Row) 1 */
		/* preparing */
	mov T0, ST03
	eor T0, ST02 ; T0 = t
	mov T1, ST00
	eor T1, ST01 ; T1 = u
	mov T2, T0
	eor T2, T1

	mov B, T2
	ldi A, 0x08
	rcall gf256mul
	eor T2, P  ; T2 = v'
	
	mov B, ST02
	eor B, ST00
	ldi A, 0x04
	rcall gf256mul
	mov T3, P
	eor T3, T2;  T3 = w
	
	mov B, ST03
	eor B, ST01
	ldi A, 0x04
	rcall gf256mul
	eor T2, P ; T2 = v
	 
	   /* now the big move */
	mov T4, ST00
	eor T4, ST03
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST03, T4	

	mov T4, ST02
	eor T4, ST01
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST01, T4
	
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:  eor T0, T3
	eor ST02, T0
	
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:  eor T1, T3
	eor ST00, T1

	/* invMixCol (Row) 2 */
		/* preparing */
	mov T0, ST13
	eor T0, ST12 ; T0 = t
	mov T1, ST10
	eor T1, ST11 ; T1 = u
	mov T2, T0
	eor T2, T1

	mov B, T2
	ldi A, 0x08
	rcall gf256mul
	eor T2, P  ; T2 = v'
	
	mov B, ST12
	eor B, ST10
	ldi A, 0x04
	rcall gf256mul
	mov T3, P
	eor T3, T2;  T3 = w
	
	mov B, ST13
	eor B, ST11
	ldi A, 0x04
	rcall gf256mul
	eor T2, P ; T2 = v
	 
	   /* now the big move */
	mov T4, ST10
	eor T4, ST13
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST13, T4	

	mov T4, ST12
	eor T4, ST11
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST11, T4
	
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:  eor T0, T3
	eor ST12, T0
	
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:  eor T1, T3
	eor ST10, T1
	
	/* invMixCol (Row) 2 */
		/* preparing */
	mov T0, ST23
	eor T0, ST22 ; T0 = t
	mov T1, ST20
	eor T1, ST21 ; T1 = u
	mov T2, T0
	eor T2, T1

	mov B, T2
	ldi A, 0x08
	rcall gf256mul
	eor T2, P  ; T2 = v'
	
	mov B, ST22
	eor B, ST20
	ldi A, 0x04
	rcall gf256mul
	mov T3, P
	eor T3, T2;  T3 = w
	
	mov B, ST23
	eor B, ST21
	ldi A, 0x04
	rcall gf256mul
	eor T2, P ; T2 = v
	 
	   /* now the big move */
	mov T4, ST20
	eor T4, ST23
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST23, T4	

	mov T4, ST22
	eor T4, ST21
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST21, T4
	
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:  eor T0, T3
	eor ST22, T0
	
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:  eor T1, T3
	eor ST20, T1
	
	/* invMixCol (Row) 3 */
		/* preparing */
	mov T0, ST33
	eor T0, ST32 ; T0 = t
	mov T1, ST30
	eor T1, ST31 ; T1 = u
	mov T2, T0
	eor T2, T1

	mov B, T2
	ldi A, 0x08
	rcall gf256mul
	eor T2, P  ; T2 = v'
	
	mov B, ST32
	eor B, ST30
	ldi A, 0x04
	rcall gf256mul
	mov T3, P
	eor T3, T2;  T3 = w
	
	mov B, ST33
	eor B, ST31
	ldi A, 0x04
	rcall gf256mul
	eor T2, P ; T2 = v
	 
	   /* now the big move */
	mov T4, ST30
	eor T4, ST33
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST33, T4	

	mov T4, ST32
	eor T4, ST31
	lsl T4
	brcc 3f
	eor T4, xREDUCER
3:  eor T4, T2
	eor ST31, T4
	
	lsl T0
	brcc 3f
	eor T0, xREDUCER
3:  eor T0, T3
	eor ST32, T0
	
	lsl T1
	brcc 3f
	eor T1, xREDUCER
3:  eor T1, T3
	eor ST30, T1
	
	rjmp 1b
	
	
	
