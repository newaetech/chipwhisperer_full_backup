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
 *  V2Protocol handler, to process V2 Protocol commands used in Atmel programmer devices.
 */

#define  INCLUDE_FROM_V2PROTOCOL_C
#include "V2Protocol.h"
#include "../pdi/XPROGTimeout.h"
#include <string.h>

/** Current memory address for FLASH/EEPROM memory read/write commands */
uint32_t CurrentAddress;

/** Flag to indicate that the next read/write operation must update the device's current extended FLASH address */
bool MustLoadExtendedAddress;


/** Initializes the hardware and software associated with the V2 protocol command handling. */
void V2Protocol_Init(void)
{
	;
}

static uint8_t status_payload[32];

uint8_t avrisp_status_payload[32];
uint16_t avrisp_status_payload_size;

#define AVR_BUF_SIZE 256

/** Master V2 Protocol packet handler, for received V2 Protocol packets from a connected host.
 *  This routine decodes the issued command and passes off the handling of the command to the
 *  appropriate function.
 */
bool V2Protocol_ProcessCommand(void)
{
	status_payload[0] = udd_g_ctrlreq.req.wValue & 0xff;
	uint8_t V2Command = status_payload[0];
	
	static uint8_t avr_rambuf[AVR_BUF_SIZE];
	uint8_t offset;

	
	/* Reset timeout counter duration and start the timer */
	//TimeoutTicksRemaining = COMMAND_TIMEOUT_TICKS;
	start_timeoutcnt();

	switch (V2Command)
	{
		case CMD_SIGN_ON:
			V2Protocol_SignOn();
			break;
		case CMD_SET_PARAMETER:
		case CMD_GET_PARAMETER:
			V2Protocol_GetSetParam(V2Command);
			break;
		case CMD_LOAD_ADDRESS:
			V2Protocol_LoadAddress();
			break;
		case CMD_RESET_PROTECTION:
			V2Protocol_ResetProtection();
			break;

		case CMD_ENTER_PROGMODE_ISP:
			ISPProtocol_EnterISPMode();
			break;
		case CMD_LEAVE_PROGMODE_ISP:
			ISPProtocol_LeaveISPMode();
			break;
		case CMD_PROGRAM_FLASH_ISP:
		case CMD_PROGRAM_EEPROM_ISP:
			ISPProtocol_ProgramMemory(V2Command, avr_rambuf);
			break;
		case CMD_READ_FLASH_ISP:
		case CMD_READ_EEPROM_ISP:
			ISPProtocol_ReadMemory(V2Command, avr_rambuf);
			break;
		case CMD_CHIP_ERASE_ISP:
			ISPProtocol_ChipErase();
			break;
		case CMD_READ_FUSE_ISP:
		case CMD_READ_LOCK_ISP:
		case CMD_READ_SIGNATURE_ISP:
		case CMD_READ_OSCCAL_ISP:
			ISPProtocol_ReadFuseLockSigOSCCAL(V2Command);
			break;
		case CMD_PROGRAM_FUSE_ISP:
		case CMD_PROGRAM_LOCK_ISP:
			ISPProtocol_WriteFuseLock(V2Command);
			break;
		case CMD_SPI_MULTI:
			ISPProtocol_SPIMulti();
			break;

		case CMD_GET_STATUS:
			udd_g_ctrlreq.payload = avrisp_status_payload;
			udd_g_ctrlreq.payload_size = avrisp_status_payload_size;
			avrisp_status_payload_size = 0;
			return true;
			
			
		//Read data from internal RAM buffer
		case CMD_GET_RAMBUF:
			offset = (udd_g_ctrlreq.req.wValue >> 8) & 0xff;
			if ((offset + udd_g_ctrlreq.req.wLength) > AVR_BUF_SIZE){
				//nice try!
				return false;
			}
		
			udd_g_ctrlreq.payload = avr_rambuf + offset;
			udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
			return true;
			break;
		
		//Write data to intername RAM buffer
		case CMD_SET_RAMBUF:
			offset = (udd_g_ctrlreq.req.wValue >> 8) & 0xff;
			if ((offset + udd_g_ctrlreq.req.wLength) > AVR_BUF_SIZE){
				//nice try!
				return false;
			}
		
			memcpy(avr_rambuf + offset, udd_g_ctrlreq.payload, udd_g_ctrlreq.req.wLength);
			return true;
			break;

		default:
			V2Protocol_UnknownCommand(V2Command);
			break;
	}

	return true;
}

