/*
 Copyright (c) 2015-2016 NewAE Technology Inc. All rights reserved.

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
#include "cdce906.h"

#define CDCE906_ADDR 0x69

/* Init the CDCE906 chip, set offline */
bool cdce906_init(void)
{
	gpio_configure_pin(PIN_CDCE_SDA, PIN_CDCE_SDA_FLAGS);
	gpio_configure_pin(PIN_CDCE_SCL, PIN_CDCE_SCL_FLAGS);
	
	twi_master_options_t opt = {
		.speed = 50000,
		.chip  = CDCE906_ADDR
	};
	
	twi_master_setup(TWI1, &opt);
	
	uint8_t data = 0;
	
	/* Read addr 0 */
	if (cdce906_read(0, &data) == false){
		return false;
	}
	
	/* Check vendor ID matches expected */
	if ((data & 0x0F) == 0x01){
		return true;
	}

	return false;
}

bool cdce906_write(uint8_t addr, uint8_t data)
{
	twi_package_t packet_write = {
		.addr         = {0x80 | addr},      // TWI slave memory address data
		.addr_length  = 1,    // TWI slave memory address data size
		.chip         = CDCE906_ADDR,      // TWI slave bus address
		.buffer       = &data, // transfer data source buffer
		.length       = 1  // transfer data size (bytes)
	};
	
	if (twi_master_write(TWI1, &packet_write) == TWI_SUCCESS){
		return true;
	} else {
		return false;
	}
}

bool cdce906_read(uint8_t addr, uint8_t * data)
{
	twi_package_t packet_read = {
		.addr         = {0x80 | addr},      // TWI slave memory address data
		.addr_length  = 1,    // TWI slave memory address data size
		.chip         = CDCE906_ADDR,      // TWI slave bus address
		.buffer       = data,        // transfer data destination buffer
		.length       = 1,                    // transfer data size (bytes)
	};
	
	if(twi_master_read(TWI1, &packet_read) == TWI_SUCCESS){
		return true;
	} else {
		return false;
	}	
}