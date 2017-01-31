/*
 *  Copyright (c) 2016, Intel Corporation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. Neither the name of the Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Real Time Clock (RTC)
 *
 * This app sets up RTC to fire an interrupt every quarter second which results
 * in the callback function being invoked. The app disables the RTC once there
 * have been 5 callbacks.
 */

#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_rtc.h"

#define ALARM_INTERVAL (QM_RTC_ALARM_SECOND(CLK_RTC_DIV_4)) /* 0.25 second. */
#define NUM_CALLBACKS (5)

static volatile uint32_t cb_count = 0;
static volatile bool cb_fired = false;

/* Example RTC callback. */
void rtc_example_callback(void *data)
{
	/* Set alarm to trigger again ALARM_INTERVAL after. */
	qm_rtc_set_alarm(QM_RTC_0,
			 (QM_RTC[QM_RTC_0].rtc_ccvr + ALARM_INTERVAL));

	++cb_count;
	cb_fired = true;
}

int main(void)
{
	qm_rtc_config_t cfg;

	QM_PUTS("Starting: RTC");

	/* Configure RTC and request the interrupt. */
	cfg.init_val = 0;
	cfg.alarm_en = true;
	cfg.alarm_val = ALARM_INTERVAL;
	cfg.callback = rtc_example_callback;
	cfg.callback_data = NULL;
	cfg.prescaler = CLK_RTC_DIV_1;

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	/* Enable RTC. */
	clk_periph_enable(CLK_PERIPH_RTC_REGISTER);

	/* RTC actually starts here. */
	qm_rtc_set_config(QM_RTC_0, &cfg);

	/* Wait for RTC to fire NUM_CALLBACKS times. */
	while (cb_count < NUM_CALLBACKS) {
		/* Wait for the callback to fire. */
		while (!cb_fired) {
		}
		cb_fired = false;
		QM_PRINTF("RTC alarm trigger count: %d\n", cb_count);
	}

	/* Disable RTC. */
	clk_periph_disable(CLK_PERIPH_RTC_REGISTER);

	QM_PRINTF("RTC alarm triggered %d times \n", cb_count);

	QM_PUTS("Finished: RTC");

	return 0;
}
