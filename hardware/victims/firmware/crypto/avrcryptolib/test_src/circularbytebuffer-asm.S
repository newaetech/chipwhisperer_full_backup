/* circularbytebuffer-asm.S */
/*
    This file is part of the AVR-circularbytebuffer.
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
 * \file     circularbytebuffer-asm.S
 * \email    daniel.otte@rub.de
 * \author   Daniel Otte 
 * \date     2009-07-25
 * \license  GPLv3 or later
 * \ingroup  circularbytebuffer
 * \brief    declaration for circular byte buffer
 */
/* 
 typedef struct {
	 uint8_t buffer_size;
	 uint8_t fillcount;
	 uint8_t* buffer;
	 uint8_t* head;
	 uint8_t* tail;
	 uint8_t* top;
} circularbytebuffer_t;
*/

#include "config.h"

#define BUFFER_SIZE_OFFSET 0
#define FILLCOUNT_OFFSET   1
#define BUFFER_OFFSET      2
#define HEAD_OFFSET        4
#define TAIL_OFFSET        6
#define TOP_OFFSET         8

#ifndef CIRCULARBYTEBUFFER_NO_MALLOC
#  define CIRCULARBYTEBUFFER_NO_MALLOC 0
#endif

#ifndef CIRCULARBYTEBUFFER_NO_INIT2
#  define CIRCULARBYTEBUFFER_NO_INIT2 0
#endif

#if CIRCULARBYTEBUFFER_NO_MALLOC==0

/******************************************************************************/
/*
 *	uint8_t circularbytebuffer_init(uint8_t buffersize, circularbytebuffer_t* cb){
 *		cb->buffer_size = buffersize;
 *		cb->buffer = malloc(buffersize);
 *		cb->head = cb->tail = cb->buffer;
 *		cb->top = cb->buffer + cb->buffer_size;
 *		cb->fillcount = 0;
 *		if(cb->buffer)
 *			return 1; / * success * /
 *		return 0; / * malloc failed * /	
 *	}
 * 
 * param buffersize:  r24
 * param cb:          r22:r23
 */
.global circularbytebuffer_init
circularbytebuffer_init:
	push r28
	push r29
	movw r28, r22
	std Y+0, r24 /* set buffer_size */
	std Y+1, r1  /* set fillcount to 0 */
	clr r25
	call malloc
	adiw r24, 0
	brne 10f
	clr r24
	rjmp 99f
10:
	std Y+2, r24
	std Y+3, r25
	std Y+4, r24
	std Y+5, r25
	std Y+6, r24
	std Y+7, r25
	ld r22, Y
	add r24, r22
	adc r25, r1
	std Y+8, r24
	std Y+9, r25
	ldi r24, 1	
99:
	clr r25
	pop r29
	pop r28
	ret

/******************************************************************************/
/*
 *	void circularbytebuffer_free(circularbytebuffer_t* cb){
 *		free(cb->buffer);
 *	}
 *
 * param cb:  r24:r25
 */
.global circularbytebuffer_free 
circularbytebuffer_free:
	movw r30, r24
	ldd r24, Z+BUFFER_OFFSET
	ldd r25, Z+BUFFER_OFFSET+1
	jmp free

#endif /* CIRCULARBYTEBUFFER_NO_MALLOC==0 */

#if CIRCULARBYTEBUFFER_NO_INIT2==0

/******************************************************************************/
/*
 *	void circularbytebuffer_init2(uint8_t buffersize, circularbytebuffer_t* cb, void* buffer){
 *		cb->buffer_size = buffersize;
 *		cb->buffer = buffer
 *		cb->head = cb->tail = cb->buffer;
 *		cb->top = cb->buffer + cb->buffer_size;
 *		cb->fillcount = 0;
 *	}
 * 
 * param buffersize:  r24
 * param cb:          r22:r23
 * param buffer:      r20:r21
 */
.global circularbytebuffer_init2
circularbytebuffer_init2:
	movw r30, r22
	std Z+0, r24 /* set buffer_size */
	std Z+1, r1  /* set fillcount to 0 */
	std Z+2, r20
	std Z+3, r21
	std Z+4, r20
	std Z+5, r21
	std Z+6, r20
	std Z+7, r21
	add r20, r24
	adc r21, r1
	std Z+8, r20
	std Z+9, r21
	ret

#endif /* CIRCULARBYTEBUFFER_NO_INIT2==0 */

/******************************************************************************/
/*
 *	uint8_t circularbytebuffer_cnt(circularbytebuffer_t* cb){
 *		return (cb->fillcount);
 *	}
 *
 * param cb:  r24:r25
 */
.global circularbytebuffer_cnt
circularbytebuffer_cnt:
	movw r30, r24
	ldd r24, Z+FILLCOUNT_OFFSET
	clr r25
	ret
	
/******************************************************************************/
/*
 *	uint16_t circularbytebuffer_get_lifo(circularbytebuffer_t* cb){
 *		uint8_t ret;
 *		if(cb->fillcount==0)
 *			return 0xffff;
 *		--cb->fillcount;
 *		ret=*(cb->tail);
 *		cb->tail = (uint8_t*)(cb->tail) + 1;
 *		if(cb->tail>=cb->top)	
 *			cb->tail = (uint8_t*)(cb->tail) - cb->buffer_size;
 *		return ret;	
 *	}
 * param cb:  r24:r25
 */
.global circularbytebuffer_get_lifo
circularbytebuffer_get_lifo:
	movw r30, r24
	ldd r23, Z+FILLCOUNT_OFFSET
	tst r23
	brne 10f
	ser r24
	ser r25
	ret
