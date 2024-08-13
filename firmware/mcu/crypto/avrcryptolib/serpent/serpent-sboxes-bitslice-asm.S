/* serpent-sboxes-bitslice.c */
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
/* serpent-sboxes.c
 * a bitsliced implementation of the serpent sboxes
 * author: Daniel Otte 
 * email:  daniel.otte@rub.de
 * license: GPLv3
 */

#include "avr-asm-macros.S"
IN0  = 22
IN1  = 23
IN2  = 24
IN3  = 25
OUT0 = 18
OUT1 = 19
OUT2 = 20
OUT3 = 21
T00 =  2
T00 =  3
T01 =  4
T02 =  5
T03 =  6
T04 =  7
T05 =  8
T06 =  9
T07 = 10
T08 = 11
T09 = 12
T10 = 13
T11 = 14
T12 = 15
T13 = 16
T14 = 17
T15 = 26
T16 = 27
T17 =  0

/* S0:   3  8 15  1 10  6  5 11 14 13  4  2  7  0  9 12 */

/* depth = 5,7,4,2, Total gates=18 */
sb0:
    mov T00, IN1
    eor T00, IN2
    mov T01, IN0
    or  T01, IN3
    mov T02, IN0
    eor T02, IN1
    mov OUT3, T01
    eor OUT3, T00
    mov T04, IN2
    or  T04, OUT3
    mov T05, IN0
    eor T05, IN3
    mov T06, IN1
    or  T06, IN2
    mov T07, IN3
    and T07, T04
    mov T08, T02
    and T08, T06
    mov OUT2, T08
    eor OUT2, T07
    mov T10, T08
    and T10, OUT2
    mov T11, IN2
    eor T11, IN3
    mov T12, T06
    eor T12, T10
    mov T13, IN1
    and T13, T05
    mov T14, T05
    eor T14, T12
	mov OUT0, T14
	com OUT0
	mov T16, OUT0
    eor T16, T13
    mov OUT1, T11
    eor OUT1, T16
	ret

	
/* InvS0:  13  3 11  0 10  6  5 12  1 14  4  7 15  9  8  2 */

/* depth = 8,4,3,6, Total gates=19 */
sb0_inv:
    mov T00, IN2
    eor T00, IN3
    mov T01, IN0
    or  T01, IN1
    mov T02, IN1
    or  T02, IN2
    mov T03, IN2
    and T03, T00
    mov T04, T01
    eor T04, T00
    mov T05, IN0
    or  T05, T03
	mov OUT2, T04
	com OUT2
    mov T07, IN1
    eor T07, IN3
    mov T08, T02
    and T08, T07
    mov T09, IN3
    or  T09, OUT2
    mov OUT1, T08
    eor OUT1, T05
    mov T11, IN0
    or  T11, T04
    mov T12, OUT1
    eor T12, T11
    mov T13, T02
    eor T13, T09
    mov T14, IN0
    eor T14, IN2
    mov OUT3, T13
    eor OUT3, T12
    mov T16, T04
    and T16, T12
    mov T17, T13
    or  T17, T16
    mov OUT0, T14
    eor OUT0, T17
	ret


/* S1:  15 12  2  7  9  0  5 10  1 11 14  8  6 13  3  4 */

/* depth = 10,7,3,5, Total gates=18 */
sb1:
    mov T00, IN0
    or  T00, IN3
    mov T01, IN2
    eor T01, IN3
	mov T02, IN1
	com T02
    mov T03, IN0
    eor T03, IN2
    mov T04, IN0
    or  T04, T02
    mov T05, IN3
    and T05, T03
    mov T06, T00
    and T06, T01
    mov T07, IN1
    or  T07, T05
    mov OUT2, T01
    eor OUT2, T04
    mov T09, T06
    eor T09, T07
    mov T10, T00
    eor T10, T09
    mov T11, OUT2
    eor T11, T10
    mov T12, IN1
    and T12, IN3
	mov OUT3, T09
	com OUT3
    mov OUT1, T12
    eor OUT1, T11
    mov T15, T09
    or  T15, OUT1
    mov T16, T04
    and T16, T15
    mov OUT0, IN2
    eor OUT0, T16
	ret


/* InvS1:   5  8  2 14 15  6 12  3 11  4  7  9  1 13 10  0 */

