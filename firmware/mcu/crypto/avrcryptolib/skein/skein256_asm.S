/* skein256_asm.S */
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
/**
 * \author  Daniel Otte
 * \email   daniel.otte@rub.de
 * \date    2009-03-16
 * \license GPLv3 or later
 */

#include "avr-asm-macros.S"

/******************************************************************************/
/*
void skein256_init(skein256_ctx_t* ctx, uint16_t outsize_b){
	skein_config_t conf;
	uint8_t null[UBI256_BLOCKSIZE_B];
	memset(null, 0, UBI256_BLOCKSIZE_B);
	memset(&conf, 0, sizeof(skein_config_t));
	conf.schema[0] = 'S';
	conf.schema[1] = 'H';
	conf.schema[2] = 'A';
	conf.schema[3] = '3';
	conf.version = 1;
	conf.out_length = outsize_b;
	ctx->outsize_b = outsize_b;
	ubi256_init(&(ctx->ubictx), null, UBI_TYPE_CFG);
	ubi256_lastBlock(&(ctx->ubictx), &conf, 256);
	ubi256_init(&(ctx->ubictx), ctx->ubictx.g, UBI_TYPE_MSG);
}
*/
/*
 * param ctx:       r24:r25
 * param outsize_b: r22:r23
 */
UBICTX0 = 2
UBICTX1 = 3
CONF0   = 4
CONF1   = 5
.global skein256_init
skein256_init:
	push_range 2, 5
	stack_alloc 64-22
	adiw r30, 1
	movw CONF0, r30
	movw r26, r24
	st X+, r22
	st X+, r23
	movw UBICTX0, r26
	ldi r24, 'S'
	st Z+, r24
	ldi r24, 'H'
	st Z+, r24
	ldi r24, 'A'
	st Z+, r24
	ldi r24, '3'
	st Z+, r24
	ldi r24, 1
	st Z+, r24
	st Z+, r1
	st Z+, r1
	st Z+, r1
	st Z+, r22
	st Z+, r23
	ldi 24, 22+10
1:  st Z+, r1
	dec r24
	brne 1b
	/* call ubi256_init*/
	sbiw r30, 32
	movw r24, UBICTX0
	movw r22, r30
	ldi r20, 4
	rcall ubi256_init
	/* call ubi256_lastBlock*/
	movw r24, UBICTX0
	movw r22, CONF0
	ldi r21, 1
	clr r20
	rcall ubi256_lastBlock
	/* call ubi256_init*/
	movw r24, UBICTX0
	adiw r24, 16
	movw r22, r24
	movw r24, UBICTX0
	ldi r20, 48
	rcall ubi256_init
	stack_free 64-22
	pop_range 2, 5
	ret

/******************************************************************************/
.global skein256_nextBlock
skein256_nextBlock:
	adiw r24, 2
	rjmp ubi256_nextBlock

/******************************************************************************/
.global skein256_lastBlock
skein256_lastBlock:
	adiw r24, 2
	rjmp ubi256_lastBlock

/******************************************************************************/
/*
void skein256_ctx2hash(void* dest, skein256_ctx_t* ctx){
	ubi256_ctx_t uctx;
	uint16_t outsize_b;

	uint64_t counter=0;
	uint8_t outbuffer[UBI256_BLOCKSIZE_B];
	ubi256_init(&(ctx->ubictx), ctx->ubictx.g, UBI_TYPE_OUT);

	outsize_b = ctx->outsize_b;
	while(1){
		memcpy(&uctx, &(ctx->ubictx), sizeof(ubi256_ctx_t));
		ubi256_lastBlock(&uctx, &counter, 64);
		ubi256_ctx2hash(outbuffer, &uctx);
		if(outsize_b<=UBI256_BLOCKSIZE){
			memcpy(dest, outbuffer, (outsize_b+7)/8);
			break;
		}else{
			memcpy(dest, outbuffer, UBI256_BLOCKSIZE_B);
			dest = (uint8_t*)dest + UBI256_BLOCKSIZE_B;
			outsize_b -= UBI256_BLOCKSIZE;
			counter++;
		}
	}
}
*/
/*
 * param dest: r24:r25
 * param ctx:  r22:r23
 */
 OUTSIZE_B0 = 16
 OUTSIZE_B1 = 17
 UCTX0      = 14
 UCTX1      = 15
 UBICTX0    = 12
 UBICTX1    = 13
 DEST0      = 10
 DEST1      = 11
