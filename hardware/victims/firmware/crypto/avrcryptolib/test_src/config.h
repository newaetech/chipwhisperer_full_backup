/* config.h */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2008  Daniel Otte (daniel.otte@rub.de)

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
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <avr/io.h>
//#define F_CPU 20000000
// #define F_CPU 16000000         /* oscillator-frequency in Hz */
// #define F_CPU 14745600

#define DEBUG_METHOD uart

#include "uart_defs.h"

#define UART0_I 1
#ifndef UART0_BAUD_RATE
#define UART0_BAUD_RATE  115200
#endif
#define UART0_PARATY     UART_PARATY_NONE
#define UART0_STOPBITS   UART_STOPBITS_1
#define UART0_DATABITS   UART_DATABITS_8
#define UART0_RXBUFFER_SIZE 255
#define UART0_TXBUFFER_SIZE 120
#define UART0_SWFLOWCTRL     1
#define UART0_THRESH_LOW     0
#define UART0_THRESH_HIGH   32

#define CLI_AUTO_HELP

#endif

