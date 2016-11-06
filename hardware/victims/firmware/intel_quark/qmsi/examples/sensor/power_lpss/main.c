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
 * Sensor Subsystem (SS) Low Power Sensing Standby (LPSS) State
 *
 * This application must run in conjunction with its Host counterpart
 * located in ./examples/quark_se/power_lpss/.
 * Refer to the host application for the board setup.
 *
 * States executed in this example are:
 * LPSS: Combination of C2/C2LP (Host state) and SS2
 */

#include "clk.h"
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_rtc.h"
#include "qm_soc_regs.h"
#include "qm_ss_interrupt.h"
#include "qm_ss_isr.h"
#include "qm_ss_timer.h"
#include "qm_uart.h"
#include "ss_power_states.h"

#define PIN_OUT (0)

#define QM_SCSS_GP_SENSOR_READY BIT(2)

#define RTC_SYNC_CLK_COUNT (5)

#define GPIO_TOGGLE_DELAY 500

int main(void)
{
	qm_rtc_config_t rtc_cfg;
	uint32_t aonc_start;

	/*  Initialise RTC configuration. */
	rtc_cfg.init_val = 0; /* Set initial value to 0. */
	rtc_cfg.alarm_en = 1; /* Enable alarm. */
	rtc_cfg.alarm_val = QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1); /* 1s alarm. */
	rtc_cfg.callback = NULL;
	rtc_cfg.callback_data = NULL;
	rtc_cfg.prescaler = CLK_RTC_DIV_1;
	qm_rtc_set_config(QM_RTC_0, &rtc_cfg);

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

	/*
	 * Enable LPSS by the Sensor Subsystem.
	 * This will clock gate sensor peripherals.
	 */
	ss_power_soc_lpss_enable();

	/*
	 * Signal to the x86 core that the Sensor Subsystem
	 * is ready to enter LPSS mode.
	 */
	QM_SCSS_GP->gps2 |= QM_SCSS_GP_SENSOR_READY;

	/* Go to LPSS, RTC will wake the Sensor Subsystem up. */
	ss_power_cpu_ss2();

	/* Clear the SENSOR_READY flag in General Purpose Sticky Register 2. */
	QM_SCSS_GP->gps2 &= ~QM_SCSS_GP_SENSOR_READY;

	/*
	 * Disable LPSS.
	 * This will restore clock gating of sensor peripherals.
	 */
	ss_power_soc_lpss_disable();

	/* Core still in C2 mode. */
	qm_gpio_clear_pin(QM_GPIO_0, PIN_OUT);
	clk_sys_udelay(GPIO_TOGGLE_DELAY);
	qm_gpio_set_pin(QM_GPIO_0, PIN_OUT);
	clk_sys_udelay(GPIO_TOGGLE_DELAY);
	qm_gpio_clear_pin(QM_GPIO_0, PIN_OUT);

	/* Set another alarm 1 second from now. */
	qm_rtc_set_alarm(QM_RTC_0, QM_RTC[QM_RTC_0].rtc_ccvr +
				       (QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1)));
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

	/*
	 * Enable LPSS by the Sensor Subsystem.
	 * This will clock gate sensor peripherals.
	 */
	ss_power_soc_lpss_enable();

	/*
	 * Signal to the x86 core that the Sensor Subsystem
	 * is ready to enter LPSS mode.
	 */
	QM_SCSS_GP->gps2 |= QM_SCSS_GP_SENSOR_READY;

	/* Go to LPSS, RTC will wake the Sensor Subsystem up. */
	ss_power_cpu_ss2();

	/* Clear the SENSOR_READY flag in General Purpose Sticky Register 2. */
	QM_SCSS_GP->gps2 &= ~QM_SCSS_GP_SENSOR_READY;

	/*
	 * Disable LPSS.
	 * This will restore clock gating of sensor peripherals.
	 */
	ss_power_soc_lpss_disable();

	/* Core still in C2LP mode. */
	qm_gpio_clear_pin(QM_GPIO_0, PIN_OUT);
	clk_sys_udelay(GPIO_TOGGLE_DELAY);
	qm_gpio_set_pin(QM_GPIO_0, PIN_OUT);
	clk_sys_udelay(GPIO_TOGGLE_DELAY);
	qm_gpio_clear_pin(QM_GPIO_0, PIN_OUT);

	return 0;
}
