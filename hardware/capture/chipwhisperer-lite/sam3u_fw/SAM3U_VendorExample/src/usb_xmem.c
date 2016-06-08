/*
 Copyright (c) 2015 NewAE Technology Inc. All rights reserved.

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
#include "fpga_xmem.h"

/* Access pointer for FPGA Interface */
uint8_t volatile *xram = (uint8_t *) PSRAM_BASE_ADDRESS;

static volatile fpga_lockstatus_t _fpga_locked = fpga_unlocked;

void FPGA_setlock(fpga_lockstatus_t lockstatus)
{
	_fpga_locked = lockstatus;
}

fpga_lockstatus_t FPGA_lockstatus(void)
{
	return _fpga_locked;
}

void FPGA_setaddr(uint32_t addr)
{
	pio_sync_output_write(FPGA_ADDR_PORT, addr);
	gpio_set_pin_low(FPGA_ALE_GPIO);
	gpio_set_pin_high(FPGA_ALE_GPIO);
}

/*
Read four bytes from a given register, return as 32-bit number.

"Unsafe" as doesn't check/modify locking status.
*/
uint32_t unsafe_readuint32(uint16_t fpgaaddr)
{
	uint32_t data;
	
	FPGA_setaddr(fpgaaddr);
	data = *xram;
	data |= *(xram+1) << 8;
	data |= *(xram+2) << 16;
	data |= *(xram+3) << 24;
	return data;
}

// Read numBytes bytes from memory
void unsafe_readbytes(uint16_t fpgaaddr, uint8_t* data, int numBytes)
{
	FPGA_setaddr(fpgaaddr);
	
	for(int i = 0; i < numBytes; i++)
	{
		data[i] = *(xram+i);
	}
}

// Write 4 bytes to memory
void unsafe_writebytes(uint16_t fpgaaddr, uint8_t* data, int numBytes)
{
	FPGA_setaddr(fpgaaddr);
	
	for(int i = 0; i < numBytes; i++)
	{
		*(xram+i) = data[i];
	}
}

