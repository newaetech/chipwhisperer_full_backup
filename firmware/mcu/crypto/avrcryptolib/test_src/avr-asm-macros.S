/* avr-asm-macros.S */
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
 * File:        avr-asm-macros.S
 * Author:      Daniel Otte
 * Date:        2008-08-13
 * License:     GPLv3 or later
 * Description: some macros which are quite usefull
 * 
 */
 
#include <avr/io.h>

/*******************************************************************************
*  MACRO SECTION                                                               *
*******************************************************************************/

.macro push_ p1:req, p2:vararg
	push \p1
.ifnb \p2	
	push_ \p2
.endif
.endm

.macro pop_ p1:req, p2:vararg
	pop \p1
.ifnb \p2	
	pop_ \p2
.endif
.endm

.macro push_range from:req, to:req
	push \from
.if     \to-\from
	push_range "(\from+1)",\to
.endif		
.endm

.macro pop_range from:req, to:req
	pop \to
.if     \to-\from
	pop_range \from,"(\to-1)"	
.endif
.endm

.macro stack_alloc size:req, reg1=r30, reg2=r31
	in r0, _SFR_IO_ADDR(SREG)
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	sbiw \reg1, \size 
	cli
	out _SFR_IO_ADDR(SPH), \reg2
	out _SFR_IO_ADDR(SREG), r0
	out _SFR_IO_ADDR(SPL), \reg1
.endm

.macro stack_free size:req, reg1=r30, reg2=r31
	in r0, _SFR_IO_ADDR(SREG)
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	adiw \reg1, \size 
	cli
	out _SFR_IO_ADDR(SPH), \reg2
	out _SFR_IO_ADDR(SREG), r0
	out _SFR_IO_ADDR(SPL), \reg1
.endm


.macro stack_alloc_large size:req, reg1=r30, reg2=r31
	in r0, _SFR_IO_ADDR(SREG)
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	subi \reg1, lo8(\size)
	sbci \reg2, hi8(\size)	 
	cli
	out _SFR_IO_ADDR(SPH), \reg2
	out _SFR_IO_ADDR(SREG), r0
	out _SFR_IO_ADDR(SPL), \reg1
.endm

.macro stack_free_large size:req, reg1=r30, reg2=r31
	in r0, _SFR_IO_ADDR(SREG)
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	adiw \reg1, 63
	adiw \reg1, (\size-63) 
	cli
	out _SFR_IO_ADDR(SPH), \reg2
	out _SFR_IO_ADDR(SREG), r0
	out _SFR_IO_ADDR(SPL), \reg1
.endm

.macro stack_free_large2 size:req, reg1=r30, reg2=r31
	in r0, _SFR_IO_ADDR(SREG)
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	adiw \reg1, 63
	adiw \reg1, 63
	adiw \reg1, (\size-63*2) 
	cli
	out _SFR_IO_ADDR(SPH), \reg2
	out _SFR_IO_ADDR(SREG), r0
	out _SFR_IO_ADDR(SPL), \reg1
.endm

.macro stack_free_large3 size:req, reg1=r30, reg2=r31
	in r0, _SFR_IO_ADDR(SREG)
	in \reg1, _SFR_IO_ADDR(SPL)
	in \reg2, _SFR_IO_ADDR(SPH)
	push r16
	push r17
	ldi r16, lo8(\size)
	ldi r17, hi8(\size)
	add \reg1, r16
	adc \reg2, r17
	pop r17
	pop r16 
	cli
	out _SFR_IO_ADDR(SPH), \reg2
	out _SFR_IO_ADDR(SREG), r0
	out _SFR_IO_ADDR(SPL), \reg1
.endm

.macro LOAD_IO reg:req io:req
.if _SFR_IO_REG_P(\io)
	in \reg, _SFR_IO_ADDR(\io)
.else
	lds \reg, _SFR_MEM_ADDR(\io)
.endif
.endm

.macro STORE_IO io:req reg:req
.if _SFR_IO_REG_P(\io)
	out _SFR_IO_ADDR(\io), \reg
.else
	sts _SFR_MEM_ADDR(\io), \reg
.endif
.endm


.macro CLEAR_BIT_IO io:req bit:req reg:req
.if _SFR_IO_REG_P(\io)
	cbi _SFR_IO_ADDR(\io), \bit
.else
	lds \reg, _SFR_MEM_ADDR(\io)
	andi \reg, ~_BV(\bit)
	sts _SFR_MEM_ADDR(\io), \reg
.endif
.endm

.macro SET_BIT_IO io:req bit:req reg:req
.if _SFR_IO_REG_P(\io)
	sbi _SFR_IO_ADDR(\io), \bit
.else
	lds \reg, _SFR_MEM_ADDR(\io)
	ori \reg, _BV(\bit)
	sts _SFR_MEM_ADDR(\io), \reg
.endif
.endm

/*******************************************************************************
* END of MACRO SECTION                                                         *
*******************************************************************************/



