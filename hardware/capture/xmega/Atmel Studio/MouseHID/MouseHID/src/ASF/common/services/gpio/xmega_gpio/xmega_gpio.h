/**
 * \file
 *
 * \brief Common gpio data/structure for all AVR XMEGA implementations.
 *
 * Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _XMEGA_GPIO_H_
#define _XMEGA_GPIO_H_

#include "compiler.h"
#include "ioport.h"

#define gpio_pin_is_low(io_id) \
	ioport_pin_is_low(io_id)

#define gpio_pin_is_high(io_id) \
	ioport_pin_is_high(io_id)

#define gpio_set_pin_high(io_id) \
	ioport_set_value(io_id,1)

#define gpio_set_pin_low(io_id) \
	ioport_set_value(io_id,0)

#define gpio_toggle_pin(io_id) \
	ioport_toggle_pin(io_id)

#define gpio_configure_pin(io_id,io_flags) \
	ioport_configure_pin(io_id,io_flags)

#define gpio_configure_group(port_id,port_mask,io_flags) \
	ioport_configure_group(port_id,port_mask,io_flags)

#define gpio_set_pin_group_high(port_id,mask) \
	ioport_set_group_high(port_id,mask)

#define gpio_set_pin_group_low(port_id,mask) \
	ioport_set_group_low(port_id,mask)

#define gpio_toggle_pin_group(port_id,mask) \
	ioport_tgl_group(port_id,mask)

#endif  // _XMEGA_GPIO_H_
