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
    Functions for USB devices with ZTEX descriptor 1, Interface 1
    Interface capabilities and vendor requests (VR) / commands (VC):
    0.0  : EEPROM support
	VR 0x38 : read from EEPROM
	    Returns:
		EEPROM data
	VC 0x39 : write to EEPROM
	VR 0x3a : EEPROM state
	    Returns:
	        Offs	Description
		0-1   	bytes written
		2	checksum
		3	0:idle, 1:busy or error
	    
    0.1  : FPGA Configuration
	VR 0x30 : get FPGA state
	    Returns:
	        Offs	Description
		0	1: unconfigured, 0:configured
	        1	checksum
		2-5	transferred bytes
		6  	INIT_B state
		7	Flash configuration result
		8	Flash bitstream bit order (1=swapped)

	VC 0x31 : reset FPGA
	VC 0x32 : send FPGA configuration data (Bitstream)

    0.2  : Flash memory support
	VR 0x40 : read Flash state
	    Returns:
	        Offs	Description
		0	1:enabled, 0:disabled
		1-2	Sector size
		3-6	Number of sectors
		7	Error code
	VR 0x41 : read from Flash
	VC 0x42 : write to Flash
    0.3  : Debug helper support
	VR 0x28 : read debug data
	    Returns:
	        Offs	Description
		0-1	number of messages
		2	stack size in messages
		3       message size in bytes
		>=4	message stack
    0.4  : XMEGA support
	VR 0x48 : read XMEGA status information
	    Returns:
	        Offs	Description
		0	error code
                1-2     Flash size in pages
                3-4     EEPROM size in pages
                5	Flash page size as power of two	(e.g. 9 means 512 bytes)
                6	EEPROM page size as power of two
	VC 0x49 : reset XMEGA
	VR 0x4A,0x4B,0x4C,0x4D : read XMEGA NVM using PDI address space / relative to Flash address base / EEPROM address base / Fuse address base
	VC 0x4B,0x4C : write exactly one Flash / EEPROM page
	VC 0x4D : write Fuse

    0.5  : High speed FPGA configuration support
	VR 0x33 : Return Endpoint settings
	    Returns:
	        Offs	Description
		0	Endpoint number
		1	Interface number
	VC 0x34 : Start high speed FPGA configuration
	VC 0x35 : Finish high speed FPGA configuration

    0.6  : MAC EEPROM support
	VR 0x3B : read from MAC EEPROM
	    Returns:
		MAC EEPROM data
	VC 0x3C : write to MAC EEPROM
	VR 0x3D : MAC EEPROM state
	    Returns:
	        Offs	Description
		0	0:idle, 1:busy or error
        
    0.7  : Multi-FPGA support
	VR 0x50 : Return multi-FPGA information
	    Returns:
	        Offs	Description
	        0	Number of FPGA's - 1
	        1       Selected FPGA - 1 
	        2       Parallel configuration support (0:no, 1:yes)
	VC 0x51 : set CS
	    Parameters:
	        index: Select command
	    	    0 : select single FPGA
	    	    1 : select all FPGA's for configuration
	        value: FPGA to select - 1

    1.0  : Temperature sensor
	VR 0x58 : Return temperature data
	    Returns:
	        Offs	Description
	        0	Protocol number
	        1..n    Data

*/
package ztex;

import java.io.*;
import java.util.*;

import ch.ntb.usb.*;

/**
  * This class implements the communication protocol of the interface version 1 for the interaction with the ZTEX firmware.
  * <p>
  * The features supported by this interface can be accessed via vendor commands and vendor requests via Endpoint 0.
  * Each feature can be enabled or disabled by the firmware and also depends from the hardware.
  * The presence of a feature is indicated by a 1 in the corresponding feature bit of the ZTEX descriptor 1, see {@link ZtexDevice1}.
  * The following table gives an overview about the features
  * <table bgcolor="#404040" cellspacing=1 cellpadding=10>
  *   <tr>
  *     <td bgcolor="#d0d0d0" valign="bottom"><b>Capability bit</b></td>
  *     <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.0</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *	  EEPROM support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x38</td>
  *           <td bgcolor="#ffffff" valign="top">Read from EEPROM</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VC 0x39</td>
  *           <td bgcolor="#ffffff" valign="top">Write to EEPROM</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x3a</td>
  *           <td bgcolor="#ffffff" valign="top">Get EEPROM state. Returns:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0-1</td>
  *                 <td bgcolor="#ffffff" valign="top">Number of bytes written.</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">2</td>
  *                 <td bgcolor="#ffffff" valign="top">Checksum</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">3</td>
  *                 <td bgcolor="#ffffff" valign="top">0:idle, 1:busy or error</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *       </table>
  *	</td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.1</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *       FPGA Configuration<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x30</td>
  *           <td bgcolor="#ffffff" valign="top">Get FPGA state. Returns:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0</td>
  *                 <td bgcolor="#ffffff" valign="top">1: unconfigured, 0:configured</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">1</td>
  *                 <td bgcolor="#ffffff" valign="top">Checksum</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">2-5</td>
  *                 <td bgcolor="#ffffff" valign="top">Number of bytes transferred.</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">6</td>
  *                 <td bgcolor="#ffffff" valign="top">INIT_B states.</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">7</td>
  *                 <td bgcolor="#ffffff" valign="top">Flash configuration result.</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">8</td>
  *                 <td bgcolor="#ffffff" valign="top">Flash Bitstreambit order (1=swapped).</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VC 0x31</td>
  *           <td bgcolor="#ffffff" valign="top">Reset FPGA</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x32</td>
  *           <td bgcolor="#ffffff" valign="top">Send Bitstream</td>
  *         </tr>
  *       </table>
  *     </td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.2</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *       Flash memory support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x40</td>
  *           <td bgcolor="#ffffff" valign="top">Get Flash state. Returns:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0</td>
  *                 <td bgcolor="#ffffff" valign="top">1:enabled, 0:disabled</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">1-2</td>
  *                 <td bgcolor="#ffffff" valign="top">Sector size</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">3-6</td>
  *                 <td bgcolor="#ffffff" valign="top">Number of sectors</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">7</td>
  *                 <td bgcolor="#ffffff" valign="top">Error code</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x41</td>
  *           <td bgcolor="#ffffff" valign="top">Read one sector from Flash</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VC 0x42</td>
  *           <td bgcolor="#ffffff" valign="top">Write one sector to Flash</td>
  *         </tr>
  *       </table>
  *     </td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.3</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *       Debug helper support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x28</td>
  *           <td bgcolor="#ffffff" valign="top">Get debug data. Returns:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0-1</td>
  *                 <td bgcolor="#ffffff" valign="top">Number of the last message</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">2</td>
  *                 <td bgcolor="#ffffff" valign="top">Stack size in messages</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">3</td>
  *                 <td bgcolor="#ffffff" valign="top">Message size in bytes</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">&ge;4</td>
  *                 <td bgcolor="#ffffff" valign="top">Message stack</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *       </table>
  *     </td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.4</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *       XMEGA support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x48</td>
  *           <td bgcolor="#ffffff" valign="top">Read XMEGA status information. Returns:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0</td>
  *                 <td bgcolor="#ffffff" valign="top">Error code</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">1-2</td>
  *                 <td bgcolor="#ffffff" valign="top">Flash size in pages</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">3-4</td>
  *                 <td bgcolor="#ffffff" valign="top">EEPROM sie in pages</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">5</td>
  *                 <td bgcolor="#ffffff" valign="top">Flash page size as power of two	(e.g. 9 means 512 bytes)</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">6</td>
  *                 <td bgcolor="#ffffff" valign="top">EEPROM page size as power of two</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VC 0x49</td>
  *           <td bgcolor="#ffffff" valign="top">Reset XMEGA</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VRs 0x4A, 0x4B, 0x4C, 0x4D</td>
  *           <td bgcolor="#ffffff" valign="top">Read XMEGA NVM using PDI address space / relative to Flash address base / EEPROM address base / Fuse address base</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VCs 0x4B, 0x4C</td>
  *           <td bgcolor="#ffffff" valign="top">Write exactly one Flash / EEPROM page</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VCs 0x4D</td>
  *           <td bgcolor="#ffffff" valign="top">Write Fuse</td>
  *         </tr>
  *       </table>
  *     </td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.5</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *	  High speed FPGA configuration support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x33</td>
  *           <td bgcolor="#ffffff" valign="top">Read Endpoint settings. Returns:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0</td>
  *                 <td bgcolor="#ffffff" valign="top">Endpoint number</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">1</td>
  *                 <td bgcolor="#ffffff" valign="top">Interface number</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x34</td>
  *           <td bgcolor="#ffffff" valign="top">Start FPGA configuration</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VC 0x35</td>
  *           <td bgcolor="#ffffff" valign="top">Finish FPGA configuration</td>
  *         </tr>
  *       </table>
  *	</td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.6</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *	  MAC EEPROM support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x3B</td>
  *           <td bgcolor="#ffffff" valign="top">Read from MAC EEPROM</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VC 0x3C</td>
  *           <td bgcolor="#ffffff" valign="top">Write to MAC EEPROM</td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x3D</td>
  *           <td bgcolor="#ffffff" valign="top">Get MAC EEPROM state. Returns:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0</td>
  *                 <td bgcolor="#ffffff" valign="top">0:idle, 1:busy or error</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *       </table>
  *	</td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0.7</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *	  Multi-FPGA support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x50</td>
  *           <td bgcolor="#ffffff" valign="top">Return multi-FPGA information:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0</td>
  *                 <td bgcolor="#ffffff" valign="top">Number of FPGA's - 1</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">1</td>
  *                 <td bgcolor="#ffffff" valign="top">Selected FPGA - 1</td>
  *               </tr>
  *               <tr>
  *		    <td bgcolor="#ffffff" valign="top">2</td>
  *		    <td bgcolor="#ffffff" valign="top">Parallel configuration support (0:no, 1:yes)</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VC 0x51</td>
  *           <td bgcolor="#ffffff" valign="top">Parameters:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Parameter</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">index</td>
  *                 <td bgcolor="#ffffff" valign="top">Select command<br> 0: Select single FPGA <br> 1: Select all FPGA's for configuration</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">value</td>
  *                 <td bgcolor="#ffffff" valign="top">FPGA to select - 1</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *       </table>
  *	</td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">1.0</td>
  *     <td bgcolor="#ffffff" valign="top" colspan=2>
  *	  Temperature sensor support<p>
  *       <table bgcolor="#404040" cellspacing=1 cellpadding=6>
  *         <tr>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Vendor request (VR)<br> or command (VC)</b></td>
  *           <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *         </tr>
  *         <tr>
  *           <td bgcolor="#ffffff" valign="top">VR 0x58</td>
  *           <td bgcolor="#ffffff" valign="top">Return temperature data:
  *             <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *               <tr>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *                 <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">0</td>
  *                 <td bgcolor="#ffffff" valign="top">Protocol</td>
  *               </tr>
  *               <tr>
  *                 <td bgcolor="#ffffff" valign="top">1..n</td>
  *                 <td bgcolor="#ffffff" valign="top">Data</td>
  *               </tr>
  *             </table>
  *           </td>
  *         </tr>
  *       </table>
  *	</td>
  *   </tr>
  * </table>
  * @see ZtexDevice1
  * @see Ztex1
  */


