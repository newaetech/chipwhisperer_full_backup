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
    Support for SD cards in SPI mode. 
    This module is not MMC compatible anymore since multi block read/write commands are used.
*/    

#ifeq[MMC_PORT][E]
#define[MMC_bmMODE]
#elifeq[MMC_PORT][A]
#elifeq[MMC_PORT][B]
#elifeq[MMC_PORT][C]
#elifneq[MMC_PORT][D]
#error[Macro `MMC_PORT' is not defined correctly. Valid values are: `A', `B', `C', `D' and `E'.]
#endif

#ifndef[ZTEX_FLASH1_H]
#define[ZTEX_FLASH1_H]

#define[@CAPABILITY_FLASH;]

#ifndef[MMC_PORT]
#error[MMC_PORT not defined]
#endif

#ifndef[MMC_BIT_CS]
#error[MMC_BIT_CS not defined]
#endif

#ifndef[MMC_BIT_DI]
#error[MMC_BIT_DI not defined]
#endif

#ifndef[MMC_BIT_DO]
#error[MMC_BIT_DO not defined]
#endif

#ifndef[MMC_BIT_CLK]
#error[MMC_BIT_CLK not defined]
#endif

#ifndef[MMC__PORT_DO]
#define[MMC__PORT_DO][MMC_PORT]
#endif

#ifdef[MMC_bmMODE]
#ifneq[MMC__PORT_DO][MMC_PORT]
#error[MMC__PORT_DO and MMC_PORT must be equal]
#endif
#endif

#define[MMC_bmCS][bmBITMMC_BIT_CS]
#define[MMC_bmDI][bmBITMMC_BIT_DI]
#define[MMC_bmDO][bmBITMMC_BIT_DO]
#define[MMC_bmCLK][bmBITMMC_BIT_CLK]

#define[MMC_IO][IOMMC_PORT]
#define[MMC__IO_DO][IOMMC__PORT_DO]

#ifndef[MMC_bmMODE]
#define[MMC_CS][IOMMC_PORTMMC_BIT_CS]
#define[MMC_CLK][IOMMC_PORTMMC_BIT_CLK]
#define[MMC_DI][IOMMC_PORTMMC_BIT_DI]
#define[MMC_DO][IOMMC__PORT_DOMMC_BIT_DO]
#endif

// may be redefined if the first sectors are reserved (e.g. for a FPGA bitstream)
#define[FLASH_FIRST_FREE_SECTOR][0]

__xdata BYTE flash_enabled;	// 0	1: enabled, 0:disabled
__xdata WORD flash_sector_size; // 1    sector size
__xdata DWORD flash_sectors;	// 3	number of sectors
__xdata BYTE flash_ec; 	        // 7	error code

__xdata BYTE mmc_last_cmd;	// 0
__xdata BYTE mmc_response;	// 1
__xdata BYTE mmc_version;	// 2
__xdata BYTE mmc_buffer[16];	// 3

__xdata BYTE mmc_ep0_wait;

#define[FLASH_EC_CMD_ERROR][1]
#define[FLASH_EC_TIMEOUT][2]
#define[FLASH_EC_BUSY][3]
#define[FLASH_EC_PENDING][4]
#define[FLASH_EC_READ_ERROR][5]
#define[FLASH_EC_WRITE_ERROR][6]
#define[FLASH_EC_NOTSUPPORTED][7]

/* *********************************************************************
   ***** mmc_clocks ****************************************************
   ********************************************************************* */
// perform c (256 if c=0) clocks
void mmc_clocks (BYTE c) {
	c;					// this avoids stupid warnings
__asm
	mov 	r2,dpl
#ifdef[MMC_bmMODE]
	mov 	a,_MMC_IO
	anl	a, #(~MMC_bmCLK)
	mov	r3, a
	orl	a, #(MMC_bmCLK)
	mov	r4, a
010014$:
	mov	_MMC_IO,r4
	nop
	mov	_MMC_IO,r3
#else
010014$:
        setb	_MMC_CLK
	nop
        clr	_MMC_CLK
#endif
	djnz 	r2,010014$
__endasm;    
}


/* *********************************************************************
   ***** flash_read_byte ***********************************************
   ********************************************************************* */
