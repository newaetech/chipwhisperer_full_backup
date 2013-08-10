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
   Configuration macros 
*/

#ifndef[ZTEX_CONF_H]
#define[ZTEX_CONF_H]

/* 
   Don't expand macros in comments
*/
#disablelineinfo
#define[//][
][#noexpand[!dnapxeon!]//$0!dnapxeon!
]
#disablelineinfo
#define[/*][*/][#noexpand[!dnapxeon!]/*$0*/!dnapxeon!]

/* 
   This macro defines the USB Vendor ID and USB Product ID  (not the product ID
   from the ZTEX descriptor). The Vendor ID must be purchased from the USB-IF
   (http://www.usb.org). 
   
   The default vendor ID is the ZTEX vendor ID 0x221A, default product ID is
   0x100 which is assigned to ZTEX modules. These ID's can be shared by many
   different products which are identified by the product ID of the ZTEX
   descriptor. According to the USB-IF rules these ID's must not be used by
   hardware which is not manufactured by ZTEX. (Of course, this ID's can be 
   used during the development process or for internal purposes.)
   
   Please read the http://www.ztex.de/firmware-kit/usb_ids.e.html for more 
   informations about this topic.   
   
   Usage:
	SET_VPID(<Vendor ID>,<Product ID>);
*/
#define[SET_VPID(][,$1);][#define[USB_VENDOR_ID][$0]
#define[USB_PRODUCT_ID][$1]]
SET_VPID(0x221a,0x100);

/* 
   This macro is called before FPGA Firmware is reset, e.g. to save some
   settings. After this macro is called the I/O ports are set to default
   states in order to avoid damage during / after the FPGA configuration.
   To append something to this macro use the following definition:
#define[PRE_FPGA_RESET][PRE_FPGA_RESET
...]
*/
#define[PRE_FPGA_RESET][]


/* 
   This macro is called after FPGA Firmware has been configured. This is
   usually used to configure the I/O ports.
   To append something to this macro use the following definition:
#define[POST_FW_LOAD][POST_FW_LOAD
...]
*/
#define[POST_FPGA_CONFIG][]

/* 
   On multi FPGA boards this macro is called betwen deselection and
   selection of a FPGA. This can be used to store / resore I/O contents.
   To append something to this macro use the following definition:
#define[PRE_FPGA_SELECT][PRE_FPGA_SELECT
...]
*/
#define[PRE_FPGA_SELECT][]


/* 
  Add a vedor request for endpoint 0, 

   Usage:
     ADD_EP0_VENDOR_REQUEST((<request number>,,<code executed after setup package received>,,<code executed after data package received>''));
   Example:
     ADD_EP0_VENDOR_REQUEST((0x33,,initHSFPGAConfiguration();,,));;
...]
*/
#define[EP0_VENDOR_REQUESTS_SU;][]
#define[EP0_VENDOR_REQUESTS_DAT;][]
#define[ADD_EP0_VENDOR_REQUEST((][,,$1,,$2));;][#define[EP0_VENDOR_REQUESTS_SU;][EP0_VENDOR_REQUESTS_SU;
case $0:
    $1
    break;
]
#define[EP0_VENDOR_REQUESTS_DAT;][EP0_VENDOR_REQUESTS_DAT;
case $0:
    $2
    break;
]]


/* 
   Add a vedor command for endpoint 0, 

   Usage:
     ADD_EP0_VENDOR_COMMAND((<request number>,,<code executed after setup package received>,,<code executed after data package received>''));
   Example:
     ADD_EP0_VENDOR_COMMAND((0x33,,initHSFPGAConfiguration();,,));;
...]
*/
#define[EP0_VENDOR_COMMANDS_SU;][]
#define[EP0_VENDOR_COMMANDS_DAT;][]
#define[ADD_EP0_VENDOR_COMMAND((][,,$1,,$2));;][#define[EP0_VENDOR_COMMANDS_SU;][EP0_VENDOR_COMMANDS_SU;
case $0:			
    $1
    break;
]
#define[EP0_VENDOR_COMMANDS_DAT;][EP0_VENDOR_COMMANDS_DAT;
case $0:			
    $2
    break;
]]

/* 
  This macro generates a EP0 stall and aborts the current loop. Stalls are usually used to indicate errors.
*/
#define[EP0_STALL;][{
    EP0CS |= 0x01;	// set stall
    ep0_payload_remaining = 0;
    break;
}]


