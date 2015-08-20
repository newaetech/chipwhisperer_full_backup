/* bigint_adjust.S */
/*
    This file is part of the ARM-Crypto-Lib.
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

/*
void bigint_adjust(bigint_t* a){
	while(a->length_B!=0 && a->wordv[a->length_B-1]==0){
		a->length_B--;
	}
	if(a->length_B==0){
		a->info=0;
		return;
	}
	uint8_t t;
	uint8_t i = 0x07;
	t = a->wordv[a->length_B-1];
	while((t&0x80)==0 && i){
		t<<=1;
		i--;
	}
	SET_FBS(a, i);
}
*/
.global bigint_adjust
bigint_adjust:
	movw r30, r24
	ldd r24, Z+0
	ldd r25, Z+1
	ldd r26, Z+3
	ldd r27, Z+4
	add r26, r24
	adc r27, r25
20:
	sbiw r24, 1
	brmi 30f
	ld r23, -X
	tst r23
	brne 40f
	rjmp 20b
30:
	std Z+0, r1
	std Z+0, r1
	std Z+2, r1
	ret
40:
	adiw r24, 1
	std Z+0, r24
	std Z+1, r25
	clr r24
50:
	sbrc r23, 0
	mov r25, r24
	lsr r23
	inc r24
	brpl 50b
	ldd r23, Z+2
	andi r23, 0xF8
	or r23, r25
	std Z+2, r23
bigint_adjust_ret:
	ret

/*
void bigint_copy(bigint_t* dest, const bigint_t* src){
	memcpy(dest->wordv, src->wordv, src->length_B);
	dest->length_B = src->length_B;
	dest->info = src->info;
}
*/
.global bigint_copy
bigint_copy:
	movw r26, r24
	movw r30, r22
	/* copy length */
	ld r24, Z+
	st X+, r24
	ld r25, Z+
	st X+, r25
	/* copy info */
	ld r22, Z+
	st X+, r22
	/* load wordv pointers */
	ld r22, Z+
	ld r23, Z
	movw r30, r22
	ld r22, X+
	ld r23, X
	movw r26, r22
10:
	sbiw r24, 1
	brmi bigint_adjust_ret
	ld r22, Z+
	st X+, r22
	rjmp 10b

