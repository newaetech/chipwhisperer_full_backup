/**
 * \file
 *
 * \brief USB Vendor class interface.
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_protocol_vendor.h"
#include "udd.h"
#include "udc.h"
#include "udi_vendor.h"
#include <string.h>

// Configuration check
#ifndef UDI_VENDOR_ENABLE_EXT
# error UDI_VENDOR_ENABLE_EXT must be defined in conf_usb.h file.
#endif
#ifndef UDI_VENDOR_DISABLE_EXT
# error UDI_VENDOR_DISABLE_EXT must be defined in conf_usb.h file.
#endif

/**
 * \ingroup udi_vendor_group
 * \defgroup udi_vendor_group_udc Interface with USB Device Core (UDC)
 *
 * Structures and functions required by UDC.
 *
 * @{
 */
bool udi_vendor_enable(void);
void udi_vendor_disable(void);
bool udi_vendor_setup(void);
uint8_t udi_vendor_getsetting(void);

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_vendor = {
	.enable = udi_vendor_enable,
	.disable = udi_vendor_disable,
	.setup = udi_vendor_setup,
	.getsetting = udi_vendor_getsetting,
	.sof_notify = NULL,
};
//@}


/**
 * \ingroup udi_vendor_group
 * \defgroup udi_vendor_group_internal Implementation of UDI Vendor Class
 *
 * Class internal implementation
 * @{
 */

//! USB descriptor alternate setting used
static uint8_t udi_vendor_alternate_setting = 0;

/**
 * \name Internal routines
 */
//@{
bool udi_vendor_enable(void)
{
	udi_vendor_alternate_setting = udc_get_interface_desc()->bAlternateSetting;
	if (0 == udi_vendor_alternate_setting) {
		// Call application callback
		// to notify that interface is enabled
		if (!UDI_VENDOR_ENABLE_EXT()) {
			return false;
		}
	}
	return true;
}


void udi_vendor_disable(void)
{
	if (0 == udi_vendor_alternate_setting) {
		UDI_VENDOR_DISABLE_EXT();
	}
}


bool udi_vendor_setup(void)
{
	if (Udd_setup_is_in()) {
		if ((Udd_setup_type() == USB_REQ_TYPE_VENDOR)) {
			return UDI_VENDOR_SETUP_IN_RECEIVED();
		}
	}
	if (Udd_setup_is_out()) {
		if ((Udd_setup_type() == USB_REQ_TYPE_VENDOR)) {
			return UDI_VENDOR_SETUP_OUT_RECEIVED();
		}
	}
	return false; // Not supported request
}

uint8_t udi_vendor_getsetting(void)
{
	return udi_vendor_alternate_setting;
}
//@}

#if UDI_VENDOR_EPS_SIZE_INT_FS
/**
 * \brief Start a transfer on interrupt IN
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_vendor_interrupt_in_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	return udd_ep_run(UDI_VENDOR_EP_INTERRUPT_IN,
			false,
			buf,
			buf_size,
			callback);
}


/**
 * \brief Start a transfer on interrupt OUT
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_vendor_interrupt_out_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	return udd_ep_run(UDI_VENDOR_EP_INTERRUPT_OUT,
			false,
			buf,
			buf_size,
			callback);
}
#endif

#if UDI_VENDOR_EPS_SIZE_BULK_FS
/**
 * \brief Start a transfer on bulk IN
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_vendor_bulk_in_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	return udd_ep_run(UDI_VENDOR_EP_BULK_IN,
			false,
			buf,
			buf_size,
			callback);
}


/**
 * \brief Start a transfer on bulk OUT
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_vendor_bulk_out_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	return udd_ep_run(UDI_VENDOR_EP_BULK_OUT,
			false,
			buf,
			buf_size,
			callback);
}
#endif


#if UDI_VENDOR_EPS_SIZE_ISO_FS
/**
 * \brief Start a transfer on interrupt IN
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_vendor_iso_in_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	return udd_ep_run(UDI_VENDOR_EP_ISO_IN,
			false,
			buf,
			buf_size,
			callback);
}


/**
 * \brief Start a transfer on interrupt OUT
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_vendor_iso_out_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback)
{
	return udd_ep_run(UDI_VENDOR_EP_ISO_OUT,
			false,
			buf,
			buf_size,
			callback);
}
#endif

//@}
