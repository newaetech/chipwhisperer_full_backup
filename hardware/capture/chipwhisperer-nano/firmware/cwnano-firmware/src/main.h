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

#ifndef _MAIN_H_
#define _MAIN_H_

/*! \brief Notify via user interface that enumeration is ok
 * This is called by vendor interface when USB Host enable it.
 *
 * \retval true if vendor startup is successfully done
 */
bool main_vendor_enable(void);

/*! \brief Notify via user interface that enumeration is disabled
 * This is called by vendor interface when USB Host disable it.
 */
void main_vendor_disable(void);

/*! \brief Manages the leds behaviors
 * Called when a start of frame is received on USB line each 1ms.
 */
void main_sof_action(void);

/*! \brief Enters the application in low power mode
 * Callback called when USB host sets USB line in suspend state
 */
void main_suspend_action(void);

/*! \brief Turn on a led to notify active mode
 * Called when the USB line is resumed from the suspend state
 */
void main_resume_action(void);

/*! \brief Manage the reception of setup request OUT
 *
 * \retval true if request accepted
 */
bool main_setup_out_received(void);

/*! \brief Manage the reception of setup request IN
 *
 * \retval true if request accepted
 */
bool main_setup_in_received(void);


void ui_init(void);
void ui_powerdown(void);
void ui_wakeup(void);
void ui_loop_back_state(bool b_started);
void ui_process(uint16_t framenumber);
void adc_capture_start(void);
unsigned int adc_capture_done(void);

#define ADC_CLKSRC_INT 0
#define ADC_CLKSRC_EXT 1

void init_gpio_as_input(void);
void cwnano_adc_init(void);
void adc_set_clksrc(unsigned int src);
void adc_set_clken(unsigned int en);

void cwnano_glitch_init(void);
void cwnano_setup_glitch(unsigned int offset, unsigned int length);
void cwnano_glitch_insert(void);
void cwnano_glitch_enable(void);
extern uint32_t glitch_offset;
extern uint32_t glitch_width;

/** nS per cycle for glitch */
#define GLITCH_NS_PER_CYCLE 8


#define SIZE_BUFF_RECEPT 100000

/** PIO receive buffer. */
extern uint8_t pio_rx_buffer[SIZE_BUFF_RECEPT];

extern uint32_t capture_req_length;


#endif // _MAIN_H_
