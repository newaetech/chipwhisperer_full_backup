/*
 * Copyright (c) 2013 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
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
#if !defined(INTERRUPT_MANAGER_H)
#define INTERRUPT_MANAGER_H
#include "MPC5676R.h"
#include "MPC5676R_features.h"

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, A cast shall not be performed
 * between pointer to void and an arithmetic type.
 * The address of hardware modules is provided as integer so
 * it needs to be cast to pointer.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be performed
 * between a pointer to object and an integer type.
 * The address of hardware modules is provided as integer so
 * a conversion between a pointer and an integer has to be performed.
 */

 /*! @file interrupt_manager.h */

/*! @addtogroup interrupt_manager*/
/*! @{*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#if FEATURE_MSCM_HAS_CPU_INTERRUPT_ROUTER

/*! @brief The target for directed CPU interrupts */
typedef enum
{
    INTERRUPT_MANAGER_TARGET_SELF       = -2,
    INTERRUPT_MANAGER_TARGET_OTHERS     = -1,
    INTERRUPT_MANAGER_TARGET_NONE       =  0,
    INTERRUPT_MANAGER_TARGET_CP0        =  1,
    INTERRUPT_MANAGER_TARGET_CP1        =  2,
    INTERRUPT_MANAGER_TARGET_CP0_CP1    =  3
} interrupt_manager_cpu_targets_t;

#endif /* FEATURE_MSCM_HAS_CPU_INTERRUPT_ROUTER */

#if FEATURE_INTERRUPT_MULTICORE_SUPPORT

/*! @brief Interrupt enabled on core
 *  Implements : interrupt_core_enable_t_Class
 */
typedef enum
{
	INTERRUPT_CORE_DISABLED     = 0U,
    INTERRUPT_CORE_ENABLED      = 1U
} interrupt_core_enable_t;

#endif /* FEATURE_INTERRUPT_MULTICORE_SUPPORT */

/*! @brief Interrupt handler type */
typedef void (* isr_t)(void);

/*******************************************************************************
 * Default interrupt handler - implemented in startup.s
 ******************************************************************************/
/*! @brief Default ISR. */
void DefaultISR(void);

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*! @name Interrupt manager APIs*/
/*@{*/

/*!
 * @brief Installs an interrupt handler routine for a given IRQ number. 
 *
 * This function lets the application register/replace the interrupt
 * handler for a specified IRQ number. See a chip-specific reference
 * manual for details and the  startup_<SoC>.s file for each chip
 * family to find out the default interrupt handler for each device.
 *
 * @note This method is applicable only if interrupt vector is copied in RAM.
 *
 * @param irqNumber   IRQ number
 * @param newHandler  New interrupt handler routine address pointer
 * @param oldHandler  Pointer to a location to store current interrupt handler
 */
void INT_SYS_InstallHandler(IRQn_Type irqNumber,
                            const isr_t newHandler,
                            isr_t* const oldHandler);

/*!
 * @brief Enables an interrupt for a given IRQ number. 
 *
 * This function  enables the individual interrupt for a specified IRQ number.
 *
 * @param irqNumber IRQ number
 */
void INT_SYS_EnableIRQ(IRQn_Type irqNumber);

/*!
 * @brief Disables an interrupt for a given IRQ number. 
 *
 * This function disables the individual interrupt for a specified IRQ number.
 *
 * @param irqNumber IRQ number
 */
void INT_SYS_DisableIRQ(IRQn_Type irqNumber);

/*!
 * @brief Enables system interrupt.
 *
 * This function enables the global interrupt by calling the core API.
 *
 */
void INT_SYS_EnableIRQGlobal(void);

/*!
 * @brief Disable system interrupt. 
 *
 * This function disables the global interrupt by calling the core API.
 *
 */
void INT_SYS_DisableIRQGlobal(void);

/*! @brief  Set Interrupt Priority
 *
 *   The function sets the priority of an interrupt.
 *
 *   @param  irqNumber  Interrupt number.
 *   @param  priority  Priority to set.
 */
void INT_SYS_SetPriority(IRQn_Type irqNumber, uint8_t priority);

/*! @brief  Get Interrupt Priority
 *
 *   The function gets the priority of an interrupt.
 *
 *   @param  irqNumber  Interrupt number.
 *   @return priority   Priority of the interrupt.
 */
uint8_t INT_SYS_GetPriority(IRQn_Type irqNumber);

#if FEATURE_INTERRUPT_HAS_PENDING_STATE
/*!
 * @brief Clear Pending Interrupt
 *
 * The function clears the pending bit of a peripheral interrupt
 * or a directed interrupt to this CPU (if available).
 *
 * @param irqNumber IRQ number
 */
void INT_SYS_ClearPending(IRQn_Type irqNumber);

