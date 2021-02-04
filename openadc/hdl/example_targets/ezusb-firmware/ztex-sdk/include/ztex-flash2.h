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
    supports AT45D* Flash on Port E
*/    

#ifndef[ZTEX_FLASH2_H]
#define[ZTEX_FLASH2_H]

#define[@CAPABILITY_FLASH;]

#define[FLASH_SI_BIT][6]
#define[FLASH_SO_BIT][4]
#define[FLASH_CLK_BIT][5]
#ifeq[PRODUCT_IS][UFM-1_1]
#define[FLASH_CS_BIT][3]
//#warning[FLASH_CS_BIT=3]
#else
#define[FLASH_CS_BIT][7]
//#warning[FLASH_CS_BIT=7]
#endif

__xdata BYTE flash_buffer = 1;          // current buffer 1
__xdata BYTE flash_bufferModified = 0;  // 1 indicates that the current flash page needs to be reprogrammeded
__xdata WORD flash_page = 0xffff;       // current flash page
__xdata WORD flash_nextPage = 0xffff;   // next flash page (usually flashPage+1)
__xdata WORD flash_maxPage = 0;         // next flash page (usually flashPage+1)
__xdata WORD flash_pageSize = 528;      // page size (512 or 528)
__xdata BYTE flash_register[4];         // used to store register content 

/* *********************************************************************
   ***** flash_setPage *************************************************
   ********************************************************************* */
/* set the current page, i.e. executes the following steps
    1. wait if busy
    2. read the desired page from the flash array into the new buffer
    3. write the old buffer to the flash array (if modified)
*/   
static void flash_setPage(WORD page) 
{
    flash_nextPage = page + 1;
}

/* *********************************************************************
   ***** flash_write ***************************************************
   ********************************************************************* */
/* writes <bytes> bytes at address <addr> to the current buffer starting at <offs>.
   If <setNextPage> = 1 the next page (defined by <flash_nextPage> is set automatically
*/
static void flash_write(WORD addr, WORD bytes, WORD offs, BYTE setNextPage) 
{
    addr = 0;
    bytes = 0;
    offs = 0;
    setNextPage = 0;
}

/* *********************************************************************
   ***** flash_write ***************************************************
   ********************************************************************* */
static void flash_readRegister(BYTE cmd) 
{
    BYTE oe,b,i,j;
    
    b = OEE & ( ~( (1 << FLASH_SO_BIT) | (1 << FLASH_SI_BIT) | (1 << FLASH_CLK_BIT) | (1 << FLASH_CS_BIT) ) );
    oe = IOE & b;
    IOE = oe | (1 << FLASH_CS_BIT);   // CS = 1
    OEE = b | (1 << FLASH_SI_BIT) | (1 << FLASH_CLK_BIT) | (1 << FLASH_CS_BIT);
    IOE = oe;			      // CS = 0

    // CLK=0
    for ( i=0; i<8; i++ ) {
	IOE = b = oe | ((cmd & 128) >> (7-FLASH_SI_BIT)); // CLK = 0, SI=x
	IOE = b | (1 << FLASH_CLK_BIT);	 	     	  // CLK = 1, SI=x
	cmd <<= 1;
    }
    
    for ( j=0; j<4; j++ ) {
        // CLK = 1
	b = 0;
	for ( i=0; i<8; i++ ) {
	    IOE = oe;		    	       // CLK = 0, SO=x
	    b = (b << 1) | ((IOE >> FLASH_SO_BIT) & 1);
	    IOE = oe | (1 << FLASH_CLK_BIT);  // CLK = 1
	}
	flash_register[j]=b;
    }

    IOE = oe | (1 << FLASH_CS_BIT);   // CS = 1
}


/* *********************************************************************
   ***** EP0 vendor request 0x40 ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_REQUEST((0x40,,
    flash_readRegister(SETUPDAT[2]);
    EP0BUF[0] = flash_register[0];
    EP0BUF[1] = flash_register[1];
    EP0BUF[2] = flash_register[2];
    EP0BUF[3] = flash_register[3];
    EP0BCH = 0;
    EP0BCL = 4;
,,));;

#endif  /*ZTEX_FLASH2_H*/
