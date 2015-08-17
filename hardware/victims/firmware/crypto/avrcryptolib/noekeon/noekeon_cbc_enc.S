/* noekeon_cbc_enc.S */
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

.macro push_ p1:req p2:vararg
	push \p1
.ifnb \p2
	push_ \p2
.endif
.endm

.macro pop_ p1:req p2:vararg
	pop \p1
.ifnb \p2
	pop_ \p2
.endif
.endm

.extern noekeon_enc

/*
 * void noekeon_cbc_enc(void* buffer, uint8_t block_cnt, const void* key)
 */

/* param buffer    is passed in r24:r25
 * param block_cnt is passed in r22 (r23 is 0)
 * param key       is passed in r20:r21
 */
.global noekeon_cbc_enc
 noekeon_cbc_enc:
	push r22
	movw r22, r20
	push_ r22, r23, r24, r25
	rcall noekeon_enc
1:
	pop_ r27, r26, r23, r22
	pop r16 /* block  counter */
	dec r16
	breq 9f
	push r16
	/* xor blocks */
	movw r30, r26
	adiw r30, 16
	ldi r16, 16
2:
	ld r17, X+
	ld r18, Z
	eor r18, r17
	st Z+, r18
	dec r16
	brne 2b

	/* call encryption function; X points to our new block */
	push_ r22, r23, r26, r27
	movw r24, r26
	rcall noekeon_enc
	rjmp 1b
9:
	ret
















