/* cscipher_tiny_asm.S */
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

#include "avr-asm-macros.S"

/*
uint8_t p(uint8_t a){
	a ^= pgm_read_byte(fg_table+(a&0xf))&0xf0;
	a ^= pgm_read_byte(fg_table+(a>>4)) &0x0f;
	a ^= pgm_read_byte(fg_table+(a&0xf))&0xf0;
	return a;
}
*/

fg_table:
.byte	0xfa, 0xd6, 0xb0, 0xb2, 0x7b, 0x5e, 0x71, 0x78
.byte	0xed, 0xd4, 0xa5, 0xb3, 0xef, 0xdc, 0xe7, 0xf9

.global p
p:
	ldi r30, lo8(fg_table)
	ldi r31, hi8(fg_table)
	movw r26, r30
	mov r25, r24
	andi r25, 0x0F
	add r30, r25
	adc r31, r1
	lpm r25, Z
	andi r25, 0xF0
	eor r24, r25

	movw r30, r26
	mov r25, r24
	swap r25
	andi r25, 0x0F
	add r30, r25
	adc r31, r1
	lpm r25, Z
	andi r25, 0x0F
	eor r24, r25

	movw r30, r26
	mov r25, r24
	andi r25, 0x0F
	add r30, r25
	adc r31, r1
	lpm r25, Z
	andi r25, 0xF0
	eor r24, r25
	clr r25
	ret

ks_const:
.byte	0x29,0x0d,0x61,0x40,0x9c,0xeb,0x9e,0x8f
.byte	0x1f,0x85,0x5f,0x58,0x5b,0x01,0x39,0x86
.byte	0x97,0x2e,0xd7,0xd6,0x35,0xae,0x17,0x16
.byte	0x21,0xb6,0x69,0x4e,0xa5,0x72,0x87,0x08
.byte	0x3c,0x18,0xe6,0xe7,0xfa,0xad,0xb8,0x89
.byte	0xb7,0x00,0xf7,0x6f,0x73,0x84,0x11,0x63
.byte	0x3f,0x96,0x7f,0x6e,0xbf,0x14,0x9d,0xac
.byte	0xa4,0x0e,0x7e,0xf6,0x20,0x4a,0x62,0x30
.byte	0x03,0xc5,0x4b,0x5a,0x46,0xa3,0x44,0x65

CTX_0 = 18
CTX_1 = 19
CNT   = 17
.global cscipher_init
cscipher_init:
	push CNT
	push_range 28, 29
	stack_alloc 24, 28, 29
	adiw r28, 1
	movw r30, r24
	movw CTX_0, r22
	/* copy key to local tmp_key */
	ldi r22, 16
10: ld r23, Z+
	st Y+, r23
	dec r22
	brne 10b
	sbiw r28, 16
	ldi CNT, 0xff
10: /* main loop */
	inc CNT
	/* copy part of tmp_key to tmp */
	ldi r23, 8
11:	ldd r22, Y+0
	sbrc CNT, 0
	ldd r22, Y+8
	std Y+16, r22
	adiw r28, 1
	dec r23
	brne 11b
	adiw r28, 8 /* Y points at tmp */
	/* xor ks constant into tmp */
	movw r24, r28
	ldi r22, lo8(ks_const)
	ldi r23, hi8(ks_const)
	mov r21, CNT
	swap r21
	lsr r21
	add r22, r21
	adc r23, r1
	clr r21
	ldi r20, 8
	call memxor_P
	/* do P transformation */
	ldi r22, 8
20:	ld r24, Y
	rcall p
	st Y+, r24
	dec r22
	brne 20b
	sbiw r28, 8 /* Y points at tmp */
	movw r26, r28
	sbiw r26, 8
	sbrc CNT, 0
	sbiw r26, 8
	/* do T transformation */
	movw r30, CTX_0
	ldi r22, 8
30:	ldi r23, 8
35:	ld r24, Y
	rol r24
	rol r21
	st Y+, r24
	dec r23
	brne 35b
	sbiw r28, 8 /* Y points at tmp */
	ld r24, X
	eor r21, r24
	st X+, r21
	st Z+, r21
	dec r22
	brne 30b
	sbiw r28, 16 /* Y points at tmp_key (again) */
	movw CTX_0, r30
	sbrs CNT, 3
	rjmp 10b
	stack_free 24
	pop_range 28, 29
	pop CNT
	ret


round_const:
.byte	0xb7, 0xe1, 0x51, 0x62, 0x8a, 0xed, 0x2a, 0x6a
.byte	0xbf, 0x71, 0x58, 0x80, 0x9c, 0xf4, 0xf3, 0xc7

