/*
 *  Copyright (c) 2016, Intel Corporation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *  3. Neither the name of the Intel Corporation nor the names of its
 *  contributors may be used to endorse or promote products derived from this
 *  software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @brief DFU class driver
 *
 * USB DFU device class driver
 *
 */

#include <errno.h>
#include <stdio.h>

#include "qm_flash.h"
#include "qm_gpio.h"
#include "qm_init.h"
#include "qm_interrupt.h"
#include "qm_isr.h"
#include "qm_pic_timer.h"

#include "usb_device.h"
#include "usb_dfu.h"
#include "usb_struct.h"

#include "bl_data.h"
#include "dfu/core/dfu_core.h"
#include "fw-manager.h"
#include "fw-manager_utils.h"

#define USB_VBUS_GPIO_PIN 28
#define USB_VBUS_GPIO_PORT QM_GPIO_0
#define DBG QM_PRINTF

#define LOW_BYTE(x) ((x)&0xFF)
#define HIGH_BYTE(x) ((x) >> 8)

/* Lakemont application's entry point (Flash1) */
#if (UNIT_TEST)
uint32_t test_lmt_app;
#define LMT_APP_ADDR (&test_lmt_app)
#else
#define LMT_APP_ADDR (0x40030000)
#endif

/* Generates one interrupt after 10 seconds with a 32MHz sysclk. */
#define TIMEOUT 0x14000000

/* Forward declarations */
static void dfu_status_cb(void *data, int error, qm_usb_status_t status);
static int dfu_class_handle_req(usb_setup_packet_t *pSetup, uint32_t *data_len,
				uint8_t **data);
static void timeout(void *data);

static uint8_t usb_buffer[DFU_MAX_XFER_SIZE]; /* DFU data buffer */

/* Set on USB detach, needed for the proprietary 'detach' extension of DFU. */
static bool usb_detached = false;

static const qm_pic_timer_config_t pic_conf = {.mode =
						   QM_PIC_TIMER_MODE_PERIODIC,
					       .int_en = true,
					       .callback = timeout,
					       .callback_data = NULL};