.global skein256_ctx2hash
skein256_ctx2hash:
	push_range 10, 17
	                      /*  48  ||  8      ||  32       */
	stack_alloc_large 88  /* uctx || counter || outbuffer */
	movw DEST0, r24
	adiw r30, 1
	movw UCTX0, r30
	adiw r30, 48
	st Z+, r1
	st Z+, r1
	st Z+, r1
	st Z+, r1
	st Z+, r1
	st Z+, r1
	st Z+, r1
	st Z+, r1
	movw r26, 22
	ld OUTSIZE_B0, X+
	ld OUTSIZE_B1, X+
	movw UBICTX0, r26
   /* call ubi256_init */
	movw r24, UBICTX0
	adiw r24, 16
	movw r22, r24
	movw r24, UBICTX0
	ldi r20, 63
	rcall ubi256_init

   /* main loop */
   /* copy ubictx in uctx*/
1:	movw r30, UCTX0
	movw r26, UBICTX0
	ldi r24, 48
2:	ld r25, X+
	st Z+, r25
	dec r24
	brne 2b
  /* call ubi256_lastBlock */
    movw r24, UCTX0
	adiw r24, 48
	movw r22, r24
	movw r24, UCTX0
	clr r21
	ldi r20, 64
	rcall ubi256_lastBlock
  /* copy uctx->g to outbuffer */
	movw r26, UCTX0
	adiw r26, 16
	movw r30, UCTX0
	adiw r30, 56
	ldi r24, 32
2:	ld r25, X+
    st Z+, r25
	dec r24
	brne 2b
   /* compare outsize_b with 256*/
	cpi OUTSIZE_B1, 2
	brge 5f
	cpi OUTSIZE_B1, 1
	brlo 3f
	tst OUTSIZE_B0
	breq 3f
5:	/* copy outbuffer to dest */
	movw r30, DEST0
	movw r26, UCTX0
	adiw r26, 56
	ldi r24, 32
6:	ld r25, X+
	st Z+, r25
	dec r24
	brne 6b
	/* store new dest */
	movw DEST0, r30 ;XXX r26
	/* adjust counter and outsize_b*/
	dec OUTSIZE_B1
	movw r30, UCTX0
	adiw r30, 48
	ldi r24, 1
	ld r25, Z
	add r25, r24
	st Z+, r25
	ldi r24, 7
6:	ld r25, Z
	adc r25, r1
	st Z+, r25
	dec r24
	brne 6b
	rjmp 1b
3:	/* last iteraton */
	movw r24, OUTSIZE_B0
	adiw r24, 7
	lsr r25
	ror r24
	lsr r24
	lsr r24
	movw r30, DEST0
	movw r26, UCTX0
	adiw r26, 56
	tst r24
	breq 8f
7:	ld r25, X+
	st Z+, r25
	dec r24
	brne 7b
8:
	stack_free_large 88
	pop_range 10, 17
	ret

/******************************************************************************/
/*
void skein256(void* dest, uint16_t outlength_b, const void* msg, uint32_t length_b){
	skein256_ctx_t ctx;
	skein256_init(&ctx, outlength_b);
	while(length_b>SKEIN256_BLOCKSIZE){
		skein256_nextBlock(&ctx, msg);
		msg = (uint8_t*)msg + SKEIN256_BLOCKSIZE_B;
		length_b -= SKEIN256_BLOCKSIZE;
	}
	skein256_lastBlock(&ctx, msg, length_b);
	skein256_ctx2hash(dest, &ctx);
}
*/
/*
 * param dest:         r24:r25
 * param outlength_b:  r22:r23
 * param msg:          r20:r21
 * param length_b:     r16:r19
 */
LENGTH_B0 =  2
LENGTH_B1 =  3
LENGTH_B2 =  4
LENGTH_B3 =  5
DEST0     =  6
DEST1     =  7
MSG0      =  8
MSG1      =  9
CTX0      = 10
CTX1      = 11
.global skein256
skein256:
	push_range 2, 11
	stack_alloc 50
	adiw r30, 1
	movw CTX0, r30
	movw DEST0, r24
	movw MSG0, r20
	movw LENGTH_B0, r16
	movw LENGTH_B2, r18
	/* call skein256_init */
	movw r24, r30
	rcall skein256_init
1:	tst LENGTH_B2
	brne 4f
	tst LENGTH_B3
	brne 4f
   /* call skein256_lastBlock */
	movw r24, CTX0
	movw r22, MSG0
	movw r20, LENGTH_B0
	rcall skein256_lastBlock
   /* call skein256_ctx2hash */
	movw r24, DEST0
	movw r22, CTX0
	rcall skein256_ctx2hash
   /* return */
	stack_free 50
	pop_range 2, 11
	ret

4: /* process preceeding blocks */
	movw r24, CTX0
	movw r22, MSG0
	rcall skein256_nextBlock
	movw r24, MSG0
	adiw r24, 32
	movw MSG0, r24
	mov r24, LENGTH_B1
	mov r25, LENGTH_B2
	sbiw r24, 1
	sbc LENGTH_B3, r1
	mov LENGTH_B1, r24
	mov LENGTH_B2, r25
	rjmp 1b