/*
void cscipher_enc(void* buffer, const cscipher_ctx_t* ctx){
	uint8_t i,j,k;
	uint8_t tmp[8];
	for(i=0; i<8; ++i){
		for(j=0; j<3; ++j){
			if(j==0){
				memxor(buffer, ctx->keys[i], 8);
			}else{
				memxor_P(buffer, round_const+((j==1)?0:8), 8);
			}
			for(k=0; k<4; ++k){
				((uint16_t*)tmp)[k] = m(((uint16_t*)buffer)[k]);
			}
			for(k=0; k<4; ++k){
				((uint8_t*)buffer)[k]   = tmp[2*k];
				((uint8_t*)buffer)[k+4] = tmp[2*k+1];
			}
		}
	}
	memxor(buffer, ctx->keys[8], 8);
}
*/
TMP_0 =  2
TMP_1 =  3
TMP_2 =  4
TMP_3 =  5
TMP_4 =  6
TMP_5 =  7
TMP_6 =  8
TMP_7 =  9
CTX_0 = 10
CTX_1 = 11
CNT_0 = 16
CNT_1 = 17
DST_0 = 12
DST_1 = 13
SRC_0 = 14
SRC_1 = 15
.global cscipher_enc
cscipher_enc:
	push_range 2, 17
	push_range 28, 29
	movw r28, r24
	movw CTX_0, r22
	ldi CNT_0, 8
	/* main loop */
10: ldi CNT_1, 2
	clt
	/* sub loop */
20: ldi r27, 0
	ldi r26, TMP_0
	movw DST_0, r26
	ldi r30, lo8(round_const)
	ldi r31, hi8(round_const)
	sbrs CNT_1, 0
	adiw r30, 8
	sbrc CNT_1, 1
	movw r30, CTX_0
	movw SRC_0, r30
	ldi r21, 4
	/* xor and m transformation */
25:	ld r24, Y+
	ld r25, Y+
	movw r30, SRC_0
	brts 30f
	ld r22, Z+
	ld r23, Z+
	rjmp 35f
30:	lpm r22, Z+
	lpm r23, Z+
35:
	movw SRC_0, r30
	eor r24, r22
	eor r25, r23

	movw r22, r24
	mov r25, r22
	rol r25
	adc r25, r1
	mov r22, r25
	andi r22, 0x55
	eor r22, r24
	eor r22, r23
	eor r23, r25
	mov r24, r23
	rcall p
	mov r23, r24
	mov r24, r22
	rcall p

	movw r26, DST_0
	st X+, r24
	st X+, r23
	movw DST_0, r26
	dec r21
	brne 25b
	sbrc CNT_1, 1
	movw CTX_0, SRC_0
	sbiw r28, 8
	std Y+0, TMP_0
	std Y+4, TMP_1
	std Y+1, TMP_2
	std Y+5, TMP_3
	std Y+2, TMP_4
	std Y+6, TMP_5
	std Y+3, TMP_6
	std Y+7, TMP_7
	set
	dec CNT_1
	brpl 20b

	dec CNT_0
	brne 10b

	movw r24, r28
	movw r22, CTX_0
	clr r21
	ldi r20, 8

	pop_range 28, 29
	pop_range 2, 17
	rjmp memxor

/*
void cscipher_dec(void* buffer, const cscipher_ctx_t* ctx){
	uint8_t i=7,j,k;
	uint8_t tmp[8];
	memxor(buffer, ctx->keys[8], 8);
	do{
		for(j=0; j<3; ++j){
			for(k=0; k<4; ++k){
				tmp[2*k]   = ((uint8_t*)buffer)[k];
				tmp[2*k+1] = ((uint8_t*)buffer)[4+k];
			}
			for(k=0; k<4; ++k){
				((uint16_t*)buffer)[k] = m_inv(((uint16_t*)tmp)[k]);
			}
			if(j==2){
				memxor(buffer, ctx->keys[i], 8);
			}else{
				memxor_P(buffer, round_const+((j==1)?0:8), 8);
			}

		}
	}while(i--);
}

*/
.global cscipher_dec
cscipher_dec:
	push_range 2, 17
	push_range 28, 29
	movw r28, r24
	movw r26, r22
	adiw r26, 7*8
	adiw r26, 8
	movw CTX_0, r26
	movw r22, r26
	clr r21
	ldi r20, 8
	call memxor
	ldi CNT_0, 7
10:
	ldi CNT_1, 3
20:
	clr r27
	ldi r26, TMP_0
	movw DST_0, r26
	ldi r21, 4
30:
	ldd r23, Y+4
	ld  r24, Y+
/* m_inv transformation */
;	mov r23, r25
	rcall p
	mov r22, r24
	mov r24, r23
	rcall p
	eor r22, r24
	mov r25, r24
	mov r24, r22
	rol r24
	adc r24, r1
	andi r24, 0xaa
	eor r24, r22
	mov r22, r24
	rol r22
	adc r22, r1
	eor r25, r22

	movw r26, DST_0
	st X+, r24
	st X+, r25
	movw DST_0, r26
	dec r21
	brne 30b
	sbiw r28, 4
	std Y+0, TMP_0
	std Y+1, TMP_1
	std Y+2, TMP_2
	std Y+3, TMP_3
	std Y+4, TMP_4
	std Y+5, TMP_5
	std Y+6, TMP_6
	std Y+7, TMP_7
	movw r24, r28
	clr r21
	ldi r20, 8
	sbrc CNT_1, 1
	rjmp 40f
	movw r26, CTX_0
	sbiw r26, 8
	movw CTX_0, r26
	movw r22, r26
	call memxor
	rjmp 45f
40:
	ldi r26, lo8(round_const)
	ldi r27, hi8(round_const)
	sbrc CNT_1, 0
	adiw r26, 8
	movw r22, r26
	call memxor_P
45:

	dec CNT_1
	brne 20b
	dec CNT_0
	brpl 10b
90:
	pop_range 28, 29
	pop_range 2, 17
	ret