public class Ztex1v1 extends Ztex1 {
    /** * Capability index for EEPROM support. */
    public static final int CAPABILITY_EEPROM = 0;
    /** * Capability index for FPGA configuration support. */
    public static final int CAPABILITY_FPGA = 1;
    /** * Capability index for FLASH memory support. */
    public static final int CAPABILITY_FLASH = 2;
    /** * Capability index for DEBUG helper support. */
    public static final int CAPABILITY_DEBUG = 3;
    /** * Capability index for AVR XMEGA support. */
    public static final int CAPABILITY_XMEGA = 4;
    /** * Capability index for AVR XMEGA support. */
    public static final int CAPABILITY_HS_FPGA = 5;
    /** * Capability index for AVR XMEGA support. */
    public static final int CAPABILITY_MAC_EEPROM = 6;
    /** * Capability index for multi FPGA support */
    public static final int CAPABILITY_MULTI_FPGA = 7;
    /** * Capability index for Temperature sensor support */
    public static final int CAPABILITY_TEMP_SENSOR = 8;

    /** * The names of the capabilities */
    public static final String capabilityStrings[] = {
	"EEPROM read/write" ,
	"FPGA configuration" ,
	"Flash memory support",
	"Debug helper",
	"XMEGA support", 
	"High speed FPGA configuration",
	"MAC EEPROM read/write",
	"Multi FPGA Support",
	"Temperature Sensor Support" 
    };
    
    /** * Enables extra FPGA configuration checks. Certain Bistream settings may cause false warnings.  */
    public boolean enableExtraFpgaConfigurationChecks = false;

    private boolean fpgaConfigured = false;
    private int fpgaChecksum = 0;
    private int fpgaBytes = 0;
    private int fpgaInitB = 0;
    private int fpgaFlashResult = 255;
    private boolean fpgaFlashBitSwap = false;
    
    /** * Number of bytes written to EEPROM. (Obtained by {@link #eepromState()}.) */
    public int eepromBytes = 0;
    /** * Checksum of the last EEPROM transfer. (Obtained by {@link #eepromState()}.) */
    public int eepromChecksum = 0;

    private int flashEnabled = -1;
    private int flashSectorSize = -1;
    private int flashSectors = -1;

    /** * Last Flash error code obtained by {@link #flashState()}. See FLASH_EC_* for possible error codes. */
    public int flashEC = 0;
    /** * Means no error. */
    public static final int FLASH_EC_NO_ERROR = 0;
    /** * Signals an error while attempting to execute a command. */
    public static final int FLASH_EC_CMD_ERROR = 1;
    /** * Signals that a timeout occurred. */
    public static final int FLASH_EC_TIMEOUT = 2;
    /** * Signals that Flash memory it busy. */
    public static final int FLASH_EC_BUSY = 3;
    /** * Signals that another Flash operation is pending. */
    public static final int FLASH_EC_PENDING = 4;
    /** * Signals an error while attempting to read from Flash. */
    public static final int FLASH_EC_READ_ERROR = 5;
    /** * Signals an error while attempting to write to Flash. */
    public static final int FLASH_EC_WRITE_ERROR = 6;
    /** * Signals the the installed Flash memory is not supported. */
    public static final int FLASH_EC_NOTSUPPORTED = 7;
    
    private int debugStackSize = -1;
    private int debugMsgSize = -1;
    private int debugLastMsg = 0;
    /** * Is set by {@link #debugReadMessages(boolean,byte[])} and contains the number of new messages. */
    public int debugNewMessages = 0;

    private int xmegaFlashPages = -1;
    private int xmegaEepromPages = -1;
    private int xmegaFlashPageSize;
    private int xmegaEepromPageSize;

    /** * Last ATxmega error code obtained by {@link #xmegaState()}. See XMEGA_EC_* for possible error codes. */
    public int xmegaEC = 0;
    /** * Means no error. */
    public static final int XMEGA_EC_NO_ERROR = 0;
    /** * Signals a PDI read error. */
    public static final int XMEGA_EC_PDI_READ_ERROR = 1;
    /** * Signals that an NVM timeout occurred. */
    public static final int XMEGA_EC_NVM_TIMEOUT = 2;
    /** * Signals that the ATxmega controller is not supported. */
    public static final int XMEGA_EC_INVALID_DEVICE = 3;
    /** * Signals an address error (invalid address or wrong page size). */
    public static final int XMEGA_EC_ADDRESS_ERROR = 4;
    /** * Signals that the NVM is busy. */
    public static final int XMEGA_EC_NVM_BUSY = 5;
    
    private int numberOfFpgas = -1;
    private int selectedFpga = -1;
    private boolean parallelConfigSupport = false;
    
    private long lastTempSensorReadTime = 0;
    private byte[] tempSensorBuf = new byte[9];
    /** * smallest temperature sensor update interval in ms */
    public int tempSensorUpdateInterval = 100;
    
// ******* Ztex1v1 *************************************************************
/** 
  * Constructs an instance from a given device.
  * @param pDev The given device.
  * @throws UsbException if an communication error occurred.
  */
    public Ztex1v1 ( ZtexDevice1 pDev ) throws UsbException {
	super ( pDev );
    }

// ******* valid ***************************************************************
/** 
  * Returns true if ZTEX interface 1 is available.
  * @return true if ZTEX interface 1 is available.
  */
    public boolean valid ( ) {
	return dev().valid() && dev().interfaceVersion()==1;
    }

/** 
  * Returns true if ZTEX interface 1 and capability i.j are available.
  * @param i byte index of the capability
  * @param j bit index of the capability
  * @return true if ZTEX interface 1 and capability i.j are available.
  */
    public boolean valid ( int i, int j) {
	return dev().valid() && dev().interfaceVersion()==1 && dev().interfaceCapabilities(i,j);
    }

// ******* compatible **********************************************************
/** 
  * Checks whether the given product ID is compatible to the device corresponding to this class and whether interface 1 is supported.<br>
  * The given product ID is compatible
  * <pre>if ( this.productId(0)==0 || productId0<=0 || this.productId(0)==productId0 ) && 
   ( this.productId(0)==0 || productId1<=0 || this.productId(1)==productId1 ) && 
   ( this.productId(2)==0 || productId2<=0 || this.productId(2)==productId2 ) && 
   ( this.productId(3)==0 || productId3<=0 || this.productId(3)==productId3 ) </pre>
  * @param productId0 Byte 0 of the given product ID
  * @param productId1 Byte 1 of the given product ID
  * @param productId2 Byte 2 of the given product ID
  * @param productId3 Byte 3 of the given product ID
  * @return true if the given product ID is compatible and interface 1 is supported.
  */
    public boolean compatible ( int productId0, int productId1, int productId2, int productId3 ) {
	return dev().valid() && dev().compatible ( productId0, productId1, productId2, productId3 ) && dev().interfaceVersion()==1;
    }

// ******* checkValid **********************************************************
/** 
  * Checks whether ZTEX descriptor 1 is available and interface 1 is supported.
  * @throws InvalidFirmwareException if ZTEX descriptor 1 is not available or interface 1 is not supported.
  */
    public void checkValid () throws InvalidFirmwareException {
	super.checkValid();
	if ( dev().interfaceVersion() != 1 )
	    throw new InvalidFirmwareException(this, "Wrong interface: " + dev().interfaceVersion() + ", expected: 1" );
    }

// ******* checkCapability *****************************************************
/** 
  * Checks whether ZTEX descriptor 1 is available and interface 1 and a given capability are supported.
  * @param i byte index of the capability
  * @param j bit index of the capability
  * @throws InvalidFirmwareException if ZTEX descriptor 1 is not available or interface 1 is not supported.
  * @throws CapabilityException if the given capability is not supported.
  */
    public void checkCapability ( int i, int j ) throws InvalidFirmwareException, CapabilityException {
	checkValid();
	if ( ! dev().interfaceCapabilities(i,j) ) {
	    int k = i*8 + j;
	    if ( k>=0 && k<capabilityStrings.length )
	    throw new CapabilityException( this, ( k>=0 && k<=capabilityStrings.length ) ? capabilityStrings[k] : ("Capabilty " + i + "," + j) ); 
	}
    }

/** 
  * Checks whether ZTEX descriptor 1 is available and interface 1 and a given capability are supported.
  * @param i capability index (0..47)
  * @throws InvalidFirmwareException if ZTEX descriptor 1 is not available or interface 1 is not supported.
  * @throws CapabilityException if the given capability is not supported.
  */
    public void checkCapability ( int i ) throws InvalidFirmwareException, CapabilityException {
	checkCapability(i/8, i%8);
    }

// ******* checkCompatible *****************************************************
/**
  * Checks whether the given product ID is compatible to the device corresponding to this class and whether interface 1 is supported.
  * See {@link #compatible(int,int,int,int)}.
  * @param productId0 Byte 0 of the given product ID
  * @param productId1 Byte 1 of the given product ID
  * @param productId2 Byte 2 of the given product ID
  * @param productId3 Byte 3 of the given product ID
  * @throws InvalidFirmwareException if the given product ID is not compatible or interface 1 is not supported.
  */
    public void checkCompatible ( int productId0, int productId1, int productId2, int productId3 ) throws InvalidFirmwareException {
	checkValid();
	if ( ! dev().compatible ( productId0, productId1, productId2, productId3 ) )
	    throw new InvalidFirmwareException(this, "Incompatible Product ID");
    }

// ******* getFpgaState ********************************************************
    private void getFpgaState () throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buffer = new byte[9];
	checkCapability(CAPABILITY_FPGA);
	vendorRequest2(0x30, "getFpgaState", buffer, 9);
	fpgaConfigured = buffer[0] == 0;
	fpgaChecksum = buffer[1] & 0xff;
	fpgaBytes = ((buffer[5] & 0xff)<<24) | ((buffer[4] & 0xff)<<16) | ((buffer[3] & 0xff)<<8) | (buffer[2] & 0xff);
	fpgaInitB = buffer[6] & 0xff;
	fpgaFlashResult = buffer[7];
	fpgaFlashBitSwap = buffer[8] != 0;
    }

