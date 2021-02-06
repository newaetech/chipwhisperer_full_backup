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
   EZ-USB registers
*/

#ifndef[EZREGS_H]
#define[EZREGS_H]

#include[ztex-utils.h]

/* This syncronization delay is valid if <CPU Clock> <= 5/3*<Interface Clock>, 
   i.e. if the Interface Clock is equal or greater than 28.8 MHz
   
   The formula for the synchonization delay is:
   
                        /    <CPU Clock>        \
   <syncdelay> >= 1.5 * | ----------------- + 1 |
                        \ <Interface Clock>     /
   
   Overwrite this macro if this formula is not satisfied 
*/   

#ifndef[SYNCDELAY;]
#define[SYNCDELAY;][__asm 
	nop 
	nop 
	nop 
	nop 
    __endasm;
]
#endif
    
// GPIF Waveform Memories
__xdata __at 0xE400 volatile BYTE GPIF_WAVE_DATA[128];	// GPIF Waveform Descriptor 0, 1, 2 3, data
__xdata __at 0xE400 volatile BYTE GPIF_WAVE0_DATA[32];	// GPIF Waveform Descriptor 0 data
__xdata __at 0xE420 volatile BYTE GPIF_WAVE1_DATA[32];	// GPIF Waveform Descriptor 1 data
__xdata __at 0xE440 volatile BYTE GPIF_WAVE2_DATA[32];	// GPIF Waveform Descriptor 2 data
__xdata __at 0xE460 volatile BYTE GPIF_WAVE3_DATA[32];	// GPIF Waveform Descriptor 3 data

// General Configuration
__xdata __at 0xE50D volatile BYTE GPCR2;			// General Purpose Configuration Register 2
__xdata __at 0xE600 volatile BYTE CPUCS;			// Control & Status
__xdata __at 0xE601 volatile BYTE IFCONFIG;			// Interface Configuration
__xdata __at 0xE602 volatile BYTE PINFLAGSAB;		// FIFO FLAGA and FLAGB Assignments
__xdata __at 0xE603 volatile BYTE PINFLAGSCD;		// FIFO FLAGC and FLAGD Assignments
__xdata __at 0xE604 volatile BYTE FIFORESET;		// Restore FIFOS to default state
__xdata __at 0xE605 volatile BYTE BREAKPT;			// Breakpoint
__xdata __at 0xE606 volatile BYTE BPADDRH; 			// Breakpoint Address H
__xdata __at 0xE607 volatile BYTE BPADDRL;			// Breakpoint Address L
__xdata __at 0xE608 volatile BYTE UART230;			// 230 Kbaud clock for T0,T1,T2
__xdata __at 0xE609 volatile BYTE FIFOPINPOLAR;		// FIFO polarities
__xdata __at 0xE60A volatile BYTE REVID;			// Chip Revision
__xdata __at 0xE60B volatile BYTE REVCTL;			// Chip Revision Control

// UDMA
__xdata __at 0xE60C volatile BYTE GPIFHOLDAMOUNT;		// MSTB Hold Time (for UDMA)

