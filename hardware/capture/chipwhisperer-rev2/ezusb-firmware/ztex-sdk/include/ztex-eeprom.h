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
    EEPROM support an some I2C helper routines
*/    

#ifndef[ZTEX_EEPROM_H]
#define[ZTEX_EEPROM_H]

#define[@CAPABILITY_EEPROM;]
#define[EEPROM_ADDR][0xA2]

/* *********************************************************************
   ***** global variables **********************************************
   ********************************************************************* */
__xdata WORD eeprom_addr;
__xdata WORD eeprom_write_bytes;
__xdata BYTE eeprom_write_checksum;


/* *********************************************************************
   ***** i2c_waitWrite *************************************************
   ********************************************************************* */
/* Do the necessary steps after writing I2DAT register. Returns 1 on error. */
BYTE i2c_waitWrite()
{
    unsigned char i2csbuf,toc;
    for ( toc=0; toc<255 && !(I2CS & bmBIT0); toc++ );
    i2csbuf = I2CS;
    if ( (i2csbuf & bmBIT2) || (!(i2csbuf & bmBIT1)) ) {
        I2CS |= bmBIT6;
	return 1;
    }
    return 0;
}

/* *********************************************************************
   ***** i2c_waitRead **************************************************
   ********************************************************************* */
/* Do the necessary steps after reading I2DAT register. Returns 1 on error. */
BYTE i2c_waitRead(void)
{
    unsigned char i2csbuf, toc;
    for ( toc=0; toc<255 && !(I2CS & bmBIT0); toc++ );
    i2csbuf = I2CS;
    if (i2csbuf & bmBIT2) {
        I2CS |= bmBIT6;
	return 1;
    }
    return 0;
}

/* *********************************************************************
   ***** i2c_waitStart *************************************************
   ********************************************************************* */
/* Do the necessary steps after start bit. Returns 1 on error. */
BYTE i2c_waitStart()
{
    BYTE toc;
    for ( toc=0; toc<255; toc++ ) {
	if ( ! (I2CS & bmBIT2) )
	    return 0;
    }
    return 1;
}

/* *********************************************************************
   ***** i2c_waitStop **************************************************
   ********************************************************************* */
/* Do the necessary steps after stop bit. Returns 1 on error. */
BYTE i2c_waitStop()
{
    BYTE toc;
    for ( toc=0; toc<255; toc++ ) {
	if ( ! (I2CS & bmBIT6) )
	    return 0;
    }
    return 1;
}

/* *********************************************************************
   ***** eeprom_select *************************************************
   ********************************************************************* */
/* Select the EEPROM device, i.e. send the control Byte. 
   <to> specifies the time to wait in 0.1ms steps if the EEPROM is busy (during a write cycle).
   if <stop>=0 no sop bit is sent. Returns 1 on error or if EEPROM is busy. */
BYTE eeprom_select (BYTE addr, BYTE to, BYTE stop ) {
    BYTE toc = 0;
eeprom_select_start:
    I2CS |= bmBIT7;		// start bit
    i2c_waitStart();
    I2DAT = addr;		// select device for writing
    if ( ! i2c_waitWrite() ) {
        if ( stop ) {
            I2CS |= bmBIT6;
	    i2c_waitStop();
    	}
	return 0;
    }
    else if (toc<to) {
	uwait(10);
	goto eeprom_select_start;
    }
    if ( stop ) {
	I2CS |= bmBIT6;
    }
    return 1;
}

/* *********************************************************************
   ***** eeprom_read ***************************************************
   ********************************************************************* */
/* Reads <length> bytes from EEPROM address <addr> and write them to buf. 
   Returns the number of bytes read. This number is 0 during a write cycle. */
