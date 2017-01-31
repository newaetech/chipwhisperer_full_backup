/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include "soc_boot.h"
#include "interrupt/idt.h"
#include "qm_interrupt.h"
#include "qm_pinmux.h"
#include "boot.h"
#include "boot_clk.h"
#include "flash_layout.h"
#include "power_states.h"
/* Firmware manager hook */
#include "fm_hook.h"

#if (DEBUG)
static QM_ISR_DECLARE(double_fault_isr)
{
	soc_boot_halt_cpu();
}
#endif

/*
 * Version number defines for the ROM
 */

/*
 * ROM Major version number
 */
#define QM_VER_ROM_MAJOR (1)

/*
 * ROM Minor version number
 */
#define QM_VER_ROM_MINOR (1)

/*
 * ROM Patch version number
 */
#define QM_VER_ROM_PATCH (0)

/* ROM version number combined */
static const unsigned int rom_version __attribute__((used))
__attribute__((section(".rom_version"))) =
    (QM_VER_ROM_MAJOR * 10000) + (QM_VER_ROM_MINOR * 100) + QM_VER_ROM_PATCH;

/* Factory settings for Crystal Oscillator */
/* 7.45 pF load cap for Crystal */
#define OSC0_CFG1_OSC0_FADJ_XTAL_DEFAULT (0x4)
/* Crystal count value set to 5375 */
#define OSC0_CFG0_OSC0_XTAL_COUNT_VALUE_DEFAULT (0x2)

/*
 * System power settings
 */
static __inline__ void power_setup(void)
{
#if (QUARK_SE)
	/* Pin MUX slew rate settings */
	QM_PMUX_SLEW0 = QM_PMUX_SLEW_4MA_DRIVER;
	QM_PMUX_SLEW1 = QM_PMUX_SLEW_4MA_DRIVER;
	QM_PMUX_SLEW2 = QM_PMUX_SLEW_4MA_DRIVER;
	QM_PMUX_SLEW3 = QM_PMUX_SLEW_4MA_DRIVER;
#endif
	/*
	 * On Quark D2000, all pins are 12 mA by default; this should be fine
	 * for now.
	 */
}

/*
 * System clock settings
 */
static __inline__ void clock_setup(void)
{
#if (HAS_HYB_XTAL)
	/* Apply factory settings for Crystal Oscillator stabilization
	 * These settings adjust the trimming value and the counter value
	 * for the Crystal Oscillator */
	QM_SCSS_CCU->osc0_cfg1 &= ~OSC0_CFG1_OSC0_FADJ_XTAL_MASK;
	QM_SCSS_CCU->osc0_cfg1 |=
	    (OSC0_CFG1_OSC0_FADJ_XTAL_DEFAULT << OSC0_CFG1_OSC0_FADJ_XTAL_OFFS);
	QM_SCSS_CCU->osc0_cfg0 &= ~OSC0_CFG0_OSC0_XTAL_COUNT_VALUE_MASK;
	QM_SCSS_CCU->osc0_cfg0 |= (OSC0_CFG0_OSC0_XTAL_COUNT_VALUE_DEFAULT
				   << OSC0_CFG0_OSC0_XTAL_COUNT_VALUE_OFFS);
#endif /* HAS_HYB_XTAL */

	/*
	 * Switch to each silicon oscillator to set up trim data
	 * This sets up the trim codes for the first boot.
	 * This consists of computing the trim code if not available
	 * in non volatile memory and write this results in flash.
	 *
	 * This step is only performed if the shadow region
	 * is not populated.
	 * We rely on the 32MHz trim code to be shadowed to
	 * consider the region populated.
	 *
	 * This can be modified if this policy does not match your
	 * specific requirements.
	 */
	if ((QM_FLASH_DATA_TRIM_CODE->osc_trim_32mhz &
	     QM_FLASH_TRIM_PRESENT_MASK) != QM_FLASH_TRIM_PRESENT) {
		boot_clk_trim_code_setup();
	}

	/* Switch to 32MHz silicon oscillator */
	boot_clk_hyb_set_mode(CLK_SYS_HYB_OSC_32MHZ, CLK_SYS_DIV_1);
	clk_trim_apply(QM_FLASH_DATA_TRIM_CODE->osc_trim_32mhz);
}

/*
 * SCSS interrupt routing initialization.
 */
static __inline__ void irq_setup(void)
{
	uint8_t i;

	/* Apply POR settings to SCSS int routing as SCSS regs are sticky */
	for (i = 0; i < QM_INTERRUPT_ROUTER_MASK_NUMREG; i++) {
		*((volatile uint32_t *)QM_INTERRUPT_ROUTER + i) =
		    QM_INTERRUPT_ROUTER_MASK_DEFAULT;
	}
}

/*
 * C runtime initialization.
 * This will be called from rom_startup.s
 */
void rom_startup(void)
{
	void (*lmt_app_entry)(void) = (void *)LMT_APP_ADDR;

	extern uint32_t __bss_start[];
	extern uint32_t __data_vma[];
	extern uint32_t __data_lma[];
	extern uint32_t __data_size[];
	extern uint32_t __bss_end[];

	/* Zero out bss */
	memset(__bss_start, 0x00, (uint32_t)__bss_end - (uint32_t)__bss_start);

	/* Copy initialised variables */
	memcpy(__data_vma, __data_lma, (size_t)__data_size);

	power_setup();
	clock_setup();

	boot_sense_jtag_probe();

	/* Interrupt initialisation */
	irq_setup();
	idt_init();
	boot_aon_handle_spurious_irq();
#if (DEBUG)
	qm_int_vector_request(QM_X86_DOUBLE_FAULT_INT, double_fault_isr);
#endif
	soc_boot_init_interrupt_controller();
	__asm__ __volatile__("sti");

#if (ENABLE_FIRMWARE_MANAGER)
	fm_hook();
#endif /* ENABLE_FIRMWARE_MANAGER */

	/*
	 * Execute application on Lakemont, provided that the application has
	 * been
	 * programmed.
	 */

	/* Execute application, provided that it has been programmed. */
	if (0xffffffff != *(uint32_t *)LMT_APP_ADDR) {
		lmt_app_entry();
	}

	soc_boot_sleep();
}
