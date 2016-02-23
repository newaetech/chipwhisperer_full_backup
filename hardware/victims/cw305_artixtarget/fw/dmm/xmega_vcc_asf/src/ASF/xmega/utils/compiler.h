/**
 * \file
 *
 * \brief Commonly used includes, types and macros.
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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
#ifndef UTILS_COMPILER_H
#define UTILS_COMPILER_H

/**
 * \defgroup group_xmega_utils XMEGA compiler driver
 *
 * Compiler abstraction layer and code utilities for 8-bit AVR.
 * This module provides various abstraction layers and utilities to make code compatible between different compilers.
 *
 * \{
 */

#if defined(__GNUC__)
#  include <avr/io.h>
#  include <avr/builtins.h>
#elif defined(__ICCAVR__)
#  include <ioavr.h>
#  include <intrinsics.h>
#else
#  error Unsupported compiler.
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <parts.h>

#ifdef __ICCAVR__
/*! \name Compiler Keywords
 *
 * Port of some keywords from GCC to IAR Embedded Workbench.
 */
//! @{
#define __asm__            asm
#define __inline__         inline
#define __volatile__
//! @}
#endif

/**
 * \def UNUSED
 * \brief Marking \a v as a unused parameter or value.
 */
#define UNUSED(v)          (void)(v)

/**
 * \def unused
 * \brief Marking \a v as a unused parameter or value.
 */
#define unused(v)          do { (void)(v); } while(0)

/**
 * \def barrier
 * \brief Memory barrier
 */
#ifdef __GNUC__
#  define barrier()        asm volatile("" ::: "memory")
#else
#  define barrier()        asm ("")
#endif

/**
 * \brief Emit the compiler pragma \a arg.
 *
 * \param arg The pragma directive as it would appear after \e \#pragma
 * (i.e. not stringified).
 */
#define COMPILER_PRAGMA(arg)            _Pragma(#arg)

/*
 * AVR arch does not care about alignment anyway.
 */
#define COMPILER_PACK_RESET(alignment)
#define COMPILER_PACK_SET(alignment)

/**
 * \brief Set aligned boundary.
 */
#if (defined __GNUC__)
#define COMPILER_ALIGNED(a)    __attribute__((__aligned__(a)))
#elif (defined __ICCAVR__)
#define COMPILER_ALIGNED(a)    COMPILER_PRAGMA(data_alignment = a)
#endif

/**
 * \brief Set word-aligned boundary.
 */
#if (defined __GNUC__)
#define COMPILER_WORD_ALIGNED    __attribute__((__aligned__(2)))
#elif (defined __ICCAVR__)
#define COMPILER_WORD_ALIGNED    COMPILER_PRAGMA(data_alignment = 2)
#endif

/**
 * \name Tag functions as deprecated
 *
 * Tagging a function as deprecated will produce a warning when and only
 * when the function is called.
 *
 * Usage is to add the __DEPRECATED__ symbol before the function definition.
 * E.g.:
 * __DEPRECATED__ uint8_t some_deprecated_function (void)
 * {
 *     ...
 * }
 *
 * \note Only supported by GCC 3.1 and above, no IAR support
 * @{
 */
#if ((defined __GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >=1)))
#define __DEPRECATED__  __attribute__((__deprecated__))
#else
#define __DEPRECATED__
#endif
//! @}

/*! \name Usual Types
 */
//! @{
typedef unsigned char           Bool; //!< Boolean.
#ifndef __cplusplus
#if !defined(__bool_true_false_are_defined)
typedef unsigned char           bool; //!< Boolean.
#endif
#endif
typedef int8_t     S8 ;  //!< 8-bit signed integer.
typedef uint8_t    U8 ;  //!< 8-bit unsigned integer.
typedef int16_t    S16;  //!< 16-bit signed integer.
typedef uint16_t   U16;  //!< 16-bit unsigned integer.
typedef uint16_t   le16_t;
typedef uint16_t   be16_t;
typedef int32_t    S32;  //!< 32-bit signed integer.
typedef uint32_t   U32;  //!< 32-bit unsigned integer.
typedef uint32_t   le32_t;
typedef uint32_t   be32_t;
typedef int64_t    S64;  //!< 64-bit signed integer.
typedef uint64_t   U64;  //!< 64-bit unsigned integer.
typedef float      F32;  //!< 32-bit floating-point number.
typedef double     F64;  //!< 64-bit floating-point number.
typedef uint16_t   iram_size_t;
//! @}


/*! \name Status Types
 */
