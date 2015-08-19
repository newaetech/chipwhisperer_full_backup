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
 *  ISP Protocol handler, to process V2 Protocol wrapped ISP commands used in Atmel programmer devices.
 */

#include "ISPProtocol.h"
#include "delay.h"
#include <string.h>

//static uint8_t status_payload[32];


/** Handler for the CMD_ENTER_PROGMODE_ISP command, which attempts to enter programming mode on
 *  the attached device, returning success or failure back to the host.
 */
void ISPProtocol_EnterISPMode(void)
{
	struct
	{
		uint8_t TimeoutMS;
		uint8_t PinStabDelayMS;
		uint8_t ExecutionDelayMS;
		uint8_t SynchLoops;
		uint8_t ByteDelay;
		uint8_t PollValue;
		uint8_t PollIndex;
		uint8_t EnterProgBytes[4];
	} Enter_ISP_Params;

	if (udd_g_ctrlreq.payload_size != 11){
		printf("Payload size = %d, error!?\n", udd_g_ctrlreq.payload_size);
		return;
	}

	memcpy(&Enter_ISP_Params, udd_g_ctrlreq.payload, sizeof(Enter_ISP_Params));

	uint8_t ResponseStatus = STATUS_CMD_FAILED;

	CurrentAddress = 0;

	/* Perform execution delay, initialize SPI bus */
	ISPProtocol_DelayMS(Enter_ISP_Params.ExecutionDelayMS);
	ISPTarget_EnableTargetISP();

	ISPTarget_ChangeTargetResetLine(true);
	ISPProtocol_DelayMS(Enter_ISP_Params.PinStabDelayMS);
	
	/* Continuously attempt to synchronize with the target until either the number of attempts specified
	 * by the host has exceeded, or the the device sends back the expected response values */
	while (Enter_ISP_Params.SynchLoops-- && TimeoutTicksRemaining)
	{
		uint8_t ResponseBytes[4];		

		for (uint8_t RByte = 0; RByte < sizeof(ResponseBytes); RByte++)
		{
			ISPProtocol_DelayMS(Enter_ISP_Params.ByteDelay);
			ResponseBytes[RByte] = ISPTarget_TransferByte(Enter_ISP_Params.EnterProgBytes[RByte]);
		}

		/* Check if polling disabled, or if the polled value matches the expected value */
		if (!(Enter_ISP_Params.PollIndex) || (ResponseBytes[Enter_ISP_Params.PollIndex - 1] == Enter_ISP_Params.PollValue))
		{
			ResponseStatus = STATUS_CMD_OK;
			break;
		}
		else
		{
			ISPTarget_ChangeTargetResetLine(false);
			ISPProtocol_DelayMS(Enter_ISP_Params.PinStabDelayMS);
			ISPTarget_ChangeTargetResetLine(true);
			ISPProtocol_DelayMS(Enter_ISP_Params.PinStabDelayMS);
		}
	}

	printf("AVRPROG: Enter ISP Mode: %x\n", ResponseStatus);
	avrisp_status_payload[0] = CMD_ENTER_PROGMODE_ISP;
	avrisp_status_payload[1] = ResponseStatus;
	avrisp_status_payload_size = 2;
}

/** Handler for the CMD_LEAVE_ISP command, which releases the target from programming mode. */
void ISPProtocol_LeaveISPMode(void)
{
	struct
	{
		uint8_t PreDelayMS;
		uint8_t PostDelayMS;
	} Leave_ISP_Params;

	if (udd_g_ctrlreq.payload_size != 2){
		printf("Payload size = %d, error!?\n", udd_g_ctrlreq.payload_size);
		return;
	}

	memcpy(&Leave_ISP_Params, udd_g_ctrlreq.payload, sizeof(Leave_ISP_Params));

	/* Perform pre-exit delay, release the target /RESET, disable the SPI bus and perform the post-exit delay */
	ISPProtocol_DelayMS(Leave_ISP_Params.PreDelayMS);
	ISPTarget_ChangeTargetResetLine(false);
	ISPTarget_DisableTargetISP();
	ISPProtocol_DelayMS(Leave_ISP_Params.PostDelayMS);

	avrisp_status_payload[0] = CMD_LEAVE_PROGMODE_ISP;
	avrisp_status_payload[1] = STATUS_CMD_OK;
	avrisp_status_payload_size = 2;
}

