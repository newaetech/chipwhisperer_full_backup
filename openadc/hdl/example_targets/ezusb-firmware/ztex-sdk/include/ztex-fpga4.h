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
    FPGA support for ZTEX USB FPGA Modules 1.15 (not 1.15y)
*/    

#ifndef[ZTEX_FPGA_H]
#define[ZTEX_FPGA_H]

#define[@CAPABILITY_FPGA;]

__xdata BYTE fpga_checksum;         // checksum
__xdata DWORD fpga_bytes;           // transfered bytes
__xdata BYTE fpga_init_b;           // init_b state (should be 222 after configuration)
__xdata BYTE fpga_flash_result;     // result of automatic fpga configuarion from Flash

/* *********************************************************************
   ***** reset_fpga ****************************************************
   ********************************************************************* */
static void reset_fpga_int (BYTE mode) {		// reset FPGA
    unsigned short k;
    IFCONFIG = bmBIT7;
    SYNCDELAY; 
    PORTACFG = 0;
    PORTCCFG = 0;
    OEC &= ~( bmBIT1 | bmBIT2);  // in: DOUT, INIT_B
//  out:  RESET,   M1,      CCLK,    M0,      CSI
    OEA = bmBIT1 | bmBIT2 | bmBIT4 | bmBIT5 | bmBIT7;
    IOA = bmBIT7 | bmBIT4 | mode;
    OEC |= bmBIT3;              // out: RDWR_B
    IOC &= ~bmBIT3;
    wait(10);
		    
    IOA = bmBIT1 | mode;				// ready for configuration
    k=0;
    while (!IOC2 && k<65535)
	k++;

    fpga_init_b = IOC2 ? 200 : 100;
    fpga_bytes = 0;
    fpga_checksum = 0;
}    

static void reset_fpga () {
    reset_fpga_int(bmBIT2);
}

static void reset_fpga_flash () {
    reset_fpga_int(bmBIT2 | bmBIT4 | bmBIT5 );
}
    
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
    BYTE w;
    fpga_init_b += IOC2 ? 22 : 11;

    for ( w=0; w<64; w++ ) {
        IOA4 = 1; IOA4 = 0; 
    }
    IOA7 = 1;
    IOA4 = 1; IOA4 = 0;
    IOA4 = 1; IOA4 = 0;
    IOA4 = 1; IOA4 = 0;
    IOA4 = 1; IOA4 = 0;

    OEA = 0;
    OEC &= ~bmBIT3;
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
    EP0BUF[8] = 1;			// bit order for bitstream in Flash memory: swapped
    
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
void fpga_send_ep0() {			// send FPGA configuration data
    BYTE oOEB;
    oOEB = OEB;
    OEB = 255;
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
	movx	a,@dptr			// 2
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	add 	a,r1			// 1
	mov 	r1,a                    // 1
	clr	_IOA4                   // 2
	djnz	r2, 010001$		// 4

	mov	dptr,#_fpga_checksum
	mov	a,r1
	movx	@dptr,a
	
010000$:
    	__endasm; 
    OEB = oOEB;
    if ( EP0BCL<64 ) {
    	finish_fpga_configuration();
    } 
}

ADD_EP0_VENDOR_COMMAND((0x32,,		// send FPGA configuration data
,,
    fpga_send_ep0();
));;


#ifdef[HS_FPGA_CONF_EP]

#ifeq[HS_FPGA_CONF_EP][2]
#elifeq[HS_FPGA_CONF_EP][4]
#elifeq[HS_FPGA_CONF_EP][6]
#elifneq[HS_FPGA_CONF_EP][8]
#error[`HS_FPGA_CONF_EP' is not defined correctly. Valid values are: `2', `4', `6', `8'.]
#endif

#define[@CAPABILITY_HS_FPGA;]

/* *********************************************************************
   ***** EP0 vendor request 0x33 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_REQUEST((0x33,,		// get high speed fpga configuration endpoint and interface 
    EP0BUF[0] = HS_FPGA_CONF_EP;	// endpoint
    EP0BUF[1] = EPHS_FPGA_CONF_EP_INTERFACE; // interface
    EP0BCH = 0;
    EP0BCL = 2;
,,));;


#ifeq[UFM_1_15X_DETECTION_ENABLED][1]
/* *********************************************************************
   ***** interrupt routine for EPn *************************************
   ********************************************************************* */
