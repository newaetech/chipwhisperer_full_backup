/* dump-asm.S */
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
#include <avr/io.h>
#include <avr/sfr_defs.h>
#define DUMP_WIDTH 16

#define ADDR_BASE 16
/******************************************************************************/
/* uint8_t charisinstr_P(char c, PGM_P str)
 *   param c:    r24
 *   param str:  r22:r23
 */
charisinstr_P:
	movw r30, r22
1:
	lpm r22, Z+
	tst r22
	brne 2f
	clr r24
	ret
2:
	cp r22, r24
	brne 1b
	ldi r24, 1
	ret
	
/******************************************************************************/
/* void dump_chars(uint8_t* buffer, uint8_t len)
 *   param buffer:  r24:r25
 *   param len:     r22
 */
dump_chars:
	push r16
	push r17
	push r28
	push r29
	movw r28, r24
	mov r16, r22
	ldi r24, '|'
	clr r25
	call cli_putc
	mov r17, r16
1:
	ld r24, Y
	clr	r25
	call isprint
	tst r24
	brne 2f
	ldi r24, '.'
	adiw r28, 1
	rjmp 3f
2:	ld r24, Y+	
3:	
	clr r25
	call cli_putc
	dec r17
	brne 1b
	subi r16, DUMP_WIDTH
	tst r16
	breq 5f
	neg r16
4:	
	ldi r24, ' '
	clr r25
	call cli_putc
	dec r16
	brne 4b
5:
	ldi r24, '|'
	clr r25
	call cli_putc	
	pop r29
	pop r28
	pop r17
	pop r16
	ret
 
 /*****************************************************************************/
 /* void print_aligned(unsigned long value, uint8_t align)
  *    param value: r22:r25
  *    param align: r20
  */
print_aligned:
	push r16
	push r28
	push r29
	stack_alloc 12, r28, r29
	adiw r28, 1
	mov r16, r20
	movw r20, r28
	ldi  r18, ADDR_BASE
	clr  r19
	call ultoa
	movw r24, r28
	call strlen
	sub r16, r24
	brmi 3f
	breq 3f
1:
	ldi r24, ' '
	clr r25
	call cli_putc
	dec r16
	brne 1b
3:	
	movw r24, r28
	call cli_putstr
	stack_free 12
	pop r29
	pop r28
	pop r16
	ret 

 /*****************************************************************************/
 /* void dump(char* s)
  *    param s: r24:r25
  */

STR_0    =  4
STR_1    =  5
ADDR_0   =  6
ADDR_1   =  7
ADDR_2   =  8
ADDR_3   =  9
SIZE_0   = 10
SIZE_1   = 11
SIZE_2   = 12
SIZE_3   = 13
MEM_0    = 14
MEM_1    = 15
xREADLEN = 16
TMP      = 17

.global dump
dump:
	call strstrip
	push_range 4, 17
	push_range 28, 29
	movw r26, r24
	ld TMP, X
	movw STR_0, r26
	ldi r30, lo8(memtype_desc)
	ldi r31, hi8(memtype_desc)
1:
	lpm r22, Z+
	lpm r23, Z+
	tst r22
	brne 2f
	tst r23
	breq 5f
2:
	movw r28, r30
	mov r24, TMP
	clr r25
	rcall charisinstr_P
	movw r30, r28
	tst r24
	brne 6f
	adiw r30, 4
	rjmp 1b
5:
	ldi r30, lo8(memtype_desc+2)
	ldi r31, hi8(memtype_desc+2)
6:	
	movw MEM_0, r30
	movw r26, STR_0
61:
	ld r20, X+
	ori r20, 'A'^'a'
	cpi r20, 'a'
	brmi 7f
	cpi r20, 'z'+1
	brge 7f
	rjmp 61b
7:
	sbiw r26, 1
	stack_alloc 2+DUMP_WIDTH, r28, r29
	adiw r28, 1
	movw r24, r26
	movw r22, r28
	clr r20
	clr r21
	call strtoul
	movw ADDR_0, r22
	movw ADDR_2, r24
	ld r24, Y+
	ld r25, Y+
	clr r23
	clr r22
	clr r21
	clr r20
	call strtoul
	movw SIZE_0, r22
	movw SIZE_2, r24
	tst SIZE_0
	brne 72f
	tst SIZE_1
	brne 72f
	tst SIZE_2
	brne 72f
	tst SIZE_3
	brne 72f
	ldi TMP, 128
	mov SIZE_0, TMP
