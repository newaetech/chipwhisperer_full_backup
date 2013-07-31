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
/** * 	Signals that an error occurred while attempting to upload the bitstream. */
public class BitstreamUploadException extends Exception {
/** 
 * Constructs an instance from the given error message.
 * @param msg The error message.
 */
        public BitstreamUploadException (String msg) {
	super( "Error uploading bitstream: "+msg );
    }

/** * Constructs an instance using a standard message. */
    public BitstreamUploadException () {
	super( "Error uploading bitstream" );
    }
}    
