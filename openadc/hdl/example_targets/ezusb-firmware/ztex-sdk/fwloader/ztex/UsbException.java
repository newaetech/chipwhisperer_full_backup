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

import ch.ntb.usb.*;
/** * Signals an USB error. */
public class UsbException extends Exception {
/** 
 * Constructs an instance from the given error message.
 * @param msg The error message.
 */
   public UsbException(String msg) {
	super( msg );
    }

/** 
 * Constructs an instance from the given device and error message.
 * @param dev The device.
 * @param msg The error message.
 */
    public UsbException(Usb_Device dev,  String msg) {
	super( "bus=" + dev.getBus().getDirname() + "  device=" + dev.getFilename() + ": " + msg );
    }
}    