// Endpoint Configuration
__xdata __at 0xE610 volatile BYTE EP1OUTCFG;		// Endpoint 1-OUT Configuration
__xdata __at 0xE611 volatile BYTE EP1INCFG;			// Endpoint 1-IN Configuration
__xdata __at 0xE612 volatile BYTE EP2CFG;			// Endpoint 2 Configuration
__xdata __at 0xE613 volatile BYTE EP4CFG;			// Endpoint 4 Configuration
__xdata __at 0xE614 volatile BYTE EP6CFG;			// Endpoint 6 Configuration
__xdata __at 0xE615 volatile BYTE EP8CFG;			// Endpoint 8 Configuration
__xdata __at 0xE618 volatile BYTE EP2FIFOCFG;		// Endpoint 2 FIFO configuration
__xdata __at 0xE619 volatile BYTE EP4FIFOCFG;		// Endpoint 4 FIFO configuration
__xdata __at 0xE61A volatile BYTE EP6FIFOCFG;		// Endpoint 6 FIFO configuration
__xdata __at 0xE61B volatile BYTE EP8FIFOCFG;		// Endpoint 8 FIFO configuration
__xdata __at 0xE620 volatile BYTE EP2AUTOINLENH;		// Endpoint 2 Packet Length H (IN only)
__xdata __at 0xE621 volatile BYTE EP2AUTOINLENL;		// Endpoint 2 Packet Length L (IN only)
__xdata __at 0xE622 volatile BYTE EP4AUTOINLENH;		// Endpoint 4 Packet Length H (IN only)
__xdata __at 0xE623 volatile BYTE EP4AUTOINLENL;		// Endpoint 4 Packet Length L (IN only)
__xdata __at 0xE624 volatile BYTE EP6AUTOINLENH;		// Endpoint 6 Packet Length H (IN only)
__xdata __at 0xE625 volatile BYTE EP6AUTOINLENL;		// Endpoint 6 Packet Length L (IN only)
__xdata __at 0xE626 volatile BYTE EP8AUTOINLENH;		// Endpoint 8 Packet Length H (IN only)
__xdata __at 0xE627 volatile BYTE EP8AUTOINLENL;		// Endpoint 8 Packet Length L (IN only)
__xdata __at 0xE628 volatile BYTE ECCCFG;			// ECC Configuration
__xdata __at 0xE629 volatile BYTE ECCRESET;			// ECC Reset
__xdata __at 0xE62A volatile BYTE ECC1B0;			// ECC1 Byte 0 Address
__xdata __at 0xE62B volatile BYTE ECC1B1;			// ECC1 Byte 1 Address
__xdata __at 0xE62C volatile BYTE ECC1B2;			// ECC1 Byte 2 Address
__xdata __at 0xE62D volatile BYTE ECC2B0;			// ECC2 Byte 0 Address
__xdata __at 0xE62E volatile BYTE ECC2B1;			// ECC2 Byte 1 Address
__xdata __at 0xE62F volatile BYTE ECC2B2;			// ECC2 Byte 2 Address
__xdata __at 0xE630 volatile BYTE EP2FIFOPFH;		// EP2 Programmable Flag trigger H
__xdata __at 0xE631 volatile BYTE EP2FIFOPFL;		// EP2 Programmable Flag trigger L
__xdata __at 0xE632 volatile BYTE EP4FIFOPFH;		// EP4 Programmable Flag trigger H
__xdata __at 0xE633 volatile BYTE EP4FIFOPFL;		// EP4 Programmable Flag trigger L
__xdata __at 0xE634 volatile BYTE EP6FIFOPFH;		// EP6 Programmable Flag trigger H
__xdata __at 0xE635 volatile BYTE EP6FIFOPFL;		// EP6 Programmable Flag trigger L
__xdata __at 0xE636 volatile BYTE EP8FIFOPFH;		// EP8 Programmable Flag trigger H
__xdata __at 0xE637 volatile BYTE EP8FIFOPFL;		// EP8 Programmable Flag trigger L
__xdata __at 0xE640 volatile BYTE EP2ISOINPKTS;		// EP2 (if ISO) IN Packets per frame (1-3)
__xdata __at 0xE641 volatile BYTE EP4ISOINPKTS;		// EP4 (if ISO) IN Packets per frame (1-3)
__xdata __at 0xE642 volatile BYTE EP6ISOINPKTS;		// EP6 (if ISO) IN Packets per frame (1-3)
__xdata __at 0xE643 volatile BYTE EP8ISOINPKTS;		// EP8 (if ISO) IN Packets per frame (1-3)
__xdata __at 0xE648 volatile BYTE INPKTEND;			// Force IN Packet End
__xdata __at 0xE649 volatile BYTE OUTPKTEND;		// Force OUT Packet End