/** Handler for the CMD_PROGRAM_FLASH_ISP and CMD_PROGRAM_EEPROM_ISP commands, writing out bytes,
 *  words or pages of data to the attached device.
 *
 *  \param[in] V2Command  Issued V2 Protocol command byte from the host
 */
void ISPProtocol_ProgramMemory(uint8_t V2Command, uint8_t * buffer)
{
	struct
	{
		uint16_t BytesToWrite;
		uint8_t  ProgrammingMode;
		uint8_t  DelayMS;
		uint8_t  ProgrammingCommands[3];
		uint8_t  PollValue1;
		uint8_t  PollValue2;
	} Write_Memory_Params; 

	if (udd_g_ctrlreq.payload_size != 9){
		printf("Payload size = %d, error!?\n", udd_g_ctrlreq.payload_size);
		return;
	}

	memcpy(&Write_Memory_Params, udd_g_ctrlreq.payload, sizeof(Write_Memory_Params));

	if (Write_Memory_Params.BytesToWrite > 256)
	{
		avrisp_status_payload[0] = V2Command;
		avrisp_status_payload[1] = STATUS_CMD_FAILED;
		avrisp_status_payload_size = 2;
		return;
	}


	uint8_t  ProgrammingStatus = STATUS_CMD_OK;
	uint8_t  PollValue         = (V2Command == CMD_PROGRAM_FLASH_ISP) ? Write_Memory_Params.PollValue1 :
	                                                                    Write_Memory_Params.PollValue2;
	uint16_t PollAddress       = 0;
	uint8_t* NextWriteByte     = buffer;
	uint16_t PageStartAddress  = (CurrentAddress & 0xFFFF);

	for (uint16_t CurrentByte = 0; CurrentByte < Write_Memory_Params.BytesToWrite; CurrentByte++)
	{
		uint8_t ByteToWrite     = *(NextWriteByte++);
		uint8_t ProgrammingMode = Write_Memory_Params.ProgrammingMode;

		/* Check to see if we need to send a LOAD EXTENDED ADDRESS command to the target */
		if (MustLoadExtendedAddress)
		{
			ISPTarget_LoadExtendedAddress();
			MustLoadExtendedAddress = false;
		}

		ISPTarget_SendByte(Write_Memory_Params.ProgrammingCommands[0]);
		ISPTarget_SendByte(CurrentAddress >> 8);
		ISPTarget_SendByte(CurrentAddress & 0xFF);
		ISPTarget_SendByte(ByteToWrite);

		/* AVR FLASH addressing requires us to modify the write command based on if we are writing a high
		 * or low byte at the current word address */
		if (V2Command == CMD_PROGRAM_FLASH_ISP)
		  Write_Memory_Params.ProgrammingCommands[0] ^= READ_WRITE_HIGH_BYTE_MASK;

		/* Check to see if we have a valid polling address */
		if (!(PollAddress) && (ByteToWrite != PollValue))
		{
			if ((CurrentByte & 0x01) && (V2Command == CMD_PROGRAM_FLASH_ISP))
			  Write_Memory_Params.ProgrammingCommands[2] |=  READ_WRITE_HIGH_BYTE_MASK;
			else
			  Write_Memory_Params.ProgrammingCommands[2] &= ~READ_WRITE_HIGH_BYTE_MASK;

			PollAddress = (CurrentAddress & 0xFFFF);
		}

		/* If in word programming mode, commit the byte to the target's memory */
		if (!(ProgrammingMode & PROG_MODE_PAGED_WRITES_MASK))
		{
			/* If the current polling address is invalid, switch to timed delay write completion mode */
			if (!(PollAddress) && !(ProgrammingMode & PROG_MODE_WORD_READYBUSY_MASK))
			  ProgrammingMode = (ProgrammingMode & ~PROG_MODE_WORD_VALUE_MASK) | PROG_MODE_WORD_TIMEDELAY_MASK;

			ProgrammingStatus = ISPTarget_WaitForProgComplete(ProgrammingMode, PollAddress, PollValue,
			                                                  Write_Memory_Params.DelayMS,
			                                                  Write_Memory_Params.ProgrammingCommands[2]);

			/* Abort the programming loop early if the byte/word programming failed */
			if (ProgrammingStatus != STATUS_CMD_OK)
			  break;

			/* Must reset the polling address afterwards, so it is not erroneously used for the next byte */
			PollAddress = 0;
		}

		/* EEPROM just increments the address each byte, flash needs to increment on each word and
		 * also check to ensure that a LOAD EXTENDED ADDRESS command is issued each time the extended
		 * address boundary has been crossed during FLASH memory programming */
		if ((CurrentByte & 0x01) || (V2Command == CMD_PROGRAM_EEPROM_ISP))
		{
			CurrentAddress++;

			if ((V2Command == CMD_PROGRAM_FLASH_ISP) && !(CurrentAddress & 0xFFFF))
			  MustLoadExtendedAddress = true;
		}
	}

	/* If the current page must be committed, send the PROGRAM PAGE command to the target */
	if (Write_Memory_Params.ProgrammingMode & PROG_MODE_COMMIT_PAGE_MASK)
	{
		ISPTarget_SendByte(Write_Memory_Params.ProgrammingCommands[1]);
		ISPTarget_SendByte(PageStartAddress >> 8);
		ISPTarget_SendByte(PageStartAddress & 0xFF);
		ISPTarget_SendByte(0x00);

		/* Check if polling is enabled and possible, if not switch to timed delay mode */
		if ((Write_Memory_Params.ProgrammingMode & PROG_MODE_PAGED_VALUE_MASK) && !(PollAddress))
		{
			Write_Memory_Params.ProgrammingMode = (Write_Memory_Params.ProgrammingMode & ~PROG_MODE_PAGED_VALUE_MASK) |
												   PROG_MODE_PAGED_TIMEDELAY_MASK;
		}

		ProgrammingStatus = ISPTarget_WaitForProgComplete(Write_Memory_Params.ProgrammingMode, PollAddress, PollValue,
		                                                  Write_Memory_Params.DelayMS,
		                                                  Write_Memory_Params.ProgrammingCommands[2]);

		/* Check to see if the FLASH address has crossed the extended address boundary */
		if ((V2Command == CMD_PROGRAM_FLASH_ISP) && !(CurrentAddress & 0xFFFF))
		  MustLoadExtendedAddress = true;
	}

	//printf("AVRPROG: Programming Done: %x\n", ProgrammingStatus);
	avrisp_status_payload[0] = V2Command;
	avrisp_status_payload[1] = ProgrammingStatus;
	avrisp_status_payload_size = 2;

}

