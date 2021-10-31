/******************************************************************************
*  Filename:       interrupt.c
*  Revised:        $Date: 2013-03-20 14:47:53 +0100 (Wed, 20 Mar 2013) $
*  Revision:       $Revision: 9489 $
*
*  Description:    Driver for the NVIC Interrupt Controller.
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
//! \addtogroup interrupt_api
//! @{
//
//*****************************************************************************

#include "hw_ints.h"
#include "hw_nvic.h"
#include "hw_sys_ctrl.h"
#include "cpu.h"
#include "debug.h"
#include "interrupt.h"

//*****************************************************************************
//
// This is a mapping between priority grouping encodings and the number of
// preemption priority bits.
//
//*****************************************************************************
static const uint32_t g_pui32Priority[] =
{
    NVIC_APINT_PRIGROUP_0_8, NVIC_APINT_PRIGROUP_1_7, NVIC_APINT_PRIGROUP_2_6,
    NVIC_APINT_PRIGROUP_3_5, NVIC_APINT_PRIGROUP_4_4, NVIC_APINT_PRIGROUP_5_3,
    NVIC_APINT_PRIGROUP_6_2, NVIC_APINT_PRIGROUP_7_1
};

//*****************************************************************************
//
// This is a mapping between interrupt number and the register that contains
// the priority encoding for that interrupt.
//
//*****************************************************************************
static const uint32_t g_pui32Regs[] =
{
    0, NVIC_SYS_PRI1, NVIC_SYS_PRI2, NVIC_SYS_PRI3, NVIC_PRI0,  NVIC_PRI1,
    NVIC_PRI2,  NVIC_PRI3,  NVIC_PRI4,  NVIC_PRI5,  NVIC_PRI6,  NVIC_PRI7,
    NVIC_PRI8,  NVIC_PRI9,  NVIC_PRI10, NVIC_PRI11, NVIC_PRI12, NVIC_PRI13,
    NVIC_PRI14, NVIC_PRI15, NVIC_PRI16, NVIC_PRI17, NVIC_PRI18, NVIC_PRI19,
    NVIC_PRI20, NVIC_PRI21, NVIC_PRI22, NVIC_PRI23, NVIC_PRI24, NVIC_PRI25,
    NVIC_PRI26, NVIC_PRI27, NVIC_PRI28, NVIC_PRI29, NVIC_PRI30, NVIC_PRI31,
    NVIC_PRI32, NVIC_PRI33, NVIC_PRI34, NVIC_PRI35, NVIC_PRI36
};

//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt enable for that
// interrupt.
//
//*****************************************************************************
static const uint32_t g_pui32EnRegs[] =
{
    NVIC_EN0, NVIC_EN1, NVIC_EN2, NVIC_EN3, NVIC_EN4
};

//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt disable for that
// interrupt.
//
//*****************************************************************************
static const uint32_t g_pui32DisRegs[] =
{
    NVIC_DIS0, NVIC_DIS1, NVIC_DIS2, NVIC_DIS3, NVIC_DIS4
};

//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt pend for that interrupt.
//
//*****************************************************************************
static const uint32_t g_pui32PendRegs[] =
{
    NVIC_PEND0, NVIC_PEND1, NVIC_PEND2, NVIC_PEND3, NVIC_PEND4
};

//*****************************************************************************
//
// This is a mapping between interrupt number (for the peripheral interrupts
// only) and the register that contains the interrupt unpend for that
// interrupt.
//
//*****************************************************************************
static const uint32_t g_pui32UnpendRegs[] =
{
    NVIC_UNPEND0, NVIC_UNPEND1, NVIC_UNPEND2, NVIC_UNPEND3, NVIC_UNPEND4
};

//*****************************************************************************
//
//! \internal
//! The default interrupt handler
//!
//! This is the default interrupt handler for all interrupts.  It simply loops
//! forever so that the system state is preserved for observation by a
//! debugger.  Since interrupts should be disabled before unregistering the
//! corresponding handler, this should never be called.
//!
//! \return None
//
//*****************************************************************************
static void
IntDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// The processor vector table
//
// This contains a list of the handlers for the various interrupt sources in
// the system.  The layout of this list is defined by the hardware; assertion
// of an interrupt causes the processor to start executing directly at the
// address given in the corresponding location in this list.
//
//*****************************************************************************
#if defined(__ICCARM__)
#pragma data_alignment=1024
static __no_init void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) @ "VTABLE";
#elif defined(__TI_COMPILER_VERSION__) || defined(DOXYGEN)
#pragma DATA_ALIGN(g_pfnRAMVectors, 1024)
#pragma DATA_SECTION(g_pfnRAMVectors, ".vtable")
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void);
#else
static __attribute__((section("vtable")))
void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void) __attribute__((aligned(1024)));
#endif

//*****************************************************************************
//
//! Enables the processor interrupt
//!
//! This function allows the processor to respond to interrupts.  This does not
//! affect the set of interrupts enabled in the interrupt controller; it just
//! gates the single interrupt from the controller to the processor.
//!
//! \return Returns \b true if interrupts were disabled when the function was
//! called or \b false if they were initially enabled.
//
//*****************************************************************************
bool
IntMasterEnable(void)
{
    //
    // Enable processor interrupts.
    //
    return(CPUcpsie());
}

//*****************************************************************************
//
//! Disables the processor interrupt
//!
//! This function prevents the processor from receiving interrupts. This does
//! not affect the set of interrupts enabled in the interrupt controller; it
//! just gates the single interrupt from the controller to the processor.
//!
//! \return Returns \b true if interrupts were already disabled when the
//! function was called or \b false if they were initially enabled.
//
//*****************************************************************************
bool
IntMasterDisable(void)
{
    //
    // Disable processor interrupts.
    //
    return(CPUcpsid());
}

//*****************************************************************************
//
//! Registers a function to be called when an interrupt occurs
//!
//! \param ui32Interrupt specifies the interrupt in question.
//! \param pfnHandler is a pointer to the function to be called.
//!
//! This function specifies the handler function to be called when the
//! given interrupt is asserted to the processor.  When the interrupt occurs,
//! if it is enabled (through IntEnable()), the handler function is called in
//! interrupt context. Because the handler function can preempt other code, care
//! must be taken to protect memory or peripherals that are accessed by the
//! handler and other nonhandler code.
//!
//! \note This function (directly or indirectly through a peripheral
//! driver interrupt register function) moves the interrupt vector table from
//! flash to SRAM.  Therefore, care must be taken when linking the application
//! to ensure that the SRAM vector table is located at the beginning of SRAM;
//! otherwise NVIC will not look in the correct portion of memory for the
//! vector table (it requires the vector table be on a 1-kB memory alignment).
//! Normally, the SRAM vector table is so placed through the use of linker
//! scripts. See the discussion of compile-time versus runtime interrupt handler
//! registration in the introduction to this chapter.
//!
//! \return None
//
//*****************************************************************************
void
IntRegister(uint32_t ui32Interrupt, void (*pfnHandler)(void))
{
    uint32_t ui32Idx, ui32Value;

    //
    // Check the arguments.
    //
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    // Check below could be removed in final application
#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
    ASSERT(IntAltMapIsEnabled());
#else
    ASSERT(~IntAltMapIsEnabled());
#endif

    //
    // Make sure that the RAM vector table is correctly aligned.
    //
    ASSERT(((uint32_t)g_pfnRAMVectors & 0x000003ff) == 0);

    //
    // See if the RAM vector table has been initialized.
    //
    if(HWREG(NVIC_VTABLE) != (uint32_t)g_pfnRAMVectors)
    {
        //
        // Copy the vector table from the beginning of FLASH to the RAM vector
        // table.
        //
        ui32Value = HWREG(NVIC_VTABLE);
        for(ui32Idx = 0; ui32Idx < NUM_INTERRUPTS; ui32Idx++)
        {
            g_pfnRAMVectors[ui32Idx] = (void (*)(void))HWREG((ui32Idx * 4) +
                                       ui32Value);
        }

        //
        // Point NVIC at the RAM vector table.
        //
        HWREG(NVIC_VTABLE) = (uint32_t)g_pfnRAMVectors;
    }

    //
    // Save the interrupt handler.
    //
    g_pfnRAMVectors[ui32Interrupt] = pfnHandler;
}

//*****************************************************************************
//
//! Unregisters the function to be called when an interrupt occurs
//!
//! \param ui32Interrupt specifies the interrupt in question.
//!
//! This function indicates that no handler should be called when the
//! given interrupt is asserted to the processor.  The interrupt source is
//! automatically disabled (through IntDisable()) if necessary.
//!
//! \sa See IntRegister() for important information about registering interrupt
//! handlers.
//!
//! \return None
//
//*****************************************************************************
void
IntUnregister(uint32_t ui32Interrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    //
    // Check that at least one interrupt was dynamically registered
    // (by calling IntRegister())
    //
    ASSERT(HWREG(NVIC_VTABLE) == (uint32_t)g_pfnRAMVectors);

    // Check below could be removed in final application
#ifdef CC2538_USE_ALTERNATE_INTERRUPT_MAP
    ASSERT(IntAltMapIsEnabled());
#else
    ASSERT(~IntAltMapIsEnabled());
#endif


    //
    // Reset the interrupt handler.
    //
    g_pfnRAMVectors[ui32Interrupt] = IntDefaultHandler;
}

//*****************************************************************************
//
//! Sets the priority grouping of the interrupt controller
//!
//! \param ui32Bits specifies the number of bits of preemptable priority.
//!
//! This function specifies the split between preemptable priority levels and
//! subpriority levels in the interrupt priority specification.  The range of
//! the grouping values depend on the hardware implementation; on
//! the CC2538 device family, 3 bits are available for hardware interrupt
//! prioritization and therefore priority grouping values of three through
//! seven have the same effect.
//!
//! \return None
//
//*****************************************************************************
void
IntPriorityGroupingSet(uint32_t ui32Bits)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Bits < NUM_PRIORITY);

    //
    // Set the priority grouping.
    //
    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | g_pui32Priority[ui32Bits];
}

//*****************************************************************************
//
//! Gets the priority grouping of the interrupt controller
//!
//! This function returns the split between preemptable priority levels and
//! subpriority levels in the interrupt priority specification.
//!
//! \return Returns the number of bits of preemptable priority
//
//*****************************************************************************
uint32_t
IntPriorityGroupingGet(void)
{
    uint32_t ui32Loop, ui32Value;

    //
    // Read the priority grouping.
    //
    ui32Value = HWREG(NVIC_APINT) & NVIC_APINT_PRIGROUP_M;

    //
    // Loop through the priority grouping values.
    //
    for(ui32Loop = 0; ui32Loop < NUM_PRIORITY; ui32Loop++)
    {
        //
        // Stop looping if this value matches.
        //
        if(ui32Value == g_pui32Priority[ui32Loop])
        {
            break;
        }
    }

    //
    // Return the number of priority bits.
    //
    return(ui32Loop);
}

//*****************************************************************************
//
//! Sets the priority of an interrupt
//!
//! \param ui32Interrupt specifies the interrupt in question.
//! \param ui8Priority specifies the priority of the interrupt.
//!
//! This function sets the priority of an interrupt.  When multiple
//! interrupts are asserted simultaneously, those with the highest priority
//! are processed before the lower priority interrupts.  Smaller numbers
//! correspond to higher interrupt priorities; priority 0 is the highest
//! interrupt priority.
//!
//! The hardware priority mechanism will looks only at the upper N bits of the
//! priority level (where N is 3 for the CC2538 device family), so any
//! prioritization must be performed in those bits.  The remaining bits can be
//! used to subprioritize the interrupt sources, and may be used by the
//! hardware priority mechanism on a future part.  This arrangement allows
//! priorities to migrate to different NVIC implementations without changing
//! the gross prioritization of the interrupts.
//! Thus for CC2538 to set a priority of 3, the parameter \e ui8Priority must
//! be set to (3<<5).
//!
//! \return None
//
//*****************************************************************************
void
IntPrioritySet(uint32_t ui32Interrupt, uint8_t ui8Priority)
{
    uint32_t ui32Temp;

    //
    // Check the arguments.
    //
    ASSERT((ui32Interrupt >= 4) && (ui32Interrupt < NUM_INTERRUPTS));

    //
    // Set the interrupt priority.
    //
    ui32Temp = HWREG(g_pui32Regs[ui32Interrupt >> 2]);
    ui32Temp &= ~(0xFF << (8 * (ui32Interrupt & 3)));
    ui32Temp |= ui8Priority << (8 * (ui32Interrupt & 3));
    HWREG(g_pui32Regs[ui32Interrupt >> 2]) = ui32Temp;
}

//*****************************************************************************
//
//! Gets the priority of an interrupt
//!
//! \param ui32Interrupt specifies the interrupt in question.
//!
//! This function gets the priority of an interrupt.  See IntPrioritySet() for
//! a definition of the priority value.
//!
//! \return Returns the interrupt priority, or -1 if an invalid interrupt was
//! specified
//
//*****************************************************************************
int32_t
IntPriorityGet(uint32_t ui32Interrupt)
{
    //
    // Check the arguments.
    //
    ASSERT((ui32Interrupt >= 4) && (ui32Interrupt < NUM_INTERRUPTS));

    //
    // Return the interrupt priority.
    //
    return((HWREG(g_pui32Regs[ui32Interrupt >> 2]) >> (8 * (ui32Interrupt & 3))) &
           0xFF);
}

//*****************************************************************************
//
//! Enables an interrupt
//!
//! \param ui32Interrupt specifies the interrupt to be enabled.
//!
//! This function enables the specified interrupt in the interrupt controller.
//! Other enables for the interrupt (such as at the peripheral level) are
//! unaffected by this function.
//!
//! \return None
//
//*****************************************************************************
void
IntEnable(uint32_t ui32Interrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to enable.
    //
    if(ui32Interrupt == FAULT_MPU)
    {
        //
        // Enable the MemManage interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_MEM;
    }
    else if(ui32Interrupt == FAULT_BUS)
    {
        //
        // Enable the bus fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_BUS;
    }
    else if(ui32Interrupt == FAULT_USAGE)
    {
        //
        // Enable the usage fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_USAGE;
    }
    else if(ui32Interrupt == FAULT_SYSTICK)
    {
        //
        // Enable the System Tick interrupt.
        //
        HWREG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_INTEN;
    }
    else if(ui32Interrupt >= 16)
    {
        //
        // Enable the general interrupt.
        //
        HWREG(g_pui32EnRegs[(ui32Interrupt - 16) / 32]) =
            1 << ((ui32Interrupt - 16) & 31);
    }
}

//*****************************************************************************
//
//! Disables an interrupt
//!
//! \param ui32Interrupt specifies the interrupt to be disabled.
//!
//! This function disables specified interrupt in the interrupt controller.
//! Other enables for the interrupt (such as at the peripheral level) are
//! unaffected by this function.
//!
//! \return None
//
//*****************************************************************************
void
IntDisable(uint32_t ui32Interrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to disable.
    //
    if(ui32Interrupt == FAULT_MPU)
    {
        //
        // Disable the MemManage interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_MEM);
    }
    else if(ui32Interrupt == FAULT_BUS)
    {
        //
        // Disable the bus fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_BUS);
    }
    else if(ui32Interrupt == FAULT_USAGE)
    {
        //
        // Disable the usage fault interrupt.
        //
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_USAGE);
    }
    else if(ui32Interrupt == FAULT_SYSTICK)
    {
        //
        // Disable the System Tick interrupt.
        //
        HWREG(NVIC_ST_CTRL) &= ~(NVIC_ST_CTRL_INTEN);
    }
    else if(ui32Interrupt >= 16)
    {
        //
        // Disable the general interrupt.
        //
        HWREG(g_pui32DisRegs[(ui32Interrupt - 16) / 32]) =
            1 << ((ui32Interrupt - 16) & 31);
    }
}

//*****************************************************************************
//
//! Pends an interrupt
//!
//! \param ui32Interrupt specifies the interrupt to be pended.
//!
//! This function pends the specified interrupt in the interrupt controller.
//! This causes the interrupt controller to execute the corresponding interrupt
//! handler at the next available time, based on the current interrupt state
//! priorities.  For example, if called by a higher priority interrupt handler,
//! the specified interrupt handler is not called until after the current
//! interrupt handler executes. The interrupt must have been enabled for
//! it to be called.
//!
//! \return None
//
//*****************************************************************************
void
IntPendSet(uint32_t ui32Interrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to pend.
    //
    if(ui32Interrupt == FAULT_NMI)
    {
        //
        // Pend the NMI interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_NMI_SET;
    }
    else if(ui32Interrupt == FAULT_PENDSV)
    {
        //
        // Pend the PendSV interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
    }
    else if(ui32Interrupt == FAULT_SYSTICK)
    {
        //
        // Pend the SysTick interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PENDSTSET;
    }
    else if(ui32Interrupt >= 16)
    {
        //
        // Pend the general interrupt.
        //
        HWREG(g_pui32PendRegs[(ui32Interrupt - 16) / 32]) =
            1 << ((ui32Interrupt - 16) & 31);
    }
}

//*****************************************************************************
//
//! Unpends an interrupt
//!
//! \param ui32Interrupt specifies the interrupt to be unpended.
//!
//! This function unpends the specified interrupt in the interrupt controller.
//! This will cause any previously generated interrupts that have not been
//! handled yet (due to higher priority interrupts or the interrupt no having
//! been enabled yet) to be discarded.
//!
//! \return None
//
//*****************************************************************************
void
IntPendClear(uint32_t ui32Interrupt)
{
    //
    // Check the arguments.
    //
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    //
    // Determine the interrupt to unpend.
    //
    if(ui32Interrupt == FAULT_PENDSV)
    {
        //
        // Unpend the PendSV interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_UNPEND_SV;
    }
    else if(ui32Interrupt == FAULT_SYSTICK)
    {
        //
        // Unpend the SysTick interrupt.
        //
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PENDSTCLR;
    }
    else if(ui32Interrupt >= 16)
    {
        //
        // Unpend the general interrupt.
        //
        HWREG(g_pui32UnpendRegs[(ui32Interrupt - 16) / 32]) =
            1 << ((ui32Interrupt - 16) & 31);
    }
}

//*****************************************************************************
//
//! Sets the priority masking level
//!
//! \param ui32PriorityMask is the priority level that will be masked.
//!
//! This function sets the interrupt priority masking level so that all
//! interrupts at the specified or lesser priority level is masked.  This
//! can be used to globally disable a set of interrupts with priority below
//! a predetermined threshold.  A value of 0 disables priority
//! masking.
//!
//! Smaller numbers correspond to higher interrupt priorities.  For example,
//! a priority level mask of 4 allows interrupts of priority level 0-3,
//! and interrupts with a numerical priority of 4 and greater are blocked.
//!
//! The hardware priority mechanism looks only at the upper N bits of the
//! priority level (where N is 3 for the CC2538 device family), so any
//! prioritization must be performed in those bits.
//!
//! \return None
//
//*****************************************************************************
void
IntPriorityMaskSet(uint32_t ui32PriorityMask)
{
    CPUbasepriSet(ui32PriorityMask);
}

//*****************************************************************************
//
//! Gets the priority masking level
//!
//! This function gets the current setting of the interrupt priority masking
//! level.  The value returned is the priority level such that all interrupts
//! of that priority and lesser priorities are masked.  A value of 0 disables
//! priority masking.
//!
//! Smaller numbers correspond to higher interrupt priorities.  For example,
//! a priority level mask of 4 allows interrupts of priority level 0-3,
//! and interrupts with a numerical priority of 4 and greater will be blocked.
//!
//! The hardware priority mechanism looks only at the upper N bits of the
//! priority level (where N is 3 for the CC2538 device family), so any
//! prioritization must be performed in those bits.
//!
//! \return Returns the value of the interrupt priority level mask
//
//*****************************************************************************
uint32_t
IntPriorityMaskGet(void)
{
    return(CPUbasepriGet());
}

//*****************************************************************************
//
//! Enables the alternate interrupt mapping
//!
//! This function enables the alternate (that is, smaller) interrupt map.
//!
//! \sa See also IntAltMapDisable() and IntAltMapIsEnabled().
//!
//! \return None
//
//*****************************************************************************
void IntAltMapEnable(void)
{
    HWREG(SYS_CTRL_I_MAP) |= SYS_CTRL_I_MAP_ALTMAP;
}

//*****************************************************************************
//
//! Disable the alternate interrupt mapping
//!
//! This function disables the alternate (that is, smaller) interrupt map.
//!
//! \sa See also IntAltMapDisable() and IntAltMapIsEnabled().
//!
//! \return None
//
//*****************************************************************************
void IntAltMapDisable(void)
{
    HWREG(SYS_CTRL_I_MAP) &= ~SYS_CTRL_I_MAP_ALTMAP;
}

//*****************************************************************************
//
//! Checks to see if the Alternate Interrupt Mapping is in use
//!
//! \sa See also IntAltMapDisable() and IntAltMapIsEnabled().
//!
//! \return Returns \b true if the Alternate Mapping is in use and \b false
//! otherwise.
//
//*****************************************************************************
bool IntAltMapIsEnabled(void)
{
    if(HWREG(SYS_CTRL_I_MAP) & SYS_CTRL_I_MAP_ALTMAP)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
