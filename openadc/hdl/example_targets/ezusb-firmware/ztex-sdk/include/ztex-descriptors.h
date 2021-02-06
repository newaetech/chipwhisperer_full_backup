/*!
   ZTEX Firmware Kit for EZ-USB FX2 Microcontrollers
   Copyright (C) 2009-2011 ZTEX GmbH.
   http://www.ztex.de

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 3 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see http://www.gnu.org/licenses/.
!*/

/* 
   Defines the USB descriptors
*/

#ifndef[ZTEX_DESCRIPTORS_H]
#define[ZTEX_DESCRIPTORS_H]

#define[ZTEX_DESCRIPTOR_OFFS][0x06c]
#define[ZTEX_DESCRIPTOR_LEN][40]

__xdata __at ZTEX_DESCRIPTOR_OFFS BYTE ZTEX_DESCRIPTOR;

/* ZTEX descriptor version. Must be 1. */
__xdata __at ZTEX_DESCRIPTOR_OFFS+1 BYTE ZTEX_DESCRIPTOR_VERSION;

/* Must not be modified, ID="ZTEX" */
__xdata __at ZTEX_DESCRIPTOR_OFFS+2 BYTE ZTEXID[4];  

/* 
   Product ID and firmware compatibility information. 
   
   A firmware can overwrite an installed one if
   if ( INSTALLED.PRODUCTID[0]==0 || PRODUCTID[0]==0 || INSTALLED.PRODUCTID[0]==PRODUCTID[0] ) &&
      ( INSTALLED.PRODUCTID[1]==0 || PRODUCTID[1]==0 || INSTALLED.PRODUCTID[1]==PRODUCTID[1] ) &&
      ( INSTALLED.PRODUCTID[2]==0 || PRODUCTID[2]==0 || INSTALLED.PRODUCTID[2]==PRODUCTID[2] ) &&
      ( INSTALLED.PRODUCTID[3]==0 || PRODUCTID[3]==0 || INSTALLED.PRODUCTID[3]==PRODUCTID[3] ) 

   Reserved Product ID's:
   
   0.0.0.0		// default Product ID (no product specified)
   1.*.*.*   		// may be used for experimental purposes
   10.*.*.*		// used for ZTEX products
   10.11.*.*		// ZTEX USB-FPGA-Module 1.2
   10.12.*.*		// ZTEX USB-FPGA-Module 1.11
   10.13.*.*		// ZTEX USB-FPGA-Module 1.15 (not 1.15y)
   10.14.*.*		// ZTEX USB-FPGA-Module 1.15x
   10.15.*.*		// ZTEX USB-FPGA-Module 1.15y
   10.20.*.*		// ZTEX USB-Module 1.0
   10.30.*.*		// ZTEX USB-XMEGA-Module 1.0
   10.0.1.1		// ZTEX bitminer firmware
   
   Please contact me (http://www.ztex.de --> Impressum/Kontakt) if you want to register/reserve a Product ID (range).
*/
__xdata __at ZTEX_DESCRIPTOR_OFFS+6 BYTE PRODUCT_ID[4];  

/* Firmware version, may be used to distinguish seveveral firmware versions */
__xdata __at ZTEX_DESCRIPTOR_OFFS+10 BYTE FW_VERSION;  

/* Interface version. Must be 1. */
__xdata __at ZTEX_DESCRIPTOR_OFFS+11 BYTE INTERFACE_VERSION;

/* 
    Standard interface capabilities:
	0.0  : EEPROM read/write, see ztex-eeprom.h
	0.1  : FPGA configuration, see ztex-fpga.h
	0.2  : Flash memory support, see ztex-flash1.h
	0.3  : Debug helper, see ztex-debug.h
	0.4  : AVR XMEGA support, see ztex-xmega.h
	0.5  : High speed FPGA configuration support
	0.6  : MAC EEPROM support
	0.7  : Multi-FPGA support
	1.0  : Temperature sensor support 
*/
__xdata __at ZTEX_DESCRIPTOR_OFFS+12 BYTE INTERFACE_CAPABILITIES[6];

/* Space for settings which depends on PRODUCT_ID, e.g extra capabilities */
__xdata __at ZTEX_DESCRIPTOR_OFFS+18 BYTE MODULE_RESERVED[12];

