/**
 * \file
 *
 * \brief Descriptors for an USB Composite Device
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

#include "conf_usb.h"
#include "udd.h"
#include "udc_desc.h"

/**
 * \defgroup udi_group_desc Descriptors for a USB Device
 * composite
 *
 * @{
 */

/**INDENT-OFF**/

//! USB Device Descriptor
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE usb_dev_desc_t udc_device_desc = {
	.bLength                   = sizeof(usb_dev_desc_t),
	.bDescriptorType           = USB_DT_DEVICE,
	.bcdUSB                    = LE16(USB_V2_0),
	.bDeviceClass              = 0,
	.bDeviceSubClass           = 0,
	.bDeviceProtocol           = 0,
	.bMaxPacketSize0           = USB_DEVICE_EP_CTRL_SIZE,
	.idVendor                  = LE16(USB_DEVICE_VENDOR_ID),
	.idProduct                 = LE16(USB_DEVICE_PRODUCT_ID),
	.bcdDevice                 = LE16((USB_DEVICE_MAJOR_VERSION << 8)
		| USB_DEVICE_MINOR_VERSION),
#ifdef USB_DEVICE_MANUFACTURE_NAME
	.iManufacturer             = 1,
#else
	.iManufacturer             = 0,  // No manufacture string
#endif
#ifdef USB_DEVICE_PRODUCT_NAME
	.iProduct                  = 2,
#else
	.iProduct                  = 0,  // No product string
#endif
#if (defined USB_DEVICE_SERIAL_NAME || defined USB_DEVICE_GET_SERIAL_NAME_POINTER)
	.iSerialNumber             = 3,
#else
	.iSerialNumber             = 0,  // No serial string
#endif
	.bNumConfigurations        = 1
};


#ifdef USB_DEVICE_HS_SUPPORT
//! USB Device Qualifier Descriptor for HS
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE usb_dev_qual_desc_t udc_device_qual = {
	.bLength                   = sizeof(usb_dev_qual_desc_t),
	.bDescriptorType           = USB_DT_DEVICE_QUALIFIER,
	.bcdUSB                    = LE16(USB_V2_0),
	.bDeviceClass              = 0,
	.bDeviceSubClass           = 0,
	.bDeviceProtocol           = 0,
	.bMaxPacketSize0           = USB_DEVICE_EP_CTRL_SIZE,
	.bNumConfigurations        = 1
};
#endif

//! Structure for USB Device Configuration Descriptor
COMPILER_PACK_SET(1)
typedef struct {
	usb_conf_desc_t conf;
	UDI_COMPOSITE_DESC_T;
} udc_desc_t;
COMPILER_PACK_RESET()

#if NAEUSB_MPSSE_SUPPORT == 1
udi_api_t *mpsse_udi_apis[2] = {
	&udi_api_vendor,
	&udi_api_vendor
};

// #define  UDI_CDC_DATA_EP_IN_0          (5 | USB_EP_DIR_IN)  // TX
// #define  UDI_CDC_DATA_EP_OUT_0         (6 | USB_EP_DIR_OUT) // RX

#ifdef USB_DEVICE_HS_SUPPORT
#define MPSSE_DESC \
	.iface0.bLength            = sizeof(usb_iface_desc_t),\
	.iface0.bDescriptorType    = USB_DT_INTERFACE,\
	.iface0.bInterfaceNumber   = UDI_CDC_COMM_IFACE_NUMBER_0,\
	.iface0.bAlternateSetting  = 0 /*1*/,\
	.iface0.bNumEndpoints      = UDI_VENDOR_EP_NB,\
	.iface0.bInterfaceClass    = VENDOR_CLASS,\
	.iface0.bInterfaceSubClass = VENDOR_SUBCLASS,\
	.iface0.bInterfaceProtocol = VENDOR_PROTOCOL,\
	.iface0.iInterface         = UDI_VENDOR_STRING_ID,\
	.ep_bulk_in.bLength                = sizeof(usb_ep_desc_t),\
	.ep_bulk_in.bDescriptorType        = USB_DT_ENDPOINT,\
	.ep_bulk_in.bEndpointAddress       = UDI_MPSSE_EP_BULK_IN,\
	.ep_bulk_in.bmAttributes           = USB_EP_TYPE_BULK,\
	.ep_bulk_in.bInterval              = 0,\
	.ep_bulk_out.bLength               = sizeof(usb_ep_desc_t),\
	.ep_bulk_out.bDescriptorType       = USB_DT_ENDPOINT,\
	.ep_bulk_out.bEndpointAddress      = UDI_MPSSE_EP_BULK_OUT,\
	.ep_bulk_out.bmAttributes          = USB_EP_TYPE_BULK,\
	.ep_bulk_out.bInterval             = 0,\
	.ep_bulk_in.wMaxPacketSize         = LE16(UDI_VENDOR_EPS_SIZE_BULK_FS),\
	.ep_bulk_out.wMaxPacketSize        = LE16(UDI_VENDOR_EPS_SIZE_BULK_FS),\
	.ep_bulk_in.wMaxPacketSize         = LE16(UDI_VENDOR_EPS_SIZE_BULK_HS),\
	.ep_bulk_out.wMaxPacketSize        = LE16(UDI_VENDOR_EPS_SIZE_BULK_HS),
