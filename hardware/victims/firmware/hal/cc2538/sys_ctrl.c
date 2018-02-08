/******************************************************************************
*  Filename:       sys_ctrl.c
*  Revised:        $Date: 2013-04-29 09:29:18 +0200 (Mon, 29 Apr 2013) $
*  Revision:       $Revision: 9921 $
*
*  Description:    Driver for the system controller.
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

//*****************************************************************************
//
//! \addtogroup sysctl_api
//! @{
//
//*****************************************************************************

#include "hw_ints.h"
#include "hw_sys_ctrl.h"
#include "hw_flash_ctrl.h"
#include "hw_nvic.h"
#include "cpu.h"
#include "debug.h"
#include "interrupt.h"
#include "sys_ctrl.h"


//*****************************************************************************
//
// Arrays that maps the "peripheral set" number (which is stored in the
// third nibble of the SYS_CTRL_PERIPH_* defines) to the SYSCTL register that
// contains the relevant bit for that peripheral.
//
//*****************************************************************************

// Run mode registers
static const uint32_t g_pui32RCGCRegs[] =
{
    SYS_CTRL_RCGCGPT,
    SYS_CTRL_RCGCSSI,
    SYS_CTRL_RCGCUART,
    SYS_CTRL_RCGCI2C,
    SYS_CTRL_RCGCSEC,
    SYS_CTRL_RCGCRFC
};

// Sleep mode registers
static const uint32_t g_pui32SCGCRegs[] =
{
    SYS_CTRL_SCGCGPT,
    SYS_CTRL_SCGCSSI,
    SYS_CTRL_SCGCUART,
    SYS_CTRL_SCGCI2C,
    SYS_CTRL_SCGCSEC,
    SYS_CTRL_SCGCRFC
};

// Deep sleep mode registers
static const uint32_t g_pui32DCGCRegs[] =
{
    SYS_CTRL_DCGCGPT,
    SYS_CTRL_DCGCSSI,
    SYS_CTRL_DCGCUART,
    SYS_CTRL_DCGCI2C,
    SYS_CTRL_DCGCSEC,
    SYS_CTRL_DCGCRFC
};

// Reset registers
static const uint32_t g_pui32SRRegs[] =
{
    SYS_CTRL_SRGPT,
    SYS_CTRL_SRSSI,
    SYS_CTRL_SRUART,
    SYS_CTRL_SRI2C,
    SYS_CTRL_SRSEC,
};

// Masks for determining if a peripheral is enabled
static const uint32_t g_pui32DieCfgMask[] =
{
    FLASH_CTRL_DIECFG1_GPTM0_EN,
    FLASH_CTRL_DIECFG1_SSI0_EN,
    FLASH_CTRL_DIECFG1_UART0_EN,
    FLASH_CTRL_DIECFG1_I2C_EN,
    FLASH_CTRL_DIECFG2_PKA_EN,
    FLASH_CTRL_DIECFG2_RF_CORE_EN
};

//*****************************************************************************
//
// This macro extracts the array index out of the peripheral number.
//
//*****************************************************************************
#define SYS_CTRL_PERIPH_INDEX(a)  (((a) >> 8) & 0xf)


//*****************************************************************************
//
// This macro extracts the peripheral instance number and generates bit mask
//
//*****************************************************************************
#define SYS_CTRL_PERIPH_MASKBIT(a) (0x00000001 << ((a) & 0xf))

//*****************************************************************************
//
// This macro extracts the instance number out of the peripheral number.
//
//*****************************************************************************
#define SYS_CTRL_PERIPH_INSTANCE(a)  ((a) & 0xf)

//*****************************************************************************
//
//! \internal
//! Checks a peripheral identifier
//!
//! \param ui32Peripheral is the peripheral identifier.
//!
//! This function determines if a peripheral identifier is valid.
//!
//! \return Returns \b true if the peripheral identifier is valid and \b false
//! otherwise.
//
//*****************************************************************************
#ifdef ENABLE_ASSERT
static bool
SysCtrlPeripheralValid(uint32_t ui32Peripheral)
{
    return((ui32Peripheral == SYS_CTRL_PERIPH_GPT0)  ||
           (ui32Peripheral == SYS_CTRL_PERIPH_GPT1)  ||
           (ui32Peripheral == SYS_CTRL_PERIPH_GPT2)  ||
           (ui32Peripheral == SYS_CTRL_PERIPH_GPT3)  ||
           (ui32Peripheral == SYS_CTRL_PERIPH_SSI0)  ||
           (ui32Peripheral == SYS_CTRL_PERIPH_SSI1)  ||
           (ui32Peripheral == SYS_CTRL_PERIPH_UART0) ||
           (ui32Peripheral == SYS_CTRL_PERIPH_UART1) ||
           (ui32Peripheral == SYS_CTRL_PERIPH_I2C)   ||
           (ui32Peripheral == SYS_CTRL_PERIPH_PKA)   ||
           (ui32Peripheral == SYS_CTRL_PERIPH_AES)   ||
           (ui32Peripheral == SYS_CTRL_PERIPH_RFC));
}
#endif


//*****************************************************************************
//
//! Sets the general clocking of the device
//!
//! \param bExternalOsc32k is set to true for applications with
//!        external 32kHz crystal.
//! \param bInternalOsc selects internal 1-16MHz RC oscillator. If set to
//!        false, the external 0-32MHz crystal is used.
//! \param ui32SysDiv System Clock Setting.
//!
//! This function configures the clocking of the device.
//! The oscillator used and the system clock divider settings are
//! configured with this function.
//!
//! The \e ui32SysDiv argument must be only one of the following values:
//! \b SYS_CTRL_SYSDIV_32MHZ, \b SYS_CTRL_SYSDIV_16MHZ,
//! \b SYS_CTRL_SYSDIV_8MHZ,  \b SYS_CTRL_SYSDIV_4MHZ,
//! \b SYS_CTRL_SYSDIV_2MHZ,  \b SYS_CTRL_SYSDIV_1MHZ,
//! \b SYS_CTRL_SYSDIV_500KHZ,\b SYS_CTRL_SYSDIV_250KHZ.
//! Note \b SYS_CTRL_SYSDIV_32MHZ can not be selected when Internal Oscillator
//! is selected.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlClockSet(bool bExternalOsc32k, bool bInternalOsc,
                uint32_t ui32SysDiv)
{
    uint32_t ui32STA;
    uint32_t ui32Reg;
    uint32_t ui32TimeoutVal;
    uint32_t ui32Osc;

    // check input parameters
    ASSERT((ui32SysDiv == SYS_CTRL_SYSDIV_32MHZ  ||
            ui32SysDiv == SYS_CTRL_SYSDIV_16MHZ  ||
            ui32SysDiv == SYS_CTRL_SYSDIV_8MHZ   ||
            ui32SysDiv == SYS_CTRL_SYSDIV_4MHZ   ||
            ui32SysDiv == SYS_CTRL_SYSDIV_2MHZ   ||
            ui32SysDiv == SYS_CTRL_SYSDIV_1MHZ   ||
            ui32SysDiv == SYS_CTRL_SYSDIV_500KHZ ||
            ui32SysDiv == SYS_CTRL_SYSDIV_250KHZ) &&
           !((ui32SysDiv == SYS_CTRL_SYSDIV_32MHZ) && bInternalOsc));

    //
    // Enable AMP detect to make sure XOSC starts correctly
    //
    if(!bInternalOsc)
    {
        ui32Reg = HWREG(SYS_CTRL_CLOCK_CTRL) | SYS_CTRL_CLOCK_CTRL_AMP_DET;
        HWREG(SYS_CTRL_CLOCK_CTRL) = ui32Reg;
    }

    //
    // Set 32kHz clock, Osc and SysDiv
    //
    ui32Reg = HWREG(SYS_CTRL_CLOCK_CTRL);
    ui32Reg &= ~(SYS_CTRL_CLOCK_CTRL_OSC32K | SYS_CTRL_CLOCK_CTRL_OSC |
                 SYS_CTRL_CLOCK_CTRL_SYS_DIV_M);
    if(!bExternalOsc32k)
    {
        ui32Reg |= SYS_CTRL_CLOCK_CTRL_OSC32K;
    }
    ui32Osc = bInternalOsc ? SYS_CTRL_CLOCK_CTRL_OSC : 0;
    ui32Reg |= ui32Osc;
    ui32Reg |= ui32SysDiv;
    HWREG(SYS_CTRL_CLOCK_CTRL) = ui32Reg;

    //
    // Note: This wait loop could potentially be removed, if caution
    // is taken in code running after this call.
    //
    // If we have changed Osc settings, wait until change happens
    //
    ui32STA = HWREG(SYS_CTRL_CLOCK_STA);
    ui32TimeoutVal = 0;
    while((ui32Osc != (ui32STA & SYS_CTRL_CLOCK_CTRL_OSC)) &&
            (ui32TimeoutVal < SYS_CTRL_TIMEOUT))
    {
        SysCtrlDelay(16);
        ui32STA = HWREG(SYS_CTRL_CLOCK_STA);
        ui32TimeoutVal++;
    }
    ASSERT(ui32TimeoutVal < SYS_CTRL_TIMEOUT);

} // SysCtrlClockSet


//*****************************************************************************
//
//! Gets the processor clock rate
//!
//! This function determines the clock rate of the processor clock.
//!
//! \return The processor clock rate
//
//*****************************************************************************
uint32_t
SysCtrlClockGet(void)
{
    uint32_t ui32Clk;
    uint32_t ui32STA;
    uint32_t ui32SysDiv;

    ui32STA = HWREG(SYS_CTRL_CLOCK_STA);
    ui32SysDiv =
        (ui32STA & SYS_CTRL_CLOCK_STA_SYS_DIV_M) >> SYS_CTRL_CLOCK_STA_SYS_DIV_S;
    ui32Clk = 0;

    switch(ui32SysDiv)
    {
    case SYS_CTRL_SYSDIV_32MHZ:
        ui32Clk = SYS_CTRL_32MHZ;
        break;
    case SYS_CTRL_SYSDIV_16MHZ:
        ui32Clk = SYS_CTRL_16MHZ;
        break;
    case SYS_CTRL_SYSDIV_8MHZ:
        ui32Clk = SYS_CTRL_8MHZ;
        break;
    case SYS_CTRL_SYSDIV_4MHZ:
        ui32Clk = SYS_CTRL_4MHZ;
        break;
    case SYS_CTRL_SYSDIV_2MHZ:
        ui32Clk = SYS_CTRL_2MHZ;
        break;
    case SYS_CTRL_SYSDIV_1MHZ:
        ui32Clk = SYS_CTRL_1MHZ;
        break;
    case SYS_CTRL_SYSDIV_500KHZ:
        ui32Clk = SYS_CTRL_500KHZ;
        break;
    case SYS_CTRL_SYSDIV_250KHZ:
        ui32Clk = SYS_CTRL_250KHZ;
        break;
    }

    //
    // Return the computed clock rate.
    //
    return(ui32Clk);
} // SysCtrlClockGet

//*****************************************************************************
//
//! Sets the IO clocking of the device
//!
//! \param ui32IODiv System Clock Setting.
//!
//! This function configures the IO clocking of the device (that is, the
//! Baud rate clock for SSI and UART).
//!
//! The \e ui32IODiv argument must be only one of the following values:
//! \b SYS_CTRL_SYSDIV_32MHZ, \b SYS_CTRL_SYSDIV_16MHZ,
//! \b SYS_CTRL_SYSDIV_8MHZ,  \b SYS_CTRL_SYSDIV_4MHZ,
//! \b SYS_CTRL_SYSDIV_2MHZ,  \b SYS_CTRL_SYSDIV_1MHZ,
//! \b SYS_CTRL_SYSDIV_500KHZ,\b SYS_CTRL_SYSDIV_250KHZ.
//!
//! Note \b SYS_CTRL_SYSDIV_32MHZ cannot be selected when Internal Oscillator
//! is selected.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlIOClockSet(uint32_t ui32IODiv)
{
    uint32_t ui32RegVal;

    // check input parameters
    ASSERT(ui32IODiv == SYS_CTRL_SYSDIV_32MHZ  ||
           ui32IODiv == SYS_CTRL_SYSDIV_16MHZ  ||
           ui32IODiv == SYS_CTRL_SYSDIV_8MHZ   ||
           ui32IODiv == SYS_CTRL_SYSDIV_4MHZ   ||
           ui32IODiv == SYS_CTRL_SYSDIV_2MHZ   ||
           ui32IODiv == SYS_CTRL_SYSDIV_1MHZ   ||
           ui32IODiv == SYS_CTRL_SYSDIV_500KHZ ||
           ui32IODiv == SYS_CTRL_SYSDIV_250KHZ);

    ui32RegVal = HWREG(SYS_CTRL_CLOCK_CTRL);
    ui32RegVal &= ~SYS_CTRL_CLOCK_CTRL_IO_DIV_M;
    ui32RegVal |= (ui32IODiv << SYS_CTRL_CLOCK_CTRL_IO_DIV_S);
    HWREG(SYS_CTRL_CLOCK_CTRL) = ui32RegVal;
} // SysCtrlIOClockSet


//*****************************************************************************
//
//! Gets the IO clock rate
//!
//! This function returns the IO clocking of the device, i.e. the Baud
//! rate clock for SSI and UART.
//!
//! \return The IO clock rate
//
//*****************************************************************************
uint32_t
SysCtrlIOClockGet(void)
{
    uint32_t ui32Clk;
    uint32_t ui32STA;
    uint32_t ui32IODiv;

    ui32STA = HWREG(SYS_CTRL_CLOCK_STA);
    ui32IODiv =
        (ui32STA & SYS_CTRL_CLOCK_STA_IO_DIV_M) >> SYS_CTRL_CLOCK_STA_IO_DIV_S;

    ui32Clk = 0;

    switch(ui32IODiv)
    {
    case SYS_CTRL_SYSDIV_32MHZ:
        ui32Clk = SYS_CTRL_32MHZ;
        break;
    case SYS_CTRL_SYSDIV_16MHZ:
        ui32Clk = SYS_CTRL_16MHZ;
        break;
    case SYS_CTRL_SYSDIV_8MHZ:
        ui32Clk = SYS_CTRL_8MHZ;
        break;
    case SYS_CTRL_SYSDIV_4MHZ:
        ui32Clk = SYS_CTRL_4MHZ;
        break;
    case SYS_CTRL_SYSDIV_2MHZ:
        ui32Clk = SYS_CTRL_2MHZ;
        break;
    case SYS_CTRL_SYSDIV_1MHZ:
        ui32Clk = SYS_CTRL_1MHZ;
        break;
    case SYS_CTRL_SYSDIV_500KHZ:
        ui32Clk = SYS_CTRL_500KHZ;
        break;
    case SYS_CTRL_SYSDIV_250KHZ:
        ui32Clk = SYS_CTRL_250KHZ;
        break;
    }

    //
    // Return the computed clock rate.
    //
    return(ui32Clk);
} // SysCtrlIOClockGet


//*****************************************************************************
//
//! Provides a small delay
//!
//! \param ui32Count is the number of delay loop iterations to perform.
//!
//! This function provides a means of generating a constant length delay and
//! is written in assembly to keep the delay consistent across tool chains,
//! avoiding the need to tune the delay based on the tool chain in use.
//!
//! The loop takes 3 cycles/loop.
//!
//! \return None
//
//*****************************************************************************
#if defined(__ICCARM__) || defined(DOXYGEN)
void
SysCtrlDelay(uint32_t ui32Count)
{
    __asm("    subs    r0, #1\n"
          "    bne.n   SysCtrlDelay\n"
          "    bx      lr");
}
#endif
#if defined(__GNUC__)
void __attribute__((naked))
SysCtrlDelay(uint32_t ui32Count)
{
    __asm("    subs    r0, #1\n"
          "    bne     SysCtrlDelay\n"
          "    bx      lr");
}
#endif
#if defined(__KEIL__) || defined(__ARMCC_VERSION)
__asm void
SysCtrlDelay(uint32_t ui32Count)
{
    subs    r0, #1;
    bne     SysCtrlDelay;
    bx      lr;
}
#endif
//
// For CCS implement this function in pure assembly.  This prevents the TI
// compiler from doing funny things with the optimizer.
//
#if defined(__TI_COMPILER_VERSION__)
__asm("    .sect \".text:SysCtrlDelay\"\n"
      "    .clink\n"
      "    .thumbfunc SysCtrlDelay\n"
      "    .thumb\n"
      "    .global SysCtrlDelay\n"
      "SysCtrlDelay:\n"
      "    subs r0, #1\n"
      "    bne.n SysCtrlDelay\n"
      "    bx lr\n");
#endif


//*****************************************************************************
//
//! Resets the device
//!
//! This function performs a software reset of the entire device.  The
//! processor and all peripherals are reset and all device registers are
//! returned to their default values.
//!
//! \return This function does not return.
//
//*****************************************************************************
void
SysCtrlReset(void)
{
    //
    // Perform a software reset request.  This request causes the device to
    // reset, no further code is executed.
    //
    HWREG(SYS_CTRL_PWRDBG) = SYS_CTRL_PWRDBG_FORCE_WARM_RESET;

    //
    // The device should have reset, so this should never be reached.  Just in
    // case, loop forever.
    //
    while(1)
    {
    }
}


//*****************************************************************************
//
//! Puts the processor into sleep mode
//!
//! This function places the processor into sleep mode and does not return
//! until the processor returns to run mode.  The peripherals that are enabled
//! by SysCtrlPeripheralSleepEnable() continue to operate and can wake up the
//! processor.
//!
//! \sa SysCtrlPeripheralSleepEnable()
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlSleep(void)
{
    //
    // Wait for an interrupt.
    //
    CPUwfi();
}


//*****************************************************************************
//
//! Puts the processor into deep-sleep mode
//!
//! This function places the processor into deep-sleep mode and does not return
//! until the processor returns to run mode.  The peripherals that are enabled
//! by SysCtrlPeripheralDeepSleepEnable() continue to operate and can wake up
//! the processor (if not in power mode 1, 2, or 3).
//! Note the power mode should be set before going into deep sleep.
//!
//! \sa SysCtrlPowerModeSet(), SysCtrlPeripheralDeepSleepEnable()
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlDeepSleep(void)
{
#ifndef NO_CLOCK_DIVIDER_RESTORE
    bool  bRestoreSys;
    bool  bRestoreIO;
    uint32_t ui32Reg;

    ui32Reg = HWREG(SYS_CTRL_CLOCK_STA);
    bRestoreSys = (ui32Reg & SYS_CTRL_CLOCK_STA_SYS_DIV_M)==0;
    bRestoreIO  = (ui32Reg & SYS_CTRL_CLOCK_STA_IO_DIV_M)==0;
    if(bRestoreSys || bRestoreIO)
    {
        ui32Reg = HWREG(SYS_CTRL_CLOCK_CTRL);
        ui32Reg |= bRestoreSys? 0x1:0x0;
        ui32Reg |= bRestoreIO? 0x100:0x0;
        HWREG(SYS_CTRL_CLOCK_CTRL) = ui32Reg;
    }
#endif
    
    //
    // Enable deep-sleep.
    //
    HWREG(NVIC_SYS_CTRL) |= NVIC_SYS_CTRL_SLEEPDEEP;

    //
    // Wait for an interrupt.
    //
    CPUwfi();

    //
    // Disable deep-sleep so that a future sleep will work correctly.
    //
    HWREG(NVIC_SYS_CTRL) &= ~(NVIC_SYS_CTRL_SLEEPDEEP);

#ifndef NO_CLOCK_DIVIDER_RESTORE
    if(bRestoreSys || bRestoreIO)
    {
        ui32Reg = HWREG(SYS_CTRL_CLOCK_CTRL);
        ui32Reg &= bRestoreSys ? ~SYS_CTRL_CLOCK_CTRL_SYS_DIV_M : 0xffffffff;
        ui32Reg &= bRestoreIO ?  ~SYS_CTRL_CLOCK_CTRL_IO_DIV_M : 0xffffffff;
        HWREG(SYS_CTRL_CLOCK_CTRL) = ui32Reg;
    }
#endif
}



//*****************************************************************************
//
//! Determines if a peripheral is present
//!
//! \param ui32Peripheral is the peripheral in question.
//!
//! Determines if a particular peripheral is present in the device (that is,
//! is not permanently disabled).
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES,   \b SYS_CTRL_PERIPH_RFC.
//!
//! \return Returns \b true if the specified peripheral is present and \b false
//! if it is permanently disabled.
//
//*****************************************************************************
bool
SysCtrlPeripheralPresent(uint32_t ui32Peripheral)
{
    uint32_t ui32DieCfg;
    uint32_t ui32Mask;

    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));

    if(SYS_CTRL_PERIPH_INDEX(ui32Peripheral) ==
            SYS_CTRL_PERIPH_INDEX(SYS_CTRL_PERIPH_PKA) ||
            SYS_CTRL_PERIPH_INDEX(ui32Peripheral) ==
            SYS_CTRL_PERIPH_INDEX(SYS_CTRL_PERIPH_AES) ||
            SYS_CTRL_PERIPH_INDEX(ui32Peripheral) ==
            SYS_CTRL_PERIPH_INDEX(SYS_CTRL_PERIPH_RFC))
    {
        ui32DieCfg = HWREG(FLASH_CTRL_DIECFG2);
    }
    else
    {
        ui32DieCfg = HWREG(FLASH_CTRL_DIECFG1);
    }

    ui32Mask = (g_pui32DieCfgMask[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)] <<
                SYS_CTRL_PERIPH_INSTANCE(ui32Peripheral));

    // Mask with correct  mask and determine if this peripheral is
    // permanently disabled.
    if(ui32DieCfg & ui32Mask)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

//*****************************************************************************
//
//! Performs a software reset of a peripheral
//!
//! \param ui32Peripheral is the peripheral to reset.
//!
//! This function performs a software reset of the specified peripheral.  An
//! individual peripheral reset signal is asserted for a brief period and then
//! deasserted, leaving the peripheral in a operating state but in its reset
//! condition.
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPeripheralReset(uint32_t ui32Peripheral)
{
    volatile uint32_t ui32Delay;

    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));
    ASSERT(!(ui32Peripheral == SYS_CTRL_PERIPH_RFC));

    // Put the peripheral into the reset state.
    HWREG(g_pui32SRRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) |=
        SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);

    // Delay for a little bit.
    for(ui32Delay = 0; ui32Delay < 16; ui32Delay++) { }

    // Take the peripheral out of the reset state.
    HWREG(g_pui32SRRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) &=
        ~SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);
}


//*****************************************************************************
//
//! Enables a peripheral (in Run mode)
//!
//! \param ui32Peripheral is the peripheral to enable.
//!
//! Peripherals are enabled with this function.  At power-up, some peripherals
//! are disabled and must be enabled to operate or respond to
//! register reads/writes.
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES,   \b SYS_CTRL_PERIPH_RFC.
//!
//! \note The actual enabling of the peripheral might be delayed until some
//! time after this function returns. Ensure that the peripheral is not
//! accessed until enabled.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPeripheralEnable(uint32_t ui32Peripheral)
{
    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));

    // Enable module in Run Mode
    HWREG(g_pui32RCGCRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) |=
        SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);
}


//*****************************************************************************
//
//! Disables a peripheral (in Run mode)
//!
//! \param ui32Peripheral is the peripheral to disable.
//!
//! Peripherals are disabled with this function. Once disabled, peripherals do
//! not operate or respond to register reads/writes.
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES,   \b SYS_CTRL_PERIPH_RFC.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPeripheralDisable(uint32_t ui32Peripheral)
{
    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));

    // Disable module in Run Mode
    HWREG(g_pui32RCGCRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) &=
        ~SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);
}


//*****************************************************************************
//
//! Enables a peripheral in sleep mode
//!
//! \param ui32Peripheral is the peripheral to enable in sleep mode.
//!
//! This function allows a peripheral to continue operating when the processor
//! goes into sleep mode.  Because the clocking configuration of the device does
//! not change, any peripheral can safely continue operating while the
//! processor is in sleep mode, and can therefore wake the processor from sleep
//! mode.
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES,   \b SYS_CTRL_PERIPH_RFC.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPeripheralSleepEnable(uint32_t ui32Peripheral)
{
    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));

    // Enable this peripheral in sleep mode.
    HWREG(g_pui32SCGCRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) |=
        SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);
}


//*****************************************************************************
//
//! Disables a peripheral in sleep mode
//!
//! \param ui32Peripheral is the peripheral to disable in sleep mode.
//!
//! This function causes a peripheral to stop operating when the processor goes
//! into sleep mode.  Disabling peripherals while in sleep mode helps lower
//! the current draw of the device. If enabled (by SysCtrlPeripheralEnable()),
//! the peripheral automatically resume operation when the processor
//! leaves sleep mode, maintaining the entire state before entry into sleep
//! mode.
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES,   \b SYS_CTRL_PERIPH_RFC.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPeripheralSleepDisable(uint32_t ui32Peripheral)
{
    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));

    // Disable this peripheral in sleep mode.
    HWREG(g_pui32SCGCRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) &=
        ~SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);
}


//*****************************************************************************
//
//! Enables a peripheral in deep-sleep mode
//!
//! \param ui32Peripheral is the peripheral to enable in deep-sleep mode.
//!
//! This function allows a peripheral to continue operating when the processor
//! goes into deep-sleep mode. Because the clocking configuration of the device
//! can change, not all peripherals can safely continue operating while the
//! processor is in sleep mode. Safe operation depends on the chosen power mode.
//! The caller must make sensible choices.
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES,   \b SYS_CTRL_PERIPH_RFC.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPeripheralDeepSleepEnable(uint32_t ui32Peripheral)
{
    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));

    // Enable this peripheral in deep-sleep mode.
    HWREG(g_pui32DCGCRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) |=
        SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);
}


//*****************************************************************************
//
//! Disables a peripheral in deep-sleep mode
//!
//! \param ui32Peripheral is the peripheral to disable in deep-sleep mode.
//!
//! This function causes a peripheral to stop operating when the processor goes
//! into deep-sleep mode.  Disabling peripherals while in deep-sleep mode helps
//! to lower the current draw of the device, and can keep peripherals that
//! require a particular clock frequency from operating when the clock changes
//! as a result of entering deep-sleep mode.  If enabled (by
//! SysCtrlPeripheralEnable()), the peripheral automatically resumes operation
//! when the processor leaves deep-sleep mode, maintaining its entire
//! state from before entry into deep-sleep mode.
//!
//! The \e ui32Peripheral parameter must be one of the values:
//! \b SYS_CTRL_PERIPH_GPT0 , \b SYS_CTRL_PERIPH_GPT1,
//! \b SYS_CTRL_PERIPH_GPT2,  \b SYS_CTRL_PERIPH_GPT3,
//! \b SYS_CTRL_PERIPH_SSI0,  \b SYS_CTRL_PERIPH_SSI1,
//! \b SYS_CTRL_PERIPH_UART0, \b SYS_CTRL_PERIPH_UART1,
//! \b SYS_CTRL_PERIPH_I2C,   \b SYS_CTRL_PERIPH_PKA,
//! \b SYS_CTRL_PERIPH_AES,   \b SYS_CTRL_PERIPH_RFC.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPeripheralDeepSleepDisable(uint32_t ui32Peripheral)
{
    // Check the arguments.
    ASSERT(SysCtrlPeripheralValid(ui32Peripheral));

    // Disable this peripheral in deep-sleep mode.
    HWREG(g_pui32DCGCRegs[SYS_CTRL_PERIPH_INDEX(ui32Peripheral)]) &=
        ~SYS_CTRL_PERIPH_MASKBIT(ui32Peripheral);
}


//*****************************************************************************
//
//! Set Power Mode
//!
//! \param ui32PowerMode is the power mode to be entered.
//!
//! This function selects the power mode to enter when CM3 enters Deep Sleep
//! mode.
//! Power mode PM0 (\b SYS_CTRL_PM_NOACTION) is entered when the CPU
//! wakes up due to an interrupt.
//! Note only transitions to and from PM0 are legal  (that is, PM1 to PM2
//! cannot happen).
//!
//! The \e ui32PowerMode argument must be only one of the following values:
//! \b SYS_CTRL_PM_NOACTION, \b SYS_CTRL_PM_1, \b SYS_CTRL_PM_2 or
//! \b SYS_CTRL_PM_3.
//!
//! \sa SysCtrlDeepSleep().
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlPowerModeSet(uint32_t ui32PowerMode)
{
    // Check the arguments.
    ASSERT(ui32PowerMode == SYS_CTRL_PM_NOACTION ||
           ui32PowerMode == SYS_CTRL_PM_1        ||
           ui32PowerMode == SYS_CTRL_PM_2        ||
           ui32PowerMode == SYS_CTRL_PM_3);

    // Set power mode
    HWREG(SYS_CTRL_PMCTL) = ui32PowerMode;
}


//*****************************************************************************
//
//! Get Power Mode
//!
//! This function returns the current Power Mode setting.
//!
//! \return Power mode, i.e. one of the following values:
//! \b SYS_CTRL_PM_NOACTION, \b SYS_CTRL_PM_1, \b SYS_CTRL_PM_2 or
//! \b SYS_CTRL_PM_3.
//
//*****************************************************************************
uint32_t
SysCtrlPowerModeGet(void)
{
    uint32_t ui32PowerMode;

    ui32PowerMode = HWREG(SYS_CTRL_PMCTL);
    ui32PowerMode &= SYS_CTRL_PMCTL_PM_M;

    return(ui32PowerMode);
}



//*****************************************************************************
//
//! Enable Clock Loss Detection
//!
//! This function enables clock loss detection.
//!
//! \return None
//
//*****************************************************************************
void
SysCtrlClockLossDetectEnable(void)
{
    uint32_t ui32CLD;

    ui32CLD = HWREG(SYS_CTRL_CLD);
    ui32CLD |= SYS_CTRL_CLD_EN;
    HWREG(SYS_CTRL_CLD) = ui32CLD;
}


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