// read a single byte from the flash
BYTE flash_read_byte() { // uses r2,r3,r4
__asm  
#ifdef[MMC_bmMODE]
	// 8 - 1 + 8*13 + 1 + 6 = 118 clocks
	mov 	a,_MMC_IO
	anl	a, #(~MMC_bmCLK)
	mov	r2, a
	orl	a, #(MMC_bmCLK)
	mov	r3, a
	
	mov	a,_MMC_IO	// 7
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r2

	mov	a,_MMC_IO	// 6
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r2

	mov	a,_MMC_IO	// 5
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r2

	mov	a,_MMC_IO	// 4
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r2
	
	mov	a,_MMC_IO	// 3
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r2
	
	mov	a,_MMC_IO	// 2
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r2

	mov	a,_MMC_IO	// 1
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r2
	
	mov	a,_MMC_IO	// 0
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	xrl	a,#255
	mov	_MMC_IO, r2
	
#else
	// 8*7 + 6 = 62 clocks 
	mov	c,_MMC_DO	// 7
        setb	_MMC_CLK
        rlc 	a		
        clr	_MMC_CLK

        mov	c,_MMC_DO	// 6
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 5
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 4
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 3
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 2
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 1
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 0
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK
#endif
        mov	dpl,a
        ret
__endasm;
	return 0;		// never ever called (just to avoid warnings)
} 

/* *********************************************************************
   ***** flash_read ****************************************************
   ********************************************************************* */
// read len (256 if len=0) bytes from the flash to the buffer
void flash_read(__xdata BYTE *buf, BYTE len) {
	*buf;					// this avoids stupid warnings
	len;					// this too
__asm						// *buf is in dptr, len is in _flash_read_PARM_2
	mov	r2,_flash_read_PARM_2
#ifdef[MMC_bmMODE]
	mov 	a,_MMC_IO
	anl	a, #(~MMC_bmCLK)
	mov	r5, a
	orl	a, #(MMC_bmCLK)
	mov	r3, a
	
010012$:
	// 8 + len*(-1 + 8*13 - 1 + 2 + 9) + 4 = 12 + len*113 clocks
	mov	a,_MMC_IO	// 7
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r5

	mov	a,_MMC_IO	// 6
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r5

	mov	a,_MMC_IO	// 5
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r5

	mov	a,_MMC_IO	// 4
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r5
	
	mov	a,_MMC_IO	// 3
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r5
	
	mov	a,_MMC_IO	// 2
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r5

	mov	a,_MMC_IO	// 1
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	mov 	r4,a
	mov	_MMC_IO, r5
	
	mov	a,_MMC_IO	// 0
	anl	a,#(MMC_bmDO)
	mov	_MMC_IO, r3
	subb 	a,#1
	mov 	a,r4
	rlc	a
	xrl	a,#255
	mov	_MMC_IO, r5
#else
010012$:
	// 2 + len*(8*7 + 9) + 4 = 6 + len*65 clocks
	mov	c,_MMC_DO	// 7
        setb	_MMC_CLK
        rlc 	a		
        clr	_MMC_CLK

        mov	c,_MMC_DO	// 6
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 5
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 4
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 3
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 2
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 1
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK

        mov	c,_MMC_DO	// 0
        setb 	_MMC_CLK
        rlc 	a		
        clr 	_MMC_CLK
#endif
	movx	@dptr,a
	inc	dptr
	djnz 	r2,010012$
__endasm;
} 

/* *********************************************************************
   ***** flash_write_byte **********************************************
   ********************************************************************* */
// send one bytes from buffer buf to the card
void flash_write_byte (BYTE b) {	// b is in dpl
	b;				// this avoids stupid warnings
__asm
#ifdef[MMC_bmMODE]
	// up to 7 + 8*12 + 6 = 109 clocks
	mov 	a,_MMC_IO
	anl	a, #(255 - MMC_bmCLK - MMC_bmDI )
	mov	r3, a
	mov 	a,dpl
	
	rlc	a		// 7
	mov 	_MMC_IO,r3
	jnc 	0100157$
	orl	_MMC_IO, #(MMC_bmDI)
0100157$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 6
	mov	_MMC_IO,r3
	jnc 	0100156$
	orl	_MMC_IO, #(MMC_bmDI)
0100156$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 5
	mov	_MMC_IO,r3
	jnc 	0100155$
	orl	_MMC_IO, #(MMC_bmDI)
0100155$:
	orl 	_MMC_IO, #(MMC_bmCLK)

	rlc	a		// 4
	mov 	_MMC_IO,r3
	jnc 	0100154$
	orl	_MMC_IO, #(MMC_bmDI)
0100154$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 3
	mov	_MMC_IO,r3
	jnc 	0100153$
	orl	_MMC_IO, #(MMC_bmDI)
0100153$:
	orl 	_MMC_IO, #(MMC_bmCLK)

	rlc	a		// 2
	mov 	_MMC_IO,r3
	jnc 	0100152$
	orl	_MMC_IO, #(MMC_bmDI)
0100152$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 1
	mov	_MMC_IO,r3
	jnc 	0100151$
	orl	_MMC_IO, #(MMC_bmDI)
0100151$:
	orl 	_MMC_IO, #(MMC_bmCLK)

	rlc	a		// 0
	mov	_MMC_IO,r3
	jnc 	0100150$
	orl	_MMC_IO, #(MMC_bmDI)
0100150$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	mov 	_MMC_IO,r3
#else
        // 3 + 8*7 + 4 = 63 clocks 
	mov 	a,dpl
	rlc	a		// 7

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 6
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 5
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 4
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 3
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 2
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 1
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 0
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	nop
        clr	_MMC_CLK
#endif	
__endasm;
}  

