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
 * Low Power Sensing Standby (LPSS) State
 *
 * This application must run in conjonction with its Sensor Subsystem
 * counterpart located in examples/sensor/configure_lpss/.
 *
 * Sensor Subsystem collaboration is needed as LPSS can only be achieved as a
 * combination of the two cores states.
 * All LPSS wake event interrupts are registered on the Sensor Subsystem.
 * Once woken up the sensor will transition back to SS2.
 * This transition is mandatory to be able to reach the LPSS state again.
 *
 * The sequence of operation is:
 *  1) Sensor Subsystem registers required ISR to wake up after LPSS.
 *  2) Sensor Subsystem configures the LPSS state and goes to SS2.
 *  3) x86 configures the RTC to trigger an alarm in 1s.
 *  4) x86 transitions to C2. SoC enters the LPSS state.
 *  5) Sensor and x86 wake up with the RTC alarm.
 *  6) Sensor transitions back to SS2.
 *  7) x86 waits 2s before restarting from 3).
 *
 * States executed in this example are:
 * LPSS: Combination of C2 and SS2 (Sensor Subsystem state)
 */

#include "power_states.h"
#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_rtc.h"

#define NB_LOOP (4)
#define RTC_SYNC_CLK_COUNT (5)

static void configure_rtc_alarm(void)
{
	qm_rtc_config_t cfg;
	uint32_t aonc_start;

	cfg.init_val = 0;
	cfg.alarm_en = true;
	cfg.alarm_val = QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1);
	cfg.callback = NULL;
	cfg.callback_data = NULL;
	cfg.prescaler = CLK_RTC_DIV_1;

	qm_rtc_set_config(QM_RTC_0, &cfg);

	/*
	 * The RTC clock resides in a different clock domain
	 * to the system clock.
	 * It takes 3-4 RTC ticks for a system clock write to propagate
	 * to the RTC domain.
	 * If an entry to sleep is initiated without waiting for the
	 * transaction to complete the SOC will not wake from sleep.
	 */
	aonc_start = QM_AONC[0].aonc_cnt;
	while (QM_AONC[0].aonc_cnt - aonc_start < RTC_SYNC_CLK_COUNT) {
	}

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);
}

int main(void)
{
	int i;

	QM_PUTS("Starting: Configure LPSS example.");

	for (i = 1; i <= NB_LOOP; i++) {
		/* Wake up with RTC alarm = 1s. */
		configure_rtc_alarm();

		QM_PRINTF("#%d: Go to LPSS with x86 core in C2.\n", i);

		/*
		 * Go to LPSS.
		 * SS will wake up in SS0 and x86 in C0.
		 * SS will go back to SS2 ready for a new LPSS transition.
		 * In case the C2 state needs to be achieved instead of LPSS,
		 * CCU_SS_LPS_EN in CCU_LP_CLK_CTL register needs to be cleared.
		 */
		power_cpu_c2();

		QM_PRINTF("#%d: Wake up from LPSS.\n", i);

		/* Wait for 2s before next transition. */
		clk_sys_udelay(2000000);
	}

	/*
	 * Disable LPSS to avoid entering that state after the
	 * end of the example.
	 */
	QM_SCSS_CCU->ccu_lp_clk_ctl &= ~QM_SCSS_CCU_SS_LPS_EN;

	QM_PUTS("Finished: Configure LPSS example.");

	return 0;
}
