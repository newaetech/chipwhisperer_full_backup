#
# Copyright (c) 2016, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#
# This file contains the early system initialization code.
# It handles the transition from 16-bit real mode to 32-bit protected mode.
#

.extern rom_startup
.extern __stack_start
.extern __x86_restore_info

#
# CR0 cache control bit definition
#
.equ CR0_CACHE_DISABLE, 0x040000000
.equ CR0_NO_WRITE, 0x020000000

#
# Context save/restore definitions
#
.equ GPS0_REGISTER, 0xb0800100
.equ RESTORE_BIT, 1

.text
#----------------------------------------------------------------------------
#
# Procedure:	_rom_start
#
# Input:	None
#
# Output:	None
#
# Destroys:	Assume all registers
#
# Description:
#	Transition to non-paged flat-model protected mode from a
#	fixed GDT that provides exactly two descriptors.
#	After enabling protected mode, a far jump is executed to
#	transfer to the C runtime environment.
#	This code needs to be located at the beginning of the ROM image.
#
# Return:	None
#
#----------------------------------------------------------------------------
.global _rom_start
_rom_start:
	#
	# Store the the Built-in-Self-Test (BIST) value in EBP
	#
	.byte 0xbe,0x00,0xf0	#movw  $0xF000, %si
	.byte 0x8e,0xde		#movw  %si, %ds
	.byte 0xbe,0xf0,0xff	#movw  $0xFFF0, %si
	.byte 0x66,0x8b,0xe8	#movl  %eax, %ebp

	#
	# Load the GDT table in gdt_descriptor
	#
	.byte 0x66,0xbe		#movl  $gdt_descriptor, %esi
	.long gdt_descriptor

	.byte 0x66,0x2e,0x0f,0x01,0x14	#lgdt  %cs:(%si)

	#
	# Transition to 16 bit protected mode
	#
	.byte 0x0f,0x20,0xc0	  #movl  %cr0, %eax	  # Get control register 0
	.byte 0x66,0x83,0xc8,0x03 #orl   $0x0000003, %eax # Set PE bit (bit #0) & MP bit (bit #1)
	.byte 0x0f,0x22,0xc0	  #movl  %eax, %cr0	  # Activate protected mode

	#
	# Now we're in 16 bit protected mode
	# Set up the selectors for 32 bit protected mode entry
	#
	.byte 0xb8		#movw  SYS_DATA_SEL, %ax
	.word SYS_DATA_SEL
	.byte 0x8e,0xd8		#movw  %ax, %ds
	.byte 0x8e,0xc0		#movw  %ax, %es
	.byte 0x8e,0xe0		#movw  %ax, %fs
	.byte 0x8e,0xe8		#movw  %ax, %gs
	.byte 0x8e,0xd0		#movw  %ax, %ss

	#
	# Transition to Flat 32 bit protected mode
	# The jump to a far pointer causes the transition to 32 bit mode
	#
	.byte 0x66,0xbe		#movl  protected_mode_entry_linear_address, %esi
	.long protected_mode_entry_linear_address
	.byte 0x66,0x2e,0xff,0x2c	#jmp   %cs:(%esi)

#----------------------------------------------------------------------------
#
# Procedure:	protected_mode_entry
#
# Input:	Executing in 32 Bit Protected (flat) mode
#	cs: 0-4GB
#	ds: 0-4GB
#	es: 0-4GB
#	fs: 0-4GB
#	gs: 0-4GB
#	ss: 0-4GB
#
# Output:	This function never returns
#
# Destroys:
#	ecx
#	edi
#	esi
#	esp
#
# Description:
#	Essential early platform initialization.
#	Initializes stack, configures cache, and calls C entry point.
#
#----------------------------------------------------------------------------
protected_mode_entry:
	#
	# Enable cache. Cache at this point is clean, as any previous
	# contents have been invalidated by the reset vector.
	#
	movl %cr0, %eax
	andl $~(CR0_CACHE_DISABLE + CR0_NO_WRITE), %eax
	movl %eax, %cr0

#if (ENABLE_RESTORE_CONTEXT)
	#
	# Check if we are returning from a 'sleep' state and jump to the
	# restore trap if that's the case. The restore trap will restore the
	# execution context we had before entering in 'sleep' state.
	#
	btr $RESTORE_BIT, GPS0_REGISTER
	jnc regular_boot

	movl $__x86_restore_info, %eax
	jmp *(%eax)

regular_boot:
#endif
	#
	# Set up stack pointer. The stack start address is defined in
	# the linker script. ESP = top of the stack (the stack grows
	# downwards).
	#
	movl $__stack_start, %esp

	#
	# Jump to C entry point
	#
	call rom_startup

	#
	# Forever loop at end of last routine so should not return here.
	#
	jmp .

#----------------------------------------------------------------------------
#
# Procedure:	gdt_table
#
# Input:	N/A
#
# Output:	N/A
#
# Destroys:	N/A
#
# Description:
#	Below are a set of constants used in setting up the
#	Global Descriptor Table (GDT) before the switch to 32 bit mode
#
#----------------------------------------------------------------------------
Function:
	.long gdt_table
	.align 16

gdt_table:
	#
	# GDT entry 0: Not used
	#
	.equ NULL_SEL, 0
	.long 0
	.long 0

	#
	# GDT entry 1: Linear code segment descriptor
	#
	.equ LINEAR_CODE_SEL, . - gdt_table
	.word 0xFFFF	# limit 0FFFFh
	.word 0		# base 0
	.byte 0
	.byte 0x9B	# present, ring 0, user, code, expand-up, writable
	.byte 0xCF	# 4 KB page-granular, 32-bit
	.byte 0

	#
	# GDT entry 2: System data segment descriptor
	#
	.equ SYS_DATA_SEL, . - gdt_table
	.word 0xFFFF	# limit 0FFFFh
	.word 0		# base 0
	.byte 0
	.byte 0x93	# present, ring 0, user, data, expand-up, writable
	.byte 0xCF	# 4KB page-granular, 32-bit
	.byte 0

	.equ GDT_SIZE, . - gdt_table  # Size, in bytes

#
# GDT Descriptor
#
gdt_descriptor:
	.word GDT_SIZE - 1
	.long gdt_table

#
# Protected mode entry linear address
#
protected_mode_entry_linear_address:
	.long protected_mode_entry
	.word LINEAR_CODE_SEL
