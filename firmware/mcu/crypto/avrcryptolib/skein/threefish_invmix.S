/* threefish_invmix.S */
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
 * \date    2009-03-21
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

/*
#define X0 (((uint64_t*)data)[0])
#define X1 (((uint64_t*)data)[1])
void threefish_invmix(void* data, uint8_t rot){
	uint64_t x;
	x = X1;
	x ^= X0;
	X1 = ((x>>rot)|(x<<(64-rot)));
	X0 -= X1;
}
*/
A0 = 10
A1 = 11
A2 = 12
A3 = 13
A4 = 14
A5 = 15
A6 = 16
A7 = 17

B0 = 18
B1 = 19
B2 = 20
B3 = 21
B4 = 22
B5 = 23
B6 = 24
B7 = 25
vROT = 27
/*
 * param data:  r24:r25
 * param rot:   r22
 */

.global threefish_invmix_asm
threefish_invmix_asm:
	movw r28, r24
	mov vROT,r22
	ldd A0, Y+ 0
	ldd A1, Y+ 1
	ldd A2, Y+ 2
	ldd A3, Y+ 3
	ldd A4, Y+ 4
	ldd A5, Y+ 5
	ldd A6, Y+ 6
	ldd A7, Y+ 7
	ldd B0, Y+ 8
	ldd B1, Y+ 9
	ldd B2, Y+10
	ldd B3, Y+11
	ldd B4, Y+12
	ldd B5, Y+13
	ldd B6, Y+14
	ldd B7, Y+15
	eor B0, A0
	eor B1, A1
	eor B2, A2
	eor B3, A3
	eor B4, A4
	eor B5, A5
	eor B6, A6
	eor B7, A7

	mov r26, vROT
	swap r26
	andi r26, 0x07
	ldi r30, pm_lo8(byte_rot_jmptable)
	ldi r31, pm_hi8(byte_rot_jmptable)
	add r30, r26
	adc r31, r1
	ijmp
post_byterot:
	bst vROT, 3
	andi vROT, 0x07
	brts 1f
	rjmp bit_rotr
1:	rjmp bit_rotl
post_bitrot:	
	sub A0, B0
	sbc A1, B1
	sbc A2, B2
	sbc A3, B3
	sbc A4, B4
	sbc A5, B5
	sbc A6, B6
	sbc A7, B7

	std Y+ 0, A0
	std Y+ 1, A1
	std Y+ 2, A2
	std Y+ 3, A3
	std Y+ 4, A4
	std Y+ 5, A5
	std Y+ 6, A6
	std Y+ 7, A7
	std Y+ 8, B0
	std Y+ 9, B1
	std Y+10, B2
	std Y+11, B3
	std Y+12, B4
	std Y+13, B5
	std Y+14, B6
	std Y+15, B7
exit:
	ret

byte_rot_jmptable:
	rjmp  post_byterot;ret; rjmp  byte_rotr_0
	rjmp  byte_rotr_1
	rjmp  byte_rotr_2
	rjmp  byte_rotr_3
	rjmp  byte_rotr_4
	rjmp  byte_rotr_5
	rjmp  byte_rotr_6
	rjmp  byte_rotr_7
	rjmp  post_byterot;ret; rjmp  byte_rotr_0
	

	
; 0 1 2 3 4 5 6 7
; 1 2 3 4 5 6 7 0
;.global byte_rotr_1
;.global byte_rotr_0
byte_rotr_1: /* 10 words */
	mov r0, B0
	mov B0, B1
	mov B1, B2
	mov B2, B3
	mov B3, B4
	mov B4, B5
	mov B5, B6
	mov B6, B7
	mov B7, r0
byte_rotr_0:	
	rjmp post_byterot

; 0 1 2 3 4 5 6 7	
; 2 3 4 5 6 7 0 1
;.global byte_rotr_2	
byte_rotr_2: /* 11 words */
	mov r0, B0
	mov B0, B2
	mov B2, B4
	mov B4, B6
	mov B6, r0
	mov r0, B1
	mov B1, B3
	mov B3, B5
	mov B5, B7
	mov B7, r0
	rjmp post_byterot

; 0 1 2 3 4 5 6 7
; 3 4 5 6 7 0 1 2
;.global byte_rotr_3
byte_rotr_3: /* 10 words */
	mov r0, B0
	mov B0, B3
	mov B3, B6
	mov B6, B1
	mov B1, B4
	mov B4, B7
	mov B7, B2
	mov B2, B5
	mov B5, r0
	rjmp post_byterot

; 0 1 2 3 4 5 6 7
; 4 5 6 7 0 1 2 3
;.global byte_rotr_4
byte_rotr_4: /* 13 words */
	mov r0, B0
	mov B0, B4
	mov B4, r0
	
	mov r0, B1
	mov B1, B5
	mov B5, r0
	
	mov r0, B2
	mov B2, B6
	mov B6, r0
	
	mov r0, B3
	mov B3, B7
	mov B7, r0
	rjmp post_byterot

; 0 1 2 3 4 5 6 7
; 5 6 7 0 1 2 3 4
;.global byte_rotr_5
byte_rotr_5: /* 10 words */
	mov r0, B0
	mov B0, B5
	mov B5, B2
	mov B2, B7
	mov B7, B4
	mov B4, B1
	mov B1, B6
	mov B6, B3
	mov B3, r0
	rjmp post_byterot
	
; 0 1 2 3 4 5 6 7
; 6 7 0 1 2 3 4 5
;.global byte_rotr_6
byte_rotr_6: /* 11 words */
	mov r0, B0
	mov B0, B6
	mov B6, B4
	mov B4, B2
	mov B2, r0
	
	mov r0, B1
	mov B1, B7
	mov B7, B5
	mov B5, B3
	mov B3, r0
	rjmp post_byterot

; 0 1 2 3 4 5 6 7
; 7 0 1 2 3 4 5 6
;.global byte_rotr_7
byte_rotr_7: /* 10 words */
	mov r0, B7
	mov B7, B6
	mov B6, B5
	mov B5, B4
	mov B4, B3
	mov B3, B2
	mov B2, B1
	mov B1, B0
	mov B0, r0
	rjmp post_byterot

;.global bit_rotl
bit_rotl:
	tst vROT
	brne 1f
	rjmp post_bitrot
1:	mov r0, B7
	rol r0
	rol B0
	rol B1
	rol B2
	rol B3
	rol B4
	rol B5
	rol B6
	rol B7
	dec vROT
	rjmp bit_rotl
	
;.global bit_rotr
bit_rotr:
	tst vROT
	brne 1f
	rjmp post_bitrot
1:	mov r0, B0
	ror r0
	ror B7
	ror B6
	ror B5
	ror B4
	ror B3
	ror B2
	ror B1
	ror B0
	dec vROT
	rjmp bit_rotr


