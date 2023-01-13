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
#include <string.h>

void delay_setup(void);
void pin_trigglitch_handler(const uint32_t id, const uint32_t mask);

#define GLITCH_WIDTH_MAX 128

static volatile unsigned int glitch_enabled = 0;

static uint32_t glitch_width_cnt = 0;
static uint32_t glitch_offset_cnt = 0;
static uint32_t glitch_wait_cnt = 0;

uint8_t pwm_glitch = 0;
void *width_addr = 0;
void *offset_addr = 0;

volatile uint32_t *GLITCHIOREG = (uint32_t *)0x400e0e00;
			// 		"mov   r5,     #0x40000000\n\t"
			// 		"orr   r5, r5, #0x000e0000\n\t"
			// 		"orr   r5, r5, #0x00000e00\n\t"

// static uint8_t *glitch_payload = glitch_memory;

void cwnano_glitch_enable(void)
{
	/* Enable external pin interrupt too */
	pio_get_interrupt_status(PIOA);
	pio_enable_pin_interrupt(PIN_TARGET_GPIO4_MSK);
	pio_handler_set(PIOA, ID_PIOA, PIN_TARGET_GPIO4_MSK, PIO_IT_RISE_EDGE, pin_trigglitch_handler);
	pio_enable_interrupt(PIOA, PIN_TARGET_GPIO4_MSK);
	glitch_enabled = 1;

	if (pwm_glitch) {
		PWM->PWM_DIS = PWM_DIS_CHID0;                                                // channel 0 must be disabled before cwnano_glitch_insert is invoked
	}
}

#define PWM_GPIO0 IOPORT_CREATE_PIN(PIOA, 0)

/* Init the glitch pin (drive low) */
void cwnano_glitch_init(void)
{
	gpio_configure_pin(PIN_GLITCH_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
}

/* Configure the glitch code, must be called before calling insert */
void cwnano_setup_glitch(unsigned int offset, unsigned int length)
{
	uint32_t glitch_prd;
	uint32_t glitch_duty;

		if (length == 0) {
			glitch_width_cnt = 0;
			return;
		}
		uint32_t offset_case = offset % 3;
		// uint32_t width_case = length % 3;
		glitch_offset_cnt = offset / 3;
		glitch_width_cnt = length;



		asm volatile(
			"cmp %[offset_case], #0\n\t"
			"itt eq\n\t"
			"ldreq r0, =OFF_CASE_0\n\t"
			"beq OFF_ADDR_END\n\t"

			"cmp %[offset_case], #1\n\t"
			"ite eq\n\t"
			"ldreq r0, =OFF_CASE_1\n\t"
			"ldrne r0, =OFF_CASE_2\n\t"

			"OFF_ADDR_END:\n\t"
			"adds r0, #1\n\t" //thumb
			"str r0, %[offset_addr]\n\t"

			// "cmp %[width_case], #0\n\t"
			// "itt eq\n\t"
			// "ldreq r0, =WIDTH_CASE_0\n\t"
			// "beq WIDTH_ADDR_END\n\t"

			// "cmp %[width_case], #1\n\t"
			// "ite eq\n\t"
			// "ldreq r0, =WIDTH_CASE_1\n\t"
			// "ldrne r0, =WIDTH_CASE_2\n\t"

			// "WIDTH_ADDR_END:\n\t"
			// "adds r0, #1\n\t" //thumb
			// "str r0, %[width_addr]\n\t"

				: [offset_addr] "=m" (offset_addr)
				: [offset_case] "l" (offset_case)
				: "r0"
		);

		switch (glitch_width_cnt) {
			case 1:
				asm volatile("ldr r0, =WID1\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");
				break;
			case 2:
				asm volatile("ldr r0, =WID2\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");
				break;
			case 3:
				asm volatile("ldr r0, =WID3\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");
				break;
			case 4:
				asm volatile("ldr r0, =WID4\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");
				break;
			case 5:
				asm volatile("ldr r0, =WID5\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");
				break;
			case 6:
				asm volatile("ldr r0, =WID6\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");
				break;
			case 7:
				asm volatile("ldr r0, =WID7\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");
				break;
			default:
				glitch_width_cnt -= 7;
				asm volatile("ldr r0, =WIDLOOP\n\tstr r0, %[width_addr]\n\t" : [width_addr] "=m" (width_addr) : : "r0");

		}

		width_addr = (void *)((uint32_t)width_addr + 1);

}

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

void cwnano_glitch_insert(void)
{
	__disable_irq();
			asm volatile(
				"movs r0, #1\n\t"
				"ldr r1, %[offset_cnt]\n\t"
				"ldr r2, %[width_cnt]\n\t"
				"isb\n\t"
				"cmp r2, #0\n\t"
				"beq GLITCH_END\n\t"
				"bx %[offset_addr]\n\t"
				"OFF_CASE_0:\n\t"
					"subs r1, #1\n\t"
					"bpl OFF_CASE_0\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"blx %[width_addr]\n\t"
					"b GLITCH_END\n\t"
				"OFF_CASE_1:\n\t"
					"subs r1, #1\n\t"
					"bpl OFF_CASE_1\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"isb\n\t"
					"blx %[width_addr]\n\t"
					"b GLITCH_END\n\t"
				"OFF_CASE_2:\n\t"
					"subs r1, #1\n\t"
					"bpl OFF_CASE_2\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"isb\n\t"
					"blx %[width_addr]\n\t"
					"b GLITCH_END\n\t"
				"WID1:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID2:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID3:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str.w r0, [%[ioreg], #48]\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID4:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID5:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID6:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID7:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WIDLOOP:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
				 	"str r0, [%[ioreg], #48]\n\t"
				"WIDLOOP1:\n\t"
					"subs r2, #0x01\n\t"
					"bne WIDLOOP1\n\t"
				 	"str r0, [%[ioreg], #52]\n\t"
					"isb\n\t"
					"bx lr\n\t"
					"GLITCH_END:\n\t"
					"nop\n\t"
				:
				: [offset_cnt] "m" (glitch_offset_cnt), [width_cnt] "m" (glitch_width_cnt), 
					[offset_addr] "l" (offset_addr), [width_addr] "l" (width_addr), [ioreg] "l" (GLITCHIOREG)
				: "r0", "r1", "r2", "lr", "memory"
			);
	__enable_irq();

}
