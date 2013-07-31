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
    AVR XMEGA support
*/    

#ifndef[ZTEX_XMEGA_H]
#define[ZTEX_XMEGA_H]

#define[@CAPABILITY_XMEGA;]
#define[XMEGA_ENABLED][1]

#ifeq[PDI_PORT][E]
#define[PDI_bmMODE]
#elifeq[PDI_PORT][A]
#elifeq[PDI_PORT][B]
#elifeq[PDI_PORT][C]
#elifneq[PDI_PORT][D]
#error[Macro `PDI_PORT' is not defined correctly. Valid values are: `A', `B', `C', `D' and `E'.]
#endif

__xdata BYTE xmega_ec;
__xdata WORD xmega_flash_pages;   // in pages
__xdata WORD xmega_eeprom_pages;  // in pages
__xdata BYTE xmega_flash_page_size;   // as power of 2
__xdata BYTE xmega_eeprom_page_size;  // as power of 2

__xdata BYTE nvm_page_type;      	// current page type (0: none; 1:FLASH; 2:EEPROM)
__xdata WORD nvm_current_page;
__xdata DWORD nvm_addr;
__xdata BYTE nvm_boot_base;	// BOOT_BASE >> 16

#define[XMEGA_FLASH_START][0x800000]
#define[XMEGA_EEPROM_START][0x8C0000]
#define[XMEGA_FUSE_START][0x8F0020]

#define[XMEGA_EC_NO_ERROR][0]
#define[XMEGA_EC_PDI_READ_ERROR][1]
#define[XMEGA_EC_NVM_TIMEOUT][2]
#define[XMEGA_EC_INVALID_DEVICE][3]
#define[XMEGA_EC_ADDRESS_ERROR][4]
#define[XMEGA_EC_NVM_BUSY][5]

#define[PDI_IO][IOPDI_PORT]
#define[PDI_CLK][IOPDI_PORTPDI_BIT_CLK]
#define[PDI_DATA][IOPDI_PORTPDI_BIT_DATA]
#define[PDI_bmCLK][(1<<PDI_BIT_CLK)]
#define[PDI_bmDATA][(1<<PDI_BIT_DATA)]

#define[NVM_CMD][0xca]
#define[NVM_CTRLA][0xcb]
#define[NVM_STATUS][0xcf]

/* *********************************************************************
   ***** pdi_send ******************************************************
   ********************************************************************* */
// send one byte to pdi
void pdi_send(BYTE b) {
    b;			// this avoids stupid warnings
    OEPDI_PORT |= bmBITPDI_BIT_DATA;

__asm
#ifdef[PDI_bmMODE]
    mov	a,dpl
    
    anl _PDI_IO, #(~PDI_bmDATA)      	// start bit
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc	a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 0
    jnc 0100200$
    orl _PDI_IO, #(PDI_bmDATA)
0100200$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 1
    jnc 0100201$
    orl _PDI_IO, #(PDI_bmDATA)
0100201$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 2
    jnc 0100202$
    orl _PDI_IO, #(PDI_bmDATA)
0100202$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 3
    jnc 0100203$
    orl _PDI_IO, #(PDI_bmDATA)
0100203$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 4
    jnc 0100204$
    orl _PDI_IO, #(PDI_bmDATA)
0100204$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 5
    jnc 0100205$
    orl _PDI_IO, #(PDI_bmDATA)
0100205$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 6
    jnc 0100206$
    orl _PDI_IO, #(PDI_bmDATA)
0100206$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// 7
    jnc 0100207$
    orl _PDI_IO, #(PDI_bmDATA)
0100207$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    rrc a
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmDATA)      	// parity bit
    jnb _PF,0100208$
    orl _PDI_IO, #(PDI_bmDATA)
0100208$:
    anl _PDI_IO, #(~PDI_bmCLK)	
    orl _PDI_IO, #(PDI_bmCLK)	

    orl _PDI_IO, #(PDI_bmDATA)		// stop bit 1
    anl _PDI_IO, #(~PDI_bmCLK)	
    orl _PDI_IO, #(PDI_bmCLK)	

    anl _PDI_IO, #(~PDI_bmCLK)		// stop bit 1
    orl _PDI_IO, #(PDI_bmCLK)	
