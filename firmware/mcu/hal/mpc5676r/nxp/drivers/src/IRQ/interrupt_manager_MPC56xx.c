/*
 * Copyright (c) 2013 - 2016, Freescale Semiconductor, Inc.
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
 */

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.9, An object should be defined at block scope
 * if its identifier only appears in a single function.
 * __VECTOR_RAM variable is not an object with static storage duration, it needs to be
 * declared as extern.
 * @section [global]
 * Violates MISRA 2012 Required Rule 10.3, Expression assigned to a narrower or
 * different essential type.
 * This is required by the conversion of a bit-field of a register into a enum type.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 10.5, Impermissible cast; cannot cast from
 * 'essentially unsigned' type to 'essentially enum<i>'.
 * This is required by the conversion of a bit-field (uint16_t) of a register
 * into a enum type. Enum type is strictly defined as unsigned integer.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 10.8, Impermissible cast of composite expression
 * (different essential type categories).
 * This is required by the conversion of a bit/bitfield of a register into boolean or a enum type.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.1, Conversions shall not be performed
 * between a pointer to a function and any other type.
 * This is required in order to read/write from vector table memory.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be performed
 * between a pointer to object and an integer type.
 * The address of hardware modules is provided as integer so
 * a conversion between a pointer and an integer has to be performed.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, A cast shall not be performed
 * between pointer to void and an arithmetic type.
 * The address of hardware modules is provided as integer so
 * it needs to be cast to pointer.
 */

 /*! @file interrupt_manager.c */

#include <stddef.h>
#include "interrupt_manager.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * Number of cores present on the device
 */
#if defined(FEATURE_INTERRUPT_CORE_2_ENABLED)
#define INTERRUPT_NUM_OF_CORES (3u)
#elif defined(FEATURE_INTERRUPT_CORE_1_ENABLED)
#define INTERRUPT_NUM_OF_CORES (2u)
#elif defined(FEATURE_INTERRUPT_CORE_0_ENABLED)
#define INTERRUPT_NUM_OF_CORES (1u)
#else
#define INTERRUPT_NUM_OF_CORES (0u)
#endif

/*!
 * @brief Core masks
 */
#define INTERRUPT_CORE_ALL_MASK (0xFFFFu)
#define INTERRUPT_CORE_0_MASK   (0x8000u)
#define INTERRUPT_CORE_1_MASK   (0x4000u)
#define INTERRUPT_CORE_2_MASK   (0x2000u)

/*!
 * @brief Available CPU mask
 */
#define INTERRUPT_ALL_CPU_MASK ((FEATURE_INTERRUPT_CORE_0_ENABLED * INTERRUPT_CORE_0_MASK) \
                              + (FEATURE_INTERRUPT_CORE_1_ENABLED * INTERRUPT_CORE_1_MASK) \
                              + (FEATURE_INTERRUPT_CORE_2_ENABLED * INTERRUPT_CORE_2_MASK))

/*!
 * @brief Counter to manage the nested calling of global disable/enable interrupt.
 */
static volatile int32_t g_interruptDisableCount = 0;

#if (defined(MPC577x_SERIES) || defined(MPC5676R_SERIES))
/*!
 * @brief Interrupt priority shadow vector
 * FEATURE_INTERRUPT_IRQ_MAX is the highest interrupt request number.
 * Used only for platforms that do not support total interrupt masking on all cores
 */
static uint8_t g_interruptShadowPriority[((uint32_t)(FEATURE_INTERRUPT_IRQ_MAX) + 1u) / 2u];
#endif

#if defined(MPC5676R_SERIES)


/** All interrupt handlers should be of this type */
typedef void(*INTCInterruptFn)(void);
#define INTC_INTERRUPTS_REQUEST_VECTOR_TABLE_SIZE (511 * 4)

INTCInterruptFn INTCInterruptsHandlerTable_p0[INTC_INTERRUPTS_REQUEST_VECTOR_TABLE_SIZE];
#pragma pop