//! @{
typedef Bool       Status_bool_t;  //!< Boolean status.
typedef U8         Status_t;       //!< 8-bit-coded status.
//! @}


/*! \name Aliasing Aggregate Types
 */
//! @{

//! 16-bit union.
typedef union
{
  S16 s16   ;
  U16 u16   ;
  S8  s8 [2];
  U8  u8 [2];
} Union16;

//! 32-bit union.
typedef union
{
  S32 s32   ;
  U32 u32   ;
  S16 s16[2];
  U16 u16[2];
  S8  s8 [4];
  U8  u8 [4];
} Union32;

//! 64-bit union.
typedef union
{
  S64 s64   ;
  U64 u64   ;
  S32 s32[2];
  U32 u32[2];
  S16 s16[4];
  U16 u16[4];
  S8  s8 [8];
  U8  u8 [8];
} Union64;

//! Union of pointers to 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  S64 *s64ptr;
  U64 *u64ptr;
  S32 *s32ptr;
  U32 *u32ptr;
  S16 *s16ptr;
  U16 *u16ptr;
  S8  *s8ptr ;
  U8  *u8ptr ;
} UnionPtr;

//! Union of pointers to volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  volatile S64 *s64ptr;
  volatile U64 *u64ptr;
  volatile S32 *s32ptr;
  volatile U32 *u32ptr;
  volatile S16 *s16ptr;
  volatile U16 *u16ptr;
  volatile S8  *s8ptr ;
  volatile U8  *u8ptr ;
} UnionVPtr;

//! Union of pointers to constant 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  const S64 *s64ptr;
  const U64 *u64ptr;
  const S32 *s32ptr;
  const U32 *u32ptr;
  const S16 *s16ptr;
  const U16 *u16ptr;
  const S8  *s8ptr ;
  const U8  *u8ptr ;
} UnionCPtr;

//! Union of pointers to constant volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
  const volatile S64 *s64ptr;
  const volatile U64 *u64ptr;
  const volatile S32 *s32ptr;
  const volatile U32 *u32ptr;
  const volatile S16 *s16ptr;
  const volatile U16 *u16ptr;
  const volatile S8  *s8ptr ;
  const volatile U8  *u8ptr ;
} UnionCVPtr;

//! Structure of pointers to 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  S64 *s64ptr;
  U64 *u64ptr;
  S32 *s32ptr;
  U32 *u32ptr;
  S16 *s16ptr;
  U16 *u16ptr;
  S8  *s8ptr ;
  U8  *u8ptr ;
} StructPtr;

//! Structure of pointers to volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  volatile S64 *s64ptr;
  volatile U64 *u64ptr;
  volatile S32 *s32ptr;
  volatile U32 *u32ptr;
  volatile S16 *s16ptr;
  volatile U16 *u16ptr;
  volatile S8  *s8ptr ;
  volatile U8  *u8ptr ;
} StructVPtr;

//! Structure of pointers to constant 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  const S64 *s64ptr;
  const U64 *u64ptr;
  const S32 *s32ptr;
  const U32 *u32ptr;
  const S16 *s16ptr;
  const U16 *u16ptr;
  const S8  *s8ptr ;
  const U8  *u8ptr ;
} StructCPtr;

//! Structure of pointers to constant volatile 64-, 32-, 16- and 8-bit unsigned integers.
typedef struct
{
  const volatile S64 *s64ptr;
  const volatile U64 *u64ptr;
  const volatile S32 *s32ptr;
  const volatile U32 *u32ptr;
  const volatile S16 *s16ptr;
  const volatile U16 *u16ptr;
  const volatile S8  *s8ptr ;
  const volatile U8  *u8ptr ;
} StructCVPtr;

//! @}


//_____ M A C R O S ________________________________________________________

/*! \name Usual Constants
 */
//! @{
#define DISABLE   0
#define ENABLE    1
#ifndef __cplusplus
#if !defined(__bool_true_false_are_defined)
#define false     0
#define true      1
#endif
#endif
#define PASS      0
#define FAIL      1
#define LOW       0
#define HIGH      1
//! @}


//! \name Compile time error handling
//@{

/**
 * \internal
 * \def ERROR_FUNC(name, msg)
 * \brief Fail compilation if function call isn't eliminated
 *
 * If the compiler fails to optimize away all calls to the function \a
 * name, terminate compilation and display \a msg to the user.
 *
 * \note Not all compilers support this, so this is best-effort only.
 * Sometimes, there may be a linker error instead, and when optimization
 * is disabled, this mechanism will be completely disabled.
 */
