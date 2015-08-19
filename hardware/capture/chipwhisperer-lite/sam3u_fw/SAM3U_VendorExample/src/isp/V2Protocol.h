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
 *  Header file for V2Protocol.c.
 */

#ifndef _V2_PROTOCOL_
#define _V2_PROTOCOL_

	/* Includes: */
		#include <asf.h>

		#include "V2ProtocolConstants.h"
		#include "V2ProtocolParams.h"
		#include "ISPProtocol.h"
		#include "../pdi/XPROGTimeout.h"

	/* Macros: */
		/** Programmer ID string, returned to the host during the CMD_SIGN_ON command processing. */
		#define PROGRAMMER_ID              "CWLITE_AVRISP"

		/** Timeout period for each issued command from the host before it is aborted (in 10ms ticks). */
		#define COMMAND_TIMEOUT_TICKS      100

	/* External Variables: */
		extern uint32_t CurrentAddress;
		extern bool     MustLoadExtendedAddress;
		//extern uint32_t TimeoutTicksRemaining;

		extern uint8_t avrisp_status_payload[32];
		extern uint16_t avrisp_status_payload_size;

    	/* Function Prototypes: */
		void V2Protocol_Init(void);
		bool V2Protocol_ProcessCommand(void);

		#if defined(INCLUDE_FROM_V2PROTOCOL_C)
			static void V2Protocol_UnknownCommand(const uint8_t V2Command);
			static void V2Protocol_SignOn(void);
			static void V2Protocol_GetSetParam(const uint8_t V2Command);
			static void V2Protocol_ResetProtection(void);
			static void V2Protocol_LoadAddress(void);
		#endif

#endif

