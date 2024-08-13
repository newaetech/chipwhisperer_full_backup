/* cli-hexdump.S */
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
/******************************************************************************/
/* cli_hexdump_byte
 *   param data:   r24
 */
.global cli_hexdump_byte
cli_hexdump_byte:
	push r24
	swap r24
	andi r24, 0x0f
	ldi r30, lo8(hexdigit_tab_P)
	ldi r31, hi8(hexdigit_tab_P)
	add r30, r24
	adc r31, r1
	clr r25
	lpm r24, Z
	rcall cli_putc
	pop r24
	andi r24, 0x0f
	ldi r30, lo8(hexdigit_tab_P)
	ldi r31, hi8(hexdigit_tab_P)
	add r30, r24
	adc r31, r1
	clr r25
	lpm r24, Z
	rcall cli_putc
	ret

/******************************************************************************/
/* cli_hexdump
 *   param data:   r24:r25
 *   param length: r22:r23
 */
.global cli_hexdump
cli_hexdump:
	push r28
	push r29
	push r16
	push r17
	movw r28, r24
	movw r16, r22
	movw r26, r22
	adiw r26, 0
	breq hexdump_exit
2:
	ld r24, Y+
	rcall cli_hexdump_byte
	subi r16, 1
	sbci  r17, 0
	brne 2b
hexdump_exit:
	pop r17
	pop r16
	pop r29
	pop r28
	ret

/******************************************************************************/
/* cli_hexdump_rev
 *   param data:   r24:r25
 *   param length: r22:r23
 */
.global cli_hexdump_rev
cli_hexdump_rev:
	push r28
	push r29
	push r16
	push r17
	movw r28, r24
	movw r16, r22
	add r28, r22
	adc r29, r23
	movw r26, r22
	adiw r26, 0
1:
	breq hexdump_exit
	ld r24, -Y
	rcall cli_hexdump_byte
	subi r16, 1
	sbci r17, 0
	rjmp 1b

/******************************************************************************/
/* cli_hexdump2
 *   param data:   r24:r25
 *   param length: r22:r23
 */
.global cli_hexdump2
cli_hexdump2:
	push r28
	push r29
	push r16
	push r17
	movw r28, r24
	movw r16, r22
	movw r26, r16
	adiw r26, 0
1:
	breq hexdump_exit
	ld r24, Y+
	rcall cli_hexdump_byte
	clr r25
	ldi r24,' '
	rcall cli_putc

	subi r16, 1
	sbci r17, 0
	rjmp 1b

/******************************************************************************/
/* void cli_hexdump_block(const void* data, uint16_t length, uint8_t indent, uint8_t width)
 *   param data:     r24:r25
 *   param length:   r22:r23
 *   param indent:   r20
 *   param width:    r18
 */
WIDTH  =  2
INDENT =  3
DATA_0 = 28
DATA_1 = 29
LENG_0 = 16
LENG_1 = 17

.global cli_hexdump_block
cli_hexdump_block:
	movw r26, r22
	adiw r26, 0
	breq simple_ret
1:
 	push WIDTH
	push INDENT
	push DATA_0
	push DATA_1
	push LENG_0
	push LENG_1
	push r4
	mov WIDTH, r18
	mov INDENT, r20
	movw DATA_0, r24
	movw LENG_0, r22
2:
;	clr r25
	ldi r24, '\r'
	rcall cli_putc
;	clr r25
	ldi r24, '\n'
	rcall cli_putc
	mov r4, INDENT
	tst r4
	breq 4f
3:;	clr r25
	ldi r24, ' '
	rcall cli_putc
	dec r4
	brne 3b
4:
	movw r24, DATA_0
	clr r23
	mov r22, WIDTH
	tst LENG_1
	brne 7f
	cp WIDTH, LENG_0
	brlo 7f
	mov r22, LENG_0
6:	inc r4
7:
	rcall cli_hexdump2
	add DATA_0, WIDTH
	adc DATA_1, r1
	sub LENG_0, WIDTH
	sbc LENG_1, r1
	tst r4
	breq 2b
	pop r4
	pop LENG_1
	pop LENG_0
	pop DATA_1
	pop DATA_0
	pop INDENT
	pop WIDTH
simple_ret:
	ret


