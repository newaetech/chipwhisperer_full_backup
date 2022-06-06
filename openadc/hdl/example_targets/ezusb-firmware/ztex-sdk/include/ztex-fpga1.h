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
    FPGA support for ZTEX USB FPGA Modules 1.0, 1.1 and 1.2
*/    

#ifndef[ZTEX_FPGA_H]
#define[ZTEX_FPGA_H]

#define[@CAPABILITY_FPGA;]

__xdata BYTE fpga_checksum;         // checksum
__xdata DWORD fpga_bytes;           // transfered bytes
__xdata BYTE fpga_init_b;           // init_b state (should be 222 after configuration)
__xdata BYTE fpga_flash_result;     // result of FPGA configuarion from Flash at startup

/* *********************************************************************
   ***** reset_fpga ****************************************************
   ********************************************************************* */
static void reset_fpga () {		// reset FPGA
    unsigned short k;
    IFCONFIG = bmBIT7;
    SYNCDELAY; 
    PORTACFG = 0;
    PORTCCFG = 0;

    OEA = bmBIT1;
    IOA1 = 0;
    wait(10);
		    
    OEB = 0xff;				// setup IO's
    OEC = bmBIT2 | bmBIT3;
    IOC = bmBIT3;
    
    OED = bmBIT0;
    IOD0 = 0;				

    IOA1 = 1;				// ready for configuration
    IOD0 = 1;				
    k=0;
    while (!IOC0 && k<65535)
	k++;

    fpga_init_b = IOC0 ? 200 : 100;
    fpga_bytes = 0;
    fpga_checksum = 0;
    
    IOC = 0;
}    

#define[reset_fpga_flash][reset_fpga];

/* *********************************************************************
   ***** init_fpga_configuration ***************************************
   ********************************************************************* */
static void init_fpga_configuration () {
    {
	PRE_FPGA_RESET
    }
    reset_fpga();			// reset FPGA
}    

/* *********************************************************************
   ***** post_fpga_confog **********************************************
   ********************************************************************* */
static void post_fpga_config () {
    POST_FPGA_CONFIG
}

/* *********************************************************************
   ***** finish_fpga_configuration *************************************
   ********************************************************************* */
static void finish_fpga_configuration () {
    fpga_init_b += IOC0 ? 20 : 10;
    IOD0 = 0;  IOB = 0;  IOD0 = 1;
    IOD0 = 0;  IOD0 = 1;
    IOD0 = 0;  IOD0 = 1;
    IOD0 = 0;  IOD0 = 1;
    IOD0 = 0;  IOD0 = 1;
    IOD0 = 0;  IOD0 = 1;
    IOD0 = 0;  IOD0 = 1;
    IOD0 = 0;  IOD0 = 1; 
    IOD0 = 0;  IOD0 = 1; 
    IOD0 = 0;  IOD0 = 1; 
    IOD0 = 0;  IOD0 = 1; 
    OEB = 0;
    OEC = 0;
    OED = 0;
    OEA = 0;
    fpga_init_b += IOC0 ? 2 : 1;
    if ( IOA1 )  {
	IOA1 = 1;		
	post_fpga_config();
    }

    IOA1 = 1;		
    OEA |= bmBIT1;
}    


/* *********************************************************************
   ***** EP0 vendor request 0x30 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_REQUEST((0x30,,		// get FPGA state
    MEM_COPY1(fpga_checksum,EP0BUF+1,7);    
    OEA &= ~bmBIT1;
    if ( IOA1 )  {
	EP0BUF[0] = 0; 	 		// FPGA configured 
	IOA1 = 1;		
	OEA |= bmBIT1;
    }
    else {
        EP0BUF[0] = 1;			// FPGA unconfigured 
	reset_fpga();			// prepare FPGA for configuration
    }
    EP0BUF[8] = 0;			// bit order for bitstream in FLASH: non swapped
    
    EP0BCH = 0;
    EP0BCL = 9;
,,));;


/* *********************************************************************
   ***** EP0 vendor command 0x31 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_COMMAND((0x31,,init_fpga_configuration();,,));;	// reset FPGA


/* *********************************************************************
   ***** EP0 vendor command 0x32 ***************************************
   ********************************************************************* */
void fpga_send_ep0() {
    fpga_bytes += ep0_payload_transfer;
    __asm
	mov	dptr,#_EP0BCL
	movx	a,@dptr
	jz 	010000$
  	mov	r2,a
	mov 	_AUTOPTRL1,#(_EP0BUF)
	mov 	_AUTOPTRH1,#(_EP0BUF >> 8)
	mov 	_AUTOPTRSETUP,#0x07
	mov	dptr,#_fpga_checksum
	movx 	a,@dptr
	mov 	r1,a
	mov	dptr,#_XAUTODAT1
010001$:
	clr	_IOD0
	movx	a,@dptr
	mov	_IOB,a
	add 	a,r1
	mov 	r1,a
	setb	_IOD0
	djnz	r2, 010001$

	mov	dptr,#_fpga_checksum
	mov	a,r1
	movx	@dptr,a
	
010000$:
    	__endasm; 
    if ( EP0BCL<64 ) {
    	finish_fpga_configuration();
    } 
}

ADD_EP0_VENDOR_COMMAND((0x32,,		// send FPGA configuration data
,,
    fpga_send_ep0();
));;


#ifeq[FLASH_BITSTREAM_ENABLED][1]
#ifeq[FLASH_ENABLED][1]

/* *********************************************************************
   ***** fpga__send_bitstream_from_flash *******************************
   ********************************************************************* */
void fpga_send_bitstream_from_flash (WORD size) {
	size;			// this avoids stupid warnings
__asm
	mov	r5,dpl		// = size
	mov	r6,dph

	// fpga_bytes+=size
	mov	dptr,#_fpga_bytes
	movx	a,@dptr
	mov	r1,a
	inc	dptr
	movx	a,@dptr
	mov	r2,a
	inc	dptr
	movx	a,@dptr
	mov	r3,a
	inc	dptr
	movx	a,@dptr
	mov	r4,a

	mov	dptr,#_fpga_bytes
	mov	a,r5
	add	a,r1
	movx	@dptr,a
	mov	a,r6
	addc	a,r2
	inc	dptr
	movx	@dptr,a
	mov	a,#0
	addc	a,r3
	inc	dptr
	movx	@dptr,a
	mov	a,#0
	addc	a,r4
	inc	dptr
	movx	@dptr,a

	mov	dptr,#_fpga_checksum
	movx 	a,@dptr
	mov 	r7,a

010003$:
	cjne	r5,#0x00,010002$
	cjne	r6,#0x00,010002$
	mov	dptr,#_fpga_checksum
	mov 	a,r7
	movx 	@dptr,a
	ret
010002$:
	clr	_IOD0
	lcall	_flash_read_byte
	mov	a,dpl
	mov	_IOB,a
	add	a,r7
	mov	r7,a
	setb	_IOD0
	dec	r5
	cjne	r5,#0xff,010003$
	dec	r6
	sjmp	010003$
__endasm;    
}

#include[ztex-fpga-flash.h]

#else
#warning[Flash interface is not enabled but required for FPGA configuration using a bitstream from Flash meomory]
#define[FLASH_BITSTREAM_ENABLED][0]
#endif
#endif

#endif  /*ZTEX_FPGA_H*/