/** Handler for the CMD_READ_FLASH_ISP and CMD_READ_EEPROM_ISP commands, reading in bytes,
 *  words or pages of data from the attached device.
 *
 *  \param[in] V2Command  Issued V2 Protocol command byte from the host
 */
void ISPProtocol_ReadMemory(uint8_t V2Command, uint8_t * buffer)
{
	struct
	{
		uint16_t BytesToRead;
		uint8_t  ReadMemoryCommand;
	} Read_Memory_Params;
	
	if (udd_g_ctrlreq.payload_size != 3){
		printf("AVRPROG: Payload size = %d, error!?\n", udd_g_ctrlreq.payload_size);
		return;
	}

	memcpy(&Read_Memory_Params, udd_g_ctrlreq.payload, sizeof(Read_Memory_Params));

	//Default to failed status
	avrisp_status_payload[0] = V2Command;
	avrisp_status_payload[1] = STATUS_CMD_FAILED;
	avrisp_status_payload_size = 2;
		
	//printf("AVRPROG: Attempting to read %d bytes\n", Read_Memory_Params.BytesToRead);

	//Woops... too long
	if (Read_Memory_Params.BytesToRead > 256)
	{
		return;
	}

	/* Read each byte from the device and write them to the packet for the host */
	for (uint16_t CurrentByte = 0; CurrentByte < Read_Memory_Params.BytesToRead; CurrentByte++)
	{
		/* Check to see if we need to send a LOAD EXTENDED ADDRESS command to the target */
		if (MustLoadExtendedAddress)
		{
			ISPTarget_LoadExtendedAddress();
			MustLoadExtendedAddress = false;
		}

		/* Read the next byte from the desired memory space in the device */
		ISPTarget_SendByte(Read_Memory_Params.ReadMemoryCommand);
		ISPTarget_SendByte(CurrentAddress >> 8);
		ISPTarget_SendByte(CurrentAddress & 0xFF);
		//Send Garbage byte, RX data
		buffer[CurrentByte] = ISPTarget_TransferByte(0x00);

		/* AVR FLASH addressing requires us to modify the read command based on if we are reading a high
		 * or low byte at the current word address */
		if (V2Command == CMD_READ_FLASH_ISP)
		  Read_Memory_Params.ReadMemoryCommand ^= READ_WRITE_HIGH_BYTE_MASK;

		/* EEPROM just increments the address each byte, flash needs to increment on each word and
		 * also check to ensure that a LOAD EXTENDED ADDRESS command is issued each time the extended
		 * address boundary has been crossed */
		if ((CurrentByte & 0x01) || (V2Command == CMD_READ_EEPROM_ISP))
		{
			CurrentAddress++;

			if ((V2Command != CMD_READ_EEPROM_ISP) && !(CurrentAddress & 0xFFFF))
			  MustLoadExtendedAddress = true;
		}
	}

	//printf("AVRPROG: Read %d bytes\n", Read_Memory_Params.BytesToRead);
	avrisp_status_payload[1] = STATUS_CMD_OK;
	avrisp_status_payload_size = 2;
}

