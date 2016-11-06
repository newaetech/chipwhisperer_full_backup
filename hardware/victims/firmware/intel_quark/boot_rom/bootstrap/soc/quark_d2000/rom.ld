/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

ENTRY(_rom_start)

MEMORY
{
	otp_lock (r)	: ORIGIN = 0x00000000, LENGTH = 0x4
	/* ROM manufacturing data */
	rom_data (r)	: ORIGIN = 0x00000004, LENGTH = 0x150 - 0x4
	/* ROM: 8K - ROM manufacturing data - ROM version - OTP lock */
	/* The reset vector is at the start of the ROM memory region */
	rom (rw)	: ORIGIN = 0x00000150, LENGTH = 8K - 0x150 - 0x4
	/* ROM version */
	rom_ver (r)	: ORIGIN = 0x00001FFC, LENGTH = 0x4
	/* Interrupt descriptor table (IDT) (52 gates) */
	esram_idt (rw)	: ORIGIN = 0x00280000, LENGTH = 0x1A0
	/* ESRAM: RAM size - stack - IDT */
	esram (rw)	: ORIGIN = 0x002801A0, LENGTH = 8K - 1K - 0x1A0
	/* Stack */
	stack (rw)	: ORIGIN = 0x00281C00, LENGTH = 1K
}

/* Stack definition */
__stack_start = ORIGIN(stack) + LENGTH(stack);

/* IDT definition */
__idt_start = ORIGIN(esram_idt);
__idt_end = __idt_start + LENGTH(esram_idt);

SECTIONS
{
	. = ORIGIN(rom_data);
	/* Reserve area for manufacturing data */
	_rom_data_base = .;
	my : { } >rom_data

	. = ORIGIN(rom);
	.text :
	{
		*(.text.entry)
		*(.text)
		*(.text.last)
		*(.text.*)
	} >rom

	.rodata :
	{
		*(.rdata*)
		*(.rodata*)  /* .rodata* sections (constants, strings, etc.) */
	} >rom

	.data :
	{
		*(.data*)
	} >esram  AT>rom

	.bss :
	{
		*(.bss*)
		*(COMMON)
	} >esram  AT>esram

	. = ORIGIN(rom_ver);
	.rom_version :
	{
		KEEP(*(.rom_version*))
	} >rom_ver

	/* Symbols for C runtime init code. */
	__data_lma = LOADADDR(.data);
	__data_vma = ADDR(.data);
	__data_size = SIZEOF(.data);
	__bss_start = ADDR(.bss);
	__bss_end = __bss_start + SIZEOF(.bss);

	.comment 0 : { *(.comment) }
}
