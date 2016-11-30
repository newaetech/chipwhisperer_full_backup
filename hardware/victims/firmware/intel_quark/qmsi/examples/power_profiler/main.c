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
 * Power Profiler
 *
 * This app demonstrates how SoCWatch profiles enter different power modes on
 * the Intel(R) Quark(TM) development platforms.
 */

#if (!QM_SENSOR)
#include <x86intrin.h>
#endif

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
#include "soc_watch.h"

/* Configure the test features by defining these with non-zero values. */
#define SLOW_MODE_TEST (1)
#define HALT_TEST (1)
#define SLEEP_TEST (1)
#define DEEP_SLEEP (0)

/*
 * This is used for the deep sleep test. On the Intel(R) Quark(TM) D2000
 * Development Platform this pin is marked as "A5".
 */
#define WAKEUP_COMPARATOR_PIN (6)
#define BOARD_LED_PIN (24)

#if (QUARK_D2000)
#define ENTER_C1() power_cpu_halt()
#define ENTER_C2() power_soc_sleep()
#define ENTER_C2LP() power_soc_deep_sleep()
#elif(QUARK_SE)
void power_cpu_c1(void);
void power_cpu_c2(void);
void power_cpu_c2lp(void);
#define ENTER_C1() power_cpu_c1()
#define ENTER_C2() power_cpu_c2()
#define ENTER_C2LP() power_cpu_c2lp()
#endif

/* QMSI GPIO Pin configuration struct.*/
qm_gpio_port_config_t gpio_cfg;

int rtc_tick = 0;

/* Invert an LED. */
static void led_flip(unsigned int pin)
{
	QM_GPIO[QM_GPIO_0]->gpio_swporta_dr ^= 1 << pin;
}

static void rtc_example_callback()
{
	/* Log the interrupt event. */
	SOC_WATCH_LOG_EVENT(SOCW_EVENT_INTERRUPT, QM_IRQ_RTC_0_INT_VECTOR);

	/* Invert On-board LED. */
	led_flip(BOARD_LED_PIN);
	++rtc_tick;

	/* Reschedule next tick. */
	qm_rtc_set_alarm(QM_RTC_0, (QM_RTC[QM_RTC_0].rtc_ccvr +
				    (QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1) / 2)));
}

static void gpio_init()
{
	gpio_cfg.direction = 0; /* Configure all pins as inputs. */
	gpio_cfg.int_en = 0;    /* Interrupt disabled. */
	gpio_cfg.int_type = 0;  /* Turn off GPIO interrupts. */
	gpio_cfg.int_polarity = 0;
	gpio_cfg.int_debounce = 0;   /* Debounce disabled. */
	gpio_cfg.int_bothedge = 0x0; /* Both edge disabled. */
	gpio_cfg.callback = NULL;
	gpio_cfg.callback_data = NULL;

	qm_gpio_set_config(QM_GPIO_0, &gpio_cfg);
}

/*
 * Configure a GPIO pin (or onboard LED pin) as an output and drive an
 * initial value.
 */
static void gpio_set_out(unsigned int pin, unsigned int initial_value)
{
	qm_pmux_select(pin, QM_PMUX_FN_0); /* Pin Muxing. */
	gpio_cfg.direction = BIT(pin);     /* Configure pin for output. */
	qm_gpio_set_config(QM_GPIO_0, &gpio_cfg);

	if (initial_value) {
		qm_gpio_set_pin(QM_GPIO_0, pin);
	} else {
		qm_gpio_clear_pin(QM_GPIO_0, pin);
	}
}

/*
 * Returns the start and end RTC times for this busy loop.
 * Ideally, by examining the TSC and RTC times, we should be able to
 * identify their correlation.
 */
static uint64_t spin_loop(unsigned int count, unsigned int *rtc_start,
			  unsigned int *rtc_end)
{
	uint64_t start_tsc;

retry:
	*rtc_start = QM_RTC[QM_RTC_0].rtc_ccvr;
	start_tsc = _rdtsc();
	clk_sys_udelay(400);
	*rtc_end = QM_RTC[QM_RTC_0].rtc_ccvr;

	if ((*rtc_end < *rtc_start) &&
	    (!((*rtc_start & 0xF0000000) == 0xF0000000))) {
		goto retry;
	}
	return _rdtsc() - start_tsc;
}

static void test_clock_rates(void)
{
	uint64_t diff;
	unsigned int rtc_start, rtc_end = 0;

	/* Set clock to 32 MHz. */
	clk_sys_set_mode(CLK_SYS_CRYSTAL_OSC, CLK_SYS_DIV_1);
	diff = spin_loop(1000, &rtc_start, &rtc_end);
	clk_sys_set_mode(CLK_SYS_CRYSTAL_OSC, CLK_SYS_DIV_1);
	QM_PRINTF("Fast Clk loop: %d TSC ticks; RTC diff=%d  : %d - %d\n",
		  (unsigned int)(diff & 0xffffffff), rtc_end - rtc_start,
		  rtc_end, rtc_start);

	/* Set clock to 4 MHz. */
	clk_sys_set_mode(CLK_SYS_CRYSTAL_OSC, CLK_SYS_DIV_8);
	diff = spin_loop(1000, &rtc_start, &rtc_end);
	clk_sys_set_mode(CLK_SYS_CRYSTAL_OSC, CLK_SYS_DIV_1);
	/* Output is limited to 32 bits here. */
	QM_PRINTF("Slow Clk loop: %d TSC ticks; RTC diff=%d : %d - %d\n",
		  (unsigned int)(diff & 0xffffffff), rtc_end - rtc_start,
		  rtc_end, rtc_start);
}