// ******* printFpgaState ******************************************************
/**
  * Prints out the FPGA state.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public void printFpgaState () throws UsbException, InvalidFirmwareException, CapabilityException {

	final String flashResultStr[] = {
	 "Configuration successful",
	 "FPGA already configured",
	 "Flash error",
	 "No bitstream found",
	 "Configuration error"
	};
    
	getFpgaState();
	System.out.println( "size=" + fpgaBytes + ";  checksum=" + fpgaChecksum + "; INIT_B_HIST=" + fpgaInitB +"; flash_configuration_result=" + fpgaFlashResult + 
	  (fpgaFlashResult>=0 && fpgaFlashResult<flashResultStr.length ? " ("+flashResultStr[fpgaFlashResult]+")" : "" ) 
	);
    }

// ******* getFpgaConfiguration ************************************************
/**
  * Returns true if the FPGA is configured.
  * @return true if the FPGA is configured.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public boolean getFpgaConfiguration () throws UsbException, InvalidFirmwareException, CapabilityException {
	getFpgaState ();
	return fpgaConfigured;
    }

// ******* getFpgaConfigurationStr *********************************************
/**
  * Returns a string that indicates the FPGA configuration status.
  * @return a string that indicates the FPGA configuration status.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public String getFpgaConfigurationStr () throws UsbException, InvalidFirmwareException, CapabilityException {
	getFpgaState ();
	return fpgaConfigured ? "FPGA configured" : "FPGA unconfigured";
    }

// ******* resetFGPA ***********************************************************
/**
  * Resets the FPGA.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public void resetFpga () throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_FPGA);
	vendorCommand(0x31, "resetFpga" );
    }


// ******* detectBitstreamBitOrder *********************************************
    private int detectBitstreamBitOrder ( byte[] buf ) {
	for ( int i=0; i<buf.length-3; i++ ) {
	    if ( ((buf[i] & 255)==0xaa) && ((buf[i+1] & 255)==0x99) && ((buf[i+2] & 255)==0x55) && ((buf[i+3] & 255)==0x66) )
		return 1;
	    if ( ((buf[i] & 255)==0x55) && ((buf[i+1] & 255)==0x99) && ((buf[i+2] & 255)==0xaa) && ((buf[i+3] & 255)==0x66) )
		return 0;
	} 
	System.err.println("Warning: Unable to determine bitstream bit order: no signature found");
	return 0;
    }

// ******* swapBits ************************************************************
    private void swapBits ( byte[][] buf, int size ) {
	int j=0, k=0;
	for (int i=0; i<size; i++ ) {
	    while ( k >= buf[j].length ) {
		j++;
		k=0;
	    }
	    byte b = buf[j][k];
	    buf[j][k] = (byte) ( ((b & 128) >> 7) |
 		     	         ((b &  64) >> 5) |
		     	         ((b &  32) >> 3) |
		     	         ((b &  16) >> 1) |
		     	         ((b &   8) << 1) |
		     	         ((b &   4) << 3) |
		     	         ((b &   2) << 5) |
		     	         ((b &   1) << 7) );
	    k++;
	}
    }

// ******* configureFpgaLS *****************************************************
//  returns configuration time in ms
/**
  * Upload a Bitstream to the FPGA using low speed mode.
  * @param fwFileName The file name of the Bitstream. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @param force If set to true existing configurations will be overwritten. (By default an {@link AlreadyConfiguredException} is thrown).
  * @param bs 0: disable bit swapping, 1: enable bit swapping, all other values: automatic detection of bit order.
  * @throws BitstreamReadException if an error occurred while attempting to read the Bitstream.
  * @throws BitstreamUploadException if an error occurred while attempting to upload the Bitstream.
  * @throws AlreadyConfiguredException if the FPGA is already configured.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public long configureFpgaLS ( String fwFileName, boolean force, int bs ) throws BitstreamReadException, UsbException, BitstreamUploadException, AlreadyConfiguredException, InvalidFirmwareException, CapabilityException {
	final int transactionBytes = certainWorkarounds ? 256 : 2048;
	long t0 = 0;

	checkCapability(CAPABILITY_FPGA);
	
	if ( !force && getFpgaConfiguration() )
	    throw new AlreadyConfiguredException(); 

// read the Bitstream file	
        byte[][] buffer = new byte[16*1024*1024/transactionBytes][];
	int size = 0;
	try {
	    InputStream inputStream = JInputStream.getInputStream( fwFileName );
	    int j = transactionBytes;
	    for ( int i=0; i<buffer.length && j==transactionBytes; i++ ) {
		buffer[i] = new byte[transactionBytes]; 
		int k;
		j = 0;	
		do {
		    k = inputStream.read( buffer[i], j, transactionBytes-j );
		    if ( k < 0 ) 
		        k = 0;
		    j += k;
		}
		while ( j<transactionBytes && k>0 );

		if ( j < transactionBytes && j % 64 == 0 )	// ensures size % 64 != 0
		    j+=1;
		size += j;
	    }
	    
	    try {
		inputStream.close();
	    }
	    catch ( Exception e ) {
		System.err.println( "Warning: Error closing file " + fwFileName + ": " + e.getLocalizedMessage() );
	    }
	}
	catch (IOException e) {
	    throw new BitstreamReadException(e.getLocalizedMessage());
	}
	if ( size < 64 || size % 64 == 0 ) 
	    throw new BitstreamReadException("Invalid file size: " + size );

// detect bitstream bit order and swap bits if necessary 
	if ( bs<0 || bs>1 )
	    bs = detectBitstreamBitOrder ( buffer[0] );
	if ( bs == 1 )
	    swapBits(buffer,size);
	    
// upload the Bitstream file	
	for ( int tries=10; tries>0; tries-- ) {
	    
	    resetFpga();

	    try {
		t0 = -new Date().getTime();
		int cs = 0;
		bs = 0;
		    
	    	for ( int i=0; i<buffer.length && i*transactionBytes < size; i++ ) {
		    int j = size-i*transactionBytes;
		    if (j>transactionBytes) 
			j = transactionBytes;
		    vendorCommand2(0x32, "sendFpgaData", 0,0, buffer[i], j);

		    bs+=j;
		    for ( int k=0; k<buffer[i].length; k++ ) 
		        cs = ( cs + (buffer[i][k] & 0xff) ) & 0xff;
		}

 		getFpgaState();
//		System.err.println("fpgaConfigred=" + fpgaConfigured + "   fpgaBytes="+fpgaBytes + " ("+bs+")   fpgaChecksum="+fpgaChecksum + " ("+cs+")   fpgaInitB="+fpgaInitB );
		if ( ! fpgaConfigured ) {
		    throw new BitstreamUploadException( "FPGA configuration failed: DONE pin does not go high (size=" + fpgaBytes + " ,  " + (bs - fpgaBytes) + " bytes got lost;  checksum=" 
			+ fpgaChecksum + " , should be " + cs + ";  INIT_B_HIST=" + fpgaInitB +")" );
		}
		if ( enableExtraFpgaConfigurationChecks ) {
	    	    if ( fpgaBytes!=0 && fpgaBytes!=bs )
			System.err.println("Warning: Possible FPGA configuration data loss: " + (bs - fpgaBytes) + " bytes got lost");
		    if ( fpgaInitB!=222 )
			System.err.println("Warning: Possible Bitstream CRC error: INIT_B_HIST=" + fpgaInitB );
		}
//		System.out.println( "FPGA configuration: size=" + fpgaBytes + " ,  " + (bs - fpgaBytes) + " bytes got lost;  checksum=" + fpgaChecksum + " , should be " + cs + ";  INIT_B_HIST=" + fpgaInitB );
			
		tries = 0;
		t0 += new Date().getTime();

	    } 
	    catch ( BitstreamUploadException e ) {
		if ( tries>1 ) 
		    System.err.println("Warning: " + e.getLocalizedMessage() +": Retrying it ...");
		else 
		    throw e;
	    }
	}

    	try {
    	    Thread.sleep( 100 );
    	}
	catch ( InterruptedException e) {
        } 
	
	return t0;
    } 


// ******* eepromState *********************************************************
// returns true if EEPROM is ready
/**
  * Reads the current EEPROM status.
  * This method also sets the varibles {@link #eepromBytes} and {@link #eepromChecksum}.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if EEPROM access is not supported by the firmware.
  * @return true if EEPROM is ready.
  */
    public boolean eepromState ( ) throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buf = new byte[4];
	checkCapability(CAPABILITY_EEPROM);
	vendorRequest2(0x3A, "EEPROM State", 0, 0, buf, 4);
	eepromBytes = (buf[0] & 255) | (buf[1] & 255)<<8;
	eepromChecksum = buf[2] & 255;
	return buf[3] == 0;
    }

// ******* eepromWrite *********************************************************
/**
  * Writes data to the EEPROM.
  * @param addr The destination address of the EEPROM.
  * @param buf The data.
  * @param length The amount of bytes to be sent.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if EEPROM access is not supported by the firmware.
  */
    public void eepromWrite ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_EEPROM);
	if ( (addr & 63) != 0 ) {
	    int i = Math.min(length, 64-(addr & 63));
	    vendorCommand2( 0x39, "EEPROM Write", addr, 0, buf, i );
    	    try {
    		Thread.sleep( 10 );
	    }
	    catch ( InterruptedException e) {
    	    } 
    	    addr+=i;
    	    length-=i;
    	    if ( length > 0 ) {
    		byte[] buf2 = new byte[length];
    		for (int j=0; j<length; j++ ) 
    		    buf2[j] = buf[i+j];
    		vendorCommand2( 0x39, "EEPROM Write", addr, 0, buf2, length );
    	    }
	}
	else {
	    vendorCommand2( 0x39, "EEPROM Write", addr, 0, buf, length );
	}
	
        try {
		Thread.sleep( 10 );
	}
	    catch ( InterruptedException e) {
    	} 
    }

// ******* eepromRead **********************************************************
/**
  * Reads data from the EEPROM.
  * @param addr The source address of the EEPROM.
  * @param buf A buffer for the storage of the data.
  * @param length The amount of bytes to be read.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if EEPROM access is not supported by the firmware.
  */
    public void eepromRead ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_EEPROM);
	vendorRequest2( 0x38, "EEPROM Read", addr, 0, buf, length );		// sometimes a little bit slow
    	try {
    	    Thread.sleep( 10 );
    	}
	catch ( InterruptedException e) {
        } 
    }

