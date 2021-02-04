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
   Puts everything together.
*/

#ifndef[ZTEX_H]
#define[ZTEX_H]

#define[INIT_CMDS;][]

#ifneq[PRODUCT_IS][UFM-1_15]
#define[UFM_1_15X_DETECTION_ENABLED][0]
#endif

#ifeq[UFM_1_15X_DETECTION_ENABLED][1]
__xdata BYTE is_ufm_1_15x;
#endif

/* *********************************************************************
   ***** include the basic functions ***********************************
   ********************************************************************* */
#include[ztex-utils.h]

/* *********************************************************************
   ***** I2C helper functions, EEPROM and MAC EEPROM support ***********
   ********************************************************************* */
#ifneq[EEPROM_DISABLED][1]

#ifneq[EEPROM_MAC_DISABLED][1]
#ifeq[PRODUCT_IS][UFM-1_15]
#define[MAC_EEPROM_ENABLED]
#endif // PRODUCT_IS=UFM-1_15
#ifeq[PRODUCT_IS][UFM-1_15Y]
#define[MAC_EEPROM_ENABLED]
#endif // PRODUCT_IS=UFM-1_15Y
#endif // EEPROM_MAC_DISABLED

#include[ztex-eeprom.h]

#endif // EEPROM_DISABLED


/* *********************************************************************
   ***** Flash memory support ******************************************
   ********************************************************************* */
#ifeq[FLASH_ENABLED][1]

#ifeq[PRODUCT_IS][UFM-1_1]
#define[MMC_PORT][E]
#define[MMC_BIT_CS][7]
#define[MMC_BIT_DI][6]
#define[MMC_BIT_DO][4]
#define[MMC_BIT_CLK][5]
#include[ztex-flash1.h]

#elifeq[PRODUCT_IS][UFM-1_2]
#define[MMC_PORT][E]
#define[MMC_BIT_CS][7]
#define[MMC_BIT_DI][6]
#define[MMC_BIT_DO][4]
#define[MMC_BIT_CLK][5]
#include[ztex-flash1.h]

#elifeq[PRODUCT_IS][UM-1_0]
#define[MMC_PORT][C]
#define[MMC_BIT_CS][7]
#define[MMC_BIT_DI][6]
#define[MMC_BIT_DO][4]
#define[MMC_BIT_CLK][5]
#include[ztex-flash1.h]

#elifeq[PRODUCT_IS][UM-1_0]
#define[MMC_PORT][C]
#define[MMC_BIT_CS][7]
#define[MMC_BIT_DI][6]
#define[MMC_BIT_DO][4]
#define[MMC_BIT_CLK][5]
#include[ztex-flash1.h]

#elifeq[PRODUCT_IS][UFM-1_10]
#define[MMC_PORT][A]
#define[MMC__PORT_DO][D]
#define[MMC_BIT_DO][0]
#define[MMC_BIT_CS][5]
#define[MMC_BIT_DI][6]
#define[MMC_BIT_CLK][7]
#include[ztex-flash1.h]

#elifeq[PRODUCT_IS][UFM-1_11]
#define[MMC_PORT][C]
#define[MMC__PORT_DO][D]
#define[MMC_BIT_DO][0]
#define[MMC_BIT_CS][5]
#define[MMC_BIT_DI][7]
#define[MMC_BIT_CLK][6]
#include[ztex-flash1.h]

#elifeq[PRODUCT_IS][UFM-1_15]
#define[MMC_PORT][C]
#define[MMC_BIT_DO][4]
#define[MMC_BIT_CS][5]
#define[MMC_BIT_DI][7]
#define[MMC_BIT_CLK][6]
#include[ztex-flash1.h]

#elifeq[PRODUCT_IS][UXM-1_0]
#define[MMC_PORT][C]
#define[MMC_BIT_CS][7]
#define[MMC_BIT_DI][6]
#define[MMC_BIT_DO][4]
#define[MMC_BIT_CLK][5]
#include[ztex-flash1.h]

#else
#warning[Flash memory access is not supported by this product]
#define[FLASH_ENABLED][0]
#endif
#endif

/* *********************************************************************
   ***** FPGA configuration support ************************************
   ********************************************************************* */