#ifndef ERROR_FUNC
# define ERROR_FUNC(name, msg)                      \
	extern int name(void)
#endif

//@}

//! \name Function call demultiplexing
//@{

//! Error function for failed demultiplexing.
ERROR_FUNC(compiler_demux_bad_size, "Invalid parameter size");

/**
 * \internal
 * \brief Demultiplex function call based on size of datatype
 *
 * Evaluates to a function call to a function name with suffix 8, 16 or 32
 * depending on the size of the datatype. Any number of parameters can be
 * passed to the function.
 *
 * Usage:
 * \code
	 void foo8(uint8_t a, void *b);
	 void foo16(uint16_t a, void *b);
	 void foo32(uint32_t a, void *b);

	 #define foo(x, y)    compiler_demux_size(sizeof(x), foo, x, y)
\endcode
 *
 * \param size Size of the datatype.
 * \param func Base function name.
 * \param ... List of parameters to pass to the function.
 */
#define compiler_demux_size(size, func, ...)        \
	(((size) == 1) ? func##8(__VA_ARGS__) :     \
	 ((size) == 2) ? func##16(__VA_ARGS__) :    \
	 ((size) == 4) ? func##32(__VA_ARGS__) :    \
	 compiler_demux_bad_size())

//@}

/**
 * \def __always_inline
 * \brief The function should always be inlined.
 *
 * This annotation instructs the compiler to ignore its inlining
 * heuristics and inline the function no matter how big it thinks it
 * becomes.
 */
#if (defined __GNUC__)
	#define __always_inline     inline __attribute__((__always_inline__))
#elif (defined __ICCAVR__)
	#define __always_inline     _Pragma("inline=forced")
#endif

//! \name Optimization Control
//@{

/**
 * \def __always_optimize
 * \brief The function should always be optimized.
 *
 * This annotation instructs the compiler to ignore global optimization
 * settings and always compile the function with a high level of
 * optimization.
 */
#if (defined __GNUC__)
	#define __always_optimize   __attribute__((optimize(3)))
#elif (defined __ICCAVR__)
	#define __always_optimize   _Pragma("optimize=high")
#endif

/**
 * \def likely(exp)
 * \brief The expression \a exp is likely to be true
 */
#ifndef likely
# define likely(exp)    (exp)
#endif

/**
 * \def unlikely(exp)
 * \brief The expression \a exp is unlikely to be true
 */
#ifndef unlikely
# define unlikely(exp)  (exp)
#endif

/**
 * \def is_constant(exp)
 * \brief Determine if an expression evaluates to a constant value.
 *
 * \param exp Any expression
 *
 * \return true if \a exp is constant, false otherwise.
 */
#ifdef __GNUC__
# define is_constant(exp)       __builtin_constant_p(exp)
#else
# define is_constant(exp)       (0)
#endif

//! @}

/*! \name Bit-Field Handling
 */
#include "bit_handling/clz_ctz.h"
//! @{

/*! \brief Reads the bits of a value specified by a given bit-mask.
 *
 * \param value Value to read bits from.
 * \param mask  Bit-mask indicating bits to read.
 *
 * \return Read bits.
 */
#define Rd_bits( value, mask)        ((value)&(mask))

/*! \brief Writes the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue to write bits to.
 * \param mask    Bit-mask indicating bits to write.
 * \param bits    Bits to write.
 *
 * \return Resulting value with written bits.
 */
#define Wr_bits(lvalue, mask, bits)  ((lvalue) = ((lvalue) & ~(mask)) |\
                                                 ((bits  ) &  (mask)))

/*! \brief Tests the bits of a value specified by a given bit-mask.
 *
 * \param value Value of which to test bits.
 * \param mask  Bit-mask indicating bits to test.
 *
 * \return \c 1 if at least one of the tested bits is set, else \c 0.
 */
#define Tst_bits( value, mask)  (Rd_bits(value, mask) != 0)

/*! \brief Clears the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to clear bits.
 * \param mask    Bit-mask indicating bits to clear.
 *
 * \return Resulting value with cleared bits.
 */
#define Clr_bits(lvalue, mask)  ((lvalue) &= ~(mask))

/*! \brief Sets the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to set bits.
 * \param mask    Bit-mask indicating bits to set.
 *
 * \return Resulting value with set bits.
 */
#define Set_bits(lvalue, mask)  ((lvalue) |=  (mask))

/*! \brief Toggles the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to toggle bits.
 * \param mask    Bit-mask indicating bits to toggle.
 *
 * \return Resulting value with toggled bits.
 */
