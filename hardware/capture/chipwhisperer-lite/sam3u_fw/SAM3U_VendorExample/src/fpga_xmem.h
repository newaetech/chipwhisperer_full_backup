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

typedef enum fpga_lockstatus_e {fpga_unlocked = 0, fpga_generic, fpga_blockin, fpga_blockout, fpga_ctrlmem} fpga_lockstatus_t;
void FPGA_setlock(fpga_lockstatus_t lockstatus);
fpga_lockstatus_t FPGA_lockstatus(void);

void FPGA_setaddr(uint32_t addr);

/* Access pointer for FPGA Interface */
#define PSRAM_BASE_ADDRESS         (0x60000000)
extern uint8_t volatile *xram;


#endif /* FPGA_XMEM_H_ */