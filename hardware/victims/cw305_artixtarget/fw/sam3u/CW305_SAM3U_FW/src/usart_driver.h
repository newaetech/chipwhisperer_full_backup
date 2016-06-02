/*
 Copyright (c) 2015-2016 NewAE Technology Inc. All rights reserved.

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

#ifndef USART_DRIVER_H_
#define USART_DRIVER_H_

#include "circbuffer.h"

bool ctrl_usart(Usart * usart, bool directionIn);
void usart_driver_putchar(Usart * usart, tcirc_buf * txbuf, uint8_t data);
uint8_t usart_driver_getchar(Usart * usart);

#endif /* USART_DRIVER_H_ */