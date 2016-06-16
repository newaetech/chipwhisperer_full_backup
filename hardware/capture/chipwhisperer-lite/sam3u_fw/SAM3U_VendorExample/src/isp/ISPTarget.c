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
 *  Target-related functions for the ISP Protocol decoder.
 */

#include "ISPTarget.h"

// frequencies for AVRISP mkII ISP programming
uint32_t avrispmkIIfreqs[] = {
	8000000, 4000000, 2000000, 1000000, 500000, 250000, 125000,
	96386, 89888, 84211, 79208, 74767, 70797, 67227, 64000,
	61069, 58395, 55945, 51613, 49690, 47905, 46243, 43244,
	41885, 39409, 38278, 36200, 34335, 32654, 31129, 29740,
	28470, 27304, 25724, 24768, 23461, 22285, 21221, 20254,
	19371, 18562, 17583, 16914, 16097, 15356, 14520, 13914,
	13224, 12599, 12031, 11511, 10944, 10431, 9963, 9468,
	9081, 8612, 8239, 7851, 7498, 7137, 6809, 6478, 6178,
	5879, 5607, 5359, 5093, 4870, 4633, 4418, 4209, 4019,
	3823, 3645, 3474, 3310, 3161, 3011, 2869, 2734, 2611,
	2484, 2369, 2257, 2152, 2052, 1956, 1866, 1779, 1695,
	1615, 1539, 1468, 1398, 1333, 1271, 1212, 1155, 1101,
	1049, 1000, 953, 909, 866, 826, 787, 750, 715, 682,
	650, 619, 590, 563, 536, 511, 487, 465, 443, 422,
	402, 384, 366, 349, 332, 317, 302, 288, 274, 261,
	249, 238, 226, 216, 206, 196, 187, 178, 170, 162,
	154, 147, 140, 134, 128, 122, 116, 111, 105, 100,
	95, 91, 87, 83, 79, 75, 71, 68,
	65, 62, 59, 56, 54, 51};

/** Initializes the appropriate SPI driver (hardware or software, depending on the selected ISP speed) ready for
 *  communication with the attached target.
 */
void ISPTarget_EnableTargetISP(void)
{
	uint32_t sckfreq = avrispmkIIfreqs[V2Params_GetParameterValue(PARAM_SCK_DURATION)];

#if AVRISP_USEUART
	usart_spi_opt_t spiopts;
	spiopts.baudrate = sckfreq;
	spiopts.char_length = US_MR_CHRL_8_BIT;
	spiopts.channel_mode = US_MR_CHMODE_NORMAL;
	spiopts.spi_mode = SPI_MODE_0;
	
	sysclk_enable_peripheral_clock(AVRISP_USART_ID);
	usart_init_spi_master(AVRISP_USART, &spiopts, sysclk_get_cpu_hz());
	gpio_configure_pin(AVRISP_MISO_GPIO, AVRISP_MISO_FLAGS);
	gpio_configure_pin(AVRISP_MOSI_GPIO, AVRISP_MOSI_FLAGS);
	gpio_configure_pin(AVRISP_SCK_GPIO, AVRISP_SCK_FLAGS);
	usart_enable_tx(AVRISP_USART);
	usart_enable_rx(AVRISP_USART);
#else
	spi_enable_clock(SPI);
	spi_reset(SPI);
	spi_set_master_mode(SPI);
	spi_disable_mode_fault_detect(SPI);
	spi_disable_loopback(SPI);
	
	/* Silently fail and just use faster baud if user requests something too slow */
	int16_t bauddiv = spi_calc_baudrate_div(sckfreq, sysclk_get_cpu_hz());
	if (bauddiv < 0){
		bauddiv = 255;
	}
	
	//printf("SPI... %d %d\n", bauddiv, sckfreq);
		
	spi_set_clock_polarity(SPI, 0, 0);
	spi_set_clock_phase(SPI, 0, 1);
	spi_set_baudrate_div(SPI, 0, bauddiv);
	
	spi_enable(SPI);
	
	gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
#endif

}

/** Shuts down the current selected SPI driver (hardware or software, depending on the selected ISP speed) so that no
 *  further communications can occur until the driver is re-initialized.
 */
void ISPTarget_DisableTargetISP(void)
{
#if AVRISP_USEUART
	usart_disable_tx(AVRISP_USART);
	usart_disable_rx(AVRISP_USART);
	sysclk_disable_peripheral_clock(AVRISP_USART_ID);
#else
	spi_disable(SPI);
	spi_disable_clock(SPI);
#endif
}