/* depth = 7,4,5,3, Total gates=18 */
sb1_inv:
    mov T00, IN0
    eor T00, IN1
    mov T01, IN1
    or  T01, IN3
    mov T02, IN0
    and T02, IN2
    mov T03, IN2
    eor T03, T01
    mov T04, IN0
    or  T04, T03
    mov T05, T00
    and T05, T04
    mov T06, IN3
    or  T06, T02
    mov T07, IN1
    eor T07, T05
    mov T08, T06
    eor T08, T05
    mov T09, T03
    or  T09, T02
    mov T10, IN3
    and T10, T07
    mov OUT2, T08
    com OUT2
	mov OUT1, T09
    eor OUT1, T10
    mov T13, IN0
    or  T13, OUT2
    mov T14, T05
    eor T14, OUT1
    mov OUT3, T00
    eor OUT3, T03
    mov T16, IN2
    eor T16, T14
    mov OUT0, T13
    eor OUT0, T16
	ret

/* S2:   8  6  7  9  3 12 10 15 13  1 14  4  0 11  5  2 */

/* depth = 3,8,11,7, Total gates=16 */
sb2:
    mov T00, IN0
    or  T00, IN2
    mov T01, IN0
    eor T01, IN1
    mov T02, IN3
    eor T02, T00
    mov OUT0, T01
    eor OUT0, T02
    mov T04, IN2
    eor T04, OUT0
    mov T05, IN1
    eor T05, T04
    mov T06, IN1
    or  T06, T04
    mov T07, T00
    and T07, T05
    mov T08, T02
    eor T08, T06
    mov T09, T01
    or  T09, T08
    mov OUT1, T09
    eor OUT1, T07
    mov T11, IN0
    or  T11, IN3
    mov T12, T08
    eor T12, OUT1
    mov T13, IN1
    eor T13, T12
	mov OUT3, T08
	com OUT3
    mov OUT2, T11
    eor OUT2, T13
	ret

/* InvS2:  12  9 15  4 11 14  1  2  0  3  6 13  5  8 10  7 */

/* depth = 3,6,8,3, Total gates=18 */
sb2_inv:
    mov T00, IN0
    eor T00, IN3
    mov T01, IN2
    eor T01, IN3
    mov T02, IN0
    and T02, IN2
    mov T03, IN1
    or  T03, T01
    mov OUT0, T00
    eor OUT0, T03
    mov T05, IN0
    or  T05, IN2
    mov T06, IN3
    or  T06, OUT0
	mov T07, IN3
	com T07
    mov T08, IN1
    and T08, T05
    mov T09, T07
    or  T09, T02
    mov T10, IN1
    and T10, T06
    mov T11, T05
    and T11, T01
    mov OUT3, T08
    eor OUT3, T09
    mov OUT1, T11
    eor OUT1, T10
    mov T14, IN2
    and T14, OUT3
    mov T15, OUT0
    eor T15, OUT1
    mov T16, T09
    eor T16, T14
    mov OUT2, T15
    eor OUT2, T16
	ret

/* S3:   0 15 11  8 12  9  6  3 13  1  2  4 10  7  5 14 */

/* depth = 8,3,5,5, Total gates=18 */
sb3:
    mov T00, IN0
    eor T00, IN2
    mov T01, IN0
    or  T01, IN3
    mov T02, IN0
    and T02, IN3
    mov T03, T00
    and T03, T01
    mov T04, IN1
    or  T04, T02
    mov T05, IN0
    and T05, IN1
    mov T06, IN3
    eor T06, T03
    mov T07, IN2
    or  T07, T05
    mov T08, IN1
    eor T08, T06
    mov T09, IN3
    and T09, T04
    mov T10, T01
    eor T10, T09
    mov OUT3, T07
    eor OUT3, T08
    mov T12, IN3
    or  T12, OUT3
    mov T13, IN0
    or  T13, T06
    mov T14, IN1
    and T14, T12
    mov OUT2, T07
    eor OUT2, T10
    mov OUT0, T13
    eor OUT0, T14
    mov OUT1, T04
    eor OUT1, T03
	ret

/* InvS3:   0  9 10  7 11 14  6 13  3  5 12  2  4  8 15  1 */

