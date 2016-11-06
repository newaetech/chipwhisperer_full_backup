
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
 * Power SoC Reset app example.
 *
 * This app requires a comparator to wake the SoC from deep sleep. RTC will be
 * explicitly disabled in deep sleep and WDT is disabled in C2. Disabling the
 * RTC is not required but this is done to showcase comparator wake up when in
 * deep sleep mode.
 *
 * On the Intel(R) Quark(TM) SE development platform the comparator pin is
 * located on header J14 pin 20.
 *
 * Wake up from SoC sleep and SoC deep sleep states result in a processor core
 * reset. In order to retain the previous state, this application makes use of
 * General Purpose Register 2.
 *
 * States executed in this example are:
 * SLEEP: Core voltage rail off, peripherals off except Always On.
 *        Wake up from reset vector.
 * DEEP_SLEEP: Core voltage rail off,
 *             1P8V voltage regulator in linear mode,
 *             3P3V voltage regulator in linear mode,
 *             peripherals off except Always On.
 *             Wake up from reset vector.
 * ADVANCED SLEEP: Core voltage rail off.
 *                 1P8V voltage regulator in shutdown mode,
 *                 3P3V voltage regulator in linear mode,
 *                 peripherals off except Always On.
 *                 Wake up from reset vector.
 */

#if (!ENABLE_RESTORE_CONTEXT)
#include "power_soc_common.h"
#include "power_states.h"
#include "qm_common.h"
#include "vreg.h"

#define QM_SCSS_GP_SOC_STATE_SLEEP BIT(0)
#define QM_SCSS_GP_SOC_STATE_DEEP_SLEEP BIT(1)
#define QM_SCSS_GP_SOC_STATE_ADVANCED_SLEEP BIT(2)

static void app_sleep()
{
	setup_rtc_alarm();

	QM_PUTS("Go to sleep.");

	/* Save the state in GPS2 for host wake up information. */
	QM_SCSS_GP->gps2 |= QM_SCSS_GP_SOC_STATE_SLEEP;

	/* Go to sleep, RTC will wake me up. */
	power_soc_sleep();

	/* This is unreachable code and should never be executed. */
	QM_PUTS("Error: Reached unreachable code.");
}

static void app_deep_sleep()
{
	setup_aon_comparator();

	QM_PUTS("Go to deep sleep. Trigger comparator to wake up.");
	/* Save the state in GPS2 for host signal. */
	QM_SCSS_GP->gps2 = QM_SCSS_GP_SOC_STATE_DEEP_SLEEP;
	/* Disable RTC during sleep to be only woken up by external
	 * interrupt. */
	QM_SCSS_PMU->slp_cfg |= QM_SCSS_SLP_CFG_RTC_DIS;
	/* Go to deep sleep, Comparator will wake me up. */
	power_soc_deep_sleep();

	/* This is unreachable code and should never be executed. */
	QM_PUTS("Error: Reached unreachable code.");
}

static void app_advanced_deep_sleep()
{
	setup_aon_comparator();

	QM_PUTS("Go to deep sleep. Trigger comparator to wake up.");

	/* Save the state in GPS2 for host signal. */
	QM_SCSS_GP->gps2 = QM_SCSS_GP_SOC_STATE_ADVANCED_SLEEP;
	/*
	 * Disable RTC during sleep to be only woken up by external
	 * interrupt.
	 */
	QM_SCSS_PMU->slp_cfg |= QM_SCSS_SLP_CFG_RTC_DIS;

	/*
	 * Configure the regulators. This configuration enables lower
	 * power consumption than the deep sleep function. This takes
	 * the assumption that the 1P8 voltage regulator can be safely
	 * turned off. This may not be true for your board.
	 */
	vreg_plat1p8_set_mode(VREG_MODE_SHUTDOWN);
	vreg_plat3p3_set_mode(VREG_MODE_LINEAR);

	/* Enable low power sleep mode. */
	QM_SCSS_PMU->slp_cfg |= QM_SCSS_SLP_CFG_LPMODE_EN;
	QM_SCSS_PMU->pm1c |= QM_SCSS_PM1C_SLPEN;

	/* This is unreachable code and should never be executed. */
	QM_PUTS("Error: Reached unreachable code.");
}

void power_soc_reset(void)
{
	bool start = false, sleep_wakeup = false, deep_sleep_wakeup = false;

	/*
	 * The GPS2 sticky register will retain its value across resets and is
	 * used here to store the previous state of the SoC. The app will have
	 * a different flow if it is starting the first time or coming out of
	 * one of the three sleep states.
	 */
	if (QM_SCSS_GP->gps2 & QM_SCSS_GP_SOC_STATE_SLEEP) {
		sleep_wakeup = true;
		QM_PUTS("SoC states example back from sleep.");
	} else if (QM_SCSS_GP->gps2 & QM_SCSS_GP_SOC_STATE_DEEP_SLEEP) {
		deep_sleep_wakeup = true;
		QM_PUTS("SoC states example back from deep sleep.");
	} else if (QM_SCSS_GP->gps2 & QM_SCSS_GP_SOC_STATE_ADVANCED_SLEEP) {
		QM_PUTS("SoC states example back from advanced sleep.");
	} else {
		start = true;
		QM_PUTS("Starting: Power SoC");
	}

	/* Reset state in GPS2 (sticky register). */
	QM_SCSS_GP->gps2 = 0;

	if (start) {
		QM_PUTS("Press PB0 to go to sleep.");
		wait_user_ready();
		app_sleep();
	} else if (sleep_wakeup) {
		QM_PUTS(
		    "Set comparator pin to Ground and press PB0 when ready.");
		wait_user_ready();
		app_deep_sleep();
	} else if (deep_sleep_wakeup) {
		QM_PUTS(
		    "Set comparator pin to Ground and press PB0 when ready.");
		wait_user_ready();
		app_advanced_deep_sleep();
	}

	QM_PUTS("Finished: Power SoC");
}
#endif /* !ENABLE_RESTORE_CONTEXT */