xdata WORD old_hsconf_intvec_h, old_hsconf_intvec_l;
   
static void fpga_hs_send_isr () __interrupt {
    BYTE oOEB;
    oOEB = OEB;

    EUSB = 0;			// block all USB interrupts

    fpga_bytes += (EPHS_FPGA_CONF_EPBCH<<8) | EPHS_FPGA_CONF_EPBCL;

    OEB = 255;
    __asm
	mov	dptr,#_EPHS_FPGA_CONF_EPBCL
	movx	a,@dptr
  	mov	r2,a
	anl 	a,#7
	mov 	r3,a
	mov	dptr,#_EPHS_FPGA_CONF_EPBCH
	movx	a,@dptr
	addc 	a,#0
  	
  	rrc 	a
  	mov 	r1,a
  	mov 	a,r2
  	rrc 	a
  	mov 	r2,a

  	mov 	a,r1
  	rrc 	a
  	mov 	r1,a
  	mov 	a,r2
  	rrc 	a
  	mov 	r2,a

  	mov 	a,r1
  	rrc 	a
  	mov 	r1,a
  	mov 	a,r2
  	rrc 	a
  	mov 	r2,a
  	
	mov 	_AUTOPTRL1,#(_EPHS_FPGA_CONF_EPFIFOBUF)
	mov 	_AUTOPTRH1,#(_EPHS_FPGA_CONF_EPFIFOBUF >> 8)
	mov 	_AUTOPTRSETUP,#0x07
	mov	dptr,#_XAUTODAT1

	mov 	a,r3
	jz 	010011$
010012$:
	movx	a,@dptr			// 2, 1
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2
	djnz	r3, 010012$		// 4
	

	mov 	a,r2
  	jz 	010010$
010011$:
	movx	a,@dptr			// 2, 1
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	movx	a,@dptr			// 2, 2
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	movx	a,@dptr			// 2, 3
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	movx	a,@dptr			// 2, 4
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	movx	a,@dptr			// 2, 5
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	movx	a,@dptr			// 2, 6
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	movx	a,@dptr			// 2, 7
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	movx	a,@dptr			// 2, 8
	mov	_IOB,a			// 2
	setb	_IOA4			// 2
	clr	_IOA4                   // 2

	djnz	r2, 010011$		// 4
	
010010$:
    	__endasm; 
    OEB = oOEB;

    
    OUTPKTEND = 0x8HS_FPGA_CONF_EP;	// skip package, (re)arm EP
//    EPHS_FPGA_CONF_EPBCL = 0x80;	// skip package, (re)arm EP
    SYNCDELAY;

    EXIF &= ~bmBIT4;
    EPIRQ = 1 << ((HS_FPGA_CONF_EP >> 1)+3);

    EUSB = 1;
}
#endif

/* *********************************************************************
   ***** EP0 vendor command 0x34 ***************************************
   ********************************************************************* */
// FIFO write wave form
const char __xdata GPIF_WAVE_DATA_HSFPGA_24MHZ[32] =     
{ 
/* LenBr */ 0x01,     0x88,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x02,     0x07,     0x02,     0x02,     0x02,     0x02,     0x02,     0x00,
/* Output*/ 0x04,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x04, // CTL2 <-> 0x04
/* LFun  */ 0x00,     0x36,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
};                     

const char __xdata GPIF_WAVE_DATA_HSFPGA_12MHZ[32] =     
{ 
/* LenBr */ 0x02,     0x01,     0x90,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x02,     0x02,     0x07,     0x02,     0x02,     0x02,     0x02,     0x00,
/* Output*/ 0x04,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x04,  // CTL2 <-> 0x04
/* LFun  */ 0x00,     0x00,     0x36,     0x00,     0x00,     0x00,     0x00,     0x3F,
};                     


