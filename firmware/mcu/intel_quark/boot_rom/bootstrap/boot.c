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

#include "clk.h"
#include "boot.h"
#include "qm_gpio.h"
#include "qm_pinmux.h"
#include "rom_config.h"
#include "qm_interrupt.h"

void boot_sense_jtag_probe(void)
{
	/* Hardware default (sticky):
	 * - clock gating:
	 *     - QM_CLK_PERIPH_REGISTER and QM_CLK_PERIPH_GPIO_REGISTER enabled
	 *     - QM_CLK_PERIPH_CLK disabled
	 * - pin muxing
	 *     - gpio's muxed out
	 *     - input pads enabled
	 *     - pad pullup disabled
	 * - gpio port configured as input
	 */

	qm_gpio_state_t state;

	qm_pmux_pullup_en(WAIT_FOR_JTAG_PAD, true);
	qm_pmux_select(WAIT_FOR_JTAG_PAD, QM_PMUX_FN_0);
	qm_pmux_input_en(WAIT_FOR_JTAG_PAD, true);
	clk_periph_enable(CLK_PERIPH_REGISTER | CLK_PERIPH_CLK |
			  CLK_PERIPH_GPIO_REGISTER);

	do {
		/* Busy loop to allow JTAG access */
		qm_gpio_read_pin(QM_GPIO_0, WAIT_FOR_JTAG_GPIO, &state);
	} while (state == QM_GPIO_LOW);

	/* Restore hardware default settings */
	clk_periph_disable(CLK_PERIPH_CLK);
	qm_pmux_pullup_en(WAIT_FOR_JTAG_PAD, false);
};

static QM_ISR_DECLARE(aonpt_spurious_isr)
{
	QM_ISR_EOI(QM_IRQ_AONPT_0_INT_VECTOR);
}

static QM_ISR_DECLARE(rtc_spurious_isr)
{
	QM_ISR_EOI(QM_IRQ_RTC_0_INT);
}

static QM_ISR_DECLARE(aon_cmp_spurious_isr)
{
	QM_ISR_EOI(QM_IRQ_COMPARATOR_0_INT);
}

#if (QUARK_SE)
static QM_ISR_DECLARE(aongpio_spurious_isr)
{
	QM_ISR_EOI(QM_IRQ_AON_GPIO_0_INT_VECTOR);
}
#endif

void boot_aon_handle_spurious_irq(void)
{
	/* The PIC IRR register may be asserted by the application before a warm
	 * reset. IRR cannot be cleared by software, so let's just catch this
	 * single spurious interrupt. */
	qm_int_vector_request(QM_IRQ_AONPT_0_INT_VECTOR, aonpt_spurious_isr);
	qm_int_vector_request(QM_IRQ_COMPARATOR_0_INT_VECTOR,
			      aon_cmp_spurious_isr);
	qm_int_vector_request(QM_IRQ_RTC_0_INT_VECTOR, rtc_spurious_isr);
#if (QUARK_SE)
	qm_int_vector_request(QM_IRQ_AON_GPIO_0_INT_VECTOR,
			      aongpio_spurious_isr);
#endif
}