/* *********************************************************************
   ***** flash_write ***************************************************
   ********************************************************************* */
// write len (256 id len=0) bytes from the buffer to the flash
void flash_write(__xdata BYTE *buf, BYTE len) {
	*buf;					// this avoids stupid warnings
	len;					// this too
__asm						// *buf is in dptr, len is in _flash_read_PARM_2
	mov	r2,_flash_read_PARM_2
#ifdef[MMC_bmMODE]
	// 7 + len*(2 + 8*12 + 7 ) + 6 = 13 + len*105 clocks
	mov 	a,_MMC_IO
	anl	a, #(255 - MMC_bmCLK - MMC_bmDI )
	mov	r3, a
010013$:
	movx	a,@dptr
	
	rlc	a		// 7
	mov 	_MMC_IO,r3
	jnc 	0100167$
	orl	_MMC_IO, #(MMC_bmDI)
0100167$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 6
	mov	_MMC_IO,r3
	jnc 	0100166$
	orl	_MMC_IO, #(MMC_bmDI)
0100166$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 5
	mov	_MMC_IO,r3
	jnc 	0100165$
	orl	_MMC_IO, #(MMC_bmDI)
0100165$:
	orl 	_MMC_IO, #(MMC_bmCLK)

	rlc	a		// 4
	mov 	_MMC_IO,r3
	jnc 	0100164$
	orl	_MMC_IO, #(MMC_bmDI)
0100164$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 3
	mov	_MMC_IO,r3
	jnc 	0100163$
	orl	_MMC_IO, #(MMC_bmDI)
0100163$:
	orl 	_MMC_IO, #(MMC_bmCLK)

	rlc	a		// 2
	mov 	_MMC_IO,r3
	jnc 	0100162$
	orl	_MMC_IO, #(MMC_bmDI)
0100162$:
	orl 	_MMC_IO, #(MMC_bmCLK)
	
	rlc	a		// 1
	mov	_MMC_IO,r3
	jnc 	0100161$
	orl	_MMC_IO, #(MMC_bmDI)
0100161$:
	orl 	_MMC_IO, #(MMC_bmCLK)

	rlc	a		// 0
	mov	_MMC_IO,r3
	jnc 	0100160$
	orl	_MMC_IO, #(MMC_bmDI)
0100160$:
	orl 	_MMC_IO, #(MMC_bmCLK)

	inc	dptr
	djnz 	r2,010013$ 
	
	mov 	_MMC_IO,r3
#else
010013$:
	// 2 + len*(3 + 8*7 - 1 + 7 ) + 4 = 6 + len*65 clocks
	movx	a,@dptr
	rlc	a		// 7

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 6
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 5
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 4
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 3
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 2
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 1
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	rlc	a		// 0
        clr	_MMC_CLK

	mov	_MMC_DI,c
        setb	_MMC_CLK
	inc	dptr
        clr	_MMC_CLK 

	djnz 	r2,010013$ 
#endif
__endasm;
} 

/* *********************************************************************
   ***** mmc_wait_busy *************************************************
   ********************************************************************* */
BYTE mmc_wait_busy () {
    WORD i;
    flash_ec = FLASH_EC_BUSY;
    MMC_IO |= MMC_bmDI;				// avoids that in-data is interpreted as command
    for (i=0; (flash_read_byte()!=255) && i<65535; i++ ) ;
//    for (i=0; (flash_read_byte()==0) && i<65535; i++ ) ;
    if ( MMC__IO_DO & MMC_bmDO ) {
	flash_ec = 0;
	return 0;
    }
    return 1;
}

