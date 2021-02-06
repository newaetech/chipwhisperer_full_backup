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

/** * Thrown is a required capability is not available. */
public class CapabilityException extends Exception {
/** 
 * Constructs an instance from a givig device and capability string.
 * @param ztex The device.
 * @param cap Capability name.
 */
    public CapabilityException ( Ztex1 ztex, String cap) {
	super( "bus=" + ztex.dev().dev().getBus().getDirname() + "  device=" + ztex.dev().dev().getFilename() + ": " + cap + " not supported" );
    }
}    