// Interrupts
__xdata __at 0xE650 volatile BYTE EP2FIFOIE;		// Endpoint 2 Flag Interrupt Enable
__xdata __at 0xE651 volatile BYTE EP2FIFOIRQ;		// Endpoint 2 Flag Interrupt Request
__xdata __at 0xE652 volatile BYTE EP4FIFOIE;		// Endpoint 4 Flag Interrupt Enable
__xdata __at 0xE653 volatile BYTE EP4FIFOIRQ;		// Endpoint 4 Flag Interrupt Request
__xdata __at 0xE654 volatile BYTE EP6FIFOIE;		// Endpoint 6 Flag Interrupt Enable
__xdata __at 0xE655 volatile BYTE EP6FIFOIRQ;		// Endpoint 6 Flag Interrupt Request
__xdata __at 0xE656 volatile BYTE EP8FIFOIE;		// Endpoint 8 Flag Interrupt Enable
__xdata __at 0xE657 volatile BYTE EP8FIFOIRQ;		// Endpoint 8 Flag Interrupt Request
__xdata __at 0xE658 volatile BYTE IBNIE;			// IN-BULK-NAK Interrupt Enable
__xdata __at 0xE659 volatile BYTE IBNIRQ;			// IN-BULK-NAK interrupt Request
__xdata __at 0xE65A volatile BYTE NAKIE;			// Endpoint Ping NAK interrupt Enable
__xdata __at 0xE65B volatile BYTE NAKIRQ;			// Endpoint Ping NAK interrupt Request
__xdata __at 0xE65C volatile BYTE USBIE;			// USB Int Enables
__xdata __at 0xE65D volatile BYTE USBIRQ;			// USB Interrupt Requests
__xdata __at 0xE65E volatile BYTE EPIE;			// Endpoint Interrupt Enables
__xdata __at 0xE65F volatile BYTE EPIRQ;			// Endpoint Interrupt Requests
__xdata __at 0xE660 volatile BYTE GPIFIE;			// GPIF Interrupt Enable
__xdata __at 0xE661 volatile BYTE GPIFIRQ;			// GPIF Interrupt Request
__xdata __at 0xE662 volatile BYTE USBERRIE;			// USB Error Interrupt Enables
__xdata __at 0xE663 volatile BYTE USBERRIRQ;		// USB Error Interrupt Requests
__xdata __at 0xE664 volatile BYTE ERRCNTLIM;		// USB Error counter and limit
__xdata __at 0xE665 volatile BYTE CLRERRCNT;		// Clear Error Counter EC[3..0]
__xdata __at 0xE666 volatile BYTE INT2IVEC;			// Interupt 2 (USB) Autovector
__xdata __at 0xE667 volatile BYTE INT4IVEC;			// Interupt 4 (FIFOS & GPIF) Autovector
__xdata __at 0xE668 volatile BYTE INTSETUP;			// Interrupt 2&4 Setup

// Input/Output
__xdata __at 0xE670 volatile BYTE PORTACFG;			// I/O PORTA Alternate Configuration
__xdata __at 0xE671 volatile BYTE PORTCCFG;			// I/O PORTC Alternate Configuration
__xdata __at 0xE672 volatile BYTE PORTECFG;			// I/O PORTE Alternate Configuration
__xdata __at 0xE678 volatile BYTE I2CS;			// Control & Status
__xdata __at 0xE679 volatile BYTE I2DAT;			// Data
__xdata __at 0xE67A volatile BYTE I2CTL;			// I2C Control
__xdata __at 0xE67B volatile BYTE XAUTODAT1;		// Autoptr1 MOVX access
__xdata __at 0xE67B volatile BYTE EXTAUTODAT1;		// Autoptr1 MOVX access
__xdata __at 0xE67C volatile BYTE XAUTODAT2;		// Autoptr2 MOVX access
__xdata __at 0xE67C volatile BYTE EXTAUTODAT2;		// Autoptr2 MOVX access

// UDMA CRC
__xdata __at 0xE67D volatile BYTE UDMACRCH;			// UDMA CRC MSB
__xdata __at 0xE67E volatile BYTE UDMACRCL;			// UDMA CRC LSB
__xdata __at 0xE67F volatile BYTE UDMACRCQUALIFIER;		// UDMA CRC Qualifier

// USB Control
__xdata __at 0xE680 volatile BYTE USBCS;			// USB Control & Status
__xdata __at 0xE681 volatile BYTE SUSPEND;			// Put chip into suspend
__xdata __at 0xE682 volatile BYTE WAKEUPCS;			// Wakeup source and polarity
__xdata __at 0xE683 volatile BYTE TOGCTL;			// Toggle Control
__xdata __at 0xE684 volatile BYTE USBFRAMEH;		// USB Frame count H
__xdata __at 0xE685 volatile BYTE USBFRAMEL;		// USB Frame count L
__xdata __at 0xE686 volatile BYTE MICROFRAME;		// Microframe count, 0-7
__xdata __at 0xE687 volatile BYTE FNADDR;			// USB Function address

