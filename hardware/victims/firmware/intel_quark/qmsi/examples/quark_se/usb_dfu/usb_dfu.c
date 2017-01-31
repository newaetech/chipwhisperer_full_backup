/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "usb_dfu.h"
#include "usb_device.h"
#include <errno.h>
#include <stdio.h>

#include "qm_init.h"

/* Assert the Firmware Management bit to request FM mode on the next boot. */
#define FM_STICKY_BIT_ASSERT() (QM_SCSS_GP->gps0 |= BIT(0))

#define LOW_BYTE(x) ((x)&0xFF)
#define HIGH_BYTE(x) ((x) >> 8)

/* Device data structure. */
struct dfu_data_t {
	uint8_t buffer[DFU_MAX_XFER_SIZE]; /* DFU data buffer. */
	enum dfu_state state;		   /* State of the DFU device. */
	enum dfu_status status;		   /* Status of the DFU device. */
};

static struct dfu_data_t dfu_data = {
    .state = appIDLE, .status = statusOK,
};

/* Structure representing the global USB description. */
static const uint8_t dfu_runtime_usb_description[] = {
    /* Device descriptor. */
    USB_DEVICE_DESC_SIZE,		   /* Descriptor size. */
    USB_DEVICE_DESC,			   /* Descriptor type. */
    LOW_BYTE(USB_1_1), HIGH_BYTE(USB_1_1), /* USB version in BCD format. */
    0x00,				   /* Class - Interface specific. */
    0x00,				   /* SubClass - Interface specific. */
    0x00,				   /* Protocol - Interface specific. */
    MAX_PACKET_SIZE_EP0,		   /* EP0 Max Packet Size. */
    LOW_BYTE(VENDOR_ID), HIGH_BYTE(VENDOR_ID),		 /* Vendor Id. */
    LOW_BYTE(DFU_PRODUCT_ID), HIGH_BYTE(DFU_PRODUCT_ID), /* Product Id. */
    LOW_BYTE(BCDDEVICE_RELNUM),
    HIGH_BYTE(BCDDEVICE_RELNUM), /* Device Release Number. */
    0x01,			 /* Index of Manufacturer String Descriptor. */
    0x02,			 /* Index of Product String Descriptor. */
    0x03,			 /* Index of Serial Number String Descriptor. */
    DFU_NUM_CONF,		 /* Number of Possible Configuration. */

    /* Configuration descriptor. */
    USB_CONFIGURATION_DESC_SIZE, /* Descriptor size. */
    USB_CONFIGURATION_DESC,      /* Descriptor type. */
    LOW_BYTE(DFU_CONF_SIZE),
    HIGH_BYTE(DFU_CONF_SIZE),     /* Total length in bytes of data returned. */
    DFU_NUM_ITF,		  /* Number of interfaces. */
    0x01,			  /* Configuration value. */
    0x00,			  /* Index of the Configuration string. */
    USB_CONFIGURATION_ATTRIBUTES, /* Attributes. */
    USB_MAX_LOW_POWER,		  /* Max power consumption. */

    /* Interface descriptor. */
    USB_INTERFACE_DESC_SIZE,    /* Descriptor size. */
    USB_INTERFACE_DESC,		/* Descriptor type. */
    0x00,			/* Interface index. */
    0x00,			/* Alternate setting. */
    DFU_NUM_EP,			/* Number of Endpoints. */
    USB_DFU_CLASS,		/* Class. */
    USB_DFU_INTERFACE_SUBCLASS, /* SubClass. */
    USB_DFU_RUNTIME_PROTOCOL,   /* DFU Runtime Protocol. */
    0x00,			/* Index of the Interface String Descriptor. */

    /* DFU descriptor. */
    USB_DFU_DESC_SIZE,       /* Descriptor size. */
    USB_DFU_FUNCTIONAL_DESC, /* Descriptor type DFU: Functional. */
    DFU_ATTR_CAN_DNLOAD | DFU_ATTR_CAN_UPLOAD |
	DFU_ATTR_MANIFESTATION_TOLERANT, /* DFU attributes. */
    LOW_BYTE(DFU_DETACH_TIMEOUT),
    HIGH_BYTE(DFU_DETACH_TIMEOUT), /* wDetachTimeOut. */
    LOW_BYTE(DFU_MAX_XFER_SIZE),
    HIGH_BYTE(DFU_MAX_XFER_SIZE), /* wXferSize  - 512bytes. */
    0x11, 0,			  /* DFU Version. */

    /* String descriptor language, only one, so min size 4 bytes.
     * 0x0409 English(US) language code used.
     */
    USB_STRING_DESC_SIZE, /* Descriptor size. */
    USB_STRING_DESC,      /* Descriptor type. */
    0x09, 0x04,

    /* Manufacturer String Descriptor "Intel". */
    0x0C, USB_STRING_DESC, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'l', 0,

    /* Product String Descriptor "QSE-Dev1.0". */
    0x16, USB_STRING_DESC, 'Q', 0, 'S', 0, 'E', 0, '-', 0, 'D', 0, 'e', 0, 'v',
    0, '1', 0, '.', 0, '0', 0,

    /* Serial Number String Descriptor "00.01". */
    0x0C, USB_STRING_DESC, '0', 0, '0', 0, '.', 0, '0', 0, '1', 0};