#else
#define MPSSE_DESC \
	.iface0.bLength            = sizeof(usb_iface_desc_t),\
	.iface0.bDescriptorType    = USB_DT_INTERFACE,\
	.iface0.bInterfaceNumber   = UDI_CDC_COMM_IFACE_NUMBER_0,\
	.iface0.bAlternateSetting  = 0 /*1*/,\
	.iface0.bNumEndpoints      = UDI_VENDOR_EP_NB,\
	.iface0.bInterfaceClass    = VENDOR_CLASS,\
	.iface0.bInterfaceSubClass = VENDOR_SUBCLASS,\
	.iface0.bInterfaceProtocol = VENDOR_PROTOCOL,\
	.iface0.iInterface         = UDI_VENDOR_STRING_ID,\
	.ep_bulk_in.bLength                = sizeof(usb_ep_desc_t),\
	.ep_bulk_in.bDescriptorType        = USB_DT_ENDPOINT,\
	.ep_bulk_in.bEndpointAddress       = UDI_MPSSE_EP_BULK_IN,\
	.ep_bulk_in.bmAttributes           = USB_EP_TYPE_BULK,\
	.ep_bulk_in.bInterval              = 0,\
	.ep_bulk_out.bLength               = sizeof(usb_ep_desc_t),\
	.ep_bulk_out.bDescriptorType       = USB_DT_ENDPOINT,\
	.ep_bulk_out.bEndpointAddress      = UDI_MPSSE_EP_BULK_OUT,\
	.ep_bulk_out.bmAttributes          = USB_EP_TYPE_BULK,\
	.ep_bulk_out.bInterval             = 0,\
	.ep_bulk_in.wMaxPacketSize         = LE16(UDI_VENDOR_EPS_SIZE_BULK_FS),\
	.ep_bulk_out.wMaxPacketSize        = LE16(UDI_VENDOR_EPS_SIZE_BULK_FS),
#endif
	

COMPILER_WORD_ALIGNED
udc_desc_t udc_desc_fs_mpsse = {
	.conf.bLength              = sizeof(usb_conf_desc_t),
	.conf.bDescriptorType      = USB_DT_CONFIGURATION,
	.conf.wTotalLength         = LE16(9+sizeof(udi_vendor_desc_t)+sizeof(udi_vendor_desc_t)),
	.conf.bNumInterfaces       = 2,
	.conf.bConfigurationValue  = 1,
	.conf.iConfiguration       = 0,
	.conf.bmAttributes         = USB_CONFIG_ATTR_MUST_SET | USB_DEVICE_ATTR,
	.conf.bMaxPower            = USB_CONFIG_MAX_POWER(USB_DEVICE_POWER),
	.udi_vendor				   = UDI_VENDOR_DESC_FS,
	.udi_vendor_mpsse		   = {MPSSE_DESC}
};

COMPILER_WORD_ALIGNED
udc_desc_t udc_desc_hs_mpsse = {
	.conf.bLength              = sizeof(usb_conf_desc_t),
	.conf.bDescriptorType      = USB_DT_CONFIGURATION,
	.conf.wTotalLength         = LE16(9+sizeof(udi_vendor_desc_t)+sizeof(udi_vendor_desc_t)),
	.conf.bNumInterfaces       = 2,
	.conf.bConfigurationValue  = 1,
	.conf.iConfiguration       = 0,
	.conf.bmAttributes         = USB_CONFIG_ATTR_MUST_SET | USB_DEVICE_ATTR,
	.conf.bMaxPower            = USB_CONFIG_MAX_POWER(USB_DEVICE_POWER),
	.udi_vendor 			   = UDI_VENDOR_DESC_HS,
	.udi_vendor_mpsse		   = {MPSSE_DESC}
};

