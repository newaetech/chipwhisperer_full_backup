/**
 * \file
 *
 * \brief CLZ/CTZ C implementation.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#ifndef CLZ_CTH_H
#define CLZ_CTH_H

/**
 * \brief Count leading zeros in unsigned integer
 *
 * This macro takes unsigned integers of any size, and evaluates to a call to
 * the clz-function for its size. These functions count the number of zeros,
 * starting with the MSB, before a one occurs in the integer.
 *
 * \param x Unsigned integer to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
#define clz(x)    compiler_demux_size(sizeof(x), clz, (x))

/**
 * \internal
 * \brief Count leading zeros in unsigned, 8-bit integer
 *
 * \param x Unsigned byte to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t clz8(uint8_t x)
{
	uint8_t bit = 0;

	if (x & 0xf0) {
		x >>= 4;
	} else {
		bit += 4;
	}

	if (x & 0x0c) {
		x >>= 2;
	} else {
		bit += 2;
	}

	if (!(x & 0x02)) {
		bit++;
	}

	return bit;

}

/**
 * \internal
 * \brief Count leading zeros in unsigned, 16-bit integer
 *
 * \param x Unsigned word to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t clz16(uint16_t x)
{
	uint8_t bit = 0;

	if (x & 0xff00) {
		x >>= 8;
	} else {
		bit += 8;
	}

	return bit + clz8(x);
}

/**
 * \internal
 * \brief Count leading zeros in unsigned, 32-bit integer
 *
 * \param x Unsigned double word to count the leading zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t clz32(uint32_t x)
{
	uint8_t bit = 0;

	if (x & 0xffff0000) {
		x >>= 16;
	} else {
		bit += 16;
	}

	return bit + clz16(x);
}

/**
 * \brief Count trailing zeros in unsigned integer
 *
 * This macro takes unsigned integers of any size, and evaluates to a call to
 * the ctz-function for its size. These functions count the number of zeros,
 * starting with the LSB, before a one occurs in the integer.
 *
 * \param x Unsigned integer to count the trailing zeros in.
 *
 * \return The number of trailing zeros in \a x.
 */
#define ctz(x)    compiler_demux_size(sizeof(x), ctz, (x))

/**
 * \internal
 * \brief Count trailing zeros in unsigned, 8-bit integer
 *
 * \param x Unsigned byte to count the trailing zeros in.
 *
 * \return The number of leading zeros in \a x.
 */
__always_inline static uint8_t ctz8(uint8_t x)
{
	uint8_t bit = 0;

	if (!(x & 0x0f)) {
		bit += 4;
		x >>= 4;
	}
	if (!(x & 0x03)) {
		bit += 2;
		x >>= 2;
	}
	if (!(x & 0x01))
		bit++;

	return bit;
}

/**
 * \internal
 * \brief Count trailing zeros in unsigned, 16-bit integer
 *
 * \param x Unsigned word to count the trailing zeros in.
 *
 * \return The number of trailing zeros in \a x.
 */
__always_inline static uint8_t ctz16(uint16_t x)
{
	uint8_t bit = 0;

	if (!(x & 0x00ff)) {
		bit += 8;
		x >>= 8;
	}

	return bit + ctz8(x);
}

/**
 * \internal
 * \brief Count trailing zeros in unsigned, 32-bit integer
 *
 * \param x Unsigned double word to count the trailing zeros in.
 *
 * \return The number of trailing zeros in \a x.
 */
__always_inline static uint8_t ctz32(uint32_t x)
{
	uint8_t bit = 0;

	if (!(x & 0x0000ffff)) {
		bit += 16;
		x >>= 16;
	}

	return bit + ctz16(x);
}

#endif /* CLZ_CTZ_H */
