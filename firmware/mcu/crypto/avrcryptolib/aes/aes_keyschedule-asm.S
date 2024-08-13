/* aes_keyschedule-asm */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008, 2009  Daniel Otte (daniel.otte@rub.de)

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
 * \file     aes_keyschedule-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-01-09
 * \license  GPLv3 or later
 * 
 */

#include "avr-asm-macros.S"

.global aes256_init
aes256_init:
	movw r20, r22
	ldi r23, hi8(256)
	ldi r22, lo8(256)
	rjmp aes_init
	
.global aes192_init
aes192_init:
	movw r20, r22
	ldi r23, hi8(192)
	ldi r22, lo8(192)
	rjmp aes_init
	
.global aes128_init
aes128_init:
	movw r20, r22
	clr r23
	ldi r22, 128

/* 
void aes_init(const void* key, uint16_t keysize_b, aes_genctx_t* ctx){
	uint8_t hi,i,nk, next_nk;
	uint8_t rc=1;
	uint8_t tmp[4];
	nk=keysize_b>>5; / * 4, 6, 8 * /
	hi=4*(nk+6+1);
	memcpy(ctx, key, keysize_b/8);
	next_nk = nk;
	for(i=nk;i<hi;++i){
		*((uint32_t*)tmp) = ((uint32_t*)(ctx->key[0].ks))[i-1];
		if(i!=next_nk){
			if(nk==8 && i%8==4){
				tmp[0] = pgm_read_byte(aes_sbox+tmp[0]);
				tmp[1] = pgm_read_byte(aes_sbox+tmp[1]);
				tmp[2] = pgm_read_byte(aes_sbox+tmp[2]);
				tmp[3] = pgm_read_byte(aes_sbox+tmp[3]);
			}
		} else {
			next_nk += nk;
			aes_rotword(tmp);
			tmp[0] = pgm_read_byte(aes_sbox+tmp[0]);
			tmp[1] = pgm_read_byte(aes_sbox+tmp[1]);
			tmp[2] = pgm_read_byte(aes_sbox+tmp[2]);
			tmp[3] = pgm_read_byte(aes_sbox+tmp[3]);
			tmp[0] ^= rc;
			rc<<=1;
		}
		((uint32_t*)(ctx->key[0].ks))[i] = ((uint32_t*)(ctx->key[0].ks))[i-nk]
		                                   ^ *((uint32_t*)tmp);
	}
}
*/

SBOX_SAVE0 = 14
SBOX_SAVE1 = 15
XRC = 17
NK = 22
C1 = 18
NEXT_NK = 19
HI = 23
T0 = 20
T1 = 21
T2 = 24
T3 = 25
/*
 * param key:       r24:r25
 * param keysize_b: r22:r23
 * param ctx:       r20:r21
 */
.global aes_init
aes_init:
	push_range 14, 17
	push r28
	push r29
	movw r30, r20
	movw r28, r20
	movw r26, r24
	lsr r23
	ror r22
	lsr r22
	lsr r22 /* r22 contains keysize_b/8 */
	mov C1, r22

1:	/* copy key to ctx */ 
	ld r0, X+
	st Z+, r0
	dec C1
	brne 1b
	
	lsr NK
	lsr NK
	bst NK,3 /* set T if NK==8 */
	mov NEXT_NK, NK
	mov HI, NK
	subi HI, -7
	lsl HI
	lsl HI
	movw r26, r30
	sbiw r26, 4
	mov C1, NK
	ldi r30, lo8(aes_sbox)
	ldi r31, hi8(aes_sbox)
	movw SBOX_SAVE0, r30
	ldi XRC, 1
1:	
	ld T0, X+
	ld T1, X+
	ld T2, X+
	ld T3, X+
	cp NEXT_NK, C1
	breq 2f 
	brtc 5f
	mov r16, C1
	andi r16, 0x07
	cpi r16, 0x04
	brne 5f
	movw r30, SBOX_SAVE0
	add r30, T0
	adc r31, r1
	lpm T0, Z
	movw r30, SBOX_SAVE0
	add r30, T1
	adc r31, r1
	lpm T1, Z
	movw r30, SBOX_SAVE0
	add r30, T2
	adc r31, r1
	lpm T2, Z
	movw r30, SBOX_SAVE0
	add r30, T3
	adc r31, r1
	lpm T3, Z
	rjmp 5f
2:
	add NEXT_NK, NK
	movw r30, SBOX_SAVE0
	add r30, T0
	adc r31, r1
	lpm r16, Z
	movw r30, SBOX_SAVE0
	add r30, T1
	adc r31, r1
	lpm T0, Z
	movw r30, SBOX_SAVE0
	add r30, T2
	adc r31, r1
	lpm T1, Z
	movw r30, SBOX_SAVE0
	add r30, T3
	adc r31, r1
	lpm T2, Z
	mov T3, r16
	eor T0, XRC
	lsl XRC
	brcc 3f
	ldi XRC, 0x1b
3:
5:	
	movw r30, r26

	ld r0, Y+
	eor r0, T0
	st Z+, r0 
	ld r0, Y+
	eor r0 ,T1
	st Z+, r0
	ld r0, Y+
	eor r0, T2
	st Z+, r0
	ld r0, Y+
	eor r0, T3
	st Z+, r0
	
/*
	st Z+, T0
	st Z+, T1
	st Z+, T2
	st Z+, T3
*/		
	
	inc C1
	cp C1, HI
	breq 6f
	rjmp 1b
6:	
	
	clt
	pop r29
	pop r28
	pop_range 14, 17
	ret
	
	
	
	
