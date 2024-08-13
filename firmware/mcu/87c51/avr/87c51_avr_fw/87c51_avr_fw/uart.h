/*
 * uart.h
 *
 * Created: 29/07/2016 3:39:39 PM
 *  Author: greg
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_init(unsigned int baud);

void uart_tx_byte(uint8_t data);
uint8_t uart_rx_byte(void);


#endif /* UART_H_ */