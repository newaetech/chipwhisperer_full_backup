/*
     PDI Programming
     Copyright (C) NewAE Technology Inc., 2014
*/

/*
  Copyright 2014  NewAE Technology Inc. Based on LUFA which is:
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
 *  XPROG Protocol handler, to process V2 Protocol wrapped XPROG commands used in Atmel programmer devices.
 */

#define INCLUDE_FROM_XPROGPROTOCOL_C
#include "XPROGNewAE.h"
#undef INCLUDE_FROM_XPROGPROTOCOL_C
#include "conf_usb.h"
#include <string.h>
#include "XPROGTimeout.h"

/** Base absolute address for the target's NVM controller for PDI programming */
uint32_t XPROG_Param_NVMBase       = 0x010001C0;

/** Size in bytes of the target's EEPROM page */
uint16_t XPROG_Param_EEPageSize    = 32;

/** Address of the TPI device's NVMCMD register for TPI programming */
uint8_t  XPROG_Param_NVMCMDRegAddr = 0x33;

/** Address of the TPI device's NVMCSR register for TPI programming */
uint8_t  XPROG_Param_NVMCSRRegAddr = 0x32;

/* Status of last executed command */
uint8_t XPROG_Status;

/** Handler for the CMD_XPROG command, which wraps up XPROG commands in a V2 wrapper which need to be
 *  removed and processed so that the underlying XPROG command can be handled.
 */
#define XMEGA_BUF_SIZE 256
bool XPROGProtocol_Command(void)
{
	static uint8_t status_payload[4];
	status_payload[0] = udd_g_ctrlreq.req.wValue & 0xff;	
	
	static uint8_t xprog_rambuf[XMEGA_BUF_SIZE];
	uint8_t offset;
	
	switch (status_payload[0])
	{
		case XPROG_CMD_ENTER_PROGMODE:
			start_timeoutcnt();
			XPROGProtocol_EnterXPROGMode();
			break;
		case XPROG_CMD_LEAVE_PROGMODE:
			start_timeoutcnt();
			XPROGProtocol_LeaveXPROGMode();
			break;
		case XPROG_CMD_ERASE:
			start_timeoutcnt();
			XPROGProtocol_Erase();
			break;
		case XPROG_CMD_WRITE_MEM:
			start_timeoutcnt();
			XPROGProtocol_WriteMemory(xprog_rambuf);
			break;
		case XPROG_CMD_READ_MEM:
			start_timeoutcnt();
			XPROGProtocol_ReadMemory(xprog_rambuf);
			break;
			
		case XPROG_CMD_CRC:
			start_timeoutcnt();
			//XPROGProtocol_ReadCRC();
			break;
		case XPROG_CMD_SET_PARAM:
			start_timeoutcnt();
			XPROGProtocol_SetParam();
			break;
			
		//Read data from internal RAM buffer
		case XPROG_GET_RAMBUF:
			offset = (udd_g_ctrlreq.req.wValue >> 8) & 0xff;
			if ((offset + udd_g_ctrlreq.req.wLength) > XMEGA_BUF_SIZE){
				//nice try!
				return false;
			}
		
			udd_g_ctrlreq.payload = xprog_rambuf + offset;
			udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
			return true;
			break;
			
		//Write data to intername RAM buffer
		case XPROG_SET_RAMBUF:
			offset = (udd_g_ctrlreq.req.wValue >> 8) & 0xff;
			if ((offset + udd_g_ctrlreq.req.wLength) > XMEGA_BUF_SIZE){
				//nice try!
				return false;
			}
			
			memcpy(xprog_rambuf + offset, udd_g_ctrlreq.payload, udd_g_ctrlreq.req.wLength);
			return true;
			break;		
			
		case XPROG_GET_STATUS:
			status_payload[1] = XPROG_Status;
			status_payload[2] = (uint8_t)(TimeoutTicksRemaining == 0);
			udd_g_ctrlreq.payload = status_payload;
			udd_g_ctrlreq.payload_size = 3;
			return true;
			break;
	}
	
	stop_timeoutcnt();
	
	return false;
}
/** Handler for the XPROG ENTER_PROGMODE command to establish a connection with the attached device. */
static void XPROGProtocol_EnterXPROGMode(void)
{	
	bool NVMBusEnabled = XMEGANVM_EnablePDI();
	XPROG_Status = NVMBusEnabled ? XPROG_ERR_OK : XPROG_ERR_FAILED;
}

/** Handler for the XPROG LEAVE_PROGMODE command to terminate the PDI programming connection with
 *  the attached device.
 */
