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

#include "usb_cdc_acm.h"
#include "usb_common.h"
#include "usb_device.h"
#include <string.h>

/* Internal rx buffer size. */
#define CDC_ACM_BUFFER_SIZE (2 * CDC_BULK_EP_MPS)

#define LOW_BYTE(x) ((x)&0xFF)
#define HIGH_BYTE(x) ((x) >> 8)

/*
 * Data structure for GET_LINE_CODING / SET_LINE_CODING class requests.
 * Refer to the CDC-ACM specification for members documentations.
 */
typedef struct {
	uint32_t dwDTERate;
	uint8_t bCharFormat;
	uint8_t bParityType;
	uint8_t bDataBits;
} cdc_acm_line_coding_t;

/* CDC-ACM notification data. Please refer to the spec for more information. */
typedef struct {
	uint8_t bmRequestType;
	uint8_t bNotificationType;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t data;
} cdc_acm_notification_t;

/* Device data structure. */
typedef struct {
	qm_usb_status_t usb_status;
	uint8_t tx_ready;		     /* TX ready status. */
	uint8_t rx_ready;		     /* RX ready status. */
	uint8_t tx_irq_ena;		     /* TX interrupt enable status. */
	uint8_t rx_irq_ena;		     /* RX interrupt enable status. */
	uint8_t rx_buf[CDC_ACM_BUFFER_SIZE]; /* Internal RX buffer. */
	uint32_t rx_buf_head; /* Head of the internal RX buffer. */
	uint32_t rx_buf_tail; /* Tail of the internal RX buffer. */
	/* Interface data buffer. */
	uint8_t interface_data[CDC_CLASS_REQ_MAX_DATA_SIZE];
	/* CDC ACM line coding properties. LE order. */
	cdc_acm_line_coding_t line_coding;
	/* CDC ACM line state bitmap, DTE side. */
	uint8_t line_state;
	/* CDC ACM serial state bitmap, DCE side. */
	uint8_t serial_state;
	/* CDC ACM notification sent status. */
	uint8_t notification_sent;
	void (*callback)(void);
} cdc_acm_data_t;

static cdc_acm_data_t cdc_acm_data = {
    /* 115200, no parity, 1 stop bit, 8bits char. */
    .line_coding = {115200, 0, 0, 8}};