BYTE eeprom_read ( __xdata BYTE *buf, WORD addr, BYTE length ) { 
    BYTE bytes = 0,i;
    
    if ( length == 0 ) 
	return 0;
    
    if ( eeprom_select(EEPROM_ADDR, 100,0) ) 
	goto eeprom_read_end;
    
    I2DAT = HI(addr);		// write address
    if ( i2c_waitWrite() ) goto eeprom_read_end;
    I2DAT = LO(addr);		// write address
    if ( i2c_waitWrite() ) goto eeprom_read_end;
    I2CS |= bmBIT6;
    i2c_waitStop();

    I2CS |= bmBIT7;		// start bit
    i2c_waitStart();
    I2DAT = EEPROM_ADDR | 1;	// select device for reading
    if ( i2c_waitWrite() ) goto eeprom_read_end;

    *buf = I2DAT;		// dummy read
    if ( i2c_waitRead()) goto eeprom_read_end; 
    for (; bytes<length; bytes++ ) {
	*buf = I2DAT;		// read data
	buf++;
	if ( i2c_waitRead()) goto eeprom_read_end; 
    }

    I2CS |= bmBIT5;		// no ACK
    i = I2DAT;			// dummy read
    if ( i2c_waitRead()) goto eeprom_read_end; 

    I2CS |= bmBIT6;		// stop bit
    i = I2DAT;			// dummy read
    i2c_waitStop();

eeprom_read_end:
    return bytes;
}

/* *********************************************************************
   ***** eeprom_write **************************************************
   ********************************************************************* */
/* Writes <length> bytes from buf to EEPROM address <addr>.
   <length> must be smaller or equal than 8. Returns the number of bytes
   read. This number is 0 during a write cycle. */
BYTE eeprom_write ( __xdata BYTE *buf, WORD addr, BYTE length ) {
    BYTE bytes = 0;

    if ( eeprom_select(EEPROM_ADDR, 100,0) ) 
	goto eeprom_write_end;
    
    I2DAT = HI(addr);          	// write address
    if ( i2c_waitWrite() ) goto eeprom_write_end;
    I2DAT = LO(addr);          	// write address
    if ( i2c_waitWrite() ) goto eeprom_write_end;
    
    for (; bytes<length; bytes++ ) {
	I2DAT = *buf;         	// write data 
	eeprom_write_checksum += *buf;
	buf++;
	eeprom_write_bytes+=1;
	if ( i2c_waitWrite() ) goto eeprom_write_end;
    }
    I2CS |= bmBIT6;		// stop bit
    i2c_waitStop();
	
eeprom_write_end:
    return bytes;
}

/* *********************************************************************
   ***** EP0 vendor request 0x38 ***************************************
   ********************************************************************* */
BYTE eeprom_read_ep0 () { 
    BYTE i, b;
    b = ep0_payload_transfer;
    i = eeprom_read(EP0BUF, eeprom_addr, b);
    eeprom_addr += b;
    return i;
}

ADD_EP0_VENDOR_REQUEST((0x38,, 				// read from EEPROM
    eeprom_addr =  (SETUPDAT[3] << 8) | SETUPDAT[2];	// Address
    EP0BCH = 0;
    EP0BCL = eeprom_read_ep0(); 
,,
    EP0BCH = 0;
    EP0BCL = eeprom_read_ep0(); 
));; 


/* *********************************************************************
   ***** EP0 vendor command 0x39 ***************************************
   ********************************************************************* */
void eeprom_write_ep0 ( BYTE length ) { 	
    eeprom_write(EP0BUF, eeprom_addr, length);
    eeprom_addr += length;
}

ADD_EP0_VENDOR_COMMAND((0x39,,				// write to EEPROM
    eeprom_write_checksum = 0;
    eeprom_write_bytes = 0;
    eeprom_addr =  ( SETUPDAT[3] << 8) | SETUPDAT[2];	// Address
,,
    eeprom_write_ep0(EP0BCL);
    ));; 

/* *********************************************************************
   ***** EP0 vendor request 0x3A ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_REQUEST((0x3A,,				// EEPROM state
    EP0BUF[0] = LSB(eeprom_write_bytes);
    EP0BUF[1] = MSB(eeprom_write_bytes);
    EP0BUF[2] = eeprom_write_checksum;
    EP0BUF[3] = eeprom_select(EEPROM_ADDR,0,1);		// 1 means busy or error
    EP0BCH = 0;
    EP0BCL = 4;
,,));;


#ifdef[MAC_EEPROM_ENABLED]
#define[EEPROM_MAC_ADDR][0xA6]
#define[@CAPABILITY_MAC_EEPROM;]

__xdata BYTE mac_eeprom_addr;

/* *********************************************************************
   ***** mac_eeprom_read ***********************************************
   ********************************************************************* */