#else
    mov	a,dpl
    
    clr _PDI_DATA      // start bit
    clr _PDI_CLK	
    rrc	a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 0
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 1
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 2
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 3
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 4
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 5
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 6
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov _PDI_DATA,c    // 7
    clr _PDI_CLK
    rrc a
    setb _PDI_CLK

    mov c,_PF
    mov _PDI_DATA,c    // parity bit
    clr _PDI_CLK
    setb _PDI_CLK

    setb _PDI_DATA	// stop bit 1
    clr _PDI_CLK
    setb _PDI_CLK

    clr _PDI_CLK	// stop bit 2
    setb _PDI_CLK
#endif[PDI_PORT][A]
__endasm;
}


/* *********************************************************************
   ***** pdi_read ******************************************************
   ********************************************************************* */
// read byte from PDI
BYTE pdi_read() {
    xmega_ec = XMEGA_EC_NO_ERROR;
    OEPDI_PORT &= ~bmBITPDI_BIT_DATA;
    
__asm
#ifdef[PDI_bmMODE]
    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov r1,0
010020$:		// wait for the start bit
    mov	a, _PDI_IO	// start bit
    anl	a, #(PDI_bmDATA)
    jz 010021$
    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    djnz r1,010020$
010021$:

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 0
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    mov r1,a

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 1
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    mov r1,a

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 2
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    mov r1,a

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 3
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    mov r1,a

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 4
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    mov r1,a

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 5
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    mov r1,a

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 6
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    mov r1,a

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// 7
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    rrc a
    xrl a,#255
    mov r1,a
    
    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// parity bit
    anl	a, #(PDI_bmDATA)
    subb a,#1
    mov a,r1
    jnc 010022$
    jb _PF, 010023$		// parity error
    sjmp 010026$
010022$:
    jnb _PF, 010023$		// parity error
010026$:

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// stop bit 1
    anl	a, #(PDI_bmDATA)
    jz 010024$

    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
    mov	a, _PDI_IO		// stop bit 2
    anl	a, #(PDI_bmDATA)
    jz 010024$

    mov	dpl,r1
    ret
010023$:
    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
010024$:
    anl _PDI_IO, #(~PDI_bmCLK)
    orl _PDI_IO, #(PDI_bmCLK)	
010025$:
#else
    clr _PDI_CLK
    setb _PDI_CLK
    mov r1,0
010020$:		// wait for the start bit
    mov c,_PDI_DATA
    jnc 010021$	// start bit
    clr _PDI_CLK
    setb _PDI_CLK
    djnz r1,010020$
010021$:

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 0
    rrc a

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 1
    rrc a

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 2
    rrc a
    
    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 3
    rrc a

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 4
    rrc a

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 5
    rrc a

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 6
    rrc a
    
    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// 7
    rrc a
    
    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// parity bit
    jc 010022$
    jb _PF, 010023$	// parity error
    sjmp 010026$
010022$:
    jnb _PF, 010023$	// parity error
010026$:

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// stop bit 1
    jnc 010024$

    clr _PDI_CLK
    setb _PDI_CLK
    mov c,_PDI_DATA	// stop bit 2
    jnc 010025$

    mov	dpl,a
    ret
010023$:
    clr _PDI_CLK
    setb _PDI_CLK
010024$:
    clr _PDI_CLK
    setb _PDI_CLK
010025$:
#endif
__endasm;
    xmega_ec = XMEGA_EC_PDI_READ_ERROR;
    
    return 0;		// never called (just to avoid warnings)
} 

/* *********************************************************************
   ***** pdi_enable ****************************************************
   ********************************************************************* */
// enable the pdi
void pdi_enable() {	// enter PDI mode
    BYTE i;
    PDI_IO &= ~PDI_bmDATA;
    PDI_IO |= PDI_bmCLK;
    OEPDI_PORT |= bmBITPDI_BIT_DATA | bmBITPDI_BIT_CLK;
    
    PDI_IO |= PDI_bmDATA;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;

    for (i=0; i<16; i++) {
        PDI_IO &= ~PDI_bmCLK;
        PDI_IO |= PDI_bmCLK;
    }

    pdi_send(0xc2);
    pdi_send(2);
} 

/* *********************************************************************
   ***** nvm_status ****************************************************
   ********************************************************************* */
// read the NVM_STATUS register
BYTE nvm_status () {
    pdi_send(0x0c);
    pdi_send(NVM_STATUS);
    pdi_send(0x01);
    pdi_send(0x00);
    pdi_send(0x01);
    return pdi_read();
}


/* *********************************************************************
   ***** nvm_wait_busy *************************************************
   ********************************************************************* */