static void slow_mode_test(void)
{
#if SLOW_MODE_TEST
	/* Drop into low-power compute mode. */
	QM_PUTS("\nSlow");
	clk_sys_set_mode(CLK_SYS_CRYSTAL_OSC, CLK_SYS_DIV_8);
	ENTER_C1();
	clk_sys_set_mode(CLK_SYS_CRYSTAL_OSC, CLK_SYS_DIV_1);
#endif
}

static void halt_test(void)
{
#if HALT_TEST
	/* Halt the CPU, RTC alarm will wake me up. */
	QM_PUTS("Halt");
	ENTER_C1();
#endif
}

static void sleep_test(void)
{
#if SLEEP_TEST
	/* Go to sleep, (power down some IO). RTC will wake me up. */
	QM_PUTS("Sleep");
	ENTER_C2();
#endif
}

#if DEEP_SLEEP

void ac_example_callback(void *data, uint32_t int_status)
{
	/*
	 * The analog comparators use level triggered interrupts so we will get
	 * a constant stream of interrupts if we do not mask them. Comment the
	 * following line if you want to get more than one interrupt.
	 */
	QM_INTERRUPT_ROUTER->comparator_0_host_int_mask |=
	    BIT(WAKEUP_COMPARATOR_PIN);
}

static void deep_sleep_test(void)
{
	/*
	 * Setup the RTC to get out of sleep mode. deep sleep will require an
	 * analog comparator interrupt to wake up the system.
	 */
	uint32_t pmux_sel_save[2], pmux_in_en_save, pmux_pullup_save;

	qm_ac_config_t ac_cfg;

	/*
	 * Physical step at this stage to raise the V on the comparator
	 * pin.
	 *
	 * Go to deep sleep, a comparator should wake me up.
	 */
	ac_cfg.reference =
	    BIT(WAKEUP_COMPARATOR_PIN); /* Ref internal voltage. */
	ac_cfg.polarity = 0x0; /* Fire if greater than ref (high level). */
	ac_cfg.power = BIT(WAKEUP_COMPARATOR_PIN);  /* Normal operation mode. */
	ac_cfg.int_en = BIT(WAKEUP_COMPARATOR_PIN); /* Enable comparator. */
	ac_cfg.callback = ac_example_callback;
	qm_ac_set_config(&ac_cfg);

	qm_irq_request(QM_IRQ_COMPARATOR_0_INT, qm_comparator_0_isr);

	/*
	 * Comparator pin will fire an interrupt when the input voltage
	 * is greater than the reference voltage (0.95V).
	 */

	/*
	 * In order to minimise power, pmux_sel must be set to 0, input
	 * enable must be cleared for any pins not expecting to be
	 * used to wake the SoC from deep sleep mode, in this example
	 * we are using AC 6.
	 */
	pmux_sel_save[0] = QM_SCSS_PMUX->pmux_sel[0];
	pmux_sel_save[1] = QM_SCSS_PMUX->pmux_sel[1];
	pmux_in_en_save = QM_SCSS_PMUX->pmux_in_en[0];

	pmux_pullup_save = QM_SCSS_PMUX->pmux_pullup[0];

	QM_SCSS_PMUX->pmux_sel[0] = QM_SCSS_PMUX->pmux_sel[1] = 0;
	QM_SCSS_PMUX->pmux_in_en[0] = BIT(WAKEUP_COMPARATOR_PIN);
	QM_SCSS_PMUX->pmux_pullup[0] = 0;

	/* Mux out comparator. */
	qm_pmux_select(QM_PIN_ID_6, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_6, true);

	ENTER_C2LP();

	/* Restore previous pinmuxing settings. */
	QM_SCSS_PMUX->pmux_sel[0] = pmux_sel_save[0];
	QM_SCSS_PMUX->pmux_sel[1] = pmux_sel_save[1];
	QM_SCSS_PMUX->pmux_in_en[0] = pmux_in_en_save;
	QM_SCSS_PMUX->pmux_pullup[0] = pmux_pullup_save;
}
#endif /* DEEP_SLEEP */

int main(void)
{
	qm_rtc_config_t rtc_cfg;
	unsigned int count = 0;
	/* Maximum number of 3-second iterations. */
	const unsigned int loop_max = 5;
	gpio_init();
	gpio_set_out(BOARD_LED_PIN, 0); /* Configure the onboard LED pin. */

	/* Clear Screen. */
	QM_PUTS("Starting: Power Profiler");
	QM_PUTS("Low power mode example.");
	QM_PRINTF("Increment = %d\n", QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1));

	clk_periph_enable(CLK_PERIPH_RTC_REGISTER | CLK_PERIPH_CLK);

	/* Initialise RTC configuration: Run, but don't interrupt. */
	rtc_cfg.init_val = 0;
	rtc_cfg.alarm_en = 0;
	rtc_cfg.alarm_val = QM_RTC_ALARM_SECOND(CLK_RTC_DIV_1);
	rtc_cfg.callback = rtc_example_callback;
	rtc_cfg.prescaler = CLK_RTC_DIV_1;
	qm_rtc_set_config(QM_RTC_0, &rtc_cfg);

	qm_irq_request(QM_IRQ_RTC_0_INT, qm_rtc_0_isr);

	test_clock_rates();

	/* Enable the RTC Interrupt. */
	rtc_cfg.alarm_en = 1;
	qm_rtc_set_config(QM_RTC_0, &rtc_cfg);

	count = 0;
	while (++count < loop_max) {
		QM_PRINTF("\nC:%d R:%d => ", count, rtc_tick);

		slow_mode_test();
		halt_test();
		sleep_test();
#if DEEP_SLEEP
		deep_sleep_test();
#endif
	}
	QM_PUTS("Finished: Power Profiler");
	return 0;
}
