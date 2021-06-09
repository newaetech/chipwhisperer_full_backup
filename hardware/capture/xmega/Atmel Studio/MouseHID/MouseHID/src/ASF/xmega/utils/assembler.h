/**
 * \file
 *
 * \brief Assembler abstraction layer and utilities
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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
#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#if !defined(__ASSEMBLER__) && !defined(__IAR_SYSTEMS_ASM__) \
		&& !defined(__DOXYGEN__)
# error This file may only be included from assembly files
#endif

#if defined(__ASSEMBLER__)
# include "assembler/gas.h"
# include <avr/io.h>
#elif defined(__IAR_SYSTEMS_ASM__)
# include "assembler/iar.h"
# include <ioavr.h>
#endif

/**
 * \ingroup group_xmega_utils
 * \defgroup assembler_group Assembler Support
 *
 * This group provides a good handful of macros intended to smooth out
 * the differences between various assemblers, similar to what compiler.h does
 * for compilers, except that assemblers tend to be much less standardized than
 * compilers.
 *
 * @{
 */

//! \name Control Statements
//@{
/**
 * \def REPEAT(count)
 * \brief Repeat the following statements \a count times
 */
/**
 * \def END_REPEAT()
 * \brief Mark the end of the statements to be repeated
 */
/**
 * \def SET_LOC(offset)
 * \brief Set the location counter to \a offset
 */
/**
 * \def END_FILE()
 * \brief Mark the end of the file
 */
//@}

//! \name Data Objects
//@{
/**
 * \def FILL_BYTES(count)
 * \brief Allocate space for \a count bytes
 */
//@}

//! \name Symbol Definition
//@{
/**
 * \def L(name)
 * \brief Turn \a name into a local symbol, if possible
 */
/**
 * \def EXTERN_SYMBOL(name)
 * \brief Declare \a name as an external symbol referenced by this file
 */
/**
 * \def FUNCTION(name)
 * \brief Define a file-local function called \a name
 */
/**
 * \def PUBLIC_FUNCTION(name)
 * \brief Define a globally visible function called \a name
 */
/**
 * \def WEAK_FUNCTION(name)
 * \brief Define a weak function called \a name
 *
 * Weak functions are only referenced if no strong definitions are found
 */
/**
 * \def WEAK_FUNCTION_ALIAS(name, strong_name)
 * \brief Define \a name as a weak alias for the function \a strong_name
 * \sa WEAK_FUNCTION
 */
/**
 * \def END_FUNC(name)
 * \brief Mark the end of the function called \a name
 */
//@}

//! \name Section Definition
//@{
/**
 * \def TEXT_SECTION(name)
 * \brief Start a new section containing executable code
 */
/**
 * \def RODATA_SECTION(name)
 * \brief Start a new section containing read-only data
 */
/**
 * \def DATA_SECTION(name)
 * \brief Start a new section containing writeable initialized data
 */
/**
 * \def BSS_SECTION(name)
 * \brief Start a new section containing writeable zero-initialized data
 */
//@}

//! @}

#endif /* ASSEMBLER_H_INCLUDED */