/* depth = 3,6,4,4, Total gates=17 */
sb3_inv:
    mov T00, IN2
    or  T00, IN3
    mov T01, IN0
    or  T01, IN3
    mov T02, IN2
    eor T02, T01
    mov T03, IN1
    eor T03, T01
    mov T04, IN0
    eor T04, IN3
    mov T05, T03
    and T05, T02
    mov T06, IN1
    and T06, T00
    mov OUT2, T04
    eor OUT2, T05
    mov T08, IN0
    eor T08, T02
    mov OUT0, T06
    eor OUT0, T02
    mov T10, OUT0
    or  T10, T04
    mov T11, T08
    and T11, T10
    mov T12, IN0
    and T12, OUT2
    mov T13, T00
    eor T13, T04
    mov OUT1, IN1
    eor OUT1, T11
    mov T15, IN1
    or  T15, T12
    mov OUT3, T13
    eor OUT3, T15
	ret

/* S4:   1 15  8  3 12  0 11  6  2  5  4 10  9 14  7 13 */

/* depth = 6,7,5,3, Total gates=19 */
sb4:
    mov T00, IN0
    or  T00, IN1
    mov T01, IN1
    or  T01, IN2
    mov T02, IN0
    eor T02, T01
    mov T03, IN1
    eor T03, IN3
    mov T04, IN3
    or  T04, T02
    mov T05, IN3
    and T05, T00
    mov OUT3, T02
    eor OUT3, T05
    mov T07, OUT3
    and T07, T03
    mov T08, T03
    and T08, T04
    mov T09, IN2
    eor T09, T05
    mov T10, IN1
    and T10, IN2
    mov T11, T03
    eor T11, T07
    mov T12, T10
    or  T12, T02
    mov T13, T09
    eor T13, T08
    mov T14, IN0
    and T14, T04
    mov T15, T10
    or  T15, T11
    mov OUT2, T12
    eor OUT2, T07
    mov OUT1, T14
    eor OUT1, T15
	mov OUT0, T13
	com OUT0
	ret

/* InvS4:   5  0  8  3 10  9  7 14  2 12 11  6  4 15 13  1 */

/* depth = 6,4,7,3, Total gates=17 */
sb4_inv:
    mov T00, IN1
    or  T00, IN3
    mov T01, IN2
    or  T01, IN3
    mov T02, IN0
    and T02, T00
    mov T03, IN1
    eor T03, T01
    mov T04, IN2
    eor T04, IN3
	mov T05, T02
	com T05
    mov T06, IN0
    and T06, T03
    mov OUT1, T04
    eor OUT1, T06
    mov T08, OUT1
    or  T08, T05
    mov T09, IN0
    eor T09, T06
    mov T10, T00
    eor T10, T08
    mov T11, IN3
    eor T11, T03
    mov T12, IN2
    or  T12, T09
    mov OUT3, T02
    eor OUT3, T11
    mov T14, IN0
    eor T14, T03
    mov OUT2, T10
    eor OUT2, T12
    mov OUT0, T14
    eor OUT0, T08
	ret

/* S5:  15  5  2 11  4 10  9 12  0  3 14  8 13  6  7  1 */

/* depth = 4,6,8,6, Total gates=17 */
sb5:
    mov T00, IN1
    eor T00, IN3
    mov T01, IN1
    or  T01, IN3
    mov T02, IN0
    and T02, T00
    mov T03, IN2
    eor T03, T01
    mov T04, T02
    eor T04, T03
	mov OUT0, T04
	com OUT0
    mov T06, IN0
    eor T06, T00
    mov T07, IN3
    or  T07, OUT0
    mov T08, IN1
    or  T08, T04
    mov T09, IN3
    eor T09, T07
    mov T10, IN1
    or  T10, T06
    mov T11, T02
    or  T11, OUT0
    mov T12, T06
    or  T12, T09
    mov T13, T00
    eor T13, T10
    mov OUT2, T08
    eor OUT2, T12
    mov OUT1, T06
    eor OUT1, T07
    mov OUT3, T11
    eor OUT3, T13
	ret

/* InvS5:   8 15  2  9  4  1 13 14 11  6  5  3  7 12 10  0 */

/* depth = 4,6,9,7, Total gates=17 */
sb5_inv:
    mov T00, IN0
    and T00, IN3
    mov T01, IN2
    eor T01, T00
    mov T02, IN0
    eor T02, IN3
    mov T03, IN1
    and T03, T01
    mov T04, IN0
    and T04, IN2
    mov OUT0, T02
    eor OUT0, T03
    mov T06, IN0
    and T06, OUT0
    mov T07, T00
    eor T07, OUT0
    mov T08, IN1
    or  T08, T04
	mov T09, IN1
	com T09
    mov OUT1, T07
    eor OUT1, T08
    mov T11, T09
    or  T11, T06
    mov T12, OUT0
    or  T12, OUT1
    mov OUT3, T01
    eor OUT3, T11
    mov T14, T01
    eor T14, T12
    mov T15, IN1
    eor T15, IN3
    mov OUT2, T15
    eor OUT2, T14
	ret

