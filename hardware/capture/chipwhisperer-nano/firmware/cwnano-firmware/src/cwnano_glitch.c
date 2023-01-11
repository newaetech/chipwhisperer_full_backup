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
	if (pwm_glitch) {
		PMC->PMC_PCER0               = (1<<ID_PWM);                                  // clock on the PWM module
		PIOA->PIO_PDR                = PIO_PDR_P0;                                   // PA0 pin is not PIO anymore
		PIOA->PIO_ABCDSR[1]         &= ~PIO_ABCDSR_P0;                               // PA0 pin should use peripheral A mode
	} else {
		gpio_configure_pin(PIN_GLITCH_IDX, PIO_OUTPUT_0 | PIO_DEFAULT);
	}

}

/* Configure the glitch code, must be called before calling insert */
void cwnano_setup_glitch(unsigned int offset, unsigned int length)
{
	uint32_t glitch_prd;
	uint32_t glitch_duty;

	if (pwm_glitch) {
		if ((length > 0 && length != glitch_width_cnt) || (offset > 0 && offset != glitch_offset_cnt)) {
			glitch_width_cnt             = length;
			glitch_offset_cnt            = offset;
			glitch_duty                  = 16;                                   // we want to have the time to enable channel 0, start our wait loop, etc.
																				//  and switch off output after the glitch
			glitch_prd                   = glitch_duty + length;
			glitch_wait_cnt              = (glitch_prd + (glitch_prd>>1)) / 5;   // 1 full period +50% margin at 5 cycles per loop

			PWM->PWM_CH_NUM[0].PWM_CMR   = PWM_CMR_CPRE_MCK;                     // use MCK, polarity=LOW
			PWM->PWM_CH_NUM[0].PWM_CPRD  = glitch_prd;                           // period cycles
			PWM->PWM_CH_NUM[0].PWM_CDTY  = glitch_duty;                          // duty cycles
			PWM->PWM_OOV                 = ~PWM_OOV_OOVH0;                       // value to output (0) when override is selected
			PWM->PWM_OS                  = PWM_OS_OSH0;                          // override: output 0, not taking care of the pwm output
		}
	} else {
		glitch_offset_cnt = offset;
		glitch_width_cnt = length;
	}

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

	if (pwm_glitch) {
		PWM->PWM_OS                  = 0;                // clear override
		PWM->PWM_ENA                 = PWM_ENA_CHID0;    // enable channel 0 => resets pwm counter

		#if 0
		// debug: output a blip after the glitch to fine tune the wait time with a scope
		PWM->PWM_OOV                 = PWM_OOV_OOVH0;    // override: value 1
		PWM->PWM_OS                  = PWM_OS_OSH0;      // override: output 1, not taking care of the pwm output
		PWM->PWM_OOV                 = ~PWM_OOV_OOVH0;   // override: value 0
		#else
		PWM->PWM_OS                  = PWM_OS_OSH0;      // override: output 0, not taking care of the pwm output
		#endif
	} else {
		// if ((uint32_t)(glitch_payload) & 1) {
			asm volatile(
					"mov   r5,     #0x40000000\n\t"
					"orr   r5, r5, #0x000e0000\n\t"
					"orr   r5, r5, #0x00000e00\n\t"
					"mov	r6, #1\n\t"

					"ldr	r3, %[offset_cnt]\n\t"
					"ldr	r4, %[width]\n\t"

					"subs r4, #1\n\t"
					"itt eq\n\t"
					"ldreq r2, =WID1\n\t"
					"beq WIDEND\n\t"

					"subs r4, #1\n\t"
					"itt eq\n\t"
					"ldreq r2, =WID2\n\t"
					"beq WIDEND\n\t"

					"subs r4, #1\n\t"
					"itt eq\n\t"
					"ldreq r2, =WID3\n\t"
					"beq WIDEND\n\t"

					"subs r4, #1\n\t"
					"itt eq\n\t"
					"ldreq r2, =WID4\n\t"
					"beq WIDEND\n\t"

					"ldr r2, =WIDLOOP\n\t"
				"WIDEND:\n\t"
					"adds r2, #1\n\t" //thumb?

					"isb\n\t"
				"OFFLOOP:\n\t"
					"subs 	r3, #1\n\t"
					"bpl 	OFFLOOP\n\t" //branch on underflow
					"blx 	r2\n\t" //jump to glitch payload
					"b END\n\t"
				"WID1:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID2:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID3:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"dsb\n\t"
					"str	r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID4:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"dsb\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID5:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"dsb\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID6:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"dsb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WID7:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"dsb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"WIDLOOP:\n\t"
					"isb\n\t"
					"nop\n\t"
					"nop\n\t"
					"nop\n\t"
					"str	r6, [r5, #0x30]\n\t"
					"dsb\n\t"
				"WIDLOOP1:\n\t"
					"subs r4, #0x01\n\t"
					"bne WIDLOOP1\n\t"
					"str r6, [r5, #0x34]\n\t"
					"isb\n\t"
					"bx lr\n\t"
				"END:\n\t"

				:
				: [offset_cnt] "m" (glitch_offset_cnt), [width] "m" (glitch_width_cnt)
				: "r2", "r3", "r4", "r5", "r6", "lr", "memory"
			);
		// }
	}

	__enable_irq();

}