/* Structure representing the DFU mode USB description */
static const uint8_t dfu_mode_usb_description[] = {
    /* Device descriptor */
    USB_DEVICE_DESC_SIZE,		   /* Descriptor size */
    USB_DEVICE_DESC,			   /* Descriptor type */
    LOW_BYTE(USB_1_1), HIGH_BYTE(USB_1_1), /* USB version in BCD format */
    0x00,				   /* Class - Interface specific */
    0x00,				   /* SubClass - Interface specific */
    0x00,				   /* Protocol - Interface specific */
    MAX_PACKET_SIZE_EP0,		   /* EP0 Max Packet Size */
    LOW_BYTE(VENDOR_ID), HIGH_BYTE(VENDOR_ID),		 /* Vendor Id */
    LOW_BYTE(DFU_PRODUCT_ID), HIGH_BYTE(DFU_PRODUCT_ID), /* Product Id */
    LOW_BYTE(BCDDEVICE_RELNUM),
    HIGH_BYTE(BCDDEVICE_RELNUM), /* Device Release Number */
    0x01,			 /* Index of Manufacturer String Descriptor */
    0x02,			 /* Index of Product String Descriptor */
    0x03,			 /* Index of Serial Number String Descriptor */
    DFU_NUM_CONF,		 /* Number of Possible Configuration */

    /* Configuration descriptor */
    USB_CONFIGURATION_DESC_SIZE, /* Descriptor size */
    USB_CONFIGURATION_DESC,      /* Descriptor type */
    LOW_BYTE(DFU_RUNTIME_CONF_SIZE),
    HIGH_BYTE(
	DFU_RUNTIME_CONF_SIZE),   /* Total length in bytes of data returned */
    DFU_NUM_ITF,		  /* Number of interfaces */
    0x01,			  /* Configuration value */
    0x00,			  /* Index of the Configuration string */
    USB_CONFIGURATION_ATTRIBUTES, /* Attributes */
    USB_MAX_LOW_POWER,		  /* Max power consumption */

    /* Interface descriptor, alternate setting 0 */
    USB_INTERFACE_DESC_SIZE,    /* Descriptor size */
    USB_INTERFACE_DESC,		/* Descriptor type */
    0x00,			/* Interface index */
    0x00,			/* Alternate setting */
    DFU_NUM_EP,			/* Number of Endpoints */
    USB_DFU_CLASS,		/* Class */
    USB_DFU_INTERFACE_SUBCLASS, /* SubClass */
    USB_DFU_MODE_PROTOCOL,      /* DFU Runtime Protocol */
    0x04,			/* Index of the Interface String Descriptor */

    /* Interface descriptor, alternate setting 1 */
    USB_INTERFACE_DESC_SIZE,    /* Descriptor size */
    USB_INTERFACE_DESC,		/* Descriptor type */
    0x00,			/* Interface index */
    0x01,			/* Alternate setting */
    DFU_NUM_EP,			/* Number of Endpoints */
    USB_DFU_CLASS,		/* Class */
    USB_DFU_INTERFACE_SUBCLASS, /* SubClass */
    USB_DFU_MODE_PROTOCOL,      /* DFU Runtime Protocol */
    0x05,			/* Index of the Interface String Descriptor */

    /* Interface descriptor, alternate setting 2 */
    USB_INTERFACE_DESC_SIZE,    /* Descriptor size */
    USB_INTERFACE_DESC,		/* Descriptor type */
    0x00,			/* Interface index */
    0x02,			/* Alternate setting */
    DFU_NUM_EP,			/* Number of Endpoints */
    USB_DFU_CLASS,		/* Class */
    USB_DFU_INTERFACE_SUBCLASS, /* SubClass */
    USB_DFU_MODE_PROTOCOL,      /* DFU Runtime Protocol */
    0x06,			/* Index of the Interface String Descriptor */

    /* DFU descriptor */
    USB_DFU_DESC_SIZE,       /* Descriptor size */
    USB_DFU_FUNCTIONAL_DESC, /* Descriptor type DFU: Functional */
    DFU_ATTR_CAN_DNLOAD | DFU_ATTR_CAN_UPLOAD |
	DFU_ATTR_MANIFESTATION_TOLERANT, /* DFU attributes */
    LOW_BYTE(DFU_DETACH_TIMEOUT),
    HIGH_BYTE(DFU_DETACH_TIMEOUT), /* wDetachTimeOut */
    LOW_BYTE(DFU_MAX_XFER_SIZE),
    HIGH_BYTE(DFU_MAX_XFER_SIZE), /* wXferSize  - 512bytes */
    0x11, 0,			  /* DFU Version */

    /* String descriptor language, only one, so min size 4 bytes.
     * 0x0409 English(US) language code used.
     */
    USB_STRING_DESC_SIZE, /* Descriptor size */
    USB_STRING_DESC,      /* Descriptor type */
    0x09, 0x04,

    /* Manufacturer String Descriptor "Intel" */
    0x0C, USB_STRING_DESC, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'l', 0,

    /* Product String Descriptor "ATP-Dev1.0" */
    0x16, USB_STRING_DESC, 'A', 0, 'T', 0, 'P', 0, '-', 0, 'D', 0, 'e', 0, 'v',
    0, '1', 0, '.', 0, '0', 0,

    /* Serial Number String Descriptor "00.01" */
    0x0C, USB_STRING_DESC, '0', 0, '0', 0, '.', 0, '0', 0, '1', 0,

    /* Interface alternate setting 0 String Descriptor: "QDM" */
    0x08, USB_STRING_DESC, 'Q', 0, 'D', 0, 'M', 0,

    /* Interface alternate setting 0 String Descriptor: "PARTITION0" */
    0x16, USB_STRING_DESC, 'P', 0, 'A', 0, 'R', 0, 'T', 0, 'I', 0, 'T', 0, 'I',
    0, 'O', 0, 'N', 0, '1', 0,

    /* Interface alternate setting 0 String Descriptor: "PARTITION2" */
    0x16, USB_STRING_DESC, 'P', 0, 'A', 0, 'R', 0, 'T', 0, 'I', 0, 'T', 0, 'I',
    0, 'O', 0, 'N', 0, '2', 0};

static bool lmt_partition_is_bootable()
{
	const bool app_present = (0xffffffff != *(uint32_t *)LMT_APP_ADDR);

	return app_present;
}

static void reset(void)
{
	qm_soc_reset(QM_COLD_RESET);
}

static void timeout(void *data)
{
	(void)(data);
	/*
	 * If we timeout and we have a valid LMT image, load it.
	 * Otherwise, reset the timer.
	 */
	if (lmt_partition_is_bootable()) {
		qm_pic_timer_set(0);
		reset();
	} else {
		qm_pic_timer_set(TIMEOUT);
	}
}

static __inline__ void start_timer(void)
{
	qm_int_vector_request(QM_X86_PIC_TIMER_INT_VECTOR, qm_pic_timer_0_isr);

	qm_pic_timer_set_config(&pic_conf);

	qm_pic_timer_set(TIMEOUT);
}

/**
 * @brief Custom handler for standard ('chapter 9') requests
 *        in order to catch the SET_INTERFACE request and
 *        extract the interface alternate setting
 *
 * @param pSetup    Information about the request to execute.
 * @param len       Size of the buffer.
 * @param data      Buffer containing the request result.
 *
 * @return  0 if SET_INTERFACE request, -ENOTSUP otherwise.
 */

static int dfu_custom_handle_req(usb_setup_packet_t *pSetup, uint32_t *data_len,
				 uint8_t **data)
{
	if (REQTYPE_GET_RECIP(pSetup->request_type) ==
	    REQTYPE_RECIP_INTERFACE) {
		if (pSetup->request == REQ_SET_INTERFACE) {
			DBG("DFU alternate setting %d\n", pSetup->value);

			if (pSetup->value >= DFU_MODE_ALTERNATE_SETTINGS) {
				DBG("Invalid DFU alternate setting (%d)\n",
				    pSetup->value);
			} else {
				dfu_set_alt_setting(pSetup->value);
			}
			*data_len = 0;

			/* We this was a valid DFU Request, reset timeout.*/
			qm_pic_timer_set(TIMEOUT);
			return 0;
		}
	}

	/*Ignore the unused parameter error*/
	(void)(data);

	/* Not handled by us */
	return -ENOTSUP;
}