// wait up to 40ms if NVM is busy
void nvm_wait_busy() {	
    WORD w;
    // wait if nvm controller is busy
    for ( w=0; w<40000; w++ ) {
	if ( ( (nvm_status() & bmBIT7) == 0) || xmega_ec!=0 ) 
	    return;
    }
    xmega_ec = XMEGA_EC_NVM_BUSY;
}

/* *********************************************************************
   ***** nvm_init ******************************************************
   ********************************************************************* */
// enable the NVM access via PDI
void nvm_init() {	// activate the NVM controller
    BYTE b,c,d;
    pdi_enable();
    
    pdi_send(0xc1);	// reset
    pdi_send(0x59); 

    pdi_send(0xe0);	// send key
    pdi_send(0xff); 
    pdi_send(0x88);
    pdi_send(0xd8);
    pdi_send(0xcd);
    pdi_send(0x45);
    pdi_send(0xab);
    pdi_send(0x89);
    pdi_send(0x12);

    // wait until NVM is available
    b = 0; c = 0;
    do {
	pdi_send(0x80);
	c = pdi_read();
	d = ((c & bmBIT1) == 0) && (xmega_ec==0);
	b+=1;
    } while ( (b!=0) && d );
    if ( d ) 
	xmega_ec = XMEGA_EC_NVM_TIMEOUT;
} 

/* *********************************************************************
   ***** nvm_register_write*********************************************
   ********************************************************************* */
// write to NVM register
void nvm_register_write(BYTE offs, BYTE b) {
    pdi_send(0x4c);	// direct write
    pdi_send(offs);
    pdi_send(0x01);
    pdi_send(0x00);
    pdi_send(0x01);
    pdi_send(b);
}

/* *********************************************************************
   ***** nvm_register_write ********************************************
   ********************************************************************* */
// enable the NVM access via PDI
void nvm_done() {	// deactivate the NVM controller
    pdi_send(0xc1);	// disable reset
    pdi_send(0);

    nvm_register_write(NVM_CMD,0);	// VNM NOP
    
    PDI_IO &= ~PDI_bmDATA;
    PDI_IO |= PDI_bmCLK;
}

void nvm_done2() {	// deactivate the NVM controller
    nvm_register_write(NVM_CMD,0);	// VNM NOP

    PDI_IO &= ~PDI_bmDATA;
    PDI_IO |= PDI_bmCLK;
}


/* *********************************************************************
   ***** nvm_read ******************************************************
   ********************************************************************* */
// read NVM
BYTE nvm_read ( __xdata BYTE *buf, DWORD addr, BYTE length ) { 
    BYTE ol = length;
    
    if ( length == 0 )
	return 0;
    
    nvm_register_write(NVM_CMD,0x43);    // load NVM read command into NVM CMD register

    pdi_send( 0x6b );		 // set ptr
    pdi_send( addr & 255 );	
    pdi_send( (addr>>8) & 255 );
    pdi_send( (addr>>16) & 255 );
    pdi_send( (addr>>24) & 255 );

    pdi_send( 0xa0 );		// repeat cmd
    pdi_send( length-1 );	
    pdi_send( 0x24 );		// read one byte cmd
    while ( (length!=0) && (xmega_ec==0) ) {
	*buf = pdi_read(); buf++;
	length-= 1;
    } 

    return ol-length;
}

/* *********************************************************************
   ***** nvm_page_load *************************************************
   ********************************************************************* */
// write data into NVM page buffer. Flash: cmd=0x23 , EEPROM: cmd=0x33
#define[nvm_flash_page_load(][);][nvm_page_load(0x23, $0);]
#define[nvm_eeprom_page_load(][);][nvm_page_load(0x33, $0);]

void nvm_page_load ( BYTE cmd, __xdata BYTE *buf, DWORD addr, BYTE length ) { 
    BYTE ol = length;
    
    if ( length == 0 )
	return;
    
    nvm_register_write(NVM_CMD,cmd);    // load "Load Page Buffer" command into CMD register NVM CMD register

    pdi_send( 0x6b );		 // set ptr
    pdi_send( addr & 255 );	
    pdi_send( (addr>>8) & 255 );
    pdi_send( (addr>>16) & 255 );
    pdi_send( (addr>>24) & 255 );

    pdi_send( 0xa0 );		// repeat cmd
    pdi_send( length-1 );	
    pdi_send( 0x64 );		// write one byte cmd
    while ( length!=0 ) {
	pdi_send(*buf); buf++;
	length-= 1;
    } 
}