// Endpoints
__xdata __at 0xE68A volatile BYTE EP0BCH;			// Endpoint 0 Byte Count H
__xdata __at 0xE68B volatile BYTE EP0BCL;			// Endpoint 0 Byte Count L
__xdata __at 0xE68D volatile BYTE EP1OUTBC;			// Endpoint 1 OUT Byte Count
__xdata __at 0xE68F volatile BYTE EP1INBC;			// Endpoint 1 IN Byte Count
__xdata __at 0xE690 volatile BYTE EP2BCH;			// Endpoint 2 Byte Count H
__xdata __at 0xE691 volatile BYTE EP2BCL;			// Endpoint 2 Byte Count L
__xdata __at 0xE694 volatile BYTE EP4BCH;			// Endpoint 4 Byte Count H
__xdata __at 0xE695 volatile BYTE EP4BCL;			// Endpoint 4 Byte Count L
__xdata __at 0xE698 volatile BYTE EP6BCH;			// Endpoint 6 Byte Count H
__xdata __at 0xE699 volatile BYTE EP6BCL;			// Endpoint 6 Byte Count L
__xdata __at 0xE69C volatile BYTE EP8BCH;			// Endpoint 8 Byte Count H
__xdata __at 0xE69D volatile BYTE EP8BCL;			// Endpoint 8 Byte Count L
__xdata __at 0xE6A0 volatile BYTE EP0CS;			// Endpoint Control and Status
__xdata __at 0xE6A1 volatile BYTE EP1OUTCS;			// Endpoint 1 OUT Control and Status
__xdata __at 0xE6A2 volatile BYTE EP1INCS;			// Endpoint 1 IN Control and Status
__xdata __at 0xE6A3 volatile BYTE EPXCS[4];			// Endpoint 2-8 Control and Status
__xdata __at 0xE6A3 volatile BYTE EP2CS;			// Endpoint 2 Control and Status
__xdata __at 0xE6A4 volatile BYTE EP4CS;			// Endpoint 4 Control and Status
__xdata __at 0xE6A5 volatile BYTE EP6CS;			// Endpoint 6 Control and Status
__xdata __at 0xE6A6 volatile BYTE EP8CS;			// Endpoint 8 Control and Status
__xdata __at 0xE6A7 volatile BYTE EP2FIFOFLGS;		// Endpoint 2 Flags
__xdata __at 0xE6A8 volatile BYTE EP4FIFOFLGS;		// Endpoint 4 Flags
__xdata __at 0xE6A9 volatile BYTE EP6FIFOFLGS;		// Endpoint 6 Flags
__xdata __at 0xE6AA volatile BYTE EP8FIFOFLGS;		// Endpoint 8 Flags
__xdata __at 0xE6AB volatile BYTE EP2FIFOBCH;		// EP2 FIFO total byte count H
__xdata __at 0xE6AC volatile BYTE EP2FIFOBCL;		// EP2 FIFO total byte count L
__xdata __at 0xE6AD volatile BYTE EP4FIFOBCH;		// EP4 FIFO total byte count H
__xdata __at 0xE6AE volatile BYTE EP4FIFOBCL;		// EP4 FIFO total byte count L
__xdata __at 0xE6AF volatile BYTE EP6FIFOBCH;		// EP6 FIFO total byte count H
__xdata __at 0xE6B0 volatile BYTE EP6FIFOBCL;		// EP6 FIFO total byte count L
__xdata __at 0xE6B1 volatile BYTE EP8FIFOBCH;		// EP8 FIFO total byte count H
__xdata __at 0xE6B2 volatile BYTE EP8FIFOBCL;		// EP8 FIFO total byte count L
__xdata __at 0xE6B3 volatile BYTE SUDPTRH;			// Setup Data Pointer high address byte
__xdata __at 0xE6B4 volatile BYTE SUDPTRL;			// Setup Data Pointer low address byte
__xdata __at 0xE6B5 volatile BYTE SUDPTRCTL;		// Setup Data Pointer Auto Mode
__xdata __at 0xE6B8 volatile BYTE SETUPDAT[8];		// 8 bytes of SETUP data
__xdata __at 0xE6B8 volatile BYTE bmRequestType;		// Request Type, Direction, and Recipient
__xdata __at 0xE6B9 volatile BYTE bRequest;			// The actual request
__xdata __at 0xE6BA volatile BYTE wValueL;
__xdata __at 0xE6BB volatile BYTE wValueH;
__xdata __at 0xE6BC volatile BYTE wIndexL;
__xdata __at 0xE6BD volatile BYTE wIndexH;
__xdata __at 0xE6BE volatile BYTE wLengthL;			// Number of bytes to transfer if there is a data phase
__xdata __at 0xE6BF volatile BYTE wLengthH;

// GPIF
__xdata __at 0xE6C0 volatile BYTE GPIFWFSELECT;		// Waveform Selector
__xdata __at 0xE6C1 volatile BYTE GPIFIDLECS;		// GPIF Done, GPIF IDLE drive mode
__xdata __at 0xE6C2 volatile BYTE GPIFIDLECTL;		// Inactive Bus, CTL states
__xdata __at 0xE6C3 volatile BYTE GPIFCTLCFG;		// CTL OUT pin drive
__xdata __at 0xE6C4 volatile BYTE GPIFADRH;			// GPIF Address H
__xdata __at 0xE6C5 volatile BYTE GPIFADRL;			// GPIF Address L

