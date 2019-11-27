/**
 * \file
 *
 * \brief GPIO service for SAM.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef SAM_GPIO_H_INCLUDED
#define SAM_GPIO_H_INCLUDED

#include "compiler.h"
#include "pio.h"

#define gpio_pin_is_low(io_id) \
	(pio_get_pin_value(io_id) ? 0 : 1)

#define gpio_pin_is_high(io_id) \
	(pio_get_pin_value(io_id) ? 1 : 0)

#define gpio_set_pin_high(io_id) \
	pio_set_pin_high(io_id)

#define gpio_set_pin_low(io_id) \
	pio_set_pin_low(io_id)

#define gpio_toggle_pin(io_id) \
	pio_toggle_pin(io_id)

#define gpio_configure_pin(io_id,io_flags) \
	pio_configure_pin(io_id,io_flags)

#define gpio_configure_group(port_id,port_mask,io_flags) \
	pio_configure_pin_group(port_id,port_mask,io_flags)

#define gpio_set_pin_group_high(port_id,mask) \
	pio_set_pin_group_high(port_id,mask)

#define gpio_set_pin_group_low(port_id,mask) \
	pio_set_pin_group_low(port_id,mask)

#define gpio_toggle_pin_group(port_id,mask) \
	pio_toggle_pin_group(port_id,mask)

#endif /* SAM_GPIO_H_INCLUDED */
