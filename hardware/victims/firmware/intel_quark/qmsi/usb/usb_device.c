/*
 * LPCUSB, an USB device driver for LPC microcontrollers
 * Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)
 * Copyright (c) 2016 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * USB device stack.
 *
 * This module handles control transfers, standard request handler and
 * USB Interface for device class applications.
 *
 * Control transfers handler is normally installed on the endpoint 0 callback.
 *
 * Control transfers can be of the following type:
 * 0 Standard;
 * 1 Class;
 * 2 Vendor;
 * 3 Reserved.
 *
 * When an OUT request arrives, data is collected in the data store provided
 * when the request handler was registered. When the transfer is done, the
 * callback is called.
 * When an IN request arrives, the callback is called immediately to either
 * put the control transfer data in the data store, or to get a pointer to
 * control transfer data. The data is then packetised and sent to the host.
 *
 * Standard request handler handles the 'chapter 9' processing, specifically
 * the standard device requests in table 9-3 from the universal serial bus
 * specification revision 2.0.
 */

#include "usb_device.h"
#include "qm_common.h"
#include "usb_struct.h"
#include <errno.h>
#include <stddef.h>

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

/** General descriptor field offsets. */
#define DESC_LENGTH (0)
#define DESC_DESC_TYPE (1)

/** Config descriptor field offsets. */
#define CONF_DESC_TOTAL_LEN (2)
#define CONF_DESC_CONFIG_VALUE (5)
#define CONF_DESC_ATTRIBUTES (7)

/** Interface descriptor field offsets. */
#define INTF_DESC_ALT_SETTING (3)

/** Endpoint descriptor field offsets. */
#define ENDP_DESC_ENDPOINT_ADDRESS (2)
#define ENDP_DESC_ATTRIBUTES (3)
#define ENDP_DESC_MAX_PACKET_SIZE (4)

#define MAX_NUM_REQ_HANDLERS (4)
#define MAX_STD_REQ_MSG_SIZE (8)

static int handle_standard_request(usb_setup_packet_t *pSetup, uint32_t *len,
				   uint8_t **data);

static void handle_out_control_transfer(void *data, int error,
					const qm_usb_ep_idx_t ep,
					const qm_usb_ep_status_t ep_status);

static void handle_in_control_transfer(void *data, int error,
				       const qm_usb_ep_idx_t ep,
				       const qm_usb_ep_status_t ep_status);

static const usb_device_config_t *dev_config;
static usb_setup_packet_t setup_packet; /** setup packet */
static uint8_t *data_buf;		/** pointer to data buffer */
static uint32_t data_count;		/** remaining bytes in buffer */
static uint32_t control_xfer_len;       /** total length of control transfer */
static uint8_t current_config;		/** currently selected configuration */

/** Array of installed request data pointers. */
static uint8_t *req_data_store[MAX_NUM_REQ_HANDLERS];

/** Variable to check whether the usb has been enabled yet. */
static bool usb_enabled;

/** Buffer used for storing standard usb request data. */
static uint8_t std_request_data[MAX_STD_REQ_MSG_SIZE];

static const qm_usb_ep_config_t ep0_cfg[] = {
    {.type = QM_USB_EP_CONTROL,
     .max_packet_size = QM_USB_MAX_PACKET_SIZE,
     .ep = QM_USB_OUT_EP_0,
     .callback = handle_out_control_transfer,
     .callback_data = NULL},
    {.type = QM_USB_EP_CONTROL,
     .max_packet_size = QM_USB_MAX_PACKET_SIZE,
     .ep = QM_USB_IN_EP_0,
     .callback = handle_in_control_transfer,
     .callback_data = NULL}};

/*
 * Handle a request by calling one of the installed request handlers.
 *
 * In case of data going from host to device, the data is at **data.
 * In case of data going from device to host, the handler can either
 * choose to write its data to *data or update the data pointer.
 */