/* 
   Serial number string 
   default: "0000000000"
   Should only be modified by the the firmware upload software 
*/
__xdata __at ZTEX_DESCRIPTOR_OFFS+30 BYTE SN_STRING[10];

/* Are Vendor ID and Product ID defined? */
#ifndef[USB_VENDOR_ID]
#error[No USB Vendor ID defined]
#endif

#ifndef[USB_PRODUCT_ID]
#error[No USB Product ID defined]
#endif

/* Prepare the Interfaces, i.e. check which interfaces are defined */
#define[CONFIGURE_INTERFACE(][);][
#define[CONFIG_INTERFACE$0]
#ifeq[EP1IN_INTERFACE][$0]
#elifeq[EP1OUT_INTERFACE][$0]
#elifeq[EP2_INTERFACE][$0]
#elifeq[EP4_INTERFACE][$0]
#elifeq[EP6_INTERFACE][$0]
#elifneq[EP8_INTERFACE][$0]
#udefine[CONFIG_INTERFACE$0]
#endif
#ifdef[CONFIG_INTERFACE$0]
//Interface $0: YES
#else
//Interface $0: NO
#endif]

CONFIGURE_INTERFACE(0);
CONFIGURE_INTERFACE(1);
CONFIGURE_INTERFACE(2);
CONFIGURE_INTERFACE(3);

/* define the ZTEX descriptor */
void abscode_identity()// _naked
{
    __asm	
    .area ABSCODE (ABS,CODE)

    .org ZTEX_DESCRIPTOR_OFFS
    .db ZTEX_DESCRIPTOR_LEN

    .org _ZTEX_DESCRIPTOR_VERSION
    .db 1

    .org _ZTEXID
    .ascii "ZTEX"

    .org _PRODUCT_ID
    .db PRODUCT_ID_0
    .db PRODUCT_ID_1
    .db PRODUCT_ID_2
    .db PRODUCT_ID_3

    .org _FW_VERSION
    .db FWVER

    .org _INTERFACE_VERSION
    .db 1

    .org _INTERFACE_CAPABILITIES
    .db 0
#ifdef[@CAPABILITY_EEPROM;]
#nolf
 + 1
#endif
#ifdef[@CAPABILITY_FPGA;]
#nolf
 + 2
#endif
#ifdef[@CAPABILITY_FLASH;]
#nolf
 + 4
#endif
#ifdef[@CAPABILITY_DEBUG;]
#nolf
 + 8
#endif
#ifdef[@CAPABILITY_XMEGA;]
#nolf
 + 16
#endif
#ifdef[@CAPABILITY_HS_FPGA;]
#nolf
 + 32
#endif
#ifdef[@CAPABILITY_MAC_EEPROM;]
#nolf
 + 64
#endif
#ifdef[@CAPABILITY_MULTI_FPGA;]
#nolf
 + 128
#endif
    .db 0
    .db 0
    .db 0
    .db 0
    .db 0

    .org _MODULE_RESERVED
    .db MODULE_RESERVED_00
    .db MODULE_RESERVED_01
    .db MODULE_RESERVED_02
    .db MODULE_RESERVED_03
    .db MODULE_RESERVED_04
    .db MODULE_RESERVED_05
    .db MODULE_RESERVED_06
    .db MODULE_RESERVED_07
    .db MODULE_RESERVED_08
    .db MODULE_RESERVED_09
    .db MODULE_RESERVED_10
    .db MODULE_RESERVED_11

    .org _SN_STRING
    .ascii "0000000000"

    .area CSEG    (CODE)
    __endasm;
}    

/* *********************************************************************
   ***** strings *******************************************************
   ********************************************************************* */
__code char manufacturerString[] = MANUFACTURER_STRING;
__code char productString[] = PRODUCT_STRING;
__code char configurationString[] = CONFIGURATION_STRING;


/* *********************************************************************
   ***** descriptors ***************************************************
   ********************************************************************* */
#define[EP_DESCRIPTOR(][);][
				// Endpoint $0 descriptor
	,7 			// 0, Descriptor length
	,5			// 1, Descriptor type
#ifeq[$0][1IN]
	,0x81			// 2, direction=output, address=1
#elifeq[$0][1OUT]
	,0x01			// 2, direction=output, address=1
#elifeq[EP$0_DIR][IN]		
	,0x80+$0			// 2, direction=output, address=$0
