/* bigint_add_u.S */
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
 * \file     bigint_add_u.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte
 * \date     2010-03-01
 * \license  GPLv3 or later
 *
 */


/*
 param dest: r24:r25
 param a:    r22:r23
 param b:    r20:r21
*/
LEN_A_0 = 22
LEN_A_1 = 23
LEN_B_0 = 20
LEN_B_1 = 21


.global bigint_add_u
bigint_add_u:
	push_range 28, 29
	push_range 24, 25
	movw r26, r24 ; X is our destination pointer
	movw r30, r22 ; Z = a
	movw r28, r20 ; Y = b
	ldd LEN_A_0, Z+0
	ldd LEN_A_1, Z+1
	ldd LEN_B_0, Y+0
	ldd LEN_B_1, Y+1
	cp LEN_A_0, LEN_B_0
	cpc LEN_A_1, LEN_B_1
	brsh 3f
	movw r18, LEN_A_0    ; swap length values
	movw LEN_A_0, LEN_B_0
	movw LEN_B_0, r18
	movw r18, r30        ; swap pointers
	movw r30, r28
	movw r28, r18
3:	; now a is the longer integer
    movw r24, LEN_A_0
    adiw r24, 0
    brne 4f
    st X+, r1 ; store length
    st X+, r1
    st X+, r1 ; store 0 in info field
    rjmp 9f
4:
    adiw r24, 1
    st X+, r24 ; store length
    st X+, r25
    st X+, r1 ; store 0 in info field
    ld r18, X+
    ld r19, X+
    movw r26, r18
    adiw r30, 3 ; adjust pointers to point at wordv
    ld r18, Z+
    ld r19, Z+
    movw r30, r18
    adiw r28, 3
    ld r18, Y+
    ld r19, Y+
    movw r28, r18

    sub LEN_A_0, LEN_B_0
    sbc LEN_A_1, LEN_B_1
    movw r24, LEN_B_0
    clr r0
    adiw r24, 0
    breq 6f
    clc
5:
    ld r0, Z+
    ld r1, Y+
    adc r0, r1
    st X+, r0
    dec r24
	brne 5b
	rol r0 ; store carry bit
	tst r25
	breq 6f
	dec r25
	dec r24
	ror r0 ; write carry back
	rjmp 5b
6: /* the main part is done */
	movw r24, LEN_A_0
	clr r1
	adiw r24, 0
	breq 8f
62:
	ror r0 ; write carry back
7:
    ld r0, Z+
    adc r0, r1
    st X+, r0
    dec r24
	brne 7b
	rol r0 ; store carry bit
	tst r25
	breq 8f
	dec r25
	dec r24
	rjmp 62b
8:
	ror r0
	clr r0
	rol r0
	st X+, r0
9:
	pop_range 24, 25
	pop_range 28, 29
	rjmp bigint_adjust


