/* noekeon_ctr.S */
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
 * \date    2008-08-06
 * \license GPLv3 or later
 * 
 * 
 * 
 */
 
.extern noekeon_enc

/*
 * void noekeon_ctr_next(void* buffer, const noekeon_ctr_ctx_t* ctx); 
 */
.global noekeon_ctr_next
/*
 * param buffer passed in r24:r25
 * param ctx    passed in r22:r23
 */
noekeon_ctr_next:
	/* copy counter to buffer */
	movw r26, r24 /* copy buffer  pointer to X */
	movw r30, r22 /* copy counter pointer to Z */  
	ldi r16, 16
1:	
	ld r0, Z+
	st X+, r0
	dec r16
	brne 1b
	/* increment counter */	
	movw r30, r22 /* copy counter pointer to Z */  
	ldi r17, 1
	ldi r16, 15
	ld  r0, Z
	add r0, r17
	st  Z+, r0
1:	
	ld  r0, Z
	adc r0, r1
	st  Z+, r0
	dec r16
	brne 1b
	/* call encryption routine */
	/* we can leave the first param as is, but have to adjust the second to point to the key */
	//adiw r22, 16
	ldi r16, 16
	add r22, r16
	adc r23, r0
//	rcall noekeon_enc
//	ret
	rjmp noekeon_enc /* noekeon_enc will return for us */ 