// ******* eepromUpload ********************************************************
//  returns upload time in ms
/**
  * Upload the firmware to the EEPROM.
  * In order to start the uploaded firmware the device must be reset.
  * @param ihxFile The firmware image.
  * @param force Skips the compatibility check if true.
  * @throws IncompatibleFirmwareException if the given firmware is not compatible to the installed one, see {@link #compatible(int,int,int,int)} (Upload can be enforced using the <tt>force</tt> parameter.)
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws CapabilityException if EEPROM access is not supported by the firmware.
  * @throws FirmwareUploadException if an error occurred while attempting to upload the firmware.
  */
    public long eepromUpload ( ZtexIhxFile1 ihxFile, boolean force ) throws IncompatibleFirmwareException, FirmwareUploadException, InvalidFirmwareException, CapabilityException {
	final int pagesMax = 256;
	final int pageSize = 256;
	int pages = 0;
	byte[][] buffer = new byte[pagesMax][];

	checkCapability(CAPABILITY_EEPROM);

//	ihxFile.dataInfo(System.out);
//	System.out.println(ihxFile);
	
// check for compatibility
	if ( ! force && dev().valid() ) {
	    if ( ihxFile.interfaceVersion() != 1 )
		throw new IncompatibleFirmwareException("Wrong interface version: Expected 1, got " + ihxFile.interfaceVersion() );
	
	    if ( ! dev().compatible ( ihxFile.productId(0), ihxFile.productId(1), ihxFile.productId(2), ihxFile.productId(3) ) )
		throw new IncompatibleFirmwareException("Incompatible productId's: Current firmware: " + ZtexDevice1.byteArrayString(dev().productId()) 
		    + "  Ihx File: " + ZtexDevice1.byteArrayString(ihxFile.productId()) );
	}

	Usb_Device_Descriptor dd = dev().dev().getDescriptor();
	int vid = dd.getIdVendor() & 65535;
	int pid = dd.getIdProduct() & 65535;

	buffer[0] = new byte[pageSize];
	buffer[0][0] = (byte) 0xc2;
	buffer[0][1] = (byte) (vid & 255);
	buffer[0][2] = (byte) ((vid >> 8) & 255);
	buffer[0][3] = (byte) (pid & 255);
	buffer[0][4] = (byte) ((pid >> 8) & 255);
	buffer[0][5] = 0;
	buffer[0][6] = 0;
	buffer[0][7] = 0;
	
	int ptr = 8, i = 0;
	
	while ( i < ihxFile.ihxData.length ) {
	    if ( ihxFile.ihxData[i]>=0 && ihxFile.ihxData[i]<256 ) {			// new data block
		int j = 1;
		while ( i+j<ihxFile.ihxData.length && ihxFile.ihxData[i+j]>=0 && ihxFile.ihxData[i+j]<256 ) 
		    j++;

		for (int k=ptr/pageSize + 1; k < (ptr+j+9)/pageSize + 1; k++ )	// also considers 5 bytes for the last data block
		    buffer[k] = new byte[pageSize];

		buffer[(ptr+0)/pageSize][(ptr+0) % pageSize] = (byte) ((j >> 8) & 255);	
		buffer[(ptr+1)/pageSize][(ptr+1) % pageSize] = (byte) (j & 255);		// length
		buffer[(ptr+2)/pageSize][(ptr+2) % pageSize] = (byte) ((i >> 8) & 255);
		buffer[(ptr+3)/pageSize][(ptr+3) % pageSize] = (byte) (i & 255);		// address
		ptr+=4;
		for ( int k=0; k<j; k++ )  					// data
		    buffer[(ptr+k)/pageSize][(ptr+k) % pageSize] = (byte) ihxFile.ihxData[i+k];
		ptr+=j;
		i+=j;
	    }
	    else {
		i+=1;
	    }
	}
	
	buffer[(ptr+0)/pageSize][(ptr+0) % pageSize] = (byte) 0x80;		// last data block
	buffer[(ptr+1)/pageSize][(ptr+1) % pageSize] = (byte) 0x01;
	buffer[(ptr+2)/pageSize][(ptr+2) % pageSize] = (byte) 0xe6;
	buffer[(ptr+3)/pageSize][(ptr+3) % pageSize] = (byte) 0x00;
	buffer[(ptr+3)/pageSize][(ptr+4) % pageSize] = (byte) 0x00;
	ptr+=5;


	long t0 = new Date().getTime();
	byte[] rbuf = new byte[pageSize];

	for ( i=(ptr-1)/pageSize; i>=0; i-- ) {
	    
	    int k = (i+1)*pageSize < ptr ? pageSize : ptr-i*pageSize;
	    int cs = 0;
	    for (int j=0; j<k; j++ ) {
		cs = ( cs + (buffer[i][j] & 255) ) & 255;
	    }

	    for ( int tries=3; tries>0; tries-- ) {
	    	try {
		    eepromWrite(i*pageSize, buffer[i], k);
		    eepromState();
		    if ( eepromBytes!=k )
			throw new FirmwareUploadException("Error writing data to EEPROM: Wrote " + eepromBytes + " bytes instead of "  + k + " bytes" );
		    if ( eepromChecksum!=cs )
			throw new FirmwareUploadException("Error writing data to EEPROM: Checksum error");

    		    eepromRead(i*pageSize, rbuf, k);
		    for (int j=0; j<k; j++ ) {
			if ( rbuf[j] != buffer[i][j] )
			    throw new FirmwareUploadException("Error writing data to EEPROM: Verification failed");
		    }
		    tries = 0;
		}
		catch ( Exception e ) {
		    if ( tries > 1 ) {
			System.err.println("Warning: " + e.getLocalizedMessage() +": Retrying it ...");
		    }
		    else {
			throw new FirmwareUploadException(e.getLocalizedMessage());
		    }
		}
	    } 
	}
	
	return new Date().getTime() - t0;
    }

//  returns upload time in ms
/**
  * Upload the firmware to the EEPROM.
  * In order to start the uploaded firmware the device must be reset.
  * @param ihxFileName The file name of the firmware image in ihx format. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @param force Skips the compatibility check if true.
  * @throws IncompatibleFirmwareException if the given firmware is not compatible to the installed one, see {@link #compatible(int,int,int,int)} (Upload can be enforced using the <tt>force</tt> parameter.)
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws CapabilityException if EEPROM access is not supported by the firmware.
  * @throws FirmwareUploadException if an error occurred while attempting to upload the firmware.
  */
    public long eepromUpload ( String ihxFileName, boolean force ) throws IncompatibleFirmwareException, FirmwareUploadException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_EEPROM);
	
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
	
	return eepromUpload( ihxFile, force );
    }


// ******* eepromDisable ********************************************************
/**
  * Disables the firmware stored in the EEPROM.
  * This is achived by writing a "0" to the address 0 of the EEPROM.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws CapabilityException if EEPROM access is not supported by the firmware.
  * @throws FirmwareUploadException if an error occurred while attempting to disable the firmware.
  */
    public void eepromDisable ( ) throws FirmwareUploadException, InvalidFirmwareException, CapabilityException {
	byte[] buf = { 0 };

	for ( int tries=3; tries>0; tries-- ) {
	    try {
	        eepromWrite(0, buf, 1);

    		eepromRead(0, buf, 1);
		if ( buf[0] != 0 )
		    throw new FirmwareUploadException("Error disabling EEPROM firmware: Verification failed");
		tries = 0;

    	    }
	    catch ( Exception e ) {
	        if ( tries > 1 ) {
	    	    System.err.println("Warning: " + e.getLocalizedMessage() +": Retrying it ...");
		}
		else {
		    throw new FirmwareUploadException(e.getLocalizedMessage());
		}
	    } 
	}
    } 

// ******* flashStrError *******************************************************
/** 
  * Converts a given error code into a String.
  * @param errNum The error code.
  * @return an error message.
  */
    public static String flashStrError ( int errNum ) {
	switch ( errNum ) {
	    case FLASH_EC_NO_ERROR:
		return "USB error: " + LibusbJava.usb_strerror();
	    case FLASH_EC_CMD_ERROR:
		return "Command error";
	    case FLASH_EC_TIMEOUT:
		return "Timeout error";
	    case FLASH_EC_BUSY:
		return "Busy";
	    case FLASH_EC_PENDING:
		return "Another operation is pending";
	    case FLASH_EC_READ_ERROR:
		return "Read error";
	    case FLASH_EC_WRITE_ERROR:
		return "Write error";
	    case FLASH_EC_NOTSUPPORTED:
		return "Not supported";
	}
	return "Error " + errNum;
    }

/** 
  * Gets the last Flash error from the device.
  * @return an error message.
  */
    public String flashStrError ( ) {
	try {
	    return flashStrError( getFlashEC() );
	}
	catch ( Exception e ) {
	    return "Unknown error (Error receiving errorcode: "+e.getLocalizedMessage() +")";
	}
    }

// ******* flashState **********************************************************
/**
  * Reads the the Flash memory status and information.
  * This method also sets the variables {@link #flashEnabled()}, {@link #flashSectorSize()} and {@link #flashSectors()}.
  * @return true if Flash memory is installed.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not supported by the firmware.
  */
    public boolean flashState () throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buf = new byte[8];
	checkCapability(CAPABILITY_FLASH);

	// device may be busy due to initialization, we try it up to up to 4s
	vendorRequest2(0x40, "Flash State", 0, 0, buf, 8);
    	flashEC = buf[7] & 255;
	int tries=20;	
	while ( flashEC==FLASH_EC_BUSY && tries>0 )
	{
	    try {
    		Thread.sleep( 200 );
    	    }
	    catch ( InterruptedException e) {
    	    } 
	    tries-=1;
	    vendorRequest2(0x40, "Flash State", 0, 0, buf, 8);
    	    flashEC = buf[7] & 255;
	}
	flashEnabled = buf[0] & 255;
	flashSectorSize = flashEnabled == 1 ? ((buf[2] & 255) << 8) | (buf[1] & 255) : 0;
	flashSectors = flashEnabled == 1 ? ((buf[6] & 255) << 24) | ((buf[5] & 255) << 16) | ((buf[4] & 255) << 8) | (buf[3] & 255) : 0;
	return flashEnabled == 1;
    }

// ******* getFlashEC **********************************************************
// reads the current error code
/**
  * Gets the last Flash error from the device.
  * @return The last error code.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not supported by the firmware.
  */
    public int getFlashEC () throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buf = new byte[8];
	checkCapability(CAPABILITY_FLASH);
	vendorRequest2(0x40, "Flash State", 0, 0, buf, 8);
    	flashEC = buf[7] & 255;
	return flashEC;
    }

// ******* flashReadSector ****************************************************
// read exactly one sector
/**
  * Reads one sector from the Flash.
  * @param sector The sector number to be read.
  * @param buf A buffer for the storage of the data.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  * @throws IndexOutOfBoundsException If the buffer is smaller than the Flash sector size.
  */
    public void flashReadSector ( int sector, byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException  {
	if ( buf.length < flashSectorSize() ) 
	    throw new IndexOutOfBoundsException( "Buffer smaller than the Flash sector size: " + buf.length + " < " + flashSectorSize());

	checkCapability(CAPABILITY_FLASH);
	if ( ! flashEnabled() )
	    throw new CapabilityException(this, "No Flash memory installed or");

	try {
	    vendorRequest2( 0x41, "Flash Read", sector, sector >> 16, buf, flashSectorSize );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "Flash Read: " + flashStrError() ); 
	}
    }

// read a integer number of sectors
/**
  * Reads a integer number of sectors from the Flash.
  * @param sector The number of the first sector to be read.
  * @param num The number of sectors to be read.
  * @param buf A buffer for the storage of the data.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  * @throws IndexOutOfBoundsException If the buffer is to small.
  */
    public void flashReadSector ( int sector, int num, byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException  {
	if ( buf.length < flashSectorSize() ) 
	    throw new IndexOutOfBoundsException( "Buffer is to small: " + buf.length + " < " + (num*flashSectorSize()) );

	checkCapability(CAPABILITY_FLASH);
	if ( ! flashEnabled() )
	    throw new CapabilityException(this, "No Flash memory installed or");

	try {
	    vendorRequest2( 0x41, "Flash Read", sector, sector >> 16, buf, flashSectorSize*num );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "Flash Read: " + flashStrError() ); 
	}
    }