/* *********************************************************************
   ***** mmc_select ****************************************************
   ********************************************************************* */
/* 
   select the card (CS) and waits if busy
   returns 1 if busy
*/
BYTE mmc_select() {
    MMC_IO |= MMC_bmCS;				// CS = 1
    mmc_clocks(8);				// 8 dummy clocks to finish a previous command
    MMC_IO &= ~MMC_bmCS;			// CS = 0

    return mmc_wait_busy();
}

/* *********************************************************************
   ***** mmc_deselect **************************************************
   ********************************************************************* */
// de-select the card (CS) and waits if busy
void mmc_deselect() {
    MMC_IO |= MMC_bmDI;				// CS = 1
    mmc_clocks(8);				// 8 dummy clocks to finish a previous command
    MMC_IO |= MMC_bmCS; 			// CS = 1
    mmc_clocks(8);				// 8 more dummy clocks
}
   
/* *********************************************************************
   ***** mmc_read_response *********************************************
   ********************************************************************* */
// read the first response byte
BYTE mmc_read_response() {
	MMC_IO |= MMC_bmDI;			// avoids that in-data is interpreted as command
__asm
	mov	r2,#0x255
010010$:
	lcall	_flash_read_byte
	mov	a,dpl
	jnb	acc.7,010011$
	djnz 	r2, 010010$

010011$:
	mov	dptr,#_mmc_response
	movx	@dptr,a
	mov	dpl,a
	ret
__endasm;    
	return 0;				// never ever called, just to avoid stupid warnings
} 

/* *********************************************************************
   ***** mmc_send_cmd **************************************************
   ********************************************************************* */
// send a command   
#define[mmc_send_cmd(][,$1.$2.$3.$4,$5);][{			// send a command, argument=0
    mmc_clocks(8);				// 8 dummy clocks
    mmc_last_cmd = $0;
    mmc_buffer[0] = 64 | ($0 & 127);
    mmc_buffer[1] = $1;
    mmc_buffer[2] = $2;
    mmc_buffer[3] = $3;
    mmc_buffer[4] = $4;
    mmc_buffer[5] = $5 | 1;
    flash_write(mmc_buffer,6);
    mmc_read_response();
}]    

/* *********************************************************************
   ***** mmc_wait_start ************************************************
   ********************************************************************* */
/*
   wait for the start byte
   returns 1 on error
*/   
BYTE mmc_wait_start() {
    WORD to;
    to=0;
    MMC_IO |= MMC_bmDI;				// avoids that in-data is interpreted as command
    while ( flash_read_byte() != 0xfe ) {	// wait for the start byte
	if ( ++to == 0 )   			// 65536 * 8 clocks
	    return 1;
    }
    return 0;
} 

/* *********************************************************************
   ***** flash_read_init ***********************************************
   ********************************************************************* */
/*
   Start the initialization sequence for reading sector s.
   The whole sector must be read.
   returns an error code (FLASH_EC_*). 0 means no error.
*/   
BYTE flash_read_init(DWORD s) {
    if ( (MMC_IO & MMC_bmCS) == 0 ) {
	return FLASH_EC_PENDING;		// we interrupted a pending Flash operation
    }  
    if ( mmc_select() )	{			// select the card
	mmc_deselect();
	return FLASH_EC_BUSY;
    }
    mmc_clocks(8);				// 8 dummy clocks
    mmc_last_cmd = 18;
    mmc_buffer[0] = 18 | 64;
    if ( mmc_version == 0 ) {
	s = s << 1;
	mmc_buffer[1] = s >> 16;
	mmc_buffer[2] = s >> 8;
	mmc_buffer[3] = s;
	mmc_buffer[4] = 0;
    }
    else {
	mmc_buffer[1] = s >> 24;
	mmc_buffer[2] = s >> 16;
	mmc_buffer[3] = s >> 8;
	mmc_buffer[4] = s;
    }
    mmc_buffer[5] = 1;
    flash_write(mmc_buffer,6);
    mmc_read_response();
    if ( mmc_response != 0 ) {
	mmc_deselect();
	return FLASH_EC_CMD_ERROR; 
    }

    if ( mmc_wait_start() ) {			// wait for the start byte
	mmc_deselect();
	return FLASH_EC_TIMEOUT;
    }
    return 0;
}

