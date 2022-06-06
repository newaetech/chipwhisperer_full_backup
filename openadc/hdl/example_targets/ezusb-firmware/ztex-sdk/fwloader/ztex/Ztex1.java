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
    Functions for USB devices with ZTEX descriptor 1
*/
package ztex;

import java.io.*;
import java.util.*;

import ch.ntb.usb.*;

/**
  * This class implements the interface-independent part of the communication protocol for the interaction with the ZTEX firmware.<p>
  * All firmware implementations that provide the ZTEX descriptor 1 are supported.
  * A description of this descriptor can be found in {@link ZtexDevice1}.
  * <p>
  * The most important features of this class are the functions for uploading the firmware
  * and the renumeration management.
  * <p>
  * The interface dependent part of the communication protocol (currently only one is supported)
  * can be found in {@link Ztex1v1}.
  * @see ZtexDevice1
  * @see Ztex1v1
  */
public class Ztex1 {
    private final int maxDevNum = 1023;
    private long handle;
    private ZtexDevice1 dev = null;
    private boolean oldDevices[] = new boolean[maxDevNum+1];
    private int oldDevNum = -1;
    private String usbBusName = null;
    private boolean[] interfaceClaimed = new boolean[256];
    private boolean configurationSet = false;
/** * Setting to true enables certain workarounds, e.g. to deal with bad driver/OS implementations. */
    public boolean certainWorkarounds = false;
/** * The timeout for  control messages in ms. */    
    public int controlMsgTimeout = 1000;	// in ms
    private long lastVendorCommandT = 0;
    
    

// ******* Ztex1 ***************************************************************
/** 
  * Constructs an instance from a given device.
  * @param pDev The given device.
  * @throws UsbException if an communication error occurred.
  */
    public Ztex1 ( ZtexDevice1 pDev ) throws UsbException {
	dev = pDev;
	
	for (int i=0; i<256; i++)
	    interfaceClaimed[i] = false;

	handle = LibusbJava.usb_open(dev.dev());
//	if ( handle<=0 ) 
//	    throw new UsbException(dev.dev(), "Error opening device");
    }

// ******* finalize ************************************************************
/** * The destructor closes the USB file handle. */
    protected void finalize () {
	for (int i=0; i<256; i++)
	    if ( interfaceClaimed[i] ) 
		LibusbJava.usb_release_interface(handle, i);

	LibusbJava.usb_close(handle);
    }

// ******* handle **************************************************************
/** * Returns the USB file handle. */
    public final long handle() 
    {
        return handle;
    }

// ******* dev *****************************************************************
/** 
  * Returns the corresponding {@link ZtexDevice1}. 
  * @return the corresponding {@link ZtexDevice1}. 
  */
    public final ZtexDevice1 dev() 
    {
        return dev;
    }

// ******* valid ***************************************************************
/** 
  * Returns true if ZTEX descriptor 1 is available.
  * @return true if ZTEX descriptor 1 is available.
  */
    public boolean valid ( ) {
	return dev.valid();
    }

// ******* checkValid **********************************************************
/** 
  * Checks whether ZTEX descriptor 1 is available.
  * @throws InvalidFirmwareException if ZTEX descriptor 1 is not available.
  */
    public void checkValid () throws InvalidFirmwareException {
	if ( ! dev.valid() ) 
	    throw new InvalidFirmwareException(this, "Can't read ZTEX descriptor 1");
    }

// ******* vendorCommand *******************************************************
/**
  * Sends a vendor command to Endpoint 0 of the EZ-USB device.
  * The command may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The command number (0..255).
  * @param func The name of the command. This string is used for the generation of error messages.
  * @param value The value (0..65535), i.e bytes 2 and 3 of the setup data.
  * @param index The index (0..65535), i.e. bytes 4 and 5 of the setup data.
  * @param length The size of the payload data (0..65535), i.e. bytes 6 and 7 of the setup data.
  * @param buf The payload data buffer.
  * @return the number of bytes sent.
  * @throws UsbException if a communication error occurs.
  */
    public synchronized int vendorCommand (int cmd, String func, int value, int index, byte[] buf, int length) throws UsbException {
	long t0 = new Date().getTime()-100;
	int trynum = 0;
	int i = -1;
	if ( controlMsgTimeout < 200 )
	    controlMsgTimeout = 200;
//	while ( i<=0 && new Date().getTime()-t0<controlMsgTimeout ) {		// we repeat the message until the timeout has reached
	    i = LibusbJava.usb_control_msg(handle, 0x40, cmd, value, index, buf, length, controlMsgTimeout);
	    if ( certainWorkarounds ) {
		try {
    	    	    Thread.sleep(2);
		}
	    	    catch ( InterruptedException e ) {
		}	
	    }
	    lastVendorCommandT = new Date().getTime();
	    if ( i < 0 ) {
		System.err.println("Warning (try " + (trynum+1) + "): " + LibusbJava.usb_strerror() );
		try {
    		    Thread.sleep( 1 << trynum );				// we don't want to bother the USB device to often
		}
		    catch ( InterruptedException e ) {
		}	
		trynum++;
	    }
//	} 
	if ( i < 0 )
	    throw new UsbException( dev.dev(), (func != null ? func + ": " : "" )+ LibusbJava.usb_strerror());
	return i;
    }

/**
  * Sends a vendor command with no payload data to Endpoint 0 of the EZ-USB device.
  * The command may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The command number (0..255).
  * @param func The name of the command. This string is used for the generation of error messages.
  * @param value The value (0..65535), i.e bytes 2 and 3 of the setup data.
  * @param index The index (0..65535), i.e. bytes 4 and 5 of the setup data.
  * @return the number of bytes sent.
  * @throws UsbException if a communication error occurs.
  */
    public int vendorCommand (int cmd, String func, int value, int index) throws UsbException {
	byte[] buf = { 0 };
	return vendorCommand (cmd, func, value, index, buf, 0);
    }

/**
  * Sends a vendor command with no payload data and no setup data to Endpoint 0 of the EZ-USB device.
  * The command may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The command number (0..255).
  * @param func The name of the command. This string is used for the generation of error messages.
  * @return the number of bytes sent.
  * @throws UsbException if a communication error occurs.
  */
    public int vendorCommand (int cmd, String func) throws UsbException {
	byte[] buf = { 0 };
	return vendorCommand (cmd, func, 0, 0, buf, 0);
    }

// ******* vendorRequest *******************************************************
/**
  * Sends a vendor request to Endpoint 0 of the EZ-USB device.
  * The request may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The request number (0..255).
  * @param func The name of the request. This string is used for the generation of error messages.
  * @param value The value (0..65535), i.e bytes 2 and 3 of the setup data.
  * @param index The index (0..65535), i.e. bytes 4 and 5 of the setup data.
  * @param maxlen The size of the requested payload data (0..65535), i.e. bytes 6 and 7 of the setup data.
  * @param buf The payload data buffer.
  * @return the number of bytes received.
  * @throws UsbException if a communication error occurs.
  */
    public synchronized int vendorRequest (int cmd, String func, int value, int index, byte[] buf, int maxlen) throws UsbException {
	long t0 = new Date().getTime()-100;
	int trynum = 0;
	int i = -1;
	if ( controlMsgTimeout < 200 )
	    controlMsgTimeout = 200;
	while ( i<=0 && new Date().getTime()-t0<controlMsgTimeout ) {		// we repeat the message until the timeout has reached
	    /* 
		The HSNAK mechanism of EP0 usually avoids that a request is sent before a command has been completed.
		Unfortunately this mechanism is only 99.99% reliable. Therefore we wait at least 1ms after the last
		command has been send before we transmit a new request.
	    */
	    long ms = new Date().getTime() - lastVendorCommandT;
	    if ( ms < 2 ) {	//
		try {
    	    	    Thread.sleep(1);
		}
	    	    catch ( InterruptedException e ) {
		}	
	    }
		
	    i = LibusbJava.usb_control_msg(handle, 0xc0, cmd, value, index, buf, maxlen, controlMsgTimeout);
	    if ( certainWorkarounds ) {
		try {
    	    	    Thread.sleep(2);
		}
	    	    catch ( InterruptedException e ) {
		}	
	    }
	    if ( i < 0 ) {
		System.err.println("Warning (try " + (trynum+1) + "): " + LibusbJava.usb_strerror() );
		try {
    		    Thread.sleep( 1 << trynum );				// we don't want to bother the USB device to often
		}
		    catch ( InterruptedException e ) {
		}	
		trynum++;
	    }
	} 
	if ( i < 0 )
	    throw new UsbException( dev.dev(), (func != null ? func + ": " : "" ) + LibusbJava.usb_strerror());
	return i;
    }

/**
  * Sends a vendor request to Endpoint 0 of the EZ-USB device.
  * The request may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The request number (0..255).
  * @param func The name of the request. This string is used for the generation of error messages.
  * @param maxlen The size of the requested payload data (0..65535), i.e. bytes 6 and 7 of the setup data.
  * @param buf The payload data buffer.
  * @return the number of bytes sent.
  * @throws UsbException if a communication error occurs.
  */
    public int vendorRequest (int cmd, String func, byte[] buf, int maxlen) throws UsbException {
	return vendorRequest (cmd, func, 0, 0, buf, maxlen);
    }

// ******* vendorCommand2 ******************************************************
/**
  * Sends a vendor command to Endpoint 0 of the EZ-USB device and throws an {@link UsbException} if not all of the payload has been sent.
  * The command may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The command number (0..255).
  * @param func The name of the command. This string is used for the generation of error messages.
  * @param value The value (0..65535), i.e bytes 2 and 3 of the setup data.
  * @param index The index (0..65535), i.e. bytes 4 and 5 of the setup data.
  * @param length The size of the payload data (0..65535), i.e. bytes 6 and 7 of the setup data.
  * @param buf The payload data buffer.
  * @throws UsbException if a communication error occurs or if not all of the payload has been sent.
  */
    public synchronized void vendorCommand2 (int cmd, String func, int value, int index, byte[] buf, int length) throws UsbException {
	int i = vendorCommand (cmd, func, value, index, buf, length);
	if ( i != length )
	    throw new UsbException( dev.dev(), (func != null ? func + ": " : "" ) + "Send " + i + " byte of data instead of " + length + " bytes");
    }

// ******* vendorRequest2 ******************************************************
/**
  * Sends a vendor request to Endpoint 0 of the EZ-USB device and throws an {@link UsbException} if not all of the payload has been received.
  * The request may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The request number (0..255).
  * @param func The name of the request. This string is used for the generation of error messages.
  * @param value The value (0..65535), i.e bytes 2 and 3 of the setup data.
  * @param index The index (0..65535), i.e. bytes 4 and 5 of the setup data.
  * @param maxlen The size of the requested payload data (0..65535), i.e. bytes 6 and 7 of the setup data.
  * @param buf The payload data buffer.
  * @throws UsbException if a communication error occurs or not all of the payload has been received.
  */
    public void vendorRequest2 (int cmd, String func, int value, int index, byte[] buf, int maxlen) throws UsbException {
	int i = vendorRequest(cmd, func, value, index, buf, maxlen);
	if ( i != maxlen )
	    throw new UsbException( dev.dev(), (func != null ? func + ": " : "" ) + "Received " + i + " byte of data, expected "+maxlen+" bytes");
    }

/**
  * Sends a vendor request to Endpoint 0 of the EZ-USB device and throws an {@link UsbException} if not all of the payload has been received.
  * The request may be send multiple times until the {@link #controlMsgTimeout} is reached.
  * @param cmd The request number (0..255).
  * @param func The name of the request. This string is used for the generation of error messages.
  * @param maxlen The size of the requested payload data (0..65535), i.e. bytes 6 and 7 of the setup data.
  * @param buf The payload data buffer.
  * @throws UsbException if a communication error occurs or not all of the payload has been received.
  */
    public void vendorRequest2 (int cmd, String func, byte[] buf, int maxlen) throws UsbException {
	vendorRequest2(cmd, func, 0, 0, buf, maxlen);
    }



// ******* setConfiguration ****************************************************
/**
  * Sets the configuration.
  * @param config The configuration number (usually 1)
  * @throws UsbException if an error occurs while attempting to set the configuration.
  */
    public void setConfiguration ( int config) throws UsbException{
	if ( LibusbJava.usb_set_configuration(handle(), config) < 0 )
	    throw new UsbException("Setting configuration to " + config + " failed: " + LibusbJava.usb_strerror());
	configurationSet = true;
    }


// ******* trySetConfiguration ****************************************************
/**
  * Tries to set the configuration.
  * If an error occurs while attempting to set the configuration, a warning messaage is printed to stderr.
  * @param config The configuration number (usually 1)
  */
    public void trySetConfiguration ( int config) {
	if ( LibusbJava.usb_set_configuration(handle(), config) < 0 )
	    System.err.println("Setting configuration to " + config + " failed: " + LibusbJava.usb_strerror());
	configurationSet = true;
    }


// ******* getInterfaceClaimed *************************************************
/**
  * Returns true if interface is claimed.
  * @return true if interface is claimed
  * @param iface The interface number
  */
    public boolean getInterfaceClaimed ( int iface ) {
	return iface>=0 && iface<256 && interfaceClaimed[iface];
    }
    

// ******* claimInterface ******************************************************
/**
  * Claims an interface.
  * @param iface The interface number (usually 0)
  * @throws UsbException if an error occurs while attempting to claim the interface.
  */
    public void claimInterface ( int iface) throws UsbException{
	if ( ! configurationSet )
	    trySetConfiguration(1);
	if ( ( iface<0 || iface>=256 || (! interfaceClaimed[iface]) ) && ( LibusbJava.usb_claim_interface(handle(), iface) < 0 ) )
	    throw new UsbException("Claiming interface " + iface + " failed: " + LibusbJava.usb_strerror());
	if ( iface>=0 && iface < 256 )
	    interfaceClaimed[iface]=true;
    }


// ******* releaseInterface ****************************************************
/**
  * Releases an interface.
  * @param iface The interface number (usually 0)
  */
    public void releaseInterface ( int iface ) {
	if ( iface<0 || iface>=256 || interfaceClaimed[iface] ) 
	    LibusbJava.usb_release_interface(handle(), iface);
	if ( iface>=0 && iface < 256 )
	    interfaceClaimed[iface]=false;
	    
    }


// ******* findOldDevices ******************************************************
    private synchronized void findOldDevices () throws DeviceLostException {
	usbBusName = dev.dev().getBus().getDirname();

	Usb_Bus bus = LibusbJava.usb_get_busses();
	while ( bus != null && ! bus.getDirname().equals(usbBusName) ) 
	    bus = bus.getNext();
	if ( bus == null )
		throw new DeviceLostException( "findOldDevice: Bus dissapeared" );
	    
	for ( int i=0; i<=maxDevNum; i++ ) 
	    oldDevices[i] = false;
	
	Usb_Device d = bus.getDevices();
	while ( d != null ) { 
	    byte b = d.getDevnum();
	    if ( b > maxDevNum ) 
		throw new DeviceLostException( "Device number too large: " + b + " > " + maxDevNum );
	    if ( b > 0 ) 
		oldDevices[b] = true;
	    d = d.getNext();
	}
	oldDevNum = dev.dev().getDevnum();
    }

// ******* findNewDevice *******************************************************
    private synchronized Usb_Device findNewDevice ( String errMsg ) throws DeviceLostException {
	Usb_Device newDev = null;
	LibusbJava.usb_find_busses();
	LibusbJava.usb_find_devices();
	
	Usb_Bus bus = LibusbJava.usb_get_busses();
	while ( bus != null && ! bus.getDirname().equals(usbBusName) ) 
	    bus = bus.getNext();
	if ( bus == null )
		throw new DeviceLostException( "findNewDevice: Bus dissapeared" );
	
	Usb_Device d = bus != null ? bus.getDevices() : null;
	while ( d != null ) { 
	    byte b = d.getDevnum();
	    if ( b > maxDevNum ) 
		throw new DeviceLostException( "Device number too large: " + b + " > " + maxDevNum );
	    if ( b > 0 && ! oldDevices[b] ) {
		if ( newDev != null )
		    throw new DeviceLostException( errMsg + "More than 2 new devices found: " + newDev.getDevnum() + "(`" + newDev.getFilename() + "') and " + b + "(`" + d.getFilename() + "')");
		newDev = d;
	    }
	    d = d.getNext();
	}
	
	return newDev;
    }

// ******* initNewDevice *******************************************************
    private void initNewDevice ( String errBase, boolean scanUnconfigured ) throws DeviceLostException, UsbException, InvalidFirmwareException {
// scan the bus for up to 60 s for a new device. Boot sequence may take a while.
	Usb_Device newDev = null;
	int i;
	for ( i=0; i<300 && newDev==null; i++ ) {
	    try {
    		Thread.sleep( 200 );
	    }
		catch ( InterruptedException e ) {
	    }
	    if ( i > 10 && oldDevNum >= 0 && oldDevNum < maxDevNum ) 
		oldDevices[oldDevNum ] = false;
	    newDev = findNewDevice( errBase + ": " );
	}
	oldDevNum = -1;
	if ( newDev == null )  
	    throw new DeviceLostException( errBase + ": No new device found" );

// init new device
	Usb_Device_Descriptor dd = newDev.getDescriptor();
	int vid = dd.getIdVendor() & 65535;
	int pid = dd.getIdProduct() & 65535;
	try {
	    dev = new ZtexDevice1( newDev, vid, pid, scanUnconfigured );
	}
	catch ( DeviceNotSupportedException e ) {
	    throw new InvalidFirmwareException( e.getLocalizedMessage() );
	}
	
	handle = LibusbJava.usb_open( dev.dev() );
    }

// ******* uploadFirmware ******************************************************
/**
  * Uploads the firmware to the EZ-USB and manages the renumeration process.
  * <p>
  * Before the firmware is uploaded the device is set into a reset state.
  * After the upload the firmware is booted and the renumeration starts.
  * During this process the device disappears from the bus and a new one 
  * occurs which will be assigned to this class automatically (instead of the disappeared one).
  * @param ihxFile The firmware image.
  * @param force The compatibility check is skipped if true.
  * @throws IncompatibleFirmwareException if the given firmware is not compatible to the installed one, see {@link ZtexDevice1#compatible(int,int,int,int)} (Upload can be enforced using the <tt>force</tt> parameter)
  * @throws FirmwareUploadException If an error occurred while attempting to upload the firmware.
  * @throws UsbException if a communication error occurs.
  * @throws InvalidFirmwareException if ZTEX descriptor 1 is not available.
  * @throws DeviceLostException if a device went lost after renumeration.
  * @return the upload time in ms.
  */
//  returns upload time in ms
    public long uploadFirmware ( ZtexIhxFile1 ihxFile, boolean force ) throws IncompatibleFirmwareException, FirmwareUploadException, UsbException, InvalidFirmwareException, DeviceLostException {
// load the ihx file
//	ihxFile.dataInfo(System.out);
//	System.out.println(ihxFile);

// check for compatibility
	if ( ! force && dev.valid() ) {
	    if ( ihxFile.interfaceVersion() != 1 )
		throw new IncompatibleFirmwareException("Wrong interface version: Expected 1, got " + ihxFile.interfaceVersion() );
	
	    if ( ! dev.compatible ( ihxFile.productId(0), ihxFile.productId(1), ihxFile.productId(2), ihxFile.productId(3) ) )
		throw new IncompatibleFirmwareException("Incompatible productId's: Current firmware: " + ZtexDevice1.byteArrayString(dev.productId()) 
		    + "  Ihx File: " + ZtexDevice1.byteArrayString(ihxFile.productId()) );
	}

// scan the bus for comparison
	findOldDevices();
	
// upload the firmware
	long time = EzUsb.uploadFirmware( handle, ihxFile );
	
// find and init new device
	initNewDevice("Device lost after uploading Firmware", false);
	
	return time;
    }

/**
  * Uploads the firmware to the EZ-USB and manages the renumeration process.
  * <p>
  * Before the firmware is uploaded the device is set into a reset state.
  * After the upload the firmware is booted and the renumeration starts.
  * During this process the device disappears from the bus and a new one 
  * occurs which will be assigned to this class automatically (instead of the disappeared one).
  * @param ihxFileName The file name of the firmware image in ihx format. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @param force The compatibility check is skipped if true.
  * @throws IncompatibleFirmwareException if the given firmware is not compatible to the installed one, see {@link ZtexDevice1#compatible(int,int,int,int)} (Upload can be enforced using the <tt>force</tt> parameter)
  * @throws FirmwareUploadException If an error occurred while attempting to upload the firmware.
  * @throws UsbException if a communication error occurs.
  * @throws InvalidFirmwareException if ZTEX descriptor 1 is not available.
  * @throws DeviceLostException if a device went lost after renumeration.
  * @return the upload time in ms.
  */
//  returns upload time in ms
    public long uploadFirmware ( String ihxFileName, boolean force ) throws IncompatibleFirmwareException, FirmwareUploadException, UsbException, InvalidFirmwareException, DeviceLostException {
// load the ihx file
	ZtexIhxFile1 ihxFile;
	try {
	    ihxFile = new ZtexIhxFile1( ihxFileName );
	}
	catch ( IOException e ) {
	    throw new FirmwareUploadException( e.getLocalizedMessage() );
	}
	catch ( IhxFileDamagedException e ) {
	    throw new FirmwareUploadException( e.getLocalizedMessage() );
	}
	return uploadFirmware( ihxFile, force );
    }

// ******* resetEzUsb **********************************************************
/**
  * Resets the EZ-USB and manages the renumeration process.
  * <p>
  * After the reset the renumeration starts.
  * During this process the device disappears from the bus and a new one 
  * occurs which will be assigned to this class automatically (instead of the disappeared one).
  * @throws FirmwareUploadException If an error occurred while attempting to upload the firmware.
  * @throws UsbException if a communication error occurs.
  * @throws InvalidFirmwareException if ZTEX descriptor 1 is not available.
  * @throws DeviceLostException if a device went lost after renumeration.
  */
    public void resetEzUsb () throws FirmwareUploadException, UsbException, InvalidFirmwareException, DeviceLostException {
// scan the bus for comparison
	findOldDevices();
	
// reset the EZ-USB
	EzUsb.reset(handle,true);
	try {
	    EzUsb.reset(handle,false);		// error (may caused by re-numeration) can be ignored
	}
	catch ( FirmwareUploadException e ) {
	}
	
// find and init new device
	initNewDevice( "Device lost after resetting the EZ-USB", true );
    }

// ******* toString ************************************************************
/** 
  * Returns a lot of useful information about the corresponding device.
  * @return a lot of useful information about the corresponding device.
  */
    public String toString () {
	return dev.toString();
    }

}    
