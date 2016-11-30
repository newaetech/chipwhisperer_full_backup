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
 * Sensor Subsystem (SS) Interrupt
 *
 * This application contains both a Sensor Subsystem GPIO interrupt driven
 * example and SW driven common interrupt example.
 *
 * This example includes two apps:
 * 1. SS GPIO interrupt driven app - interrupt handling is requested with:
 *
 *    qm_ss_irq_request(QM_SS_IRQ_GPIO_0_INT, ss_gpio_interrupt_isr);
 *
 *    The app requires an Intel(R) Quark(TM) SE development platform to be set
 *    up with a jumper cable connecting the pins listed below so the output pin
 *    can trigger an interrupt on the input pin. PIN_OUT will be configured as
 *    an output pin and PIN_INTR will be configured as an input pin with
 *    interrupts enabled. PIN_OUT and PIN_INTR are located on header
 *    J14 pin 14 and 16 respectively.
 *
 * 2. SW driven common interrupt app - interrupt handling is requested with:
 *
 *    qm_irq_request(QM_IRQ_I2C_1_INT, sw_interrupt_isr);
 *
 */

#include "clk.h"
#include "qm_common.h"
#include "qm_interrupt.h"
#include "qm_sensor_regs.h"
#include "qm_soc_regs.h"
#include "qm_ss_interrupt.h"
#include "qm_uart.h"

#define PIN_OUT (2)
#define PIN_INTR (3)

#define LED_BIT (25) /* LED on Quark(TM) SE development platform. */

#define DELAY (300000) /* 0.3 seconds. */
#define NUM_LOOPS (5)

QM_ISR_DECLARE(sw_interrupt_isr);
QM_ISR_DECLARE(ss_gpio_interrupt_isr);

volatile int counter = 0;

static void ss_gpio_interrupt_example(void)
{
	uint32_t i;

	QM_PUTS("Starting: SS GPIO interrupt");

	/* Enable clock to interrupt controller. */
	__builtin_arc_sr(BIT(31) + BIT(0),
			 QM_SS_GPIO_0_BASE + QM_SS_GPIO_LS_SYNC);

	/* Set SS GPIO pin 2 as OUTPUT. */
	__builtin_arc_sr(BIT(2), QM_SS_GPIO_0_BASE + QM_SS_GPIO_SWPORTA_DDR);

	/* Register the SS GPIO interrupt. */
	qm_ss_irq_request(QM_SS_IRQ_GPIO_0_INT, ss_gpio_interrupt_isr);

	/* Set the bit 3 to rising edge-sensitive. */
	__builtin_arc_sr(BIT(3), QM_SS_GPIO_0_BASE + QM_SS_GPIO_INTTYPE_LEVEL);
	/* Unmask SS GPIO pin 3 interrupt only. */
	__builtin_arc_sr(~BIT(3), QM_SS_GPIO_0_BASE + QM_SS_GPIO_INTMASK);
	/* Clear SS GPIO interrupt requests. */
	__builtin_arc_sr(BIT(3), QM_SS_GPIO_0_BASE + QM_SS_GPIO_PORTA_EOI);
	/* Enable SS GPIO interrupt. */
	__builtin_arc_sr(BIT(3), QM_SS_GPIO_0_BASE + QM_SS_GPIO_INTEN);

	for (i = 0; i < NUM_LOOPS; i++) {
		/*
		 * Toggle the SS GPIO 2, will trigger the interrupt on SS
		 * GPIO 3.
		 */
		clk_sys_udelay(DELAY);
		__builtin_arc_sr(BIT(2),
				 QM_SS_GPIO_0_BASE + QM_SS_GPIO_SWPORTA_DR);
		QM_GPIO[0]->gpio_swporta_dr |= BIT(LED_BIT);
		clk_sys_udelay(DELAY);
		__builtin_arc_sr(0, QM_SS_GPIO_0_BASE + QM_SS_GPIO_SWPORTA_DR);
		QM_GPIO[0]->gpio_swporta_dr &= ~BIT(LED_BIT);
	}
	/* Unmask all SS GPIO interrupts. */
	__builtin_arc_sr(0xff, QM_SS_GPIO_0_BASE + QM_SS_GPIO_INTMASK);
	if (counter == NUM_LOOPS) {
		QM_PUTS("Success");
	} else {
		QM_PUTS("Error: Check are pins 14 and 16 on J14 connector "
			"short connected?");
	}

	QM_PUTS("Finished: SS GPIO interrupt");
}

static void ss_sw_triggered_interrupt_example(void)
{
	uint32_t i;
	QM_PUTS("Starting: SW triggered interrupt");

	qm_irq_request(QM_IRQ_I2C_1_INT, sw_interrupt_isr);

	QM_GPIO[0]->gpio_swporta_ddr |= BIT(LED_BIT);
	for (i = 0; i < NUM_LOOPS; i++) {
		QM_GPIO[0]->gpio_swporta_dr |= BIT(LED_BIT);
		clk_sys_udelay(DELAY);
		QM_GPIO[0]->gpio_swporta_dr &= ~BIT(LED_BIT);
		clk_sys_udelay(DELAY);
		/* SW mainpulated interrupt trigger, triggers I2C_1. */
		__builtin_arc_sr(QM_IRQ_I2C_1_INT_VECTOR, QM_SS_AUX_IRQ_HINT);
	}
	if (counter == NUM_LOOPS) {
		QM_PUTS("Success");
	} else {
		QM_PUTS("Error: SW interrupt didn't fire correctly");
	}
	QM_PUTS("Finished: SW triggered interrupt");
}

QM_ISR_DECLARE(ss_gpio_interrupt_isr)
{
	__builtin_arc_sr(BIT(3), QM_SS_GPIO_0_BASE + QM_SS_GPIO_PORTA_EOI);
	QM_PRINTF("GPIO isr call No %d.\n", counter++);
}

QM_ISR_DECLARE(sw_interrupt_isr)
{
	/* Disable the sw triggered interrupt trigger. */
	__builtin_arc_sr(0, QM_SS_AUX_IRQ_HINT);
	QM_PRINTF("sw triggered isr call No_%d\n", counter);
	counter++;
}

int main(void)
{
	ss_gpio_interrupt_example();
	counter = 0;
	ss_sw_triggered_interrupt_example();
	return 0;
}
