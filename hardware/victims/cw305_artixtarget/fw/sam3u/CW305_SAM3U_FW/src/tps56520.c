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
#include "tps56520.h"

#define TPS56520_ADDR 0x34

unsigned char checkoddparity(unsigned char p);

/* Is current byte odd-parity already? */
unsigned char checkoddparity(unsigned char p)
{
	p = p ^ (p >> 4 | p << 4);
	p = p ^ (p >> 2);
	p = p ^ (p >> 1);
	return p & 1;
}

/* Init the TPS56520 chip, set to 1.00V output */
bool tps56520_init(void)
{
	gpio_configure_pin(PIN_PWD_SDA, PIN_PWD_SDA_FLAGS);
	gpio_configure_pin(PIN_PWD_SCL, PIN_PWD_SCL_FLAGS);
	
	twi_master_options_t opt = {
		.speed = 50000,
		.chip  = TPS56520_ADDR
	};
	
	twi_master_setup(TWI0, &opt);	
	for(int retry = 3; retry > 0; retry--){
		if(tps56520_set(1000)){
			return true;
		}
	}
	
	return false;
}

/* Set voltage in mV for FPGA VCC_INT Voltage */
bool tps56520_set(uint16_t mv_output)
{
	/* Validate output voltage is in range */
	if ((mv_output < 600) || (mv_output > 1800)){
		return false;
	}
	
	/* Avoid frying FPGA */
	if (mv_output > 1200){
		return false;
	}
	
	uint8_t setting = (mv_output - 600) / 10;
	
	if (!checkoddparity(setting)){
		setting |= 1<<7;
	}
	twi_package_t packet_write = {
		.addr         = {0,0,0},      // TWI slave memory address data
		.addr_length  = 1,    // TWI slave memory address data size
		.chip         = TPS56520_ADDR,      // TWI slave bus address
		.buffer       = &setting, // transfer data source buffer
		.length       = 1  // transfer data size (bytes)
	};
	
	if (twi_master_write(TWI0, &packet_write) != TWI_SUCCESS){
		return false;
	}
	
	uint8_t volt_read;
	
	twi_package_t packet_read = {
		.addr         = {0,0,0},      // TWI slave memory address data
		.addr_length  = 1,    // TWI slave memory address data size
		.chip         = TPS56520_ADDR,      // TWI slave bus address
		.buffer       = &volt_read,        // transfer data destination buffer
		.length       = 1                    // transfer data size (bytes)
	};
	
	if(twi_master_read(TWI0, &packet_read) != TWI_SUCCESS){
		return false;
	}
	
	if (volt_read == setting){
		return true;
	}
	
	return false;
}