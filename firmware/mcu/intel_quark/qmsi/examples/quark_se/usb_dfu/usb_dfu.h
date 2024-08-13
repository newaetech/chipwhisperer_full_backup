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

#ifndef __USB_DFU_H__
#define __USB_DFU_H__

#include "qm_flash.h"
#include "usb_common.h"

#define DFU_MAX_XFER_SIZE QM_FLASH_PAGE_SIZE_BYTES

/**
 * DFU Product ID.
 *
 * This is used by the host to select the host side driver for this
 * USB device. The below is just a placeholder.
 *
 * NOTE: Apparently, to make dfu-util work properly on Windows, the Product ID
 * in the application code (i.e., Product ID in Run-time mode) must be
 * different from the one in the bootloader (i.e., Product ID in DFU mode).
 */
#define DFU_PRODUCT_ID (0X48FC)

#define DFU_NUM_CONF (0x01) /* Number of configurations for the USB Device. */
#define DFU_NUM_ITF (0x01)  /* Number of interfaces in the configuration. */
#define DFU_NUM_EP (0x00)   /* Number of Endpoints in the interface. */

/* Class specific requests. */
#define DFU_DETACH (0x00)
#define DFU_DNLOAD (0x01)
#define DFU_UPLOAD (0x02)
#define DFU_GETSTATUS (0x03)
#define DFU_CLRSTATUS (0x04)
#define DFU_GETSTATE (0x05)
#define DFU_ABORT (0x06)

/* DFU Attributes. */
#define DFU_ATTR_CAN_DNLOAD (0x01)
#define DFU_ATTR_CAN_UPLOAD (0x02)
#define DFU_ATTR_MANIFESTATION_TOLERANT (0x4)

#define DFU_DETACH_TIMEOUT (1000)

/* DFU STATUS CODE. */
enum dfu_status {
	statusOK,
	errTARGET,
	errFILE,
	errWRITE,
	errERASE,
	errCHECK_ERASED,
	errPROG,
	errVERIFY,
	errADDRESS,
	errNOTDONE,
	errFIRMWARE,
	errVENDOR,
	errUSB,
	errPOR,
	errUNKNOWN,
	errSTALLEDPKT
};

/* DFU STATES. */
enum dfu_state {
	appIDLE,
	appDETACH,
	dfuIDLE,
	dfuDNLOAD_SYNC,
	dfuDNBUSY,
	dfuDNLOAD_IDLE,
	dfuMANIFEST_SYNC,
	dfuMANIFEST,
	dfuMANIFEST_WAIT_RST,
	dfuUPLOAD_IDLE,
	dfuERROR,
};

/*
 * Size in bytes of the configuration sent to the Host on
 * GetConfiguration() request.
 *
 * For DFU this is: (CONF + ITF + DFU) -> 27 bytes.
 */
#define DFU_CONF_SIZE                                                          \
	(USB_CONFIGURATION_DESC_SIZE + USB_INTERFACE_DESC_SIZE +               \
	 USB_DFU_DESC_SIZE)

/**
 * DFU class driver start routine
 *
 * @return Standard errno return type for QMSI.
 * @retval 0 on success.
 * @retval Negative @ref errno for possible error codes.
 */
int start_dfu(void);

#endif /* __USB_DFU_H__ */