#ifeq[PRODUCT_IS][UFM-1_0]
#include[ztex-fpga1.h]
#elifeq[PRODUCT_IS][UFM-1_1]
#include[ztex-fpga1.h]
#elifeq[PRODUCT_IS][UFM-1_2]
#include[ztex-fpga1.h]
#elifeq[PRODUCT_IS][UFM-1_10]
#include[ztex-fpga2.h]
#elifeq[PRODUCT_IS][UFM-1_11]
#include[ztex-fpga3.h]
#elifeq[PRODUCT_IS][UFM-1_15]
#include[ztex-fpga4.h]
#elifeq[PRODUCT_IS][UFM-1_15Y]
#include[ztex-fpga5.h]
#endif


/* *********************************************************************
   ***** DEBUG helper functions ****************************************
   ********************************************************************* */
#ifeq[DEBUG_ENABLED][1]
#include[ztex-debug.h]
#endif


/* *********************************************************************
   ***** XMEGA support *************************************************
   ********************************************************************* */
#ifneq[XMEGA_DISABLED][1]

#ifeq[PRODUCT_IS][UXM-1_0]
#define[PDI_PORT][A]
#define[PDI_BIT_CLK][0]
#define[PDI_BIT_DATA][1]
#include[ztex-xmega.h]
#endif

#ifeq[EXP_1_10_ENABLED][1]
#ifneq[PRODUCT_IS][UFM-1_0]
#elifneq[PRODUCT_IS][UFM-1_1]
#elifneq[PRODUCT_IS][UFM-1_2]
#elifneq[PRODUCT_IS][UFM-1_10]
#elifneq[PRODUCT_IS][UFM-1_11]
#elifneq[PRODUCT_IS][UFM-1_15]
#warning[ZTEX Experimental Board 1.10 is not supported by this product.]
#endif
#define[PDI_PORT][E]
#define[PDI_BIT_CLK][5]
#define[PDI_BIT_DATA][4]
#include[ztex-xmega.h]
#endif

#endif

/* *********************************************************************
   ***** define the descriptors ****************************************
   ********************************************************************* */
#include[ztex-descriptors.h]


/* *********************************************************************
   ***** Temperature sensor support ************************************
   ********************************************************************* */
#ifneq[EEPROM_DISABLED][1]
#ifneq[TEMP_SENSOR_DISABLED][1]
#ifeq[PRODUCT_IS][UFM-1_15Y]
#include[ztex-temp1.h]
#endif

#endif // TEMP_SENSOR_DISABLED
#endif // EEPROM_DISABLED


/* *********************************************************************
   ***** interrupt routines ********************************************
   ********************************************************************* */
#include[ztex-isr.h]


/* *********************************************************************
   ***** mac_eeprom_init ***********************************************
   ********************************************************************* */
#ifdef[@CAPABILITY_MAC_EEPROM;]
void mac_eeprom_init ( ) { 
    BYTE b,c,d;
    __xdata BYTE buf[5];
    __code char hexdigits[] = "0123456789ABCDEF";    
    
    for (b=0; b<10; b++) {	// abort if SN != "0000000000"
	if ( SN_STRING[b] != 48 )
	    return;
    }

    mac_eeprom_read ( buf, 0xfb, 5 );	// read the last 5 MAC digits

    c=0;
    for (b=0; b<5; b++) {	// convert to MAC to SN string
	d = buf[b];
	SN_STRING[c] = hexdigits[d>>4];
	c++;
	SN_STRING[c] = hexdigits[d & 15];
	c++;
    } 
}
#endif


/* *********************************************************************
   ***** init_USB ******************************************************
   ********************************************************************* */
#define[EPXCFG(][);][    EP$0CFG = 
#ifeq[EP$0_DIR][IN]
	bmBIT7 | bmBIT6
#elifeq[EP$0_DIR][OUT]
	bmBIT7
#else
	0
#endif
#ifeq[EP$0_TYPE][BULK]
	| bmBIT5 
#elifeq[EP$0_TYPE][ISO]
	| bmBIT4
#elifeq[EP$0_TYPE][INT]
	| bmBIT5 | bmBIT4
