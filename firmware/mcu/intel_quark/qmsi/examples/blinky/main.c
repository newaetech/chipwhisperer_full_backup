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
 * LED_Blink
 *
 * This app will blink an LED on the development platform indefinitely.
 *
 * In order for this application to work correctly on the Intel(R) Quark(TM)
 * Microcontroller D2000 Development Platform, jumper J3 must be set to USR.
 */

#include "clk.h"
#include "qm_gpio.h"
#include "qm_pinmux.h"

/* The following defines the pin and pin mux details for each SoC. */
#if (QUARK_SE)
#define PIN_OUT 25
#define LED_PIN_ID (QM_PIN_ID_59)
#elif(QUARK_D2000)
#define PIN_OUT 24
#define LED_PIN_ID (QM_PIN_ID_24)
#endif
#define PIN_MUX_FN (QM_PMUX_FN_0)
#define DELAY 250000UL /* 0.25 seconds. */

int main(void)
{
	static qm_gpio_port_config_t cfg;

	/* Set the GPIO pin muxing. */
	qm_pmux_select(LED_PIN_ID, PIN_MUX_FN);

	/* Set the GPIO pin direction to out and write the config. */
	cfg.direction = BIT(PIN_OUT);
	qm_gpio_set_config(QM_GPIO_0, &cfg);

	/* Loop indefinitely while blinking the LED. */
	while (1) {
		qm_gpio_set_pin(QM_GPIO_0, PIN_OUT);
		clk_sys_udelay(DELAY);
		qm_gpio_clear_pin(QM_GPIO_0, PIN_OUT);
		clk_sys_udelay(DELAY);
	}
}
