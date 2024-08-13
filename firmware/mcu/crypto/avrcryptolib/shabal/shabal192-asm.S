/* shabal192-asm.S */
/*
    This file is part of the AVR-Crypto-Lib.
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
/*
 * \file    shabal192-asm.S
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-04-27
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

shabal192_iv:
	/* A */
.long	0xFD749ED4, 0xB798E530, 0x33904B6F, 0x46BDA85E
.long	0x076934B4, 0x454B4058, 0x77F74527, 0xFB4CF465
.long	0x62931DA9, 0xE778C8DB, 0x22B3998E, 0xAC15CFB9
	/* B */
.long	0x58BCBAC4, 0xEC47A08E, 0xAEE933B2, 0xDFCBC824
.long	0xA7944804, 0xBF65BDB0, 0x5A9D4502, 0x59979AF7
.long	0xC5CEA54E, 0x4B6B8150, 0x16E71909, 0x7D632319
.long	0x930573A0, 0xF34C63D1, 0xCAF914B4, 0xFDD6612C
	/* C */
.long	0x61550878, 0x89EF2B75, 0xA1660C46, 0x7EF3855B
.long	0x7297B58C, 0x1BC67793, 0x7FB1C723, 0xB66FC640
.long	0x1A48B71C, 0xF0976D17, 0x088CE80A, 0xA454EDF3
.long	0x1C096BF4, 0xAC76224B, 0x5215781C, 0xCD5D2669


/******************************************************************************/
/*
void shabal192_init(shabal_ctx_t* ctx){
	uint8_t i;
	ctx->b = ctx->b_buffer;
	ctx->c = ctx->c_buffer;
	ctx->w.w64 = 1LL;
	for(i=0;i<SHABAL_R;++i){
		ctx->a[i] = pgm_read_dword(&(shabal192_iv[i]));
	}
	for(i=0;i<16;++i){
		ctx->b[i] = pgm_read_dword(&(shabal192_iv[SHABAL_R+i]));
	}
	for(i=0;i<16;++i){
		ctx->c[i] = pgm_read_dword(&(shabal192_iv[SHABAL_R+16+i]));
	}
}
*/
/*
 * param ctx: r24,r25
 */
.global shabal192_init
shabal192_init:
	movw r26, r24
	ldi r24, 1
	st X+, r24
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	st X+, r1
	movw r24, r26
	adiw r24, 12*4+2+2
	st X+, r24
	st X+, r25
	adiw r24, 4*16-1
	adiw r24, 1
	st X+, r24
	st X+, r25 
	ldi r24, (12+16+16)
	ldi r30, lo8(shabal192_iv)
	ldi r31, hi8(shabal192_iv)
1:	lpm r0, Z+
	st X+, r0
	lpm r0, Z+
	st X+, r0
	lpm r0, Z+
	st X+, r0
	lpm r0, Z+
	st X+, r0
	dec r24
	brne 1b
	ret	

/******************************************************************************/
/*
 * param dest: r24:r25
 * param ctx:  r22:r23
 */
.global shabal192_ctx2hash
shabal192_ctx2hash:
	movw r26, r24
	movw r30, r22
	ldd r24, Z+(8+2)
	ldd r25, Z+(8+2+1)
	movw r30, r24
	adiw r30, (16-192/32)*4
	ldi r24, 192/8
1:  ld r0, Z+
	st X+, r0
	dec r24
	brne 1b
	ret	
	
/******************************************************************************/
/*
void shabal192(void* dest, void* msg, uint32_t length_b){
	shabal_ctx_t ctx;
	shabal192_init(&ctx);
	while(length_b>=SHABAL_BLOCKSIZE){
		shabal_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg+SHABAL_BLOCKSIZE_B;
		length_b -= SHABAL_BLOCKSIZE;
	}
	shabal_lastBlock(&ctx, msg, length_b);
	shabal192_ctx2hash(dest, &ctx);
}
*/
/*
 * param dest:      r24:r25
 * param msg:       r22:r23
 * param length_b:  r18:r21
 */
DST0 =  8
DST1 =  9
CTX0 = 10
CTX1 = 11
MSG0 = 12
MSG1 = 13
LEN2 = 14
LEN3 = 15
LEN0 = 16
LEN1 = 17
.global shabal192
shabal192: 
	push_range 8, 17
	stack_alloc_large 188
	adiw r30, 1 /* Z points to stack space (ctx) */
	movw CTX0, r30
	movw DST0, r24
	movw MSG0, r22
	movw LEN0, r18
	movw LEN2, r20
	movw r24, r30
	rcall shabal192_init
2:
	tst LEN2
	brne 3f
	tst LEN3
	breq 4f
3:
	movw r24, CTX0
	movw r22, MSG0
	rcall shabal_nextBlock
	subi LEN1, 0x02
	sbc LEN2, r1
	sbc LEN3, r1
	ldi r18, 64
	add MSG0, r18
	adc MSG1, r1	
	rjmp 2b
4:	
	movw r24, CTX0
	movw r22, MSG0
	movw r20, LEN0
	rcall shabal_lastBlock
	movw r24, DST0
	movw r22, CTX0
	rcall shabal192_ctx2hash
	stack_free_large2 188
	pop_range 8, 17
	ret
	
	
