/* ubi512_asm.S */
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
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-25
 * \license GPLv3 or later
 */ 

#include "avr-asm-macros.S"

/******************************************************************************/
/*
void ubi512_init(ubi512_ctx_t* ctx, const void* g, uint8_t type){
	memset(ctx->tweak, 0, 15);
	ctx->tweak[15] = 0x40+type;
	memcpy(ctx->g, g, UBI512_BLOCKSIZE_B);
}
*/
/*
 * param ctx:  r24:r25
 * param g:    r22:r23
 * param type: r20
 */
.global ubi512_init
ubi512_init:
	movw r26, r24
	ldi r21, 15
1:	st X+, r1
	dec r21
	brne 1b
	ori r20, 0x40
	st X+, r20
	ldi r21, 64
	movw r30, r22
2:  ld r20, Z+
	st X+, r20
	dec r21
	brne 2b
	ret	

/******************************************************************************/
/*
void ubi512_ctx2hash(void* dest, const ubi512_ctx_t* ctx){
	memcpy(dest, ctx->g, UBI512_BLOCKSIZE_B);
}
*/
/*
 * param dest: r24:r24
 * param ctx:  r22:r23
 */
.global ubi512_ctx2hash
ubi512_ctx2hash:
	movw r26, r24
	movw r30, r22
	adiw r30, 16
	ldi r22, 64
1:	ld r23, Z+
	st X+, r23
	dec r22
	brne 1b
	ret

/******************************************************************************/
/*
void ubi512_nextBlock(ubi512_ctx_t* ctx, const void* block){
	threefish512_ctx_t tfctx;
	((uint64_t*)(ctx->tweak))[0] += UBI512_BLOCKSIZE_B;
	threefish512_init(ctx->g, ctx->tweak, &tfctx);
	memcpy(ctx->g, block, UBI512_BLOCKSIZE_B);
	threefish512_enc(ctx->g, &tfctx);
	memxor(ctx->g, block, UBI512_BLOCKSIZE_B);
	ctx->tweak[15] &= (uint8_t)~0x40;
}  
*/
/*
 * param ctx:   r24:r25
 * param block: r22:r23
 */
CTX0   = 2
CTX1   = 3
BLOCK0 = 4
BLOCK1 = 5
TFCTX0 = 6
TFCTX1 = 7 
.global ubi512_nextBlock
ubi512_nextBlock:
	stack_alloc_large 12*8
	push_range 2, 7
	adiw r30, 1 /* Z points to tfctx */
	movw TFCTX0, r30
	movw CTX0, r24
	movw BLOCK0, r22
	movw r26, r24
/* add BLOCKSIZE_B (64) to tweak */
	ldi r25, 64
	ld r24, X
	add r24, r25
	st X+, r24
	ldi r25, 11
1:	ld r24, X 
	adc r24, r1
	st X+, r24
	dec r25
	brne 1b
/* call threefish512_init */	
	movw r24, CTX0
	adiw r24, 16
	movw r22, CTX0
	movw CTX0, r24  /* CTX points to ctx->g */
	movw r20, TFCTX0
	rcall threefish512_init
	/* copy block to ctx->g */	
	movw r26, CTX0
	movw r30, BLOCK0
	ldi r25, 64
1:	ld r24, Z+
	st X+, r24
	dec r25
	brne 1b
/* call threefish512_enc */	
	movw r24, CTX0
	movw r22, TFCTX0
	rcall threefish512_enc
/* xor block into ctx->g */	
	movw r26, BLOCK0
	movw r30, CTX0
	ldi r25, 64
1:	ld r24, X+
	ld r23, Z
	eor r23, r24
	st Z+, r23	
	dec r25
	brne 1b
/* clear 'first' bit in tweak */
	sbiw r30, 33
	sbiw r30, 32	
	ld r24, Z
	andi r24, ~0x40
	st Z, r24
exit:
	pop_range 2, 7
	stack_free_large 12*8
	ret

