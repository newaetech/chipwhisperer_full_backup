/**
 * \file
 *
 * \brief Program memory access
 *
 * Copyright (c) 2010 Atmel Corporation. All rights reserved.
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

#ifndef UTILS_PROGMEM_H
#define UTILS_PROGMEM_H

/**
 * \defgroup group_xmega_utils_progmem Program memory
 *
 * \ingroup group_xmega_utils
 *
 * \{
 */

/*! \name Program memory
 *
 * Macros for locating and accessing data in program memory.
 *
 * @{
 */
#if defined(__GNUC__) || defined(__DOXYGEN__)
# include <avr/pgmspace.h>
# define PROGMEM_LOCATION(type, name, loc) \
		type name __attribute__((section (#loc)))
# define PROGMEM_DECLARE(type, name) const type name __attribute__((__progmem__))
# define PROGMEM_STRING(x) PSTR(x)
# define PROGMEM_STRING_T  PGM_P
# define PROGMEM_T const
# define PROGMEM_PTR_T const *
# define PROGMEM_BYTE_ARRAY_T uint8_t*
# define PROGMEM_WORD_ARRAY_T uint16_t*
# define PROGMEM_READ_BYTE(x) pgm_read_byte(x)
# define PROGMEM_READ_WORD(x) pgm_read_word(x)

#elif defined(__ICCAVR__)
# include <pgmspace.h>
# ifndef __HAS_ELPM__
#  define _MEMATTR_ASF  __flash
# else /* __HAS_ELPM__ */
#  define _MEMATTR_ASF  __hugeflash
# endif /* __HAS_ELPM__ */
# define PROGMEM_LOCATION(type, name, loc) const _MEMATTR_ASF type name @ loc
# define PROGMEM_DECLARE(type, name) _MEMATTR_ASF type name
# define PROGMEM_STRING(x) ((_MEMATTR_ASF const char *)(x))
# define PROGMEM_STRING_T  char const _MEMATTR_ASF *
# define PROGMEM_T const _MEMATTR_ASF
# define PROGMEM_PTR_T const _MEMATTR_ASF *
# define PROGMEM_BYTE_ARRAY_T uint8_t const _MEMATTR_ASF *
# define PROGMEM_WORD_ARRAY_T uint16_t const _MEMATTR_ASF *
# define PROGMEM_READ_BYTE(x) *(x)
# define PROGMEM_READ_WORD(x) *(x)
#endif
//! @}

/**
 * \}
 */

#endif /* UTILS_PROGMEM_H */
