/*
 *  LPCUSB, an USB device driver for LPC microcontrollers
 *  Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)
 *  Copyright (c) 2016 Intel Corporation.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __USB_DEVICE_H__
#define __USB_DEVICE_H__

#include "qm_usb.h"

/**
 * USB device protocol implementation.
 *
 * @defgroup groupUSB USB
 * @{
 */

/**
 * USB Setup Packet.
 */
typedef struct {
	uint8_t request_type; /**< Request direction, type and recipient. */
	uint8_t request;      /**< Specific request being made. */
	uint16_t value;       /**< Request specific parameter. */
	uint16_t index;       /**< Request specific parameter. */
	uint16_t length;      /**< Data length transferred in data phase. */
} usb_setup_packet_t;

/**
 * Request handler function signature.
 *
 * This will handle Class specific requests corresponding to an
 * interface number specified in the device descriptor table.
 *
 * @param[in] packet   Setup Packet.
 * @param[in,out] len  Pointer to data length.
 * @param[in,out] data Pointer to a data buffer.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
typedef int (*usb_request_handler_t)(usb_setup_packet_t *packet, uint32_t *len,
				     uint8_t **data);
/**
 * USB Interface Configuration.
 */
typedef struct {
	/**< Class Request Handler for Control Endpoint (EP 0). */
	usb_request_handler_t class_handler;

	/**< Custom (subClass) Request Handler for Control Endpoint (EP 0). */
	usb_request_handler_t custom_handler;

	/**
	 * Device Class Data buffer allocated by the application.
	 *
	 * Stores command data specific to the Device Class. Must be large
	 * enough to store the largest payload associated with the largest
	 * class command set. It may be used for both USB IN or OUT transfers.
	 */
	uint8_t *data;
} usb_interface_config_t;

/**
 * USB Device Configuration.
 *
 * This is set using the "usb_set_config" function. Once this function is
 * called, changes to this structure will result in undefined behaviour.
 */
typedef struct {
	/**< USB device description. */
	const uint8_t *const device_description;

	/**< USB connection status callback. */
	qm_usb_status_callback_t status_callback;

	/**< USB interface configuration. */
	usb_interface_config_t interface;

	/**< Number of individual non-control endpoints. */
	uint8_t num_endpoints;

	/**< Endpoint array of length equal to number of non-control EPs. */
	qm_usb_ep_config_t *endpoints;
} usb_device_config_t;

/**
 * Enable USB device controller with the given descriptors configuration.
 *
 * This function will configure and enable the USB device controller, after
 * starting the USB protocol state machine and configuring the control
 * enpoints. Upon success, the USB module is no longer clock gated in hardware,
 * and the USB PLL will be activated.
 *
 * @param[in] cfg New device configuration for the USB stack.
 *		  This must not be NULL.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int usb_enable(const usb_device_config_t *cfg);

/**
 * Disable the USB device controller and clear internal configuration pointer.
 *
 * Upon success, the specified USB interface is clock gated and the USB PLL
 * is disabled. The USB controller won't work until usb_enable() has been
 * called again with a new config.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int usb_disable(void);
#endif /* __USB_DEVICE_H__ */