#elifeq[EP$0_DIR][OUT]
	,$0			// 2, direction=input, address=$0
#else
#error[Invalid direction for endpoint $0 (`IN' or `ÒUT' expected)]
#endif
#ifeq[EP$0_TYPE][ISO]
	,1			// 3, ISO transferns
#elifeq[EP$0_TYPE][BULK]
	,2			// 3, BULK transferns
#elifeq[EP$0_TYPE][INT]
	,3			// 3, INT transferns
#else
#error[Invalid type for endpoint $0: `EP$0_TYPE' (`ISO', 'BULK' or `INT' expected)]
#endif 
#ifdef[HIGH_SPEED]
	,EP$0_SIZE & 0xff 	// 4, max. packet size (L) 
	,EP$0_SIZE >> 8 	// 5, max. packet size (H) 
#ifneq[EP$0_TYPE][BULK]
	    | ( (EP$0_PPMF-1) << 3 )
#endif
#else
	,64			// 4, max. packet size (L) 
	,0 			// 5, max. packet size (H) 
#endif	
#ifeq[EP$0_TYPE][BULK]
	,0 			// 6, Polling interval
#elifeq[EP$0_TYPE][ISO]
	,1 			// 6, Polling interval (1ms if Full Speed mode, 125µs in High Speed Mode)
#else
	,EP$0_POLL 		// 6, Polling interval (1ms if Full Speed mode, 125µs in High Speed Mode)
#endif	
]
   
#define[INTERFACE_DESCRIPTOR(][);][
		// Interface $0 descriptor
	,9	// 0, Descriptor length
	,0x04	// 1, Descriptor type
	,0	// 2, Zero-based index of this interface
#ifneq[$0][0]
#ifdef[CONFIG_INTERFACE0]
	  +1
#endif
#ifneq[$0][1]
#ifdef[CONFIG_INTERFACE1]
	  +1
#endif
#ifneq[$0][2]
#ifdef[CONFIG_INTERFACE2]
	  +1
#endif
#endif
#endif
#endif
	,0	// 3, Alternate setting	0
	,0	// 4, Number of end points 
#ifeq[EP1IN_INTERFACE][$0]
	  +1
#endif	  
#ifeq[EP1OUT_INTERFACE][$0]
	  +1
#endif	  
#ifeq[EP2_INTERFACE][$0]
	  +1
#endif	  
#ifeq[EP4_INTERFACE][$0]
	  +1
#endif	  
#ifeq[EP6_INTERFACE][$0]
	  +1
#endif	  
#ifeq[EP8_INTERFACE][$0]
	  +1
#endif	  
	,0xff	// 5, Interface class
	,0xff	// 6, Interface sub class
	,0xff   // 7, Interface protocol
	,0 	// 8, Index of interface string descriptor
#ifeq[EP1IN_INTERFACE][$0]
	 EP_DESCRIPTOR(1IN);
#endif	  
#ifeq[EP1OUT_INTERFACE][$0]
	 EP_DESCRIPTOR(1OUT);
#endif	  
#ifeq[EP2_INTERFACE][$0]
	 EP_DESCRIPTOR(2);
#endif	  
#ifeq[EP4_INTERFACE][$0]
	 EP_DESCRIPTOR(4);
#endif	  
#ifeq[EP6_INTERFACE][$0]
	 EP_DESCRIPTOR(6);
#endif	  
#ifeq[EP8_INTERFACE][$0]
	 EP_DESCRIPTOR(8);
#endif
]

#define[APPEND_PADBYTE(][);][__code BYTE $0_PadByte[2-(sizeof($0) & 1)] = { 0 };]

#ifdef[PAD_BYTE]		// to ensure word alignment of the descriptors; PAD_BYTE is defined automatically by bmpsdcc script
__code BYTE PadByte = 0;
#endif

__code BYTE DeviceDescriptor[] = 
    {
	18, 	// 0, Descriptor length
	0x01,	// 1, Descriptor type
	0x00,	// 2, Specification Version (L)
	0x02,	// 3, Specification Version (H)
	0xff,	// 4, Device class
	0xff,	// 5, Device sub-class
	0xff,	// 6, Device protocol
	64,	// 7, Maximum packet size for EP0
	(USB_VENDOR_ID) & 255,	// 8, VENDOR_ID (L)
	(USB_VENDOR_ID) >> 8,	// 9, VENDOR_ID (H)
	(USB_PRODUCT_ID) & 255,	// 10, PRODUCT_ID (L)
	(USB_PRODUCT_ID) >> 8,	// 11, PRODUCT_ID (H)
	0x00,   // 12, device release number (BCD, L)
	0x00,   // 13, device release number (BCD, H)
	1,	// 14, Manufacturer string index
	2,	// 15, Product string index
	3,	// 16, Serial number string index
	1	// 17, Number of configurations
    };
    