/*!
 * @brief Set Pending Interrupt
 *
 * The function configures the pending bit of a peripheral interrupt.
 *
 * @param irqNumber IRQ number
 */
void INT_SYS_SetPending(IRQn_Type irqNumber);

/*!
 * @brief Get Pending Interrupt
 *
 * The function gets the pending bit of a peripheral interrupt
 * or a directed interrupt to this CPU (if available).
 *
 * @param irqNumber IRQ number
 * @return pending  Pending status 0/1
 */
uint32_t INT_SYS_GetPending(IRQn_Type irqNumber);

#endif /* FEATURE_INTERRUPT_HAS_PENDING_STATE */

#if FEATURE_INTERRUPT_HAS_ACTIVE_STATE
/*!
 * @brief Get Active Interrupt
 *
 * The function gets the active state of a peripheral interrupt.
 *
 * @param irqNumber IRQ number
 * @return active   Active status 0/1
 */
uint32_t INT_SYS_GetActive(IRQn_Type irqNumber);

#endif /* FEATURE_INTERRUPT_HAS_ACTIVE_STATE */

#if FEATURE_INTERRUPT_HAS_SOFTWARE_IRQ

/*!
 * @brief Set software interrupt request
 *
 * The function sets a software settable interrupt request.
 *
 * @param irqNumber IRQ number
 */
void INT_SYS_SetSoftwareIRQRequest(IRQn_Type irqNumber);

/*!
 * @brief Clear software interrupt request
 *
 * The function clears a software settable interrupt request.
 *
 * @param irqNumber IRQ number
 */
void INT_SYS_ClearSoftwareIRQRequest(IRQn_Type irqNumber);

#endif /* FEATURE_INTERRUPT_HAS_SOFTWARE_IRQ */


#if FEATURE_INTERRUPT_MULTICORE_SUPPORT

/*!
 * @brief Enables an interrupt for a given IRQ number, on the given cores.
 *
 * This function enables the individual interrupt for a specified IRQ number,
 * and on the specified cores.
 *
 * @param irqNumber IRQ number
 * @param coresIds array with the cores ids for which to enable the interrupt
 * @param coresCnt the number of cores in the coresIds array
 */
void INT_SYS_EnableIRQ_MC(IRQn_Type irqNumber, const uint8_t *coresIds, uint8_t coresCnt);

/*!
 * @brief Gets the cores on which an interrupt for the specified IRQ
 * number is enabled.
 *
 * This function will populate an array with all the supported cores,
 * for which the value will be INTERRUPT_CORE_ENABLED if interrupt is enabled
 * on that core, and INTERRUPT_CORE_DISABLED if interrupt is not enabled on
 * that core. The array has to be previously allocated using the
 * NUMBER_OF_CORES define value.
 *
 * @param irqNumber IRQ number
 * @param cores array with array index as core number; it has to be previously
 * allocated using the NUMBER_OF_CORES define value. it will be populated with the
 * following value:
 *  - INTERRUPT_CORE_DISABLED interrupt disabled for that core
 * 	- INTERRUPT_CORE_ENABLED enabled for that core
 */
void INT_SYS_GetCoresForIRQ(IRQn_Type irqNumber, interrupt_core_enable_t *cores);

/*!
 * @brief Disables an interrupt for a given IRQ number, on the given cores.
 *
 * This function disables the individual interrupt for a specified IRQ number,
 * and on the specified cores.
 *
 * @param irqNumber IRQ number
 * @param coresIds array with the cores ids for which to enable the interrupt
 * @param coresCnt the number of cores in the coresIds array
 */
void INT_SYS_DisableIRQ_MC(IRQn_Type irqNumber, const uint8_t *coresIds, uint8_t coresCnt);

/*!
 * @brief Disables an interrupt for a given IRQ number, on all cores.
 *
 * This function disables the individual interrupt for a specified IRQ number,
 * on all cores. It also clears priority for that IRQ number
 *
 * @param irqNumber IRQ number
 */
void INT_SYS_DisableIRQ_MC_All(IRQn_Type irqNumber);

#endif /* FEATURE_INTERRUPT_MULTICORE_SUPPORT */


#if FEATURE_MSCM_HAS_CPU_INTERRUPT_ROUTER

/*!
 * @brief Generate Directed CPU Interrupt
 *
 * The function generates a directed interrupt to (one or more) CPUs defined by target.
 *
 * @param irqNumber  IRQ number
 * @param cpu_target Target CPUs for the directed interrupt
 */
void INT_SYS_GenerateDirectedCpuInterrupt(IRQn_Type irqNumber, interrupt_manager_cpu_targets_t cpu_target);

#endif /* FEATURE_MSCM_HAS_CPU_INTERRUPT_ROUTER */

/*@}*/

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* INTERRUPT_MANAGER_H */


/*******************************************************************************
 * EOF
 ******************************************************************************/