/* S6:   7  2 12  5  8  4  6 11 14  9  1 15 13  3 10  0 */

/* depth = 8,3,6,3, Total gates=19 */
sb6:
    mov T00, IN0
    and T00, IN3
    mov T01, IN1
    eor T01, IN2
    mov T02, IN0
    eor T02, IN3
    mov T03, T00
    eor T03, T01
    mov T04, IN1
    or  T04, IN2
	mov OUT1, T03
	com OUT1
    mov T06, T02
    and T06, T04
    mov T07, IN1
    and T07, OUT1
    mov T08, IN0
    or  T08, IN2
    mov T09, T06
    eor T09, T07
    mov T10, IN1
    or  T10, IN3
    mov T11, IN2
    eor T11, T10
    mov T12, T08
    eor T12, T09
	mov OUT2, T12
	com OUT2
    mov T14, OUT1
    and T14, T02
    mov OUT3, T11
    eor OUT3, T06
    mov T16, IN0
    eor T16, IN1
    mov T17, OUT2
    eor T17, T14
    mov OUT0, T16
    eor OUT0, T17
	ret

/* InvS6:  15 10  1 13  5  3  6  0  4  9 14  7  2 12  8 11 */

/* depth = 5,3,8,6, Total gates=19 */
sb6_inv:
    mov T00, IN0
    eor T00, IN2
	mov T01, IN2
	com T01
    mov T02, IN1
    and T02, T00
    mov T03, IN1
    or  T03, T01
    mov T04, IN3
    or  T04, T02
    mov T05, IN1
    eor T05, IN3
    mov T06, IN0
    and T06, T03
    mov T07, IN0
    or  T07, T01
    mov T08, T06
    eor T08, T04
    mov OUT1, T05
    eor OUT1, T07
	mov OUT0, T08
	com OUT0
    mov T11, IN1
    and T11, OUT0
    mov T12, T00
    and T12, T04
    mov T13, T00
    eor T13, T11
    mov T14, T06
    eor T14, T12
    mov T15, IN3
    or  T15, T01
    mov T16, IN0
    eor T16, OUT1
    mov OUT3, T16
    eor OUT3, T14
    mov OUT2, T15
    eor OUT2, T13
	ret

/* S7:   1 13 15  0 14  8  2 11  7  4 12 10  9  3  5  6 */

/* depth = 10,7,10,4, Total gates=19 */
sb7:
    mov T00, IN0
    and T00, IN2
	mov T01, IN3
	com T01
    mov T02, IN0
    and T02, T01
    mov T03, IN1
    or  T03, T00
    mov T04, IN0
    and T04, IN1
    mov T05, IN2
    eor T05, T03
    mov OUT3, T02
    eor OUT3, T05
    mov T07, IN2
    or  T07, OUT3
    mov T08, IN3
    or  T08, T04
    mov T09, IN0
    eor T09, T07
    mov T10, T03
    and T10, OUT3
    mov OUT1, T08
    eor OUT1, T09
    mov T12, IN1
    eor T12, OUT1
    mov T13, T00
    eor T13, OUT1
    mov T14, IN2
    eor T14, T04
    mov T15, T10
    or  T15, T12
    mov T16, T01
    or  T16, T13
    mov OUT0, T14
    eor OUT0, T16
    mov OUT2, IN0
    eor OUT2, T15
	ret

/* InvS7:   3  0  6 13  9 14 15  8  5 12 11  7 10  1  4  2 */

/* depth = 9,7,3,3, Total gates=18 */
sb7_inv:
    mov T00, IN0
    and T00, IN1
    mov T01, IN0
    or  T01, IN1
    mov T02, IN2
    or  T02, T00
    mov T03, IN3
    and T03, T01
    mov OUT3, T02
    eor OUT3, T03
    mov T05, IN1
    eor T05, T03
    mov T06, IN3
    eor T06, OUT3
    mov T07, T06
    com T07
    mov T08, T05
    or  T08, T07
    mov T09, IN1
    eor T09, IN3
    mov T10, IN0
    or  T10, IN3
    mov OUT1, IN0
    eor OUT1, T08
    mov T12, IN2
    eor T12, T05
    mov T13, IN2
    and T13, T10
    mov T14, IN3
    or  T14, OUT1
    mov T15, T00
    or  T15, T09
    mov OUT0, T12
    eor OUT0, T14
    mov OUT2, T13
    eor OUT2, T15
	ret