/* 
   Endoint 1,2,4,5,8 configuration:

   EP_CONFIG(<EP number>,<interface>,<type>,<direction>,<size>,<buffers>)
        <EP number> = 1IN | 1OUT | 2 | 4 | 6 | 8	Endpoint number
        <INTERFACE> = 0 | 1 | 2 | 3			To which interface this endpoint belongs
	<type>      = BULK  | ISO | INT
	<dir>       = IN | OUT
	<size>      = 512 | 1024
	<buffers>   = 1 | 2 | 3 | 4
   Example: EP_CONFIG(2,0,ISO,OUT,1024,4);
   Important note: No spaces next to the commas


   Endpoint 1 configuration:
   
   These Endpoints are defined by default as bulk endpoints and are assigned to interface 0.
   Endpoint size is always 64 bytes, but reported Endpoint size will be 512 bytes for USB 2.0 compliance. 
   
   These Endpoints can be redefined using EP_CONFIG or using:
   
   EP1IN_CONFIG(<interface>);
           <INTERFACE> = 0 | 1 | 2 | 3		Interface to which EP1IN belongs; default: 0
   EP1OUT_CONFIG(<interface>);
           <INTERFACE> = 0 | 1 | 2 | 3		Interface to which EP1OUT belongs; default: 0
   EP1_CONFIG(<interface>);
           <INTERFACE> = 0 | 1 | 2 | 3		Interface to which EP1IN and EP1OUT belongs; default: 0

   The following (maximum) configurations are possible:
   EP2		EP4	EP6	EP8
   2x512	2x512	2x512	2x512
   2x512	2x512	4x512	
   2x512	2x512	2x1024
   4x512		2x512	2x512
   4x512		4x512	
   4x512		2x1024
   2x1024		2x512	2x512
   2x1024		4x512	
   2x1024		2x1024
   3x512		3x512	2x512
   3x1024			2x512
   4x1024		
*/
#define[EP_CONFIG(][,$1,$2,$3,$4,$5);][
#ifeq[$0][1IN]
#elifeq[$0][1OUT]
#elifeq[$0][2]
#elifeq[$0][4]
#elifeq[$0][6]
#elifneq[$0][8]
#error[EP_CONFIG: Invalid 1st parameter: `$0'. Expected `2', `4', `6' or '8']
#endif
#ifeq[$1][0]
#elifeq[$1][1]
#elifeq[$1][2]
#elifneq[$1][3]
#error[EP_CONFIG: Invalid 2nd parameter: `$1'. Expected `0', `1', `2' or '3']
#endif
#ifeq[$2][BULK]
#elifeq[$2][ISO]
#elifneq[$2][INT]
#error[EP_CONFIG: Invalid 3nd parameter: `$2'. Expected `BULK', `ISO' or 'INT']
#endif
#ifeq[$3][IN]
#elifneq[$3][OUT]
#error[EP_CONFIG: Invalid 4th parameter: `$3'. Expected `IN' or 'OUT']
#endif
#ifeq[$4][512]
#elifneq[$4][1024]
#error[EP_CONFIG: Invalid 5th parameter: `$4'. Expected `512' or '1024']
#endif
#ifeq[$5][1]
#elifeq[$5][2]
#elifeq[$5][3]
#elifneq[$5][4]
#error[EP_CONFIG: Invalid 6th parameter: `$5'. Expected `1', `2', `3' or `4']
#endif
#define[EP$0_INTERFACE][$1]
#define[EP$0_TYPE][$2]
#define[EP$0_DIR][$3]
#define[EP$0_SIZE][$4]
#define[EP$0_BUFFERS][$5]]

#define[EP1IN_CONFIG(][);][#define[EP1IN_INTERFACE][$0]]
#define[EP1OUT_CONFIG(][);][#define[EP1OUT_INTERFACE][$0]]
#define[EP1_CONFIG(][);][#define[EP1IN_INTERFACE][$0]
#define[EP1OUT_INTERFACE][$0]]

EP_CONFIG(1IN,0,BULK,IN,512,1);
EP_CONFIG(1OUT,0,BULK,OUT,512,1);

