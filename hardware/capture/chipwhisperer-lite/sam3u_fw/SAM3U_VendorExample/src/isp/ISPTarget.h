/* This file is based on the excellent LUFA Library, which is:
  Copyright (C) Dean Camera, 2013. See www.lufa-lib.org
  
  Note it has been modified for the SAM3U by NewAE Technology Inc.,
  changes Copyright (C) NewAE Technology Inc, 2015. Changes have
  generally basterdized and reduced the functionality of the original
  code, so please see original code if using this to port. 
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
 *
 *  Header file for ISPTarget.c.
 */

#ifndef _ISP_TARGET_
#define _ISP_TARGET_

	/* Includes: */
		#include <asf.h>
		#include "delay.h"
		#include "spi.h"

		#include "V2Protocol.h"
		#include "ISPProtocol.h"

	/* Macros: */
		/** Low level device command to issue an extended FLASH address, for devices with over 128KB of FLASH. */
		#define LOAD_EXTENDED_ADDRESS_CMD     0x4D

		//By default use SPI, can define this to 1 to use the USART module
		//USART in use is defined by AVRISP_USART
		#ifndef AVRISP_USEUART
		#define AVRISP_USEUART 0
		#define AVRISP_USART USART0
		#define AVRISP_USART_ID ID_USART0
		#endif

	/* Function Prototypes: */
		void    ISPTarget_EnableTargetISP(void);
		void    ISPTarget_DisableTargetISP(void);	
		void    ISPTarget_ChangeTargetResetLine(const bool ResetTarget);
		uint8_t ISPTarget_WaitWhileTargetBusy(void);
		void    ISPTarget_LoadExtendedAddress(void);
		uint8_t ISPTarget_WaitForProgComplete(const uint8_t ProgrammingMode,
		                                      const uint16_t PollAddress,
		                                      const uint8_t PollValue,
		                                      const uint8_t DelayMS,
		                                      const uint8_t ReadMemCommand);

	/* Inline Functions: */
		/** Sends a byte of ISP data to the attached target, using the appropriate SPI hardware or
		 *  software routines depending on the selected ISP speed.
		 *
		 *  \param[in] Byte  Byte of data to send to the attached target
		 */
		static inline void ISPTarget_SendByte(const uint8_t data)
		{
#if AVRISP_USEUART
		  usart_putchar(AVRISP_USART, data);
#else
		  spi_write(SPI, data, 0, 0);
#endif
		}

		/** DO NOT USE DIRECTLY - USE ISPTarget_TransferByte() INSTEAD. This will return
		 *  the last read byte, which is probably not what you intended.
		 *
		 *  \return Received byte of data from the attached target
		 */
		static inline uint8_t ISPTarget_ReceiveByte(void)
		{			
#if AVRISP_USEUART
			uint32_t ReceivedByte;
			usart_getchar(AVRISP_USART, &ReceivedByte);
#else
			uint16_t ReceivedByte;
			uint8_t _;
			spi_read(SPI, &ReceivedByte, &_);
#endif		
			#if defined(INVERTED_ISP_MISO)
			return ~ReceivedByte;
			#else
			return  ReceivedByte;
			#endif
		}

		/** Sends and receives a byte of ISP data to and from the attached target, using the
		 *  appropriate SPI hardware or software routines depending on the selected ISP speed.
		 *
		 *  \param[in] Byte  Byte of data to send to the attached target
		 *
		 *  \return Received byte of data from the attached target
		 */
		static inline uint8_t ISPTarget_TransferByte(const uint8_t data)
		{
			ISPTarget_SendByte(data);
#if AVRISP_USEUART
			while(usart_is_tx_empty(AVRISP_USART) == 0);
#else
			while(spi_is_tx_empty(SPI) == 0);
#endif
			return ISPTarget_ReceiveByte();			
		}

#endif