/**
 * Handler called for DFU Class requests not handled by the USB stack.
 *
 * @param pSetup    Information about the request to execute.
 * @param len       Size of the buffer.
 * @param data      Buffer containing the request result.
 *
 * @return  0 on success, negative errno code on fail.
 */
static int dfu_class_handle_req(usb_setup_packet_t *pSetup, uint32_t *data_len,
				uint8_t **data)
{
	switch (pSetup->request) {
	case DFU_GETSTATUS:
		/* NOTE: support for DFU_GETSTATUS is optional in run-time. */
		QM_PRINTF("DFU_GETSTATUS: status %d, state %d\n",
			  dfu_data.status, dfu_data.state);
		(*data)[0] = dfu_data.status;
		/* bwPollTimeout (3 bytes); ignored in app mode. */
		(*data)[1] = 0;
		(*data)[2] = 0;
		(*data)[3] = 0;
		(*data)[4] = dfu_data.state;
		(*data)[5] = 0;
		*data_len = 6;
		break;

	case DFU_GETSTATE:
		/* NOTE: support for DFU_GETSTATE is optional in run-time. */
		QM_PUTS("DFU_GETSTATE");
		(*data)[0] = dfu_data.state;
		*data_len = 1;
		break;

	case DFU_DETACH:
		QM_PRINTF("DFU_DETACH timeout %d, state %d\n", pSetup->value,
			  dfu_data.state);

		if (dfu_data.state == appDETACH) {
			dfu_data.state = appIDLE;
			return -EINVAL;
		}
		/* Move to appDETACH state. */
		dfu_data.state = appDETACH;

		/*
		 * We should start a timer here but in order to
		 * keep things simple and do not increase the size
		 * we rely on the host to get us out of the appATTACHED
		 * state if needed.
		 */
		break;
	default:
		QM_PRINTF("INVALID / UNEXPECTED DFU REQUEST: %d\n",
			  pSetup->request);
		return -EINVAL;
	}

	return 0;
}

/**
 * Callback used to know the USB connection status.
 *
 * @param data     Callback data. Not used here.
 * @param error    Error returned by the driver.
 * @param status   USB status code.
 */
static void dfu_status_cb(void *data, int error, qm_usb_status_t status)
{
	(void)data;
	if (error) {
		QM_PUTS("DFU device error");
	}

	/* Check the USB status and do needed action if required. */
	switch (status) {
	case QM_USB_RESET:
		QM_PRINTF("DFU device reset detected, state %d\n",
			  dfu_data.state);
		if (dfu_data.state == appDETACH) {
			FM_STICKY_BIT_ASSERT();
			qm_soc_reset(QM_WARM_RESET);
		}
		break;
	case QM_USB_CONNECTED:
		QM_PUTS("DFU device connected");
		break;
	case QM_USB_CONFIGURED:
		QM_PUTS("DFU device configured");
		break;
	case QM_USB_DISCONNECTED:
		QM_PUTS("DFU device disconnected");
		break;
	case QM_USB_SUSPEND:
		QM_PUTS("DFU device suspended");
		break;
	case QM_USB_RESUME:
		QM_PUTS("DFU device resumed");
		break;
	default:
		QM_PUTS("USB unknown state");
		break;
	}
}

/* Configuration of the DFU Device sent to the USB Stack. */
static const usb_device_config_t dfu_config = {
    .device_description = dfu_runtime_usb_description,
    .status_callback = dfu_status_cb,
    .interface = {.class_handler = dfu_class_handle_req,
		  .data = dfu_data.buffer},
    .num_endpoints = DFU_NUM_EP};

int start_dfu(void)
{
	return usb_enable(&dfu_config);
}