/* *********************************************************************
   ***** flash_read_next ***********************************************
   ********************************************************************* */
/*
   Initialization sequence for reading the next sector.
   The whole sector must be read.
   returns an error code (FLASH_EC_*). 0 means no error.
*/   
BYTE flash_read_next() {
    mmc_clocks(16);				// 16 CRC clocks
    if ( mmc_wait_start() ) {			// wait for the start byte
	mmc_deselect();
	return FLASH_EC_TIMEOUT;
    }
    return 0;
}

/* *********************************************************************
   ***** flash_read_finish *********************************************
   ********************************************************************* */
/*
   Reads n dummy bytes (the whole sector has to be read out)
   and runs the finalization sequence for a sector read. 
*/   
void flash_read_finish(WORD n) {
    while ( n > 32 ) {
	mmc_clocks(0);			// 256 clocks = 32 dummy bytes
	n-=32;
    }
    if ( n>0) mmc_clocks(n << 3);
    mmc_clocks(16);			// 16 CRC clocks 
    mmc_send_cmd(12, 0.0.0.0, 0);	// stop transmission command, errors are ignored
//    mmc_wait_busy();			// not required here
    mmc_clocks(8);			// 8 dummy clocks
   mmc_deselect();
}


/* *********************************************************************
   ***** flash_write_init **********************************************
   ********************************************************************* */
/*
   Start the initialization sequence for writing sector s
   The whole sector must be written.
   returns an error code (FLASH_EC_*). 0 means no error.
*/   
BYTE flash_write_init(DWORD s) {
    if ( (MMC_IO & MMC_bmCS) == 0 ) {
	return FLASH_EC_PENDING;		// we interrupted a pending Flash operation
    }  
    if ( mmc_select() )	{			// select the card
	mmc_deselect();
	return FLASH_EC_BUSY;
    }
    mmc_clocks(8);				// 8 dummy clocks
    mmc_last_cmd = 25;
    mmc_buffer[0] = 25 | 64;
    if ( mmc_version == 0 ) {
	s = s << 1;
	mmc_buffer[1] = s >> 16;
	mmc_buffer[2] = s >> 8;
	mmc_buffer[3] = s;
	mmc_buffer[4] = 0;
    }
    else {
	mmc_buffer[1] = s >> 24;
	mmc_buffer[2] = s >> 16;
	mmc_buffer[3] = s >> 8;
	mmc_buffer[4] = s;
    }
    mmc_buffer[5] = 1;
    flash_write(mmc_buffer,6);
    mmc_read_response();
    if ( mmc_response != 0 ) {
	mmc_deselect();
	return FLASH_EC_CMD_ERROR; 
    }

    MMC_IO |= MMC_bmDI;				
    mmc_clocks(8);				// send one dummy byte
    flash_write_byte( 0xfc );			// send the start byte
    return 0;
}

/* *********************************************************************
   ***** flash_write_finish_sector *************************************
   ********************************************************************* */
/*
   Writes the rest of the sector (n dummy bytes + CRC, the whole sector has to be written)
   but do not write the finalization procedure. This is done by flash_write_finish 
   or flash_write_next, i.e. these functions must be called after flash_write_finish_sector.
   
   Between flash_write_finish / flash_write_next and flash_write_finish_sector some code
   may be executed because flash_write_finish / flash_write_next start with
   mmc_wait_busy().

   Returns an error code (FLASH_EC_*). 0 means no error.
*/   
BYTE flash_write_finish_sector (WORD n) {
    MMC_IO &= ~MMC_bmDI;			// value of the dummy data is 0
    while ( n > 32 ) {
	mmc_clocks(0);				// 256 clocks = 32 dummy bytes
	n-=32;
    }
    if ( n>0) mmc_clocks(n << 3);

    MMC_IO |= MMC_bmDI;
    mmc_clocks(16);				// 16 CRC clocks

    if ( (flash_read_byte() & 0xf) != 5 ) {	// data response: last four bits must be 5
       flash_ec = FLASH_EC_WRITE_ERROR; 
       mmc_send_cmd(12, 0.0.0.0, 0);		// stop transmission command, errors are ignored
       mmc_clocks(8);				// 8 dummy clocks
       mmc_deselect();
       return FLASH_EC_WRITE_ERROR; 
    }   
    return 0;
}

/* *********************************************************************
   ***** flash_write_finish ********************************************
   ********************************************************************* */
