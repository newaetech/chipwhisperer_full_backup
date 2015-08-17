/* noekeon_omac.S */
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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2008-08-24
 * \license GPLv3 or later
 * 
 * 
 * 
 */

#include <avr/io.h>
#include "avr-asm-macros.S"

.extern noekeon_enc


/******************************************************************************/

/*
 * void noekeon_omac_init(noekeon_omac_ctx_t* ctx){
 *	memset(ctx, 0, 16);
 * }
 */
/*
 * param ctx in r24:r25
 */

.global omac_noekeon_init 
omac_noekeon_init:
	movw r30, r24
	ldi r24, 16
1:
	st Z+, r1
	dec r24
	brne 1b
	ret	

/******************************************************************************/	

/*
 * void omac_noekeon_tweak(uint8_t t, const void* key, noekeon_omac_ctx_t* ctx){
 *	*ctx[15] = t;
 *	noekeon_enc(ctx, key);
 * }
 */
/*
 * param t   in r24
 * param key in r22:r23
 * param ctx in r20:r21
 */ 
.global omac_noekeon_tweak 
omac_noekeon_tweak:
	movw r30, r20
	std Z+15, r24
	movw r24, r20
	rjmp noekeon_enc

/******************************************************************************/
	
/*
 * void noekeon_omac_next(const void* buffer, const void* key, noekeon_omac_ctx_t* ctx){
 * 	memxor(ctx, buffer, 16);
 *	noekeon_enc(ctx, key);
 * }
 */
/*
 * param buffer in r24:r25
 * param key    in r22:r23
 * param ctx    in r20:r21
 */ 
.global omac_noekeon_next 
omac_noekeon_next:
	movw r26, r20
	movw r30, r24
	ldi r24, 16
1:
	ld r0, X
	ld r25, Z+
	eor r0, r25
	st X+, r0
	dec r24
	brne 1b
	movw r24, r20
	rjmp noekeon_enc	

/******************************************************************************/

/*
 * void omac_noekeon_comppad(uint8_t* pad, const void* key, uint8_t length_b){
 *	uint8_t c1,c2,r,j;
 *	memset(pad, 0, 16);
 *	noekeon_enc(pad, key);
 *	r=(length_b==128)?1:2;
 *	for(;r!=0;--r){
 *		c1=0;
 *		for(j=0;j<16;++j){
 *			c2 = c1;
 *			c1 = (pad[15-j])>>7;
 *			pad[15-j] = ((pad[15-j])<<1) | c2;
 *		}
 *		if(c1){
 *			pad[15] ^= 0x87;
 *		}
 *	}
 *	if(length_b<128){
 *		pad[(length_b)/8] ^= 0x80 >> (length_b%8);
 *	}
 *}
 */
/*
 * param pad      in r24:r25
 * param key      in r22:r23
 * param length_b in r20
 */ 
.global omac_noekeon_comppad
omac_noekeon_comppad:
	push_ r20, r24, r25
	ldi r20, 16
	movw r30, r24
1:
	st Z+, r1
	dec r20
	brne 1b
	rcall noekeon_enc
	pop_ r31, r30, r20 /* now Z points at pad, and r20 contains length_b */
	ldi r21, 1
	clt
	cpi r20, 128
	breq 2f
	set	
	inc r21
2:
	adiw r30, 16
	ldi r24, 16
	clc
3:	
	ld r0, -Z
	rol r0
	st Z, r0
	dec r24
	brne 3b
	
	brcc 4f
	ldi r24, 0x87
	ldd r0, Z+15
	eor r0, r24
	std Z+15, r0
4:
	dec r21
	brne 2b
	/* the B/P calculation is done, now we have only to insert the one for
	   messages of a length != n*128 */
	brts 5f
	ret
5:
	/* r20 contains the length in bits where a one must be appended via xor */
	mov r21, r20
	lsr r21
	lsr r21
	lsr r21
	add r30, r21
	adc r31, r1
	andi r20, 0x07
	ldi r21, 0x80
6:	tst r20
	breq 8f
7:	lsr r21
	dec r20
	brne 7b
8:	
	ld r24, Z
	eor r24, r21
	st Z, r24
	ret

