/**
 * \file
 *
 * \brief USB configuration file
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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

//#warning You must refill the following definitions with a correct values

/**
 * USB Device Configuration
 * @{
 */

#define  USB_DEVICE_VENDOR_ID             0x2B3E
#define  USB_DEVICE_PRODUCT_ID            0xACE0

#define FW_VER_MAJOR 0
#define FW_VER_MINOR 62
#define FW_VER_DEBUG 0

#define  USB_DEVICE_MAJOR_VERSION         9
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 400 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                \
(USB_CONFIG_ATTR_BUS_POWERED)
// (USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions
//#define USB_DEVICE_HS_SUPPORT 1
extern char usb_serial_number[33];

#define  USB_DEVICE_MANUFACTURE_NAME      "NewAE Technology Inc."
#define  USB_DEVICE_PRODUCT_NAME          "ChipWhisperer Nano"
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER usb_serial_number
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH 32
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
// #define  USB_DEVICE_MANUFACTURE_NAME      "Manufacture name"
// #define  USB_DEVICE_PRODUCT_NAME          "Product name"
// #define  USB_DEVICE_SERIAL_NAME           "12...EF" // Disk SN for MSC

/**
 * Device speeds support
 * @{
 */
//! To define a Low speed device
//#define  USB_DEVICE_LOW_SPEED

//! To authorize the High speed
#if (UC3A3||UC3A4)
//#define  USB_DEVICE_HS_SUPPORT
#endif
//@}
#if 0
#define UDI_VENDOR_STRING_ID     0x10
#define UDI_CDC_COMM_STRING_ID_0 0x11
//#define UDI_CDC_COMM_STRING_ID_1 0x12


#define VENDOR_STRING "CWLite Interface"
#define CDC_DATA_STRING_0 "CWLite USART"
//#define CDC_DATA_STRING_1 "CW310 USART Debug Interface"

static inline const char *get_extra_str(uint8_t id)
{
	if (id == UDI_VENDOR_STRING_ID)
	return VENDOR_STRING;
	if (id == UDI_CDC_COMM_STRING_ID_0)
	return CDC_DATA_STRING_0;
	//if (id == UDI_CDC_COMM_STRING_ID_1)
	//return CDC_DATA_STRING_1;
	return NULL;
	
}

static inline uint8_t get_extra_str_length(uint8_t id)
{
	if (id == UDI_VENDOR_STRING_ID)
	return sizeof(VENDOR_STRING)-1;
	if (id == UDI_CDC_COMM_STRING_ID_0)
	return sizeof(CDC_DATA_STRING_0)-1;
	//if (id == UDI_CDC_COMM_STRING_ID_1)
	//return sizeof(CDC_DATA_STRING_1)-1;
	return 0;
	
}

#define UDC_GET_EXTRA_STRING() (str_length = get_extra_str_length(udd_g_ctrlreq.req.wValue & 0xff), str = get_extra_str(udd_g_ctrlreq.req.wValue & 0xff))

#endif
/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
void main_sof_action(void);
void main_resume_action(void);
void main_suspend_action(void);
#define  UDC_VBUS_EVENT(b_vbus_high)
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

//@}

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the class module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Number of interfaces for this device
#define  USB_DEVICE_NB_INTERFACE       3 // 1 or more