sf_tab:
.word sb0, sb1, sb2, sb3
.word sb4, sb5, sb6, sb7

sinvf_tab:
.word sb0_inv, sb1_inv, sb2_inv, sb3_inv
.word sb4_inv, sb5_inv, sb6_inv, sb7_inv

/*
.byte pm_lo8(sb0), pm_hi8(sb0)
.byte pm_lo8(sb1), pm_hi8(sb1)
.byte pm_lo8(sb2), pm_hi8(sb2)
.byte pm_lo8(sb3), pm_hi8(sb3)
.byte pm_lo8(sb4), pm_hi8(sb4)
.byte pm_lo8(sb5), pm_hi8(sb5)
.byte pm_lo8(sb6), pm_hi8(sb6)
.byte pm_lo8(sb7), pm_hi8(sb7)


sinvf_tab:
.byte pm_lo8(sb0_inv), pm_hi8(sb0_inv)
.byte pm_lo8(sb1_inv), pm_hi8(sb1_inv)
.byte pm_lo8(sb2_inv), pm_hi8(sb2_inv)
.byte pm_lo8(sb3_inv), pm_hi8(sb3_inv)
.byte pm_lo8(sb4_inv), pm_hi8(sb4_inv)
.byte pm_lo8(sb5_inv), pm_hi8(sb5_inv)
.byte pm_lo8(sb6_inv), pm_hi8(sb6_inv)
.byte pm_lo8(sb7_inv), pm_hi8(sb7_inv)
*/
/*
void sbox128(void * w, uint8_t box){
	uint8_t i, buffer[16];
	box &= 0x7;
	
	sb_fpt fp;
	fp = (sb_fpt)pgm_read_word(&(sf_tab[box]));
	for(i=0; i<4; ++i){
		fp(buffer+i, (uint8_t*)w+i);
	}
	memcpy(w, buffer, 16);
}
*/
.global sbox128
sbox128:
	ldi r30, lo8(sf_tab)
	ldi r31, hi8(sf_tab)
1:
;	clr r1
	andi r22, 0x07
	lsl r22
	add r30, r22
	adc r31, r1
	lpm r26, Z+
	lpm r27, Z
	lsr r27
	ror r26
	push r28
	push r29
	movw r30, r26
	movw r28, r24
	push_range 2, 17
	ldd IN0, Y+0
	ldd IN1, Y+4
	ldd IN2, Y+8
	ldd IN3, Y+12
	icall 
	std Y+0, OUT0
	std Y+4, OUT1
	std Y+8, OUT2
	std Y+12, OUT3
	ldd IN0, Y+0+1
	ldd IN1, Y+4+1
	ldd IN2, Y+8+1
	ldd IN3, Y+12+1
	icall
	std Y+0+1, OUT0
	std Y+4+1, OUT1
	std Y+8+1, OUT2
	std Y+12+1, OUT3
	ldd IN0, Y+0+2
	ldd IN1, Y+4+2
	ldd IN2, Y+8+2
	ldd IN3, Y+12+2
	icall
	std Y+0+2, OUT0
	std Y+4+2, OUT1
	std Y+8+2, OUT2
	std Y+12+2, OUT3
	ldd IN0, Y+0+3
	ldd IN1, Y+4+3
	ldd IN2, Y+8+3
	ldd IN3, Y+12+3
	icall
	std Y+0+3, OUT0
	std Y+4+3, OUT1
	std Y+8+3, OUT2
	std Y+12+3, OUT3
	pop_range 2, 17
	pop r29
	pop r28
	ret
	
.global	inv_sbox128
inv_sbox128:
	ldi r30, lo8(sinvf_tab)
	ldi r31, hi8(sinvf_tab)
	rjmp 1b	
/*	
void inv_sbox128(void * w, uint8_t box){
	uint8_t i, buffer[16];
	box &= 0x7;
	
	sb_fpt fp;
	fp = (sb_fpt)pgm_read_word(&(sinvf_tab[box]));
	for(i=0; i<4; ++i){
		fp(buffer+i, (uint8_t*)w+i);
	}
	memcpy(w, buffer, 16);
}
*/