/******************************************************************************/
/*
void ubi512_lastBlock(ubi512_ctx_t* ctx, const void* block, uint16_t length_b){
	threefish512_ctx_t tfctx;
	while(length_b>UBI512_BLOCKSIZE){
		ubi512_nextBlock(ctx, block);
		block = (uint8_t*)block + UBI512_BLOCKSIZE_B;
		length_b -= UBI512_BLOCKSIZE;
	}
	ctx->tweak[15] |= 0x80;
	((uint64_t*)(ctx->tweak))[0] += (length_b+7)/8;
	if(length_b & 0x07)
		ctx->tweak[14] |= 0x80;
	threefish512_init(ctx->g, ctx->tweak, &tfctx);
	memset(ctx->g, 0, UBI512_BLOCKSIZE_B);
	memcpy(ctx->g, block, (length_b+7)/8);
	if(length_b & 0x07)
		ctx->g[(length_b+7)/8-1] |= 0x80>>(length_b&7);
	threefish512_enc(ctx->g, &tfctx);
	memxor(ctx->g, block, (length_b+7)/8);
	if(length_b & 0x07){
		ctx->g[((length_b+7)/8)-1] ^= 0x80>>(length_b&7);
	}
} 
*/
/*
 * param ctx:     r24:r25
 * param block:   r22:r23
 * param ength_b: r20:r21
 */
MASK_B  =  8 
LEN_B   =  9
TFCTX0  = 10
TFCTX1  = 11
CTX0    = 12
CTX1    = 13
BLOCK0  = 14
BLOCK1  = 15
LENGTH0 = 16
LENGTH1 = 17
.global ubi512_lastBlock
ubi512_lastBlock:
/* run nextBlock for preceding blocks*/
	push_range 8, 17
	movw CTX0, r24
	movw BLOCK0, r22
	movw LENGTH0, r20
1:	cpi LENGTH1, 3
	brlo 2f
	movw r24, CTX0
	movw r22, BLOCK0
	rcall ubi512_nextBlock
	ldi r25, 64
	add BLOCK0, r25
	adc BLOCK1, r1
	subi LENGTH1, 2
	rjmp 1b
2:	cpi LENGTH1, 2
	brlo 3f
	tst LENGTH0
	breq 3f
	movw r24, CTX0
	movw r22, BLOCK0
	rcall ubi512_nextBlock
	ldi r25, 64
	add BLOCK0, r25
	adc BLOCK1, r1
	subi LENGTH1, 2
3:	/* now the real fun */
    stack_alloc_large 8*12
	adiw r30, 1
	movw TFCTX0, r30
	/* calculate LEN_B */
	movw r24, LENGTH0
	adiw r24, 7
	lsr r25
	ror r24
	lsr r25
	ror r24
	lsr r24
	mov LEN_B, r24
	/* add length to tweak */
	movw r30, CTX0
	ld r24, Z
	add r24, LEN_B
	st Z+, r24
	ldi r25, 11
1:	ld r24, Z
	adc r24, r1
	st Z+, r24
	dec r25
	brne 1b
	/* set 'final' bit*/
	movw r30, CTX0
	ldd r24, Z+15
	ori r24, 0x80
	std Z+15, r24
	/* store in MASk_B if we do bit processing and set 'BitPad' bit*/
	clr MASK_B
	mov r24, LENGTH0
	andi r24, 0x07
	tst r24
	breq 4f
	ldd r25, Z+14
	ori r25, 0x80
	std Z+14, r25
	ldi r25, 0x80
	mov MASK_B, r25
1:	lsr MASK_B
	dec r24
	brne 1b
4:  /* call threefish512_init*/
	movw r24, CTX0
	adiw r24, 16
	movw r22, CTX0
	movw CTX0, r24 /* CTX points at ctx->g */
	movw r20, TFCTX0
	rcall threefish512_init
	/* copy block to ctx->g */
	movw r26, BLOCK0
	movw r30, CTX0
	mov r24, LEN_B
	ldi r25, 64
	sub r25, LEN_B
	tst r24
1:	breq 2f
	ld r22, X+
	st Z+, r22
	dec r24	
	rjmp 1b
2:	tst MASK_B
	breq 29f
	or r22, MASK_B
	st -Z, r22
	adiw r30, 1
29:	tst r25
3:	breq 4f
	st Z+, r1
	dec r25
	rjmp 3b
4: /* call threefish512_enc */
	movw r24, CTX0
	movw r22, TFCTX0
	rcall threefish512_enc
   /* xor block into ctx->g */
  	movw r30, CTX0
	movw r26, BLOCK0
	tst LEN_B
5:	breq 6f
	ld r22, X+
	ld r23, Z
	eor r23, r22
	st Z+, r23
	dec LEN_B
	rjmp 5b		
6:	tst MASK_B
	breq 7f
	eor r23, MASK_B
	st -Z, r23
	
7:	stack_free_large 8*12
	pop_range 8, 17
	ret


