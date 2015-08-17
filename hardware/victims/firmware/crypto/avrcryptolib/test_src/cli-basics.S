/* cli-basics.S */
/*
    This file is part of the AVR-Huffman.
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

#include "avr-asm-macros.S"

.extern cli_tx
.extern cli_rx
.extern cli_echo

/******************************************************************************/
.global cli_putc
cli_putc:
	lds r30, cli_tx
	lds r31, cli_tx+1
	adiw r30, 0
	breq 2f
	ijmp

/******************************************************************************/
.global cli_getc
cli_getc:
	lds r30, cli_rx
	lds r31, cli_rx+1
	adiw r30, 0
	breq 1f
	ijmp
1:
	ser r24
	ser r25
2:
	ret

/******************************************************************************/
.global cli_getc_cecho
cli_getc_cecho:
	lds r30, cli_rx
	lds r31, cli_rx+1
	adiw r30, 0
	brne 1f
	ser r25
	ser r24
	ret
1:
	icall
	lds r20, cli_echo
	tst r20
	brne 2f
	ret
2:
	push r2
	mov r2, r24
	rcall cli_putc
	mov r24, r2
	clr r25
	pop r2
	ret

/******************************************************************************/
.global cli_putstr
cli_putstr:
	push r28
	push r29
	movw r28, r24
1:
	ld r24, Y+
	clr r25
	tst r24
	breq 2f
	rcall cli_putc
	rjmp 1b
2:
	pop r29
	pop r28
	ret

/******************************************************************************/
.global cli_putstr_P
cli_putstr_P:
	push r28
	push r29
	movw r28, r24
1:
	movw r30, r28
	lpm r24, Z
	clr r25
	tst r24
	breq 2f
	rcall cli_putc
	adiw r28, 1
	rjmp 1b
2:
	pop r29
	pop r28
	ret

/******************************************************************************/
/*
 * param s: r24:r25
 * param n: r22:r23
 */
.global cli_getsn
cli_getsn:
	push r28
	push r29
	push r16
	push r17
	movw r28, r24
	ldi r20, 2
	movw r24, r22
	adiw r24, 0
	breq 9f
1:
	movw r16, r22
2:	clr r20
	rcall cli_getc
	cpi r24, '\r'
	breq 9f
	ldi r20, 1
	tst r24
	breq 9f
	st Y+, r24
	movw r26, r16
	sbiw r26, 1
	movw r16, r26
	brne 2b
9:
	clr r25
	mov r24, r20
	st Y, r1
	pop r17
	pop r16
	pop r29
	pop r28
	ret

/******************************************************************************/
/*
 * param s: r24:r25
 * param n: r22:r23
 */
.global cli_getsn_cecho
cli_getsn_cecho:
	push r28
	push r29
	push r16
	push r17
	movw r28, r24
	ldi r20, 2
	movw r24, r22
	adiw r24, 0
	breq 9b
1:
	movw r16, r22
2:	clr r20
	rcall cli_getc_cecho
	cpi r24, '\r'
	breq 9b
	ldi r20, 1
	tst r24
	breq 9b
	st Y+, r24
	movw r26, r16
	sbiw r26, 1
	movw r16, r26
	brne 2b
	rjmp 9b
