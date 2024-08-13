/* xtea-asm.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2011 Daniel Otte (daniel.otte@rub.de)

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

#include "avr-asm-macros.S"

B0 =  4
B1 =  5
B2 =  6
B3 =  7

A0 =  8
A1 =  9
A2 = 10
A3 = 11

V00 = 12
V01 = 13
V02 = 14
V03 = 15

V10 = 16
V11 = 17
V12 = 18
V13 = 19

S0 = 20
S1 = 21
S2 = 22
S3 = 23

xchg_V0V1:
	movw r26, V10
	movw V10, V00
	movw V00, r26
	movw r26, V12
	movw V12, V02
	movw V02, r26
	ret

eor_AB:
	eor A0, B0
	eor A1, B1
	eor A2, B2
	eor A3, B3
	ret

g_func:
	movw A0, V10
	movw A2, V12
	movw B0, V10
	movw B2, V12

	ldi r24, 4
10:
	lsl A0
	rol A1
	rol A2
	rol A3
	dec r24
	brne 10b

	ldi r24, 5
10:
	lsr B3
	ror B2
	ror B1
	ror B0
	dec r24
	brne 10b

	rcall eor_AB

	add A0, V10
	adc A1, V11
	adc A2, V12
	adc A3, V13

	ret

sum_plus_k:
	andi r24, (3<<2)
	movw r26, r30
	add r26, r24
	adc r27, r1
	ld B0, X+
	ld B1, X+
	ld B2, X+
	ld B3, X+
	add B0, S0
	adc B1, S1
	adc B2, S2
	adc B3, S3
	rcall eor_AB
	brtc 20f
	add V00, A0
	adc V01, A1
	adc V02, A2
	adc V03, A3
	ret
20:	sub V00, A0
	sbc V01, A1
	sbc V02, A2
	sbc V03, A3
	ret

main1:
	rcall g_func
	mov r24, S0
	lsl r24
	lsl r24
20:
	rcall sum_plus_k
	ret

main2:
	rcall xchg_V0V1
	rcall g_func
	mov r24, S1
	lsr r24
	rcall xchg_V0V1
	rjmp 20b

.global xtea_enc
xtea_enc:
	set
xtea_intro:
	clr r27
	ldi r26, 4
	ldi r30, 14
10:
	ld r0, X+
	push r0
	dec r30
	brne 10b

	push r24
	push r25
	movw r30, r20
/* load block */
	movw r26, r22
	ld V00, X+
	ld V01, X+
	ld V02, X+
	ld V03, X+
	ld V10, X+
	ld V11, X+
	ld V12, X+
	ld V13, X+
	ldi r24, 32
	mov r0, r24
	brtc xtea_dec_start
	clr S0
	clr S1
	movw S2, S0

10:
	rcall main1
	subi S0, 0x47
	sbci S1, 0x86
	sbci S2, 0xC8
	sbci S3, 0x61
	rcall main2

	dec r0
	brne 10b

/* store back */
xtea_enc_exit:
	pop r27
	pop r26
/*
	st X+, V00
	st X+, V01
	st X+, V02
	st X+, V03
	st X+, V10
	st X+, V11
	st X+, V12
	st X+, V13
*/
	clr r31
	ldi r30, V00
	ldi r24, 8
10:
	ld r0, Z+
	st X+, r0
	dec r24
	brne 10b

;	clr r31
	ldi r30, 18
	ldi r24, 14
10:
	pop r0
	st -Z, r0
	dec r24
	brne 10b
	ret


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

.global xtea_dec
xtea_dec:
	clt
	rjmp xtea_intro
xtea_dec_start:
	ldi S0, 0x20 /* sum = 0xC6EF3720 */
	ldi S1, 0x37
	ldi S2, 0xEF
	ldi S3, 0xC6

10:
	rcall main2
	subi S0, 0xB9
	sbci S1, 0x79
	sbci S2, 0x37
	sbci S3, 0x9E
	rcall main1

	dec r0
	brne 10b
/* store back */
	rjmp xtea_enc_exit