/******************************************************************************/

/*
 * void omac_noekeon_last(const void* buffer, uint8_t length_b, const void* key, noekeon_omac_ctx_t* ctx){
 *	while(length_b>128){
 *		omac_noekeon_next(buffer, key, ctx);
 *		buffer = (uint8_t*)buffer +16;
 *		length_b -= 128;
 *	}
 *	uint8_t pad[16];
 *	omac_noekeon_comppad(pad, key, length_b);
 *  memxor(pad, buffer, (length_b+7)/8);
 *	omac_noekeon_next(pad, key, ctx);
 *}
 */
/*
 * param buffer   in r24:r25
 * param length_b in r22
 * param key      in r20:r21
 * param ctx      in r18:r19
 */ 
.global omac_noekeon_last
omac_noekeon_last:
	push_range 10, 16
	push_ r28, r29
	movw r28, r24 /* buffer */
	movw r12, r20 /* key */
	movw r14, r18 /* ctx */
	mov r16, r22  /* length_b */
1:
	cpi r16, 129
	brlo 2f
	movw r22, r20
	movw r20, r18
	rcall omac_noekeon_next
	adiw r28, 16
	subi r16, 128
2:
	stack_alloc 16
	adiw r30, 1
	movw r10, r30
	movw r24, r30
	movw r22, r12
	mov  r20, r16
	rcall omac_noekeon_comppad
    movw r30, r10
    subi r16, -7
    lsr r16
 	lsr r16
 	lsr r16
 	breq 4f	
3:
	ld r0,  Z
	ld r24, Y+
	eor r0, r24
	st Z+, r0
	dec r16
	brne 3b	 	
4: 
 	movw r24, r10
 	movw r22, r12
 	movw r20, r14
 	rcall omac_noekeon_next
 	stack_free 16
 
	pop_ r29, r28
 	pop_range 10, 16
	ret

/******************************************************************************/

/* 
 *void omac_noekeon(void* dest, const void* msg, uint16_t msglength_b,
 *                  const void* key, uint8_t t){
 *	omac_noekeon_init(dest);
 *	if(t!=0xff)
 *		omac_noekeon_tweak(t,key,dest);
 *	while(msglength_b>128){
 *		omac_noekeon_next(msg, key, dest);
 *		msg = (uint8_t*)msg +16;
 *		msglength_b -= 128;
 *	}
 *	omac_noekeon_last(msg, msglength_b, key, dest);                  	
 *}
 */
/*
 * param dest        in r24:r25
 * param msg         in r22:r23
 * param msglength_b in r20:r21
 * param key         in r18:r19
 * param t           in r16
 */ 
MSG0 = 28
MSG1 = 29
KEY0 = 10
KEY1 = 11
LEN0 = 12
LEN1 = 13
DST0 = 14
DST1 = 15 

.global omac_noekeon
omac_noekeon:
	push_ r28, r29 
	push_range 10, 17 
 	movw MSG0, r22 /* msg */
 	movw KEY0, r18 /* key */
 	movw LEN0, r20 /* msglength_b */
 	movw DST0, r24 /* dest */
 	/* omac_noekeon_init(dest); */
 	rcall omac_noekeon_init
 	cpi r16, 0xff
 	breq 1f
 	mov  r24, r16
 	movw r22, KEY0
 	movw r20, DST0
 	/* omac_noekeon_tweak(t,key,dest); */
 	rcall omac_noekeon_tweak	
1:	
	movw r16, LEN0
	tst r17
	breq 4f
3:	
	movw r24, MSG0
	movw r22, KEY0
	movw r20, DST0
	/* omac_noekeon_next(msg, key, dest); */
	rcall omac_noekeon_next
	adiw MSG0, 16
	subi r16, 128
	sez
	sbci r17, 0 /* wont change Z if result is zero */
	brne 3b
4:
	movw r24, MSG0
	mov r22, r16
	movw r20, KEY0
	movw r18, DST0
	/* omac_noekeon_last(msg, msglength_b, key, dest); */
	call omac_noekeon_last
	
 	pop_range 10, 17
 	pop_ r29, r28
 	ret
 
 
 
 
 
 
 
 
 
 
 
 




