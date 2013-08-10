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

package ztex;

import java.io.*;
import java.util.*;

import ch.ntb.usb.*;

/** 
  * Provides methods for uploading firmware to Cypress EZ-USB devices.
  */
public class EzUsb {
// ******* reset **************************************************************
/** 
  * Controls the reset state of a Cypress EZ-USB device.
  * @param handle The handle of the device.
  * @param r The reset state (true means reset).
  * @throws FirmwareUploadException if an error occurred while attempting to control the reset state.
  */
    public static void reset ( long handle, boolean r ) throws FirmwareUploadException {
	byte buffer[] = { (byte) (r ? 1 : 0) };
	int k = LibusbJava.usb_control_msg(handle, 0x40, 0xA0, 0xE600, 0, buffer, 1, 1000);   // upload j bytes
	if ( k<0 ) 
	    throw new FirmwareUploadException( LibusbJava.usb_strerror() + ": unable to set reset="+buffer[0] );
	else if ( k!=1 ) 
	    throw new FirmwareUploadException( "Unable to set reset="+buffer[0] );
	try {
    	    Thread.sleep( r ? 50 : 400 );	// give the firmware some time for initialization
	}
	    catch ( InterruptedException e ) {
	}
    }
     
// ******* uploadFirmware ******************************************************
/** 
  * Uploads the Firmware to a Cypress EZ-USB device.
  * @param handle The handle of the device.
  * @param ihxFile The firmware image.
  * @return the upload time in ms.
  * @throws FirmwareUploadException if an error occurred while attempting to upload the firmware.
  */
    public static long uploadFirmware (long handle, IhxFile ihxFile ) throws FirmwareUploadException {
	final int transactionBytes = 256;
	byte[] buffer = new byte[transactionBytes];

	reset( handle, true );  // reset = 1
	
	long t0 = new Date().getTime();
	int j = 0;
	for ( int i=0; i<=ihxFile.ihxData.length; i++ ) {
	    if ( i >= ihxFile.ihxData.length || ihxFile.ihxData[i] < 0 || j >=transactionBytes ) {
		if ( j > 0 ) {
		    int k = LibusbJava.usb_control_msg(handle, 0x40, 0xA0, i-j, 0, buffer, j, 1000);   // upload j bytes
		    if ( k<0 ) 
			throw new FirmwareUploadException(LibusbJava.usb_strerror());
		    else if ( k!=j ) 
			throw new FirmwareUploadException();
		    try {
		        Thread.sleep( 1 );	// to avoid package loss
		    }
			catch ( InterruptedException e ) {
		    }
		}
		j = 0;
	    }

	    if ( i < ihxFile.ihxData.length && ihxFile.ihxData[i] >= 0 && ihxFile.ihxData[i] <= 255 ) {
		buffer[j] = (byte) ihxFile.ihxData[i];
		j+=1;
	    }
	}
	long t1 = new Date().getTime();

	try {
	    EzUsb.reset(handle,false);		// error (may caused re-numeration) can be ignored
	}
	catch ( FirmwareUploadException e ) {
	}
	return t1 - t0;
    }
}    

