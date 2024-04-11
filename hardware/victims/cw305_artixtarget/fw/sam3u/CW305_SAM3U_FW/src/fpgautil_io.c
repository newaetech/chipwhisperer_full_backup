/*
   I/O Routines
 
  Copyright (c) 2020 NewAE Technology Inc.  All rights reserved.
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
#include "fpgautil_io.h"
#include "spi.h"

int pin_spi1_mosi = -1;
int pin_spi1_miso = -1;
int pin_spi1_sck = -1;
int pin_spi1_cs = -1;

/* FPGA Programming: Init pins - assumes FPGA already programmed to use these */
void spi1util_init(void)
{
    /* IO setup is done when IO pins are configured right now */
	gpio_set_pin_high(pin_spi1_cs);
	gpio_set_pin_low(pin_spi1_sck);
}

/* SPI CS Pin Low */
void spi1util_cs_low(void)
{
	gpio_set_pin_low(pin_spi1_cs);
}

/* SPI CS Pin High */
void spi1util_cs_high(void)
{
	gpio_set_pin_high(pin_spi1_cs);
}

/* SPI data byte transfer */
uint8_t spi1util_xferbyte(uint8_t databyte)
{
	uint8_t result = 0;

	for(unsigned int i=0; i < 8; i++){	
		//On first read this gets thrown away
		result |= pio_get_pin_value(pin_spi1_miso) ? (1) : 0;
		gpio_set_pin_low(pin_spi1_sck);

		if (databyte & 0x80){
			gpio_set_pin_high(pin_spi1_mosi);
			} else {
			gpio_set_pin_low(pin_spi1_mosi);
		}
		
		gpio_set_pin_high(pin_spi1_sck);
		databyte = databyte << 1;
		result = result << 1;
	}

	result |= pio_get_pin_value(pin_spi1_miso) ? (1) : 0;

	gpio_set_pin_low(pin_spi1_sck);

	return result;
}


void spi1util_deinit(void)
{
	/* IO setup is done when IO pins are configured right now */
}