// ******* flashWriteSector ***************************************************
// write exactly one sector
/**
  * Writes one sector to the Flash.
  * @param sector The sector number to be written.
  * @param buf The data.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  * @throws IndexOutOfBoundsException If the buffer is smaller than the Flash sector size.
  */
    public void flashWriteSector ( int sector, byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException {
	if ( buf.length < flashSectorSize() ) 
	    throw new IndexOutOfBoundsException( "Buffer smaller than the Flash sector size: " + buf.length + " < " + flashSectorSize());

	checkCapability(CAPABILITY_FLASH);
	if ( ! flashEnabled() )
	    throw new CapabilityException(this, "No Flash memory installed or");

	try {
	    vendorCommand2( 0x42, "Flash Write", sector, sector >> 16, buf, flashSectorSize );
	}
	catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "Flash Write: " + flashStrError() );
	}
    }

// ******* flashWriteSector ***************************************************
// write integer number of sectors
/**
  * Writes a integer number of sectors to the Flash.
  * @param sector The sector number to be written.
  * @param num The number of sectors to be read.
  * @param buf The data.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  * @throws IndexOutOfBoundsException If the buffer is to small.
  */
    public void flashWriteSector ( int sector, int num, byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException {
	if ( buf.length < flashSectorSize() ) 
	    throw new IndexOutOfBoundsException( "Buffer smaller than the Flash sector size: " + buf.length + " < " + (num*flashSectorSize()));

	checkCapability(CAPABILITY_FLASH);
	if ( ! flashEnabled() )
	    throw new CapabilityException(this, "No Flash memory installed or");

	try {
	    vendorCommand2( 0x42, "Flash Write", sector, sector >> 16, buf, flashSectorSize*num );
	}
	catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "Flash Write: " + flashStrError() );
	}
    }

// ******* flashEnabled ********************************************************
// returns enabled / disabled state 
/**
  * Returns true if Flash memory is installed.
  * @return true if Flash memory is installed.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not supported by the firmware.
  */
    public boolean flashEnabled () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( flashEnabled < 0 ) // init variable
	    flashState();
	return flashEnabled == 1;
    }

// ******* flashSectorSize *****************************************************
// returns sector size of Flash memory, if available
/**
  * Returns the sector size of the Flash memory or 0, if no flash memory is installed.
  * If required, the sector size is determined form the device first.
  * @return the sector size of the Flash memory.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not supported by the firmware.
  */
    public int flashSectorSize () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( flashSectorSize < 0 ) // init variable
	    flashState();
	return flashSectorSize;
    }

// ******* flashSectors ********************************************************
// returns number of sectors of Flash memory, if available
/**
  * Returns the number of sectors of the Flash memory or 0, if no Flash memory is installed.
  * If required, the number of sectors is determined form the device first.
  * @return the number of sectors of the Flash memory.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not supported by the firmware.
  */
    public int flashSectors () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( flashSectors < 0 ) // init variable
	    flashState();
	return flashSectors;
    }

// ******* flashSize ***********************************************************
// returns size of Flash memory, if available
/**
  * Returns the size of Flash memory or 0, if no Flash memory is installed.
  * If required, the Flash size is determined form the device first.
  * @return the size of Flash memory.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not supported by the firmware.
  */
    public long flashSize () throws UsbException, InvalidFirmwareException, CapabilityException {
	return flashSectorSize() * (long)flashSectors();
    }

// ******* printMmcState *******************************************************
// returns true if Flash is available
/**
  * Prints out some debug information about *SD/MMC Flash cards in SPI mode.<br>
  * <b>Only use this method if such kind of Flash is installed.</b>
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not supported by the firmware.
  */
    public boolean printMmcState ( ) throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buf = new byte[23];
	checkCapability(CAPABILITY_FLASH);
	vendorRequest2(0x43, "MMC State", 0, 0, buf, 23);
	System.out.println("status=" + Integer.toBinaryString(256+(buf[0] & 255)).substring(1) + "." + Integer.toBinaryString(256+(buf[1] & 255)).substring(1) + 
		"   lastCmd=" + buf[3] + 
		"   lastCmdResponse=" + Integer.toBinaryString(256+(buf[4] & 255)).substring(1) + 
		"   ec=" + buf[2] +
		"   BUSY=" + buf[22] + 
		"   SDHC=" + buf[5] + 
		"   buf=" + (buf[6] & 255)+" "+(buf[7] & 255)+" "+(buf[8] & 255)+" "+(buf[9] & 255)+" "+(buf[10] & 255)+" "+(buf[11] & 255)+"  "+(buf[12] & 255)); // +" "+(buf[13] & 255)+" "+(buf[14] & 255)+" "+(buf[15] & 255)+" "+(buf[16] & 255)+" "+(buf[17] & 255));

	return flashEnabled == 1;
    }

// ******* flashUploadBitstream ************************************************
/* 
    Returns configuration time in ms.
    The format of the boot sector (sector 0 of the Flash memory) is
	0..7	
	8..9	Number of sectors, or 0 is disabled
	10..11  Number of bytes in the last sector, i.e. th total size of Bitstream is ((bs[8] | (bs[9]<<8) - 1) * flash_sector_size + ((bs[10] | (bs[11]<<8))
*/	
/**
  * Uploads a Bitstream to the Flash.
  * This allows the firmware to load the Bitstream from Flash. Together with installation of the firmware in EEPROM
  * it is possible to construct fully autonomous devices.
  * <p>
  * Information about the bitstream is stored in sector 0.
  * This so called boot sector has the following format:
  * <table bgcolor="#404040" cellspacing=1 cellpadding=4>
  *   <tr>
  *     <td bgcolor="#d0d0d0" valign="bottom"><b>Bytes</b></td>
  *     <td bgcolor="#d0d0d0" valign="bottom"><b>Description</b></td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">0..7</td>
  *     <td bgcolor="#ffffff" valign="top">ID, must be "ZTEXBS",1,1</td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">8..9</td>
  *     <td bgcolor="#ffffff" valign="top">The number of sectors used to store the Bitstream. 0 means no Bitstream.</td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">10..11</td>
  *     <td bgcolor="#ffffff" valign="top">The number of bytes in the last sector.</td>
  *   </tr>
  *   <tr>
  *     <td bgcolor="#ffffff" valign="top">12..sectorSize-1</td>
  *     <td bgcolor="#ffffff" valign="top">This data is reserved for future use and preserved by this method.</td>
  *   </tr>
  * </table>
  * <p>
  * The total size of the Bitstream is computed as ((bs[8] | (bs[9]<<8) - 1) * flash_sector_size + ((bs[10] | (bs[11]<<8))
  * where bs[i] denotes byte i of the boot sector.
  * <p>
  * The first sector of the Bitstream is sector 1.
  * @param fwFileName The file name of the Bitstream. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @param bs 0: disable bit swapping, 1: enable bit swapping, all other values: automatic detection of bit order.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  * @throws BitstreamReadException if an error occurred while attempting to read the Bitstream.
  */
    public long flashUploadBitstream ( String fwFileName, int bs ) throws BitstreamReadException, UsbException, InvalidFirmwareException, CapabilityException {
	int secNum = 2048 / flashSectorSize;
	final int bufferSize = secNum * flashSectorSize;
	checkCapability(CAPABILITY_FPGA);
	checkCapability(CAPABILITY_FLASH);
	if ( ! flashEnabled() )
	    throw new CapabilityException(this, "No Flash memory installed or");
	getFpgaState();
	
// read the Bitstream file	
        byte[][] buffer = new byte[32768][];
	int i,j,k;
	try {
	    InputStream inputStream = JInputStream.getInputStream( fwFileName );
	    j = bufferSize;
	    for ( i=0; i<buffer.length && j==bufferSize; i++ ) {
		buffer[i] = new byte[bufferSize]; 
		j = 0;	
		do {
		    k = inputStream.read( buffer[i], j, bufferSize-j );
		    if ( k < 0 ) 
		        k = 0;
		    j += k;
		}
		while ( j<bufferSize && k>0 );
	    }
	    
	    try {
		inputStream.close();
	    }
	    catch ( Exception e ) {
		System.err.println( "Warning: Error closing file " + fwFileName + ": " + e.getLocalizedMessage() );
	    }
	}
	catch (IOException e) {
	    throw new BitstreamReadException(e.getLocalizedMessage());
	}

// detect bitstream bit order and swap bits if necessary 
	if ( bs<0 || bs>1 )
	    bs = detectBitstreamBitOrder(buffer[0]);
	if ( fpgaFlashBitSwap != (bs==1) )
	    swapBits( buffer, bufferSize*i );

// upload the Bitstream file	
	byte[] sector = new byte[flashSectorSize];
	byte[] ID = new String("ZTEXBS").getBytes(); 

	flashReadSector(0,sector);				// read the boot sector (only the first 16 bytes are overwritten)
	for (k=0; k<6; k++)
	    sector[k]=ID[k];
	sector[6] = 1;
	sector[7] = 1;
	k = (i-1)*secNum + (j-1)/flashSectorSize + 1;
	sector[8] = (byte) (k & 255);
	sector[9] = (byte) ((k>>8) & 255);
	k = ((j-1) % flashSectorSize) + 1;
	sector[10] = (byte) (k & 255);
	sector[11] = (byte) ((k>>8) & 255);
	long t0 = new Date().getTime();
	flashWriteSector(0,sector);				// write the boot sector
	for (k=0; k<i-1; k++)
	    flashWriteSector( 1+k*secNum, secNum, buffer[k] );	// write the Bitstream sectors
	flashWriteSector( 1+k*secNum, (j-1)/flashSectorSize + 1, buffer[k] );

	return new Date().getTime() - t0;
    } 

/**
  * Uploads a Bitstream to the Flash.
  * This allows the firmware to load the Bitstream from Flash. Together with installation of the firmware in EEPROM
  * it is possible to construct fully autonomous devices.
  * See {@link #flashUploadBitstream(String,int)} for further details.
  * @param fwFileName The file name of the Bitstream. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  * @throws BitstreamReadException if an error occurred while attempting to read the Bitstream.
  */
    public long flashUploadBitstream ( String fwFileName ) throws BitstreamReadException, UsbException, InvalidFirmwareException, CapabilityException {
	return flashUploadBitstream(fwFileName, -1);
    }

// ******* flashResetBitstream *************************************************
// Clears a Bitstream from the Flash.
/**
  * Clears a Bitstream from the Flash.
  * This is achieved by writing 0 to bytes 8..9 of the boot sector, see {@link #flashUploadBitstream(String)}.
  * If no boot sector is installed the method returns without any write action.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  */
    public void flashResetBitstream ( ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_FLASH);
	if ( ! flashEnabled() )
	    throw new CapabilityException(this, "No Flash memory installed or");
	byte[] sector = new byte[flashSectorSize];
	byte[] ID = new String("ZTEXBS").getBytes(); 

	flashReadSector(0,sector);			// read the boot sector
	for (int k=0; k<6; k++)
	    if ( sector[k] != ID[k] )
		return;
	if (sector[6]!=1 || sector[7]!=1 )
	    return;
	sector[8] = 0;
	sector[9] = 0;
	flashWriteSector(0,sector);			// write the boot sector
    } 

