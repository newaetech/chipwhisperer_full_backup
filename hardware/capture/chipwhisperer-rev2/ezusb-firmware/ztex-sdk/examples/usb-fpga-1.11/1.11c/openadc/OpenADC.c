/*!
   intraffic -- example showing how the EZ-USB FIFO interface is used on ZTEX USB-FPGA Module 1.11c
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

#include[ztex-conf.h]	// Loads the configuration macros, see ztex-conf.h for the available macros
#include[ztex-utils.h]	// include basic functions

//Use SD FLASH Memory
ENABLE_FLASH;
ENABLE_FLASH_BITSTREAM;

// 1024 (instead of 512) byte bulk transfers. 
// According to USB standard they are invalid but usually supported and 25% faster.
//#define[fastmode]

#ifdef[fastmode]
// configure endpoint 2, in, quad buffered, 1024 bytes, interface 0
EP_CONFIG(2,0,BULK,IN,1024,4);
#else
// configure endpoint 2, in, quad buffered, 512 bytes, interface 0
EP_CONFIG(2,0,BULK,IN,512,4);
#endif

#ifdef[fastmode]
// configure endpoint 6, out, quad buffered, 1024 bytes, interface 0
EP_CONFIG(6,0,BULK,OUT,1024,4);
#else
// configure endpoint 6, out, quad buffered, 512 bytes, interface 0
EP_CONFIG(6,0,BULK,OUT,512,4);
#endif

// select ZTEX USB FPGA Module 1.11 as target  (required for FPGA configuration)
IDENTITY_UFM_1_11(10.12.0.0,0);	 

// this product string is also used for identification by the host software
#define[PRODUCT_STRING]["OpenADC for UFM 1.11"]

// this is called automatically after FPGA configuration
#define[POST_FPGA_CONFIG][POST_FPGA_CONFIG
	IOA0 = 1;				// reset on
	IOA3 = 0;				// controlled mode
	OEA |= bmBIT0 | bmBIT3;

	EP2CS &= ~bmBIT0;			// clear stall bit
    
	REVCTL = 0x3;
	SYNCDELAY; 

	IFCONFIG = bmBIT7 | bmBIT5 | 3;	        // internel 30MHz clock, drive IFCLK ouput, slave FIFO interface
	SYNCDELAY; 
	
	FIFORESET = 0x80;			// reset FIFO
	SYNCDELAY;
	FIFORESET = 2;
	SYNCDELAY;
	FIFORESET = 0x00;
	SYNCDELAY;
	
	//ENDPOINT 2 CONFIG
	EP2FIFOCFG = bmBIT3;           // AOTUOIN, 8-bit word length
	SYNCDELAY;
    		
#ifdef[fastmode]
	EP2AUTOINLENH = 4;                 	// 1024 bytes 
#else	
	EP2AUTOINLENH = 2;                 	// 512 bytes 
#endif	
	SYNCDELAY;
	EP2AUTOINLENL = 0;
	SYNCDELAY;
	   
	FIFORESET = 0x80;			// reset FIFO
	SYNCDELAY;
	FIFORESET = 6;
	SYNCDELAY;
	FIFORESET = 0x00;
	SYNCDELAY;
		
	OUTPKTEND = 0x86;
	SYNCDELAY;
	OUTPKTEND = 0x86;
	SYNCDELAY;
	OUTPKTEND = 0x86;
	SYNCDELAY;
	OUTPKTEND = 0x86;
	
	//ENDPOINT 6 CONFIG
	EP6FIFOCFG = bmBIT4; //AUTOOUT, 8-bit word length
	SYNCDELAY;
	
	//General other config
	
	FIFOPINPOLAR = 0;
	SYNCDELAY; 
	//FLAGA = EP2 FULL (b1100 = 0x0C)
	//FLAGB = EP6 EMPTY(b1010 = 0x0A)
	PINFLAGSAB = 0xAC;
	SYNCDELAY; 
	PINFLAGSCD = 0;
	SYNCDELAY; 

	IOA0 = 0;				// reset off
]

// set mode
ADD_EP0_VENDOR_COMMAND((0x60,,
	IOA0 = 1;				// reset on
	IOA3 = SETUPDAT[2] ? 1 : 0;
	IOA0 = 0;				// reset off
,,
	NOP;
));;

// include the main part of the firmware kit, define the descriptors, ...
#include[ztex.h]

void main(void)	
{
    init_USB();

    while (1) {	
    }
}

