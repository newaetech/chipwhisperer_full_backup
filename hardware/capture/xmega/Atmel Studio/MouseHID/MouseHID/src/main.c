/**
 * \file
 *
 * \brief Main functions for Mouse example
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <asf.h>
#include "conf_usb.h"
#include "ui.h"
#include "adc_dma.h"

static volatile bool main_b_generic_enable = false;


/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	sysclk_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	// Initialize the sleep manager
	sleepmgr_init();

	board_init();
	ui_init();
	ui_powerdown();

	// Start USB stack to authorize VBus monitoring
	udc_start();
	
	adc_init();
	dma_init();
	
	//Start clock for DUT
	timer_setup();
	interrupt_setup();
	
	volatile uint8_t waste;
	
	// The main loop manages only the power mode
	// because the USB management is done by interrupt
	while (true) {
		//sleepmgr_enter_sleep();
		waste++;	
	}
}

void main_suspend_action(void)
{
	ui_powerdown();
}

void main_resume_action(void)
{
	ui_wakeup();
}

void main_sof_action(void)
{
	if (!main_b_generic_enable)
	return;
	ui_process(udd_get_frame_number());
}

void main_remotewakeup_enable(void)
{
	ui_wakeup_enable();
}

void main_remotewakeup_disable(void)
{
	ui_wakeup_disable();
}

bool main_generic_enable(void)
{
	main_b_generic_enable = true;
	return true;
}

void main_generic_disable(void)
{
	main_b_generic_enable = false;
}


void main_hid_set_feature(uint8_t* report)
{
	if (report[0] == 0xAA && report[1] == 0x55
	&& report[2] == 0xAA && report[3] == 0x55) {
		// Disconnect USB Device
		udc_stop();
		ui_powerdown();
	}
}

/**
 * \mainpage ASF USB Device HID Mouse
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device HID Mouse
 * on Atmel MCU with USB module.
 * The application note AVR4903 provides information about this implementation.
 *
 * \section startup Startup
 * The example uses the buttons or sensors available on the board
 * to simulate a standard mouse.
 * After loading firmware, connect the board (EVKxx,Xplain,...) to the USB Host.
 * When connected to a USB host system this application provides a mouse application
 * in the Unix/Mac/Windows operating systems.
 * This example uses the native HID driver for these operating systems.
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and HID modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/hid/
 *   <br>services/usb/class/hid/mouse/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (buttons, leds)
 */
