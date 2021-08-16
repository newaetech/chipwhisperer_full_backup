/*
 * rs232_driver.h
 *
 * Created: 16/12/2014 10:02:58 AM
 *  Author: colin
 */ 

#ifndef USART_DRIVER_H_
#define USART_DRIVER_H_

#include "circbuffer.h"

bool ctrl_usart(Usart * usart, bool directionIn);
void usart_driver_putchar(Usart * usart, tcirc_buf * txbuf, uint8_t data);
uint8_t usart_driver_getchar(Usart * usart);

#endif /* USART_DRIVER_H_ */