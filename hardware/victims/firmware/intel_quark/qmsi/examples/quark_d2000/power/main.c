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
 * Quark D2000 Power
 *
 * This app demonstrates the power functions of the Intel(R) Quark(TM)
 * D2000 SoC.
 *
 */

#include "clk.h"
#include "power_states.h"
#include "qm_adc.h"
#include "qm_common.h"
#include "qm_comparator.h"
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_rtc.h"

/*
 * On the Intel(R) Quark(TM) D2000 development platform the wakeup comparator
 * pin is marked as "A5".
 *
 * This pin should be connected to ground before running the example! If not,
 * the IRQ may trigger before going into deep sleep. The callback will turn off
 * the GPIO/comparator IRQ and this will result in the board not being able to
 * recover from deep sleep.
 */
#define WAKEUP_COMPARATOR_PIN (6)
/*
 * Both the analog comparator and GPIO can be used to wake up from deep sleep.
 * The value of USE_COMPARATOR_FOR_DEEP_SLEEP determines which of the two
 * is to be used.
 */
#define USE_COMPARATOR_FOR_DEEP_SLEEP (1)
#define ENABLE_JTAG_PINS (0x0F00000)

#if (USE_COMPARATOR_FOR_DEEP_SLEEP)
static void ac_example_callback()
{
	/*
	 * The analog comparators use level triggered interrupts so we will get
	 * a constant stream of interrupts if we do not mask them. Comment out
	 * the following line if you want to get more than one interrupt.
	 */
	QM_INTERRUPT_ROUTER->comparator_0_host_int_mask |=
	    BIT(WAKEUP_COMPARATOR_PIN);
}
#else
static void gpio_example_callback(void *data, uint32_t status)
{
	/*
	 * The GPIO pin is configured as a level triggered interrupt so we will
	 * get a constant stream of interrupts if we do not mask it. Comment
	 * out the following line if you want to get more than one interrupt.
	 */
	QM_INTERRUPT_ROUTER->gpio_0_int_mask |= BIT(0);
}
#endif
static void rtc_sleep_wakeup()
{

	qm_rtc_config_t rtc_cfg;

	clk_periph_enable(CLK_PERIPH_RTC_REGISTER | CLK_PERIPH_CLK);

	/*
	 * Setup the RTC to get out of sleep mode. Deep sleep will require an
	 * analog comparator interrupt to wake up the system.
	 */
	rtc_cfg.init_val = 0;
	rtc_cfg.alarm_en = 1;
	rtc_cfg.alarm_val = QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1);
	rtc_cfg.callback = NULL;
	rtc_cfg.callback_data = NULL;
	rtc_cfg.prescaler = CLK_RTC_DIV_1;
	qm_rtc_set_config(QM_RTC_0, &rtc_cfg);

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	QM_PUTS("CPU Halt.");
	/* Halt the CPU, RTC alarm will wake. */
	power_cpu_halt();
	QM_PUTS("CPU Halt wakeup.");

	/* Setup wake up isr for RTC. */
	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	/* Set another alarm one second from now. */
	qm_rtc_set_alarm(QM_RTC_0, QM_RTC[QM_RTC_0].rtc_ccvr +
				       QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1));
	QM_PUTS("Go to sleep.");
	/* Go to sleep, RTC will wake. */
	power_soc_sleep();
	QM_PUTS("Wake up from sleep.");

	QM_PUTS("Go to deep sleep with RTC.");
	qm_rtc_set_alarm(QM_RTC_0, QM_RTC[QM_RTC_0].rtc_ccvr +
				       QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1) * 5);
	power_soc_deep_sleep(POWER_WAKE_FROM_RTC);
}

