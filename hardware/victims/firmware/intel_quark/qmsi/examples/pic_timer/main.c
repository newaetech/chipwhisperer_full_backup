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
 * Programmable Interrupt Controller (PIC) Timer
 *
 * This app sets up a PIC timer to fire every 0.125 seconds which results in the
 * callback function being invoked. The app disables the timer once there has
 * been 5 callbacks.
 */

#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pic_timer.h"

#define NUM_CALLBACKS (5)

static volatile unsigned int cb_count;

/* Example timer callback. */
static void example_timer_callback(void *data)
{
	++cb_count;
}

int main(void)
{
	qm_pic_timer_config_t cfg;
	cb_count = 0;
	uint32_t timer_count;

	QM_PUTS("Starting: PIC Timer");

	/* Configure the PIC timer. */
	cfg.mode = QM_PIC_TIMER_MODE_PERIODIC;
	cfg.int_en = true;
	cfg.callback = example_timer_callback;

#if (HAS_APIC) /* Request the interrupt. */
	qm_int_vector_request(QM_X86_PIC_TIMER_INT_VECTOR, qm_pic_timer_0_isr);
#elif(HAS_MVIC)
	qm_irq_request(QM_IRQ_PIC_TIMER, qm_pic_timer_0_isr);
#endif /* HAS_APIC */

	/* Write the config and start the timer. */
	qm_pic_timer_set_config(&cfg);
	qm_pic_timer_set(0x3D0900); /* 0.125 seconds. */

	/* Wait for the correct number of callbacks to be invoked. */
	while (cb_count < NUM_CALLBACKS)
		;

	QM_PRINTF("Number of interrupts: %u\n", cb_count);

	/* Get the current value of the PIC timer and print it. */
	if (qm_pic_timer_get(&timer_count) == 0) {
		QM_PRINTF("Current PIC timer value: %u\n", timer_count);
	} else {
		QM_PUTS("Error: Could not read PIC timer value");
	}

	/* Stop the timer. */
	qm_pic_timer_set(0);

	QM_PUTS("Finished: PIC Timer");

	return 0;
}