72:	
	ldi r24, lo8(dumping)
	ldi r25, hi8(dumping)
	call cli_putstr_P
	movw r24, SIZE_2
	movw r22, SIZE_0
	movw r20, r28
	clr r19
	ldi r18, 10
	call ultoa
	movw r24, r28
	call cli_putstr
	ldi r24, lo8(bytesof)
	ldi r25, hi8(bytesof)
	call cli_putstr_P
	movw r30, MEM_0
	lpm r24, Z+
	lpm r25, Z+
	call cli_putstr_P	
	ldi r24, lo8(beginning)
	ldi r25, hi8(beginning)
	call cli_putstr_P
	movw r24, ADDR_2
	movw r22, ADDR_0
	movw r20, r28
	clr r19
	ldi r18, 16
	call ultoa
	movw r24, r28
	call cli_putstr
	ldi r24, lo8(suffix)
	ldi r25, hi8(suffix)
	call cli_putstr_P
	
	movw r30, MEM_0
	adiw r30, 2
	lpm MEM_0, Z+
	lpm MEM_1, Z
	
1:	
	ldi xREADLEN, DUMP_WIDTH
	tst SIZE_3
	brne 3f
	tst SIZE_2
	brne 3f
	tst SIZE_1
	brne 3f
	tst SIZE_0
	brne 2f
	rjmp 90f
2:	
	cp DUMP_WIDTH, SIZE_0
	brmi 3f
	mov xREADLEN, SIZE_0
3:	
	movw r24, r28
	movw r22, ADDR_2
	movw r20, ADDR_0
	clr r19
	mov r18, xREADLEN
	movw r30, MEM_0
	icall
	movw r24, ADDR_2
	movw r22, ADDR_0
	clr r21
	ldi r20, 6
	rcall print_aligned	
	clr r25
	ldi r24, ':'
	call cli_putc
	clr r25
	ldi r24, ' '
	call cli_putc
	movw r24, r28
	clr r23
	mov r22, xREADLEN
	call cli_hexdump2
	ldi TMP, DUMP_WIDTH
	sub TMP, xREADLEN
	tst TMP
	breq 5f
4:
	clr r25
	ldi r24, ' '
	call cli_putc
	clr r25
	ldi r24, ' '
	call cli_putc
	clr r25
	ldi r24, ' '
	call cli_putc
	dec TMP
	brne 4b
5:
	clr r25
	ldi r24, '\t'
	call cli_putc
	movw r24, r28
	clr r23
	mov r22, xREADLEN
	rcall dump_chars
	add ADDR_0, xREADLEN
	adc ADDR_1, r1
	adc ADDR_2, r1
	adc ADDR_3, r1
	sub SIZE_0, xREADLEN
	sbc SIZE_1, r1
	sbc SIZE_2, r1
	sbc SIZE_3, r1
	clr r25
	ldi r24, '\r'
	call cli_putc
	clr r25
	ldi r24, '\n'
	call cli_putc
	rjmp 1b
90:	
	stack_free 2+DUMP_WIDTH
	
	pop_range 28,29
	pop_range 4, 17
	ret
	 
dumping:	 
.asciz "\r\ndumping "
bytesof:
.asciz " bytes of "
beginning:
.asciz ", beginnig at 0x"
suffix:
.asciz ":\r\n"
.byte 0

/******************************************************************************/
/* void pgm_read_block(void* buffer, uint32_t addr, uint8_t length)
 *   param buffer:  r24:r25
 *   param addr:    r20:r23
 *   param length:  r18
 */
.global pgm_read_block
pgm_read_block:
#if RAMEND<0x10000
	movw r26, r24
	movw r30, r20
	tst r18
	breq 3f
1:
	lpm r20, Z+
	st X+, r20
	dec r18
	brne 1b
3:	
	ret
#else
	movw r26, r24
	movw r30, r20
	out RAMPZ, r22
	tst r18
	breq 3f
1:
	elpm r20, Z+
	st X+, r20
	dec r18
	brne 1b
3:	
	ret

#endif	
	
/******************************************************************************/
/* void ram_read_block(void* buffer, uint32_t addr, uint8_t length)
 *   param buffer:  r24:r25
 *   param addr:    r20:r23
 *   param length:  r18
 */
.global ram_read_block
ram_read_block:
	movw r26, r24
	movw r30, r20
	tst r18
	breq 3f
1:
	ld r20, Z+
	st X+, r20
	dec r18
	brne 1b
3:	
	ret

/******************************************************************************/
/* void ee_read_block(void* buffer, uint32_t addr, uint8_t length)
 *   param buffer:  r24:r25
 *   param addr:    r20:r23
 *   param length:  r18
 */
#ifdef EEWE
# define EEPE EEWE
#endif

.global ee_read_block
ee_read_block:
	movw r26, r24
	movw r30, r20
	tst r18
	breq 3f
1:
	sbic _SFR_IO_ADDR(EECR), EEPE
	rjmp 1b
	out _SFR_IO_ADDR(EEARH), r31
	out _SFR_IO_ADDR(EEARL), r30
	sbi _SFR_IO_ADDR(EECR), EERE
	adiw r30, 1
	in r20, _SFR_IO_ADDR(EEDR)
	st X+, r20
	dec r18
	brne 1b
3:	
	ret
