/*	wdtcon.h -- Lock/unlock TriCore's ENDINIT bit and modify WDTCON0.

    Copyright (C) 2000 - 2013 HighTec EDV-Systeme GmbH.

	This file is part of GCC.

	GCC is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3, or (at your option)
	any later version.

	GCC is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	Under Section 7 of GPL version 3, you are granted additional
	permissions described in the GCC Runtime Library Exception, version
	3.1, as published by the Free Software Foundation.

	You should have received a copy of the GNU General Public License and
	a copy of the GCC Runtime Library Exception along with this program;
	see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
	<http://www.gnu.org/licenses/>.  */


#ifndef __wdtcon_h
#define __wdtcon_h


#ifdef __cplusplus
extern "C" {
#endif

/* To modify ENDINIT-protected registers, use the following sequence:

	unlock_wdtcon (); // reset ENDINIT bit
	write new value(s) to desired register(s)
	lock_wdtcon (); // set ENDINIT bit

   Another way would be:

	WDT_Passwd (); // prepare WDTCON0 for write access
	WDT_Modify (0, 0x1); // reset ENDINIT bit
	write new value(s) to desired register(s)
	WDT_Passwd ();
	WDT_Modify (1, 0x1); // set ENDINIT bit

   The basic difference between the two approaches is that WDT_Modify()
   allows to manipulate every bit in WDTCON0 at the same time, while
   (un)lock_wdtcon() only (re)sets the ENDINIT bit.

	On multi core targets (like TC27x) the functions WDT_Passwd and
	WDT_Modify are not implemented.
*/

extern void lock_wdtcon (void);
extern void unlock_wdtcon (void);

extern void WDT_Passwd (void);
extern void WDT_Modify (unsigned long modify, unsigned long mask);

/* functions for handling multi core targets' safety watchdog */
extern void lock_safety_wdtcon (void);
extern void unlock_safety_wdtcon (void);

#ifdef __cplusplus
}
#endif

#endif /* __wdtcon_h */