static void XPROGProtocol_LeaveXPROGMode(void)
{
    XMEGANVM_DisablePDI();
	XPROG_Status = XPROG_ERR_OK;
}

/** Handler for the XPRG ERASE command to erase a specific memory address space in the attached device. */
static void XPROGProtocol_Erase(void)
{
	XPROG_Status = XPROG_ERR_OK;
	
	if (udd_g_ctrlreq.payload_size < 5){
		XPROG_Status = XPROG_ERR_FAILED;
		return;
	}
	
	uint8_t MemoryType = udd_g_ctrlreq.payload[0]; //Not used
	uint32_t Address = (udd_g_ctrlreq.payload[4] << 24) | (udd_g_ctrlreq.payload[3] << 16) | (udd_g_ctrlreq.payload[2] << 8) | (udd_g_ctrlreq.payload[1]);

	uint8_t EraseCommand;

	/* Determine which NVM command to send to the device depending on the memory to erase */
	switch (MemoryType)
	{
		case XPROG_ERASE_CHIP:
		EraseCommand = XMEGA_NVM_CMD_CHIPERASE;
		break;
		case XPROG_ERASE_APP:
		EraseCommand = XMEGA_NVM_CMD_ERASEAPPSEC;
		break;
		case XPROG_ERASE_BOOT:
		EraseCommand = XMEGA_NVM_CMD_ERASEBOOTSEC;
		break;
		case XPROG_ERASE_EEPROM:
		EraseCommand = XMEGA_NVM_CMD_ERASEEEPROM;
		break;
		case XPROG_ERASE_APP_PAGE:
		EraseCommand = XMEGA_NVM_CMD_ERASEAPPSECPAGE;
		break;
		case XPROG_ERASE_BOOT_PAGE:
		EraseCommand = XMEGA_NVM_CMD_ERASEBOOTSECPAGE;
		break;
		case XPROG_ERASE_EEPROM_PAGE:
		EraseCommand = XMEGA_NVM_CMD_ERASEEEPROMPAGE;
		break;
		case XPROG_ERASE_USERSIG:
		EraseCommand = XMEGA_NVM_CMD_ERASEUSERSIG;
		break;
		default:
		EraseCommand = XMEGA_NVM_CMD_NOOP;
		break;
	}

	/* Erase the target memory, indicate timeout if occurred */
	if (XMEGANVM_EraseMemory(EraseCommand, Address) == false) {
		XPROG_Status = XPROG_ERR_TIMEOUT;		
	}
}

/** Handler for the XPROG WRITE_MEMORY command to write to a specific memory space within the attached device. */
static void XPROGProtocol_WriteMemory(uint8_t * outbuf)
{
	XPROG_Status = XPROG_ERR_OK;
	
	if (udd_g_ctrlreq.req.wLength < 8) {
		XPROG_Status = XPROG_ERR_FAILED;
	}
	
	uint8_t MemoryType = udd_g_ctrlreq.payload[0];
	uint8_t  PageMode = udd_g_ctrlreq.payload[1];
	uint32_t Address = (udd_g_ctrlreq.payload[5] << 24) | (udd_g_ctrlreq.payload[4] << 16) | (udd_g_ctrlreq.payload[3] << 8) | (udd_g_ctrlreq.payload[2]);
	uint16_t Length = udd_g_ctrlreq.payload[6] | (udd_g_ctrlreq.payload[7] << 8);
	
	if (Length > XMEGA_BUF_SIZE) {
		Length = XMEGA_BUF_SIZE;
	}	

	/* Assume FLASH page programming by default, as it is the common case */
	uint8_t WriteCommand     = XMEGA_NVM_CMD_WRITEFLASHPAGE;
	uint8_t WriteBuffCommand = XMEGA_NVM_CMD_LOADFLASHPAGEBUFF;
	uint8_t EraseBuffCommand = XMEGA_NVM_CMD_ERASEFLASHPAGEBUFF;
	bool    PagedMemory      = true;

	switch (MemoryType)
	{
		case XPROG_MEM_TYPE_APPL:
		WriteCommand     = XMEGA_NVM_CMD_WRITEAPPSECPAGE;
		break;
		case XPROG_MEM_TYPE_BOOT:
		WriteCommand     = XMEGA_NVM_CMD_WRITEBOOTSECPAGE;
		break;
		case XPROG_MEM_TYPE_EEPROM:
		WriteCommand     = XMEGA_NVM_CMD_ERASEWRITEEEPROMPAGE;
		WriteBuffCommand = XMEGA_NVM_CMD_LOADEEPROMPAGEBUFF;
		EraseBuffCommand = XMEGA_NVM_CMD_ERASEEEPROMPAGEBUFF;
		break;
		case XPROG_MEM_TYPE_USERSIG:
		WriteCommand     = XMEGA_NVM_CMD_WRITEUSERSIG;
		break;
		case XPROG_MEM_TYPE_FUSE:
		WriteCommand     = XMEGA_NVM_CMD_WRITEFUSE;
		PagedMemory      = false;
		break;
		case XPROG_MEM_TYPE_LOCKBITS:
		WriteCommand     = XMEGA_NVM_CMD_WRITELOCK;
		PagedMemory      = false;
		break;
	}

	/* Send the appropriate memory write commands to the device, indicate timeout if occurred */
	if ((PagedMemory && !(XMEGANVM_WritePageMemory(WriteBuffCommand, EraseBuffCommand, WriteCommand,
												   PageMode, Address,outbuf, Length)))          ||
	     (!PagedMemory && !(XMEGANVM_WriteByteMemory(WriteCommand, Address, outbuf[0]))) )
	{
		XPROG_Status = XPROG_ERR_TIMEOUT;
	}

}



