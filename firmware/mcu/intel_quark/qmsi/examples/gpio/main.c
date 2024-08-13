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
 * General Purpose Input/Output (GPIO)
 *
 * This app is intended to demonstrate the basic usage of the QMSI GPIO API.
 *
 * It requires a board to be set up with a jumper cable connecting the pins
 * listed below so the output pin can trigger an interrupt on the input pin.
 * PIN_OUT will be configured as an output pin and PIN_INTR will be configured
 * as an input pin with interrupts enabled.
 */

#include <inttypes.h>
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"

/*
 * On the Intel(R) Quark(TM) D2000 development platform PIN_OUT and PIN_INTR
 * are marked "SSO 10" and "A0" respectively.
 * On the Intel(R) Quark(TM) SE development platform, PIN_OUT and PIN_INTR are
 * located on header J15 pin 42 and 40 respectively.
 */
#define PIN_OUT (0)
#define PIN_INTR (3)

static volatile bool callback_invoked = false;
static volatile uint32_t callback_status;

/* Example GPIO callback function. */
static void gpio_example_callback(void *data, uint32_t status)
{
	callback_invoked = true;
	callback_status = status;
}

int main(void)
{
	qm_gpio_port_config_t cfg;
	qm_gpio_state_t state;

	QM_PUTS("Starting: GPIO");

	/* Pin muxing. */
	qm_pmux_pullup_en(PIN_INTR, true);
	qm_pmux_select(PIN_INTR, QM_PMUX_FN_0);
	qm_pmux_input_en(PIN_INTR, true);
	qm_pmux_select(PIN_OUT, QM_PMUX_FN_0);

	/* Request IRQ and write GPIO port config. */
	cfg.direction = BIT(PIN_OUT);
	cfg.int_en = BIT(PIN_INTR);	/* Interrupt enabled. */
	cfg.int_type = BIT(PIN_INTR);      /* Edge sensitive interrupt. */
	cfg.int_polarity = ~BIT(PIN_INTR); /* Falling edge. */
	cfg.int_debounce = BIT(PIN_INTR);  /* Debounce enabled. */
	cfg.int_bothedge = 0x0;		   /* Both edge disabled. */
	cfg.callback = gpio_example_callback;
	cfg.callback_data = NULL;

	qm_irq_request(QM_IRQ_GPIO_0_INT, qm_gpio_0_isr);

	/* Enable pullup on interrupt pin. */
	qm_pmux_pullup_en(PIN_INTR, true);

	qm_gpio_set_config(QM_GPIO_0, &cfg);

	/* Clear PIN_OUT to trigger PIN_INTR interrupt. */
	qm_gpio_set_pin(QM_GPIO_0, PIN_OUT);
	qm_gpio_clear_pin(QM_GPIO_0, PIN_OUT);

	/* Wait for the GPIO callback to be invoked and print the status. */
	while (!callback_invoked)
		;

	QM_PRINTF("GPIO callback status 0x%u\n", callback_status);

	/* Read the pin and check that it is low. */
	if (qm_gpio_read_pin(QM_GPIO_0, PIN_OUT, &state)) {
		QM_PUTS("Error: read pin failed");
		return 1;
	}
	if (state != QM_GPIO_LOW) {
		QM_PUTS("Error: pin comparison failed");
		return 1;
	}

	QM_PUTS("Finished: GPIO");

	return 0;
}