#define Tgl_bits(lvalue, mask)  ((lvalue) ^=  (mask))

/*! \brief Reads the bit-field of a value specified by a given bit-mask.
 *
 * \param value Value to read a bit-field from.
 * \param mask  Bit-mask indicating the bit-field to read.
 *
 * \return Read bit-field.
 */
#define Rd_bitfield( value,mask)           (Rd_bits( value, (uint32_t)mask) >> ctz(mask))

/*! \brief Writes the bit-field of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue    C lvalue to write a bit-field to.
 * \param mask      Bit-mask indicating the bit-field to write.
 * \param bitfield  Bit-field to write.
 *
 * \return Resulting value with written bit-field.
 */
#define Wr_bitfield(lvalue, mask, bitfield) (Wr_bits(lvalue, mask, (uint32_t)(bitfield) << ctz(mask)))

//! @}


/*! \brief This macro is used to test fatal errors.
 *
 * The macro tests if the expression is false. If it is, a fatal error is
 * detected and the application hangs up. If TEST_SUITE_DEFINE_ASSERT_MACRO
 * is defined, a unit test version of the macro is used, to allow execution
 * of further tests after a false expression.
 *
 * \param expr  Expression to evaluate and supposed to be nonzero.
 */
#if defined(_ASSERT_ENABLE_)
#  if defined(TEST_SUITE_DEFINE_ASSERT_MACRO)
	// Assert() is defined in unit_test/suite.h
#    include "unit_test/suite.h"
#  else
#    define Assert(expr) \
	{\
		if (!(expr)) while (true);\
	}
#  endif
#else
#  define Assert(expr) ((void) 0)
#endif

/*! \name Bit Reversing
 */
//! @{

/*! \brief Reverses the bits of \a u8.
 *
 * \param u8  U8 of which to reverse the bits.
 *
 * \return Value resulting from \a u8 with reversed bits.
 */
#define bit_reverse8(u8)    ((U8)(bit_reverse32((U8)(u8)) >> 24))

/*! \brief Reverses the bits of \a u16.
 *
 * \param u16 U16 of which to reverse the bits.
 *
 * \return Value resulting from \a u16 with reversed bits.
 */
#define bit_reverse16(u16)  ((U16)(bit_reverse32((U16)(u16)) >> 16))

/*! \brief Reverses the bits of \a u32.
 *
 * \param u32 U32 of which to reverse the bits.
 *
 * \return Value resulting from \a u32 with reversed bits.
 */
#if (defined __GNUC__)
  #define bit_reverse32(u32) \
  (\
    {\
      unsigned int __value = (U32)(u32);\
      __asm__ ("brev\t%0" : "+r" (__value) :  : "cc");\
      (U32)__value;\
    }\
  )
#elif (defined __ICCAVR__)
  #define bit_reverse32(u32)  ((U32)__bit_reverse((U32)(u32)))
#endif

/*! \brief Reverses the bits of \a u64.
 *
 * \param u64 U64 of which to reverse the bits.
 *
 * \return Value resulting from \a u64 with reversed bits.
 */
#define bit_reverse64(u64)  ((U64)(((U64)bit_reverse32((U64)(u64) >> 32)) |\
                                   ((U64)bit_reverse32((U64)(u64)) << 32)))

//! @}

//! \name Logarithmic functions
//! @{

/**
 * \internal
 * Undefined function. Will cause a link failure if ilog2() is called
 * with an invalid constant value.
 */
int_fast8_t ilog2_undefined(void);

/**
 * \brief Calculate the base-2 logarithm of a number rounded down to
 * the nearest integer.
 *
 * \param x A 32-bit value
 * \return The base-2 logarithm of \a x, or -1 if \a x is 0.
 */