__code BYTE DeviceQualifierDescriptor[] =
    {
	10, 	// 0, Descriptor length
	0x06,	// 1, Decriptor type
	0x00,	// 2, Specification Version (L)
	0x02,	// 3, Specification Version (H)
	0xff,	// 4, Device class
	0xff,	// 5, Device sub-class
	0xff,	// 6, Device protocol
	64,	// 7, Maximum packet size (EP0?)
	1,	// 8, Number of configurations
	0,	// 9, Reserved, must be zero
    };

__code BYTE HighSpeedConfigDescriptor[] = 
    {   
#define[HIGH_SPEED]
	9	// 0, Descriptor length
	,0x02	// 1, Decriptor type
	,sizeof(HighSpeedConfigDescriptor) & 0xff	// 2, Total length (LSB)
//	,sizeof(HighSpeedConfigDescriptor) >> 8		// 3, Total length (MB)
	,0						// 3, To avoid warnings, descriptor length will never exceed 255 bytes
	,0	// 4, Number of Interfaces
#ifdef[CONFIG_INTERFACE0]
	  +1
#endif
#ifdef[CONFIG_INTERFACE1]
	  +1
#endif
#ifdef[CONFIG_INTERFACE2]
	  +1
#endif
#ifdef[CONFIG_INTERFACE3]
	  +1
#endif
	
	,1	// 5, Configuration number
	,4	// 6, Configuration string
	,0xc0	// 7, Attributes: bus and self powered
	,50	// Maximum bus power 100 mA
#ifdef[CONFIG_INTERFACE0]
	INTERFACE_DESCRIPTOR(0);
#endif
#ifdef[CONFIG_INTERFACE1]
	INTERFACE_DESCRIPTOR(1);
#endif
#ifdef[CONFIG_INTERFACE2]
	INTERFACE_DESCRIPTOR(2);
#endif
#ifdef[CONFIG_INTERFACE3]
	INTERFACE_DESCRIPTOR(3);
#endif
#udefine[HIGH_SPEED]
    };
APPEND_PADBYTE(HighSpeedConfigDescriptor);

__code BYTE FullSpeedConfigDescriptor[] = 
    {   
	9 	// 0, Descriptor length
	,0x02	// 1, Decriptor type
	,sizeof(FullSpeedConfigDescriptor) & 0xff	// 2, Total length (LSB)
//	,sizeof(FullSpeedConfigDescriptor) >> 8		// 3, Total length (MSB)
	,0						// 3, To avoid warnings, descriptor length will never exceed 255 bytes
	,0	// 4, Number of Interfaces
#ifdef[CONFIG_INTERFACE0]
	  +1
#endif
#ifdef[CONFIG_INTERFACE1]
	  +1
#endif
#ifdef[CONFIG_INTERFACE2]
	  +1
#endif
#ifdef[CONFIG_INTERFACE3]
	  +1
#endif
	
	,1	// 5, Configuration number
	,4	// 6, Configuration string
	,0xc0	// 7, Attributes: bus and self powered
	,50	// Maximum bus power 100 mA
#ifdef[CONFIG_INTERFACE0]
	INTERFACE_DESCRIPTOR(0);
#endif
#ifdef[CONFIG_INTERFACE1]
	INTERFACE_DESCRIPTOR(1);
#endif
#ifdef[CONFIG_INTERFACE2]
	INTERFACE_DESCRIPTOR(2);
#endif
#ifdef[CONFIG_INTERFACE3]
	INTERFACE_DESCRIPTOR(3);
#endif
    };
APPEND_PADBYTE(FullSpeedConfigDescriptor);

__code BYTE EmptyStringDescriptor[] = 
    {
	sizeof(EmptyStringDescriptor),  	// Length
	0x03,					// Descriptor type
	0, 0
    };

#endif  /*ZTEX_DESCRIPTORS_H*/