#pragma push             /* save the current state */
#pragma force_active on  /* make sure this function remains */
#pragma function_align 16 /* We use 16 bytes alignment for Exception handlers */
/** Handle the interrupt source by jumping to the ISR branch table (IACKR) */


#endif 
/*!
 * @brief Declaration of vector table.
 * FEATURE_INTERRUPT_IRQ_MAX is the highest interrupt request number.
 */
extern uint32_t __VECTOR_RAM[(uint32_t)(FEATURE_INTERRUPT_IRQ_MAX)];

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

#if (defined(MPC577x_SERIES) || defined(MPC5676R_SERIES))

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_GetShadowPriority
 * Description   : Get priority from shadow buffer
 *
 * irqNumber     : Interrupt index
 * return        : 4 bit interrupt priority
 *END**************************************************************************/
static inline uint8_t INT_GetShadowPriority(uint32_t irqNumber)
{
    uint8_t current = g_interruptShadowPriority[irqNumber >> 1u];

    /* Last bit based indexing */
    return ((irqNumber & 0x1u) != 0u) ? (current & 0x0Fu) : (current >> 4u);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SetShadowPriority
 * Description   : Set priority into the shadow buffer
 *
 * irqNumber     : Interrupt index
 * value         : 4 bit interrupt priority
 *END**************************************************************************/
static inline void INT_SetShadowPriority(uint32_t irqNumber, uint8_t value)
{
    uint8_t current = g_interruptShadowPriority[irqNumber >> 1u];

    /* Last bit based indexing */
    if ((irqNumber & 0x1u) != 0u)
    {
        current = (current & 0xF0u) | (value & 0x0Fu);
    }
    else
    {
        current = (current & 0x0Fu) | (value << 4u);
    }
    
    /* Write back */
    g_interruptShadowPriority[irqNumber >> 1u] = current;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_EnableIRQ
 * Description   : Enable interrupt by core mask
 *
 * irqNumber     : Interrupt index
 * coreMask      : Mask, where core 0 is 2^15, core 1 is 2^14 ... core 16 is 2^0
 *END**************************************************************************/

static inline void INT_EnableIRQ(uint32_t irqNumber, uint16_t coreMask)
{
    /* Mask any unavailable core */
    uint16_t lMask = coreMask & INTERRUPT_ALL_CPU_MASK;

    /* Only for a non-empty core mask */
    if (lMask != 0u)
    {
#if (defined(MPC577x_SERIES) || defined(MPC5676R_SERIES))
        /* Save the current vector priority data */
        uint8_t tPrio = INTC->PSR[irqNumber] & INTC_PSR_PRIN_MASK;

        /* Get the shadow priority */
        uint8_t sPrio = INT_GetShadowPriority(irqNumber);

        /* Assignment logic depends on the current plus what's in the shadow buffer */
        if (tPrio == 0u)
        {
            if (sPrio != 0u)
            {
                tPrio = sPrio;
            }
            else
            {
                tPrio = FEATURE_INTERRUPT_DEFAULT_PRIO;

                /* Update shadow priority */
                INT_SetShadowPriority(irqNumber, tPrio);
            }
        }

        /* Update the CPU assignment. */
        tPrio |= ((lMask >> 8u) ^ 0x80u);
#else
        /* Save the current vector priority data */
        uint16_t tPrio = INTC->PSR[irqNumber] & INTC_PSR_PRIN_MASK;

        /* 0 Means that the IRQ is disabled, and we need to assign it a default value */
        if (tPrio == 0u)
        {
            tPrio = FEATURE_INTERRUPT_DEFAULT_PRIO;
        }

        /* Update the CPU assignment. */
        tPrio |= lMask;
#endif
        /* Restore */
        INTC->PSR[irqNumber] = tPrio;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_DisableIRQ
 * Description   : Disable interrupt by core mask
 *
 * irqNumber     : Interrupt index
 * coreMask      : Mask, where core 0 is 2^15, core 1 is 2^14 ... core 16 is 2^0
 *END**************************************************************************/

static inline void INT_DisableIRQ(uint32_t irqNumber, uint16_t coreMask)
{
    /* Mask any unavailable core */
    uint16_t lMask = coreMask & INTERRUPT_ALL_CPU_MASK;

    /* Only for a non-empty core mask */
    if (lMask != 0u)
    {
#if defined(MPC577x_SERIES)
        /* Current state data */
        uint8_t tPsr = INTC->PSR[irqNumber];
        uint8_t tPrio = tPsr & INTC_PSR_PRIN_MASK;
        uint8_t tCores = tPsr & INTC_PSR_PRC_SELn_MASK;

        /* Update the CPU assignment. */
        switch (tCores)
        {
        /* Core 0 active */
        case INTC_PSR_PRC_SELn(0u):
            /* Reset the priority */
            if ((lMask & INTERRUPT_CORE_0_MASK) != 0u)
            {
                tPrio = 0u;
            }
            break;

        /* Both cores active */
        case INTC_PSR_PRC_SELn(1u):
            /* Reset the priority and assigned cores */
            if ((lMask & INTERRUPT_CORE_0_MASK) != 0u)
            {
                if ((lMask & INTERRUPT_CORE_1_MASK) != 0u)
                {
                    tPrio = 0u;
                }
                else
                {
                    tCores = INTC_PSR_PRC_SELn(3u);
                }     
            }
            else
            {
                tCores = 0u;
            }
            break;

        /* Invalid selection, should not get here */
        case INTC_PSR_PRC_SELn(2u):
            /* Reset the priority */
            if ((lMask & (INTERRUPT_CORE_0_MASK | INTERRUPT_CORE_1_MASK)) != 0u)
            {
                tPrio = 0u;
            }

            /* Update assigned cores */
            tCores = 0u;
            break;

        /* Core 1 active */
        case INTC_PSR_PRC_SELn(3u):
            /* Reset the priority */
            if ((lMask & INTERRUPT_CORE_1_MASK) != 0u)
            {
                tPrio = 0u;
            }
            break;

        default:
            /* Unreachable */
            break;
        }

        /* Form */
        tPsr = (tCores | tPrio);
#else
        /* Current state data */
        uint16_t tPsr = INTC->PSR[irqNumber];
        uint16_t tPrio = tPsr & INTC_PSR_PRIN_MASK;
        uint16_t tCores = tPsr & (INTERRUPT_ALL_CPU_MASK);

        /* Update the CPU assignment. */
        tPsr = (tCores & ~lMask) | tPrio;
#endif

        /* Restore */
        INTC->PSR[irqNumber] = tPsr;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_GetCores
 * Description   : Get a mask for enabled cores
 *
 * irqNumber     : Interrupt index
 * return        : Mask, where core 0 is 2^15, core 1 is 2^14 ... core 16 is 2^0
 *END**************************************************************************/

static inline uint16_t INT_GetCores(uint32_t irqNumber)
{
    uint16_t coreMask;

    /* Form the masks */
#if defined(MPC577x_SERIES)
    uint8_t tPsr = INTC->PSR[irqNumber];
    uint8_t tCores = tPsr & INTC_PSR_PRC_SELn_MASK;
    uint8_t tPrio = tPsr & INTC_PSR_PRIN_MASK;

    /* Compute the mask. We will return 0 in case of invalid priority */
    if (tPrio != 0u)
    {
        coreMask = (uint16_t)((uint16_t)tCores ^ (uint16_t)0x80u) << 8u;
    }
    else
    {
        coreMask = 0u;
    }
    
#else
    coreMask = INTC->PSR[irqNumber] & (INTERRUPT_ALL_CPU_MASK);
#endif

    return coreMask;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SetPriority
 * Description   : Sets the priority for a interrupt
 *
 * irqNumber     : Interrupt index
 * prioValue     : New priority
 *END**************************************************************************/

static inline void INT_SetPriority(uint32_t irqNumber, uint8_t prioValue)
{
#if defined(MPC577x_SERIES)
    /* Get current register image and data */
    uint8_t tPsr = INTC->PSR[irqNumber];
    uint8_t tPrio = tPsr & INTC_PSR_PRIN_MASK;
    uint8_t tCores = tPsr & INTC_PSR_PRC_SELn_MASK;

    /* Form */
    if (tPrio != 0u)
    {
        tPrio = prioValue & INTC_PSR_PRIN_MASK;
    }

    /* Update shadow priority */
    INT_SetShadowPriority(irqNumber, prioValue);
    
    /* Form */
    tPsr = tCores | tPrio;

#else
    /* Get current register image */
    uint16_t tPsr = INTC->PSR[irqNumber] & (INTERRUPT_ALL_CPU_MASK);

    /* Form */
    tPsr |= (uint16_t)(prioValue & INTC_PSR_PRIN_MASK);
#endif

    /* Write back */
    INTC->PSR[irqNumber] = tPsr;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_GetPriority
 * Description   : Returns the priority for a interrupt
 *
 * irqNumber     : Interrupt index
 * return        : Priority value
 *END**************************************************************************/

static inline uint8_t INT_GetPriority(uint32_t irqNumber)
{
#if defined(MPC577x_SERIES)
    /* Return from the shadow buffer */
    return INT_GetShadowPriority(irqNumber);
#else
    /* Direct on all other platforms */
    return INTC->PSR[irqNumber] & INTC_PSR_PRIN_MASK;
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SetSwIrq
 * Description   : Triggers a software IRQ
 *
 * irqNumber     : Interrupt index
 *END**************************************************************************/

static inline void INT_SetSwIrq(uint32_t irqNumber)
{
    /* Trigger the interrupt */
    INTC->SSCIR[irqNumber] = INTC_SSCIR_SET_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_ClearSwIrq
 * Description   : Clears a software IRQ
 *
 * irqNumber     : Interrupt index
 *END**************************************************************************/

static inline void INT_ClearSwIrq(uint32_t irqNumber)
{
    /* Clear the interrupt */
    INTC->SSCIR[irqNumber] = INTC_SSCIR_CLR_MASK;
}

/*******************************************************************************
 * Public functions
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_InstallHandler
 * Description   : Install an interrupt handler routine for a given IRQ number
 * This function will let application register/replace the interrupt
 * handler for specified IRQ number. Refer to startup_<CPU>.s
 * file for each chip family to find out the default interrupt handler for each device.
 *
 * Implements INT_SYS_InstallHandler_Activity
 *
 *END**************************************************************************/
void INT_SYS_InstallHandler(IRQn_Type irqNumber,
                            const isr_t newHandler,
                            isr_t* const oldHandler)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
    //DEV_ASSERT(newHandler != (isr_t)0);

#if defined(MPC5676R_SERIES)

    if (oldHandler != (isr_t *)0)
    {
        *oldHandler =  INTCInterruptsHandlerTable_p0[(int32_t)irqNumber];
    }

    /* Set the function pointer in the ISR Handler table */
    INTCInterruptsHandlerTable_p0[(int32_t)irqNumber] = (uint32_t)newHandler;
    /* Set the PSR Priority */
    INTC->PSR[(int32_t)irqNumber] = 1; 

#else
    /* Save the former handler pointer */
    if (oldHandler != (isr_t *)0)
    {
        *oldHandler = (isr_t)__VECTOR_RAM[((int32_t)irqNumber)];
    }

    /* Set handler into vector table */
    __VECTOR_RAM[(int32_t)irqNumber] = (uint32_t)newHandler;
#endif 
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_EnableIRQ
 * Description   : Enables an interrupt for a given IRQ number.
 * This function will enable an interrupt corresponding to the interrupt number
 * provided as parameter. Interrupt request will be sent to current core,
 * with priority defined in FEATURE_INTERRUPT_DEFAULT_PRIO, which is the
 * maximum priority value.
 *
 * Implements INT_SYS_EnableIRQ_Activity
 *END**************************************************************************/
void INT_SYS_EnableIRQ(IRQn_Type irqNumber)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);

    INT_EnableIRQ((uint32_t)irqNumber, (INTERRUPT_CORE_0_MASK >> GET_CORE_ID()));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_DisableIRQ
 * Description   : Disable individual interrupt for a specified IRQ
 * This function will disable an interrupt corresponding to the interrupt number
 * provided as parameter. It is considered interrupt request was sent to
 * current core.
 *
 * Implements INT_SYS_DisableIRQ_Activity
 *
 *END**************************************************************************/
void INT_SYS_DisableIRQ(IRQn_Type irqNumber)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);

    INT_DisableIRQ((uint32_t)irqNumber, (INTERRUPT_CORE_0_MASK >> GET_CORE_ID()));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_EnableIRQ_MC
 * Description   : Enables an interrupt for a given IRQ number and the given
 * cores.
 * This function will enable an interrupt corresponding to the interrupt number
 * provided as parameter. Interrupt request will be sent to the array of cores
 * sent as parameter. Interrupt will be enabled with priority defined in
 * FEATURE_INTERRUPT_DEFAULT_PRIO, which is the maximum priority value.
 *
 * Implements INT_SYS_EnableIRQ_MC_Activity
 *END**************************************************************************/
void INT_SYS_EnableIRQ_MC(IRQn_Type irqNumber, const uint8_t *coresIds, uint8_t coresCnt)
{
    /* Check for correct interrupt ID and parameters */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
    //DEV_ASSERT(coresIds != NULL);
    //DEV_ASSERT(coresCnt <= 16u);

    /* Local core mask */
    uint16_t tMask = 0u;

    /* Enable interrupt for each requested core */
    for (uint8_t i = 0u; i < coresCnt; i++)
    {
        tMask |= (INTERRUPT_CORE_0_MASK >> coresIds[i]);
    }

    INT_EnableIRQ((uint32_t)irqNumber, tMask);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_DisableIRQ_MC
 * Description   : Disable individual interrupt for a specified IRQ, on the given
 * cores.
 * This function will disable an interrupt corresponding to the interrupt number
 * provided as parameter. It will disable interrupt only on the cores given as
 * parameter.
 *
 * Implements INT_SYS_DisableIRQ_MC_Activity
 *
 *END**************************************************************************/
void INT_SYS_DisableIRQ_MC(IRQn_Type irqNumber, const uint8_t *coresIds, uint8_t coresCnt)
{
    /* Check for correct interrupt ID and parameters */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
    //DEV_ASSERT(coresIds != NULL);
    //DEV_ASSERT(coresCnt <= 16u);

    /* Local core mask */
    uint16_t tMask = 0u;

    /* Enable interrupt for each requested core */
    for (uint8_t i = 0u; i < coresCnt; i++)
    {
        tMask |= (INTERRUPT_CORE_0_MASK >> coresIds[i]);
    }

    INT_DisableIRQ((uint32_t)irqNumber, tMask);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_GetCoresForIRQ
 * Description   : Gets the cores on which an interrupt for the specified IRQ
 * number is enabled.
 * This function expects the cores parameter to be an array for which the
 * index represents the core ID. The array has to be previously allocated 
 * using the NUMBER_OF_CORES define value.
 * The function will populate this array as follows:
 * - INTERRUPT_CORE_ENABLED if interrupt is enabled on that core
 * - INTERRUPT_CORE_DISABLED if interrupt is not enabled on that core.
 *
 * Implements INT_SYS_GetCoresForIRQ_Activity
 *
 *END**************************************************************************/
void INT_SYS_GetCoresForIRQ(IRQn_Type irqNumber, interrupt_core_enable_t *cores)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
    //DEV_ASSERT(cores != (interrupt_core_enable_t *)0);

    uint16_t cMask = INT_GetCores((uint32_t)irqNumber);

    for (uint32_t cInd = 0u; cInd < INTERRUPT_NUM_OF_CORES; cInd++)
    {
        /* Only if bit in mask */
        if ((cMask & (uint16_t)INTERRUPT_CORE_0_MASK) != 0u)
        {
            cores[cInd] = INTERRUPT_CORE_ENABLED;
        }
        else
        {
            cores[cInd] = INTERRUPT_CORE_DISABLED;
        }
        
        /* Next entry */
        cMask <<= 1u;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_DisableIRQ_MC
 * Description   : Disable individual interrupt for a specified IRQ, on all cores.
 * This function will disable an interrupt corresponding to the interrupt number
 * provided as parameter. It clears the priority and then all the processors for
 * which it was enabled.
 *
 * Implements INT_SYS_DisableIRQ_MC_All_Activity
 *
 *END**************************************************************************/
void INT_SYS_DisableIRQ_MC_All(IRQn_Type irqNumber)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);

    INT_DisableIRQ((uint32_t)irqNumber, INTERRUPT_CORE_ALL_MASK);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_EnableIRQGlobal
 * Description   : Enable system interrupt
 * This function will enable the global interrupt by calling the core API
 *
 * Implements INT_SYS_EnableIRQGlobal_Activity
 *
 *END**************************************************************************/
void INT_SYS_EnableIRQGlobal(void)
{
    /* Check and update */
    if (g_interruptDisableCount > 0)
    {
        g_interruptDisableCount--;

        if (g_interruptDisableCount <= 0)
        {
            /* Enable the global interrupt*/
            ENABLE_INTERRUPTS();
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_DisableIRQGlobal
 * Description   : Disable system interrupt
 * This function will disable the global interrupt by calling the core API
 *
 * Implements INT_SYS_DisableIRQGlobal_Activity
 *
 *END**************************************************************************/
void INT_SYS_DisableIRQGlobal(void)
{
    /* Disable the global interrupt */
    DISABLE_INTERRUPTS();

    /* Update counter*/
    g_interruptDisableCount++;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_SetPriority
 * Description   : Set the priority of an interrupt
 * This function will set the priority of an interrupt.
 * Note that setting the priority to 0 will cause the
 * interrupt to no longer trigger even if it is enabled.
 *
 * Implements INT_SYS_SetPriority_Activity
 *
 *END**************************************************************************/
void INT_SYS_SetPriority(IRQn_Type irqNumber, uint8_t priority)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
    //DEV_ASSERT(priority <= INTC_PSR_PRIN_MASK);
    //DEV_ASSERT(priority > 0u);

    INT_SetPriority((uint32_t)irqNumber, priority);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_GetPriority
 * Description   : Get the priority of an interrupt
 * This function will get the priority of an interrupt.
 *
 * Implements INT_SYS_GetPriority_Activity
 *
 *END**************************************************************************/
uint8_t INT_SYS_GetPriority(IRQn_Type irqNumber)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_IRQ_MAX);
    
    return INT_GetPriority((uint32_t)irqNumber);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_SetSoftwareIRQRequest
 * Description   : Set a software settable interrupt request
 * This function will set a software settable interrupt request.
 *
 * Implements INT_SYS_SetSoftwareIRQRequest_Activity
 *
 *END**************************************************************************/
void INT_SYS_SetSoftwareIRQRequest(IRQn_Type irqNumber)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_SOFTWARE_IRQ_MAX);

    INT_SetSwIrq((uint32_t)irqNumber);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : INT_SYS_ClearSoftwareIRQRequest
 * Description   : Clear a software settable interrupt request
 * This function will clear a software settable interrupt request.
 *
 * Implements INT_SYS_ClearSoftwareIRQRequest_Activity
 *
 *END**************************************************************************/
void INT_SYS_ClearSoftwareIRQRequest(IRQn_Type irqNumber)
{
    /* Check for correct interrupt ID */
    //DEV_ASSERT((int32_t)irqNumber >= (int32_t)FEATURE_INTERRUPT_IRQ_MIN);
    //DEV_ASSERT((int32_t)irqNumber <= (int32_t)FEATURE_INTERRUPT_SOFTWARE_IRQ_MAX);

    INT_ClearSwIrq((uint32_t)irqNumber);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/