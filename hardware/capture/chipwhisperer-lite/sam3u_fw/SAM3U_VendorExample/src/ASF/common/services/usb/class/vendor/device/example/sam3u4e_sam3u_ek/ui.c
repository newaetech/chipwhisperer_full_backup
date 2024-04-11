/**
 * \file
 *
 * \brief User Interface
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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
#include "ui.h"

void ui_init(void)
{
	// Initialize LEDs
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
}

void ui_powerdown(void)
{
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
	
	// Power off FPGA
	board_power(0);
		
}

void ui_wakeup(void)
{
	LED_On(LED0_GPIO);

	// TODO: Move this to once enumeration is done	
	// Power up FPGA side
	
	//Following is a soft-start hack. Not used right now.
	/*
	for(unsigned int i = 0; i < 40; i++){
		board_power(1);
		board_power(0);
		board_power(0);	
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);	
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);
		board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);board_power(0);				
	}
	*/	
	
	board_power(1);
}

void ui_loop_back_state(bool b_started)
{
	if (b_started) {
		LED_On(LED1_GPIO);
	} else {
		LED_Off(LED1_GPIO);
	}
}
extern volatile uint32_t usb_checked;
void ui_process(uint16_t framenumber)
{
	if ((framenumber % 1000) == 0) {
		LED_On(LED0_GPIO);
	}
	if ((framenumber % 1000) == 500) {
		LED_Off(LED0_GPIO);
	}
	usb_checked = 0x00;
}

/**
 * \defgroup UI User Interface
 *
 * Human interface on SAM3U-EK:
 * - Led 0 (D2) blinks when USB host has checked and enabled vendor interface
 * - Led 1 (D3) is on when loopback is running
 */