10: 
	dec r23
	std Z+FILLCOUNT_OFFSET, r23
	ldd r26, Z+TAIL_OFFSET
	ldd r27, Z+TAIL_OFFSET+1
	ld r24, X+
	clr r25
	ldd r22, Z+TOP_OFFSET
	ldd r23, Z+TOP_OFFSET+1
	cp  r26, r22
	cpc r27, r23
	brlo 20f
	ldd r22, Z+BUFFER_SIZE_OFFSET
	sub r26, r22
	sbc r27, r1
20:	
	std Z+TAIL_OFFSET, r26
	std Z+TAIL_OFFSET+1, r27
	ret	

/******************************************************************************/
/*
 *	uint16_t circularbytebuffer_get_fifo(circularbytebuffer_t* cb){
 *		uint8_t ret;
 *		if(cb->fillcount==0)
 *			return 0xffff;
 *		--cb->fillcount;
 *		ret=*(cb->head);
 *		cb->head = (uint8_t*)(cb->head) - 1;
 *		if(cb->head<cb->buffer)	
 *			cb->head = (uint8_t*)(cb->head) + cb->buffer_size;
 *		return ret;
 *	}
 *
 * param cb:  r24:r25
 * return:    r24
 * modifys:   r22-r27,r30,r31
 */
.global circularbytebuffer_get_fifo
circularbytebuffer_get_fifo:
	movw r30, r24
	ldd r23, Z+FILLCOUNT_OFFSET
	tst r23
	brne 10f
	ser r24
	ser r25
	ret
10: 
	dec r23
	std Z+FILLCOUNT_OFFSET, r23
	ldd r26, Z+HEAD_OFFSET
	ldd r27, Z+HEAD_OFFSET+1
	ld r24, X
	clr 25
	sbiw r26, 1
	ldd r22, Z+BUFFER_OFFSET
	ldd r23, Z+BUFFER_OFFSET+1
	cp  r26, r22
	cpc r27, r23
	brge 20f
	ldd r22, Z+BUFFER_SIZE_OFFSET
	add r26, r22
	adc r27, r1
20:	
	std Z+HEAD_OFFSET, r26
	std Z+HEAD_OFFSET+1, r27
	ret	

/******************************************************************************/
/*
 *	uint8_t circularbytebuffer_append(uint8_t elem, circularbytebuffer_t* cb){
 *		if(cb->fillcount==cb->buffer_size)
 *			return 1;
 *		cb->fillcount++;	
 *		cb->tail = cb->tail - 1;
 *		if(cb->tail<cb->buffer)	
 *			cb->tail = (uint8_t*)(cb->tail) + cb->buffer_size;
 *		if(cb->fillcount==1)
 *			cb->head = cb->tail;
 *		*(cb->tail) = elem;
 *		return 0;
 *	}
 *
 * param elem:  r24
 * param cb:    r22:r23
 */	
.global circularbytebuffer_append
circularbytebuffer_append:
	movw r30, r22
	ldd r22, Z+FILLCOUNT_OFFSET
	ldd r23, Z+BUFFER_SIZE_OFFSET
	cp r22, r23
	brne 10f
5:
	ldi r24, 1
	ret
10:
	clt
	tst r22
	brne 11f
	set
11:	
	inc r22
	std Z+FILLCOUNT_OFFSET, r22
	ldd r26, Z+TAIL_OFFSET
	ldd r27, Z+TAIL_OFFSET+1
	sbiw r26, 1
	ldd r22, Z+BUFFER_OFFSET
	ldd r23, Z+BUFFER_OFFSET+1
	cp  r26, r22
	cpc r27, r23
	brge 20f
	ldd r22, Z+BUFFER_SIZE_OFFSET
	add r26, r22
	adc r27, r1	
20:
	std Z+TAIL_OFFSET, r26
	std Z+TAIL_OFFSET+1, r27
	brtc 30f
	std Z+HEAD_OFFSET, r26
	std Z+HEAD_OFFSET+1, r27
30:
	st X, r24
	clr r24
	ret	

/******************************************************************************/
/*
 *	uint8_t circularbytebuffer_push(uint8_t elem, circularbytebuffer_t* cb){
 *		if(cb->fillcount==cb->buffer_size)
 *			return 1;
 *		cb->fillcount++;	
 *		cb->head = cb->head + 1;
 *		if(cb->head>=cb->top)	
 *			cb->head = (uint8_t*)(cb->head) - cb->buffer_size;
 *		if(cb->fillcount==1)
 *			cb->tail = cb->head;
 *		*(cb->head) = elem;
 *		return 0;
 *	}
 * 
 * param elem:  r24
 * param cb:    r22:r23
 */
.global circularbytebuffer_push
circularbytebuffer_push:
	movw r30, r22
	ldd r22, Z+FILLCOUNT_OFFSET
	ldd r23, Z+BUFFER_SIZE_OFFSET
	cp r22, r23
	brlo 10f
	rjmp 5b
;	ldi r24, 1
;	ret
10:
;	clt
;	tst r22
;	brne 11f
;	set
11:	
	inc r22
	std Z+FILLCOUNT_OFFSET, r22
	ldd r26, Z+HEAD_OFFSET
	ldd r27, Z+HEAD_OFFSET+1
	adiw r26, 1
	ldd r22, Z+TOP_OFFSET
	ldd r23, Z+TOP_OFFSET+1
	cp  r26, r22
	cpc r27, r23
	brlo 20f
	ldd r22, Z+BUFFER_SIZE_OFFSET
	sub r26, r22
	sbc r27, r1	
20:
	std Z+HEAD_OFFSET, r26
	std Z+HEAD_OFFSET+1, r27
;	brtc 30b
;	std Z+TAIL_OFFSET, r26
;	std Z+TAIL_OFFSET+1, r27
	rjmp 30b