/* 
   ISO and INT Transactions per microframe:

   Default value is 1 for all endpoints.

   EP_PPMF(<EP number>,<transactions per microframe>)
        <EP number>                  = 1IN | 1OUT | 2 | 4 | 6 | 8	Endpoint
        <transactions per microframe> = 1 | 2 | 3			Transactions per microframe
        
   Example: EP_PPMF(2,3);
   Important note: No spaces next to the commas
*/
#define[EP_PPMF(][,$1);][
#ifeq[$0][1IN]
#elifeq[$0][1OUT]
#elifeq[$0][2]
#elifeq[$0][4]
#elifeq[$0][6]
#elifneq[$0][8]
#error[EP_CONFIG: Invalid 1st parameter: `$0'. Expected `1IN', `1OUT', `2', `4', `6' or '8']
#endif
#ifeq[$1][1]
#elifeq[$1][2]
#elifneq[$1][3]
#error[EP_CONFIG: Invalid 2nd parameter: `$1'. Expected `1', `2' or '3']
#endif
#define[EP$0_PPMF][$1]]

EP_PPMF(1IN,1);
EP_PPMF(1OUT,1);
EP_PPMF(2,1);
EP_PPMF(4,1);
EP_PPMF(6,1);
EP_PPMF(8,1);

/* 
   Polling interval in microframes for INT transactions:

   Default value is 1 for all endpoints.

   EP_POLL(<EP number>,<polling interval>)
        <EP number>        = 1IN | 1OUT | 2 | 4 | 6 | 8		Endpoint
        <polling interval> = 1 | 2 | 3				Polling interval
        
   Example: EP_POLL(2,1);
   Important note: No spaces next to the commas
*/
#define[EP_POLL(][,$1);][
#ifeq[$0][1IN]
#elifeq[$0][1OUT]
#elifeq[$0][2]
#elifeq[$0][4]
#elifeq[$0][6]
#elifneq[$0][8]
#error[EP_CONFIG: Invalid 1st parameter: `$0'. Expected `1IN', `1OUT', `2', `4', `6' or '8']
#endif
#define[EP$0_POLL][$1]]

EP_POLL(1IN,1);
EP_POLL(1OUT,1);
EP_POLL(2,1);
EP_POLL(4,1);
EP_POLL(6,1);
EP_POLL(8,1);



/* 
   Settings which depends PRODUCT_ID, e.g extra capabilities.
   Overwrite this macros as desired.
*/
#define[MODULE_RESERVED_00][0]
#define[MODULE_RESERVED_01][0]
#define[MODULE_RESERVED_02][0]
#define[MODULE_RESERVED_03][0]
#define[MODULE_RESERVED_04][0]
#define[MODULE_RESERVED_05][0]
#define[MODULE_RESERVED_06][0]
#define[MODULE_RESERVED_07][0]
#define[MODULE_RESERVED_08][0]
#define[MODULE_RESERVED_09][0]
#define[MODULE_RESERVED_10][0]
#define[MODULE_RESERVED_11][0]

#define[FWVER][0]

#define[PRODUCT_ID_0][0]
#define[PRODUCT_ID_1][0]
#define[PRODUCT_ID_2][0]
#define[PRODUCT_ID_3][0]


/* 
   Identify as ZTEX USB FPGA Module 1.0
   Usage: IDENTITY_UFM_1_0(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UFM_1_0(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UFM-1_0]
#define[PRODUCT_STRING]["USB-FPGA Module 1.0"]]


/* 
   Identify as ZTEX USB FPGA Module 1.1
   Usage: IDENTITY_UFM_1_1(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UFM_1_1(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UFM-1_1]
#define[PRODUCT_STRING]["USB-FPGA Module 1.1"]]


/* 
   Identify as ZTEX USB FPGA Module 1.2
   Usage: IDENTITY_UFM_1_2(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UFM_1_2(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UFM-1_2]
#define[PRODUCT_STRING]["USB-FPGA Module 1.2"]]

/* 
   Identify as ZTEX USB FPGA Module 1.10
   Usage: IDENTITY_UFM_1_10(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UFM_1_10(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UFM-1_10]
#define[PRODUCT_STRING]["USB-FPGA Module 1.10"]]

/* 
   Identify as ZTEX USB FPGA Module 1.11
   Usage: IDENTITY_UFM_1_10(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UFM_1_11(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UFM-1_11]
#define[PRODUCT_STRING]["USB-FPGA Module 1.11"]]

/* 
   Identify as ZTEX USB FPGA Module 1.15
   Usage: IDENTITY_UFM_1_10(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UFM_1_15(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UFM-1_15]
#define[PRODUCT_STRING]["USB-FPGA Module 1.15"]
#define[NUMBER_OF_FPGAS][1]]

/* 
   Identify as ZTEX USB FPGA Module 1.15y
   Usage: IDENTITY_UFM_1_10(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UFM_1_15Y(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UFM-1_15Y]
#define[PRODUCT_STRING]["USB-FPGA Module 1.15y"]
#define[NUMBER_OF_FPGAS][4]]


/* 
   Identify as ZTEX USB Module 1.0
   Usage: IDENTITY_UM_1_0(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UM_1_0(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UM-1_0]
#define[PRODUCT_STRING]["USB Module 1.0"]]


/* 
   Identify as ZTEX USB XMEGA Module 1.0
   Usage: IDENTITY_UM_1_0(<PRODUCT_ID_0>.<PRODUCT_ID_1><PRODUCT_ID_2>.<PRODUCT_ID_3>,<FW_VERSION>);
*/
#define[IDENTITY_UXM_1_0(][.$1.$2.$3,$4);][#define[PRODUCT_ID_0][$0]
#define[PRODUCT_ID_1][$1]
#define[PRODUCT_ID_2][$2]
#define[PRODUCT_ID_3][$3]
#define[FWVER][$4]
#define[PRODUCT_IS][UXM-1_0]
#define[PRODUCT_STRING]["USB XMEGA Module 1.0"]]


