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
 * Sensor Subsystem (SS) Timer
 *
 * This app configures a timer to expire every 0.5 seconds, at which point the
 * callback function will be invoked to increase the count and a message is
 * printed to the console.
 *
 * This app is specific to the Intel(R) Quark(TM) SE development platform.
 */

#include "qm_ss_interrupt.h"
#include "qm_ss_isr.h"
#include "qm_ss_timer.h"

#define INTERVAL (0x00F42400) /* 0.5 seconds. */
#define NUM_CALLBACK (5)

static volatile uint32_t cb_count = 0;
static volatile bool cb_fired = false;
static volatile bool complete = false;

/* Example timer callback function. */
static void timer_example_callback(void *data)
{
	/* Reset the timer if necessary. */
	if (cb_count < NUM_CALLBACK) {
		cb_count++;
		cb_fired = true;
		qm_ss_timer_set(QM_SS_TIMER_0, 0x00000000);
	} else {
		complete = true;
	}
}

int main(void)
{
	qm_ss_timer_config_t cfg;

	QM_PUTS("Starting: Sensor Timer");

	/*
	 * Request and unmask the timer interrupt. Timer interrupts and
	 * exceptions must be registered with the following function.
	 */
	qm_ss_int_vector_request(QM_ARC_TIMER_0_INT, qm_ss_timer_0_isr);
	qm_ss_irq_unmask(QM_ARC_TIMER_0_INT);

	/* Configure the timer. */
	cfg.watchdog_mode = false;
	cfg.inc_run_only = false;
	cfg.int_en = true;
	cfg.count = INTERVAL;
	cfg.callback = timer_example_callback;
	cfg.callback_data = NULL;

	if (0 != qm_ss_timer_set_config(QM_SS_TIMER_0, &cfg)) {
		QM_PUTS("Error: Set Config for TIMER0 failed");
		return 1;
	}

	/* Set the count. */
	if (0 != qm_ss_timer_set(QM_SS_TIMER_0, 0x00000000)) {
		QM_PUTS("Error: Set Count for TIMER0 failed");
		return 1;
	}

	/* Wait for the app to complete. */
	while (false == complete) {
		if (cb_fired) {
			cb_fired = false;
			QM_PUTS("Timer fired!");
		}
	}

	/* Disable the interrupt, the timer will continue to run. */
	cfg.int_en = false;
	if (0 != qm_ss_timer_set_config(QM_SS_TIMER_0, &cfg)) {
		QM_PUTS("Error: Set Config for TIMER0 failed");
		return 1;
	}

	QM_PUTS("Finished: Sensor Timer");

	return 0;
}