/* Structure representing the global USB description. */
static const uint8_t cdc_acm_usb_description[] = {
    /* Device descriptor. */
    USB_DEVICE_DESC_SIZE,		   /* Descriptor size. */
    USB_DEVICE_DESC,			   /* Descriptor type. */
    LOW_BYTE(USB_1_1), HIGH_BYTE(USB_1_1), /* USB version in BCD format. */
    USB_COMMUNICATION_DEVICE_CLASS,	/* Class. */
    0x00,				   /* SubClass - Interface specific. */
    0x00,				   /* Protocol - Interface specific. */
    QM_USB_MAX_PACKET_SIZE,		   /* Max Packet Size. */
    LOW_BYTE(VENDOR_ID), HIGH_BYTE(VENDOR_ID),		 /* Vendor Id. */
    LOW_BYTE(CDC_PRODUCT_ID), HIGH_BYTE(CDC_PRODUCT_ID), /* Product Id. */
    LOW_BYTE(BCDDEVICE_RELNUM),
    HIGH_BYTE(BCDDEVICE_RELNUM), /* Device Release Number. */
    /* Index of Manufacturer String Descriptor. */
    0x01,
    /* Index of Product String Descriptor. */
    0x02,
    /* Index of Serial Number String Descriptor. */
    0x03, CDC_NUM_CONF, /* Number of Possible Configuration. */

    /* Configuration descriptor. */
    USB_CONFIGURATION_DESC_SIZE, /* Descriptor size. */
    USB_CONFIGURATION_DESC,      /* Descriptor type. */
    /* Total length in bytes of data returned. */
    LOW_BYTE(CDC_CONF_SIZE), HIGH_BYTE(CDC_CONF_SIZE),
    CDC_NUM_ITF,		  /* Number of interfaces. */
    0x01,			  /* Configuration value. */
    0x00,			  /* Index of the Configuration string. */
    USB_CONFIGURATION_ATTRIBUTES, /* Attributes. */
    USB_MAX_LOW_POWER,		  /* Max power consumption. */

    /* Interface descriptor. */
    USB_INTERFACE_DESC_SIZE,	/* Descriptor size. */
    USB_INTERFACE_DESC,		    /* Descriptor type. */
    0x00,			    /* Interface index. */
    0x00,			    /* Alternate setting. */
    CDC_ITF1_NUM_EP,		    /* Number of Endpoints. */
    USB_COMMUNICATION_DEVICE_CLASS, /* Class. */
    USB_ACM_SUBCLASS,		    /* SubClass. */
    USB_V25TER_PROTOCOL,	    /* Protocol. */
    /* Index of the Interface String Descriptor. */
    0x00,

    /* Header Functional Descriptor. */
    USB_HFUNC_DESC_SIZE,		   /* Descriptor size. */
    CS_INTERFACE,			   /* Descriptor type. */
    USB_HFUNC_SUBDESC,			   /* Descriptor SubType. */
    LOW_BYTE(USB_1_1), HIGH_BYTE(USB_1_1), /* CDC Device Release Number. */

    /* Call Management Functional Descriptor. */
    USB_CMFUNC_DESC_SIZE, /* Descriptor size. */
    CS_INTERFACE,	 /* Descriptor type. */
    USB_CMFUNC_SUBDESC,   /* Descriptor SubType. */
    0x00,		  /* Capabilities. */
    0x01,		  /* Data Interface. */

    /* ACM Functional Descriptor. */
    USB_ACMFUNC_DESC_SIZE, /* Descriptor size. */
    CS_INTERFACE,	  /* Descriptor type. */
    USB_ACMFUNC_SUBDESC,   /* Descriptor SubType. */

    /* Set_Line_Coding | Set_Control_Line_State | Get_Line_Coding | State */
    0x02,

    /* Union Functional Descriptor. */
    USB_UFUNC_DESC_SIZE, /* Descriptor size. */
    CS_INTERFACE,	/* Descriptor type. */
    USB_UFUNC_SUBDESC,   /* Descriptor SubType. */
    0x00,		 /* Master Interface. */
    0x01,		 /* Slave Interface. */

    /* Endpoint INT. */
    USB_ENDPOINT_DESC_SIZE, /* Descriptor size. */
    USB_ENDPOINT_DESC,      /* Descriptor type. */
    CDC_ENDP_INT,	   /* Endpoint address. */
    QM_USB_EP_INTERRUPT,    /* Attributes. */
    LOW_BYTE(CDC_INTERRUPT_EP_MPS),
    HIGH_BYTE(CDC_INTERRUPT_EP_MPS), /* Max packet size. */
    0x0A,			     /* Interval. */

    /* Interface descriptor. */
    USB_INTERFACE_DESC_SIZE,		 /* Descriptor size. */
    USB_INTERFACE_DESC,			 /* Descriptor type. */
    0x01,				 /* Interface index. */
    0x00,				 /* Alternate setting. */
    CDC_ITF2_NUM_EP,			 /* Number of Endpoints. */
    USB_COMMUNICATION_DEVICE_CLASS_DATA, /* Class. */
    0x00,				 /* SubClass. */
    0x00,				 /* Protocol. */
    /* Index of the Interface String Descriptor. */
    0x00,

    /* First Endpoint IN. */
    USB_ENDPOINT_DESC_SIZE, /* Descriptor size. */
    USB_ENDPOINT_DESC,      /* Descriptor type. */
    CDC_ENDP_IN,	    /* Endpoint address. */
    QM_USB_EP_BULK,	 /* Attributes. */
    LOW_BYTE(CDC_BULK_EP_MPS),
    HIGH_BYTE(CDC_BULK_EP_MPS), /* Max packet size. */
    0x00,			/* Interval. */

    /* Second Endpoint OUT. */
    USB_ENDPOINT_DESC_SIZE, /* Descriptor size. */
    USB_ENDPOINT_DESC,      /* Descriptor type. */
    CDC_ENDP_OUT,	   /* Endpoint address. */
    QM_USB_EP_BULK,	 /* Attributes. */
    LOW_BYTE(CDC_BULK_EP_MPS),
    HIGH_BYTE(CDC_BULK_EP_MPS), /* Max packet size. */
    0x00,			/* Interval. */

    /*
     * String descriptor language, only one, so min size 4 bytes.
     * 0x0409 English(US) language code used
     */
    USB_STRING_DESC_SIZE, /* Descriptor size. */
    USB_STRING_DESC,      /* Descriptor type. */
    0x09, 0x04,

    /* Manufacturer String Descriptor "Intel". */
    0x0C, USB_STRING_DESC, 'I', 0, 'n', 0, 't', 0, 'e', 0, 'l', 0,

    /* Product String Descriptor "CDC-ACM". */
    0x10, USB_STRING_DESC, 'C', 0, 'D', 0, 'C', 0, '-', 0, 'A', 0, 'C', 0, 'M',
    0,

    /* Serial Number String Descriptor "00.01". */
    0x0C, USB_STRING_DESC, '0', 0, '0', 0, '.', 0, '0', 0, '1', 0,
};

