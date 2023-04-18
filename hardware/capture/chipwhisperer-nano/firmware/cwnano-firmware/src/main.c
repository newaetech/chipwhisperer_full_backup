 /*
 * Copyright (c) 2018-2019 NewAE Technology Inc.
 * All rights reserved.
 *
 * If you would like to integrate ChipWhisperer-Nano in your development board,
 * you can receive alternate licensed versions of this project, contact NewAE.
 *
 * ChipWhisperer is a trademark of NewAE Technology Inc., registered in the United
 * States of America, European Union, Peoples Republic of China, and other locations.
 *
 * This project is distributed under the BSD 3-Clause Clear License: 
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or other
 *   materials provided with the distribution.
 *
 * * Neither the name of the author nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include <asf.h>
#include "led.h"
#include "main.h"
#include "conf_usb.h"
#include "circbuffer.h"
#include "naeusb_default.h"
#include "naeusb_usart.h"
#include "naeusb_nano.h"
#include "naeusb_mpsse.h"

//Serial Number - will be read by device ID
char usb_serial_number[33] = "000000000000DEADBEEF";

void cwnano_glitch_insert2(void);

int main (void)
{
	uint32_t serial_number[4];
	
	// Read Device-ID from SAM4S. Do this before enabling interrupts etc.
	flash_read_unique_id(serial_number, sizeof(serial_number));
	
	/* Initialize the system */
	sysclk_init();
	board_init();
	cwnano_glitch_init();
	init_gpio_as_input();

	//Convert serial number to ASCII for USB Serial number
	for(unsigned int i = 0; i < 4; i++){
		sprintf(usb_serial_number+(i*8), "%08x", (unsigned int)serial_number[i]);
	}
	usb_serial_number[32] = 0;

	/* Insert application code here, after the board has been initialized. */
	LED_On(LED0_GPIO);
	udc_start();
	
	/* Parallel capture setup */
	cwnano_adc_init();
	
	cpu_irq_enable();
	
	naeusb_register_handlers();
	naeusart_register_handlers();
	nano_register_handlers();
	mpsse_register_handlers();

	while (true) {
		//sleepmgr_enter_sleep();
		cdc_send_to_pc();
		MPSSE_main_sendrecv_byte();
				
	}
	
}


void ui_init(void)
{
	// Initialize LEDs
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);
}

void ui_powerdown(void)
{
	return;
	LED_Off(LED0_GPIO);
	LED_Off(LED1_GPIO);		
}

void ui_wakeup(void)
{	
	return;
	LED_On(LED0_GPIO);
	
}

void ui_loop_back_state(bool b_started)
{
	if (b_started) {
		LED_On(LED1_GPIO);
	} else {
		LED_Off(LED1_GPIO);
	}
}

void ui_process(uint16_t framenumber)
{
	if ((framenumber % 1024) == 0) {
		LED_On(LED0_GPIO);
	}
	if ((framenumber % 1024) == 512) {
		LED_Off(LED0_GPIO);
		LED_Off(LED1_GPIO);
	}

	if (LED_SETTING == CW_LED_DEBUG_SETTING) {
		if ((framenumber % 1024) == 0) {
			LED_On(LED1_GPIO);
		}
		if ((framenumber % 1024) == 512) {
			LED_Off(LED1_GPIO);
		}
	}

	if (LED_SETTING == CW_LED_ERR_SETTING) {
		if (CURRENT_ERRORS == 0) {
			LED_Off(LED1_GPIO);
		} else {
			LED_On(LED1_GPIO);
		}
	}
	
	if ((framenumber % 512) == 0) {
		// LED_Off(LED1_GPIO);
		//LED_Off(LED2_GPIO);
	}
}

/* The prototype shows it is a naked function - in effect this is just an
assembly function. */
void HardFault_Handler( void ) __attribute__( ( naked ) );

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
void HardFault_Handler(void)
{
	__asm volatile
	(
	" tst lr, #4                                                \n"
	" ite eq                                                    \n"
	" mrseq r0, msp                                             \n"
	" mrsne r0, psp                                             \n"
	" ldr r1, [r0, #24]                                         \n"
	" ldr r2, handler2_address_const                            \n"
	" bx r2                                                     \n"
	" handler2_address_const: .word prvGetRegistersFromStack    \n"
	);
}

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
	/* These are volatile to try and prevent the compiler/linker optimising them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

	r0 = pulFaultStackAddress[ 0 ];
	r1 = pulFaultStackAddress[ 1 ];
	r2 = pulFaultStackAddress[ 2 ];
	r3 = pulFaultStackAddress[ 3 ];

	r12 = pulFaultStackAddress[ 4 ];
	lr = pulFaultStackAddress[ 5 ];
	pc = pulFaultStackAddress[ 6 ];
	psr = pulFaultStackAddress[ 7 ];

	/* When the following line is hit, the variables contain the register values. */
	for( ;; );
}