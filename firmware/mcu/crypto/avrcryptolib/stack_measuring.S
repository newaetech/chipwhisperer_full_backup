/* stack_measuring.S */
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

#include <avr/io.h>

.extern __brkval

.global stack_measure_init
stack_measure_init:
	movw r30, r24
	lds r20, __brkval
	lds r21, __brkval+1
	in r0, _SFR_IO_ADDR(SREG)
	cli
	in r26, _SFR_IO_ADDR(SPL)
	out _SFR_IO_ADDR(SREG), r0
	in r27, _SFR_IO_ADDR(SPH)
	st Z+, r20
	st Z+, r21
	st Z+, r26
	st Z+, r27
	st Z, r22
	movw r24, r26
	sub r24, r20
	sbc r25, r21
	sbiw r24, 1
	st X, r22
1:  st -X, r22
	sbiw r24, 1
	brne 1b
	ret

.global stack_measure_final
stack_measure_final:
	movw r30, r24
	lds r20, __brkval
	lds r21, __brkval+1
	ld r26, Z+
	ld r27, Z+
	cp r20, r26
	cpc r21, r27
	brlo 1f
	movw r26, r20
1:
	adiw r26, 1
	ld r24, Z+
	ld r25, Z+
	ld r22, Z
	sub r24, r26
	sbc r25, r27
	adiw r24, 2
1:
	sbiw r24, 1
	breq 2f
	ld r20, X+
	cpse r20, r22
2:  ret
	rjmp 1b

/* for testing only
.global stack_measure_usestack
stack_measure_usestack:
	adiw r24, 0
	brne 2f
1:	ret
2:
	in r0, _SFR_IO_ADDR(SREG)
	cli
	in r26, _SFR_IO_ADDR(SPL)
	out _SFR_IO_ADDR(SREG), r0
	in r27, _SFR_IO_ADDR(SPH)
    st X, r22
	sbiw r24, 1
	breq 1b
3:  st -X, r22
	sbiw r24, 1
	breq 1b
	rjmp 3b

	*/