/** Handler for the CMD_CHI_ERASE_ISP command, clearing the target's FLASH memory. */
void ISPProtocol_ChipErase(void)
{
	struct
	{
		uint8_t EraseDelayMS;
		uint8_t PollMethod;
		uint8_t EraseCommandBytes[4];
	} Erase_Chip_Params;	
	
	if (udd_g_ctrlreq.payload_size != sizeof(Erase_Chip_Params)){
		printf("AVRPROG: Erase: Payload size = %d, error!?\n", udd_g_ctrlreq.payload_size);
		return;
	}

	memcpy(&Erase_Chip_Params, udd_g_ctrlreq.payload, sizeof(Erase_Chip_Params));

	uint8_t ResponseStatus = STATUS_CMD_OK;

	/* Send the chip erase commands as given by the host to the device */
	for (uint8_t SByte = 0; SByte < sizeof(Erase_Chip_Params.EraseCommandBytes); SByte++) {
	  ISPTarget_SendByte(Erase_Chip_Params.EraseCommandBytes[SByte]);
	  //printf("Sending %02x", Erase_Chip_Params.EraseCommandBytes[SByte]);
	}

	/* Use appropriate command completion check as given by the host (delay or busy polling) */
	if (!(Erase_Chip_Params.PollMethod))
	  ISPProtocol_DelayMS(Erase_Chip_Params.EraseDelayMS);
	else
	  ResponseStatus = ISPTarget_WaitWhileTargetBusy();

	//printf("AVRPROG: Chip Erase: %x\n", ResponseStatus);

	avrisp_status_payload[0] = CMD_CHIP_ERASE_ISP;
	avrisp_status_payload[1] = ResponseStatus;
	avrisp_status_payload_size = 2;
}

