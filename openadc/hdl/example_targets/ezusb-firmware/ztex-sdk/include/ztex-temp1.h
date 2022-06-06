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
    Temperature sensor support for USB-FPGA Modules 1.15y
*/    

#ifndef[ZTEX_TEMP1_H]
#define[ZTEX_TEMP1_H]

#define[TEMP1_ENABLED][1]

#define[PCA9691_ADDR][0x90]

void temp1_init() {
    I2CS |= bmBIT7;		// start bit
    i2c_waitStart();
    I2DAT = PCA9691_ADDR;		// select device for writing
    if ( i2c_waitWrite() ) return;
    I2DAT = bmBIT2 | bmBIT6;    // control byte
    if ( i2c_waitWrite() ) return;
    I2DAT = 20;			// DAC output
    I2CS |= bmBIT6;		// stop bit
    if ( i2c_waitStop() ) return;
    INTERFACE_CAPABILITIES[1] |= bmBIT0;
}


ADD_EP0_VENDOR_REQUEST((0x58,,
    I2CS |= bmBIT7;		// start bit
    i2c_waitStart();
    I2DAT = PCA9691_ADDR | 1;	// select device for reading
    i2c_waitWrite();

    EP0BUF[0] = 1;		// protocol #1
    
    EP0BUF[1] = I2DAT;		// dummy read
    i2c_waitRead();
    EP0BUF[1] = I2DAT;		// BYTE 0
    i2c_waitRead();
    EP0BUF[2] = I2DAT;		// BYTE 1
    i2c_waitRead();
    EP0BUF[3] = I2DAT;		// BYTE 2
    i2c_waitRead();
    EP0BUF[4] = I2DAT;		// BYTE 3
    i2c_waitRead();
    EP0BUF[1] = I2DAT;		// BYTE 4
    i2c_waitRead();
    EP0BUF[2] = I2DAT;		// BYTE 5
    i2c_waitRead();
    I2CS |= bmBIT5;		// no ACK
    EP0BUF[3] = I2DAT;		// BYTE 6
    i2c_waitRead();
    I2CS |= bmBIT6;		// stop bit
    EP0BUF[4] = I2DAT;		// BYTE 7
    i2c_waitStop();

    EP0BCH = 0;
    EP0BCL = 5;
,,
));;


#endif  /*ZTEX_TEMP1_H*/
