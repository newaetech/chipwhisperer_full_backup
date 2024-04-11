/*
   Xilinx Spartan 6 FPGA Programming Routines
 
  Copyright (c) 2014-2015 NewAE Technology Inc.  All rights reserved.
    Author: Colin O'Flynn, <coflynn@newae.com>
 
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

#include <asf.h>
#include "gpio.h"
#include "fpgaspi_program.h"
#include "spi.h"

/* FPGA Programming: Init pins - assumes FPGA already programmed to use these */
void fpgaspi_program_init(void)
{
	FPGASPI_CCLK_SETUP();
	FPGASPI_DO_SETUP();
	FPGASPI_DI_SETUP();
	FPGASPI_CS_SETUP();
	FPGASPI_CS_HIGH();
	FPGASPI_CCLK_LOW();
}


/* SPI CS Pin Low */
void fpgaspi_cs_low(void)
{
	FPGASPI_CS_LOW();
}

/* SPI CS Pin High */
void fpgaspi_cs_high(void)
{
	FPGASPI_CS_HIGH();
}


/* SPI data byte transfer */
uint8_t fpgaspi_xferbyte(uint8_t databyte)
{
	uint8_t result = 0;

	for(unsigned int i=0; i < 8; i++){	
		//On first read this gets thrown away
		result |= FPGASPI_DI_STATUS() ? (1) : 0;
		FPGASPI_CCLK_LOW();

		if (databyte & 0x80){
			FPGASPI_DO_HIGH();
			} else {
			FPGASPI_DO_LOW();
		}
		
		FPGASPI_CCLK_HIGH();
		databyte = databyte << 1;
		result = result << 1;
	}

	result |= FPGASPI_DI_STATUS() ? (1) : 0;

	FPGASPI_CCLK_LOW();

	return result;
}


void fpgaspi_program_deinit(void)
{
	FPGASPI_CCLK_RELEASE();
	FPGASPI_DO_RELEASE();
	FPGASPI_CS_RELEASE();
}