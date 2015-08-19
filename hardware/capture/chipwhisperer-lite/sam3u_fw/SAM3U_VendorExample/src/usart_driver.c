/*
 * Copyright (c) 2014-2015 NewAE Technology Inc.
 * All rights reserved.
 *
 * Based on code from FIP, the Flexible IP Stack.
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
#include "circbuffer.h"
#include "usart_driver.h"
#include "usart.h"


#define USART_WVREQ_INIT    0x0010
#define USART_WVREQ_ENABLE  0x0011
#define USART_WVREQ_DISABLE 0x0012
#define USART_WVREQ_NUMWAIT 0x0014

#define word2buf(buf, word)   do{buf[0] = LSB0W(word); buf[1] = LSB1W(word); buf[2] = LSB2W(word); buf[3] = LSB3W(word);}while (0)
#define buf2word(word, buf)   do{word = *((uint32_t *)buf);}while (0)
	
//TODO: Improve this by using the following array instead of hard-coded hacks
/*	
typedef struct {
	Usart * usart;
	sam_usart_opt_t usartopts;
	tcirc_buf rxbuf;
	tcirc_buf txbuf;
	int usart_id;
} usart_driver_t;
	
#define NUM_USARTS 3

usart_driver_t usarts[NUM_USARTS];
*/
	
tcirc_buf rx0buf, tx0buf;
tcirc_buf rx1buf, tx1buf;
tcirc_buf rx2buf, tx2buf;

static inline void usart0_enableIO(void)
{
	gpio_configure_pin(PIN_USART0_RXD, PIN_USART0_RXD_FLAGS);
	gpio_configure_pin(PIN_USART0_TXD, PIN_USART1_TXD_FLAGS);
}

#define usart1_enableIO() do{;}while(0)
#define usart2_enableIO() do{;}while(0)

bool ctrl_usart(Usart * usart, bool directionIn)
{
	uint32_t baud;
	static uint8_t ctrlbuffer[16];
	sam_usart_opt_t usartopts;
	uint32_t cnt;
			
	
	/*
	
	Info block same as CDC:
	
	uint32_t 	dwDTERate //Baud rate
	uint8_t 	bCharFormat //Stop bits: 0=1, 1=1.5, 2=2
	uint8_t 	bParityType //Partity Type, 0=None, 1=Odd, 2=Even, 3=Mark, 4=Space
	uint8_t 	bDataBits //Data bits 5,6,7,8
	
	*/
	
	switch(udd_g_ctrlreq.req.wValue & 0xFF)
	{
		case USART_WVREQ_INIT:
			if (directionIn){
				if (udd_g_ctrlreq.req.wLength == 4){
					/*
					udd_g_ctrlreq.payload_size = 4;
					udd_g_ctrlreq.payload = ctrlbuffer;
					word2buf(ctrlbuffer, baud);
					*/
					return true;
				}
			} else {	
					if (udd_g_ctrlreq.req.wLength == 7) 
						{
					
						buf2word(baud, udd_g_ctrlreq.payload);	
					
						usartopts.baudrate = baud;
						
						/* Stop Bits */
						switch(udd_g_ctrlreq.payload[4])
							{
							case 0:
								usartopts.stop_bits = US_MR_NBSTOP_1_BIT;
								break;
							case 1:
								usartopts.stop_bits = US_MR_NBSTOP_1_5_BIT;
								break;
							case 2:
								usartopts.stop_bits = US_MR_NBSTOP_2_BIT;
								break;
							default:
								usartopts.stop_bits = US_MR_NBSTOP_1_BIT;
							}
					
						/* Parity */
						switch(udd_g_ctrlreq.payload[5])
							{
							case 0:
								usartopts.parity_type = US_MR_PAR_NO;
								break;
							case 1:
								usartopts.parity_type = US_MR_PAR_ODD;
								break;
							case 2:
								usartopts.parity_type = US_MR_PAR_EVEN;
								break;
							case 3:
								usartopts.parity_type = US_MR_PAR_MARK;
								break;
							case 4:
								usartopts.parity_type = US_MR_PAR_SPACE;
								break;							
							default:
								usartopts.parity_type = US_MR_PAR_NO;
							}
					
						/* Data Bits */
						switch(udd_g_ctrlreq.payload[6])
							{
							case 5:
								usartopts.char_length = US_MR_CHRL_5_BIT;
								break;
							case 6:
								usartopts.char_length = US_MR_CHRL_6_BIT;
								break;
							case 7:
								usartopts.char_length = US_MR_CHRL_7_BIT;
								break;					
							case 8:							
							default:
								usartopts.char_length = US_MR_CHRL_8_BIT;
							}
							
						usartopts.channel_mode = US_MR_CHMODE_NORMAL;

						if (usart == USART0)
						{
							sysclk_enable_peripheral_clock(ID_USART0);
							init_circ_buf(&tx0buf);
							init_circ_buf(&rx0buf);
							printf("Enabling USART0\n");
						} else if (usart == USART1)
						{
							sysclk_enable_peripheral_clock(ID_USART1);
							init_circ_buf(&tx1buf);
							init_circ_buf(&rx1buf);
						} else if (usart == USART2)
						{
							sysclk_enable_peripheral_clock(ID_USART2);
							init_circ_buf(&tx2buf);
							init_circ_buf(&rx2buf);
						}

						usart_init_rs232(usart, &usartopts,  sysclk_get_cpu_hz());						 
					} else {
						printf("ERR: Invalid USART Configuration packet?\n");
					}
			}
			break;
			
		case USART_WVREQ_ENABLE:
			if (directionIn == false){
				usart_enable_rx(usart);
				usart_enable_tx(usart);
				
				usart_enable_interrupt(usart, UART_IER_RXRDY);
				
				/* Enable IO Pins */
				if (usart == USART0){
					usart0_enableIO();
					irq_register_handler(USART0_IRQn, 5);
				} else if (usart == USART1) {
					usart1_enableIO();
					irq_register_handler(USART1_IRQn, 5);
				} else if (usart == USART2) {
					usart2_enableIO();
					irq_register_handler(USART2_IRQn, 5);
				}
			}
			break;
						
		case USART_WVREQ_DISABLE:
			if (directionIn == false){
				usart_disable_rx(usart);
				usart_disable_rx(usart);
				usart_disable_interrupt(usart, UART_IER_RXRDY|UART_IER_TXRDY);
			}
		
			break;
			
		case USART_WVREQ_NUMWAIT:		
			if (directionIn){
				if (udd_g_ctrlreq.req.wLength == 4){
		
					udd_g_ctrlreq.payload = ctrlbuffer;
					udd_g_ctrlreq.payload_size = 4;

					if (usart == USART0){
						cnt = circ_buf_count(&rx0buf);
					} else if (usart == USART1){
						cnt = circ_buf_count(&rx1buf);
					} else if (usart == USART2){
						cnt = circ_buf_count(&rx2buf);
					}			
					
					word2buf(ctrlbuffer, cnt);
			
					return true;
				}
			}
			break;		
	}
	
	return false;
}