void init_cpld_fpga_configuration() {
    IFCONFIG = bmBIT7 | bmBIT6 | bmBIT5 | 2;	// Internal source, 48MHz, GPIF

    GPIFREADYCFG = 0; //bmBIT7 | bmBIT6 | bmBIT5;
    GPIFCTLCFG = 0x0; 
    GPIFIDLECS = 0;
    GPIFIDLECTL = 4;
    GPIFWFSELECT = 0x4E;
    GPIFREADYSTAT = 0;

    MEM_COPY1(GPIF_WAVE_DATA_HSFPGA_24MHZ,GPIF_WAVE3_DATA,32);

    FLOWSTATE = 0;
    FLOWLOGIC = 0x10;
    FLOWEQ0CTL = 0;
    FLOWEQ1CTL = 0;
    FLOWHOLDOFF = 0;
    FLOWSTB = 0;
    FLOWSTBEDGE = 0;
    FLOWSTBHPERIOD = 0;

    REVCTL = 0x1;				// reset fifo
    SYNCDELAY; 
    FIFORESET = 0x80;
    SYNCDELAY;
    FIFORESET = HS_FPGA_CONF_EP;
    SYNCDELAY;
    FIFORESET = 0x0;
    SYNCDELAY; 

    EPHS_FPGA_CONF_EPFIFOCFG = 0;		// config fifo
    SYNCDELAY; 
    EPHS_FPGA_CONF_EPFIFOCFG = bmBIT4 | 0;
    SYNCDELAY;
    EPHS_FPGA_CONF_EPGPIFFLGSEL = 1;
    SYNCDELAY;

    GPIFTCB3 = 1;				// abort after at least 14*65536 transactions
    SYNCDELAY;
    GPIFTCB2 = 0;
    SYNCDELAY;
    GPIFTCB1 = 0;
    SYNCDELAY;
    GPIFTCB0 = 0;
    SYNCDELAY;
    
    EPHS_FPGA_CONF_EPGPIFTRIG = 0xff;		// arm fifos
    SYNCDELAY;
    
    OEA &= ~bmBIT4;				// disable CCLK output
    OEA |= bmBIT0;				// enable GPIF mode of CPLD
    IOA0 = 0;
}

#ifeq[UFM_1_15X_DETECTION_ENABLED][1]
xdata WORD old_hsconf_intvec_h, old_hsconf_intvec_l;

void init_epn_fpga_configuration() {

    IFCONFIG = bmBIT7;

    REVCTL = 0x03;				// reset fifo
    SYNCDELAY; 
    FIFORESET = 0x80;
    SYNCDELAY;
    FIFORESET = HS_FPGA_CONF_EP;
    SYNCDELAY;
    FIFORESET = 0x0;
    SYNCDELAY; 

    EPHS_FPGA_CONF_EPFIFOCFG = 0;		// config fifo
    SYNCDELAY;
    
    OUTPKTEND = 0x8HS_FPGA_CONF_EP;	// skip package, (re)arm EP
    SYNCDELAY;
    OUTPKTEND = 0x8HS_FPGA_CONF_EP;	// skip package, (re)arm EP
    SYNCDELAY;
    OUTPKTEND = 0x8HS_FPGA_CONF_EP;	// skip package, (re)arm EP
    SYNCDELAY;
    OUTPKTEND = 0x8HS_FPGA_CONF_EP;	// skip package, (re)arm EP
    SYNCDELAY; 

/*    EPHS_FPGA_CONF_EPBCL = 0x80;	// skip package, (re)arm EP
    SYNCDELAY;
    EPHS_FPGA_CONF_EPBCL = 0x80;	// skip package, (re)arm EP
    SYNCDELAY;
    EPHS_FPGA_CONF_EPBCL = 0x80;	// skip package, (re)arm EP
    SYNCDELAY;
    EPHS_FPGA_CONF_EPBCL = 0x80;	// skip package, (re)arm EP
    SYNCDELAY; */

    old_hsconf_intvec_l = INTVEC_EPHS_FPGA_CONF_EP.addrL;
    old_hsconf_intvec_h = INTVEC_EPHS_FPGA_CONF_EP.addrH;
    INTVEC_EPHS_FPGA_CONF_EP.addrH=((unsigned short)(&fpga_hs_send_isr)) >> 8;
    INTVEC_EPHS_FPGA_CONF_EP.addrL=(unsigned short)(&fpga_hs_send_isr);

    EXIF &= ~bmBIT4;
    EPIRQ = 1 << ((HS_FPGA_CONF_EP >> 1)+3);
}
#endif

