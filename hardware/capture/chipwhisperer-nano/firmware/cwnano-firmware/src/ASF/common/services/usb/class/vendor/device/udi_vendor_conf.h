/**
 * \file
 *
 * \brief Default Vendor class configuration for a USB Device
 * with a single interface
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

#ifndef _UDI_VENDOR_CONF_H_
#define _UDI_VENDOR_CONF_H_

#include "conf_usb.h"

/**
 * \addtogroup udi_vendor_group_single_desc
 * @{
 */

//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Endpoint numbers used by vendor interface.
//! Note: The order of endpoint can depend on USB hardware capability
//! when a specific mapping is used on USB DPRAM.
#if SAM3S || SAM4S || SAM4E
#  define  UDI_VENDOR_EP_INTERRUPT_IN  (3 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_INTERRUPT_OUT (6 | USB_EP_DIR_OUT)
#  define  UDI_VENDOR_EP_BULK_IN       (1 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_BULK_OUT      (2 | USB_EP_DIR_OUT)
#  define  UDI_VENDOR_EP_ISO_IN        (4 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_ISO_OUT       (5 | USB_EP_DIR_OUT)
#elif SAM3U
#  define  UDI_VENDOR_EP_INTERRUPT_IN  (3 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_INTERRUPT_OUT (4 | USB_EP_DIR_OUT)
#  define  UDI_VENDOR_EP_BULK_IN       (1 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_BULK_OUT      (2 | USB_EP_DIR_OUT)
#  define  UDI_VENDOR_EP_ISO_IN        (5 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_ISO_OUT       (6 | USB_EP_DIR_OUT)
#elif SAM3XA
#  define  UDI_VENDOR_EP_INTERRUPT_IN  (3 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_INTERRUPT_OUT (4 | USB_EP_DIR_OUT)
#  define  UDI_VENDOR_EP_BULK_IN       (5 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_BULK_OUT      (6 | USB_EP_DIR_OUT)
#  define  UDI_VENDOR_EP_ISO_IN        (1 | USB_EP_DIR_IN)
#  define  UDI_VENDOR_EP_ISO_OUT       (2 | USB_EP_DIR_OUT)
#elif SAMG55
#define  UDI_VENDOR_EP_INTERRUPT_IN  (1 | USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_INTERRUPT_OUT (2 | USB_EP_DIR_OUT)
#define  UDI_VENDOR_EP_BULK_IN       (3 | USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_BULK_OUT      (4 | USB_EP_DIR_OUT)
#else
#define  UDI_VENDOR_EP_INTERRUPT_IN  (1 | USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_INTERRUPT_OUT (2 | USB_EP_DIR_OUT)
#define  UDI_VENDOR_EP_BULK_IN       ((((UDI_VENDOR_EPS_SIZE_INT_FS)?2:0)+1)\
		| USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_BULK_OUT      ((((UDI_VENDOR_EPS_SIZE_INT_FS)?2:0)+2)\
		| USB_EP_DIR_OUT)
#define  UDI_VENDOR_EP_ISO_IN        ((((UDI_VENDOR_EPS_SIZE_INT_FS)?2:0)+ \
	((UDI_VENDOR_EPS_SIZE_BULK_FS)?2:0)+1)\
		| USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_ISO_OUT       ((((UDI_VENDOR_EPS_SIZE_INT_FS)?2:0)+ \
	((UDI_VENDOR_EPS_SIZE_BULK_FS)?2:0)+2)\
		| USB_EP_DIR_OUT)
#endif

//! Interface number is 0 because it is the unique interface
#define  UDI_VENDOR_IFACE_NUMBER 0

/**
 * \name UDD Configuration
 */
//@{
//! Maximum 6 endpoints used by vendor interface
#define UDI_VENDOR_EP_NB_INT  ((UDI_VENDOR_EPS_SIZE_INT_FS)?2:0)
#define UDI_VENDOR_EP_NB_BULK ((UDI_VENDOR_EPS_SIZE_BULK_FS)?2:0)
#define UDI_VENDOR_EP_NB_ISO  ((UDI_VENDOR_EPS_SIZE_ISO_FS)?2:0)
#undef USB_DEVICE_MAX_EP   // undefine this definition in header file
#define USB_DEVICE_MAX_EP     (UDI_VENDOR_EP_NB_INT+UDI_VENDOR_EP_NB_BULK+UDI_VENDOR_EP_NB_ISO)
//@}

//@}

#include "udi_vendor.h"

#endif // _UDI_VENDOR_CONF_H_
