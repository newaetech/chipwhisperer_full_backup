/*
             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
 *
 *  Target-related functions for the PDI Protocol decoder.
 */

#define  INCLUDE_FROM_XPROGTARGET_C
#include "XPROGTarget.h"
#include "board.h"
#include "usart.h"
#include "sysclk.h"
#include "gpio.h"
#include "ioport.h"
#include "delay.h"

/** Flag to indicate if the USART is currently in Tx or Rx mode. */
static bool IsSending;

/** Enables the target's PDI interface, holding the target in reset until PDI mode is exited. */
void XPROGTarget_EnableTargetPDI(void)
{
	IsSending = false;
	
	/* Turn on clock */
	sysclk_enable_peripheral_clock(USART_PDI_ID);

	/* Set Tx and XCK as outputs, Rx as input */
	gpio_configure_pin(PIN_PDIDTX_GPIO, PIN_PDIDTX_OUT_FLAGS);
	gpio_configure_pin(PIN_PDIDRX_GPIO, PIN_PDIDRX_FLAGS);
	gpio_configure_pin(PIN_PDIC_GPIO, PIN_PDIC_OUT_FLAGS);

	delay_us(50);

	/* Set DATA line high for at least 90ns to disable /RESET functionality */
	gpio_set_pin_high(PIN_PDIDTX_GPIO);
	delay_us(10);

	/* Set up the synchronous USART for XMEGA communications - 8 data bits, even parity, 2 stop bits */
	const sam_usart_opt_t usart_pdid_settings = {
		PDI_BAUD_RATE,
		US_MR_CHRL_8_BIT,
		US_MR_PAR_EVEN,
		US_MR_NBSTOP_2_BIT,
		US_MR_CHMODE_NORMAL
	};
	usart_init_sync_master(USART_PDI, &usart_pdid_settings, sysclk_get_main_hz());

	/* Turn on clock */
	gpio_configure_pin(PIN_PDIC_GPIO, PIN_PDIC_USART_FLAGS);

	/* Send two IDLEs of 12 bits each to enable PDI interface (need at least 16 idle bits) */
	XPROGTarget_SendIdle();
	XPROGTarget_SendIdle();
}

/** Disables the target's PDI interface, exits programming mode and starts the target's application. */
void XPROGTarget_DisableTargetPDI(void)
{
	/* Switch to Rx mode to ensure that all pending transmissions are complete */
	if (IsSending)
	  XPROGTarget_SetRxMode();

	/* Turn off receiver and transmitter of the USART, clear settings */
	usart_disable_rx(USART_PDI);
	usart_disable_tx(USART_PDI);

	/* Tristate all pins */
	gpio_configure_pin(PIN_PDIC_GPIO, PIN_PDIC_IN_FLAGS);
	gpio_configure_pin(PIN_PDIDRX_GPIO, PIN_PDIDRX_FLAGS);
	gpio_configure_pin(PIN_PDIDTX_GPIO, PIN_PDIDTX_IN_FLAGS);
	
	/* Turn off USART */
	sysclk_disable_peripheral_clock(USART_PDI_ID);
}

/** Sends a byte via the USART.
 *
 *  \param[in] Byte  Byte to send through the USART
 */
void XPROGTarget_SendByte(const uint8_t data)
{
	/* Switch to Tx mode if currently in Rx mode */
	if (!(IsSending))
	  XPROGTarget_SetTxMode();

	/* Wait until there is space in the hardware Tx buffer before writing */
	usart_putchar(USART_PDI, data);	
}

/** Receives a byte via the hardware USART, blocking until data is received or timeout expired.
 *
 *  \return Received byte from the USART
 */
uint8_t XPROGTarget_ReceiveByte(void)
{
	uint32_t dummy_read;
		
	/* Switch to Rx mode if currently in Tx mode */
	if (IsSending)
	  XPROGTarget_SetRxMode();

	/* Wait until a byte has been received before reading */
	//usart_getchar(USART_PDI, &dummy_read);
	while((usart_read(USART_PDI, &dummy_read) == 1) && (TimeoutTicksRemaining));

	return dummy_read;
}

/** Sends an IDLE via the USART to the attached target, consisting of a full frame of idle bits. */
void XPROGTarget_SendIdle(void)
{
	/* Switch to Tx mode if currently in Rx mode */
	if (!(IsSending))
	  XPROGTarget_SetTxMode();

	/* Need to do nothing for a full frame to send an IDLE */
	for (uint8_t i = 0; i < BITS_IN_USART_FRAME; i++)
	{
		/* Wait for a full cycle of the clock */
		while(gpio_pin_is_high(PIN_PDIC_GPIO) && TimeoutTicksRemaining);
		while(gpio_pin_is_low(PIN_PDIC_GPIO) && TimeoutTicksRemaining);
		while(gpio_pin_is_high(PIN_PDIC_GPIO) && TimeoutTicksRemaining);
	}
}

static void XPROGTarget_SetTxMode(void)
{
	/* Wait for a full cycle of the clock */
	while(gpio_pin_is_high(PIN_PDIC_GPIO) && TimeoutTicksRemaining);
	while(gpio_pin_is_low(PIN_PDIC_GPIO) && TimeoutTicksRemaining);
	while(gpio_pin_is_high(PIN_PDIC_GPIO) && TimeoutTicksRemaining);
		
	usart_disable_rx(USART_PDI);
	usart_enable_tx(USART_PDI);
	gpio_configure_pin(PIN_PDIDTX_GPIO, PIN_PDIDTX_USART_FLAGS);

	IsSending = true;
}

static void XPROGTarget_SetRxMode(void)
{
	while(usart_is_tx_empty(USART_PDI) == 0);

	usart_disable_tx(USART_PDI);
	usart_enable_rx(USART_PDI);
	
	gpio_configure_pin(PIN_PDIDTX_GPIO, PIN_PDIDTX_IN_FLAGS);
	
	IsSending = false;
}

