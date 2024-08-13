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
 * Pulse Width Modulation (PWM)
 *
 * This example application demonstrates the PWM functionality on the
 * Intel(R) Quark(TM) development platforms.
 */

#include "clk.h"
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_pwm.h"

#define DELAY (500000) /* 0.5 seconds. */
#define NUM_CALLBACK (50)

#if (QUARK_D2000)
#define QM_PWM_CH_1_PIN (24)
#define QM_PWM_CH_1_FN_GPIO (0)
#define QM_PWM_CH_1_FN_PWM (2)
#elif(QUARK_SE)
#define QM_PWM_CH_1_PIN (64)
#define QM_PWM_CH_1_FN_GPIO (0)
#define QM_PWM_CH_1_FN_PWM (1)
#endif /* QUARK_D2000 */

// static uint32_t interrupt_from;
static uint32_t cb_count;
static volatile bool complete;

static void pwm_example_callback(void *data, uint32_t pwm_int)
{
	if (cb_count < NUM_CALLBACK) {
		++cb_count;
	} else {
		complete = true;
	}
}

/*
 * Duty cycles of 0% and 100% are not handled by the PWM block. Instead, these
 * are essentially a constant low, or constant high value on the pin.
 */
static void set_pwm_as_gpio(const qm_pwm_t pwm, const qm_pwm_id_t id, bool high)
{
#if (QUARK_D2000)
	/* Set pin as output. */
	qm_gpio_port_config_t cfg;

	cfg.direction = QM_GPIO[QM_GPIO_0]->gpio_swporta_ddr;
	cfg.int_en = QM_GPIO[QM_GPIO_0]->gpio_inten;
	cfg.int_type = QM_GPIO[QM_GPIO_0]->gpio_inttype_level;
	cfg.int_polarity = QM_GPIO[QM_GPIO_0]->gpio_int_polarity;
	cfg.int_debounce = QM_GPIO[QM_GPIO_0]->gpio_debounce;
	cfg.int_bothedge = QM_GPIO[QM_GPIO_0]->gpio_int_bothedge;
	cfg.callback = NULL;
	cfg.callback_data = NULL;
#elif(QUARK_SE)
/*
 * Warning: Quark SE pins are on sensor subsystem, need to set sensor subsystem
 * port. This can not be done from the Lakemont core.
 */
#endif
	/* Perform pin muxing. */
	qm_pmux_select(QM_PWM_CH_1_PIN, QM_PWM_CH_1_FN_GPIO);

#if (QUARK_D2000)
	cfg.direction |= BIT(QM_PWM_CH_1_PIN);
	qm_gpio_set_config(QM_GPIO_0, &cfg);
	if (true == high) {
		qm_gpio_set_pin(QM_GPIO_0, QM_PWM_CH_1_PIN);
	} else {
		qm_gpio_clear_pin(QM_GPIO_0, QM_PWM_CH_1_PIN);
	}
#elif(QUARK_SE)
/*
 * Warning: Quark SE pins are on sensor subsystem, need to set sensor subsystem
 * port. This can not be done from the Lakemont core.
 */
#endif
}

int main(void)
{
	qm_pwm_config_t cfg;
	uint32_t lo_cnt, hi_cnt;

	QM_PUTS("Starting: PWM example");

	/* Initialise PWM configuration. */
	cfg.lo_count = 0x100000; /* Set low count. */
	cfg.hi_count = 0x100000; /* Set high count. */
	cfg.mode = QM_PWM_MODE_PWM;
	cfg.mask_interrupt = false;
	cfg.callback = pwm_example_callback;
	cfg.callback_data = NULL;

	/* Enable clocking for the PWM block. */
	clk_periph_enable(CLK_PERIPH_PWM_REGISTER | CLK_PERIPH_CLK);

	/* Set the configuration of the PWM. */
	qm_pwm_set_config(QM_PWM_0, QM_PWM_ID_1, &cfg);
	/* Register the ISR with the SoC. */
	qm_irq_request(QM_IRQ_PWM_0_INT, qm_pwm_0_isr);

	qm_pmux_select(QM_PWM_CH_1_PIN, QM_PWM_CH_1_FN_PWM);
	/* Start PWM0 channel 1. */
	qm_pwm_start(QM_PWM_0, QM_PWM_ID_1);

	while (!complete)
		;

	QM_PUTS("PWM channel 1 fired.");

	/* Set new values into the PWM. */
	lo_cnt = hi_cnt = 0x40000;
	qm_pwm_set(QM_PWM_0, QM_PWM_ID_1, lo_cnt, hi_cnt);

	cb_count = 0;
	while (!complete)
		;

	QM_PUTS("PWM channel 1 fired.");

	/* Set PWM with 0% duty cycle. */
	set_pwm_as_gpio(QM_PWM_0, QM_PWM_ID_1, false);
	clk_sys_udelay(DELAY);
	/* Set PWM with 100% duty cycle. */
	set_pwm_as_gpio(QM_PWM_0, QM_PWM_ID_1, true);
	clk_sys_udelay(DELAY);

	/* Stop the PWM from running. */
	qm_pwm_stop(QM_PWM_0, QM_PWM_ID_1);
	/* Disable clocking for the PWM block. */
	clk_periph_disable(CLK_PERIPH_PWM_REGISTER);

	QM_PUTS("Finished: PWM example");

	return 0;
}
