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

#ifndef FPGA_XMEM_H_
#define FPGA_XMEM_H_

#include <stdint.h>

typedef enum fpga_lockstatus_e {fpga_unlocked = 0, fpga_generic, fpga_blockin, fpga_streamin, fpga_blockout, fpga_ctrlmem, fpga_usblocked} fpga_lockstatus_t;
// Try to get the FPGA lock. Returns 1 if it's ours and 0 if we cannot proceed.
// lockstatus: the lock we're trying to apply
// interrupts_on: should interrupts be enabled 
int FPGA_setlock(fpga_lockstatus_t lockstatus);

// Release the lock.
void FPGA_releaselock(void);

// Check the lock status. Useful in the USB stack
fpga_lockstatus_t FPGA_lockstatus(void);

// Useful functions for entering/exiting critical sections
int try_enter_cs(void);
void exit_cs(void);

// Read 4 bytes into a single uint32_t
uint32_t unsafe_readuint32(uint16_t fpgaaddr);
uint32_t safe_readuint32(uint16_t fpgaaddr);

// Read a number of bytes into a byte array
void unsafe_readbytes(uint16_t fpgaaddr, uint8_t* data, int numBytes);
void safe_readbytes(uint16_t fpgaaddr, uint8_t* data, int numBytes);

// Write a number of bytes to the FPGA
void unsafe_writebytes(uint16_t fpgaaddr, uint8_t* data, int numBytes);

void FPGA_setaddr(uint32_t addr);

void smc_fasttiming(void);
void smc_normaltiming(void);

/* Access pointer for FPGA Interface */
#define PSRAM_BASE_ADDRESS         (0x60000000)
extern uint8_t volatile *xram;


#endif /* FPGA_XMEM_H_ */