static bool handle_request(usb_setup_packet_t *pSetup, uint32_t *len,
			   uint8_t **data)
{
	const int32_t type = REQTYPE_GET_TYPE(pSetup->request_type);
	usb_request_handler_t handler;

	switch (type) {
	case (REQTYPE_TYPE_STANDARD):
		handler = &handle_standard_request;
		break;
	case (REQTYPE_TYPE_CLASS):
		handler = dev_config->interface.class_handler;
		break;
	default:
		return false;
	}

	if ((*handler)(pSetup, len, data) < 0) {
		return false;
	}

	return true;
}

/**
 * Sends next chunk of data (possibly 0 bytes) to host.
 */
static void transmit_data(void)
{
	uint32_t chunk = 0;

	chunk = MIN(QM_USB_MAX_PACKET_SIZE, data_count);

	/* Always EP0 for control. */
	qm_usb_ep_write(QM_USB_0, QM_USB_IN_EP_0, data_buf, chunk, &chunk);
	data_buf += chunk;
	data_count -= chunk;
}

/**
 * Handles Setup Packet from an OUT Control transfer.
 */
static void handle_setup_packet()
{
	uint32_t type = 0;
	/* Setup packet, reset request message state machine. */
	if (qm_usb_ep_read(QM_USB_0, QM_USB_OUT_EP_0, (uint8_t *)&setup_packet,
			   sizeof(setup_packet), NULL) < 0) {
		qm_usb_ep_set_stall_state(QM_USB_0, QM_USB_IN_EP_0, true);
	} else {
		/* Defaults for data pointer and residue. */
		type = REQTYPE_GET_TYPE(setup_packet.request_type);
		data_buf = req_data_store[type];
		data_count = setup_packet.length;
		control_xfer_len = setup_packet.length;

		if ((setup_packet.length == 0) ||
		    (REQTYPE_GET_DIR(setup_packet.request_type) ==
		     REQTYPE_DIR_TO_HOST)) {
			/* Ask installed handler to process
			 * request.
			 */
			if (!handle_request(&setup_packet, &control_xfer_len,
					    &data_buf)) {
				qm_usb_ep_set_stall_state(QM_USB_0,
							  QM_USB_IN_EP_0, true);
			} else {
				/* Send smallest of requested
				 * and offered length.
				 */
				data_count =
				    MIN(control_xfer_len, setup_packet.length);
				/* Send first part (possibly a
				 * zero-length
				 * status message)
				 */
				transmit_data();
			}
		}
	}
}

/**
 * Handles OUT transfers on EP0.
 */
static void handle_out_control_transfer(void *data, int error,
					const qm_usb_ep_idx_t ep,
					const qm_usb_ep_status_t ep_status)
{
	uint32_t chunk = 0;
	uint32_t type = 0;
	(void)(data);  /*Variable is not used in this callback.*/
	(void)(error); /*Variable is not used in this callback.*/
	(void)(ep);    /*Variable is not used in this callback.*/
	if (ep_status == QM_USB_EP_SETUP) {
		handle_setup_packet();
		return;
	}

	if (data_count > 0) {
		if (qm_usb_ep_read(QM_USB_0, QM_USB_OUT_EP_0, data_buf,
				   data_count, &chunk) < 0) {
			qm_usb_ep_set_stall_state(QM_USB_0, QM_USB_IN_EP_0,
						  true);
			return;
		}

		data_buf += chunk;
		data_count -= chunk;
		if (data_count == 0) {
			/*Received all, send data to handler*/
			type = REQTYPE_GET_TYPE(setup_packet.request_type);
			data_buf = req_data_store[type];
			if (!handle_request(&setup_packet, &control_xfer_len,
					    &data_buf)) {
				qm_usb_ep_set_stall_state(QM_USB_0,
							  QM_USB_IN_EP_0, true);
			} else {
				/*Send status to host*/
				transmit_data();
			}
		}
	} else {
		/* absorb zero-length status message */
		if (qm_usb_ep_read(QM_USB_0, QM_USB_OUT_EP_0, data_buf, 0,
				   &chunk) < 0) {
			qm_usb_ep_set_stall_state(QM_USB_0, QM_USB_IN_EP_0,
						  true);
		}
	}
}