/* Configuration of the DFU Device send to the USB Driver */
static const usb_device_config_t dfu_config = {
    .device_description = dfu_mode_usb_description,
    .status_callback = dfu_status_cb,
    .interface = {.class_handler = dfu_class_handle_req,
		  .custom_handler = dfu_custom_handle_req,
		  .data = usb_buffer},
    .num_endpoints = DFU_NUM_EP};

/**
 * @brief Handler called for DFU Class requests not handled by the USB stack.
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
	dfu_dev_state_t state;
	dfu_dev_status_t status;
	uint32_t poll_timeout;
	int retv;
	uint16_t len;

	/* We get a DFU Request, reset timeout. */
	qm_pic_timer_set(TIMEOUT);

	switch (pSetup->request) {
	case DFU_GETSTATUS:
		DBG("DFU_GETSTATUS\n");
		retv = dfu_get_status(&status, &state, &poll_timeout);
		if (retv < 0) {
			return -EINVAL;
		}
		(*data)[0] = status;
		(*data)[1] = poll_timeout & 0xFF;
		(*data)[2] = (poll_timeout >> 8) & 0xFF;
		(*data)[3] = (poll_timeout >> 16) & 0xFF;
		(*data)[4] = state;
		(*data)[5] = 0;
		*data_len = 6;
		break;

	case DFU_GETSTATE:
		DBG("DFU_GETSTATE\n");
		retv = dfu_get_state(&state);
		if (retv < 0) {
			return -EINVAL;
		}
		(*data)[0] = state;
		*data_len = 1;
		break;

	case DFU_ABORT:
		DBG("DFU_ABORT\n");
		retv = dfu_abort();
		if (retv < 0) {
			return -EINVAL;
		}
		break;

	case DFU_CLRSTATUS:
		DBG("DFU_CLRSTATUS\n");
		retv = dfu_clr_status();
		if (retv < 0) {
			return -EINVAL;
		}
		break;

	case DFU_DNLOAD:
		DBG("DFU_DNLOAD block %d, len %d\n", pSetup->value,
		    pSetup->length);

		retv = dfu_process_dnload(pSetup->value, *data, pSetup->length);
		if (retv < 0) {
			return -EINVAL;
		}
		break;

	case DFU_UPLOAD:
		DBG("DFU_UPLOAD block %d, len %d\n", pSetup->value,
		    pSetup->length);
		retv = dfu_process_upload(pSetup->value, pSetup->length, *data,
					  &len);
		if (retv < 0) {
			return -EINVAL;
		}
		*data_len = len;
		break;
	case DFU_DETACH:
		DBG("DFU_DETACH timeout %d\n", pSetup->value);
		usb_detached = true;
		break;
	default:
		DBG("DFU UNKNOWN STATE: %d\n", pSetup->request);
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

	/* We get a DFU Request, reset timeout. */
	qm_pic_timer_set(TIMEOUT);

	/* Check the USB status and do needed action if required */
	switch (status) {
	case QM_USB_RESET:
		DBG("USB device reset detected\n");
		/*
		 * Linux seems to send several resets in short time, so reseting
		 * the system on any USB reset won't work.
		 * dfu-util has a proprietary extension 'detach' to work around
		 * this issue: only reset after an USB detach.
		 */
		if (usb_detached) {
			reset();
		}
		break;
	case QM_USB_CONNECTED:
		DBG("USB device connected\n");
		break;
	case QM_USB_CONFIGURED:
		DBG("USB device configured\n");
		break;
	case QM_USB_DISCONNECTED:
		DBG("USB device disconnected\n");
		break;
	case QM_USB_SUSPEND:
		DBG("USB device suspended\n");
		break;
	case QM_USB_RESUME:
		DBG("USB device resumed\n");
		break;
	default:
		DBG("USB unknown state\n");
		break;
	}
}

static void enable_usb_vbus(void)
{
	qm_gpio_port_config_t cfg = {0};
	cfg.direction |= BIT(USB_VBUS_GPIO_PIN);
	/* Here we assume the GPIO pinmux hasn't changed. */
	qm_gpio_set_config(USB_VBUS_GPIO_PORT, &cfg);
	qm_gpio_set_pin(USB_VBUS_GPIO_PORT, USB_VBUS_GPIO_PIN);
}

int usb_dfu_start(void)
{
	int ret;

	DBG("Starting DFU Device class\n");

	/* Initialize the DFU state machine */
	dfu_init();
	/* Set alternate setting for partition 0 (x86) */
	dfu_set_alt_setting(1);

	/* On MountAtlas we must set GPIO 28 to enable VCC_USB into the SoC. */
	enable_usb_vbus();

	/* Enable USB driver */
	ret = usb_enable(&dfu_config);
	if (ret < 0) {
		DBG("Failed to enable USB\n");
		return ret;
	}

	start_timer();

	return 0;
}