/** Handler for the XPROG READ_MEMORY command to read data from a specific address space within the
 *  attached device.
 */
static void XPROGProtocol_ReadMemory(uint8_t * outbuf)
{
	XPROG_Status = XPROG_ERR_OK;
	//uint8_t MemoryType = udd_g_ctrlreq.payload[0]; //Not used
	uint32_t Address = (udd_g_ctrlreq.payload[4] << 24) | (udd_g_ctrlreq.payload[3] << 16) | (udd_g_ctrlreq.payload[2] << 8) | (udd_g_ctrlreq.payload[1]);
	uint16_t Length = udd_g_ctrlreq.payload[5] | (udd_g_ctrlreq.payload[6] << 8);
	
	if (Length > XMEGA_BUF_SIZE) {
		Length = XMEGA_BUF_SIZE;
	}
	
	//printf("Reading from %x, %d\n", Address, Length);

	/* Read the PDI target's memory, indicate timeout if occurred */
	if (!(XMEGANVM_ReadMemory(Address, outbuf, Length))) {
	  XPROG_Status = XPROG_ERR_TIMEOUT;
	}
}


static void XPROGProtocol_SetParam(void)
{
	XPROG_Status = XPROG_ERR_OK;

	uint8_t XPROGParam = udd_g_ctrlreq.payload[0];
	
	//At least 2 bytes needed - parameters + value
	if (udd_g_ctrlreq.payload_size < 2){
		XPROG_Status = XPROG_ERR_FAILED;
		return;
	}

	/* Determine which parameter is being set, store the new parameter value */
	switch (XPROGParam)
	{
		case XPROG_PARAM_NVMBASE:
			//5 bytes - parameters + value
			if (udd_g_ctrlreq.payload_size < 5){
				XPROG_Status = XPROG_ERR_FAILED;
				return;
			}
			XPROG_Param_NVMBase       = (udd_g_ctrlreq.payload[4] << 24) | (udd_g_ctrlreq.payload[3] << 16) | (udd_g_ctrlreq.payload[2] << 8) | (udd_g_ctrlreq.payload[1]);
			break;
		case XPROG_PARAM_EEPPAGESIZE:
			//3 bytes - parameters + value
			if (udd_g_ctrlreq.payload_size < 3){
				XPROG_Status = XPROG_ERR_FAILED;
				return;
			}
			XPROG_Param_EEPageSize    = udd_g_ctrlreq.payload[1] | (udd_g_ctrlreq.payload[2] << 8);
			break;
		case XPROG_PARAM_NVMCMD_REG:
			XPROG_Param_NVMCMDRegAddr = udd_g_ctrlreq.payload[1];
			break;
		case XPROG_PARAM_NVMCSR_REG:
			XPROG_Param_NVMCSRRegAddr = udd_g_ctrlreq.payload[1];
			break;
		case XPROG_PARAM_TIMEOUT:
			//5 bytes - parameters + value
			if (udd_g_ctrlreq.payload_size < 5){
				XPROG_Status = XPROG_ERR_FAILED;
				return;
			}
			set_timeout((udd_g_ctrlreq.payload[4] << 24) | (udd_g_ctrlreq.payload[3] << 16) | (udd_g_ctrlreq.payload[2] << 8) | (udd_g_ctrlreq.payload[1]));
			break;
		default:
			XPROG_Status = XPROG_ERR_FAILED;
			break;
	}
}

