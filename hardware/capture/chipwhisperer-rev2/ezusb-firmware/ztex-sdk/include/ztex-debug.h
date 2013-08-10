/*!
   ZTEX Firmware Kit for EZ-USB FX2 Microcontrollers
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
    debug helper
*/    
#ifndef[ZTEX_DEBUG_H]
#define[ZTEX_DEBUG_H]

#ifndef[DEBUG_STACK_SIZE]
#error[Macro `DEBUG_STACK_SIZE' is not defined.]
#endif

#ifndef[DEBUG_MSG_SIZE]
#error[Macro `DEBUG_MSG_SIZE' is not defined.]
#endif

#define[@CAPABILITY_DEBUG;]

__xdata WORD debug_counter;				  // message number 
__xdata BYTE debug_stack_size;				  // stack size in messages
__xdata BYTE debug_msg_size;				  // message size in bytes
__xdata BYTE debug_stack[DEBUG_STACK_SIZE*DEBUG_MSG_SIZE];  // message stack
__xdata BYTE* __xdata debug_stack_ptr;                        // pointer to the last message added

__xdata BYTE debug_msg_buf[DEBUG_MSG_SIZE];		  // user buffer: user should write data to this buffer
__xdata BYTE* __xdata debug_read_addr;

/* *********************************************************************
   ***** debug_add_msg *************************************************
   ********************************************************************* */
// adds the current message (stored in debug_msg_buf) to the message stack
void debug_add_msg () {
    BYTE i;
    i = debug_counter % DEBUG_STACK_SIZE;
    debug_stack_ptr = &debug_stack[i*DEBUG_MSG_SIZE];
    MEM_COPY1(debug_msg_buf,*debug_stack_ptr,DEBUG_MSG_SIZE);
    debug_counter += 1;
}

/* *********************************************************************
   ***** debug_add_msg *************************************************
   ********************************************************************* */
// init the variables
void debug_init () {
    debug_counter = 0;				  // message number 
    debug_stack_size = DEBUG_STACK_SIZE;
    debug_msg_size = DEBUG_MSG_SIZE;
    debug_stack_ptr = debug_stack;
}

/* *********************************************************************
   ***** EP0 vendor request 0x28 ***************************************
   ********************************************************************* */
// read the debug data
BYTE debug_read_ep0 () { 
    BYTE b;
    b = ep0_payload_transfer;
    if ( b != 0) {
	MEM_COPY1(*debug_read_addr,EP0BUF,b);
    }
    debug_read_addr += b;
    return b;
}

ADD_EP0_VENDOR_REQUEST((0x28,, 				// read the debug data
    debug_read_addr = (__xdata BYTE*)&debug_counter;
    EP0BCH = 0;
    EP0BCL = debug_read_ep0(); 
,,
    EP0BCH = 0;
    EP0BCL = debug_read_ep0(); 
));; 

#endif  /*ZTEX_DEBUG_H*/
