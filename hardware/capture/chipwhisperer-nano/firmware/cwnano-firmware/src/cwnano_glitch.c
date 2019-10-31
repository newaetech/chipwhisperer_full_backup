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
#include "main.h"

void delay_setup(void);
void pin_trigglitch_handler(const uint32_t id, const uint32_t mask);

static volatile unsigned int glitch_enabled = 0;

/* Handler for all PIOA events */
void pin_trigglitch_handler(const uint32_t id, const uint32_t mask)
{
	if ((id == ID_PIOA) && (mask == PIN_TARGET_GPIO4_MSK)){
		
		/* Disable interrupt now */
		pio_disable_interrupt(PIOA, PIN_TARGET_GPIO4_MSK);
		
		if(glitch_width && glitch_enabled){
			cwnano_glitch_insert();
			glitch_enabled = 0;
		}		
	}
}


void cwnano_glitch_enable(void)
{
	/* Enable external pin interrupt too */
	pio_get_interrupt_status(PIOA);
	pio_enable_pin_interrupt(PIN_TARGET_GPIO4_MSK);
	pio_handler_set(PIOA, ID_PIOA, PIN_TARGET_GPIO4_MSK, PIO_IT_RISE_EDGE, pin_trigglitch_handler);
	pio_enable_interrupt(PIOA, PIN_TARGET_GPIO4_MSK);
	glitch_enabled = 1;
}

/* Init the glitch pin (drive low) */
void cwnano_glitch_init(void)
{
	gpio_configure_pin(PIN_GLITCH_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
}

static uint32_t glitch_width_case;
static uint32_t glitch_width_cnt;

static uint32_t glitch_offset_case;
static uint32_t glitch_offset_cnt;

/* Configure the glitch code, must be called before calling insert */
void cwnano_setup_glitch(unsigned int offset, unsigned int length)
{
	glitch_width_cnt = length / 3;
	glitch_width_case = length % 3;
	
	glitch_offset_cnt = offset / 3;
	glitch_offset_case = offset % 3;
}


#pragma GCC push_options
#pragma GCC optimize ("O1")

/* Insert the glitch by driving pin */
void cwnano_glitch_insert(void)
{
	if (glitch_width_case | glitch_width_cnt){
		__disable_irq();
		asm("push {r5-r6}");
		asm volatile(
		 "mov   r5,     #0x40000000\n\t"
		 "orr   r5, r5, #0x000e0000\n\t"
		 "orr   r5, r5, #0x00000e00\n\t"
		 "movs	r6,     #1\n\t"
		 );
		 
		 
		/* The following is very hacky, but works for now (TM). Basically:
		
		  1) You need the 'isb' to clear pipeline before each delay call. If you don't do that you'll see different delays
		     depending on state of pipeline. This makes the case 0/1/2 not work for example since the base delay amount
			 isn't what you expect.
			 
		  2) Due to pipeline etc you can't easily cycle count. The instruction choices have been tested on HW. Should have some
		     way of verifying this automatically still.
			 
		  3) Other attemps such as SRAM functions didn't work well.
		  
		  4) The choice of r5/r6 is just verified in debugger for now. Should actually do entire thing as assembly function
		     at some point.	 
		*/
		 
		switch(glitch_offset_case){
			case 0:
				asm volatile("isb");
				asm volatile("str	r6, [r5, #52]");
				for(unsigned int i = glitch_offset_cnt; i != 0; i--);
				asm volatile("str	r6, [r5, #52]");
				asm volatile("isb");
				break;
			
			case 1:
				asm volatile("isb");
				asm volatile("str	r6, [r5, #52]");
				for(unsigned int i = glitch_offset_cnt; i != 0; i--);
				asm volatile("dsb");
				asm volatile("str	r6, [r5, #52]");
				asm volatile("isb");
				break;

			case 2:
				asm volatile("isb");
				asm volatile("str	r6, [r5, #52]");
				for(unsigned int i = glitch_offset_cnt; i != 0; i--);
				asm volatile("str	r6, [r5, #52]");
				asm volatile("str	r6, [r5, #52]");
				asm volatile("isb");
				break;
		}
		 
		 switch(glitch_width_case){
			 case 0:	
				 asm volatile("isb");
				 asm volatile("str	r6, [r5, #48]");
				 for(unsigned int i = glitch_width_cnt; i != 0; i--);
				 asm volatile("str	r6, [r5, #52]");
				 asm volatile("isb");
				 break;
				 
			case 1:
				asm volatile("isb");
				asm volatile("str	r6, [r5, #48]");
				for(unsigned int i = glitch_width_cnt; i != 0; i--);
				asm volatile("dsb");
				asm volatile("str	r6, [r5, #52]");
				asm volatile("isb");
				break;

			case 2:
				asm volatile("isb");
				asm volatile("str	r6, [r5, #48]");
				for(unsigned int i = glitch_width_cnt; i != 0; i--);
				asm volatile("str	r6, [r5, #48]");
				asm volatile("str	r6, [r5, #52]");
				asm volatile("isb");
				break;
		 }		 
		asm("pop {r5-r6}");
		__enable_irq();
	}
}

#pragma GCC pop_options