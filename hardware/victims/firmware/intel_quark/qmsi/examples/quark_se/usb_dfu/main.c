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
 * USB Device Firmware Update (DFU) Device Class
 *
 * This app demonstrates the basic usage of the QMSI USB API through the
 * implementation of the DFU Device Class. Only runtime mode from DFU is
 * implemented.
 */

#include "qm_comparator.h"
#include "qm_gpio.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pinmux.h"
#include "qm_usb.h"
#include "usb_dfu.h"
#include <string.h>

/*
 * On the Quark SE Development Board, comparator 7 reads the USB_DETECT signal
 * and GPIO 28 is the USB VBUS Enable drive.
 */
#define USB_DETECT_CMP (7)
#define USB_VBUS_GPIO_PIN (28)
#define USB_VBUS_GPIO_PORT (QM_GPIO_0)

static bool usb_plugged = false;
static qm_ac_config_t ac_cfg = {0};

static void usb_detected_cb(void *data, uint32_t status)
{
	/* Avoid an interrupt 'deluge'. */
	QM_INTERRUPT_ROUTER->comparator_0_host_int_mask |= BIT(USB_DETECT_CMP);

	usb_plugged = !usb_plugged;

	if (usb_plugged) {
		QM_PUTS("USB cable PLUGGED!");
		/* Enable USB driver. */
		if (qm_usb_attach(QM_USB_0)) {
			QM_PUTS("Failed to enable USB");
		}

		/*
		 * Invert comparator polarity in order to guarantee that
		 * we detect when the cable is unplugged.
		 */
		ac_cfg.polarity |= BIT(USB_DETECT_CMP);
		qm_ac_set_config(&ac_cfg);
	} else {
		QM_PUTS("USB cable UNPLUGGED!");
		/* Cable unplugged, disable USB driver. */
		if (qm_usb_detach(QM_USB_0)) {
			QM_PUTS("Failed to disable USB");
		}

		/*
		 * Invert comparator polarity in order to guarantee that
		 * we also detect when the cable is plugged again.
		 */
		ac_cfg.polarity &= ~BIT(USB_DETECT_CMP);
		qm_ac_set_config(&ac_cfg);
	}

	/* Enable interrupts for this comparator once again. */
	QM_INTERRUPT_ROUTER->comparator_0_host_int_mask &= ~BIT(USB_DETECT_CMP);
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
	QM_PUTS("Starting: DFU Test Application");

	/* Setup USB_DETECT comparator pin. */
	ac_cfg.int_en = BIT(USB_DETECT_CMP);     /* AIN7 enable */
	ac_cfg.reference = BIT(USB_DETECT_CMP);  /* Ref internal voltage */
	ac_cfg.polarity &= ~BIT(USB_DETECT_CMP); /* Fire if > ref. */
	ac_cfg.power = BIT(USB_DETECT_CMP);      /* Normal operation mode */
	ac_cfg.callback = usb_detected_cb;

	/* Pinmux setup for Comparator 7. */
	qm_pmux_select(QM_PIN_ID_7, QM_PMUX_FN_1);
	qm_pmux_input_en(QM_PIN_ID_7, true);

	/* Setup ISRs for USB and Comparator. */
	qm_irq_request(QM_IRQ_COMPARATOR_0_INT, qm_comparator_0_isr);
	qm_irq_request(QM_IRQ_USB_0_INT, qm_usb_0_isr);

	/* Apply comparator config. */
	qm_ac_set_config(&ac_cfg);

	/*
	 * On Quark SE Dev Board we must explicitly set GPIO 28 to enable
	 * VCC_USB. Call enable_usb_vbus() which will take care of that.
	 */
	enable_usb_vbus();

	/* Start the USB DFU Device mode. */
	start_dfu();

	while (1) {
		/* Just busy-loop waiting for DFU requests. */
	}

	return 0;
}
