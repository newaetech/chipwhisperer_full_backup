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

/** * Thrown if the FPGA is already configured. */
public class AlreadyConfiguredException extends Exception {
/** 
 * Constructs an instance from the given input string.
 * @param msg A message.
 */
    public AlreadyConfiguredException (String msg) {
	super( "FPGA already configured: "+msg );
    }

/** * Constructs an instance using a standard message. */
    public AlreadyConfiguredException() {
	super( "FPGA already configured" );
    }
}    
