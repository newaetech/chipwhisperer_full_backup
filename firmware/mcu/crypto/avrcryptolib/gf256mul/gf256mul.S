/* gf256mul.S */
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

/* 
 * File:        gf256mul.S
 * Author:      Daniel Otte
 * Date:        2008-12-19
 * License:     GPLv3 or later
 * Description: peasant's algorithm for multiplication in GF(2^8)
 * 
 */

#include <avr/io.h>
#define OPTIMIZE_SMALL_A

/*
 * param a: r24
 * param b: r22
 * param reducer: r20
 */
A = 23
B = 22
P = 24
.global gf256mul

#ifdef OPTIMIZE_SMALL_A
gf256mul:
	mov A, r24
	clr r24
1:	
	lsr A
	breq 4f
	brcc 2f
	eor P, B
2:
	lsl B
	brcc 3f
	eor B, r20	
3:
	rjmp 1b
4:
	brcc 2f
	eor P, B
2:
	ret

#else

gf256mul:
	mov r21, r24
	clr r24
	ldi r25, 8
1:	
	lsr A
	brcc 2f
	eor P, B
2:
	lsl B
	brcc 3f
	eor B, r20	
3:
	dec r25
	brne 1b
	ret

#endif
