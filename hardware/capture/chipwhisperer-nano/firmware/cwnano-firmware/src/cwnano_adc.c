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

static void capture_handler(Pio *p_pio);
void pin_edge_handler(const uint32_t id, const uint32_t mask);

/** PIOA interrupt priority. */
#define PIO_IRQ_PRI                    (4)

/** PIO receive buffer. */
uint8_t pio_rx_buffer[SIZE_BUFF_RECEPT];

/** Test if a callback is received. */
volatile uint8_t g_uc_cbk_received = 0;

/** PDC data packet. */
pdc_packet_t packet0, packet1;

/** Pointer to PDC register base. */
Pdc *p_pdc;

void cwnano_adc_init(void)
{
	pio_capture_handler_set(capture_handler);
	pio_capture_set_mode(PIOA, PIO_PCMR_DSIZE_BYTE);
	pio_capture_enable(PIOA);
}

/**
 *  \brief Handler for capture function interrupt.
 *
 */
static void capture_handler(Pio *p_pio)
{
	uint32_t dummy_data;
	
	if (p_pio->PIO_PCISR & PIO_PCIDR_RXBUFF){

		pio_capture_disable_interrupt(p_pio, PIO_PCIDR_RXBUFF);
		/* Disable PDC transfer. */
		pdc_disable_transfer(p_pdc, PERIPH_PTCR_RXTEN);
		/* Clear any unwanted data */
		pio_capture_read(PIOA, &dummy_data);
	
		/* Disable capture */
		gpio_set_pin_high(PIO_CAPTURELATCH_nSET);
		gpio_set_pin_low(PIO_CAPTURELATCH_nRESET);

		g_uc_cbk_received = 1;
		
		LED_Off(LED2_GPIO);
		LED_Off(LED3_GPIO);
	}
}

/* Handler for all PIOA events */
void pin_edge_handler(const uint32_t id, const uint32_t mask)
{
	if ((id == ID_PIOA) && (mask == PIO_CAPTURE_EN2_MASK)){
	
		/* Force output high */
		gpio_set_pin_low(PIO_CAPTURELATCH_nSET);
		
		/* Disable interrupt now */
		pio_disable_interrupt(PIOA, PIO_CAPTURE_EN2_MASK);
		
		LED_On(LED3_GPIO);
	}
}

uint32_t capture_req_length = SIZE_BUFF_RECEPT;

void adc_capture_start(void)
{
	
	static pdc_packet_t * packet0p;
	static pdc_packet_t * packet1p;
	
	LED_On(LED2_GPIO);
	
	NVIC_DisableIRQ(PIOA_IRQn);
	NVIC_ClearPendingIRQ(PIOA_IRQn);
	NVIC_SetPriority(PIOA_IRQn, PIO_IRQ_PRI);
	NVIC_EnableIRQ(PIOA_IRQn);

	g_uc_cbk_received = 0;
	
	if (capture_req_length > SIZE_BUFF_RECEPT){
		capture_req_length = SIZE_BUFF_RECEPT;
	}
	
	packet0p = &packet0;
	packet1p = NULL;
	if (capture_req_length < (uint32_t)0xFFFF){
		/* Set up PDC receive buffer */
		packet0.ul_addr = (uint32_t) pio_rx_buffer;
		packet0.ul_size = capture_req_length;
	} else {
		packet0.ul_addr = (uint32_t) pio_rx_buffer;
		packet0.ul_size = (uint16_t)0xFFFF;
		packet1.ul_addr = (uint32_t) (pio_rx_buffer + (uint32_t)0xFFFF);
		packet1.ul_size = capture_req_length - (uint32_t)0xFFFF;
		packet1p = &packet1;
	}
		
		
	p_pdc = pio_capture_get_pdc_base(PIOA);
	pdc_rx_init(p_pdc, packet0p, packet1p);

	/* Enable PDC transfer. */
	pdc_enable_transfer(p_pdc, PERIPH_PTCR_RXTEN);
	
	/* Enable external pin interrupt too */	
	pio_get_interrupt_status(PIOA);
	pio_enable_pin_interrupt(PIO_CAPTURE_EN2_IDX);
	pio_handler_set(PIOA, ID_PIOA, PIO_CAPTURE_EN2_MASK, PIO_IT_RISE_EDGE, pin_edge_handler);
	pio_enable_interrupt(PIOA, PIO_CAPTURE_EN2_MASK);
	
	/* Configure the PIO capture interrupt mask. */
	pio_capture_enable_interrupt(PIOA, (PIO_PCIER_RXBUFF));
	
	gpio_set_pin_high(PIO_CAPTURELATCH_nSET);
	gpio_set_pin_high(PIO_CAPTURELATCH_nRESET);
	
	cwnano_glitch_enable();
}

unsigned int adc_capture_done(void)
{	
	return g_uc_cbk_received;	
}

void adc_set_clksrc(unsigned int src)
{
	if (src){
		gpio_set_pin_high(ADCCLK_SRC_IDX);
	} else {
		gpio_set_pin_low(ADCCLK_SRC_IDX);
	}
}

void adc_set_clken(unsigned int en)
{
	if (en){
		gpio_set_pin_high(ADCCLK_EN_IDX);
	} else {
		gpio_set_pin_low(ADCCLK_EN_IDX);
	}
}