/* Reads <length> bytes from EEPROM address <addr> and write them to buf. 
   Returns the number of bytes read. This number is 0 during a write cycle. */
BYTE mac_eeprom_read ( __xdata BYTE *buf, BYTE addr, BYTE length ) { 
    BYTE bytes = 0,i;
    
    if ( length == 0 ) 
	return 0;
    
    if ( eeprom_select(EEPROM_MAC_ADDR, 100,0) ) 
	goto mac_eeprom_read_end;
    
    I2DAT = addr;		// write address
    if ( i2c_waitWrite() ) goto mac_eeprom_read_end;
    I2CS |= bmBIT6;
    i2c_waitStop();


    I2CS |= bmBIT7;		// start bit
    i2c_waitStart();
    I2DAT = EEPROM_MAC_ADDR | 1;  // select device for reading
    if ( i2c_waitWrite() ) goto mac_eeprom_read_end;

    *buf = I2DAT;		// dummy read
    if ( i2c_waitRead()) goto mac_eeprom_read_end; 
    for (; bytes<length; bytes++ ) {
	*buf = I2DAT;		// read data
	buf++;
	if ( i2c_waitRead()) goto mac_eeprom_read_end; 
    }

    I2CS |= bmBIT5;		// no ACK
    i = I2DAT;			// dummy read
    if ( i2c_waitRead()) goto mac_eeprom_read_end; 

    I2CS |= bmBIT6;		// stop bit
    i = I2DAT;			// dummy read
    i2c_waitStop();

mac_eeprom_read_end:
    return bytes;
}

/* *********************************************************************
   ***** mac_eeprom_write **********************************************
   ********************************************************************* */
/* Writes <length> bytes from buf to and write them EEPROM address <addr>.
   <length> must be smaller or equal than 8. Returns the number of bytes
   read. This number is 0 during a write cycle. */
BYTE mac_eeprom_write ( __xdata BYTE *buf, BYTE addr, BYTE length ) {
    BYTE bytes = 0;

    if ( eeprom_select(EEPROM_MAC_ADDR, 100,0) ) 
	goto mac_eeprom_write_end;
    
    I2DAT = addr;          	// write address
    if ( i2c_waitWrite() ) goto mac_eeprom_write_end;
    
    for (; bytes<length; bytes++ ) {
	I2DAT = *buf;         	// write data 
	buf++;
	if ( i2c_waitWrite() ) goto mac_eeprom_write_end;
    }
    I2CS |= bmBIT6;		// stop bit
    i2c_waitStop();
	
mac_eeprom_write_end:
    return bytes;
}

/* *********************************************************************
   ***** EP0 vendor request 0x3B ***************************************
   ********************************************************************* */
BYTE mac_eeprom_read_ep0 () { 
    BYTE i, b;
    b = ep0_payload_transfer;
    i = mac_eeprom_read(EP0BUF, mac_eeprom_addr, b);
    return i;
}

ADD_EP0_VENDOR_REQUEST((0x3B,, 				// read from EEPROM
    mac_eeprom_addr =  SETUPDAT[2];			// Address
    EP0BCH = 0;
    EP0BCL = mac_eeprom_read_ep0(); 
,,
    EP0BCH = 0;
    EP0BCL = mac_eeprom_read_ep0(); 
));; 


/* *********************************************************************
   ***** EP0 vendor command 0x3C ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_COMMAND((0x3C,,				// write to EEPROM
    mac_eeprom_addr =  SETUPDAT[2];			// address
,,
    mac_eeprom_write(EP0BUF, mac_eeprom_addr, EP0BCL);
    ));; 

/* *********************************************************************
   ***** EP0 vendor request 0x3D ***************************************
   ********************************************************************* */
ADD_EP0_VENDOR_REQUEST((0x3D,,				// EEPROM state
    EP0BUF[0] = eeprom_select(EEPROM_MAC_ADDR,0,1);	// 1 means busy or error
    EP0BCH = 0;
    EP0BCL = 1;
,,));;


#endif // MAC_EEPROM_ENABLED

#endif  /*ZTEX_EEPROM_H*/
