/******************************************************************************
*  Filename:       cpu.c
*  Revised:        $Date: 2013-01-21 15:25:21 +0100 (Mon, 21 Jan 2013) $
*  Revision:       $Revision: 9178 $
*
*  Description:    Instruction wrappers for special CPU instructions needed by
*                  the drivers.
*
*  Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/


#include "cpu.h"

//*****************************************************************************
//
// Wrapper function for the CPSID instruction.  Returns the state of PRIMASK
// on entry.
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked))
CPUcpsid(void)
{
    uint32_t ui32Ret;

    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n"
          "    bx      lr\n"
          : "=r" (ui32Ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ui32Ret);
}
#endif
#if (__ICCARM__)
uint32_t
CPUcpsid(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm uint32_t
CPUcpsid(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    mrs     r0, PRIMASK;
    cpsid   i;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
uint32_t
CPUcpsid(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n"
          "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif

//*****************************************************************************
//
// Wrapper function returning the state of PRIMASK (indicating whether
// interrupts are enabled or disabled).
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked))
CPUprimask(void)
{
    uint32_t ui32Ret;

    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    bx      lr\n"
          : "=r" (ui32Ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ui32Ret);
}
#endif
#if (__ICCARM__)
uint32_t
CPUprimask(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm uint32_t
CPUprimask(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    mrs     r0, PRIMASK;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
uint32_t
CPUprimask(void)
{
    //
    // Read PRIMASK and disable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif

//*****************************************************************************
//
// Wrapper function for the CPSIE instruction.  Returns the state of PRIMASK
// on entry.
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked))
CPUcpsie(void)
{
    uint32_t ui32Ret;

    //
    // Read PRIMASK and enable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsie   i\n"
          "    bx      lr\n"
          : "=r" (ui32Ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ui32Ret);
}
#endif
#if (__ICCARM__)
uint32_t
CPUcpsie(void)
{
    //
    // Read PRIMASK and enable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsie   i\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm uint32_t
CPUcpsie(void)
{
    //
    // Read PRIMASK and enable interrupts.
    //
    mrs     r0, PRIMASK;
    cpsie   i;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
uint32_t
CPUcpsie(void)
{
    //
    // Read PRIMASK and enable interrupts.
    //
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsie   i\n"
          "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif

//*****************************************************************************
//
// Wrapper function for the WFI instruction.
//
//*****************************************************************************
#if defined(__GNUC__)
void __attribute__((naked))
CPUwfi(void)
{
    //
    // Wait for the next interrupt.
    //
    __asm("    wfi\n"
          "    bx      lr\n");
}
#endif
#if (__ICCARM__)
void
CPUwfi(void)
{
    //
    // Wait for the next interrupt.
    //
    __asm("    wfi\n");
}
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm void
CPUwfi(void)
{
    //
    // Wait for the next interrupt.
    //
    wfi;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
void
CPUwfi(void)
{
    //
    // Wait for the next interrupt.
    //
    __asm("    wfi\n");
}
#endif

//*****************************************************************************
//
// Wrapper function for the WFE instruction.
//
//*****************************************************************************
#if defined(__GNUC__)
void __attribute__((naked))
CPUwfe(void)
{
    //
    // Wait for the next event
    //
    __asm("    wfe\n"
          "    bx      lr\n");
}
#endif
#if (__ICCARM__)
void
CPUwfe(void)
{
    //
    // Wait for the next event
    //
    __asm("    wfe\n");
}
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm void
CPUwfe(void)
{
    //
    // Wait for the next event
    //
    wfe;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
void
CPUwfe(void)
{
    //
    // Wait for the next event
    //
    __asm("    wfe\n");
}
#endif

//*****************************************************************************
//
// Wrapper function for the SEV instruction (Send event).
//
//*****************************************************************************
#if defined(__GNUC__)
void __attribute__((naked))
CPUsev(void)
{
    //
    // Send event
    //
    __asm("    sev\n"
          "    bx      lr\n");
}
#endif
#if (__ICCARM__)
void
CPUsev(void)
{
    //
    // Send event
    //
    __asm("    sev\n");
}
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm void
CPUsev(void)
{
    //
    // Send event
    //
    sev;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
void
CPUsev(void)
{
    //
    // Send event
    //
    __asm("    sev\n");
}
#endif

//*****************************************************************************
//
// Wrapper function for writing the BASEPRI register.
//
//*****************************************************************************
#if defined(__GNUC__)
void __attribute__((naked))
CPUbasepriSet(uint32_t ui32NewBasepri)
{

    //
    // Set the BASEPRI register
    //
    __asm("    msr     BASEPRI, r0\n"
          "    bx      lr\n");
}
#endif
#if (__ICCARM__)
void
CPUbasepriSet(uint32_t ui32NewBasepri)
{
    //
    // Set the BASEPRI register
    //
    __asm("    msr     BASEPRI, r0\n");
}
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm void
CPUbasepriSet(uint32_t ui32NewBasepri)
{
    //
    // Set the BASEPRI register
    //
    msr     BASEPRI, r0;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
void
CPUbasepriSet(uint32_t ui32NewBasepri)
{
    //
    // Set the BASEPRI register
    //
    __asm("    msr     BASEPRI, r0\n");
}
#endif

//*****************************************************************************
//
// Wrapper function for reading the BASEPRI register.
//
//*****************************************************************************
#if defined(__GNUC__)
uint32_t __attribute__((naked))
CPUbasepriGet(void)
{
    uint32_t ui32Ret;

    //
    // Read BASEPRI
    //
    __asm("    mrs     r0, BASEPRI\n"
          "    bx      lr\n"
          : "=r" (ui32Ret));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ui32Ret);
}
#endif
#if (__ICCARM__)
uint32_t
CPUbasepriGet(void)
{
    //
    // Read BASEPRI
    //
    __asm("    mrs     r0, BASEPRI\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(rvmdk) || defined(__ARMCC_VERSION)
__asm uint32_t
CPUbasepriGet(void)
{
    //
    // Read BASEPRI
    //
    mrs     r0, BASEPRI;
    bx      lr
}
#endif
#if defined(__TI_COMPILER_VERSION__)
uint32_t
CPUbasepriGet(void)
{
    //
    // Read BASEPRI
    //
    __asm("    mrs     r0, BASEPRI\n"
          "    bx      lr\n");

    //
    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    //
    return(0);
}
#endif
