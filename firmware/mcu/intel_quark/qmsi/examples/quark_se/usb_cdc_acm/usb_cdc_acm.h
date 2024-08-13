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

#ifndef __USB_CDC_ACM_H__
#define __USB_CDC_ACM_H__

#include <stdint.h>

/**
 * CDC-ACM Product ID.
 *
 * This is used by the host to select the host side driver for this
 * USB device. The below is just a placeholder.
 */
#define CDC_PRODUCT_ID (0xD017)

/* Max packet size for Bulk endpoints. */
#define CDC_BULK_EP_MPS (64)

/* Max packet size for Interrupt endpoints. */
#define CDC_INTERRUPT_EP_MPS (16)

/* Max CDC ACM class request max data size. */
#define CDC_CLASS_REQ_MAX_DATA_SIZE (8)

/* Number of configurations for the USB Device. */
#define CDC_NUM_CONF (0x01)
/* Number of interfaces in the configuration. */
#define CDC_NUM_ITF (0x02)
/* Number of Endpoints in the first interface. */
#define CDC_ITF1_NUM_EP (0x01)
/* Number of Endpoints in the second interface. */
#define CDC_ITF2_NUM_EP (0x02)

#define CDC_ENDP_INT (0x81)
#define CDC_ENDP_OUT (0x03)
#define CDC_ENDP_IN (0x84)

/* Decriptor size in bytes. */
#define USB_HFUNC_DESC_SIZE (5)   /* Header Functional Descriptor. */
#define USB_CMFUNC_DESC_SIZE (5)  /* Call Management Functional Descriptor. */
#define USB_ACMFUNC_DESC_SIZE (4) /* ACM Functional Descriptor. */
#define USB_UFUNC_DESC_SIZE (5)   /* Union Functional Descriptor. */

/* Descriptor type. */
#define CS_INTERFACE (0x24)
#define CS_ENDPOINT (0x25)

/* Descriptor subtype. */
#define USB_HFUNC_SUBDESC (0x00)
#define USB_CMFUNC_SUBDESC (0x01)
#define USB_ACMFUNC_SUBDESC (0x02)
#define USB_UFUNC_SUBDESC (0x06)

/* Class specific request. */
#define CDC_SET_LINE_CODING (0x20)
#define CDC_GET_LINE_CODING (0x21)
#define CDC_SET_CONTROL_LINE_STATE (0x22)

/* Control line state signal bitmap. */
#define CDC_CONTROL_LINE_STATE_DTR (0x1)
#define CDC_CONTROL_LINE_STATE_RTS (0x2)

/* Serial state notification bitmap. */
#define CDC_CONTROL_SERIAL_STATE_DCD (0x1)
#define CDC_CONTROL_SERIAL_STATE_DSR (0x2)

/* Serial state notification timeout. */
#define CDC_CONTROL_SERIAL_STATE_TIMEOUT_US (100000)

/*
 * Size in bytes of the configuration sent to the Host on GetConfiguration()
 * request.
 *
 * For CDC-ACM: CONF + (2 x ITF) + (3 x EP) + HF + CMF + ACMF + UF -> 67 bytes.
 */
#define CDC_CONF_SIZE                                                          \
	(USB_CONFIGURATION_DESC_SIZE + (2 * USB_INTERFACE_DESC_SIZE) +         \
	 (3 * USB_ENDPOINT_DESC_SIZE) + 19)

/* Common line controls for console. */
#define LINE_CTRL_BAUD_RATE (1 << 0)
#define LINE_CTRL_RTS (1 << 1)
#define LINE_CTRL_DTR (1 << 2)
#define LINE_CTRL_DCD (1 << 3)
#define LINE_CTRL_DSR (1 << 4)

/**
 * Initialize CDC-ACM Device.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int cdc_acm_init(void);

/**
 * Manipulate line control for console.
 *
 * @param[in] ctrl The line control to be manipulated.
 * @param[in] val Value to set the line control.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int cdc_acm_line_ctrl_set(uint32_t ctrl, uint32_t val);

/**
 * Read line control for console.
 *
 * @param[in] ctrl The line control to be manipulated.
 * @param[in] val Value to set the line control.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int cdc_acm_line_ctrl_get(uint32_t ctrl, uint32_t *const val);

/**
 * Set the callback function pointer for IRQ.
 *
 * @param[in] cb Callback function pointer.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
void cdc_acm_irq_callback_set(void (*callback)(void));

/**
 * Check if Rx IRQ has been raised.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int cdc_acm_irq_rx_ready(void);

/**
 * Enable RX interrupt.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
void cdc_acm_irq_rx_enable(void);

/**
 * Check if TX IRQ has been raised.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int cdc_acm_irq_tx_ready(void);

/**
 * Enable TX interrupt.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
void cdc_acm_irq_tx_enable(void);

/**
 * Disable TX interrupt.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
void cdc_acm_irq_tx_disable(void);

/**
 * Fill FIFO with data.
 *
 * @param[in] tx_data Data to transmit.
 * @param[in] len Number of bytes to send.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int cdc_acm_fifo_fill(const uint8_t *const tx_data, int len);

/**
 * Read data from FIFO.
 *
 * @param[out] rx_data Pointer to data container.
 * @param[in] size Container size.
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int cdc_acm_fifo_read(uint8_t *const rx_data, const int size);

#endif /* __USB_CDC_ACM_H__ */
