/*
 * Copyright (c) 2015-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*!
 * @file s32_core_e200.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Directive 4.9, Function-like macro
 * Function-like macros are used instead of inline functions in order to ensure
 * that the performance will not be decreased if the functions will not be
 * inlined by the compiler.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 2.5, Global macro not referenced.
 * The macros defined are used only on some of the drivers, so this might be reported
 * when the analysis is made only on one driver.
 */

/*
 * Tool Chains:
 *   __GNUC__   : GNU Compiler Collection
 *   __ghs__    : Green Hills ARM Compiler
 *   __ICCARM__ : IAR ARM Compiler
 *   __DCC__    : Wind River Diab Compiler
 */

#if !defined (CORE_E200_H)
#define CORE_E200_H


#ifdef __cplusplus
extern "C" {
#endif

/* definitions for assembler functions */

#if defined (__GNUC__)   /* GCC */
#define PPCASM             __asm__
#define VEC_ALIGN          PPCASM (" .align 4 ")
#define MFSPR( rn )        ({unsigned int rval; PPCASM volatile("mfspr %0,%1" : "=r" (rval) : "I" (rn)); rval;})
#define MTSPR(rn, v)       PPCASM volatile("mtspr %0,%1" :: "I" (rn), "r" (v))
#elif defined (__ghs__)    /* GreenHills */
#include <ppc_ghs.h>    /* for intrinsic functions only */
#define PPCASM             __asm
#define VEC_ALIGN          PPCASM (" .skip 0x0C ")
#define MFSPR( spr )       __MFSPR( spr )
#define MTSPR( spr, val )  __MTSPR( spr, val )
#elif defined (__DCC__) /* Diab */
#include <diab/ppcasm.h>    /* for intrinsic functions only */
#define PPCASM             asm
#define VEC_ALIGN          PPCASM (" .align 4 ")
#define MFSPR( spr )       __mfspr( spr )
#define MTSPR( spr, val )  __mtspr( spr, val )
#else
/* Keep compatibility with software analysis tools */
#define PPCASM             asm
#define VEC_ALIGN          
#define MFSPR( rn )        ((uint8_t)0U)
#define MTSPR(rn, v)       ((void)(v))
#endif

/** \brief  BKPT_ASM
 *
 *   Macro to be used to trigger an debug interrupt
 */
#define BKPT_ASM    PPCASM ("se_illegal")

/** \brief  Enable FPU
 *
 *   ENABLE_FPU indicates whether SystemInit will enable the Floating point unit (FPU)
 */
#if defined (__GNUC__)
#if defined (__VFP_FP__) && !defined (__SOFTFP__)
#define ENABLE_FPU
#endif
#endif /* if defined (__GNUC__) */


/** \brief  Enable interrupts
 */
#define ENABLE_INTERRUPTS() PPCASM volatile ("wrteei      1")

/** \brief  Disable interrupts
 */
#define DISABLE_INTERRUPTS() PPCASM volatile ("wrteei      0")


/** \brief  Enter low-power standby state
 *    WFI (Wait For Interrupt) makes the processor suspend execution (Clock is stopped) until an IRQ interrupts.
 */
#define STANDBY() PPCASM volatile ("wait")


/** \brief  Reverse byte order in a word.
 */
#define REV_BYTES_32(a, b) (b = ((a & 0xFF000000U) >> 24U) | ((a & 0xFF0000U) >> 8U) \
                                | ((a & 0xFF00U) << 8U) | ((a & 0xFFU) << 24U))

/** \brief  Reverse byte order in each halfword independently.
 */
#define REV_BYTES_16(a, b) (b = ((a & 0xFF000000U) >> 8U) | ((a & 0xFF0000U) << 8U) \
                                | ((a & 0xFF00U) >> 8U) | ((a & 0xFFU) << 8U))

/** \brief  Places a function in RAM.
 */
#if defined ( __GNUC__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION      __attribute__((section (".code_ram")))
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__ ((longcall));
#elif defined ( __ghs__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION      _Pragma("ghs callmode=far")
    #define END_FUNCTION_DECLARATION_RAMSECTION        __attribute__((section (".code_ram")));\
                                                       _Pragma("ghs callmode=default")
#elif defined ( __DCC__ )
    #define START_FUNCTION_DECLARATION_RAMSECTION      _Pragma("section CODE \".code_ram\"") \
                                                       _Pragma("use_section CODE")
    #define END_FUNCTION_DECLARATION_RAMSECTION        ; \
                                                       _Pragma("section CODE \".text_vle\"")

#else
    /* Keep compatibility with software analysis tools */
    #define START_FUNCTION_DECLARATION_RAMSECTION
    #define END_FUNCTION_DECLARATION_RAMSECTION        ;
#endif

    #define DISABLE_CHECK_RAMSECTION_FUNCTION_CALL
    #define ENABLE_CHECK_RAMSECTION_FUNCTION_CALL



/** \brief  Get Core ID
 *
 *   GET_CORE_ID returns the processor identification number for e200z
 */
#define GET_CORE_ID()	MFSPR(286)

/** \brief  Data alignment.
 */
#if defined ( __GNUC__ ) || defined ( __ghs__ ) || defined ( __DCC__ )
    #define ALIGNED(x)      __attribute__((aligned(x)))
#elif defined ( __ICCARM__ )
    #define stringify(s) tostring(s)
    #define tostring(s) #s
    #define ALIGNED(x)      _Pragma(stringify(data_alignment=x))
#else
    /* Keep compatibility with software analysis tools */
    #define ALIGNED(x)
#endif

/** \brief  Endianness.
 */
#define CORE_BIG_ENDIAN

#ifdef __cplusplus
}
#endif

#endif /* CORE_E200_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
