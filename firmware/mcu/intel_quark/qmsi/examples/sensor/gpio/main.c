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
 * Sensor Subsystem (SS) General-Purpose Input/Output (GPIO)
 *
 * This app demonstrates the basic usage of the QMSI SS GPIO API.
 *
 * It requires an Intel (R) Quark (TM) SE development board to be set up with a
 * jumper cable connecting the pins listed below so the output pin can trigger
 * an interrupt on the input pin. PIN_OUT will be configured as an output pin
 * and PIN_INTR will be configured as an input pin with interrupts enabled.
 *
 * PIN_OUT and PIN_INTR are located on header J14 pin 14 and 16 respectively.
 *
 * This app is specific to the Intel(R) Quark(TM) SE development platform.
 */

#include "clk.h"
#include "qm_common.h"
#include "qm_pinmux.h"
#include "qm_sensor_regs.h"
#include "qm_soc_regs.h"
#include "qm_ss_gpio.h"
#include "qm_ss_interrupt.h"
#include "qm_ss_isr.h"
#include "qm_uart.h"
#include "ss_clk.h"

#define PIN_OUT (2)
#define PIN_INTR (3)

static volatile bool callback_invoked = false;
static volatile uint32_t callback_status;

/* Example SS_GPIO callback function. */
static void gpio_example_callback(void *data, uint32_t status)
{
	callback_invoked = true;
	callback_status = status;
}

int main(void)
{
	qm_ss_gpio_state_t state;
	qm_ss_gpio_port_config_t conf;

	QM_PUTS("Starting: SS GPIO");

	/* Set GPIO mux mode. */
	qm_pmux_pullup_en(QM_PIN_ID_11, true);
	qm_pmux_select(QM_PIN_ID_11, QM_PMUX_FN_0);
	qm_pmux_select(QM_PIN_ID_10, QM_PMUX_FN_0);

	/* Enable input on PIN_INTR. */
	qm_pmux_input_en(QM_PIN_ID_11, true);

	/* Request IRQ and write SS GPIO port config. */
	conf.direction = BIT(PIN_OUT);      /* Set PIN_OUT to output. */
	conf.int_en = BIT(PIN_INTR);	/* Interrupt enabled. */
	conf.int_type = BIT(PIN_INTR);      /* Edge sensitive interrupt. */
	conf.int_polarity = ~BIT(PIN_INTR); /* Falling edge. */
	conf.int_debounce = BIT(PIN_INTR);  /* Debounce enabled. */
	conf.callback = gpio_example_callback;
	conf.callback_data = NULL;

	/* Enable clock. */
	ss_clk_gpio_enable(QM_SS_GPIO_0);

	qm_ss_irq_request(QM_SS_IRQ_GPIO_0_INT, qm_ss_gpio_0_isr);

	qm_ss_gpio_set_config(QM_SS_GPIO_0, &conf);

	/* Clear PIN_OUT to trigger PIN_INTR interrupt. */
	qm_ss_gpio_set_pin(QM_SS_GPIO_0, PIN_OUT);
	qm_ss_gpio_clear_pin(QM_SS_GPIO_0, PIN_OUT);

	/* Wait for callback to be invoked */
	while (!callback_invoked)
		;

	QM_PRINTF("Callback fired, status: 0x%x\n", callback_status);

	if (qm_ss_gpio_read_pin(QM_SS_GPIO_0, PIN_OUT, &state)) {
		QM_PUTS("Error: read pin failed");
		return 1;
	}
	if (state != QM_SS_GPIO_LOW) {
		QM_PUTS("Error: SS GPIO pin out comparison failed");
		return 1;
	}

	QM_PUTS("Finished: SS GPIO");

	return 0;
}