#define USB_EXTRA_FS_DESC ,{\
	.desc		= (usb_conf_desc_t *)&udc_desc_fs_mpsse,\
	.udi_apis	= mpsse_udi_apis,\
}

#define USB_EXTRA_HS_DESC ,{\
	.desc		= (usb_conf_desc_t *)&udc_desc_hs_mpsse,\
	.udi_apis	= mpsse_udi_apis,\
}
#endif

//! USB Device Configuration Descriptor filled for FS
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udc_desc_t udc_desc_fs = {
	.conf.bLength              = sizeof(usb_conf_desc_t),
	.conf.bDescriptorType      = USB_DT_CONFIGURATION,
	.conf.wTotalLength         = LE16(sizeof(udc_desc_t)),
	.conf.bNumInterfaces       = USB_DEVICE_NB_INTERFACE,
	.conf.bConfigurationValue  = 1,
	.conf.iConfiguration       = 0,
	.conf.bmAttributes         = USB_CONFIG_ATTR_MUST_SET | USB_DEVICE_ATTR,
	.conf.bMaxPower            = USB_CONFIG_MAX_POWER(USB_DEVICE_POWER),
	UDI_COMPOSITE_DESC_FS
};

#ifdef USB_DEVICE_HS_SUPPORT
//! USB Device Configuration Descriptor filled for HS
COMPILER_WORD_ALIGNED
UDC_DESC_STORAGE udc_desc_t udc_desc_hs = {
	.conf.bLength              = sizeof(usb_conf_desc_t),
	.conf.bDescriptorType      = USB_DT_CONFIGURATION,
	.conf.wTotalLength         = LE16(sizeof(udc_desc_t)),
	.conf.bNumInterfaces       = USB_DEVICE_NB_INTERFACE,
	.conf.bConfigurationValue  = 1,
	.conf.iConfiguration       = 0,
	.conf.bmAttributes         = USB_CONFIG_ATTR_MUST_SET | USB_DEVICE_ATTR,
	.conf.bMaxPower            = USB_CONFIG_MAX_POWER(USB_DEVICE_POWER),
	UDI_COMPOSITE_DESC_HS
};
#endif


/**
 * \name UDC structures which contains all USB Device definitions
 */
//@{

//! Associate an UDI for each USB interface
UDC_DESC_STORAGE udi_api_t *udi_apis[USB_DEVICE_NB_INTERFACE] = {
	UDI_COMPOSITE_API
};

//! Add UDI with USB Descriptors FS
UDC_DESC_STORAGE udc_config_speed_t   udc_config_lsfs[1] = {{
	.desc          = (usb_conf_desc_t UDC_DESC_STORAGE*)&udc_desc_fs,
	.udi_apis      = udi_apis,
}
};

#ifdef USB_DEVICE_HS_SUPPORT
//! Add UDI with USB Descriptors HS
UDC_DESC_STORAGE udc_config_speed_t   udc_config_hs[1] = {{
	.desc          = (usb_conf_desc_t UDC_DESC_STORAGE*)&udc_desc_hs,
	.udi_apis      = udi_apis,
}
};
#endif

//! Add all information about USB Device in global structure for UDC
UDC_DESC_STORAGE udc_config_t udc_config = {
	.confdev_lsfs = &udc_device_desc,
	.conf_lsfs = udc_config_lsfs,
#ifdef USB_DEVICE_HS_SUPPORT
	.confdev_hs = &udc_device_desc,
	.qualifier = &udc_device_qual,
	.conf_hs = udc_config_hs,
#endif
};

// hack to get around windows being unable to handle multiple configurations
#if NAEUSB_MPSSE_SUPPORT == 1
void switch_configurations()
{
	udc_config.conf_lsfs->desc = (usb_conf_desc_t *)&udc_desc_fs_mpsse;
	udc_config.conf_lsfs->udi_apis = mpsse_udi_apis;
	#ifdef USB_DEVICE_HS_SUPPORT
	udc_config.conf_hs->desc = (usb_conf_desc_t *)&udc_desc_hs_mpsse;
	udc_config.conf_hs->udi_apis = mpsse_udi_apis;
	#endif
}
#endif

//@}
/**INDENT-ON**/
//@}
