/*!
   Java host software API of ZTEX EZ-USB FX2 SDK
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
    Reads an ihx file with ZTEX descriptor 1
*/
package ztex;

import java.io.*;
import java.util.*;

/**
  * Represents a firmware image with ZTEX descriptor 1 loaded from an ihx (Intel Hex format) file. <br>
  * The ZTEX descriptor is usually located at the position 0x6x of the firmware image. <br>
  * A description of the ZTEX descriptor 1 can be found in {@link ZtexDevice1}.
  * @see ZtexDevice1
  * @see Ztex1
*/
public class ZtexIhxFile1 extends IhxFile {
    private static final int defaultZtexDescriptorOffs = 0x6c;

    private int ztexDescriptorOffs = defaultZtexDescriptorOffs;

    private byte productId[] = { 0,0,0,0 };	// product ID from the ZTEX descriptor, not the USB product ID
    private byte fwVersion = 0;
    private byte interfaceVersion = 0;
    private byte interfaceCapabilities[] = { 0,0,0,0, 0,0 };
    private byte moduleReserved[] = { 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    private char snString[] = new char[10];

// ******* ZtexIhxFile1 ********************************************************
/**
  * Constructs an instance from a given file name and descriptor position.<br>
  * This method can also read system resources, e.g. files from the current jar archive.
  * @param fileName The file name.
  * @param pZtexDescriptorOffs The position of the descriptor in bytes. The default position is 0x6c.
  * @throws IOException If an read error occurred.
  * @throws IhxFileDamagedException If the ihx file is damaged.
  * @throws IncompatibleFirmwareException If the firmware image contains no valid ZTEX descriptor 1 at the specified position.
  */
    public ZtexIhxFile1( String fileName, int pZtexDescriptorOffs ) throws IOException, IhxFileDamagedException, IncompatibleFirmwareException {
	super( fileName );

	ztexDescriptorOffs = pZtexDescriptorOffs;
	
	if ( ihxData[ztexDescriptorOffs]!=40 || ihxData[ztexDescriptorOffs+1]!=1 || ihxData[ztexDescriptorOffs+2]!='Z' || ihxData[ztexDescriptorOffs+3]!='T' || ihxData[ztexDescriptorOffs+4]!='E' || ihxData[ztexDescriptorOffs+5]!='X' )
	    throw new IncompatibleFirmwareException( "Invalid ZTEX descriptor" );
	    
	productId[0] = (byte) ihxData[ztexDescriptorOffs+6];
	productId[1] = (byte) ihxData[ztexDescriptorOffs+7];
	productId[2] = (byte) ihxData[ztexDescriptorOffs+8];
	productId[3] = (byte) ihxData[ztexDescriptorOffs+9];
	fwVersion = (byte) ihxData[ztexDescriptorOffs+10];
	interfaceVersion = (byte) ihxData[ztexDescriptorOffs+11];
	interfaceCapabilities[0] = (byte) ihxData[ztexDescriptorOffs+12];
	interfaceCapabilities[1] = (byte) ihxData[ztexDescriptorOffs+13];
	interfaceCapabilities[2] = (byte) ihxData[ztexDescriptorOffs+14];
	interfaceCapabilities[3] = (byte) ihxData[ztexDescriptorOffs+15];
	interfaceCapabilities[4] = (byte) ihxData[ztexDescriptorOffs+16];
	interfaceCapabilities[5] = (byte) ihxData[ztexDescriptorOffs+17];
	moduleReserved[0] = (byte) ihxData[ztexDescriptorOffs+18];
	moduleReserved[1] = (byte) ihxData[ztexDescriptorOffs+19];
	moduleReserved[2] = (byte) ihxData[ztexDescriptorOffs+20];
	moduleReserved[3] = (byte) ihxData[ztexDescriptorOffs+21];
	moduleReserved[4] = (byte) ihxData[ztexDescriptorOffs+22];
	moduleReserved[5] = (byte) ihxData[ztexDescriptorOffs+23];
	moduleReserved[6] = (byte) ihxData[ztexDescriptorOffs+24];
	moduleReserved[7] = (byte) ihxData[ztexDescriptorOffs+25];
	moduleReserved[8] = (byte) ihxData[ztexDescriptorOffs+26];
	moduleReserved[9] = (byte) ihxData[ztexDescriptorOffs+27];
	moduleReserved[10] = (byte) ihxData[ztexDescriptorOffs+28];
	moduleReserved[11] = (byte) ihxData[ztexDescriptorOffs+29];

	for (int i=0; i<10; i++ ) {
	    int b = ihxData[ztexDescriptorOffs+30+i];
	    if ( b>=0 && b<=255 ) {
		snString[i] = (char) b;
	    }
	    else {
		throw new IncompatibleFirmwareException( "Invalid serial number string" );
	    }
	}

	// ensure word aligned upload data
	for ( int i=0; i+1<ihxData.length; i+=2 )
	    if ( ihxData[i]<0 && ihxData[i+1]>=0 )
		ihxData[i] = 0;
    }

/**
  * Constructs an instance from a given file name.
  * The ZTEX descriptor 1 is expected to be at the position 0x6c of the firmware image.<br>
  * This method can also read system resources, e.g. files from the current jar archive.
  * @param fileName The file name.
  * @throws IOException If an read error occurred.
  * @throws IhxFileDamagedException If the ihx file is damaged.
  * @throws IncompatibleFirmwareException If the firmware image contains no valid ZTEX descriptor 1 at the specified position.
  */
    public ZtexIhxFile1( String fileName ) throws IOException, IhxFileDamagedException, IncompatibleFirmwareException {
	this( fileName, defaultZtexDescriptorOffs );
    }

// ******* productId ***********************************************************
/** 
  * Returns the product ID (all 4 bytes).
  * @return PRODUCT_ID, see {@link ZtexDevice1}.
  */
    public final byte[] productId() {
	return productId;
    }

/** 
  * Returns byte i of the product ID.
  * @return PRODUCT_ID[i], see {@link ZtexDevice1}.
  * @param i index 
  */
    public int productId( int i ) {
	return productId[i] & 255;
    }

// ******* fwVersion ***********************************************************
/** 
  * Returns the firmware version.
  * @return FW_VERSION, see {@link ZtexDevice1}.
  */
    public final int fwVersion() {
	return fwVersion & 255;
    }

// ******* interfaceVersion *****************************************************
/** 
  * Returns the interface version.
  * @return INTERFACE_VERSION, see {@link ZtexDevice1}.
  */
    public final int interfaceVersion() {
	return interfaceVersion & 255;
    }

// ******* interfaceCapabilities ************************************************
/** 
  * Returns the interface capabilities (all 6 bytes).
  * @return INTERFACE_CAPABILITIES, see {@link ZtexDevice1}.
  */
    public final byte[] interfaceCapabilities() {
	return interfaceCapabilities;
    }

/** 
  * Returns byte i of the interface capabilities.
  * @return INTERFACE_CAPABILITIES[i], see {@link ZtexDevice1}.
  * @param i index 
  */
    public final int interfaceCapabilities( int i ) {
	return interfaceCapabilities[i] & 255;
    }

// ******* moduleReserved ******************************************************
/** 
  * Returns the application specific information (all 12 bytes).
  * @return MODULE_RESERVED, see {@link ZtexDevice1}.
  */
    public final byte[] moduleReserved() {
	return moduleReserved;
    }

/** 
  * Returns byte i of the application specific information.
  * @return MODULE_RESERVED[i], see {@link ZtexDevice1}.
  * @param i index 
  */
    public final int moduleReserved( int i ) {
	return moduleReserved[i] & 255;
    }

// ******* snString ************************************************************
/** 
  * Returns the serial number string.
  * @return SN_STRING, see {@link ZtexDevice1}.
  */
    public final String snString() {
	return new String( snString );
    }

// ******* setSnString **********************************************************
/** 
  * Modifies the serial number string. 
  * @param s The new serial number string which must not be longer then 10 characters. 
  */
    public final void setSnString( String s ) throws IncompatibleFirmwareException {
	if ( s.length()>10 ) 
	    throw new IncompatibleFirmwareException( "Serial number too long (max. 10 characters)" );
	
	int i=0;
	for (; i<s.length(); i++ ) {
	    ihxData[ztexDescriptorOffs+30+i] = (byte) s.charAt(i);
	}
	for (; i<10; i++ ) {
	    ihxData[ztexDescriptorOffs+30+i] = 0;
	}
    }

// ******* toString ************************************************************
/** 
  * Returns a string representation if the instance.
  * @return a string representation if the instance.
  */
    public String toString () {
	return "productID=" + ZtexDevice1.byteArrayString(productId) + "  fwVer="+(fwVersion & 255) + "  ifVer="+(interfaceVersion & 255)+ "  snString=\"" + snString() + "\"";
    }

}    
