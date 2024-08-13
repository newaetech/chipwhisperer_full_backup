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

ENTRY(_start)

MEMORY
{
	otp_lock (rw)   : ORIGIN = 0xFFFFE000, LENGTH = 0x4
	/* ROM manufacturing data */
	rom_data (rw)   : ORIGIN = 0xFFFFE004, LENGTH = 0x1FC
	/* ROM: 8K - ROM version - reset vector - manufacturing data - OTP lock */
	rom (rw)	: ORIGIN = 0xFFFFE200, LENGTH = 8K - 0x4 - 16 - 0x1FC - 0x4
	/* ROM version */
	rom_ver (r)	: ORIGIN = 0xFFFFFFEC, LENGTH = 0x4
	reset_vector (r): ORIGIN = 0xFFFFFFF0, LENGTH = 16
	/* Sensor Subsystem vector table (68 entries) */
	sensor_vectors (rw) : ORIGIN = 0xA8000000, LENGTH = 68
	/* Interrupt descriptor table (IDT) (68 gates) */
	esram_idt (rw)	: ORIGIN = 0xA800A000, LENGTH = 0x220
	/*
	 * ESRAM: half RAM size - stack - IDT
	 *
	 * TODO: try with full RAM size (80K), since ARC is off while the
	 * 2nd-stage bootloader runs.
	 */
	esram (rw)	: ORIGIN = 0xA800A220, LENGTH = 40K - 1K - 0x220
	/* Stack */
	stack (rw)	: ORIGIN = 0xA8013C00, LENGTH = 1K - 0x4
	/* Shared RAM */
	esram_restore_info (rw) : ORIGIN = 0xA8013FFC, LENGTH = 0x4
}

/* Sensor Subsystem reset vector definition */
__sensor_reset_vector = ORIGIN(sensor_vectors);

/* Stack definition */
__stack_start = ORIGIN(stack) + LENGTH(stack);

/* IDT definition */
__idt_start = ORIGIN(esram_idt);
__idt_end = __idt_start + LENGTH(esram_idt);

/* Shared RAM definition */
__x86_restore_info = ORIGIN(esram_restore_info);

SECTIONS
{
	. = ORIGIN(reset_vector);
	.reset_vector :
	{
		*(.text.entry)
		*reset_vector.s.o (.text)
	} > reset_vector

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
		*(.rodata*) /* .rodata* sections (constants, strings, etc.) */
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
