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
 * USB Communication Device Class (Abstract Control Model)
 *
 * This app demonstrates the basic usage of the QMSI USB API through the
 * implementation of a serial console over USB through the
 * CDC-ACM Device Class.
 */

#include <stdio.h>
#include <string.h>

#include "clk.h"
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "usb_cdc_acm.h"

#define USB_VBUS_GPIO_PIN (28)
#define USB_VBUS_GPIO_PORT (QM_GPIO_0)
#define ONE_SEC_UDELAY (1000000)

static const char *banner1 = "Send characters to the UART device\r\n";
static const char *banner2 = "Characters read:\r\n";

static volatile bool data_transmitted;
static volatile bool data_received;
static char data_buf[64];

static void interrupt_handler()
{
	if (cdc_acm_irq_tx_ready()) {
		data_transmitted = true;
	}

	if (cdc_acm_irq_rx_ready()) {
		data_received = true;
	}
}

static void write_data(const char *buf, int len)
{
	cdc_acm_irq_tx_enable();

	data_transmitted = false;
	cdc_acm_fifo_fill((uint8_t *)buf, len);
	while (data_transmitted == false)
		;

	cdc_acm_irq_tx_disable();
}

static void read_and_echo_data(int *bytes_read)
{
	while (data_received == false)
		;

	data_received = false;

	/* Read all data and echo it back. */
	while ((*bytes_read =
		    cdc_acm_fifo_read((uint8_t *)data_buf, sizeof(data_buf)))) {
		write_data(data_buf, *bytes_read);
	}
}

static void enable_usb_vbus(void)
{
	qm_gpio_port_config_t cfg = {0};
	cfg.direction |= BIT(USB_VBUS_GPIO_PIN);
	qm_gpio_set_config(USB_VBUS_GPIO_PORT, &cfg);
	qm_gpio_set_pin(USB_VBUS_GPIO_PORT, USB_VBUS_GPIO_PIN);
}

int main(void)
{
	uint32_t baudrate, dtr = 0;
	int bytes_read;

	QM_PUTS("Starting: USB CDC ACM Example");

	qm_irq_request(QM_IRQ_USB_0_INT, qm_usb_0_isr);

	/* Enable the USB VBUS on Quark SE DevBoard. */
	enable_usb_vbus();

	/* Init USB CDC ACM. */
	cdc_acm_init();

	QM_PUTS("CDC ACM Initialized. Waiting for DTR.");
	do {
		cdc_acm_line_ctrl_get(LINE_CTRL_DTR, &dtr);
	} while (!dtr);

	QM_PUTS("DTR set, start test.");

	/* Wait 1 sec for the host to do all settings. */
	clk_sys_udelay(ONE_SEC_UDELAY);

	if (cdc_acm_line_ctrl_get(LINE_CTRL_BAUD_RATE, &baudrate)) {
		QM_PUTS("Failed to get baudrate, ret code.");
	} else {
		QM_PRINTF("Baudrate detected: %d\n", baudrate);
	}

	QM_PUTS("USB CDC ACM set. Switch to the USB Serial Console.");
	cdc_acm_irq_callback_set(interrupt_handler);
	write_data(banner1, strlen(banner1));
	write_data(banner2, strlen(banner2));

	/* Enable RX interrupts. */
	cdc_acm_irq_rx_enable();

	/* Echo the received data. */
	while (1) {
		read_and_echo_data(&bytes_read);
	}

	return 0;
}