static inline int_fast8_t ilog2(uint32_t x)
{
	if (is_constant(x))
		return ((x) & (1ULL << 31) ? 31 :
			(x) & (1ULL << 30) ? 30 :
			(x) & (1ULL << 29) ? 29 :
			(x) & (1ULL << 28) ? 28 :
			(x) & (1ULL << 27) ? 27 :
			(x) & (1ULL << 26) ? 26 :
			(x) & (1ULL << 25) ? 25 :
			(x) & (1ULL << 24) ? 24 :
			(x) & (1ULL << 23) ? 23 :
			(x) & (1ULL << 22) ? 22 :
			(x) & (1ULL << 21) ? 21 :
			(x) & (1ULL << 20) ? 20 :
			(x) & (1ULL << 19) ? 19 :
			(x) & (1ULL << 18) ? 18 :
			(x) & (1ULL << 17) ? 17 :
			(x) & (1ULL << 16) ? 16 :
			(x) & (1ULL << 15) ? 15 :
			(x) & (1ULL << 14) ? 14 :
			(x) & (1ULL << 13) ? 13 :
			(x) & (1ULL << 12) ? 12 :
			(x) & (1ULL << 11) ? 11 :
			(x) & (1ULL << 10) ? 10 :
			(x) & (1ULL <<  9) ?  9 :
			(x) & (1ULL <<  8) ?  8 :
			(x) & (1ULL <<  7) ?  7 :
			(x) & (1ULL <<  6) ?  6 :
			(x) & (1ULL <<  5) ?  5 :
			(x) & (1ULL <<  4) ?  4 :
			(x) & (1ULL <<  3) ?  3 :
			(x) & (1ULL <<  2) ?  2 :
			(x) & (1ULL <<  1) ?  1 :
			(x) & (1ULL <<  0) ?  0 :
			ilog2_undefined());

	return 31 - clz(x);
}

//! @}

/*! \name Alignment
 */
//! @{

/*! \brief Tests alignment of the number \a val with the \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return \c 1 if the number \a val is aligned with the \a n boundary, else \c 0.
 */
#define Test_align(val, n     ) (!Tst_bits( val, (n) - 1     )   )

/*! \brief Gets alignment of the number \a val with respect to the \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return Alignment of the number \a val with respect to the \a n boundary.
 */
#define Get_align( val, n     ) (  Rd_bits( val, (n) - 1     )   )

/*! \brief Sets alignment of the lvalue number \a lval to \a alg with respect to the \a n boundary.
 *
 * \param lval  Input/output lvalue.
 * \param n     Boundary.
 * \param alg   Alignment.
 *
 * \return New value of \a lval resulting from its alignment set to \a alg with respect to the \a n boundary.
 */
#define Set_align(lval, n, alg) (  Wr_bits(lval, (n) - 1, alg)   )

/*! \brief Aligns the number \a val with the upper \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return Value resulting from the number \a val aligned with the upper \a n boundary.
 */
#define Align_up(  val, n     ) (((val) + ((n) - 1)) & ~((n) - 1))

/*! \brief Aligns the number \a val with the lower \a n boundary.
 *
 * \param val Input value.
 * \param n   Boundary.
 *
 * \return Value resulting from the number \a val aligned with the lower \a n boundary.
 */
#define Align_down(val, n     ) ( (val)              & ~((n) - 1))

//! @}


/*! \name Mathematics
 *
 * Compiler optimization for non-constant expressions, only for abs under WinAVR
 */
//! @{

/*! \brief Takes the absolute value of \a a.
 *
 * \param a Input value.
 *
 * \return Absolute value of \a a.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Abs(a)              (((a) <  0 ) ? -(a) : (a))
#ifndef abs
#define abs(a)              Abs(a)
#endif

/*! \brief Takes the minimal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Minimal value of \a a and \a b.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Min(a, b)           (((a) < (b)) ?  (a) : (b))
#define min(a, b)           Min(a, b)

/*! \brief Takes the maximal value of \a a and \a b.
 *
 * \param a Input value.
 * \param b Input value.
 *
 * \return Maximal value of \a a and \a b.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Max(a, b)           (((a) > (b)) ?  (a) : (b))
#define max(a, b)           Max(a, b)

//! @}


/*! \brief Calls the routine at address \a addr.
 *
 * It generates a long call opcode.
 *
 * For example, `Long_call(0x80000000)' generates a software reset on a UC3 if
 * it is invoked from the CPU supervisor mode.
 *
 * \param addr  Address of the routine to call.
 *
 * \note It may be used as a long jump opcode in some special cases.
 */
#define Long_call(addr)                   ((*(void (*)(void))(addr))())

/*! \name System Register Access
 */
//! @{

/*! \brief Gets the value of the \a sysreg system register.
 *
 * \param sysreg  Address of the system register of which to get the value.
 *
 * \return Value of the \a sysreg system register.
 */
#if (defined __GNUC__)
  #define Get_system_register(sysreg)         __builtin_mfsr(sysreg)
#elif (defined __ICCAVR__)
  #define Get_system_register(sysreg)         __get_system_register(sysreg)
#endif

/*! \brief Sets the value of the \a sysreg system register to \a value.
 *
 * \param sysreg  Address of the system register of which to set the value.
 * \param value   Value to set the \a sysreg system register to.
 */