/**
 * Handler called for Class requests not handled by the USB stack.
 *
 * @param pSetup    Information about the request to execute.
 * @param len       Size of the buffer.
 * @param data      Buffer containing the request result.
 */
static int cdc_acm_class_handle_req(usb_setup_packet_t *pSetup, uint32_t *len,
				    uint8_t **data)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	switch (pSetup->request) {
	case CDC_SET_LINE_CODING:
		memcpy(&dev_data->line_coding, *data,
		       sizeof(dev_data->line_coding));
		QM_PRINTF("CDC_SET_LINE_CODING %d %d %d %d\n",
			  dev_data->line_coding.dwDTERate,
			  dev_data->line_coding.bCharFormat,
			  dev_data->line_coding.bParityType,
			  dev_data->line_coding.bDataBits);
		break;

	case CDC_SET_CONTROL_LINE_STATE:
		dev_data->line_state = (uint8_t)(pSetup->value);
		QM_PRINTF("CDC_SET_CONTROL_LINE_STATE 0x%x\n",
			  dev_data->line_state);
		break;

	case CDC_GET_LINE_CODING:
		*data = (uint8_t *)(&dev_data->line_coding);
		*len = sizeof(dev_data->line_coding);
		QM_PRINTF("CDC_GET_LINE_CODING %d %d %d %d\n",
			  dev_data->line_coding.dwDTERate,
			  dev_data->line_coding.bCharFormat,
			  dev_data->line_coding.bParityType,
			  dev_data->line_coding.bDataBits);
		break;

	default:
		QM_PRINTF("CDC ACM request 0x%x, value 0x%x\n", pSetup->request,
			  pSetup->value);
		return -EINVAL;
	}

	return 0;
}

/**
 * EP Bulk IN handler, used to send data to the Host.
 *
 * @param ep        Endpoint address.
 * @param ep_status Endpoint status code.
 */
static void cdc_acm_bulk_in(void *data, int error, qm_usb_ep_idx_t ep,
			    qm_usb_ep_status_t status)
{
	QM_PUTS("CDC ACM IN BULK callback!");
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	dev_data->tx_ready = 1;
	/* Call callback only if TX IRQ enabled. */
	if (dev_data->callback && dev_data->tx_irq_ena)
		dev_data->callback();
}

/**
 * EP Bulk OUT handler, used to read the data received from the Host.
 *
 * @param ep        Endpoint address.
 * @param ep_status Endpoint status code.
 */
static void cdc_acm_bulk_out(void *data, int error, qm_usb_ep_idx_t ep,
			     qm_usb_ep_status_t status)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;
	uint32_t bytes_to_read, i, j, buf_head;
	uint8_t tmp_buf[4];

	QM_PUTS("CDC ACM OUT BULK callback!");
	/* Check how many bytes were received. */
	qm_usb_ep_get_bytes_read(QM_USB_0, ep, &bytes_to_read);

	buf_head = dev_data->rx_buf_head;

	/*
	 * Quark SE USB controller is always storing data
	 * in the FIFOs per 32-bit words.
	 */
	for (i = 0; i < bytes_to_read; i += 4) {
		qm_usb_ep_read(QM_USB_0, ep, tmp_buf, 4, NULL);
		for (j = 0; j < 4; j++) {
			if (i + j == bytes_to_read) {
				/* We read all the data. */
				break;
			}

			if (((buf_head + 1) % CDC_ACM_BUFFER_SIZE) ==
			    dev_data->rx_buf_tail) {
				/* FIFO full, discard data. */
				QM_PUTS("CDC buffer full!");
			} else {
				dev_data->rx_buf[buf_head] = tmp_buf[j];
				buf_head = (buf_head + 1) % CDC_ACM_BUFFER_SIZE;
			}
		}
	}

	dev_data->rx_buf_head = buf_head;
	dev_data->rx_ready = 1;
	/* Call callback only if RX IRQ enabled. */
	if (dev_data->callback && dev_data->rx_irq_ena)
		dev_data->callback();
}

