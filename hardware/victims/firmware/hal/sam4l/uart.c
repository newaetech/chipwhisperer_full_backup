/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "sam4l_hal.h"

void usart1_reset(void)
{
	/* Disable the Write Protect. */
	USART1->US_WPMR = US_WPMR_WPKEY(0x555341U);

	/* Reset registers that could cause unpredictable behavior after reset. */
	USART1->US_MR = 0;
	USART1->US_RTOR = 0;
	USART1->US_TTGR = 0;

	/* Disable TX and RX. */
	USART1->US_CR = US_CR_RSTRX | US_CR_RXDIS;
	USART1->US_CR = US_CR_RSTTX | US_CR_TXDIS;
	
	/* Reset status bits. */
	USART1->US_CR = US_CR_RSTSTA;
}

void uart1_init(void)
{
    /* Turn on clocks! */
    periclk_usart1_init();

	/* Reset the USART and shut down TX and RX. */
	usart1_reset();
	
	/* 7.3728 MHz clock, 38400 baud. See table 24-7 in SAM4L Datasheet  */
	USART1->US_BRGR = (12 << US_BRGR_CD_Pos);

	USART1->US_MR |= (US_MR_USART_MODE_NORMAL | US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | US_MR_CHMODE_NORMAL);
	USART1->US_CR = US_CR_TXEN;
	USART1->US_CR = US_CR_RXEN;
	
	GPIO->GPIO_PORT[0].GPIO_GPERC = GPIO_PA15A_USART1_RXD;
	GPIO->GPIO_PORT[0].GPIO_GPERC = GPIO_PA16A_USART1_TXD;
}

void uart1_putch(char c)
{
    while(!(USART1->US_CSR & US_CSR_TXRDY));
	
    USART1->US_THR = US_THR_TXCHR(c);
}

char uart1_getch(void)
{
    while(!(USART1->US_CSR & US_CSR_RXRDY));
    
    return USART1->US_RHR & US_RHR_RXCHR_Msk;    
}