/** Asserts or deasserts the target's reset line, using the correct polarity as set by the host using a SET PARAM command.
 *  When not asserted, the line is tristated so as not to interfere with normal device operation.
 *
 *  \param[in] ResetTarget  Boolean true when the target should be held in reset, \c false otherwise
 */
void ISPTarget_ChangeTargetResetLine(const bool ResetTarget)
{	
	if (ResetTarget)
	{
		gpio_configure_pin(PIN_TARG_NRST_GPIO, (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT));

		if (!(V2Params_GetParameterValue(PARAM_RESET_POLARITY))){
		  gpio_set_pin_high(PIN_TARG_NRST_GPIO);
		}else{
    	  gpio_set_pin_low(PIN_TARG_NRST_GPIO);
		}
	}
	else
	{
		gpio_configure_pin(PIN_TARG_NRST_GPIO, (PIO_TYPE_PIO_INPUT | PIO_DEFAULT));
	}
}

/** Waits until the target has completed the last operation, by continuously polling the device's
 *  BUSY flag until it is cleared, or until the command timeout period has expired.
 *
 *  \return V2 Protocol status \ref STATUS_CMD_OK if the no timeout occurred, \ref STATUS_RDY_BSY_TOUT otherwise
 */
uint8_t ISPTarget_WaitWhileTargetBusy(void)
{
	do
	{
		ISPTarget_SendByte(0xF0);
		ISPTarget_SendByte(0x00);
		ISPTarget_SendByte(0x00);
	}
	while ((ISPTarget_TransferByte(0x00) & 0x01) && TimeoutTicksRemaining);

	return (TimeoutTicksRemaining > 0) ? STATUS_CMD_OK : STATUS_RDY_BSY_TOUT;
}

/** Sends a low-level LOAD EXTENDED ADDRESS command to the target, for addressing of memory beyond the
 *  64KB boundary. This sends the command with the correct address as indicated by the current address
 *  pointer variable set by the host when a SET ADDRESS command is issued.
 */
void ISPTarget_LoadExtendedAddress(void)
{
	ISPTarget_SendByte(LOAD_EXTENDED_ADDRESS_CMD);
	ISPTarget_SendByte(0x00);
	ISPTarget_SendByte(CurrentAddress >> 16);
	ISPTarget_SendByte(0x00);
}

/** Waits until the last issued target memory programming command has completed, via the check mode given and using
 *  the given parameters.
 *
 *  \param[in] ProgrammingMode  Programming mode used and completion check to use, a mask of \c PROG_MODE_* constants
 *  \param[in] PollAddress      Memory address to poll for completion if polling check mode used
 *  \param[in] PollValue        Poll value to check against if polling check mode used
 *  \param[in] DelayMS          Milliseconds to delay before returning if delay check mode used
 *  \param[in] ReadMemCommand   Device low-level READ MEMORY command to send if value check mode used
 *
 *  \return V2 Protocol status \ref STATUS_CMD_OK if the no timeout occurred, \ref STATUS_RDY_BSY_TOUT or
 *          \ref STATUS_CMD_TOUT otherwise
 */
uint8_t ISPTarget_WaitForProgComplete(const uint8_t ProgrammingMode,
                                      const uint16_t PollAddress,
                                      const uint8_t PollValue,
                                      const uint8_t DelayMS,
                                      const uint8_t ReadMemCommand)
{
	uint8_t ProgrammingStatus = STATUS_CMD_OK;

	/* Determine method of Programming Complete check */
	switch (ProgrammingMode & ~(PROG_MODE_PAGED_WRITES_MASK | PROG_MODE_COMMIT_PAGE_MASK))
	{
		case PROG_MODE_WORD_TIMEDELAY_MASK:
		case PROG_MODE_PAGED_TIMEDELAY_MASK:
			ISPProtocol_DelayMS(DelayMS);
			break;
		case PROG_MODE_WORD_VALUE_MASK:
		case PROG_MODE_PAGED_VALUE_MASK:
			do
			{
				ISPTarget_SendByte(ReadMemCommand);
				ISPTarget_SendByte(PollAddress >> 8);
				ISPTarget_SendByte(PollAddress & 0xFF);
			}
			while ((ISPTarget_TransferByte(0x00) == PollValue) && TimeoutTicksRemaining);

			if (!(TimeoutTicksRemaining))
			  ProgrammingStatus = STATUS_CMD_TOUT;

			break;
		case PROG_MODE_WORD_READYBUSY_MASK:
		case PROG_MODE_PAGED_READYBUSY_MASK:
			ProgrammingStatus = ISPTarget_WaitWhileTargetBusy();
			break;
	}

	/* Program complete - reset timeout */
	TimeoutTicksRemaining = COMMAND_TIMEOUT_TICKS;

	return ProgrammingStatus;
}
