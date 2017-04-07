/*
 * uart.c
 *
 * Created: 29/07/2016 3:44:43 PM
 *  Author: greg
 */ 

#include "uart.h"

#include <avr/io.h>

void uart_init(unsigned int baud)
{
	UBRR0H = (unsigned char) (baud >> 8);
	UBRR0L = (unsigned char) baud;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);
}

void uart_tx_byte(uint8_t data)
{
	while( !(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

uint8_t uart_rx_byte(void)
{
	while( !(UCSR0A & (1 << RXC0)));
	return UDR0;
}