/**
 * Handles IN transfers on EP0.
 */
static void handle_in_control_transfer(void *data, int error,
				       const qm_usb_ep_idx_t ep,
				       const qm_usb_ep_status_t ep_status)
{
	(void)(data);      /*Variable is not used in this callback.*/
	(void)(error);     /*Variable is not used in this callback.*/
	(void)(ep);	/*Variable is not used in this callback.*/
	(void)(ep_status); /*Variable is not used in this callback.*/

	/*Send more data if available */
	if (data_count != 0) {
		transmit_data();
	}
}

/**
 * Parses the list of installed USB descriptors and attempts to find the
 * specified USB descriptor.
 */
static bool get_descriptor(uint16_t type_idx, uint16_t lang_id, uint32_t *len,
			   uint8_t **data)
{
	uint8_t type = 0;
	uint8_t index = 0;
	uint8_t *descriptor = NULL;
	int32_t current_idx = 0;
	/*TODO: variable can be used in the future.*/
	(void)(lang_id); /*Variable is not used in this callback.*/

	QM_ASSERT(dev_config->device_description != NULL);

	type = GET_DESC_TYPE(type_idx);
	index = GET_DESC_INDEX(type_idx);

	descriptor = (uint8_t *)dev_config->device_description;
	current_idx = 0;

	while (descriptor[DESC_LENGTH] != 0) {
		if (descriptor[DESC_DESC_TYPE] == type) {
			if (current_idx == index) {
				/* set data pointer */
				*data = descriptor;
				/* get length from structure */
				if (type == DESC_CONFIGURATION) {
					/* configuration descriptor is an
					 * exception,
					 * length is at offset 2 and 3
					 */
					*len =
					    (descriptor[CONF_DESC_TOTAL_LEN]) |
					    (descriptor[CONF_DESC_TOTAL_LEN + 1]
					     << 8);
				} else {
					/* normally length is at offset 0 */
					*len = descriptor[DESC_LENGTH];
				}
				return true;
			}
			current_idx++;
		}
		/* skip to next descriptor */
		descriptor += descriptor[DESC_LENGTH];
	}
	/* nothing found */
	return false;
}

/**
 * Configures the device according to the specified configuration index.
 *
 * The alternate setting parsed from the installed USB descriptor list
 * is also taken into account. A configuration index of 0 'deconfigures'
 * the device.
 */
static bool set_config(uint8_t config_idx, uint8_t alt_setting)
{
	uint8_t *descriptor = NULL;
	uint8_t current_cfg = 0;
	uint8_t current_alt_setting = 0;

	QM_ASSERT(dev_config->device_description != NULL);

	/* deconfig device */
	if (config_idx == 0) {
		return true;
	}

	/* configure endpoints for this configuration/altsetting */
	descriptor = (uint8_t *)dev_config->device_description;
	current_cfg = 0xFF;
	current_alt_setting = 0xFF;

	while (descriptor[DESC_LENGTH] != 0) {

		switch (descriptor[DESC_DESC_TYPE]) {

		case DESC_CONFIGURATION:
			/* remember current configuration index */
			current_cfg = descriptor[CONF_DESC_CONFIG_VALUE];
			break;

		case DESC_INTERFACE:
			/* remember current alternate setting */
			current_alt_setting = descriptor[INTF_DESC_ALT_SETTING];
			break;

		case DESC_ENDPOINT:
			if ((current_cfg == config_idx) &&
			    (current_alt_setting == alt_setting)) {
				const uint8_t ep_addr =
				    descriptor[ENDP_DESC_ENDPOINT_ADDRESS];
				uint8_t ep_idx =
				    ep_addr & ~QM_USB_EP_DIR_IN_MASK;
				if (!(ep_addr & QM_USB_EP_DIR_IN_MASK))
					ep_idx += QM_USB_IN_EP_NUM;

				int i;
				for (i = 0; i < dev_config->num_endpoints;
				     i++) {
					if (dev_config->endpoints[i].ep ==
					    ep_idx) {
						dev_config->endpoints[i].type =
						    descriptor
							[ENDP_DESC_ATTRIBUTES];
						dev_config->endpoints[i]
						    .max_packet_size =
						    (descriptor
							 [ENDP_DESC_MAX_PACKET_SIZE]) |
						    (descriptor
							 [ENDP_DESC_MAX_PACKET_SIZE +
							  1]
						     << 8);
						qm_usb_ep_set_config(
						    QM_USB_0,
						    &dev_config->endpoints[i]);
						qm_usb_ep_enable(QM_USB_0,
								 ep_idx);
						break;
					}
				}
			}
			break;
		default:
			break;
		}

		/* skip to next descriptor */
		descriptor += descriptor[DESC_LENGTH];
	}

	if (dev_config->status_callback)
		/* FIXME: add user data to this layer */
		dev_config->status_callback(NULL, 0, QM_USB_CONFIGURED);

	return true;
}

