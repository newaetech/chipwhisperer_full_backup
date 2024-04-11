/**
 * \file
 *
 * \brief Access to huge data memory with 8-bit AVR
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include "hugemem.h"
#include "compiler.h"
#include "conf_board.h"

#if defined(CONFIG_HAVE_HUGEMEM) || defined(__DOXYGEN__)
# if defined(__GNUC__) || defined(__DOXYGEN__)
uint_fast16_t hugemem_read16(const hugemem_ptr_t from)
{
	uint16_t value;

	asm(
		"movw r30, %A1 \n\t"
		"out %2, %C1 \n\t"
		"ld %A0, Z+ \n\t"
		"ld %B0, Z \n\t"
		"out %2, __zero_reg__ \n\t"
		: "=r"(value)
		: "r"(from), "i"(&RAMPZ)
		: "r30", "r31"
	);

	return value;
}

uint_fast32_t hugemem_read32(const hugemem_ptr_t from)
{
	uint32_t value;

	asm(
		"movw r30, %A1 \n\t"
		"out %2, %C1 \n\t"
		"ld %A0, Z+ \n\t"
		"ld %B0, Z+ \n\t"
		"ld %C0, Z+ \n\t"
		"ld %D0, Z \n\t"
		"out %2, __zero_reg__ \n\t"
		: "=r"(value)
		: "r"(from), "i"(&RAMPZ)
		: "r30", "r31"
	);

	return value;
}

void hugemem_read_block(void *to, const hugemem_ptr_t from, size_t size)
{
	// Ensure that the address range to copy to is within 64 kB boundary.
	Assert(((uint32_t)to + size) <= 0x10000);

	if (size > 0) {
		asm volatile(
			"movw r30, %A2 \n\t"
			"out %3, %C2 \n\t"
			"get_%=: \n\t"
			"ld __tmp_reg__, Z+ \n\t"
			"st X+, __tmp_reg__ \n\t"
			"sbiw %A1, 1 \n\t"
			"brne get_%= \n\t"
			"out %3, __zero_reg__ \n\t"
			: "+x"(to), "+w"(size)
			: "r"(from), "i"(&RAMPZ)
			: "r30", "r31"
		);
	}
}

void hugemem_write16(hugemem_ptr_t to, uint_fast16_t val)
{
	asm(
		"movw r30, %A0 \n\t"
		"out %2, %C0 \n\t"
		"st Z+, %A1 \n\t"
		"st Z, %B1 \n\t"
		"out %2, __zero_reg__ \n\t"
		:
		: "r"(to), "r"(val), "i"(&RAMPZ)
		: "r30", "r31"
	);
}

void hugemem_write32(hugemem_ptr_t to, uint_fast32_t val)
{
	asm(
		"movw r30, %A0 \n\t"
		"out %2, %C0 \n\t"
		"st Z+, %A1 \n\t"
		"st Z+, %B1 \n\t"
		"st Z+, %C1 \n\t"
		"st Z, %D1 \n\t"
		"out %2, __zero_reg__ \n\t"
		:
		: "r"(to), "r"(val), "i"(&RAMPZ)
		: "r30", "r31"
	);
}

void hugemem_write_block(hugemem_ptr_t to, const void *from, size_t size)
{
	// Ensure that the address range to copy from is within 64 kB boundary.
	Assert(((uint32_t)from + size) <= 0x10000);

	if (size > 0) {
		asm volatile(
			"movw r30, %A2 \n\t"
			"out %3, %C2 \n\t"
			"put_%=: \n\t"
			"ld __tmp_reg__, X+ \n\t"
			"st Z+, __tmp_reg__ \n\t"
			"sbiw %A1, 1 \n\t"
			"brne put_%= \n\t"
			"out %3, __zero_reg__ \n\t"
			: "+x"(from), "+w"(size)
			: "r"(to), "i"(&RAMPZ)
			: "r30", "r31"
		);
	}
}
# endif /* __GNUC__ */

# ifdef __ICCAVR__
void hugemem_read_block(void *to, const hugemem_ptr_t from, size_t size)
{
	uint8_t *to_ptr;
	uint8_t __huge *from_ptr;

	// Ensure that the address range to copy to is within 64 kB boundary.
	Assert(((uint32_t)to + size) <= 0x10000);

	to_ptr = (uint8_t *)to;
	from_ptr = (uint8_t __huge *)from;

	for (; size > 0; size--) {
		*to_ptr++ = *from_ptr++;
	}
}

void hugemem_write_block(hugemem_ptr_t to, const void *from, size_t size)
{
	uint8_t __huge *to_ptr;
	uint8_t *from_ptr;

	// Ensure that the address range to copy from is within 64 kB boundary.
	Assert(((uint32_t)from + size) <= 0x10000);

	to_ptr = (uint8_t __huge *)to;
	from_ptr = (uint8_t *)from;

	for (; size > 0; size--) {
		*to_ptr++ = *from_ptr++;
	}
}
# endif /* __ICCAVR__ */
#endif /* CONFIG_HAVE_HUGEMEM */
