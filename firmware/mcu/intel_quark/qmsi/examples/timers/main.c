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
 * Timer
 *
 * This app demonstrates the basic functionality of the PWM timers by setting a
 * timer which triggers a callback function when it expires.
 */

#include "clk.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pwm.h"

static volatile bool callback_invoked = false;

/* Timers example callback. */
static void timer_example_callback(void *data, uint32_t timer_int)
{
	callback_invoked = true;
}

int main(void)
{
	qm_pwm_config_t cfg;

	QM_PUTS("Starting: Timer");

	/* Initialise timer configuration. */
	cfg.lo_count = 0x100000; /* Timer duration in clock cycles. */
	cfg.hi_count = 0;	/* Unused in timer mode. */
	cfg.mode = QM_PWM_MODE_TIMER_COUNT;    /* PWM timer mode. */
	cfg.mask_interrupt = false;	    /* Enable the interrupt. */
	cfg.callback = timer_example_callback; /* Callback function. */
	cfg.callback_data = NULL;

	/* Enable clocking for the PWM block. */
	clk_periph_enable(CLK_PERIPH_PWM_REGISTER | CLK_PERIPH_CLK);

	/* Set the configuration of the Timer. */
	qm_pwm_set_config(QM_PWM_0, QM_PWM_ID_1, &cfg);
	/* Register the ISR with the SoC. */
	qm_irq_request(QM_IRQ_PWM_0_INT, qm_pwm_0_isr);

	/* Start the Timer. */
	qm_pwm_start(QM_PWM_0, QM_PWM_ID_1);

	/* Wait for the Timer callback to be invoked. */
	while (false == callback_invoked)
		;
	QM_PUTS("Timer fired.");

	/* Stop the Timer from running. */
	qm_pwm_stop(QM_PWM_0, QM_PWM_ID_1);
	/* Disable clocking for the PWM block. */
	clk_periph_disable(CLK_PERIPH_PWM_REGISTER);

	QM_PUTS("Finished: Timer");

	return 0;
}