/**
 * Handle a standard device request.
 */
static bool handle_std_device_req(usb_setup_packet_t *pSetup, uint32_t *len,
				  uint8_t **data)
{
	bool ret = true;
	uint8_t *const data_ptr = *data;

	switch (pSetup->request) {

	case REQ_GET_STATUS:
		/* bit 0: self-powered */
		/* bit 1: remote wakeup = not supported */
		data_ptr[0] = 0;
		data_ptr[1] = 0;
		*len = 2;
		break;

	case REQ_SET_ADDRESS:
		qm_usb_set_address(QM_USB_0, pSetup->value);
		break;

	case REQ_GET_DESCRIPTOR:
		ret = get_descriptor(pSetup->value, pSetup->index, len, data);
		break;

	case REQ_GET_CONFIGURATION:
		/* indicate if we are configured */
		data_ptr[0] = current_config;
		*len = 1;
		break;

	case REQ_SET_CONFIGURATION:
		if (!set_config(pSetup->value & 0xFF, 0)) {
			ret = false;
		} else {
			/* configuration successful, update current
			 * configuration */
			current_config = pSetup->value & 0xFF;
		}
		break;

	case REQ_CLEAR_FEATURE:
		break;
	case REQ_SET_FEATURE:

		if (pSetup->value == FEA_REMOTE_WAKEUP) {
			/* put DEVICE_REMOTE_WAKEUP code here */
		}

		if (pSetup->value == FEA_TEST_MODE) {
			/* put TEST_MODE code here */
		}
		ret = false;
		break;

	case REQ_SET_DESCRIPTOR:
		ret = false;
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

/**
 *  Handle a standard interface request.
 */
static bool handle_std_interface_req(usb_setup_packet_t *pSetup, uint32_t *len,
				     uint8_t **data)
{
	uint8_t *const data_ptr = *data;

	switch (pSetup->request) {
	case REQ_GET_STATUS:
		/* no bits specified */
		data_ptr[0] = 0;
		data_ptr[1] = 0;
		*len = 2;
		break;

	case REQ_CLEAR_FEATURE:
	case REQ_SET_FEATURE:
		/* not defined for interface */
		return false;

	case REQ_GET_INTERFACE:
		/* there is only one interface, return n-1 (= 0) */
		data_ptr[0] = 0;
		*len = 1;
		break;

	case REQ_SET_INTERFACE:
		*len = 0;
		break;

	default:
		return false;
	}

	return true;
}

/**
 * Handle a standard endpoint request.
 */
static bool handle_std_endpoint_req(usb_setup_packet_t *pSetup, uint32_t *len,
				    uint8_t **data)
{
	uint8_t *const data_ptr = *data;
	bool stall_state = false;

	/* Convert from Address to qm_usb_ep_idx_t. */
	uint8_t ep_idx = pSetup->index & ~QM_USB_EP_DIR_IN_MASK;
	if (!(pSetup->index & QM_USB_EP_DIR_IN_MASK)) {
		ep_idx += QM_USB_IN_EP_NUM;
	}

	switch (pSetup->request) {
	case REQ_GET_STATUS:
		/* bit 0 = endpoint halted or not */
		qm_usb_ep_is_stalled(QM_USB_0, ep_idx, &stall_state);
		data_ptr[0] = stall_state;
		data_ptr[1] = 0;
		*len = 2;
		break;

	case REQ_CLEAR_FEATURE:
		if (pSetup->value == FEA_ENDPOINT_HALT) {
			/* clear HALT by un-stalling */
			qm_usb_ep_set_stall_state(QM_USB_0, ep_idx, false);
			break;
		}
		/* only ENDPOINT_HALT defined for endpoints */
		return false;

	case REQ_SET_FEATURE:
		if (pSetup->value == FEA_ENDPOINT_HALT) {
			/* set HALT by stalling */
			qm_usb_ep_set_stall_state(QM_USB_0, ep_idx, true);
			break;
		}
		/* only ENDPOINT_HALT defined for endpoints */
		return false;

	case REQ_SYNCH_FRAME:
	default:
		return false;
	}

	return true;
}

/**
 * Default handler for standard ('chapter 9') requests.
 *
 * If a custom request handler was installed, it is called first.
 */
static int handle_standard_request(usb_setup_packet_t *pSetup, uint32_t *len,
				   uint8_t **data)
{
	int rc = 0;
	/* try the custom request handler first */
	if ((dev_config->interface.custom_handler != NULL) &&
	    (!dev_config->interface.custom_handler(pSetup, len, data)))
		return 0;

	switch (REQTYPE_GET_RECIP(pSetup->request_type)) {
	case REQTYPE_RECIP_DEVICE:
		if (handle_std_device_req(pSetup, len, data) == false)
			rc = -EINVAL;
		break;
	case REQTYPE_RECIP_INTERFACE:
		if (handle_std_interface_req(pSetup, len, data) == false)
			rc = -EINVAL;
		break;
	case REQTYPE_RECIP_ENDPOINT:
		if (handle_std_endpoint_req(pSetup, len, data) == false)
			rc = -EINVAL;
		break;
	default:
		rc = -EINVAL;
	}

	return rc;
}

int usb_enable(const usb_device_config_t *config)
{
	int ret;

	QM_CHECK(config, -EINVAL);

	if (usb_enabled) {
		return 0;
	}

	dev_config = config;

	req_data_store[REQTYPE_TYPE_STANDARD] = std_request_data;
	req_data_store[REQTYPE_TYPE_CLASS] = config->interface.data;

	ret = qm_usb_set_status_callback(QM_USB_0, config->status_callback);
	if (ret < 0) {
		return ret;
	}

	ret = qm_usb_attach(QM_USB_0);
	if (ret < 0) {
		return ret;
	}

	ret = qm_usb_ep_set_config(QM_USB_0, &ep0_cfg[0]);
	if (ret < 0) {
		return ret;
	}

	ret = qm_usb_ep_set_config(QM_USB_0, &ep0_cfg[1]);
	if (ret < 0) {
		return ret;
	}

	/* enable control EP */
	ret = qm_usb_ep_enable(QM_USB_0, QM_USB_OUT_EP_0);
	if (ret < 0) {
		return ret;
	}

	ret = qm_usb_ep_enable(QM_USB_0, QM_USB_IN_EP_0);
	if (ret < 0) {
		return ret;
	}

	usb_enabled = true;

	return 0;
}

int usb_disable(void)
{
	int ret;

	if (!usb_enabled) {
		return 0;
	}

	ret = qm_usb_detach(QM_USB_0);
	if (ret < 0) {
		return ret;
	}

	usb_enabled = false;
	dev_config = NULL;

	return 0;
}
