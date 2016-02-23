/*
   Xilinx Spartan 6 FPGA Programming Routines
 
  Copyright (c) 2014-2016 NewAE Technology Inc.  All rights reserved.
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
#include "fpga_program.h"
#include "spi.h"

/* FPGA Programming: Init pins, set to standby state */
void fpga_program_init(void)
{
	FPGA_NPROG_SETUP();
	FPGA_NPROG_HIGH();
}

/* FPGA Programming Step 1: Erase FPGA, setup SPI interface */
void fpga_program_setup1(void)
{
	/* Init - set program low to erase FPGA */
	FPGA_NPROG_LOW();
				
#if FPGA_USE_BITBANG

	FPGA_CCLK_SETUP();
	FPGA_DO_SETUP();
	
#else
				
	spi_enable_clock(SPI);
	spi_reset(SPI);
	spi_set_master_mode(SPI);
	spi_disable_mode_fault_detect(SPI);
	spi_disable_loopback(SPI);
				
	spi_set_clock_polarity(SPI, 0, 0);
	spi_set_clock_phase(SPI, 0, 1);
	spi_set_baudrate_div(SPI, 0, spi_calc_baudrate_div(24000000, sysclk_get_cpu_hz()));
				
	spi_enable(SPI);
				
	gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
					
#endif
}

/* FPGA Programming Step 2: Prepare FPGA for receiving programming data */
void fpga_program_setup2(void)
{
	FPGA_NPROG_HIGH();	
}

/* FPGA Programming Step 3: Send data until done */
void fpga_program_sendbyte(uint8_t databyte)
{
	#if FPGA_USE_BITBANG
	for(unsigned int i=0; i < 8; i++){
		FPGA_CCLK_LOW();
		
		if (databyte & 0x01){
			FPGA_DO_HIGH();
			} else {
			FPGA_DO_LOW();
		}
		
		FPGA_CCLK_HIGH();
		databyte = databyte >> 1;
	}
	#else
	spi_write(SPI, databyte, 0, 0);
	#endif
}
