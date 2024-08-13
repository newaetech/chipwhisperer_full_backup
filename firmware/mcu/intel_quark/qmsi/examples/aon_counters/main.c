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
 * Always-on (AON) Counters
 *
 * This app first reads the value of the counter and prints it. It then
 * configures the periodic timer to fire a number of times resulting in the
 * callback function being invoked. Finally it prints the number of times
 * the callback was invoked.
 */

#include "qm_aon_counters.h"
#include "qm_interrupt.h"
#include "qm_isr.h"

#define NUM_CALLBACKS (5)

static volatile uint32_t callback_count;

/* Always-on periodic timer example callback function. */
static void aonpt_example_callback()
{
	++callback_count;

	/* Clear the timer so it can fire again. */
	qm_aonpt_clear(QM_AONC_0);
}

int main(void)
{
	uint32_t c_val = 0, pt_val = 0;
	qm_aonpt_config_t cfg;

	QM_PUTS("Starting: Always-on Counter");

	/* Always-on Counter enable and read value. */
	qm_aonc_enable(QM_AONC_0);

	if (qm_aonc_get_value(QM_AONC_0, &c_val) == 0) {
		QM_PRINTF("Always-on Counter value: %u\n", c_val);
	} else {
		QM_PUTS("Error: Could not read aonc value.");
	}

	/* Request an IRQ and write the Always-on Periodic Timer config. */
	cfg.count = 0x1000; /* 0.125 seconds. */
	cfg.int_en = true;  /* Interrupts enabled. */
	cfg.callback = aonpt_example_callback;

	qm_irq_request(QM_IRQ_AONPT_0_INT, qm_aonpt_0_isr);

	qm_aonpt_set_config(QM_AONC_0, &cfg);

	/* Wait for the defined number of callbacks be invoked. */
	while (NUM_CALLBACKS > callback_count)
		;

	QM_PRINTF("Periodic Timer callback fired %d times.\n", callback_count);

	/* Get the value of the Always-on Periodic Timer. */
	if (qm_aonpt_get_value(QM_AONC_0, &c_val) == 0) {
		QM_PRINTF("Always-on Periodic Timer value: %u\n", pt_val);
	} else {
		QM_PUTS("Error: Could not read Periodic timer value");
	}

	/* Disable the always-on periodic timer interrupt. */
	cfg.int_en = false;
	qm_aonpt_set_config(QM_AONC_0, &cfg);

	QM_PUTS("Finished: Always-on counter");

	return 0;
}