// FLOWSTATE 
__xdata __at 0xE6C6 volatile BYTE FLOWSTATE;		// Flowstate Enable and Selector
__xdata __at 0xE6C7 volatile BYTE FLOWLOGIC;		// Flowstate Logic
__xdata __at 0xE6C8 volatile BYTE FLOWEQ0CTL;		// CTL-Pin States in Flowstate (when Logic = 0)
__xdata __at 0xE6C9 volatile BYTE FLOWEQ1CTL;		// CTL-Pin States in Flowstate (when Logic = 1)
__xdata __at 0xE6CA volatile BYTE FLOWHOLDOFF;		// Holdoff Configuration
__xdata __at 0xE6CB volatile BYTE FLOWSTB;			// Flowstate Strobe Configuration
__xdata __at 0xE6CC volatile BYTE FLOWSTBEDGE;		// Flowstate Rising/Falling Edge Configuration
__xdata __at 0xE6CD volatile BYTE FLOWSTBHPERIOD;		// Master-Strobe Half-Period
__xdata __at 0xE6CE volatile BYTE GPIFTCB3;			// GPIF Transaction Count Byte 3
__xdata __at 0xE6CF volatile BYTE GPIFTCB2;			// GPIF Transaction Count Byte 2
__xdata __at 0xE6D0 volatile BYTE GPIFTCB1;			// GPIF Transaction Count Byte 1
__xdata __at 0xE6D1 volatile BYTE GPIFTCB0;			// GPIF Transaction Count Byte 0
__xdata __at 0xE6D2 volatile BYTE EP2GPIFFLGSEL;		// EP2 GPIF Flag select
__xdata __at 0xE6D3 volatile BYTE EP2GPIFPFSTOP;		// Stop GPIF EP2 transaction on prog. flag
__xdata __at 0xE6D4 volatile BYTE EP2GPIFTRIG;		// EP2 FIFO Trigger
__xdata __at 0xE6DA volatile BYTE EP4GPIFFLGSEL;		// EP4 GPIF Flag select
__xdata __at 0xE6DB volatile BYTE EP4GPIFPFSTOP;		// Stop GPIF EP4 transaction on prog. flag
__xdata __at 0xE6DC volatile BYTE EP4GPIFTRIG;		// EP4 FIFO Trigger
__xdata __at 0xE6E2 volatile BYTE EP6GPIFFLGSEL;		// EP6 GPIF Flag select
__xdata __at 0xE6E3 volatile BYTE EP6GPIFPFSTOP;		// Stop GPIF EP6 transaction on prog. flag
__xdata __at 0xE6E4 volatile BYTE EP6GPIFTRIG;		// EP6 FIFO Trigger
__xdata __at 0xE6EA volatile BYTE EP8GPIFFLGSEL;		// EP8 GPIF Flag select
__xdata __at 0xE6EB volatile BYTE EP8GPIFPFSTOP;		// Stop GPIF EP8 transaction on prog. flag
__xdata __at 0xE6EC volatile BYTE EP8GPIFTRIG;		// EP8 FIFO Trigger
__xdata __at 0xE6F0 volatile BYTE XGPIFSGLDATH;		// GPIF Data H (16-bit mode only)
__xdata __at 0xE6F1 volatile BYTE XGPIFSGLDATLX;		// Read/Write GPIF Data L & trigger transac
__xdata __at 0xE6F2 volatile BYTE XGPIFSGLDATLNOX;		// Read GPIF Data L, no transac trigger
__xdata __at 0xE6F3 volatile BYTE GPIFREADYCFG;		// Internal RDY,Sync/Async, RDY5CFG
__xdata __at 0xE6F4 volatile BYTE GPIFREADYSTAT;		// RDY pin states
__xdata __at 0xE6F5 volatile BYTE GPIFABORT;		// Abort GPIF cycles

// Endpoint Buffers
__xdata __at 0xE740 volatile BYTE EP0BUF[64];		// EP0 IN-OUT buffer
__xdata __at 0xE780 volatile BYTE EP1OUTBUF[64];		// EP1-OUT buffer
__xdata __at 0xE7C0 volatile BYTE EP1INBUF[64];		// EP1-IN buffer
__xdata __at 0xF000 volatile BYTE EP2FIFOBUF[1024];		// 512/1024-byte EP2 buffer (IN or OUT)
__xdata __at 0xF400 volatile BYTE EP4FIFOBUF[1024];		// 512 byte EP4 buffer (IN or OUT)
__xdata __at 0xF800 volatile BYTE EP6FIFOBUF[1024];		// 512/1024-byte EP6 buffer (IN or OUT)
__xdata __at 0xFC00 volatile BYTE EP8FIFOBUF[1024];		// 512 byte EP8 buffer (IN or OUT)