//! Total endpoint used by all interfaces
//! Note:
//! It is possible to define an IN and OUT endpoints with the same number on XMEGA product only
//! E.g. MSC class can be have IN endpoint 0x81 and OUT endpoint 0x01
#define  USB_DEVICE_MAX_EP             7 // 0 to max endpoint requested by interfaces
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of CDC interface (if used)
 * @{
 */

//! Number of communication port used (1 to 3)
#define  UDI_CDC_PORT_NB 1

bool cdc_enable(uint8_t port);
void cdc_disable(uint8_t port);
//! Interface callback definition
#define  UDI_CDC_ENABLE_EXT(port)           cdc_enable(port)
#define  UDI_CDC_DISABLE_EXT(port)			cdc_disable(port)
//#define  UDI_CDC_RX_NOTIFY(port)
//#define  UDI_CDC_TX_EMPTY_NOTIFY(port)
#define  UDI_CDC_RX_NOTIFY(port) my_callback_rx_notify(port)
extern void my_callback_rx_notify(uint8_t port);
#define  UDI_CDC_TX_EMPTY_NOTIFY(port)
#include "usb_protocol_cdc.h"
extern void my_callback_config(uint8_t port, usb_cdc_line_coding_t * cfg);
#define  UDI_CDC_SET_CODING_EXT(port,cfg) my_callback_config(port,cfg)
//#define  UDI_CDC_SET_CODING_EXT(port,cfg)

#define  UDI_CDC_SET_DTR_EXT(port,set)
#define  UDI_CDC_SET_RTS_EXT(port,set)
/*
 * #define UDI_CDC_ENABLE_EXT(port) my_callback_cdc_enable()
 * extern bool my_callback_cdc_enable(void);
 * #define UDI_CDC_DISABLE_EXT(port) my_callback_cdc_disable()
 * extern void my_callback_cdc_disable(void);
 * #define  UDI_CDC_RX_NOTIFY(port) my_callback_rx_notify(port)
 * extern void my_callback_rx_notify(uint8_t port);
 * #define  UDI_CDC_TX_EMPTY_NOTIFY(port) my_callback_tx_empty_notify(port)
 * extern void my_callback_tx_empty_notify(uint8_t port);
 * #define  UDI_CDC_SET_CODING_EXT(port,cfg) my_callback_config(port,cfg)
 * extern void my_callback_config(uint8_t port, usb_cdc_line_coding_t * cfg);
 * #define  UDI_CDC_SET_DTR_EXT(port,set) my_callback_cdc_set_dtr(port,set)
 * extern void my_callback_cdc_set_dtr(uint8_t port, bool b_enable);
 * #define  UDI_CDC_SET_RTS_EXT(port,set) my_callback_cdc_set_rts(port,set)
 * extern void my_callback_cdc_set_rts(uint8_t port, bool b_enable);
 */

//! Define it when the transfer CDC Device to Host is a low rate (<512000 bauds)
//! to reduce CDC buffers size
#define  UDI_CDC_LOW_RATE

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8

/**
 * USB CDC low level configuration
 * In standalone these configurations are defined by the CDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoints' numbers used by single or first CDC port
#define  UDI_CDC_DATA_EP_IN_0          (6 | USB_EP_DIR_IN)  // TX
#define  UDI_CDC_DATA_EP_OUT_0         (7 | USB_EP_DIR_OUT) // RX
#define  UDI_CDC_COMM_EP_0             (3 | USB_EP_DIR_IN)  // Notify endpoint

//! Interface numbers used by single or first CDC port
#define  UDI_CDC_COMM_IFACE_NUMBER_0   1
#define  UDI_CDC_DATA_IFACE_NUMBER_0   2

/**
 * Configuration of vendor interface
 * @{
 */
//! Interface callback definition
bool main_vendor_enable(void);
void main_vendor_disable(void);
bool main_setup_out_received(void);
bool main_setup_in_received(void);
#define UDI_VENDOR_ENABLE_EXT()           main_vendor_enable()
#define UDI_VENDOR_DISABLE_EXT()          main_vendor_disable()
#define UDI_VENDOR_SETUP_OUT_RECEIVED()   main_setup_out_received()
#define UDI_VENDOR_SETUP_IN_RECEIVED()    main_setup_in_received()

//! endpoints size for full speed
//! Note: Disable the endpoints of a type, if size equal 0
#define UDI_VENDOR_EPS_SIZE_INT_FS    0 /*64*/
#define UDI_VENDOR_EPS_SIZE_BULK_FS   64
#if SAMG55
#define UDI_VENDOR_EPS_SIZE_ISO_FS   0
#else
#define UDI_VENDOR_EPS_SIZE_ISO_FS   0 /*256*/
#endif

//! endpoints size for high speed
#define UDI_VENDOR_EPS_SIZE_INT_HS    0 /*64*/
#define UDI_VENDOR_EPS_SIZE_BULK_HS  512
#define UDI_VENDOR_EPS_SIZE_ISO_HS    0 /*64*/

//! Endpoint numbers definition
#define  UDI_VENDOR_EP_BULK_IN       (0x01 | USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_BULK_OUT      (0x02 | USB_EP_DIR_OUT)

/**
 * \name UDD Configuration
 */
//@{
//! Maximum 6 endpoints used by vendor interface
#define UDI_VENDOR_EP_NB_INT  ((UDI_VENDOR_EPS_SIZE_INT_FS)?2:0)
#define UDI_VENDOR_EP_NB_BULK ((UDI_VENDOR_EPS_SIZE_BULK_FS)?2:0)
#define UDI_VENDOR_EP_NB_ISO  ((UDI_VENDOR_EPS_SIZE_ISO_FS)?2:0)

//! Interface number
#define  UDI_VENDOR_IFACE_NUMBER     0

#define UDI_COMPOSITE_DESC_T \
	udi_vendor_desc_t udi_vendor; \
union { \
struct { \
	usb_iad_desc_t udi_iad;\
	udi_cdc_comm_desc_t udi_cdc_comm; \
	udi_cdc_data_desc_t udi_cdc_data; \
};\
struct {\
	udi_vendor_desc_t udi_vendor_mpsse; \
};\
};

#define NAEUSB_MPSSE_SUPPORT 1
#define MPSSE_SWD_SUPPORT 1

#if NAEUSB_MPSSE_SUPPORT == 1
#define  UDI_MPSSE_EP_BULK_IN		 (0x06 | USB_EP_DIR_IN)
#define  UDI_MPSSE_EP_BULK_OUT		 (0x07 | USB_EP_DIR_OUT)
#endif

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
.udi_vendor = UDI_VENDOR_DESC_FS, \
.udi_iad = UDI_CDC_IAD_DESC_0, \
.udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
.udi_cdc_data              = UDI_CDC_DATA_DESC_0_FS, 

#define UDI_COMPOSITE_DESC_HS \
.udi_vendor = UDI_VENDOR_DESC_HS, \
.udi_iad = UDI_CDC_IAD_DESC_0, \
.udi_cdc_comm              = UDI_CDC_COMM_DESC_0, \
.udi_cdc_data              = UDI_CDC_DATA_DESC_0_HS,  

//! USB Interface APIs
#define UDI_COMPOSITE_API &udi_api_vendor, \
&udi_api_cdc_comm, \
&udi_api_cdc_data,
/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include "udi_vendor.h"
#include "udi_cdc.h"
/* Example of include for interface
#include "udi_msc.h"
#include "udi_hid_kbd.h"
#include "udi_hid_mouse.h"
#include "udi_cdc.h"
#include "udi_phdc.h"
#include "udi_vendor.h"
*/
/* Declaration of callbacks used by USB
#include "callback_def.h"
*/

#endif // _CONF_USB_H_
