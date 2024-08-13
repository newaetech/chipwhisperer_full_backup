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
 * Quark SE C1000 Power Core
 *
 * This example demonstrates C1, C2 and C2LP core states.
 *
 * The core is woken up by the PIC timer.
 *
 * States executed in this example are:
 * C1: Processor clock gated
 * C2: Processor clock gated, gateway to LPSS state
 * C2LP: Processor complex clock gated
 */

#include "power_states.h"
#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pic_timer.h"
#include "qm_rtc.h"

int main(void)
{
	qm_pic_timer_config_t pic_cfg;
	qm_rtc_config_t rtc_cfg;

	QM_PUTS("Starting: Power Core");

	/* PIC timer to wake up from C1/C2/C2LP. */
	pic_cfg.mode = QM_PIC_TIMER_MODE_PERIODIC;
	pic_cfg.int_en = true;
	pic_cfg.callback = NULL;
	pic_cfg.callback_data = NULL;

	qm_int_vector_request(QM_X86_PIC_TIMER_INT_VECTOR, qm_pic_timer_0_isr);

	qm_pic_timer_set_config(&pic_cfg);

	/* One interrupt per second with a 32MHz sysclk. */
	qm_pic_timer_set(0x2000000);

	QM_PUTS("Go to C1.");

	/* Halt the CPU, PIC TIMER INT will wake me up. */
	power_cpu_c1();

	QM_PUTS("Wake up from C1.\nGo to C2.");

	/* Go to C2, PIC TIMER INT will wake me up. */
	power_cpu_c2();

	QM_PUTS("Wake up from C2.");

	/*
	 * C2 Low Power state disables the LAPIC which also disables the usage
	 * of the PIC timer. Disable PIC timer and use RTC instead to wake up
	 * from C2LP.
	 */

	/* Remove PIC timer interrupts. */
	pic_cfg.int_en = false;
	qm_pic_timer_set_config(&pic_cfg);

	/* Initialise RTC configuration and request the IRQ. */
	rtc_cfg.init_val = 0;
	rtc_cfg.alarm_en = true;
	rtc_cfg.alarm_val = QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1) << 2;
	rtc_cfg.callback = NULL;
	rtc_cfg.callback_data = NULL;
	rtc_cfg.prescaler = CLK_RTC_DIV_1;
	qm_rtc_set_config(QM_RTC_0, &rtc_cfg);

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	QM_PUTS("Go to C2LP.");

	/* Go to C2LP, RTC will wake me up. */
	power_cpu_c2lp();

	QM_PUTS("Wake up from C2LP.");

	QM_PUTS("Finished: Power Core");

	return 0;
}
