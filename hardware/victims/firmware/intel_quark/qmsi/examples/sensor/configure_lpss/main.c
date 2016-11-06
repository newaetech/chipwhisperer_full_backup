/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

/*
 * Sensor Subsystem (SS) Low Power Sensing Standby (LPSS) State
 *
 * This application configures the ARC to be ready for LPSS.
 * LPSS will be enabled by a state transition to C2/C2LP from the x86 core
 * after this application has executed on ARC.
 *
 * In order to enter LPSS, this application can run in conjonction with its
 * x86 counterpart located in examples/quark_se/configure_lpss/.
 *
 * Any application on x86 which requires LPSS can use this example.
 *
 * Refer to the x86 example for more details.
 */

#include "qm_interrupt.h"
#include "qm_isr.h"
#include "ss_power_states.h"

/* Mask for all comparator lines */
#define QM_AC_COMPARATORS_MASK (0x7FFFF)

/* Empty ISR, the real handling will be performed by the x86 core. */
static QM_ISR_DECLARE(dummy_isr)
{
}

int main(void)
{
	/*
	 * All interrupts to wake up from LPSS state need
	 * to be registered to transition again to LPSS.
	 *
	 * The only wake events for LPSS are RTC, AON GPIO,
	 * AON Counter and AON Comparator.
	 *
	 * All interrupts below will wake up the Sensor Subsystem.
	 */
	qm_irq_request(QM_IRQ_RTC_0_INT, dummy_isr);
	qm_irq_request(QM_IRQ_COMPARATOR_0_INT, dummy_isr);
	qm_irq_request(QM_IRQ_AONPT_0_INT, dummy_isr);
	qm_irq_request(QM_IRQ_AON_GPIO_0_INT, dummy_isr);

	/* Unmask comparator interrupts for the Sensor Subsystenm */
	QM_INTERRUPT_ROUTER->comparator_0_ss_halt_int_mask &=
	    ~QM_AC_COMPARATORS_MASK;

	/*
	 * Enable LPSS by the Sensor Subsystem.
	 * This will clock gate sensor peripherals.
	 */
	ss_power_soc_lpss_enable();

	/* Loop on SS2 to be ready for LPSS even after wake-up. */
	while (1) {
		ss_power_cpu_ss2();
	}

	return 0;
}