#endif
#ifeq[EP$0_SIZE][1024]
	| bmBIT3
#endif
#ifeq[EP$0_BUFFERS][2]
	| bmBIT1
#elifeq[EP$0_BUFFERS][3]
	| bmBIT1 | bmBIT0
#endif	
	;
	SYNCDELAY;
]

#define[EP1XCFG(][);][#ifeq[EP$0_TYPE][BULK]
	EP$0CFG = bmBIT7 | bmBIT5;
#elifeq[EP$0_TYPE][ISO]
	EP$0CFG = bmBIT7 | bmBIT4;
#elifeq[EP$0_TYPE][INT]
	EP$0CFG = bmBIT7 | bmBIT5 | bmBIT4;
#else 	
	EP$0CFG = 0;
#endif
	SYNCDELAY;
]


void init_USB ()
{
    USBCS |= bmBIT3;
    
    CPUCS = bmBIT4 | bmBIT1;
    wait(2);
    CKCON &= ~7;
    
#ifeq[PRODUCT_IS][UFM-1_0]
    IOA1 = 1;		
    OEA |= bmBIT1;
#elifeq[PRODUCT_IS][UFM-1_1]
    IOA1 = 1;		
    OEA |= bmBIT1;
#elifeq[PRODUCT_IS][UFM-1_2]
    IOA1 = 1;		
    OEA |= bmBIT1;
#elifeq[PRODUCT_IS][UFM-1_10]
    IOA1 = 1;		
    OEA |= bmBIT1;
#elifeq[PRODUCT_IS][UFM-1_11]
    IOA1 = 1;		
    OEA |= bmBIT1;
#elifeq[PRODUCT_IS][UFM-1_15]
    IOA1 = 1;		
    OEA |= bmBIT1;
#elifeq[PRODUCT_IS][UFM-1_15Y]
    init_fpga();
#endif

    INIT_CMDS;    

    EA = 0;
    EUSB = 0;

    ENABLE_AVUSB;
    
    INIT_INTERRUPT_VECTOR(INTVEC_SUDAV, SUDAV_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_SOF, SOF_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_SUTOK, SUTOK_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_SUSPEND, SUSP_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_USBRESET, URES_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_HISPEED, HSGRANT_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP0ACK, EP0ACK_ISR);

    INIT_INTERRUPT_VECTOR(INTVEC_EP0IN, EP0IN_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP0OUT, EP0OUT_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP1IN, EP1IN_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP1OUT, EP1OUT_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP2, EP2_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP4, EP4_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP6, EP6_ISR);
    INIT_INTERRUPT_VECTOR(INTVEC_EP8, EP8_ISR);

    EXIF &= ~bmBIT4;
    USBIRQ = 0x7f;
    USBIE |= 0x7f; 
    EPIRQ = 0xff;
    EPIE = 0xff;
    
    EUSB = 1;
    EA = 1;

    EP1XCFG(1IN);
    EP1XCFG(1OUT);
    EPXCFG(2);
    EPXCFG(4);
    EPXCFG(6);
    EPXCFG(8);

#ifeq[UFM_1_15X_DETECTION_ENABLED][1]
    OEA &= ~bmBIT3;
    if ( IOA3 ) {
	is_ufm_1_15x = 0;
    } else {
	is_ufm_1_15x = 1;
//	INTERFACE_CAPABILITIES[0] &= ~32;
    }
#endif    
    
	//USBCS |= bmBIT1;
	
#ifeq[FLASH_ENABLED][1]
    flash_init();
#endif
#ifeq[FLASH_BITSTREAM_ENABLED][1]
    fpga_configure_from_flash_init();
#endif
#ifeq[DEBUG_ENABLED][1]
    debug_init();
#endif
#ifeq[XMEGA_ENABLED][1]
    xmega_init();
#endif
#ifdef[@CAPABILITY_MAC_EEPROM;]
    mac_eeprom_init();
#endif
#ifeq[TEMP1_ENABLED][1]
    temp1_init();
#endif

    USBCS |= bmBIT7 | bmBIT1;
    wait(10);
//    wait(250);
    USBCS &= ~bmBIT3;
}


#endif   /* ZTEX_H */
