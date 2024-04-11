/**
 * \file
 *
 * \brief XMEGA legacy IOPORT software compatibility driver interface.
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
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
#include "ioport_compat.h"

#if defined(IOPORT_XMEGA_COMPAT)
void ioport_configure_port_pin(void *port, pin_mask_t pin_mask,
		port_pin_flags_t flags)
{
	uint8_t pin;

	for (pin = 0; pin < 8; pin++) {
		if (pin_mask & (1 << pin)) {
			*((uint8_t *)port + PORT_PIN0CTRL + pin) = flags >> 8;
		}
	}
	/* Select direction and initial pin state */
	if (flags & IOPORT_DIR_OUTPUT) {
		if (flags & IOPORT_INIT_HIGH) {
			*((uint8_t *)port + PORT_OUTSET) = pin_mask;
		} else {
			*((uint8_t *)port + PORT_OUTCLR) = pin_mask;
		}

		*((uint8_t *)port + PORT_DIRSET) = pin_mask;
	} else {
		*((uint8_t *)port + PORT_DIRCLR) = pin_mask;
	}
}

#endif