#if (defined __GNUC__)
  #define Set_system_register(sysreg, value)  __builtin_mtsr(sysreg, value)
#elif (defined __ICCAVR__)
  #define Set_system_register(sysreg, value)  __set_system_register(sysreg, value)
#endif

//! @}

/*! \name Debug Register Access
 */
//! @{

/*! \brief Gets the value of the \a dbgreg debug register.
 *
 * \param dbgreg  Address of the debug register of which to get the value.
 *
 * \return Value of the \a dbgreg debug register.
 */
#if (defined __GNUC__)
  #define Get_debug_register(dbgreg)          __builtin_mfdr(dbgreg)
#elif (defined __ICCAVR__)
  #define Get_debug_register(dbgreg)          __get_debug_register(dbgreg)
#endif

/*! \brief Sets the value of the \a dbgreg debug register to \a value.
 *
 * \param dbgreg  Address of the debug register of which to set the value.
 * \param value   Value to set the \a dbgreg debug register to.
 */
#if (defined __GNUC__)
  #define Set_debug_register(dbgreg, value)   __builtin_mtdr(dbgreg, value)
#elif (defined __ICCAVR__)
  #define Set_debug_register(dbgreg, value)   __set_debug_register(dbgreg, value)
#endif

//! @}


/*! \name MCU Endianism Handling
 * xmega is a MCU little endianism.
 */
//! @{
#define  MSB(u16)          (((uint8_t* )&u16)[1])
#define  LSB(u16)          (((uint8_t* )&u16)[0])

#define  MSW(u32)          (((uint16_t*)&u32)[1])
#define  LSW(u32)          (((uint16_t*)&u32)[0])
#define  MSB0W(u32)        (((uint8_t*)&(u32))[3])  //!< Most significant byte of 1st rank of \a u32.
#define  MSB1W(u32)        (((uint8_t*)&(u32))[2])  //!< Most significant byte of 2nd rank of \a u32.
#define  MSB2W(u32)        (((uint8_t*)&(u32))[1])  //!< Most significant byte of 3rd rank of \a u32.
#define  MSB3W(u32)        (((uint8_t*)&(u32))[0])  //!< Most significant byte of 4th rank of \a u32.
#define  LSB3W(u32)        MSB0W(u32)            //!< Least significant byte of 4th rank of \a u32.
#define  LSB2W(u32)        MSB1W(u32)            //!< Least significant byte of 3rd rank of \a u32.
#define  LSB1W(u32)        MSB2W(u32)            //!< Least significant byte of 2nd rank of \a u32.
#define  LSB0W(u32)        MSB3W(u32)            //!< Least significant byte of 1st rank of \a u32.

#define  MSB0(u32)         (((uint8_t*)&u32)[3])
#define  MSB1(u32)         (((uint8_t*)&u32)[2])
#define  MSB2(u32)         (((uint8_t*)&u32)[1])
#define  MSB3(u32)         (((uint8_t*)&u32)[0])
#define  LSB0(u32)         MSB3(u32)
#define  LSB1(u32)         MSB2(u32)
#define  LSB2(u32)         MSB1(u32)
#define  LSB3(u32)         MSB0(u32)

#define  LE16(x)        (x)
#define  le16_to_cpu(x) (x)
#define  cpu_to_le16(x) (x)
#define  LE16_TO_CPU(x) (x)
#define  CPU_TO_LE16(x) (x)

#define  BE16(x)        Swap16(x)
#define  be16_to_cpu(x) swap16(x)
#define  cpu_to_be16(x) swap16(x)
#define  BE16_TO_CPU(x) Swap16(x)
#define  CPU_TO_BE16(x) Swap16(x)

#define  LE32(x)        (x)
#define  le32_to_cpu(x) (x)
#define  cpu_to_le32(x) (x)
#define  LE32_TO_CPU(x) (x)
#define  CPU_TO_LE32(x) (x)

#define  BE32(x)        Swap32(x)
#define  be32_to_cpu(x) swap32(x)
#define  cpu_to_be32(x) swap32(x)
#define  BE32_TO_CPU(x) Swap32(x)
#define  CPU_TO_BE32(x) Swap32(x)



//! @}


/*! \name Endianism Conversion
 *
 * The same considerations as for clz and ctz apply here but AVR32-GCC's
 * __builtin_bswap_16 and __builtin_bswap_32 do not behave like macros when
 * applied to constant expressions, so two sets of macros are defined here:
 *   - Swap16, Swap32 and Swap64 to apply to constant expressions (values known
 *     at compile time);
 *   - swap16, swap32 and swap64 to apply to non-constant expressions (values
 *     unknown at compile time).
 */