/** Handler for unknown V2 protocol commands. This discards all sent data and returns a
 *  STATUS_CMD_UNKNOWN status back to the host.
 *
 *  \param[in] V2Command  Issued V2 Protocol command byte from the host
 */
static void V2Protocol_UnknownCommand(const uint8_t V2Command)
{
	avrisp_status_payload[1] = STATUS_CMD_UNKNOWN;
	avrisp_status_payload_size = 2;
}

/** Handler for the CMD_SIGN_ON command, returning the programmer ID string to the host. */
static void V2Protocol_SignOn(void)
{
	status_payload[1] = STATUS_CMD_OK;
	status_payload[2] = sizeof(PROGRAMMER_ID) - 1;
	memcpy(status_payload + 3, PROGRAMMER_ID, sizeof(PROGRAMMER_ID) - 1);
	udd_g_ctrlreq.payload = status_payload;
	udd_g_ctrlreq.payload_size = 3+sizeof(PROGRAMMER_ID) - 1;
}

/** Handler for the CMD_RESET_PROTECTION command, implemented as a dummy ACK function as
 *  no target short-circuit protection is currently implemented.
 */
static void V2Protocol_ResetProtection(void)
{
	avrisp_status_payload[1] = STATUS_CMD_OK;
	avrisp_status_payload_size = 2;
}


/** Handler for the CMD_SET_PARAMETER and CMD_GET_PARAMETER commands from the host, setting or
 *  getting a device parameter's value from the parameter table.
 *
 *  \param[in] V2Command  Issued V2 Protocol command byte from the host
 */
static void V2Protocol_GetSetParam(const uint8_t V2Command)
{
	uint8_t ParamID = udd_g_ctrlreq.payload[0];
	uint8_t ParamValue;

	if (V2Command == CMD_SET_PARAMETER) {
	  ParamValue = udd_g_ctrlreq.payload[1];	  
	}

	avrisp_status_payload_size = 2;

	uint8_t ParamPrivs = V2Params_GetParameterPrivileges(ParamID);

	if ((V2Command == CMD_SET_PARAMETER) && (ParamPrivs & PARAM_PRIV_WRITE))
	{
		avrisp_status_payload[1] = STATUS_CMD_OK;
		V2Params_SetParameterValue(ParamID, ParamValue);
	}
	else if ((V2Command == CMD_GET_PARAMETER) && (ParamPrivs & PARAM_PRIV_READ))
	{
		avrisp_status_payload[1] = STATUS_CMD_OK;
		avrisp_status_payload[2] = V2Params_GetParameterValue(ParamID);
		avrisp_status_payload_size += 1;
	}
	else
	{
		avrisp_status_payload[1] = STATUS_CMD_FAILED;
	}
}

/** Handler for the CMD_LOAD_ADDRESS command, loading the given device address into a
 *  global storage variable for later use, and issuing LOAD EXTENDED ADDRESS commands
 *  to the attached device as required.
 */
static void V2Protocol_LoadAddress(void)
{
	CurrentAddress = (udd_g_ctrlreq.payload[3] << 24) | (udd_g_ctrlreq.payload[2] << 16) | (udd_g_ctrlreq.payload[1] << 8) | (udd_g_ctrlreq.payload[0]);

	if (CurrentAddress & (1UL << 31))
	  MustLoadExtendedAddress = true;
	  
	printf("AVRPROG: Address = %02x\n", CurrentAddress);

	avrisp_status_payload[1] = STATUS_CMD_OK;
	avrisp_status_payload_size = 2;
}