ADD_EP0_VENDOR_COMMAND((0x34,,			// init fpga configuration
    init_fpga_configuration();

    EPHS_FPGA_CONF_EPCS &= ~bmBIT0;		// clear stall bit

    GPIFABORT = 0xFF;				// abort pendig 
    
#ifeq[UFM_1_15X_DETECTION_ENABLED][1]
    if ( is_ufm_1_15x )
	init_epn_fpga_configuration();
    else
#endif    
	init_cpld_fpga_configuration();
    
,,));;


/* *********************************************************************
   ***** EP0 vendor command 0x35 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_COMMAND((0x35,,		// finish fpga configuration
#ifeq[UFM_1_15X_DETECTION_ENABLED][1]
    if ( is_ufm_1_15x ) {
	INTVEC_EPHS_FPGA_CONF_EP.addrL = old_hsconf_intvec_l;
	INTVEC_EPHS_FPGA_CONF_EP.addrH = old_hsconf_intvec_h;
    }
    else 
#endif    
    {
	IOA0 = 1;			    // disable GPIF mode of CPLD
	IOA4 = 1;                           // enable CCLK output
	OEA |= bmBIT4;			

	GPIFABORT = 0xFF;
	SYNCDELAY;
	IFCONFIG &= 0xf0;
	SYNCDELAY;

    }
    finish_fpga_configuration();
,,));;

#endif  // HS_FPGA_CONF_EP



#ifeq[FLASH_BITSTREAM_ENABLED][1]
#ifeq[FLASH_ENABLED][1]

/* *********************************************************************
   ***** fpga_send_bitstream_from_flash ********************************
   ********************************************************************* */
void fpga_send_bitstream_from_flash (WORD size) {
	size;			// this avoids stupid warnings
__asm
	push 	_OEB

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

// size == 512
	cjne	r5,#0,010004$	
	cjne	r6,#2,010004$	
//	sjmp	010004$	

	mov	_OEB, #0
	anl 	_OEA, #(~bmBIT4)	
	setb	_IOC6
	anl 	_OEC, #(~bmBIT6)
	orl 	_OEA, #(bmBIT3)
	clr     _IOA3
	setb    _IOA3
	anl 	_OEA, #(~bmBIT3)
	
	mov 	r2, #3			// wait > 2 clocks
010008$:
	mov 	r1, #227
010009$:
	djnz	r1, 010009$
	djnz	r2, 010008$

	setb	_IOA4     
	orl 	_OEA, #(bmBIT4)	
	orl 	_OEC, #(bmBIT6)
	clr	_IOC6
	pop 	_OEB
	ret
	
// size != 512
010004$:
	mov	_OEB,#1
010003$:
	cjne	r5,#0x00,010002$	// 4
	cjne	r6,#0x00,010002$
	pop 	_OEB
	ret
010002$:			   	// approx 105 cycles per byte
	mov	C, _IOC4  // 2
	mov     _IOB0, C  // 2
	clr	_IOA4     // 2
	setb	_IOA4     // 2
	setb	_IOC6     // 2
	clr	_IOC6     // 2

	mov	C, _IOC4  
	mov     _IOB0, C  
	clr	_IOA4     
	setb	_IOA4     
	setb	_IOC6     
	clr	_IOC6     

	mov	C, _IOC4  
	mov     _IOB0, C  
	clr	_IOA4     
	setb	_IOA4     
	setb	_IOC6     
	clr	_IOC6     

	mov	C, _IOC4  
	mov     _IOB0, C  
	clr	_IOA4     
	setb	_IOA4     
	setb	_IOC6     
	clr	_IOC6     

	mov	C, _IOC4  
	mov     _IOB0, C  
	clr	_IOA4     
	setb	_IOA4     
	setb	_IOC6     
	clr	_IOC6     

	mov	C, _IOC4  
	mov     _IOB0, C  
	clr	_IOA4     
	setb	_IOA4     
	setb	_IOC6     
	clr	_IOC6     

	mov	C, _IOC4  
	mov     _IOB0, C  
	clr	_IOA4     
	setb	_IOA4     
	setb	_IOC6     
	clr	_IOC6     

	mov	C, _IOC4  
	mov     _IOB0, C  
	clr	_IOA4     
	setb	_IOA4     
	setb	_IOC6     
	clr	_IOC6     

	dec	r5			// 1
	cjne	r5,#0xff,010003$	// 4
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