/*
   Stops the write transimssion, see flash_write_finish1.
*/   
void flash_write_finish () {
    mmc_wait_busy();			
    flash_write_byte( 0xfd );			// send the stop byte
    mmc_clocks(8);				// 8 dummy clocks
    mmc_deselect();
}

/* *********************************************************************
   ***** flash_write_next **********************************************
   ********************************************************************* */
/*
   Prepare the nexte sector for write transimssion, see flash_write_finish1.
*/   
void flash_write_next () {
    mmc_wait_busy();			
    flash_write_byte( 0xfc );			// send the stop byte
}

/* *********************************************************************
   ***** flash_init ****************************************************
   ********************************************************************* */
// init the card
void flash_init() {
    BYTE i, j, k;

#ifeq[UFM_1_15X_DETECTION_ENABLED][1]
    if ( is_ufm_1_15x ) {
	flash_enabled = 0;
	flash_ec = FLASH_EC_NOTSUPPORTED;
	return;
    }
#endif    

    flash_enabled = 1;
    flash_sector_size = 512;
    mmc_version = 0;
    
    OEMMC_PORT = OEMMC_PORT | ( MMC_bmCS | MMC_bmDI | MMC_bmCLK );
    OEMMC__PORT_DO = OEMMC__PORT_DO & ~MMC_bmDO;

    MMC_IO |= MMC_bmDI;
    MMC_IO |= MMC_bmCS;
    mmc_clocks(0);				// 256 clocks
    
    mmc_select();				// select te card
    flash_ec = FLASH_EC_BUSY;
	
    mmc_send_cmd(0, 0.0.0.0, 0x95);		// send reset command
    if ( mmc_response & ~1 ) {			// check for errors
	wait(50);				
	mmc_send_cmd(0, 0.0.0.0, 0x95);		// 2nd try
	if ( mmc_response & ~1 ) 		// check for errors 
	    goto mmc_init_cmd_err;
    }
    
    // send cmd8, valid ? 2.0 mode : 1.xx mode
    mmc_send_cmd(8, 0.0.1.0xaa, 0x87);
    if ( ( mmc_response & bmBIT2) == 0 ) {
	if ( (mmc_response & 0xfe) != 0 )  
	    goto mmc_init_cmd_err;
	flash_read(mmc_buffer,4);
	if ( ( (mmc_buffer[2] & 15) != 1) || (mmc_buffer[3] != 0xaa ) ) {
	    flash_ec = FLASH_EC_NOTSUPPORTED;
	    goto mmc_init_err;
	}
	mmc_version=1;
    }
    
    // CMD1 is not recommended, therefore we try ACMD41 first
    k=mmc_version ? 64 : 0;
    mmc_send_cmd(55, 0.0.0.0, 0);
    if ( (mmc_response & 0xfe) == 0 )
	mmc_send_cmd(41, k.0.0.0, 0);
    j = mmc_response & 0xfe;
    
    for ( i=0; mmc_response != 0 && i<255; i++ ) {	// send the init command and wait wait (up to 1s) until ready
	wait(4);
	if ( j ) {
	    mmc_send_cmd(1, k.0.0.0, 0xff);
	}
	else {
	    mmc_send_cmd(55, 0.0.0.0, 0);
	    mmc_send_cmd(41, k.0.0.0, 0);
	}
    }
    if ( mmc_response != 0 ) {			// check for errors
	goto mmc_init_cmd_err;
    } 

    if ( mmc_version ) {
	mmc_send_cmd(58, 0.0.0.0, 0);  		// check the high capacity mode
	if ( mmc_response != 0 ) 		// check for errors
	    goto mmc_init_cmd_err;
	flash_read(mmc_buffer,4);
	if ( (mmc_buffer[0] & 64) == 0 )	// card in standard capacity mode
	    mmc_version = 0;
    }

    mmc_send_cmd(9, 0.0.0.0, 0);		// read the CSD
    if ( mmc_wait_start() ) {
	flash_ec = FLASH_EC_TIMEOUT;
	goto mmc_init_err;
    }
    flash_read(mmc_buffer,16);
    mmc_clocks(16);				// CRC is clocked out

    mmc_buffer[0] &= 192;
    if ( (mmc_buffer[0] & 192) == 0  ) {
	i = (mmc_buffer[5] & 15) + ((mmc_buffer[10] >> 7) | ((mmc_buffer[9] & 3) << 1)) - 7;
	flash_sectors = ((mmc_buffer[8] >> 6) | (mmc_buffer[7] << 2) | ((mmc_buffer[6] & 3) << 10)) + 1;
	flash_sectors = flash_sectors << i;
    }
    else if ( (mmc_buffer[0] & 192) == 64  ) { // todo
	flash_sectors = ( ( ((DWORD)(mmc_buffer[7] & 63) << 16) | (mmc_buffer[8] << 8) | mmc_buffer[9]  ) +1 ) << 10;
    } 
    else {
	flash_ec = FLASH_EC_NOTSUPPORTED;
	goto mmc_init_err;
    }

    flash_ec = 0;
    mmc_deselect();
    
    return;

mmc_init_cmd_err:
    flash_ec = FLASH_EC_CMD_ERROR;
mmc_init_err:
    flash_enabled = 0;
    mmc_deselect();
}