/* *********************************************************************
   ***** nvm_page_write ************************************************
   ********************************************************************* */
// erase and write nvm page 
#define[nvm_flash_page_write(][);][nvm_page_write(((BYTE)((($0)>>16) & 255)) < nvm_boot_base ? 0x25 : 0x2d, $0);]
#define[nvm_eeprom_page_write(][);][nvm_page_write(0x35, $0);]
void nvm_page_write (BYTE cmd, DWORD addr ) { 
    nvm_register_write(NVM_CMD, cmd);   // load "erase and write page" command into NVM CMD register
    
    pdi_send( 0x4c   );		 	// direct write
    pdi_send( addr & 255 );	
    pdi_send( (addr>>8) & 255 );
    pdi_send( (addr>>16) & 255 );
    pdi_send( (addr>>24) & 255 );
    pdi_send( 0 );

    nvm_wait_busy();
}

/* *********************************************************************
   ***** nvm_fuse_write ************************************************
   ********************************************************************* */
// write fuse
void nvm_fuse_write (BYTE idx, BYTE val) { 
    nvm_register_write(NVM_CMD, 0x4c);   // load "write fuse" command into NVM CMD register
    
    pdi_send( 0x4c   );		 	// direct write
    pdi_send( 0x20 | idx );	
    pdi_send( 0 );
    pdi_send( 0x8f );
    pdi_send( 0 );
    pdi_send( val );

  nvm_wait_busy();
}

/* *********************************************************************
   ***** xmega_reset ***************************************************
   ********************************************************************* */
// reset the xmega
void xmega_reset () {
    OEPDI_PORT |= bmBITPDI_BIT_CLK;
    uwait(20);			// disable
    PDI_IO &= ~PDI_bmCLK;	// enable reset

    uwait(5);
    PDI_IO |= PDI_bmCLK;
}

/* *********************************************************************
   ***** xmega_init ****************************************************
   ********************************************************************* */
void xmega_init() {
    __xdata BYTE xmega_id[3];
    
    nvm_page_type = 0;
    xmega_flash_pages = 0;
    xmega_eeprom_pages = 64;
    xmega_eeprom_page_size = 5;
    xmega_flash_page_size = 9;
    

    nvm_init();    
    if ( xmega_ec != 0 ) 
	return;
    nvm_read(xmega_id, 0x01000090, 3);
    if ( xmega_ec != 0 ) 
	return;
    nvm_done();    
    
    if ( xmega_id[0] != 0x1e ) {
	xmega_ec = XMEGA_EC_INVALID_DEVICE;
	return;
    }

/*  if (  xmega_id[1] == 0x96 && xmega_id[2] == 0x4e ) {	// 64A1 currently not supported
	mega_flash_pages = 272;	
	mega_flash_page_size = 8;
	nvm_boot_base = 0x81;
    } */
    if (  xmega_id[1] == 0x97 && xmega_id[2] == 0x4c ) {	// 128A1
	xmega_flash_pages = 272;	
	nvm_boot_base = 0x82;
    }
    else if (  xmega_id[1] == 0x97 && xmega_id[2] == 0x4e ) {	// 192A1
	xmega_flash_pages = 400;	
	nvm_boot_base = 0x83;
    }
    else if (  xmega_id[1] == 0x98 && xmega_id[2] == 0x46 ) {	// 256A1
	xmega_flash_pages = 528;
	xmega_eeprom_pages = 128; 
	nvm_boot_base = 0x84;
    }
    else {
	xmega_ec = XMEGA_EC_INVALID_DEVICE;
    }
}



/* *********************************************************************
   ***** EP0 vendor request 0x48 ***************************************
   ********************************************************************* */
// send xmega error and status information to the host
ADD_EP0_VENDOR_REQUEST((0x48,,
    MEM_COPY1(xmega_ec,EP0BUF,7);
    EP0BCH = 0;
    EP0BCL = 7;
,,
    NOP;
));;


/* *********************************************************************
   ***** EP0 vendor request 0x49 ***************************************
   ********************************************************************* */
// reset xmega   
ADD_EP0_VENDOR_COMMAND((0x49,, 				
    xmega_reset();
,,
    NOP;
));; 


/* *********************************************************************
   ***** EP0 vendor requests 0x4A, 0x4B, 0x4C,0x4D *********************
   ********************************************************************* */
