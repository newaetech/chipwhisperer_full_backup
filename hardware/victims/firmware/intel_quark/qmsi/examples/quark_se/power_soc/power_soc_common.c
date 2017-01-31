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

#include "qm_comparator.h"
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_rtc.h"
#include "qm_uart.h"

#define WAKEUP_COMPARATOR_PIN (13)
#define QM_AC_COMPARATORS_MASK (0x7FFFF)
#define PIN_READY (4)
#define RTC_SYNC_CLK_COUNT (5)

/* Power SoC example callback. */
static void power_soc_callback(void *data, uint32_t status)
{
	/* Set comparator ready to true. */
	*(bool *)data = true;
}

void wait_user_ready(void)
{
	volatile bool ready = false;
	qm_gpio_port_config_t cfg;

	/* Configure the GPIO port and request the IRQ. */
	cfg.direction = 0;		    /* Set all pins as inputs. */
	cfg.int_en = BIT(PIN_READY);	/* Interrupt enabled. */
	cfg.int_type = BIT(PIN_READY);      /* Edge sensitive interrupt. */
	cfg.int_polarity = ~BIT(PIN_READY); /* Falling edge. */
	cfg.int_debounce = BIT(PIN_READY);  /* Debounce enabled. */
	cfg.int_bothedge = 0x0;		    /* Both edge disabled. */
	cfg.callback = power_soc_callback;
	cfg.callback_data = (void *)&ready;

	qm_irq_request(QM_IRQ_AON_GPIO_0_INT, qm_aon_gpio_0_isr);

	qm_gpio_set_config(QM_AON_GPIO_0, &cfg);

	while (ready == false) {
	}
}

void setup_rtc_alarm(void)
{
	qm_rtc_config_t rtc_cfg;
	uint32_t aonc_start;

	/* Configure the RTC and request the IRQ. */
	rtc_cfg.init_val = 0;
	rtc_cfg.alarm_en = true;
	rtc_cfg.alarm_val = QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1) << 2;
	rtc_cfg.callback = NULL;
	rtc_cfg.callback_data = NULL;
	rtc_cfg.prescaler = CLK_RTC_DIV_1;

	qm_rtc_set_config(QM_RTC_0, &rtc_cfg);

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	/*
	 * The RTC clock resides in a different clock domain to the
	 * system clock. It takes 3-4 RTC ticks for a system clock
	 * write to propagate to the RTC domain. If an entry to sleep
	 * is initiated without waiting for the transaction to complete
	 * the SOC will not wake from sleep.
	 */
	aonc_start = QM_AONC[0].aonc_cnt;
	while (QM_AONC[0].aonc_cnt - aonc_start < RTC_SYNC_CLK_COUNT)
		;
}

void setup_aon_comparator(void)
{
	qm_ac_config_t ac_cfg;

	QM_PUTS("Setting up Comparator.");

	/* Set up pin muxing and request IRQ. */
	qm_pmux_select(QM_PIN_ID_13, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_13, true);

	/* Clear all comparator pending interrupts. */
	QM_SCSS_CMP->cmp_pwr = 0;
	QM_SCSS_CMP->cmp_stat_clr = QM_AC_COMPARATORS_MASK;

	/* Configure the comparator and request the IRQ. */
	ac_cfg.reference =
	    BIT(WAKEUP_COMPARATOR_PIN); /* Ref internal voltage. */
	ac_cfg.polarity = 0x0; /* Fire if greater than ref (high level). */
	ac_cfg.power = BIT(WAKEUP_COMPARATOR_PIN);  /* Normal operation mode. */
	ac_cfg.int_en = BIT(WAKEUP_COMPARATOR_PIN); /* Enable comparator. */
	qm_ac_set_config(&ac_cfg);

	qm_irq_request(QM_IRQ_COMPARATOR_0_INT, qm_comparator_0_isr);
}
