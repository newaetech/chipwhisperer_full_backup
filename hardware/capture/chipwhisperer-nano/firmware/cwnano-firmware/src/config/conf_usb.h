/**
 * \file
 *
 * \brief USB configuration file
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

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"
#include "main.h"

/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
/* Only valid when compiling on known valid hardware */
#define  USB_DEVICE_VENDOR_ID             0x2B3E
#define  USB_DEVICE_PRODUCT_ID            0xACE0
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 400 // Consumption on VBUS line (mA)
#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
extern char usb_serial_number[33];

#define  USB_DEVICE_MANUFACTURE_NAME      "NewAE Technology Inc."
#define  USB_DEVICE_PRODUCT_NAME          "ChipWhisperer Nano"
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER usb_serial_number
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH 32

/**
 * Device speeds support
 * Low speed not supported by this vendor class
 * @{
 */
//! To authorize the High speed
#if (UC3A3||UC3A4)
# define  USB_DEVICE_HS_SUPPORT
#endif
//@}


/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
//#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              main_suspend_action()
#define  UDC_RESUME_EVENT()               main_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()

//@}


/**
 * USB Interface Configuration
 * @{
 */


/**
 * Configuration of vendor interface
 * @{
 */
//! Interface callback definition
#define UDI_VENDOR_ENABLE_EXT()           main_vendor_enable()
#define UDI_VENDOR_DISABLE_EXT()          main_vendor_disable()
#define UDI_VENDOR_SETUP_OUT_RECEIVED()   main_setup_out_received()
#define UDI_VENDOR_SETUP_IN_RECEIVED()    main_setup_in_received()

//! endpoints size for full speed
//! Note: Disable the endpoints of a type, if size equal 0
#define UDI_VENDOR_EPS_SIZE_INT_FS    0
#define UDI_VENDOR_EPS_SIZE_BULK_FS   64
#define UDI_VENDOR_EPS_SIZE_ISO_FS   0

//! endpoints size for high speed
#define UDI_VENDOR_EPS_SIZE_INT_HS    0
#define UDI_VENDOR_EPS_SIZE_BULK_HS  512
#define UDI_VENDOR_EPS_SIZE_ISO_HS    0

//@}

//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done
//! at the end of this file to avoid compile error
#include "udi_vendor_conf.h"

#endif // _CONF_USB_H_