//! @{

/*! \brief Toggles the endianism of \a u16 (by swapping its bytes).
 *
 * \param u16 U16 of which to toggle the endianism.
 *
 * \return Value resulting from \a u16 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Swap16(u16) ((U16)(((U16)(u16) >> 8) |\
                           ((U16)(u16) << 8)))

/*! \brief Toggles the endianism of \a u32 (by swapping its bytes).
 *
 * \param u32 U32 of which to toggle the endianism.
 *
 * \return Value resulting from \a u32 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Swap32(u32) ((U32)(((U32)Swap16((U32)(u32) >> 16)) |\
                           ((U32)Swap16((U32)(u32)) << 16)))

/*! \brief Toggles the endianism of \a u64 (by swapping its bytes).
 *
 * \param u64 U64 of which to toggle the endianism.
 *
 * \return Value resulting from \a u64 with toggled endianism.
 *
 * \note More optimized if only used with values known at compile time.
 */
#define Swap64(u64) ((U64)(((U64)Swap32((U64)(u64) >> 32)) |\
                           ((U64)Swap32((U64)(u64)) << 32)))

/*! \brief Toggles the endianism of \a u16 (by swapping its bytes).
 *
 * \param u16 U16 of which to toggle the endianism.
 *
 * \return Value resulting from \a u16 with toggled endianism.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#define swap16(u16) Swap16(u16)

/*! \brief Toggles the endianism of \a u32 (by swapping its bytes).
 *
 * \param u32 U32 of which to toggle the endianism.
 *
 * \return Value resulting from \a u32 with toggled endianism.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#define swap32(u32) Swap32(u32)

/*! \brief Toggles the endianism of \a u64 (by swapping its bytes).
 *
 * \param u64 U64 of which to toggle the endianism.
 *
 * \return Value resulting from \a u64 with toggled endianism.
 *
 * \note More optimized if only used with values unknown at compile time.
 */
#define swap64(u64) ((U64)(((U64)swap32((U64)(u64) >> 32)) |\
                           ((U64)swap32((U64)(u64)) << 32)))

//! @}


/*! \name Target Abstraction
 */
//! @{

#define _GLOBEXT_           extern      //!< extern storage-class specifier.
#define _CONST_TYPE_        const       //!< const type qualifier.
#define _MEM_TYPE_SLOW_                 //!< Slow memory type.
#define _MEM_TYPE_MEDFAST_              //!< Fairly fast memory type.
#define _MEM_TYPE_FAST_                 //!< Fast memory type.

typedef U8                  Byte;       //!< 8-bit unsigned integer.

#define memcmp_ram2ram      memcmp      //!< Target-specific memcmp of RAM to RAM.
#define memcmp_code2ram     memcmp      //!< Target-specific memcmp of RAM to NVRAM.
#define memcpy_ram2ram      memcpy      //!< Target-specific memcpy from RAM to RAM.
#define memcpy_code2ram     memcpy      //!< Target-specific memcpy from NVRAM to RAM.

//! @}

/**
 * \brief Calculate \f$ \left\lceil \frac{a}{b} \right\rceil \f$ using
 * integer arithmetic.
 *
 * \param a An integer
 * \param b Another integer
 *
 * \return (\a a / \a b) rounded up to the nearest integer.
 */
#define div_ceil(a, b)	(((a) + (b) - 1) / (b))

#include "preprocessor.h"
#include "progmem.h"
#include "interrupt.h"


#if (defined __GNUC__)
  #define SHORTENUM                           __attribute__ ((packed))
#elif (defined __ICCAVR__)
  #define SHORTENUM                           /**/
#endif

#if (defined __GNUC__)
  #define FUNC_PTR                            void *
#elif (defined __ICCAVR__)
#if (FLASHEND > 0x1FFFF)    // Required for program code larger than 128K
  #define FUNC_PTR                            void __farflash *
#else
  #define FUNC_PTR                            void *
#endif  /* ENABLE_FAR_FLASH */
#endif


#if (defined __GNUC__)
  #define FLASH_DECLARE(x)                  const x __attribute__((__progmem__))
#elif (defined __ICCAVR__)
  #define FLASH_DECLARE(x)                  const __flash x
#endif

#if (defined __GNUC__)
  #define FLASH_EXTERN(x) extern const x
