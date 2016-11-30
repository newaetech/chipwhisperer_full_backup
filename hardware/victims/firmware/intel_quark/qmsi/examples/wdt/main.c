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
 * Watchdog timer (WDT)
 *
 * This app sets up a watchdog timer to fire every 4.096ms which results in the
 * callback function being invoked and restarts the WDT.
 *
 * If a second timeout occurs and the WDT has not been reloaded, the SoC resets.
 * In this example, the SoC will not reset as the WDT is reloaded in interrupt
 * callback triggered at every timeout.
 *
 * Note: on Intel(R) Quark(TM) SE, the WDT will automatically pause if the
 * x86 core is in the C2 state. As a result the WDT will not fire or generate
 * interrupts. If using the sensor subsystem, it is advised to use one of the
 * supplied timers to act as a watchdog.
 */

#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_wdt.h"

#define NUM_CALLBACKS (10)

static volatile uint32_t cb_count;
static volatile bool cb_fired = false;

/* Example WDT callback. */
static void wdt_example_callback(void *data)
{
	/* Reload the WDT. */
	qm_wdt_reload(QM_WDT_0);

	++cb_count;
	cb_fired = true;
}

int main(void)
{
	qm_wdt_config_t cfg;

	QM_PUTS("Starting: WDT");

	cb_count = 0;

	/* Configure the  WDT and request the IRQ. */
	cfg.timeout = 1; /* Timeout after 2^17 cycles: 4.096ms @ 32MHz.*/
	cfg.mode = QM_WDT_MODE_INTERRUPT_RESET;
	cfg.callback = wdt_example_callback;
#if (HAS_WDT_PAUSE)
	cfg.pause_en = 0;
#endif /* HAS_WDT_PAUSE */
	cfg.callback_data = NULL;

	qm_wdt_set_config(QM_WDT_0, &cfg);
	qm_irq_request(QM_IRQ_WDT_0_INT, qm_wdt_0_isr);

	/* Start the WDT. */
	qm_wdt_start(QM_WDT_0);

	/* Wait for WDT to fire NUM_CALLBACKS times and then finish. */
	while (cb_count < NUM_CALLBACKS) {
		while (!cb_fired) {
		} /* Wait for the callback to fire. */
		cb_fired = false;
		QM_PRINTF("WDT callback trigger count: %d\n", cb_count);
	}

	QM_PRINTF("Watchdog fired %d times\n", cb_count);

	QM_PUTS("Finished: WDT");

	return 0;
}
