/*	Copyright (C) 2008 HighTec EDV-Systeme GmbH.
	
	This file is part of GCC.

	GCC is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3, or (at your option)
	any later version.

	GCC is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	Under Section 7 of GPL version 3, you are granted additional
	permissions described in the GCC Runtime Library Exception, version
	3.1, as published by the Free Software Foundation.

	You should have received a copy of the GNU General Public License and
	a copy of the GCC Runtime Library Exception along with this program;
	see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
	<http://www.gnu.org/licenses/>.  */


#ifndef _FENV_H
#define _FENV_H


/* IEEE754 rounding modes */
/* Macros according to ISO/IEC 9899:1999 */
#define FE_TONEAREST  0x0UL /* Rounding toward nearest */
#define FE_UPWARD     0x1UL /* Rounding toward +INFINITY */
#define FE_DOWNWARD   0x2UL /* Rounding toward -INFINITY */
#define FE_TOWARDZERO 0x3UL /* Rounding toward zero */


extern inline int fesetround (int)  __attribute__ ((always_inline,gnu_inline));
extern inline int fegetround (void) __attribute__ ((always_inline,gnu_inline));

/* 
   A CALL saves the context and a RET restores the context.
   Therefore, a RET invalidates the changes in PSW.
   => Functions must be inlined and cannot be part of a lib.
*/


/* Get current IEEE754 rounding mode */
extern inline __attribute__ ((always_inline,gnu_inline))
int fegetround (void)
{
  /* PSW bitfield RM, bits [25:24] contains rounding mode */
  int res;
  __asm__ volatile ("mfcr %0, $psw " : "=d" (res) : : "memory");
  return (res & 0x03000000UL) >> 24uL;
}


/* Set IEEE754 rounding mode */
extern inline __attribute__ ((always_inline,gnu_inline))
int fesetround (int round)
{
  /* Set rounding mode useing updfl */

#if defined (ERRATA_CPU114)
  __asm__ volatile ("updfl %0" :: "d" (0xf00 | (round & 3)) : "memory");
#else
  __asm__ volatile ("updfl %0" :: "d" (0x300 | (round & 3)) : "memory");
#endif

  return 0;
}


#endif  /* _FENV_H */