/** Handler for the CMD_READ_FUSE_ISP, CMD_READ_LOCK_ISP, CMD_READ_SIGNATURE_ISP and CMD_READ_OSCCAL commands,
 *  reading the requested configuration byte from the device.
 *
 *  \param[in] V2Command  Issued V2 Protocol command byte from the host
 */
void ISPProtocol_ReadFuseLockSigOSCCAL(uint8_t V2Command)
{
	struct
	{
		uint8_t RetByte;
		uint8_t ReadCommandBytes[4];
	} Read_FuseLockSigOSCCAL_Params;

	if (udd_g_ctrlreq.payload_size != 5){
		printf("Payload size = %d, error!?\n", udd_g_ctrlreq.payload_size);
		return;
	}

	memcpy(&Read_FuseLockSigOSCCAL_Params, udd_g_ctrlreq.payload, sizeof(Read_FuseLockSigOSCCAL_Params));

	uint8_t ResponseBytes[4];

	/* Send the Fuse or Lock byte read commands as given by the host to the device, store response */
	for (uint8_t RByte = 0; RByte < sizeof(ResponseBytes); RByte++)
	  ResponseBytes[RByte] = ISPTarget_TransferByte(Read_FuseLockSigOSCCAL_Params.ReadCommandBytes[RByte]);

	avrisp_status_payload[0] = V2Command;
	avrisp_status_payload[1] = STATUS_CMD_OK;
	avrisp_status_payload[2] = ResponseBytes[Read_FuseLockSigOSCCAL_Params.RetByte - 1];
	avrisp_status_payload[3] = STATUS_CMD_OK;
		
	avrisp_status_payload_size = 4;	
}

/** Handler for the CMD_WRITE_FUSE_ISP and CMD_WRITE_LOCK_ISP commands, writing the requested configuration
 *  byte to the device.
 *
 *  \param[in] V2Command  Issued V2 Protocol command byte from the host
 */
void ISPProtocol_WriteFuseLock(uint8_t V2Command)
{
	struct
	{
		uint8_t WriteCommandBytes[4];
	} Write_FuseLockSig_Params;
	
	if (udd_g_ctrlreq.payload_size != sizeof(Write_FuseLockSig_Params)){
		printf("Payload size = %d, error!?\n", udd_g_ctrlreq.payload_size);
		return;
	}

	memcpy(&Write_FuseLockSig_Params, udd_g_ctrlreq.payload, sizeof(Write_FuseLockSig_Params));

	/* Send the Fuse or Lock byte program commands as given by the host to the device */
	for (uint8_t SByte = 0; SByte < sizeof(Write_FuseLockSig_Params.WriteCommandBytes); SByte++) {
	  ISPTarget_SendByte(Write_FuseLockSig_Params.WriteCommandBytes[SByte]);
	}

	avrisp_status_payload[0] = V2Command;
	avrisp_status_payload[1] = STATUS_CMD_OK;
	//Supposed to be an extra status byte, but always OK so we don't bother
//	avrisp_status_payload[2] = STATUS_CMD_OK;
	
	avrisp_status_payload_size = 2;	
}

/** Handler for the CMD_SPI_MULTI command, writing and reading arbitrary SPI data to and from the attached device. */
void ISPProtocol_SPIMulti(void)
{
	//Not implemented for this device, alternative methods of accessing SPI interface present
	;
}

/** Blocking delay for a given number of milliseconds. This provides a simple wrapper around
 *  the avr-libc provided delay function, so that the delay function can be called with a
 *  constant value (to prevent run-time floating point operations being required).
 *
 *  \param[in] DelayMS  Number of milliseconds to delay for
 */
void ISPProtocol_DelayMS(uint8_t DelayMS)
{
	if (DelayMS){delay_ms(DelayMS);};
}

