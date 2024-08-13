/* bigint_asm.S */
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

#include "avr-asm-macros.S"
#include "bigint_adjust.S"
#include "bigint_add_u.S"


/******************************************************************************/
/*
void bigint_add_scale_u(bigint_t* dest, const bigint_t* a, uint16_t scale){
	uint16_t i,j=0;
	uint16_t t=0;
	if(scale>dest->length_B)
		memset(dest->wordv+dest->length_B, 0, scale-dest->length_B);
	for(i=scale; i<a->length_B+scale; ++i,++j){
		t = a->wordv[j] + t;
		if(dest->length_B>i){
			t += dest->wordv[i];
		}
		dest->wordv[i] = (uint8_t)t;
		t>>=8;
	}
	while(t){
		if(dest->length_B>i){
			t = dest->wordv[i] + t;
		}
		dest->wordv[i] = (uint8_t)t;
		t>>=8;
		++i;
	}
	if(dest->length_B < i){
		dest->length_B = i;
	}
	bigint_adjust(dest);
}
*/

DST_SIZE_0 = 22
DST_SIZE_1 = 23
SRC_SIZE_0 = 20
SRC_SIZE_1 = 23
SCALE_0    = 18
SCALE_1    = 19
DST_CTX_0  =  6
DST_CTX_1  =  7
SRC_CTX_0  =  8
SRC_CTX_1  =  9
TMP_0      = 10
TMP_1      = 11

.global bigint_add_scale_u


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

DST_LEN_0 = 22
DST_LEN_1 = 23
SRC_LEN_0 = 20
SRC_LEN_1 = 21
SCALE_0   = 18
SCALE_1   = 19
DST_CTX_0 =  8
DST_CTX_1 =  9
TMP_0     = 10
TMP_1     = 11

bigint_add_scale_u:
	movw r30, r24 /* dest ptr */
	movw r26, r22 /* src ptr */
	movw r24, r20 /* scale */
	/* check if scale is zero */
	movw SCALE_0, r24
	adiw r24, 0
	brne 10f
	movw r24, r30
	movw r20, r30
	rjmp bigint_add_u
10:	/* check if src is zero */
	ld r24, X+
	ld r25, X+
	adiw r24, 0
	brne 10f
	ret
10:
	movw SRC_LEN_0, r24
	push_range 8, 11
	movw DST_CTX_0, r30

	/* pad dest with zeros to length of SRC_LENGTH + scale */
	adiw r26, 1
	ld TMP_0, X+
	ld TMP_1, X+
	movw r26, TMP_0 /* move SRC_WORDV to X */
	ldd DST_LEN_0, Z+0
	ldd DST_LEN_1, Z+1
    ldd TMP_0, Z+3
    ldd TMP_1, Z+4
    movw r30, TMP_0 /* move DEST_WORDV to Z */
	movw TMP_0, SCALE_0
	sub TMP_0, DST_LEN_0
	sbc TMP_1, DST_LEN_1
	movw r24, TMP_0
	brmi 40f /* no padding needed since DST_LEN > scale */
	add r30, DST_LEN_0 /* add DST_LEN to Z (DEST_WORDV)*/
	adc r31, DST_LEN_1
	/* pad and copy src in front of dest */
10: /* padding loop */
	sbiw r24, 1
	brmi 11f
	st Z+, r1
	rjmp 10b
11:
	/* start of copy */
	movw r24, SRC_LEN_0

12: /* copy loop */
	sbiw r24, 1
	brmi 13f
	ld TMP_0, X+
	st Z+, TMP_0
	rjmp 12b
13:
	movw TMP_0, SCALE_0
	add TMP_0, SRC_LEN_0
	adc TMP_1, SRC_LEN_1
	movw r30, DST_CTX_0
	std Z+0, TMP_0
	std Z+1, TMP_1
	movw r24, r30
99:
	pop_range 8, 11
	rjmp bigint_adjust
40:
    /* Z points at DST_WORDV */
    /* X points at SRC_WORDV */
    /* r24:r25 and TMP contain scale - DST_LEN (negativ) */
    /* set T bit if DST_LEN > SCR_LEN + scale */
    clt
    add r30, SCALE_0
    adc r31, SCALE_1
    add TMP_0, SRC_LEN_0
    adc TMP_1, SRC_LEN_1
	brpl 41f
	set
	/* DST_LEN > SRC_LEN + scale && DST_LEN > scale */
	/*
	       +-------+-------+ SRC + scale
	   +------+------------+ DST
	*/
	movw r24, SRC_LEN_0
	rjmp 44f
41:
	/* DST_LEN <= SRC_LEN + scale && DST_LEN > scale */
	/*
	       +-------+-------+ SRC + scale
	          +------------+ DST
	*/
	com r24 /* negate r24:r25 ==> DST_LEN - scale */
	com r25
	adiw r24, 1
	breq 50f
44:
	inc r25
	clc
45:
46:	ld TMP_0, X+
	ld TMP_1, Z
	adc TMP_0, TMP_1
	st Z+, TMP_0
	dec r24
	brne 46b
	dec r25
	brne 46b

50: ;st Z, r1
	brtc 60f
51:	brcc 53f
52:	ld TMP_0, Z
	adc TMP_0, r1
	st Z+, TMP_0
	brcs 52b
53:
    /* epilogue */
    movw r24, r30
    movw r30, DST_CTX_0
    ldd TMP_0, Z+3
    ldd TMP_1, Z+4
    sub r24, TMP_0
    sbc r25, TMP_1
    cp  r24, DST_LEN_0
    cpc r25, DST_LEN_1
    brmi 54f
    std Z+0, r24
    std Z+1, r25
54: movw r24, r30
    rjmp 99b

60: st Z, r1
	rol r1 /* backup carry */
	movw r24, SRC_LEN_0
    add r24, SCALE_0
    adc r25, SCALE_1
    sub r24, DST_LEN_0
    sbc r25, DST_LEN_1

	adiw r24, 0
	breq 63f
	inc r25
    ror r1 /* restore carry */

61:
62: ld TMP_0, X+
    adc TMP_0, r1
    st Z+, TMP_0
    dec r24
    brne 62b
    dec r25
    brne 62b
63:
	brcc 53b
	ldi r24, 1
	st Z+, r24
	rjmp 53b
