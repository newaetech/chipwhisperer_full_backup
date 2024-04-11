/**
 * \file
 *
 * \brief Access to huge data memory with 8-bit AVR
 *
 * Do not include this file directly, but rather <hugemem.h>.
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
#ifndef AVR8_HUGEMEM_H_INCLUDED
#define AVR8_HUGEMEM_H_INCLUDED

#include <stdlib.h>
#include "conf_board.h"

/**
 * \weakgroup hugemem_group
 * @{
 */

/**
 * \internal
 * \defgroup hugemem_avr8_group Hugemem implementation for 8-bit AVR.
 *
 * GCC does not have native support for 24-bit pointers, and therefore requires
 * custom assembly functions for this purpose. The implemented functions leave
 * RAMPZ cleared upon exit.\par
 *
 * If the chip does not support huge memory, i.e., \a CONFIG_HAVE_HUGEMEM is
 * not defined, a generic implementation will be used.
 */

#if defined(CONFIG_HAVE_HUGEMEM) || defined(__DOXYGEN__)
# if XMEGA && !(XMEGA_A1 || XMEGA_A1U)
#  error CONFIG_HAVE_HUGEMEM is not supported on XMEGA parts without EBI.
# endif
# if defined(__GNUC__) || defined(__DOXYGEN__)
#include <stdint.h>

#include "compiler.h"

typedef uint32_t hugemem_ptr_t;

#define HUGEMEM_NULL    0

static inline uint_fast8_t hugemem_read8(const hugemem_ptr_t from)
{
	uint8_t value;

	asm volatile(
		"movw r30, %A1 \n\t"
		"out %2, %C1 \n\t"
		"ld %0, Z \n\t"
		"out %2, __zero_reg__ \n\t"
		: "=r"(value)
		: "r"(from), "i"(&RAMPZ)
		: "r30", "r31"
	);

	return value;
}

uint_fast16_t hugemem_read16(const hugemem_ptr_t from);
uint_fast32_t hugemem_read32(const hugemem_ptr_t from);

static inline void hugemem_write8(hugemem_ptr_t to, uint_fast8_t val)
{
	asm volatile(
		"movw r30, %A0 \n\t"
		"out %2, %C0 \n\t"
		"st Z, %1 \n\t"
		"out %2, __zero_reg__ \n\t"
		:
		: "r"(to), "r"(val), "i"(&RAMPZ)
		: "r30", "r31"
	);
}

void hugemem_write16(hugemem_ptr_t to, uint_fast16_t val);
void hugemem_write32(hugemem_ptr_t to, uint_fast32_t val);

# elif defined(__ICCAVR__)
#include <stdint.h>

typedef void __huge *hugemem_ptr_t;

#define HUGEMEM_NULL    NULL

static inline uint_fast8_t hugemem_read8(const hugemem_ptr_t from)
{
	return *(volatile __huge uint8_t *)from;
}

static inline uint_fast16_t hugemem_read16(const hugemem_ptr_t from)
{
	return *(volatile __huge uint16_t *)from;
}

static inline uint_fast32_t hugemem_read32(const hugemem_ptr_t from)
{
	return *(volatile __huge uint32_t *)from;
}

static inline void hugemem_write8(hugemem_ptr_t to, uint_fast8_t val)
{
	*(__huge uint8_t *)to = val;
}

static inline void hugemem_write16(hugemem_ptr_t to, uint_fast16_t val)
{
	*(__huge uint16_t *)to = val;
}

static inline void hugemem_write32(hugemem_ptr_t to, uint_fast32_t val)
{
	*(__huge uint32_t *)to = val;
}
# endif /* __ICCAVR__ */

void hugemem_read_block(void *to, const hugemem_ptr_t from, size_t size);
void hugemem_write_block(hugemem_ptr_t to, const void *from, size_t size);

#else
# include <generic/hugemem.h>
#endif /* CONFIG_HAVE_HUGEMEM */
//@}

#endif /* AVR8_HUGEMEM_H */