// ******* flashFirstFreeSector ************************************************
// Returns the first free sector of the Flash memory, i.e. the first sector behind the Bitstream
/**
  * Returns the first free sector of the Flash memory.
  * This is the first sector behind the Bitstream, or 0 if no boot sector is installed (or 1 if a boot sector but no Bitstream is installed).
  * @return the first free sector of the Flash memory.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  */
    public int flashFirstFreeSector ( ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_FLASH);
	if ( ! flashEnabled() )
	    throw new CapabilityException(this, "No Flash memory installed or");

	byte[] sector = new byte[flashSectorSize];
	byte[] ID = new String("ZTEXBS").getBytes(); 

	flashReadSector(0,sector);			// read the boot sector
	for (int k=0; k<6; k++)
	    if ( sector[k] != ID[k] )
		return 0;
	if (sector[6]!=1 || sector[7]!=1 )
	    return 0;
	return (sector[8] & 255) + ((sector[9] & 255) << 8) + 1;
    }
    

// ******* debugStackSize ******************************************************
/**
  * Returns the size of message stack in messages.
  * @return the size of message stack in messages.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  */
    public int debugStackSize ( ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_DEBUG);
	if ( debugStackSize<=0 || debugMsgSize<=0 ) {
	    byte[] buf = new byte[7];
	    vendorRequest2(0x28, "Read debug data", 0, 0, buf, 4);
	    debugStackSize = buf[2] & 255;
	    debugMsgSize = buf[3] & 255;
	}
	return debugStackSize;
    }

// ******* debugMsgSize ********************************************************
/**
  * Returns the size of messages in bytes.
  * @return the size of messages in bytes.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  */
    public int debugMsgSize ( ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_DEBUG);
	if ( debugMsgSize<=0 ) 
	    debugStackSize();
	
	return debugMsgSize;
    }

// ******* debugLastMsg ********************************************************
/**
  * Returns the number of the last message read out by {@link #debugReadMessages(boolean,byte[])}
  * @return the number of the last message read out by {@link #debugReadMessages(boolean,byte[])}
  */
    public final int debugLastMsg ( )  {
	return debugLastMsg;
    }

// ******* debugReadMessages ***************************************************
/**
  * Reads debug messages from message stack.
  * The number of messages stored in buf is returned. The total number of new messages is stored in {@link #debugNewMessages}.
  * The number of the latest message is returned by {@link #debugLastMsg()}.
  * @param all If true, all messages from stack are written to buf. If it is false, only the new messages are written to buf.
  * @param buf The buffer to store the messages.
  * @return the size of messages stored in buffer.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if Flash memory access is not possible.
  */
    public int debugReadMessages ( boolean all, byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_DEBUG);
	byte buf2[] = new byte[ debugStackSize()*debugMsgSize() + 4 ];
	vendorRequest2(0x28, "Read debug data", 0, 0, buf2, buf2.length);
	int lm = (buf2[0] & 255) | ((buf2[1] & 255) << 8);
	debugNewMessages = lm - debugLastMsg;
	
	int r = Math.min( Math.min( buf.length/debugMsgSize() , debugStackSize ), lm);
	if ( !all ) r = Math.min(r,debugNewMessages);
	for (int i = 0; i<r; i++) {
	    int k=(lm-r+i) % debugStackSize;
	    for (int j=0; j<debugMsgSize; j++ )
		buf[i*debugMsgSize+j] = buf2[k*debugMsgSize+j+4];
	}
	
	debugLastMsg = lm;
	return r;
    }
    
// ******* xmegaStrError *******************************************************
/** 
  * Converts a given error code into a String.
  * @param errNum The error code.
  * @return an error message.
  */
    public String xmegaStrError ( int errNum ) {
	switch ( errNum ) {
	    case XMEGA_EC_NO_ERROR:
		return "USB error: " + LibusbJava.usb_strerror();
	    case XMEGA_EC_PDI_READ_ERROR:
		return "PDI read error";
	    case XMEGA_EC_NVM_TIMEOUT:
		return "NVM timeout error";
	    case XMEGA_EC_INVALID_DEVICE:
		return "Invalid or unsupported ATxmega";
	    case XMEGA_EC_ADDRESS_ERROR:
		return "Address error (invalid address or wrong page size)";
	    case XMEGA_EC_NVM_BUSY:
		return "NVM busy";
	}
	return "Error " + errNum;
    }

/** 
  * Gets the last ATxmega error from the device.
  * @return an error message.
  */
    public String xmegaStrError ( ) {
	try {
	    return xmegaStrError( xmegaState() );
	}
	catch ( Exception e ) {
	    return "Unknown error (Error receiving error code: "+e.getLocalizedMessage() +")";
	}
    }

// ******* xmegaState **********************************************************
/**
  * Read ATxmega error and status information from the device.
  * @return The last error code.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if ATxmega controllers are not supported by the firmware.
  */
    public int xmegaState () throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buf = new byte[7];
	checkCapability(CAPABILITY_XMEGA);
	vendorRequest2(0x48, "Xmega state", 0, 0, buf, 7);
    	xmegaEC = buf[0] & 255;

    	xmegaFlashPages = ((buf[2] & 255) << 8) | (buf[1] & 255);
    	xmegaEepromPages = ((buf[4] & 255) << 8) | (buf[3] & 255);
    	xmegaFlashPageSize = 1 << (buf[5] & 15);
    	xmegaEepromPageSize = 1 << (buf[6] & 15);
	return xmegaEC;
    }

// ******* xmegaEnabled ********************************************************
/**
  * Returns true if ATxmega controller is available.
  * @return true if ATxmega controller is available.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if ATxmega controllers are not supported by the firmware.
  */
    public boolean xmegaEnabled () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( xmegaFlashPages < 0 || xmegaEepromPages < 0 ) // init variables
	    xmegaState();
	return xmegaFlashPages > 0 && xmegaEepromPages > 0;
    }

// ******* xmegaFlashPages *****************************************************
/**
  * Returns the number of the ATxmega Flash pages.
  * @return The number of the ATxmega Flash pages.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if ATxmega controllers are not supported by the firmware.
  */
    public int xmegaFlashPages () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( xmegaFlashPages < 0 || xmegaEepromPages < 0 ) // init variables
	    xmegaState();
	return xmegaFlashPages;
    }

// ******* xmegaEepromPages ****************************************************
/**
  * Returns the number of the ATxmega EEPROM pages.
  * @return The number of the ATxmega EEPROM pages.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if ATxmega controllers are not supported by the firmware.
  */
    public int xmegaEepromPages () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( xmegaFlashPages < 0 || xmegaEepromPages < 0 ) // init variables
	    xmegaState();
	return xmegaEepromPages;
    }

// ******* xmegaFlashPageSize **************************************************
/**
  * Returns the size of the ATxmega Flash pages.
  * @return The size of the ATxmega Flash pages.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if ATxmega controllers are not supported by the firmware.
  */
    public int xmegaFlashPageSize () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( xmegaFlashPages < 0 || xmegaEepromPages < 0 ) // init variables
	    xmegaState();
	return xmegaFlashPageSize;
    }

// ******* xmegaEEpromPageSize *************************************************
/**
  * Returns the size of the ATXmega EEPROM pages.
  * @return The size of the ATXmega EEPROM pages.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if ATXmega controllers are not supported by the firmware.
  */
    public int xmegaEepromPageSize () throws UsbException, InvalidFirmwareException, CapabilityException {
	if ( xmegaFlashPages < 0 || xmegaEepromPages < 0 ) // init variables
	    xmegaState();
	return xmegaEepromPageSize;
    }

// ******* xmegaReset **********************************************************
/**
  * Resets the ATxmega.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if NVRAM access to ATxmega is not supported by the firmware.
  */
    public void xmegaReset () throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_XMEGA);
	try {
	    vendorCommand( 0x49, "XMEGA Reset" );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "NVM Reset: " + xmegaStrError() ); 
	}
    }