/* *********************************************************************
   ***** EP0 vendor request 0x40 ***************************************
   ********************************************************************* */
// send mmc information structure (card size, error status,  ...) to the host
ADD_EP0_VENDOR_REQUEST((0x40,,
    MEM_COPY1(flash_enabled,EP0BUF,8);
    EP0BCH = 0;
    EP0BCL = 8;
,,
));;

/* *********************************************************************
   ***** EP0 vendor request 0x41 ***************************************
   ********************************************************************* */
void mmc_read_ep0 () { 
    if ( mmc_ep0_wait ) {
	mmc_ep0_wait = 0;
	if ( mmc_wait_start() ) {		// wait for the start byte of the next block
	    flash_ec = FLASH_EC_TIMEOUT;
            mmc_send_cmd(12, 0.0.0.0, 0);	// stop transmission command
    	    mmc_clocks(8);			// 8 dummy clocks
	    mmc_deselect();
	    return;
	}
    }

    flash_read(EP0BUF, ep0_payload_transfer);
    if ( ep0_payload_remaining == 0 ) {
	mmc_clocks(16);				// 16 CRC clocks 
	mmc_send_cmd(12, 0.0.0.0, 0);		// stop transmission command, errors are ignored
//	mmc_wait_busy();			// not required here
	mmc_clocks(8);				// 8 dummy clocks
	mmc_deselect();
    } 
    else if ( (ep0_payload_remaining & 511) == 0 ) {
	mmc_clocks(16);				// 16 CRC clocks
	mmc_ep0_wait = 1;
    }   
}

ADD_EP0_VENDOR_REQUEST((0x41,,			// read integer number of sectotrs
    if ( (MMC_IO & MMC_bmCS) == 0 ) {
	flash_ec = FLASH_EC_PENDING;		// we interrupted a pending Flash operation
	EP0_STALL;
    }  
    if ( mmc_select() )	{			// select the card
	mmc_deselect();
	EP0_STALL;
    }
    mmc_clocks(8);				// 8 dummy clocks
    mmc_last_cmd = 18;
    mmc_buffer[0] = 18 | 64;
    if ( mmc_version == 0 ) {
__asm
    clr c
    mov	dptr,#(_SETUPDAT + 2)
    movx a,@dptr
    mov	dptr,#(_mmc_buffer + 3)
    rlc a
    movx @dptr,a

    mov	dptr,#(_SETUPDAT + 3)
    movx a,@dptr
    mov	dptr,#(_mmc_buffer + 2)
    rlc a
    movx @dptr,a

    mov	dptr,#(_SETUPDAT + 4)
    movx a,@dptr
    mov	dptr,#(_mmc_buffer + 1)
    rlc a
    movx @dptr,a
__endasm;
	mmc_buffer[4] = 0;
    }
    else {
	mmc_buffer[1] = SETUPDAT[5];
	mmc_buffer[2] = SETUPDAT[4];
	mmc_buffer[3] = SETUPDAT[3];
	mmc_buffer[4] = SETUPDAT[2];
    }
    mmc_buffer[5] = 1;
    flash_write(mmc_buffer,6);
    mmc_read_response();
    if ( mmc_response != 0 ) {
	flash_ec = FLASH_EC_CMD_ERROR; 
	mmc_deselect();
	EP0_STALL;
    }

    mmc_ep0_wait = 1;
    mmc_read_ep0();  
    if ( flash_ec != 0 ) {
        EP0_STALL;
    } 
    EP0BCH = 0;
    EP0BCL = ep0_payload_transfer; 
,,
    if ( ep0_payload_transfer != 0 ) {
	flash_ec = 0;
        mmc_read_ep0(); 
        if ( flash_ec != 0 ) {
	    EP0_STALL;
	} 
    } 
    EP0BCH = 0;
    EP0BCL = ep0_payload_transfer;
));;