/* 
   This macro defines the Manufacturer string. Limited to 31 characters. 
*/
#define[MANUFACTURER_STRING]["ZTEX"]


/* 
   This macro defines the Product string. Limited to 31 characters. 
*/
#define[PRODUCT_STRING]["USB-FPGA Module"]

/* 
   This macro enables defines the Configuration string. Limited to 31 characters. 
*/
#define[CONFIGURATION_STRING]["default"]


/* 
   This macro disables EEPROM interface, I2C helper functions and all other I2C devices (enabled by default)
   Usage: DISABLE_EEPROM; 
*/
#define[DISABLE_EEPROM;][#define[EEPROM_DISABLED][1]]


/* 
   This macro enables the Flash interface, if available
   Usage: ENABLE_FLASH; 
*/
#define[ENABLE_FLASH;][#define[FLASH_ENABLED][1]]

/* 
   This macro enables the FPGA configuration using a bitstream from the Flash memory
   Usage: ENABLE_FLASH_BITSTREAM; 
*/
#define[ENABLE_FLASH_BITSTREAM;][#define[FLASH_BITSTREAM_ENABLED][1]]

/* 
   This enables the debug helper. The debug helper consists in a stack of messages which can be read out from host software.
   See ../examples/all/debug/Readme.
   Usage: ENABLE_DEBUG(<stack size>,<message_size>);
	<stack size>	number of messages in stack
	<message size>  message size in bytes
*/
#define[ENABLE_DEBUG(][,$1);][#define[DEBUG_ENABLED][1]
#define[DEBUG_STACK_SIZE][$0]
#define[DEBUG_MSG_SIZE][$1]]

/* 
   This macro disables XMEGA support, if available
   Usage: XMEGA_DISABLE;
*/
#define[XMEGA_DISABLE;][#define[XMEGA_DISABLED][1]]

/* 
   Enables support for ZTEX Experimantal Board 1.10
   Usage: EXTENSION_EXP_1_10;
*/
#define[EXTENSION_EXP_1_10;][#define[EXP_1_10_ENABLED][1]]

/* 
   Enables high speed FPGA configuration for ZTEX USB-FPGA Module 1.15 and 1.15y
   Usage: ENABLE_HS_FPGA_CONF(<ENDPOINT>);
	<endpoint>	endpoint which shall be used (any bulk output can be used)
*/
#define[ENABLE_HS_FPGA_CONF(][);][
#ifeq[$0][2]
#elifeq[$0][4]
#elifeq[$0][6]
#elifneq[$0][8]
#error[ENABLE_HS_FPGA_CONF: Invalid Endpoint. Valid values are: `2', `4', `6', `8'.]
#endif
#define[HS_FPGA_CONF_EP][$0]]

/* 
   This macro disables MAC EEPROM support, if available
   Usage: MAC_EEPROM_DISABLE;
*/
#define[DISABLE_MAC_EEPROM;][#define[EEPROM_MAC_DISABLED][1]]

/* 
   Enables detection of USB-FPGA Modules 1.15x. This avoids some warnings and makes the variable is_ufm_1_15x available.
   Usage: ENABLE_UFM_1_15X_DETECTION;
*/
#define[ENABLE_UFM_1_15X_DETECTION;][#define[UFM_1_15X_DETECTION_ENABLED][1]]

/* 
   This macro disables temperature sensor support
   Usage: TEMP_SENSOR_DISABLE;
*/
#define[TEMP_SENSOR_DISABLE;][#define[TEMP_SENSOR_DISABLED][1]]

#endif // ZTEX_CONF_H