// ******* xmegaNvmRead ********************************************************
/**
  * Reads data from the NVM of ATxmega.
  * @param addr The source address of the NVM (PDI address space).
  * @param buf A buffer for the storage of the data.
  * @param length The amount of bytes to be read.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if NVRAM access to ATxmega is not supported by the firmware.
  */
    public void xmegaNvmRead ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_XMEGA);
	
	try {
	    vendorRequest2( 0x4a, "XMEGA NVM Read", addr, addr>> 16, buf, length );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "NVM Read: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    }


// ******* xmegaFlashRead ******************************************************
/**
  * Reads data from Flash memory of ATxmega.
  * @param addr The source address relative to the Flash memory base.
  * @param buf A buffer for the storage of the data.
  * @param length The amount of bytes to be read.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException If a communication error occurs.
  * @throws CapabilityException If NVRAM access to ATxmega is not supported by the firmware.
  */
    public void xmegaFlashRead ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_XMEGA);
	
	try {
	    vendorRequest2( 0x4b, "XMEGA Flash Read", addr, addr>> 16, buf, length );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "XMEGA Flash Read: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    }



// ******* xmegaEepromRead *****************************************************
/**
  * Reads data from EEPROM memory of ATxmega.
  * @param addr The source address relative to the EEPROM memory base.
  * @param buf A buffer for the storage of the data.
  * @param length The amount of bytes to be read.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException If a communication error occurs.
  * @throws CapabilityException If NVRAM access to ATxmega is not supported by the firmware.
  */
    public void xmegaEepromRead ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_XMEGA);
	
	try {
	    vendorRequest2( 0x4c, "XMEGA EEPROM Read", addr, addr>> 16, buf, length );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "XMEGA EEPROM Read: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    }


// ******* xmegaFuseRead *******************************************************
/**
  * Reads data from Fuse memory of ATxmega.
  * @param addr The source address relative to the Fuse memory base.
  * @param buf A buffer for the storage of the data.
  * @param length The amount of bytes to be read.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException If a communication error occurs.
  * @throws CapabilityException If NVRAM access to ATxmega is not supported by the firmware.
  */
    public void xmegaFuseRead ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_XMEGA);
	
	try {
	    vendorRequest2( 0x4d, "XMEGA Fuse Read", addr, addr>> 16, buf, length );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "XMEGA Fuse Read: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    }

/**
  * Reads data one Fuse of ATxmega.
  * @param addr The index of th Fuse.
  * @return The Fuse read.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException If a communication error occurs.
  * @throws CapabilityException If NVRAM access to ATxmega is not supported by the firmware.
  */
    public int xmegaFuseRead ( int addr ) throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buf = new byte[1];
	checkCapability(CAPABILITY_XMEGA);
	try {
	    vendorRequest2( 0x4d, "XMEGA Fuse Read", addr, 0, buf, 1 );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "XMEGA Fuse Read: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    	return buf[0] & 255;
    }


// ******* xmegaFlashPageWrite *************************************************
/**
  * Writes data to Flash memory of ATxmega.
  * @param addr The source address relative to the Flash memory base.
  * @param buf A buffer that stores the data.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if NVRAM access to ATxmega is not supported by the firmware.
  * @throws IndexOutOfBoundsException If the buffer is smaller than the Flash page size.
*/
    public void xmegaFlashPageWrite ( int addr, byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException {
	checkCapability(CAPABILITY_XMEGA);

	if ( buf.length < xmegaFlashPageSize() ) 
	    throw new IndexOutOfBoundsException( "Buffer smaller than the Flash page size: " + buf.length + " < " + xmegaFlashPageSize);

	try {
	    vendorCommand2( 0x4b, "XMEGA Flash page write", addr, addr>> 16, buf, xmegaFlashPageSize );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "XMEGA Flash page write: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    }

// ******* xmegaEpromPageWrite *************************************************
/**
  * Writes data to EEPROM memory of ATxmega.
  * @param addr The source address relative to the EEPROM memory base.
  * @param buf A buffer that stores the data.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if NVRAM access to ATxmega is not supported by the firmware.
  * @throws IndexOutOfBoundsException If the buffer is smaller than the EEPROM page size.
*/
    public void xmegaEepromPageWrite ( int addr, byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException {
	checkCapability(CAPABILITY_XMEGA);

	if ( buf.length < xmegaEepromPageSize() ) 
	    throw new IndexOutOfBoundsException( "Buffer smaller than the EEPROM page size: " + buf.length + " < " + xmegaEepromPageSize);

	try {
	    vendorCommand2( 0x4c, "XMEGA EEPROM page write", addr, addr>> 16, buf, xmegaEepromPageSize );
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "XMEGA EEPROM page write: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    }

// ******* xmegaFuseWrite ******************************************************
/**
  * Writes one Fuse of the ATxmega.
  * @param addr The index of th Fuse.
  * @param val The value of th Fuse.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if NVRAM access to ATxmega is not supported by the firmware.
*/
    public void xmegaFuseWrite ( int addr, int val ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_XMEGA);

	try {
	    vendorCommand( 0x4d, "XMEGA Fuse write", val, addr);
        }
        catch ( UsbException e ) {
	    throw new UsbException( dev().dev(), "XMEGA Fuse write: " + xmegaStrError() ); 
	}
	try {
    	    Thread.sleep( 3 );
    	}
	catch ( InterruptedException e) {
    	} 
    }

// ******* xmegaIhxWrite *******************************************************
/**
  * Uploads data to NVM
*/
    private long xmegaIhxWrite ( boolean toFlash, IhxFile ihxFile ) throws UsbException, InvalidFirmwareException, CapabilityException, FirmwareUploadException { 
	final int maxTries = 3;  // maximum amount of tries
	int pageSize = toFlash ? xmegaFlashPageSize() : xmegaEepromPageSize();
	checkCapability(CAPABILITY_XMEGA);

	long t0 = new Date().getTime();

	byte buf1[] = new byte[pageSize];
	byte buf2[] = new byte[pageSize];
	
	for (int i = 0; i<65536; i+=pageSize ) {

	    boolean b = false;
	    boolean c = true;
	    for ( int j=0; (j < pageSize ) && ( i+j < 65536 ); j++ ) {
		boolean d = (ihxFile.ihxData[i+j]>=0) && (ihxFile.ihxData[i+j]<=255);	// data vaild ?
		b |= d;
		c &= d;
	    }
	    if ( b ) {	 // page contains data ==> has to be written
//		System.out.print("Page " + i +": " );

		// read page, if firmware image contains undefined bytes
		if ( ! c ) {
//		    System.out.print("R");
		    if ( toFlash ) 
			xmegaFlashRead ( i, buf1, pageSize );
		    else
	                xmegaEepromRead ( i, buf1, pageSize );
		}

		// prepare the page buffer
    		for ( int j=0; (j < pageSize ) && ( i+j < 65536 ); j++ ) {
		    if ( (ihxFile.ihxData[i+j]>=0) && (ihxFile.ihxData[i+j]<=255) )
			buf1[j]= (byte) ihxFile.ihxData[i+j];
		}
		
		for ( int k=1; b ; k++ ) {
		    // write the page
//		    System.out.print("W");
		    if ( toFlash ) 
	    		xmegaFlashPageWrite ( i, buf1 );
	    	    else
	    		xmegaEepromPageWrite ( i, buf1 );

		    // verify it
//		    System.out.print("V");
		    if ( toFlash ) 
	    		xmegaFlashRead ( i, buf2, pageSize );
	    	    else
	    		xmegaEepromRead ( i, buf2, pageSize );
	    	    b=false;
    		    for ( int j=0; (j < pageSize) && (! b ); j++ ) {
    			b |= buf1[j] != buf2[j];
    		    }
    		    if ( b ) {
    			if ( k<maxTries ) {
    			    System.err.println("Warning: xmegaWriteFirmware: Verification of " + ( toFlash ? "Flash" : "EEPROM" ) + " page" + i + " failed (try " + k +")" );
    			}
    			else {
    			    System.err.println("Warning: xmegaWriteFirmware: Verification of " + ( toFlash ? "Flash" : "EEPROM" ) + " page " + i + " failed");
    			}
    		    }
    		    b = false;
    		    
//		    System.out.println();
		}
	    }
	}
	
	return new Date().getTime() - t0;
    }



// ******* xmegaWriteFirmware **************************************************
/**
  * Uploads firmware to the flash memory
  * @param ihxFile The firmware / data image.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if NVRAM access to ATxmega is not supported by the firmware.
  * @throws FirmwareUploadException if the verification fails.
  * @return the upload time in ms.
*/
    public long xmegaWriteFirmware ( IhxFile ihxFile ) throws UsbException, InvalidFirmwareException, CapabilityException, FirmwareUploadException { 
	return xmegaIhxWrite( true, ihxFile);
    }


// ******* xmegaWriteEeprom ****************************************************
/**
  * Uploads data to the EEPROM memory
  * @param ihxFile The firmware / data image.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if NVRAM access to ATxmega is not supported by the firmware.
  * @throws FirmwareUploadException if the verification fails.
  * @return the upload time in ms.
*/
    public long xmegaWriteEeprom ( IhxFile ihxFile ) throws UsbException, InvalidFirmwareException, CapabilityException, FirmwareUploadException { 
	return xmegaIhxWrite( false, ihxFile);
    }


// ******* toString ************************************************************
/** 
  * Returns a lot of useful information about the corresponding device.
  * @return a lot of useful information about the corresponding device.
  */
    public String toString () {
	String str = dev().toString();
	try {
	    str += "\n   " + getFpgaConfigurationStr();
	}
	catch ( Exception e ) {
	}
	return str;
    }

// ******* capabilityInfo ******************************************************
/**
  * Creates a String with capability information.
  * @param pf A separator between the single capabilities, e.g. ", "
  * @return a string of the supported capabilities.
  */
    public String capabilityInfo ( String pf ) {
	String str = "";
	for ( int i=0; i<6; i++ ) 
	    for (int j=0; j<8; j++ ) 
		if ( dev().interfaceCapabilities(i,j) ) {
		    if ( ! str.equals("") ) 
			str+=pf;
		    if (i*8+j < capabilityStrings.length) 
			str+=capabilityStrings[i*8+j];
		    else
			str+=i+"."+j;
		}
	return str;
    }
// ******* configureFpgaHS *****************************************************
//  returns configuration time in ms
/**
  * Upload a Bitstream to the FPGA using high speed mode.
  * @param fwFileName The file name of the Bitstream. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @param force If set to true existing configurations will be overwritten. (By default an {@link AlreadyConfiguredException} is thrown).
  * @param bs 0: disable bit swapping, 1: enable bit swapping, all other values: automatic detection of bit order.
  * @throws BitstreamReadException if an error occurred while attempting to read the Bitstream.
  * @throws BitstreamUploadException if an error occurred while attempting to upload the Bitstream.
  * @throws AlreadyConfiguredException if the FPGA is already configured.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public long configureFpgaHS ( String fwFileName, boolean force, int bs ) throws BitstreamReadException, UsbException, BitstreamUploadException, AlreadyConfiguredException, InvalidFirmwareException, CapabilityException {
	final int transactionBytes = 16384;
	long t0 = 0;
	byte[] settings = new byte[2];
	boolean releaseIF;

	checkCapability(CAPABILITY_HS_FPGA);
	vendorRequest2(0x33, "getHSFpgaSettings", settings, 2);

	if ( !force && getFpgaConfiguration() )
	    throw new AlreadyConfiguredException(); 

	releaseIF = ! getInterfaceClaimed(settings[1] & 255);
//	System.out.println("EP "+ settings[0] + "    IF "+settings[1]+ "   claim " + releaseIF);
	
// read the Bitstream file	
        byte[][] buffer = new byte[16*1024*1024/transactionBytes][];
	int size = 0;
	try {
	    InputStream inputStream = JInputStream.getInputStream( fwFileName );
	    int j = transactionBytes;
	    for ( int i=0; i<buffer.length && j==transactionBytes; i++ ) {
		buffer[i] = new byte[transactionBytes]; 
		int k;
		j = 0;	
		do {
		    k = inputStream.read( buffer[i], j, transactionBytes-j );
		    if ( k < 0 ) 
		        k = 0;
		    j += k;
		}
		while ( j<transactionBytes && k>0 );
		size += j;
	    }
	    
	    try {
		inputStream.close();
	    }
	    catch ( Exception e ) {
		System.err.println( "Warning: Error closing file " + fwFileName + ": " + e.getLocalizedMessage() );
	    }
	}
	catch (IOException e) {
	    throw new BitstreamReadException(e.getLocalizedMessage());
	}

	if ( size < 64 ) 
	    throw new BitstreamReadException("Invalid file size: " + size );
	
// detect bitstream bit order and swap bits if necessary 
	if ( bs<0 || bs>1 )
	    bs = detectBitstreamBitOrder ( buffer[0] );
	if ( bs == 1 )
	    swapBits(buffer,size);

// remove NOP's from the end
/*	System.out.println(size);
	while ( size-2>=0 && buffer[(size-2) / transactionBytes][(size-2) % transactionBytes] == 4 && buffer[(size-1) / transactionBytes][(size-1) % transactionBytes]==0 )
	    size-=2;
	System.out.println(size);
*/	

// claim interface if required
	if ( releaseIF ) claimInterface( settings[1] & 255 );

//	System.out.println(size & 127);
	
// upload the Bitstream file	
	for ( int tries=3; tries>0; tries-- ) {
	    
    	    vendorCommand(0x34, "initHSFPGAConfiguration" );

	    try {
		t0 = -new Date().getTime();
		    
	    	for ( int i=0; i<buffer.length && i*transactionBytes < size; i++ ) {
		    int j = size-i*transactionBytes;
		    if (j>transactionBytes) 
			j = transactionBytes;
		
		    if ( j>0 ) {
			int l = LibusbJava.usb_bulk_write(handle(), settings[0] & 255, buffer[i], j, 1000);
			if ( l < 0 )
			    throw new UsbException("Error sending Bitstream: " + l + ": " + LibusbJava.usb_strerror());
			else if ( l != j )
			    throw new UsbException("Error sending Bitstream: Sent " + l +" of " + j + " bytes");
		    }
		}

		try {
    		    Thread.sleep( (size % transactionBytes) / 1000 + 10 );
    		}
		catch ( InterruptedException e) {
    		} 

		vendorCommand(0x35, "finishHSFPGAConfiguration" );
		t0 += new Date().getTime();

 		getFpgaState();
//		System.err.println("fpgaConfigred=" + fpgaConfigured + "   fpgaBytes="+fpgaBytes + " ("+size+")   fpgaInitB="+fpgaInitB + "  time=" + t0);
		if ( ! fpgaConfigured ) {
		    throw new BitstreamUploadException( "FPGA configuration failed: DONE pin does not go high, possible USB transfer errors (INIT_B_HIST=" + fpgaInitB + (fpgaBytes==0 ? "" : "; " + (size - fpgaBytes) + " bytes got lost") + ")" );
		}

		if ( enableExtraFpgaConfigurationChecks ) {
	    	    if ( fpgaBytes!=0 && fpgaBytes!=size )
			System.err.println("Warning: Possible FPGA configuration data loss: " + (size - fpgaBytes) + " bytes got lost");
		    if ( fpgaInitB!=222 )
			System.err.println("Warning: Possible Bitstream CRC error: INIT_B_HIST=" + fpgaInitB );
		}
			
		tries = 0;
	    } 
	    catch ( BitstreamUploadException e ) {
		if (tries == 1)
		    throw e;
		else if ( tries<3 || enableExtraFpgaConfigurationChecks )  
		    System.err.println("Warning: " + e.getLocalizedMessage() +": Retrying it ...");
	    }
	}
	    
	if ( releaseIF ) releaseInterface( settings[1] & 255 );

    	try {
    	    Thread.sleep( 25 );
    	}
	catch ( InterruptedException e) {
        } 
	
	return t0;
    } 

// ******* configureFpga *****************************************************
//  returns configuration time in ms
/**
  * Upload a Bitstream to the FPGA using high speed mode (if available) or low speed mode.
  * @param fwFileName The file name of the Bitstream. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @param force If set to true existing configurations will be overwritten. (By default an {@link AlreadyConfiguredException} is thrown).
  * @param bs 0: disable bit swapping, 1: enable bit swapping, all other values: automatic detection of bit order.
  * @throws BitstreamReadException if an error occurred while attempting to read the Bitstream.
  * @throws BitstreamUploadException if an error occurred while attempting to upload the Bitstream.
  * @throws AlreadyConfiguredException if the FPGA is already configured.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public long configureFpga ( String fwFileName, boolean force, int bs ) throws BitstreamReadException, UsbException, BitstreamUploadException, AlreadyConfiguredException, InvalidFirmwareException, CapabilityException {
	try {
	    return configureFpgaHS( fwFileName, force, bs );
	}
	catch ( CapabilityException e ) {
	    return configureFpgaLS( fwFileName, force, bs );
	}
	catch ( UsbException e ) {
	    System.err.println("Warning: High speed FPGA configuration failed, trying low speed mode:" + e.getLocalizedMessage() +": Trying low speed mode");
	    return configureFpgaLS( fwFileName, force, bs );
	}
	catch ( BitstreamUploadException e ) {
	    System.err.println("Warning: High speed FPGA configuration failed, trying low speed mode:" + e.getLocalizedMessage() +": Trying low speed mode");
	    return configureFpgaLS( fwFileName, force, bs );
	}
    }

/**
  * Upload a Bitstream to the FPGA using high speed mode (if available) or low speed mode.
  * @param fwFileName The file name of the Bitstream. The file can be a regular file or a system resource (e.g. a file from the current jar archive).
  * @param force If set to true existing configurations will be overwritten. (By default an {@link AlreadyConfiguredException} is thrown).
  * @throws BitstreamReadException if an error occurred while attempting to read the Bitstream.
  * @throws BitstreamUploadException if an error occurred while attempting to upload the Bitstream.
  * @throws AlreadyConfiguredException if the FPGA is already configured.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if FPGA configuration is not supported by the firmware.
  */
    public long configureFpga ( String fwFileName, boolean force ) throws BitstreamReadException, UsbException, BitstreamUploadException, AlreadyConfiguredException, InvalidFirmwareException, CapabilityException {
	return configureFpga(fwFileName, force, -1);
    }

// ******* macEepromWrite ******************************************************
/**
  * Writes data to the MAC EEPROM.
  * @param addr The destination address of the MAC EEPROM.
  * @param buf The data.
  * @param length The amount of bytes to be sent.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if MAC EEPROM access is not supported by the firmware.
  */
    public void macEepromWrite ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_MAC_EEPROM);
	byte[] buf2 = new byte[8];
	int ptr = 0;
	while (length>0 ) {
	    int i = Math.min( 8 - (addr & 7), length );
	    for ( int j=0; j<i; j++ ) 
		buf2[j] = buf[ptr+j];
	    vendorCommand2( 0x3C, "MAC EEPROM Write", addr, 0, buf2, i );
    	    try {
    		Thread.sleep( 10 );
	    }
	    catch ( InterruptedException e) {
    	    } 
    	    addr+=i;
    	    length-=i;
    	    ptr += i;
    	}
    }

// ******* macEepromRead *******************************************************
/**
  * Reads data from the MAC EEPROM.
  * @param addr The source address of the MAC EEPROM.
  * @param buf A buffer for the storage of the data.
  * @param length The amount of bytes to be read.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if MAC EEPROM access is not supported by the firmware.
  */
    public void macEepromRead ( int addr, byte[] buf, int length ) throws UsbException, InvalidFirmwareException, CapabilityException {
	checkCapability(CAPABILITY_MAC_EEPROM);
	vendorRequest2( 0x3B, "MAC EEPROM Read", addr, 0, buf, length );
    	try {
    	    Thread.sleep( 10 );
    	}
	catch ( InterruptedException e) {
        } 
    }

// ******* macEepromState ******************************************************
// returns true if MAC EEPROM is ready
/**
  * Reads the current MAC EEPROM status.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if MAC EEPROM access is not supported by the firmware.
  * @return true if MAC EEPROM is ready.
  */
    public boolean macEepromState ( ) throws UsbException, InvalidFirmwareException, CapabilityException {
	byte[] buf = new byte[1];
	checkCapability(CAPABILITY_MAC_EEPROM);
	vendorRequest2(0x3D, "MAC EEPROM State", 0, 0, buf, 1);
	return buf[0] == 0;
    }

// ******* macRead *************************************************************
/**
  * Reads MAC address from MAC EEPROM.
  * @param buf A buffer with a minimum size of 6 bytes.
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws CapabilityException if MAC EEPROM access is not supported by the firmware.
  * @throws IndexOutOfBoundsException If the buffer is smaller than 6 bytes.
  */
    public void macRead ( byte[] buf ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException {
	if ( buf.length < 6 ) 
    	    throw new IndexOutOfBoundsException( "macRead: Buffer smaller than 6 Bytes" );
    	macEepromRead(250, buf, 6);
    }

// ******* numberOfFpgas *******************************************************
/**
  * Returns the number of FPGA's
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @return number of FPGA's
  */
    public int numberOfFpgas ( ) throws UsbException, InvalidFirmwareException {
	if ( numberOfFpgas < 0 ) {
	    try {
		byte[] buffer = new byte[3];
		checkCapability(CAPABILITY_MULTI_FPGA);
		vendorRequest2(0x50, "getMultiFpgaInfo", buffer, 3);
		numberOfFpgas = (buffer[0] & 255)+1;
		selectedFpga = buffer[1] & 255;
		parallelConfigSupport = buffer[2]==1;
	    }
	    catch ( CapabilityException e ) {
		numberOfFpgas = 1;
		selectedFpga = 0;
		parallelConfigSupport = false;
	    }
	}
	return numberOfFpgas;
    }

// ******* selectFpga **********************************************************
/**
  * Select a FPGA
  * @param num FPGA to select. Valid values are 0 to {@link #numberOfFpgas()}-1
  * @throws InvalidFirmwareException if interface 1 is not supported.
  * @throws UsbException if a communication error occurs.
  * @throws IndexOutOfBoundsException If FPGA number is not in range.
  */
    public void selectFpga ( int num ) throws UsbException, InvalidFirmwareException, IndexOutOfBoundsException {
	numberOfFpgas();
	if ( num<0 || num>=numberOfFpgas )
    	    throw new IndexOutOfBoundsException( "selectFPGA: Invalid FPGA number" );
	
	if ( numberOfFpgas != 1 ) {
	    try {
		checkCapability(CAPABILITY_MULTI_FPGA);
		vendorCommand( 0x51, "selectFPGA", num, 0);
	    }
	    catch ( CapabilityException e ) {
		// should'nt occur
	    }
	}
	selectedFpga = num;
    }

// ******* TempSensorRead ******************************************************
/**
  * Read temperature sensor data.
  * @param idx Temperature sensor index
  * @return Temperature in °C
  * @throws InvalidFirmwareException If interface 1 or temperature sensor protocol is not supported.
  * @throws UsbException If a communication error occurs.
  * @throws CapabilityException If NVRAM access to ATxmega is not supported by the firmware.
  * @throws IndexOutOfBoundsException If idx is not in range.
  */
    public double tempSensorRead ( int idx ) throws UsbException, InvalidFirmwareException, CapabilityException, IndexOutOfBoundsException {
	int[] xIdx = { 3, 4, 1, 2 };
    
	checkCapability(CAPABILITY_TEMP_SENSOR);
	
	int len = 0;
	
	if ( tempSensorUpdateInterval < 40 ) 
	    tempSensorUpdateInterval = 40;
	
	if ( new Date().getTime() > lastTempSensorReadTime+tempSensorUpdateInterval ) {
	    len = vendorRequest( 0x58, "Temperature Sensor Read", 0, 0, tempSensorBuf, tempSensorBuf.length );
	    lastTempSensorReadTime = new Date().getTime();

	    if ( len != 5 || tempSensorBuf[0] != 1 )
		throw new InvalidFirmwareException("tempSensorRead: Invalid temperature sensor protocol");
	}
	
	if ( idx<0 || idx>3 ) 
	    throw new IndexOutOfBoundsException( "tempSensorRead: Invalid temperature sensor index" );
	    
	return ((tempSensorBuf[xIdx[idx]] & 255)-77.2727)/1.5454;	
    }

}    