/**
 * EP Interrupt handler.
 *
 * @param ep        Endpoint address.
 * @param ep_status Endpoint status code.
 */
static void cdc_acm_int_in(void *data, int error, qm_usb_ep_idx_t ep,
			   qm_usb_ep_status_t ep_status)
{
	QM_PUTS("CDC ACM IN INT callback!");
	cdc_acm_data.notification_sent = 1;
}

/**
 * Callback used to know the USB connection status.
 *
 * @param status USB device status code.
 */
static void cdc_acm_dev_status_cb(void *data, int error, qm_usb_status_t status)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	if (error) {
		QM_PUTS("Error: USB device error");
		return;
	}

	/* Store the new status. */
	dev_data->usb_status = status;

	/* Check the USB status and do needed action if required. */
	switch (status) {
	case QM_USB_RESET:
		QM_PUTS("USB device reset detected");
		break;
	case QM_USB_CONNECTED:
		QM_PUTS("USB device connected");
		break;
	case QM_USB_CONFIGURED:
		QM_PUTS("USB device configured");
		break;
	case QM_USB_DISCONNECTED:
		QM_PUTS("USB device disconnected");
		break;
	case QM_USB_SUSPEND:
		QM_PUTS("USB device supended");
		break;
	case QM_USB_RESUME:
		QM_PUTS("USB device resumed");
		break;
	default:
		QM_PUTS("USB unknown state");
		break;
	}
}

/* Describe EndPoints configuration. */
static qm_usb_ep_config_t cdc_acm_eps[] = {
    {
     .callback = cdc_acm_int_in, .ep = QM_USB_IN_EP_1 /* CDC_ENDP_INT */
    },
    {
     .callback = cdc_acm_bulk_out, .ep = QM_USB_OUT_EP_3 /* CDC_ENDP_OUT */
    },
    {
     .callback = cdc_acm_bulk_in, .ep = QM_USB_IN_EP_4 /* CDC_ENDP_IN */
    }};

/* Configuration of the CDC-ACM Device send to the USB Driver. */
static usb_device_config_t cdc_acm_config = {
    .device_description = cdc_acm_usb_description,
    .status_callback = cdc_acm_dev_status_cb,
    .interface = {.class_handler = cdc_acm_class_handle_req,
		  .custom_handler = NULL,
		  .data = NULL},
    .num_endpoints = CDC_ITF1_NUM_EP + CDC_ITF2_NUM_EP,
    .endpoints = cdc_acm_eps};

int cdc_acm_init(void)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;
	int ret;

	cdc_acm_config.interface.data = dev_data->interface_data;

	/* Initialize the USB driver with the right configuration. */
	ret = usb_enable(&cdc_acm_config);
	if (ret < 0) {
		QM_PUTS("Error: Failed to enable USB");
		return ret;
	}

	return 0;
}

int cdc_acm_fifo_fill(const uint8_t *tx_data, int len)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;
	uint32_t bytes_written = 0;

	if (dev_data->usb_status != QM_USB_CONFIGURED) {
		return 0;
	}

	dev_data->tx_ready = 0;
	qm_usb_ep_write(QM_USB_0, QM_USB_IN_EP_4, tx_data, len, &bytes_written);

	return bytes_written;
}