#elif (defined __ICCAVR__)
  #define FLASH_EXTERN(x) extern const __flash x
#endif


/*Defines the Flash Storage for the request and response of MAC*/
#define CMD_ID_OCTET    (0)

/* Converting of values from CPU endian to little endian. */
#define CPU_ENDIAN_TO_LE16(x)   (x)
#define CPU_ENDIAN_TO_LE32(x)   (x)
#define CPU_ENDIAN_TO_LE64(x)   (x)

/* Converting of values from little endian to CPU endian. */
#define LE16_TO_CPU_ENDIAN(x)   (x)
#define LE32_TO_CPU_ENDIAN(x)   (x)
#define LE64_TO_CPU_ENDIAN(x)   (x)

/* Converting of constants from little endian to CPU endian. */
#define CLE16_TO_CPU_ENDIAN(x)  (x)
#define CLE32_TO_CPU_ENDIAN(x)  (x)
#define CLE64_TO_CPU_ENDIAN(x)  (x)

/* Converting of constants from CPU endian to little endian. */
#define CCPU_ENDIAN_TO_LE16(x)  (x)
#define CCPU_ENDIAN_TO_LE32(x)  (x)
#define CCPU_ENDIAN_TO_LE64(x)  (x)

#if (defined __GNUC__)
  #define ADDR_COPY_DST_SRC_16(dst, src)  memcpy((&(dst)), (&(src)), sizeof(uint16_t))
  #define ADDR_COPY_DST_SRC_64(dst, src)  memcpy((&(dst)), (&(src)), sizeof(uint64_t))

/* Converts a 2 Byte array into a 16-Bit value */
#define convert_byte_array_to_16_bit(data) \
    (*(uint16_t *)(data))

/* Converts a 4 Byte array into a 32-Bit value */
#define convert_byte_array_to_32_bit(data) \
    (*(uint32_t *)(data))

/* Converts a 8 Byte array into a 64-Bit value */
#define convert_byte_array_to_64_bit(data) \
    (*(uint64_t *)(data))

/* Converts a 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_spec_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_address(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts a 32-Bit value into a 4 Byte array */
#define convert_32_bit_to_byte_array(value, data) \
    ((*(uint32_t *)(data)) = (uint32_t)(value))

/* Converts a 64-Bit value into  a 8 Byte array */
/* Here memcpy requires much less footprint */
#define convert_64_bit_to_byte_array(value, data) \
    memcpy((data), (&(value)), sizeof(uint64_t))

#elif (defined __ICCAVR__)
  #define ADDR_COPY_DST_SRC_16(dst, src)  ((dst) = (src))
  #define ADDR_COPY_DST_SRC_64(dst, src)  ((dst) = (src))

/* Converts a 2 Byte array into a 16-Bit value */
#define convert_byte_array_to_16_bit(data) \
    (*(uint16_t *)(data))

/* Converts a 4 Byte array into a 32-Bit value */
#define convert_byte_array_to_32_bit(data) \
    (*(uint32_t *)(data))

/* Converts a 8 Byte array into a 64-Bit value */
#define convert_byte_array_to_64_bit(data) \
    (*(uint64_t *)(data))

/* Converts a 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_spec_16_bit_to_byte_array(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts spec 16-Bit value into a 2 Byte array */
#define convert_16_bit_to_byte_address(value, data) \
    ((*(uint16_t *)(data)) = (uint16_t)(value))

/* Converts a 32-Bit value into a 4 Byte array */
#define convert_32_bit_to_byte_array(value, data) \
    ((*(uint32_t *)(data)) = (uint32_t)(value))

/* Converts a 64-Bit value into  a 8 Byte array */
#define convert_64_bit_to_byte_array(value, data) \
    ((*(uint64_t *)(data)) = (uint64_t)(value))
#endif

#define MEMCPY_ENDIAN memcpy
#define PGM_READ_BLOCK(dst, src, len) memcpy_P((dst), (src), (len))

#if (defined __GNUC__)
  #define PGM_READ_BYTE(x) pgm_read_byte(x)
  #define PGM_READ_WORD(x) pgm_read_word(x)
#elif (defined __ICCAVR__)
  #define PGM_READ_BYTE(x) *(x)
  #define PGM_READ_WORD(x) *(x)
#endif


#if (defined __GNUC__)
  #define nop() do { __asm__ __volatile__ ("nop"); } while (0)
#elif (defined __ICCAVR__)
  #define nop() __no_operation()
#endif


/**
 * \}
 */

#endif  // UTILS_COMPILER_H