// Special Function Registers (__sfrs)
__sfr __at 0x80 IOA;					// Port A
__sbit __at 0x80+0 IOA0;					// Port A bit 0
__sbit __at 0x80+1 IOA1;					// Port A bit 1
__sbit __at 0x80+2 IOA2;					// Port A bit 2
__sbit __at 0x80+3 IOA3;					// Port A bit 3
__sbit __at 0x80+4 IOA4;					// Port A bit 4
__sbit __at 0x80+5 IOA5;					// Port A bit 5
__sbit __at 0x80+6 IOA6;					// Port A bit 6
__sbit __at 0x80+7 IOA7;					// Port A bit 7
__sfr __at 0x81 SP;						// Stack Pointer
__sfr __at 0x82 DPL0;					// Data Pointer 0 L
__sfr __at 0x83 DPH0;					// Data Pointer 0 H
__sfr __at 0x84 DPL1;					// Data Pointer 1 L
__sfr __at 0x85 DPH1;					// Data Pointer 0 H
__sfr __at 0x86 DPS;					// Data Pointer 0/1 select
__sfr __at 0x87 PCON;					// Power Control
__sfr __at 0x88 TCON;					// Timer/Counter Control
__sbit __at 0x88+0 IT0;					// Interrupt 0 Type select
__sbit __at 0x88+1 IE0;					// Interrupt 0 Edge detect
__sbit __at 0x88+2 IT1;					// Interrupt 1 Type select
__sbit __at 0x88+3 IE1;					// Interrupt 1 Edge detect
__sbit __at 0x88+4 TR0;					// Timer 0 Run Control
__sbit __at 0x88+5 TF0;					// Timer 0 Overflow Flag
__sbit __at 0x88+6 TR1;					// Timer 1 Run Control
__sbit __at 0x88+7 TF1;					// Timer 1 Overflow Flag
__sfr __at 0x89 TMOD;					// Timer/Counter Mode Control
__sfr __at 0x8A TL0;					// Timer 0 reload L
__sfr __at 0x8B TL1;					// Timer 1 reload L
__sfr __at 0x8C TH0;					// Timer 0 reload H
__sfr __at 0x8D TH1;					// Timer 1 reload H
__sfr __at 0x8E CKCON;					// Clock Control
__sfr __at 0x90 IOB; 					// Port B
__sbit __at 0x90+0 IOB0;					// Port B bit 0
__sbit __at 0x90+1 IOB1;					// Port B bit 1
__sbit __at 0x90+2 IOB2;					// Port B bit 2
__sbit __at 0x90+3 IOB3;					// Port B bit 3
__sbit __at 0x90+4 IOB4;					// Port B bit 4
__sbit __at 0x90+5 IOB5;					// Port B bit 5
__sbit __at 0x90+6 IOB6;					// Port B bit 6
__sbit __at 0x90+7 IOB7;					// Port B bit 7
__sfr __at 0x91 EXIF;					// External Interrupt Flag(s)
__sfr __at 0x92 MPAGE;					// Upper Addr Byte of MOVX using @R0 / @R1
__sfr __at (0x92) _XPAGE;
__sfr __at 0x98 SCON0;					// Serial Port 0 Control
__sbit __at 0x98+0 RI_0;					// Recive Interrupt Flag
__sbit __at 0x98+1 TI_0;					// Transmit Interrupt Flag
__sbit __at 0x98+2 RB8_0;					// State of the 9th bit / Stop Bit received
__sbit __at 0x98+3 TB8_0;					// State of the 9th bit transmitted
__sbit __at 0x98+4 REN_0;					// Receive enable
__sbit __at 0x98+5 SM2_0;					// Multiprocessor communication enable
__sbit __at 0x98+6 SM1_0;					// Serial Port 0 mode bit 1
__sbit __at 0x98+7 SM0_0;					// Serial Port 0 mode bit 0
__sfr __at 0x99 SBUF0;					// Serial Port 0 Data Buffer
__sfr __at 0x9A AUTOPTRH1;					// Autopointer 1 Address H
__sfr __at 0x9B AUTOPTRL1;					// Autopointer 1 Address L
__sfr __at 0x9D AUTOPTRH2;					// Autopointer 2 Address H
__sfr __at 0x9E AUTOPTRL2; 					// Autopointer 2 Address L
__sfr __at 0xA0 IOC; 					// Port C
__sbit __at 0xA0+0 IOC0;					// Port C bit 0
__sbit __at 0xA0+1 IOC1;					// Port C bit 1
__sbit __at 0xA0+2 IOC2;					// Port C bit 2
__sbit __at 0xA0+3 IOC3;					// Port C bit 3
__sbit __at 0xA0+4 IOC4;					// Port C bit 4
__sbit __at 0xA0+5 IOC5;					// Port C bit 5
__sbit __at 0xA0+6 IOC6;					// Port C bit 6
__sbit __at 0xA0+7 IOC7;					// Port C bit 7
__sfr __at 0xA1 INT2CLR;					// Interrupt 2 clear
__sfr __at 0xA2 INT4CLR;					// Interrupt 4clear
__sfr __at 0xA8 IE;						// Interrupt Enable
__sbit __at 0xA8+0 EX0;					// Enable external interrupt 0
__sbit __at 0xA8+1 ET0;					// Enable Timer 0 interrupt
__sbit __at 0xA8+2 EX1;					// Enable external interrupt 1
__sbit __at 0xA8+3 ET1;					// Enable Timer 1 interrupt
__sbit __at 0xA8+4 ES0;					// Enable Serial Port 0 interrupt
__sbit __at 0xA8+5 ET2;					// Enable Timer 2 interrupt
__sbit __at 0xA8+6 ES1;					// Enable Serial Port 1 interrupt
__sbit __at 0xA8+7 EA;					// Global interrupt enable
__sfr __at 0xAA EP2468STAT;					// Endpoint 2,4,6,8 status flags
__sfr __at 0xAB EP24FIFOFLGS;				// Endpoint 2,4 slave FIFO flags
__sfr __at 0xAC EP68FIFOFLGS;				// Endpoint 6,8 slave FIFO flags
__sfr __at 0xAF AUTOPTRSETUP;				// Autopointer 1&2 set-up
__sfr __at 0xB0 IOD; 					// Port D
__sbit __at 0xB0+0 IOD0;					// Port D bit 0
__sbit __at 0xB0+1 IOD1;					// Port D bit 1
__sbit __at 0xB0+2 IOD2;					// Port D bit 2
__sbit __at 0xB0+3 IOD3;					// Port D bit 3
__sbit __at 0xB0+4 IOD4;					// Port D bit 4
__sbit __at 0xB0+5 IOD5;					// Port D bit 5
__sbit __at 0xB0+6 IOD6;					// Port D bit 6
__sbit __at 0xB0+7 IOD7;					// Port D bit 7
__sfr __at 0xB1 IOE;					// Port E
__sfr __at 0xB2 OEA;					// Port A Output Enable
__sfr __at 0xB3 OEB;					// Port B Output Enable
__sfr __at 0xB4 OEC;					// Port C Output Enable
__sfr __at 0xB5 OED;					// Port D Output Enable
__sfr __at 0xB6 OEE;					// Port E Output Enable
__sfr __at 0xB8 IP;						// Interrupt priority
__sbit __at 0xB8+0 PX0;					// External interrupt 0 priority control
__sbit __at 0xB8+1 PT0;					// Timer 0 interrupt priority control
__sbit __at 0xB8+2 PX1;					// External interrupt 1 priority control
__sbit __at 0xB8+3 PT1;					// Timer 1 interrupt priority control
__sbit __at 0xB8+4 PS0;					// Serial Port 0 interrupt priority control
__sbit __at 0xB8+5 PT2;					// Timer 2 interrupt priority control
__sbit __at 0xB8+6 PS1;					// Serial Port 1 interrupt priority control
__sfr __at 0xBA EP01STAT;					// Endpoint 0&1 Status
__sfr __at 0xBB GPIFTRIG;					// Endpoint 2,4,6,8 GPIF slafe FIFO Trigger
__sfr __at 0xBD GPIFSGLDATH;				// GPIF Data H (16-bit mode only)
__sfr __at 0xBE GPIFSGLDATLX;				// GPIF Data L w/ Trigger
__sfr __at 0xBF GPIFSGLDATLNOX;				// GPIF Data L w/ No Trigger
__sfr __at 0xC0 SCON1;					// Serial Port 1 Control
__sbit __at 0xC0+0 RI_1;					// Recive Interrupt Flag
__sbit __at 0xC0+1 TI_1;					// Transmit Interrupt Flag
__sbit __at 0xC0+2 RB8_1;					// State of the 9th bit / Stop Bit received
__sbit __at 0xC0+3 TB8_1;					// State of the 9th bit transmitted
__sbit __at 0xC0+4 REN_1;					// Receive enable
__sbit __at 0xC0+5 SM2_1;					// Multiprocessor communication enable
__sbit __at 0xC0+6 SM1_1;					// Serial Port 1 mode bit 1
__sbit __at 0xC0+7 SM0_1;					// Serial Port 1 mode bit 0
__sfr __at 0xC1 SBUF1;					// Serial Port 1 Data Buffer
__sfr __at 0xC8 T2CON;					// Timer/Counter 2 Control
__sbit __at 0xC8+0 CPRL2;					// Capture/reload flag
__sbit __at 0xC8+1 CT2;					// Counter/Timer select
__sbit __at 0xC8+2 TR2;					// Timer 2 run control flag
__sbit __at 0xC8+3 EXEN2;					// Timer 2 external enable
__sbit __at 0xC8+4 TCLK;					// Transmit clock flag
__sbit __at 0xC8+5 RCLK;					// Receive clock flag
__sbit __at 0xC8+6 EXF2;					// Timer 2 external flag
__sbit __at 0xC8+7 TF2;					// Timer 2 overflow flag
__sfr __at 0xCA RCAP2L;					// Capture for Timer 2, auto-reload, up-counter L
__sfr __at 0xCB RCAP2H;					// Capture for Timer 2, auto-reload, up-counter H
__sfr __at 0xCC TL2;					// Timer 2 reload L
__sfr __at 0xCD TH2;					// Timer 2 reload H
__sfr __at 0xD0 PSW;					// Program Status Word
__sbit __at 0xD0+0 PF;					// Parity flag
__sbit __at 0xD0+1 F1;					// User flag 1
__sbit __at 0xD0+2 OV;					// Overflow flag
__sbit __at 0xD0+3 RS0;					// Register bank select bit 0
__sbit __at 0xD0+4 RS1;					// Register bank select bit 1
__sbit __at 0xD0+5 F0;					// User flag 0
__sbit __at 0xD0+6 AC;					// Auxiliary carry flag
__sbit __at 0xD0+7 CY;					// Carry flag
__sfr __at 0xD8 EICON;					// External Interrupt Control
__sbit __at 0xD8+3 INT6;					// External interrupt 6
__sbit __at 0xD8+4 RESI;					// Wakeup interrupt flag
__sbit __at 0xD8+5 ERESI;					// Enable Resume interrupt
__sbit __at 0xD8+7 SMOD1;					// Serial Port 1 baud rate doubler enable
__sfr __at 0xE0 ACC;					// Accumulator
__sbit __at 0xE0+0 ACC0;					// Accumulator bit 0
__sbit __at 0xE0+1 ACC1;					// Accumulator bit 1
__sbit __at 0xE0+2 ACC2;					// Accumulator bit 2
__sbit __at 0xE0+3 ACC3;					// Accumulator bit 3
__sbit __at 0xE0+4 ACC4;					// Accumulator bit 4
__sbit __at 0xE0+5 ACC5;					// Accumulator bit 5
__sbit __at 0xE0+6 ACC6;					// Accumulator bit 6
__sbit __at 0xE0+7 ACC7;					// Accumulator bit 7
__sfr __at 0xE8 EIE; 					// External Interrupt Enable(s)
__sbit __at 0xE8+0 EUSB;					// Enable USB interrupt (USBINT)
__sbit __at 0xE8+1 EI2C;					// Enable I2C bus interrupt (I2CINT)
__sbit __at 0xE8+2 EIEX4;					// Enable external interrupt 4
__sbit __at 0xE8+3 EIEX5;					// Enable external interrupt 5
__sbit __at 0xE8+4 EIEX6;					// Enable external interrupt 6
__sfr __at 0xF0 BREG;					// B Register
__sbit __at 0xF0+0 BREG0;					// B Register bit 0
__sbit __at 0xF0+1 BREG1;					// B Register bit 1
__sbit __at 0xF0+2 BREG2;					// B Register bit 2
__sbit __at 0xF0+3 BREG3;					// B Register bit 3
__sbit __at 0xF0+4 BREG4;					// B Register bit 4
__sbit __at 0xF0+5 BREG5;					// B Register bit 5
__sbit __at 0xF0+6 BREG6;					// B Register bit 6
__sbit __at 0xF0+7 BREG7;					// B Register bit 7
__sfr __at 0xF8 EIP;					// External Interrupt Priority Control
__sbit __at 0xF8+0 PUSB;					// USBINT priority control
__sbit __at 0xF8+1 PI2C;					// I2CINT priority control
__sbit __at 0xF8+2 EIPX4;					// External interrupt 4 priority control
__sbit __at 0xF8+3 EIPX5;					// External interrupt 5 priority control
__sbit __at 0xF8+4 EIPX6;					// External interrupt 6 priority control

#endif   /* EZREGS_H */