static void comparator_gpio_sleep_wakeup()
{

	uint32_t pmux_sel_save[2], pmux_in_en_save, pmux_pullup_save;

#if (USE_COMPARATOR_FOR_DEEP_SLEEP)
	qm_ac_config_t ac_cfg;
#else
	qm_gpio_port_config_t gpio_cfg;
#endif /* USE_COMPARATOR_FOR_DEEP_SLEEP. */
#if (USE_COMPARATOR_FOR_DEEP_SLEEP)
	ac_cfg.reference =
	    BIT(WAKEUP_COMPARATOR_PIN); /* Ref internal voltage. */
	ac_cfg.polarity = 0x0; /* Fire if greater than ref (high level). */
	ac_cfg.power = BIT(WAKEUP_COMPARATOR_PIN);  /* Normal operation mode. */
	ac_cfg.int_en = BIT(WAKEUP_COMPARATOR_PIN); /* Enable comparator. */
	ac_cfg.callback = ac_example_callback;
	qm_ac_set_config(&ac_cfg);
	qm_irq_request(QM_IRQ_COMPARATOR_0_INT, qm_comparator_0_isr);
#else
	gpio_cfg.direction = 0;
	gpio_cfg.int_en = BIT(WAKEUP_COMPARATOR_PIN); /* Interrupt enabled. */
	gpio_cfg.int_type = 0; /* Level sensitive interrupt. */
	gpio_cfg.int_polarity = BIT(WAKEUP_COMPARATOR_PIN); /* High level. */
	gpio_cfg.int_debounce = 0;	    /* Debounce disabled. */
	gpio_cfg.int_bothedge = 0;	    /* Both edge disabled. */
	QM_GPIO[QM_GPIO_0]->gpio_ls_sync = 0; /* No synchronisation. */
	gpio_cfg.callback = gpio_example_callback;
	qm_irq_request(QM_IRQ_GPIO_0_INT, qm_gpio_0_isr);
	qm_gpio_set_config(QM_GPIO_0, &gpio_cfg);
#endif /* USE_COMPARATOR_FOR_DEEP_SLEEP. */

/*
 * Comparator pin will fire an interrupt when the input voltage is greater than
 * the reference voltage (0.95V). However, if the wakeup source is GPIO, the
 * voltage must be raised to 3V for the interrupt to fire.
 *
 * In order to minimise power, pmux_sel must be set to 0, input enable
 * must be cleared for any pins not expecting to be used to wake the
 * SoC from deep sleep mode, in this example we are using AC / GPIO 6.
 */
#if (USE_COMPARATOR_FOR_DEEP_SLEEP)
	qm_pmux_select(QM_PIN_ID_6, QM_PMUX_FN_1);
#else
	qm_pmux_select(QM_PIN_ID_6, QM_PMUX_FN_0);
#endif /* USE_COMPARATOR_FOR_DEEP_SLEEP. */

	qm_pmux_input_en(QM_PIN_ID_6, true);
	pmux_sel_save[0] = QM_SCSS_PMUX->pmux_sel[0];
	pmux_sel_save[1] = QM_SCSS_PMUX->pmux_sel[1];
	pmux_in_en_save = QM_SCSS_PMUX->pmux_in_en[0];

	pmux_pullup_save = QM_SCSS_PMUX->pmux_pullup[0];

	QM_SCSS_PMUX->pmux_sel[0] = QM_SCSS_PMUX->pmux_sel[1] = 0;
	QM_SCSS_PMUX->pmux_in_en[0] =
	    BIT(WAKEUP_COMPARATOR_PIN) | ENABLE_JTAG_PINS;
	QM_SCSS_PMUX->pmux_pullup[0] = 0;

	/* Mux out comparator. */
	qm_pmux_select(QM_PIN_ID_6, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_6, true);

	power_soc_deep_sleep(POWER_WAKE_FROM_GPIO_COMP);

	/* Restore previous pinmuxing settings. */
	QM_SCSS_PMUX->pmux_sel[0] = pmux_sel_save[0];
	QM_SCSS_PMUX->pmux_sel[1] = pmux_sel_save[1];
	QM_SCSS_PMUX->pmux_in_en[0] = pmux_in_en_save;
	QM_SCSS_PMUX->pmux_pullup[0] = pmux_pullup_save;
}

int main(void)
{

	QM_PUTS("Starting: Low power mode.");

	/* Set up the RTC to wake up the SoC from sleep. */
	rtc_sleep_wakeup();
	QM_PUTS("Wake up from deep sleep RTC.");

	/*
	 * At this stage the physical step of raising the voltage on the
	 * comparator pin is required to proceed.
	 *
	 * Go to deep sleep, a comparator should wake the SoC.
	 */
	QM_PUTS("Go to deep sleep.");

	/* Use a comparator to wake up the SoC from deep sleep. */
	comparator_gpio_sleep_wakeup();

	QM_PUTS("Wake up from deep sleep.");

	QM_PUTS("Finished: Low power mode.");

	return 0;
}
