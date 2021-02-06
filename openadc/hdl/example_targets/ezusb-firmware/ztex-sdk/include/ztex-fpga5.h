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
    FPGA support for ZTEX USB FPGA Modules 1.15y
*/    

#ifndef[ZTEX_FPGA_H]
#define[ZTEX_FPGA_H]

#define[@CAPABILITY_FPGA;]
#define[@CAPABILITY_MULTI_FPGA;]
__xdata BYTE fpga_checksum;         // checksum
__xdata DWORD fpga_bytes;           // transferred bytes
__xdata BYTE fpga_init_b;           // init_b state (should be 222 after configuration)

__xdata BYTE select_num;
__xdata BYTE prev_select_num;
__xdata BYTE select_mask;
__xdata BYTE config_mask_h;
__xdata BYTE config_mask_l;


/* *********************************************************************
   ***** init_fpga *****************************************************
   ********************************************************************* */
void init_fpga () {	
    IOE = 0x1f;		
    OEE = 0xff;

    prev_select_num = 0;
    select_num = 0;
    select_mask = 0x10;
    config_mask_h = 0x10;
    config_mask_l = 0x01;
}


/* *********************************************************************
   ***** reset_fpga ****************************************************
   ********************************************************************* */
static void reset_fpga () {		// reset FPGA
    WORD k;
    IFCONFIG = bmBIT7;
    SYNCDELAY; 
    PORTACFG = 0;
    PORTCCFG = 0;

    IOC2 = 1;			 // out: INIT_B
    OEC |= bmBIT2;
    
    OEA &= ~bmBIT6; 		 // in: CSO
    OEC &= ~bmBIT1;              // in: DOUT

    IOC3 = 0;
    OEC |= bmBIT3;    		 // out: RDWR_B

//  out:    CCLK,    M1,      GPIF,    M0,      CSI
    OEA |= bmBIT1 | bmBIT2 | bmBIT3 | bmBIT5 | bmBIT7;
    IOA5 = 0;
    IOA |= bmBIT1 | bmBIT2 | bmBIT3 |          bmBIT7;
    
    IOE = config_mask_h | ((~config_mask_l) & 0x0f);
    OEE = 0xff;
    wait(1);

    IOA7 = 0;
    IOA1 = 0;
    IOE = config_mask_h | 0x0f;
    k=0;
    OEC &= ~bmBIT2;              // in: INIT_B
    while ( (!IOC2) && (k<65535) ) {
	k++;
    }

    fpga_init_b = IOC2 ? 200 : 100;
    fpga_bytes = 0;
    fpga_checksum = 0;
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
    BYTE b;
    fpga_init_b += 22;

    for ( b=0; b<255; b++ ) {
        IOA1 = 1; IOA1 = 0; 
    }
    IOA7 = 1;
    IOA4 = 1; IOA4 = 0;
    IOA4 = 1; IOA4 = 0;
    IOA4 = 1; IOA4 = 0;
    IOA4 = 1; IOA4 = 0;

    OEA &= ~(bmBIT1 | bmBIT2 | bmBIT3 | bmBIT7);
    OEC &= ~bmBIT3;
    OEE = 0xf0;
    if ( (IOE & config_mask_l) == config_mask_l ) {
	post_fpga_config();
    }

    IOE = select_mask | 0x0f;
    OEE = 0xff;
}    


/* *********************************************************************
   ***** EP0 vendor request 0x30 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_REQUEST((0x30,,		// get FPGA state
    MEM_COPY1(fpga_checksum,EP0BUF+1,6);    
    OEE = 0xf0;
    if ( (IOE & config_mask_l) == config_mask_l )  {
	EP0BUF[0] = 0; 	 		// FPGA configured 
	IOE = select_mask | 0x0f;
	OEE = 0xff;
    }
    else {
        EP0BUF[0] = 1;			// FPGA unconfigured 
	reset_fpga();			// prepare FPGA for configuration
    }

    EP0BUF[7] = 0;			// not used
    EP0BUF[8] = 0;			// not used

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
	setb	_IOA1			// 2
	add 	a,r1			// 1
	mov 	r1,a                    // 1
	clr	_IOA1                   // 2
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


/* *********************************************************************
   ***** EP0 vendor command 0x34 ***************************************
   ********************************************************************* */
