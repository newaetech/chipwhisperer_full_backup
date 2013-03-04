/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *  \brief Board specific Buttons driver header for the Atmel XMEGA A3BU Xplained.
 *  \copydetails Group_Buttons_A3BU_XPLAINED
 *
 *  \note This file should not be included directly. It is automatically included as needed by the Buttons driver
 *        dispatch header located in LUFA/Drivers/Board/Buttons.h.
 */

/** \ingroup Group_Buttons
 *  \defgroup Group_Buttons_A3BU_XPLAINED A3BU_XPLAINED
 *  \brief Board specific Buttons driver header for the Atmel XMEGA A3BU Xplained.
 *
 *  Board specific Buttons driver header for the Atmel XMEGA A3BU Xplained.
 *
 *  <table>
 *    <tr><th>Name</th><th>Info</th><th>Active Level</th><th>Port Pin</th></tr>
 *    <tr><td>BUTTONS_BUTTON1</td><td>SW0 Button</td><td>Low</td><td>PORTE.5</td></tr>
 *    <tr><td>BUTTONS_BUTTON2</td><td>SW1 Button</td><td>Low</td><td>PORTF.1</td></tr>
 *    <tr><td>BUTTONS_BUTTON3</td><td>SW2 Button</td><td>Low</td><td>PORTF.2</td></tr>
 *  </table>
 *
 *  @{
 */

#ifndef __BUTTONS_A3BU_XPLAINED_H__
#define __BUTTONS_A3BU_XPLAINED_H__

	/* Includes: */
		#include <avr/io.h>

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_BUTTONS_H)
			#error Do not include this file directly. Include LUFA/Drivers/Board/Buttons.h instead.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Button mask for the first button on the board. */
			#define BUTTONS_BUTTON1          (1 << 4)

			/** Button mask for the second button on the board. */
			#define BUTTONS_BUTTON2          (1 << 3)

			/** Button mask for the third button on the board. */
			#define BUTTONS_BUTTON3          (1 << 2)
			
			/** Button mask for the fourth button on the board. */
			#define BUTTONS_BUTTON4          (1 << 1)

		/* Inline Functions: */
		#if !defined(__DOXYGEN__)
			static inline void Buttons_Init(void)
			{
				PORTA.OUTCLR   = BUTTONS_BUTTON1 | BUTTONS_BUTTON2 | BUTTONS_BUTTON3 | BUTTONS_BUTTON4;
				PORTA.PIN1CTRL = (PORT_OPC_PULLUP_gc | PORT_INVEN_bm);
				PORTA.PIN2CTRL = (PORT_OPC_PULLUP_gc | PORT_INVEN_bm);
				PORTA.PIN3CTRL = (PORT_OPC_PULLUP_gc | PORT_INVEN_bm);
				PORTA.PIN4CTRL = (PORT_OPC_PULLUP_gc | PORT_INVEN_bm);
			}

			static inline void Buttons_Disable(void)
			{
				PORTA.OUTCLR   = BUTTONS_BUTTON1 | BUTTONS_BUTTON2 | BUTTONS_BUTTON3 | BUTTONS_BUTTON4;
				PORTA.PIN1CTRL = 0;
				PORTA.PIN2CTRL = 0;
				PORTA.PIN3CTRL = 0;
				PORTA.PIN4CTRL = 0;				
			}

			static inline uint8_t Buttons_GetStatus(void) ATTR_WARN_UNUSED_RESULT;
			static inline uint8_t Buttons_GetStatus(void)
			{
				return (PORTA_IN & (BUTTONS_BUTTON1 | BUTTONS_BUTTON2 | BUTTONS_BUTTON3 | BUTTONS_BUTTON4));
			}
		#endif

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */

