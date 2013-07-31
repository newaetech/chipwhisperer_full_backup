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
   Various utility routines
*/

#ifndef[ZTEX_UTILS_H]
#define[ZTEX_UTILS_H]

#define[bmBIT0][1]
#define[bmBIT1][2]
#define[bmBIT2][4]
#define[bmBIT3][8]
#define[bmBIT4][16]
#define[bmBIT5][32]
#define[bmBIT6][64]
#define[bmBIT7][128]

#define[NOP;][__asm 
	nop 
    __endasm;
]

#define[MSB(][)][((BYTE)((((unsigned short)($0)) >> 8) & 0xff)) ]
#define[LSB(][)][((BYTE)($0))]
#define[HI(][)][((BYTE)((((unsigned short)($0)) >> 8) & 0xff)) ]
#define[LO(][)][((BYTE)($0))]

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#include[ezregs.h]
#include[ezintavecs.h]
/* *********************************************************************
   ***** global variables **********************************************
   ********************************************************************* */
/* 
    The following two variables are used to control HSNAK bit.
    
    ep0_payload_remaining is set to the length field of of the Setup Data
    structure (in SUDAV_ISR). At the begin of each payload data transfer (in 
    SUDAV_ISR, EP0IN_ISR and EP0OUT_ISR) the amount of payload of the current
    transfer s calculated (<=64 bytes) and subtracted from 
    ep0_payload_remaining. For Vendor Commands HSNAK bit is cleared
    automatically (at the end of EP0OUT_ISR) ifep0_payload_remaining == 0.
    For Vendor Requests HSNAK bit is always cleared at the end of SUDAV_ISR.
*/

__xdata WORD ep0_payload_remaining = 0;		// remaining amount of ep0 payload data (excluding the data of the current transfer)
__xdata BYTE ep0_payload_transfer = 0;		// transfer

/* *********************************************************************
   *********************************************************************
   ***** basic functions ***********************************************
   ********************************************************************* 
   ********************************************************************* */

/* *********************************************************************
   ***** wait **********************************************************
   ********************************************************************* */
void wait(WORD short ms) {	  // wait in ms 
    WORD i,j;
    for (j=0; j<ms; j++) 
	for (i=0; i<1200; i++);
}


/* *********************************************************************
   ***** uwait *********************************************************
   ********************************************************************* */
void uwait(WORD short us) {	  // wait in 10µs steps
    WORD i,j;
    for (j=0; j<us; j++) 
	for (i=0; i<10; i++);
}


/* *********************************************************************
   ***** MEM_COPY ******************************************************
   ********************************************************************* */
// copies 1..256 bytes 
void MEM_COPY1_int() // __naked 
{
	__asm
020001$:
	    mov		_AUTOPTRSETUP,#0x07
	    mov		dptr,#_XAUTODAT1
	    movx	a,@dptr
	    mov		dptr,#_XAUTODAT2
	    movx	@dptr,a
	    djnz	r2, 020001$
	    ret
	__endasm;
}

/* 
    ! no spaces before/after commas allowed !
    
    This will work too: 
	MEM_COPY1(fpga_checksum,EP0BUF+1,6);    
*/	

#define[MEM_COPY1(][,$1,$2);][{
	AUTOPTRL1=LO(&($0));
	AUTOPTRH1=HI(&($0));
	AUTOPTRL2=LO(&($1));
	AUTOPTRH2=HI(&($1));
        __asm
		push	ar2
  		mov	r2,#($2);
		lcall 	_MEM_COPY1_int
		pop	ar2
        __endasm; 
}]


#endif // ZTEX_UTILS_H
