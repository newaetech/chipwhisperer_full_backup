/* rotate64.S */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2010 Daniel Otte (daniel.otte@rub.de)

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

.global rotate64_1bit_left
rotate64_1bit_left:
	bst r25, 7
	rol r18
	rol r19
	rol r20
	rol r21
	rol r22
	rol r23
	rol r24
	rol r25
	bld r18, 0
	ret

.global rotate64_1bit_right
rotate64_1bit_right:
	bst r18, 0
	ror r25
	ror r24
	ror r23
	ror r22
	ror r21
	ror r20
	ror r19
	ror r18
	bld r25, 7
	ret

.global rotate64_nbit_autodir
rotate64_nbit_autodir:
	lsr r16
	brcc rotate64_nbit_left
.global rotate64_nbit_right
rotate64_nbit_right:
	ldi r30, pm_lo8(rotate64_1bit_right)
	ldi r31, pm_hi8(rotate64_1bit_right)
	rjmp icall_r16_times
.global rotate64_nbit_left
rotate64_nbit_left:
	ldi r30, pm_lo8(rotate64_1bit_left)
	ldi r31, pm_hi8(rotate64_1bit_left)
icall_r16_times:
1:	dec r16
	brmi 2f
	icall
	rjmp 1b
2:
	pop r16
    ret

rotate64_1byte_left:
	mov r0, r25
	mov r25, r24
	mov r24, r23
	mov r23, r22
	mov r22, r21
	mov r21, r20
	mov r20, r19
	mov r19, r18
	mov r18, r0
	ret

rotate64_2byte_left:
	movw r0, r24
	movw r24, r22
	movw r22, r20
	movw r20, r18
	movw r18, r0
	ret

rotate64_3byte_left:
	mov r0, r25
	mov r25, r22
	mov r22, r19
	mov r19, r24
	mov r24, r21
	mov r21, r18
	mov r18, r23
	mov r23, r20
	mov r20, r0
	ret

rotate64_4byte_left:
	movw r0, r24
	movw r24, r20
	movw r20, r0
	movw r0, r22
	movw r22, r18
	movw r18, r0
	ret

rotate64_5byte_left:
	mov r0, r25
	mov r25, r20
	mov r20, r23
	mov r23, r18
	mov r18, r21
	mov r21, r24
	mov r24, r19
	mov r19, r22
	mov r22, r0
	ret

rotate64_6byte_left:
	movw r0, r18
	movw r18, r20
	movw r20, r22
	movw r22, r24
	movw r24, r0
	ret

rotate64_7byte_left:
	mov r0, r18
	mov r18, r19
	mov r19, r20
	mov r20, r21
	mov r21, r22
	mov r22, r23
	mov r23, r24
	mov r24, r25
	mov r25, r0
	ret


byte_rot_jmp_table:
	ret
	rjmp rotate64_1byte_left
	rjmp rotate64_2byte_left
	rjmp rotate64_3byte_left
	rjmp rotate64_4byte_left
	rjmp rotate64_5byte_left
	rjmp rotate64_6byte_left
	rjmp rotate64_7byte_left

.global rotate64left_code
rotate64left_code:
	ldi r30, pm_lo8(byte_rot_jmp_table)
	ldi r31, pm_hi8(byte_rot_jmp_table)
	push r16
	mov r0, r16
	andi r16, 0x70
	swap r16
	add r30, r16
	adc r31, r1
	mov r16, r0
	andi r16, 0x0f
	icall
	clr r1
	rjmp rotate64_nbit_autodir