/* *********************************************************************
   ***** EP0 vendor command 0x42 ***************************************
   ********************************************************************* */
void mmc_send_ep0 () { 
    if ( mmc_ep0_wait ) {
	mmc_ep0_wait = 0;
	mmc_wait_busy();			
        flash_write_byte( 0xfc );			// send the start byte of the next data block
    }
    
    flash_write(EP0BUF, ep0_payload_transfer);
    if ( (ep0_payload_remaining & 511) == 0 ) {
	MMC_IO |= MMC_bmDI;
	mmc_clocks(16);					// 16 CRC clocks

	if ( (flash_read_byte() & 0xf) != 5 ) {		// data response: last four bits must be 5
	   flash_ec = FLASH_EC_WRITE_ERROR; 
	   mmc_send_cmd(12, 0.0.0.0, 0);		// stop transmission command, errors are ignored
	   mmc_clocks(8);				// 8 dummy clocks
	   mmc_deselect();
	} 

	if ( ep0_payload_remaining != 0 ) {
	    mmc_ep0_wait = 1;
    	}
    	else {
	    mmc_wait_busy();			
    	    flash_write_byte( 0xfd );			// send the stop byte
	    mmc_clocks(8);				// 8 dummy clocks
	    mmc_deselect();
	}
    }
}

ADD_EP0_VENDOR_COMMAND((0x42,,			// write integer number of sectors
    if ( (MMC_IO & MMC_bmCS) == 0 ) {
	flash_ec = FLASH_EC_PENDING;		// we interrupted a pending Flash operation
	EP0_STALL;
    } 
    if ( mmc_select() ) {			// select the card
	mmc_deselect();
	EP0_STALL;
    }
    mmc_clocks(8);				// 8 dummy clocks
    mmc_last_cmd = 25;
    mmc_buffer[0] = 25 | 64;
    if ( mmc_version == 0 ) {
__asm
    clr c
    mov	dptr,#(_SETUPDAT + 2)
    movx a,@dptr
    mov	dptr,#(_mmc_buffer + 3)
    rlc a
    movx @dptr,a

    mov	dptr,#(_SETUPDAT + 3)
    movx a,@dptr
    mov	dptr,#(_mmc_buffer + 2)
    rlc a
    movx @dptr,a

    mov	dptr,#(_SETUPDAT + 4)
    movx a,@dptr
    mov	dptr,#(_mmc_buffer + 1)
    rlc a
    movx @dptr,a
__endasm;
	mmc_buffer[4] = 0;
    }
    else {
	mmc_buffer[1] = SETUPDAT[5];
	mmc_buffer[2] = SETUPDAT[4];
	mmc_buffer[3] = SETUPDAT[3];
	mmc_buffer[4] = SETUPDAT[2];
    }

    mmc_buffer[5] = 1;
    flash_write(mmc_buffer,6);
    mmc_read_response();
    if ( mmc_response != 0 ) {
	flash_ec = FLASH_EC_CMD_ERROR; 
	mmc_deselect();
	EP0_STALL;
    }	

    MMC_IO |= MMC_bmDI;				
    mmc_clocks(8);				// send one dummy byte
    flash_write_byte( 0xfc );			// send the start byte
    mmc_ep0_wait = 0;
,,
    if ( ep0_payload_transfer != 0 ) {
	flash_ec = 0;
	mmc_send_ep0();
        if ( flash_ec != 0 ) {
	    EP0_STALL;
	} 
    } 
));;

/* *********************************************************************
   ***** EP0 vendor request 0x43 ***************************************
   ********************************************************************* */
// send detailed MMC status plus debug information
ADD_EP0_VENDOR_REQUEST((0x43,,			// this may interrupt a pending operation
    MEM_COPY1(flash_ec,EP0BUF+2,20);	
    EP0BUF[22] = (MMC__IO_DO & MMC_bmDO) == 0;
    mmc_select();
    mmc_send_cmd(13, 0.0.0.0, 0);
    EP0BUF[0] = mmc_response;
    EP0BUF[1] = flash_read_byte();
    mmc_deselect();
    EP0BCH = 0;
    EP0BCL = 23;
,,
));;

#endif  /*ZTEX_FLASH1_H*/
