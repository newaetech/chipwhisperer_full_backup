/**
 * \file
 *
 * \brief Chip-specific reset cause functions
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef XMEGA_DRIVERS_CPU_RESET_CAUSE_H
#define XMEGA_DRIVERS_CPU_RESET_CAUSE_H

#include "compiler.h"
#include "ccp.h"

/**
 * \ingroup reset_cause_group
 * \defgroup xmega_reset_cause_group XMEGA reset cause
 *
 * See \ref reset_cause_quickstart
 *
 * @{
 */

/**
 * \brief Chip-specific reset cause type capable of holding all chip reset
 * causes. Typically reflects the size of the reset cause register.
 */
typedef uint8_t         reset_cause_t;

//! \internal \name Chip-specific reset causes
//@{
//! \internal External reset cause
#define CHIP_RESET_CAUSE_EXTRST         RST_EXTRF_bm
//! \internal brown-out detected reset cause, same as for CPU
#define CHIP_RESET_CAUSE_BOD_IO         RST_BORF_bm
//! \internal Brown-out detected reset cause, same as for I/O
#define CHIP_RESET_CAUSE_BOD_CPU        RST_BORF_bm
//! \internal On-chip debug system reset cause
#define CHIP_RESET_CAUSE_OCD            RST_PDIRF_bm
//! \internal Power-on-reset reset cause
#define CHIP_RESET_CAUSE_POR            RST_PORF_bm
//! \internal Software reset reset cause
#define CHIP_RESET_CAUSE_SOFT           RST_SRF_bm
//! \internal Spike detected reset cause
#define CHIP_RESET_CAUSE_SPIKE          RST_SDRF_bm
//! \internal Watchdog timeout reset cause
#define CHIP_RESET_CAUSE_WDT            RST_WDRF_bm
//@}

static inline reset_cause_t reset_cause_get_causes(void)
{
	return (reset_cause_t)RST.STATUS;
}

static inline void reset_cause_clear_causes(reset_cause_t causes)
{
	RST.STATUS = causes;
}

static inline void reset_do_soft_reset(void)
{
	ccp_write_io((void *)&RST.CTRL, RST_SWRST_bm);

	while (1) {
		/* Intentionally empty. */
	}
}

//! @}

#endif /* XMEGA_DRIVERS_CPU_RESET_CAUSE_H */