// read from nvm using: pdi address space (0x4a), flash base (0x4b), EEPROM base (0x4c), FUSE base (0x4d) 
BYTE nvm_read_ep0 () { 
    BYTE i, b;
    b = ep0_payload_transfer;
    if ( b == 0 )
	return 0;

    nvm_init();    
    i = xmega_ec==0 ? nvm_read(EP0BUF, nvm_addr, b) : 0;
    nvm_done();
    
    nvm_addr += b;
    return i;
} 

ADD_EP0_VENDOR_REQUEST((0x4a: case 0x4b: case 0x4c: case 0x4d,, 		// read from NVM
    nvm_addr = *(DWORD*)(&SETUPDAT[2]);
    if ( bRequest == 0x4b ) {							// read from Flash
	nvm_addr += XMEGA_FLASH_START;
    }
    else if ( bRequest == 0x4c ) {						// read from EEPROM
	nvm_addr += XMEGA_EEPROM_START;
    }
    else if ( bRequest == 0x4d ) {						// read FUSES
	nvm_addr += XMEGA_FUSE_START;
    }
    EP0BCH = 0;
    EP0BCL = nvm_read_ep0(); 
,,
    EP0BCH = 0;
    EP0BCL = nvm_read_ep0(); 
));; 


/* *********************************************************************
   ***** EP0 vendor command 0x4B ***************************************
   ********************************************************************* */
// write exactly one flash page
void nvm_flash_write_ep0 () { 
    BYTE b;
    b = ep0_payload_transfer;

    nvm_init();    
    if ( xmega_ec != 0 )
	return;
	
    nvm_flash_page_load(EP0BUF, nvm_addr, b);
    
    if ( ep0_payload_remaining == 0 ) {
	nvm_flash_page_write( nvm_addr );
        nvm_done();
    }
    else {
        nvm_done2();
    }
	
    nvm_addr += b;
} 

ADD_EP0_VENDOR_COMMAND((0x4B,,
    nvm_addr = *(DWORD*)(&SETUPDAT[2]);
    nvm_addr += XMEGA_FLASH_START;
    if ( SETUPDAT[2]!=0 || (SETUPDAT[3] & 1)!=0 || SETUPDAT[7]!=2 || SETUPDAT[6]!=0 || (*(WORD*)(&SETUPDAT[3])>>1)>=xmega_flash_pages )  // only 512 byte pages supported
    {
	xmega_ec = XMEGA_EC_ADDRESS_ERROR;
	EP0_STALL;
    }
,,
    if ( ep0_payload_transfer != 0 ) {
	xmega_ec = 0;
	nvm_flash_write_ep0();
        if ( xmega_ec != 0 ) {
	    EP0_STALL;
	} 
    } 
));; 

/* *********************************************************************
   ***** EP0 vendor command 0x4C ***************************************
   ********************************************************************* */
// write exactly one EEPROM page
ADD_EP0_VENDOR_COMMAND((0x4C,,
    nvm_addr = *(DWORD*)(&SETUPDAT[2]);
    nvm_addr += XMEGA_EEPROM_START;
    if ( ( SETUPDAT[2] & 31 ) != 0 || SETUPDAT[7]!=0 || SETUPDAT[6]!=32 || (*(WORD*)(&SETUPDAT[2])>>5)>=xmega_eeprom_pages )  // only 32 byte pages supported
    {
	xmega_ec = XMEGA_EC_ADDRESS_ERROR;
	EP0_STALL;
    }

    nvm_init();    
    if ( xmega_ec != 0 )
	return;

    nvm_eeprom_page_load(EP0BUF, nvm_addr, 32);
    nvm_eeprom_page_write( nvm_addr );
    nvm_done();
,,
));; 

/* *********************************************************************
   ***** EP0 vendor command 0x4D ***************************************
   ********************************************************************* */
// write one fuse
ADD_EP0_VENDOR_COMMAND((0x4D,,
    if ( SETUPDAT[6]!=0 || SETUPDAT[7]!=0 || SETUPDAT[5]!=0 || SETUPDAT[4]>7 )
    {
	xmega_ec = XMEGA_EC_ADDRESS_ERROR;
	EP0_STALL;
    }

    nvm_init();    
    nvm_fuse_write(SETUPDAT[4], SETUPDAT[2]);
    nvm_done();
,,
    NOP;
));; 


#endif  /*ZTEX_XMEGA_H*/
