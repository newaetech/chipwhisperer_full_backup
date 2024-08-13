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
 * Comparator
 *
 * This app sets up comparator 0 to fire an interrupt when the input voltage on
 * the pin is greater than the internal reference voltage (0.95V)
 *
 * On the Intel(R) Quark(TM) Microcontroller D2000 Development Platform the pin
 * used is marked "SSO 10".
 *
 * On the Intel(R) Quark(TM) Microcontroller SE Development Platform the pin
 * used is pin 42 on header J15.
 */

#include <stdio.h>
#include "qm_common.h"
#include "qm_comparator.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"

static volatile bool callback_invoked = false;
static volatile uint32_t callback_status;

/* Analog comparator example callback. */
static void ac_example_callback(void *data, uint32_t status)
{
/*
 * The analog comparators use level triggered interrupts so it will trigger a
 * constant stream of interrupts if it is not masked. The following lines mask
 * the interrupts after the first one has fired.
 */
#if (QM_SENSOR)
	QM_INTERRUPT_ROUTER->comparator_0_ss_int_mask |= BIT(0);
#else
	QM_INTERRUPT_ROUTER->comparator_0_host_int_mask |= BIT(0);
#endif

	callback_invoked = true;
	callback_status = status;
}

int main(void)
{
	qm_ac_config_t ac_cfg;

	QM_PUTS("Starting: Analog Comparators");

#if (QM_SENSOR)
	QM_INTERRUPT_ROUTER->comparator_0_ss_int_mask &= ~BIT(0);
#endif

	/* Set up pin muxing and request IRQ. */
	qm_pmux_select(QM_PIN_ID_0, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_0, true);

	/* Request IRQ and write comparator config. */
	qm_irq_request(QM_IRQ_COMPARATOR_0_INT, qm_comparator_0_isr);

	ac_cfg.reference = BIT(0); /* Ref internal voltage. */
	ac_cfg.polarity = 0x0;     /* Fire if greater than ref (high level). */
	ac_cfg.power = BIT(0);     /* Normal operation mode. */
	ac_cfg.int_en = BIT(0);    /* AIN0 enable. */
	ac_cfg.callback = ac_example_callback;

	qm_ac_set_config(&ac_cfg);

	/* Wait for the callback to be invoked and print the status. */
	while (false == callback_invoked)
		;

	QM_PRINTF("Comparator callback status 0x%u\n", callback_status);

	QM_PUTS("Finished: Analog Comparators");

	return 0;
}
