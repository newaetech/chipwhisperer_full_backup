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
    Scan bus for devices with ZTEX descriptor 1 and/or Cypress EZ-USB FX2 devices
*/
package ztex;

import java.io.*;
import java.util.*;

import ch.ntb.usb.*;

/**
  * A class used for finding the EZ-USB devices on the USB.
  * The devices found are stored as a list of {@link ZtexDevice1} instances.
  * @see ZtexDevice1
  */

public class ZtexScanBus1 {
    private Vector<ZtexDevice1> devices = new Vector<ZtexDevice1>();

// ******* ZtexScanBus1 ********************************************************
/**
  * Scans the USB for suitable devices and constructs a list of them.
  * Four kinds of search filters can be applied
  * <ol>
  *   <li> usbVendorId and usbProductId can be used to search for devices with a given vendor and product ID. These devices must provide a ZTEX descriptor 1.</li>
  *   <li> If a certain interface version is required, it can be specified using interfaceVersion. </li>
  *   <li> Incompatible devices can be excluded by the specification of the ZTEX product ID's, see {@link ZtexDevice1#compatible(int,int,int,int)}. </li>
  *   <li> If scanUnconfigured is true, also devices without ZTEX Firmware and devices with Cypress EZ-USB USB are considered</li>
  *   <li> In multi device environment a single device can be selected by giving a serial number. </li>
  * </ol>
  * @param usbVendorId USB vendor ID of the device to be searched for
  * @param usbProductId USB product ID of the device to be searched for
  * @param scanUnconfigured if true, scan for unconfigured devices and devices with Cypress EZ-USB USB ID's
  * @param quiet if true, don't print any warnings
  * @param interfaceVersion The required interface version (&lt;0 if no interface version is required)
  * @param snString The serial number of the device
  * @param productId0 Byte 0 of a given ZTEX product ID (&le;0 if not to be considered)
  * @param productId1 Byte 1 of a given ZTEX product ID (&le;0 if not to be considered)
  * @param productId2 Byte 2 of a given ZTEX product ID (&le;0 if not to be considered)
  * @param productId3 Byte 3 of a given ZTEX product ID (&le;0 if not to be considered)
  */
    public ZtexScanBus1 (int usbVendorId, int usbProductId, boolean scanUnconfigured, boolean quiet, int interfaceVersion, String snString, int productId0, int productId1, int productId2, int productId3 ) {
	LibusbJava.usb_find_busses();
	LibusbJava.usb_find_devices();

	Usb_Bus bus = LibusbJava.usb_get_busses();

	while ( bus != null ) {
	    Usb_Device dev = bus.getDevices();
	    while ( dev != null ) { 
		try {
		    ZtexDevice1 zdev = new ZtexDevice1( dev, usbVendorId, usbProductId, scanUnconfigured );
		    if ( scanUnconfigured ||
			 ( zdev.valid() && 
			   ( interfaceVersion<0 || zdev.interfaceVersion()==interfaceVersion ) && 
			   ( snString == null || zdev.snString().equals(snString) ) && 
			    zdev.compatible(productId0, productId1, productId2, productId3) 
			 ) 
		       ) 
			    devices.add( zdev );
		}
		catch ( DeviceNotSupportedException e ) {
		}
		catch ( Exception e ) {
		    if ( ! quiet )
			System.err.println( e.getLocalizedMessage() );
		}
		dev = dev.getNext();
	    }
	    bus = bus.getNext();
	}
    }

/**
  * Scans the USB for suitable devices and constructs a list of them.
  * Three kinds of search filters can be applied
  * <ol>
  *   <li> usbVendorId and usbProductId can be used to search for devices with a given vendor and product ID. These devices must provide a ZTEX descriptor 1.</li>
  *   <li> If a certain interface version is required, it can be specified using interfaceVersion. </li>
  *   <li> If scanUnconfigured is true, also devices without ZTEX Firmware and devices with Cypress EZ-USB USB are considered</li>
  *   <li> In multi device environment a single device can be selected by giving a serial number. </li>
  * </ol>
  * @param usbVendorId USB vendor ID of the device to be searched for
  * @param usbProductId USB product ID of the device to be searched for
  * @param scanUnconfigured if true, scan for unconfigured devices and devices with Cypress EZ-USB USB ID's
  * @param quiet if true, don't print any warnings
  * @param interfaceVersion The required interface version (<0 if no interface version is required)
  */
    public ZtexScanBus1 (int usbVendorId, int usbProductId, boolean scanUnconfigured, boolean quiet, int interfaceVersion, String snString ) {
	this(usbVendorId, usbProductId, scanUnconfigured, quiet, interfaceVersion, snString, -1,-1,-1,-1 );
    }

/**
  * Scans the USB for suitable devices and constructs a list of them.
  * Three kinds of search filters can be applied
  * <ol>
  *   <li> usbVendorId and usbProductId can be used to search for devices with a given vendor and product ID. These devices must provide a ZTEX descriptor 1.</li>
  *   <li> If a certain interface version is required, it can be specified using interfaceVersion. </li>
  *   <li> If scanUnconfigured is true, also devices without ZTEX Firmware and devices with Cypress EZ-USB USB are considered</li>
  * </ol>
  * @param usbVendorId USB vendor ID of the device to be searched for
  * @param usbProductId USB product ID of the device to be searched for
  * @param scanUnconfigured if true, scan for unconfigured devices and devices with Cypress EZ-USB USB ID's
  * @param quiet if true, don't print any warnings
  * @param interfaceVersion The required interface version (<0 if no interface version is required)
  */
    public ZtexScanBus1 (int usbVendorId, int usbProductId, boolean scanUnconfigured, boolean quiet, int interfaceVersion ) {
	this(usbVendorId, usbProductId, scanUnconfigured, quiet, interfaceVersion, null, -1,-1,-1,-1 );
    }

/**
  * Scans the USB for suitable devices and constructs a list of them.
  * Two kinds of search filters can be applied
  * <ol>
  *   <li> usbVendorId and usbProductId can be used to search for devices with a given vendor and product ID. These devices must provide a ZTEX descriptor 1.</li>
  *   <li> If scanUnconfigured is true, also devices without ZTEX Firmware and devices with Cypress EZ-USB USB are considered</li>
  * </ol>
  * @param usbVendorId USB vendor ID of the device to be searched for
  * @param usbProductId USB product ID of the device to be searched for
  * @param scanUnconfigured if true, scan for unconfigured devices and devices with Cypress EZ-USB USB ID's
  * @param quiet if true, don't print any warnings
  */
    public ZtexScanBus1 (int usbVendorId, int usbProductId, boolean scanUnconfigured, boolean quiet ) {
	this(usbVendorId, usbProductId, scanUnconfigured, quiet, -1, null, -1,-1,-1,-1 );
    }

// ******* printBus ************************************************************
/**
  * Prints out a list of devices found.
  * @param out Where the output is to be printed to.
  */
    public void printBus( PrintStream out ) {
	for (int i=0; i<devices.size(); i++ ) {
	    out.println( i + ": " + devices.elementAt(i).toString() );
	}
    }
    
// ******* numberOfDevices *****************************************************
/**
  * Returns the number of devices found.
  * @return the number of devices found.
  */
    public final int numberOfDevices () {
	return devices.size();
    }

// ******* device **************************************************************
/**
  * Returns a device from the list of devices.
  * @param i The device index.
  * @return a device from the list of devices.
  * @throws IndexOutOfBoundsException if i&lt;0 or i&ge;{@link #numberOfDevices()}
  */
    public final ZtexDevice1 device (int i) throws IndexOutOfBoundsException {
	if ( i<0 || i>=devices.size() ) 
	    throw new IndexOutOfBoundsException( "Device number out of range. Valid numbers are 0.." + (devices.size()-1) ); 
	return devices.elementAt(i);
    }

}    

