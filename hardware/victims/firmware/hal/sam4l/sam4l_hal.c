/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2017 NewAE Technology Inc.

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
    
    Portions of this HAL are based on Atmel ASF.
*/

#include "sam4l_hal.h"

#define OSC_MODE_EXTERNAL 0
#define SYSCLK_SRC_OSC0 1

void sysclk_init(void);
void sysclk_priv_enable_module(uint32_t bus_id, uint32_t module_index);
static inline void sysclk_enable_pba_divmask(uint32_t mask);

void sysclk_priv_enable_module(uint32_t bus_id, uint32_t module_index)
{
	//irqflags_t flags;
	uint32_t   mask;

    //Assume IRQ not used in our simple example...
	//flags = cpu_irq_save();

	/* Enable the clock */
	mask = *(&PM->PM_CPUMASK + bus_id);
	mask |= 1U << module_index;
	PM->PM_UNLOCK = PM_UNLOCK_KEY(0xAAu) |
	BPM_UNLOCK_ADDR(((uint32_t)&PM->PM_CPUMASK - (uint32_t)PM) + (4 * bus_id));
	*(&PM->PM_CPUMASK + bus_id) = mask;

	//cpu_irq_restore(flags);
}

static inline void sysclk_enable_pba_divmask(uint32_t mask)
{
	uint32_t temp_mask;

	temp_mask = PM->PM_PBADIVMASK;
	temp_mask |= mask;
	PM->PM_UNLOCK = PM_UNLOCK_KEY(0xAAu)
	| PM_UNLOCK_ADDR((uint32_t)&PM->PM_PBADIVMASK - (uint32_t)PM);
	PM->PM_PBADIVMASK = temp_mask;
}

//! \name Bus index of maskable module clocks
//@{
#define PM_CLK_GRP_CPU               0
#define PM_CLK_GRP_HSB               1
#define PM_CLK_GRP_PBA               2
#define PM_CLK_GRP_PBB               3
#define PM_CLK_GRP_PBC               4
#define PM_CLK_GRP_PBD               5
//@}

//! \name Clocks derived from the CPU clock
//@{
//! On-Chip Debug system
#define SYSCLK_OCD              0
//@}

//! \name Clocks derived from the HSB clock
//@{
//! PDCA memory interface
#define SYSCLK_PDCA_HSB         0
//! Flash data interface
#define SYSCLK_HFLASHC_DATA     1
//! HRAMC data interface
#define SYSCLK_HRAMC1_DATA      2
//! USBC DMA and FIFO interface
#define SYSCLK_USBC_DATA        3
//! CRCCU data interface
#define SYSCLK_CRCCU_DATA       4
//! HSB<->PBA bridge
#define SYSCLK_PBA_BRIDGE       5
//! HSB<->PBB bridge
#define SYSCLK_PBB_BRIDGE       6
//! HSB<->PBC bridge
#define SYSCLK_PBC_BRIDGE       7
//! HSB<->PBD bridge
#define SYSCLK_PBD_BRIDGE       8
//! Advanced Encryption Standard
#define SYSCLK_AESA_HSB         9

#define PBA_DIVMASK_CLK_USART        (1u << 2)
#define SYSCLK_USART0           8

void periclk_usart0_init(void)
{
    if (PM->PM_PBAMASK == 0) {
		sysclk_priv_enable_module(PM_CLK_GRP_HSB, SYSCLK_PBA_BRIDGE);
	}

	/* Enable the module */
	sysclk_priv_enable_module(PM_CLK_GRP_PBA, SYSCLK_USART0);
	sysclk_enable_pba_divmask(PBA_DIVMASK_CLK_USART);
}

void periclk_aesa_init(void)
{
    /* Enable APB clock for AES */
	sysclk_priv_enable_module(PM_CLK_GRP_HSB, SYSCLK_AESA_HSB);
}


void sysclk_init(void)
{
    /* OSC0 Turn-On */
    
    //Turn on OSC0 as external oscillator
	SCIF->SCIF_UNLOCK = SCIF_UNLOCK_KEY(0xAAu) | SCIF_UNLOCK_ADDR((uint32_t)&SCIF->SCIF_OSCCTRL0 - (uint32_t)SCIF);
	SCIF->SCIF_OSCCTRL0 = SCIF_OSCCTRL0_STARTUP(2)	| OSC_MODE_EXTERNAL | SCIF_OSCCTRL0_OSCEN;
	
	//Wait for OSC0 to work
	while((SCIF->SCIF_PCLKSR & SCIF_PCLKSR_OSC0RDY) == 0);
		
	//Set OSC0 as source
	PM->PM_UNLOCK = PM_UNLOCK_KEY(0xAAu)| PM_UNLOCK_ADDR((uint32_t)&PM->PM_MCCTRL - (uint32_t)PM);
	PM->PM_MCCTRL = SYSCLK_SRC_OSC0;	
}


void platform_init(void)
{
    sysclk_init();
}