int cdc_acm_fifo_read(uint8_t *rx_data, const int size)
{
	uint32_t bytes_read, i;
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	int avail_data = (CDC_ACM_BUFFER_SIZE + dev_data->rx_buf_head -
			  dev_data->rx_buf_tail) %
			 CDC_ACM_BUFFER_SIZE;

	if (avail_data > size) {
		bytes_read = size;
	} else {
		bytes_read = avail_data;
	}

	for (i = 0; i < bytes_read; i++) {
		rx_data[i] = dev_data->rx_buf[(dev_data->rx_buf_tail + i) %
					      CDC_ACM_BUFFER_SIZE];
	}

	dev_data->rx_buf_tail =
	    (dev_data->rx_buf_tail + bytes_read) % CDC_ACM_BUFFER_SIZE;

	if (dev_data->rx_buf_tail == dev_data->rx_buf_head) {
		/* Buffer empty. */
		dev_data->rx_ready = 0;
	}

	return bytes_read;
}

void cdc_acm_irq_tx_enable(void)
{
	cdc_acm_data.tx_irq_ena = 1;
}

void cdc_acm_irq_tx_disable(void)
{
	cdc_acm_data.tx_irq_ena = 0;
}

int cdc_acm_irq_tx_ready(void)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	if (dev_data->tx_ready) {
		dev_data->tx_ready = 0;
		return 1;
	}

	return 0;
}

void cdc_acm_irq_rx_enable(void)
{
	cdc_acm_data.rx_irq_ena = 1;
}

void cdc_acm_irq_rx_disable(void)
{
	cdc_acm_data.rx_irq_ena = 0;
}

int cdc_acm_irq_rx_ready(void)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	if (dev_data->rx_ready) {
		dev_data->rx_ready = 0;
		return 1;
	}

	return 0;
}

void cdc_acm_irq_callback_set(void (*cb)(void))
{
	cdc_acm_data.callback = cb;
}

/**
 * Send serial line state notification to the Host.
 *
 * This routine sends asynchronous notification of UART status
 * on the interrupt endpoint
 *
 * @param dev CDC ACM device struct.
 * @param ep_status Endpoint status code.
 */
static int cdc_acm_send_notification(uint16_t serial_state)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	cdc_acm_notification_t notification;

	notification.bmRequestType = 0xA1;
	notification.bNotificationType = 0x20;
	notification.wValue = 0;
	notification.wIndex = 0;
	notification.wLength = sizeof(serial_state);
	notification.data = serial_state;

	dev_data->notification_sent = 0;
	qm_usb_ep_write(QM_USB_0, QM_USB_IN_EP_1,
			(const uint8_t *)&notification, sizeof(notification),
			NULL);

	return 0;
}

int cdc_acm_line_ctrl_set(uint32_t ctrl, uint32_t val)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	switch (ctrl) {
	case LINE_CTRL_BAUD_RATE:
		/* Set Line Control Baudrate. */
		cdc_acm_data.line_coding.dwDTERate = val;
		return 0;
	case LINE_CTRL_DCD:
		/* Set "Data Carrier Detect" on the Line Control. */
		dev_data->serial_state &= ~CDC_CONTROL_SERIAL_STATE_DCD;
		if (val) {
			dev_data->serial_state |= CDC_CONTROL_SERIAL_STATE_DCD;
		}
		cdc_acm_send_notification(CDC_CONTROL_SERIAL_STATE_DCD);
		return 0;
	case LINE_CTRL_DSR:
		/* Set "Data Set Ready" on the Line Control. */
		dev_data->serial_state &= ~CDC_CONTROL_SERIAL_STATE_DSR;
		if (val) {
			dev_data->serial_state |= CDC_CONTROL_SERIAL_STATE_DSR;
		}
		cdc_acm_send_notification(dev_data->serial_state);
		return 0;
	default:
		return -ENODEV;
	}

	return -ENOTSUP;
}

int cdc_acm_line_ctrl_get(uint32_t ctrl, uint32_t *val)
{
	cdc_acm_data_t *const dev_data = &cdc_acm_data;

	switch (ctrl) {
	case LINE_CTRL_BAUD_RATE:
		*val = dev_data->line_coding.dwDTERate;
		return 0;
	case LINE_CTRL_RTS:
		*val =
		    (dev_data->line_state & CDC_CONTROL_LINE_STATE_RTS) ? 1 : 0;
		return 0;
	case LINE_CTRL_DTR:
		*val =
		    (dev_data->line_state & CDC_CONTROL_LINE_STATE_DTR) ? 1 : 0;
		return 0;
	}

	return -ENOTSUP;
}