// FIFO write wave form
const char __xdata GPIF_WAVE_DATA_HSFPGA_24MHZ[32] =     
{ 
/* LenBr */ 0x01,     0x88,     0x01,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x02,     0x07,     0x02,     0x02,     0x02,     0x02,     0x02,     0x00,
/* Output*/ 0x20,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x20,
/* LFun  */ 0x00,     0x36,     0x00,     0x00,     0x00,     0x00,     0x00,     0x3F,
};                     

const char __xdata GPIF_WAVE_DATA_HSFPGA_12MHZ[32] =     
{ 
/* LenBr */ 0x02,     0x01,     0x90,     0x01,     0x01,     0x01,     0x01,     0x07,
/* Opcode*/ 0x02,     0x02,     0x07,     0x02,     0x02,     0x02,     0x02,     0x00,
/* Output*/ 0x20,     0x00,     0x00,     0x00,     0x00,     0x00,     0x00,     0x20,
/* LFun  */ 0x00,     0x00,     0x36,     0x00,     0x00,     0x00,     0x00,     0x3F,
};                     


void init_cpld_fpga_configuration() {
    IFCONFIG = bmBIT7 | bmBIT6 | 2;	// Internal source, 48MHz, GPIF
//    IFCONFIG = bmBIT7 | 2;	// Internal source, 30MHz, GPIF

    GPIFREADYCFG = 0x0;
    GPIFCTLCFG = 0; 
    GPIFIDLECS = 0;
    GPIFIDLECTL = 0x20;
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
    EPHS_FPGA_CONF_EPFIFOCFG = bmBIT4;
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
    
    IOA3 = 0; 
}


ADD_EP0_VENDOR_COMMAND((0x34,,			// init fpga configuration
    init_fpga_configuration();

    EPHS_FPGA_CONF_EPCS &= ~bmBIT0;		// clear stall bit

    GPIFABORT = 0xFF;				// abort pendig 
    
    init_cpld_fpga_configuration();
,,));;


/* *********************************************************************
   ***** EP0 vendor command 0x35 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_COMMAND((0x35,,		// finish fpga configuration
    IOA3 = 1;			        // disable GPIF mode of CPLD

    GPIFABORT = 0xFF;
    SYNCDELAY;
    IFCONFIG &= 0xf0;
    SYNCDELAY;

    finish_fpga_configuration();
,,));;

#endif  // HS_FPGA_CONF_EP


/* *********************************************************************
   ***** select_fpga ***************************************************
   ********************************************************************* */
void select_fpga ( BYTE fn ) 
{
    prev_select_num = select_num;
    select_num = fn & 3;
    select_mask = 0x10 << fn;
    config_mask_h = select_mask;

    IOE = 0x0f;
    {
	PRE_FPGA_SELECT
    }
    IOE = select_mask | 0x0f;
}    


/* *********************************************************************
   ***** EP0 vendor request 0x50 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_REQUEST((0x50,,		// Return multi-FPGA information
    EP0BUF[0] = 3;			// 1 FPGA's
    EP0BUF[1] = select_num;		// select methods: any combination
    EP0BUF[2] = 0;			// no parallel configuration support
    EP0BCH = 0;
    EP0BCL = 3;
,,));;

/* *********************************************************************
   ***** EP0 vendor command 0x51 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_COMMAND((0x51,,		// select command
    if ( SETUPDAT[4] == 1 ) {
	config_mask_h = 0xf0;
    }
    else {
	select_fpga( SETUPDAT[2] );
    }
    config_mask_l = config_mask_h >> 4;
,,
    NOP;
));;

#endif  /*ZTEX_FPGA_H*/
