/* string-extras-asm.S */
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
/*
	uint16_t stridentcnt_P(char* a, PGM_P b){
		uint16_t i=0;
		char c;
		for(;;){
			c = pgm_read_byte(b++);
			if(*a != c || c=='\0')
				return i;
			i++;
			a++;
		}
	}
	
	param a: r24:r25
	param b: r22:r23
*/	
.global stridentcnt_P
stridentcnt_P:
	movw r30, r22
	movw r26, r24
	clr r24
	clr r25
1:
	ld r22, X+
	lpm r23, Z+
	cpse r22, r23
2:	ret
	tst r22
	breq 2b
	adiw r24, 1
	rjmp 1b
		

/******************************************************************************/
/*
	uint16_t firstword_length(char* s){
		uint16_t ret=0;
		while(isgraph(*s++))
			ret++;
		return ret; 
	}
	
	param s: r24:r25
*/
.global firstword_length
firstword_length:
	push r28
	push r29
	push r2
	push r3
	movw r2, r24
	movw r28, r24
1:
	clr r25
	ld r24, Y+
	call isgraph
	adiw r24, 0
	brne 1b
	movw r24, r28
	sbiw r24, 1
	sub r24, r2
	sbc r25, r3	
	pop r3
	pop r2
	pop r29
	pop r28
	ret


/******************************************************************************/
/*
	char* strstrip(char* str){
		if(!str)
			return str;
		char* endptr;
		while(*str && (*str==' ' || *str=='\t'))
			++str;
		endptr=str;
		while(*endptr)
			++endptr;
		do{
			--endptr;
		}while(*endptr==' ' || *endptr=='\t');
		endptr[1]='\0';
		return str;
	}
  
  param str: r24:r25
*/
.global strstrip
strstrip:
	movw r26, r24
	adiw r26, 0
	breq 9f
1:
	ld r22, X+
	cpi r22, ' '
	breq 1b
	cpi r22, '\t'
	breq 1b
	sbiw r26, 1
	movw r30, r26
2:
	ld r22, Z+
	tst r22
	brne 2b
3:	
	ld r22, -Z
	cpi r22, ' '
	breq 3b
	cpi r22, '\t'
	breq 3b
	st Z, r1
	movw r24, r26
9:
	ret