void usart_driver_putchar(Usart * usart, tcirc_buf * txbuf, uint8_t data)
{
	if (txbuf == NULL){
		if (usart == USART0) txbuf = &tx0buf;
		else if (usart == USART1) txbuf = &tx1buf;
		else if (usart == USART2) txbuf = &tx2buf;
		else return;
	}
	
	// Add byte to transmit buffer
	add_to_circ_buf(txbuf, data, false);

	// Send the first byte if nothing is yet being sent
	// This is determined by seeing if the TX complete interrupt is
	// enabled.
	if ((usart_get_interrupt_mask(usart) & US_CSR_TXRDY) == 0) {
		usart_putchar(usart, get_from_circ_buf(txbuf));
		usart_enable_interrupt(usart, US_CSR_TXRDY);
	}
}

uint8_t usart_driver_getchar(Usart * usart)
{
	tcirc_buf * rxbuf = NULL;	 
	if (rxbuf == NULL){
			if (usart == USART0) rxbuf = &rx0buf;
			else if (usart == USART1) rxbuf = &rx1buf;
			else if (usart == USART2) rxbuf = &rx2buf;
			else return 0xFF;
	}
	return get_from_circ_buf(rxbuf);
}

void generic_isr(Usart * usart, tcirc_buf * rxbuf, tcirc_buf * txbuf);
void generic_isr(Usart * usart, tcirc_buf * rxbuf, tcirc_buf * txbuf)
{
	uint32_t status;
	status = usart_get_status(usart);
	if (status & US_CSR_RXRDY){
		uint32_t temp;
		temp = usart->US_RHR & US_RHR_RXCHR_Msk;
		add_to_circ_buf(rxbuf, temp, false);
	}
	
	if (status & US_CSR_TXRDY){
		if (circ_buf_has_char(txbuf)){
			//Still data to send
			usart_putchar(usart, get_from_circ_buf(txbuf));			
		} else {
			//No more data, stop this madness
			usart_disable_interrupt(usart, UART_IER_TXRDY);
		}
	}
}

ISR(USART0_Handler)
{
	generic_isr(USART0, &rx0buf, &tx0buf);
}

ISR(USART1_Handler)
{
	generic_isr(USART1, &rx1buf, &tx1buf);
}

ISR(USART2_Handler)
{
	generic_isr(USART2, &rx2buf, &tx2buf);
}



