#
# Copyright (c) 2016, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
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
#

	.extern __stack_start
	.extern _start

	/* Entry point see sensor.ld linker script */
	.section .text.entry
	.align 4
	.global _reset

#
# Context save/restore definitions
#
.equ GPS0_REGISTER, 0xb0800100
.equ RESTORE_SS_BIT, 2

/**
 * Make the first four bytes of the Sensor Subsystem flash partition point to
 * the Sensor Subsystem _reset entry point.
 * The Lakemont bootloader fetches this pointer and places it in the Sensor
 * Subsystem reset vector before taking the Sensor Subsystem core out of reset.
 */
.word 0x40000004

.func _reset
_reset:
	/* Invalidate I-Cache */
	sr 0x12345678,[0x10]

#if (ENABLE_RESTORE_CONTEXT)
	/* Check is this wakeup after sleep event. */
	ld r0,[GPS0_REGISTER]
	bbit0 r0,RESTORE_SS_BIT,_stack_handler
	bclr_s r0,r0,RESTORE_SS_BIT
	st r0,[GPS0_REGISTER]
	ld r0,[arc_restore_addr]
	j [r0]

_stack_handler:
#endif

	/* Set stack on top of the cos_stack array. */
	mov sp, __stack_start

	/* Start C code */
	bl _start
	nop
	/* Should not get here. */
_loop:
	b _loop
.endfunc

	.section .data.entry
	.align 4
	.global arc_restore_addr
arc_restore_addr: .word 0

.end
