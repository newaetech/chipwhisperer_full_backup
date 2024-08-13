/*
 * Copyright (c) 2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

ENTRY(_reset)

MEMORY
{
	flash0 (r)	: ORIGIN = 0x40000000, LENGTH = 188K
	soc_data (r)	: ORIGIN = 0x4002F000, LENGTH = 4K
	dccm (rw)	: ORIGIN = 0x80000000, LENGTH = 8K
	/* Place Vector table at the bottom of SRAM, 68 gate wide */
	vect_table(rw)	: ORIGIN = 0xA8000000, LENGTH = 0x110
	esram (rw)	: ORIGIN = 0xA8000110, LENGTH = 40K - 1K - 0x110
	stack (rw)	: ORIGIN = 0xA8009C00, LENGTH = 1K
	/* Place IDT at the bottom of LMT SRAM, 68 gate wide */
	lmt_esram_idt (rw): ORIGIN = 0xA800A000, LENGTH = 0x220
	/* Shared RAM */
	esram_restore_info (rw) : ORIGIN = 0xA8013FFC, LENGTH = 0x4
}

/* IDT definition */
__idt_start = ORIGIN(lmt_esram_idt);
__idt_end = __idt_start + LENGTH(lmt_esram_idt);
__ivt_vect_table = ORIGIN(vect_table);

/* Shared RAM definition */
__x86_restore_info = ORIGIN(esram_restore_info);

/* Stack definition */
__stack_start = ORIGIN(stack) + LENGTH(stack);

SECTIONS
{
	.text :
	{
		*(.text.entry)
		*(.text)
		*(.text.last)
		*(.text.*)
	} >flash0

	.rodata :
	{
		*(.rdata*)
		*(.rodata*)
	} >flash0

	.data :
	{
		*(.data*)	/* initialized variables */
	} >esram  AT>flash0

	.bss :
	{
		*(.bss*)	/* zero-initialized variables */
		*(COMMON)
	} >esram  AT>esram

	/* Symbols for C runtime init code. */
	__data_lma = LOADADDR(.data);
	__data_vma = ADDR(.data);
	__data_size = SIZEOF(.data);
	__bss_start = ADDR(.bss);
	__bss_end = __bss_start + SIZEOF(.bss);

	/* Heap */
	__heap = .;
	__heap_end = ORIGIN(stack);

	.comment 0 : { *(.comment) }
}
