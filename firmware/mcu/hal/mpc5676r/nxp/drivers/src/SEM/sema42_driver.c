/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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

#include "sema42_hw_access.h"
#include "sema42_driver.h"
/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * The function is defined to be used by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, Conversion between a pointer and
 * integer type.
 * The cast is required to initialize a pointer with an unsigned long define,
 * representing an address.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, Cast from unsigned int to pointer.
 * The cast is required to initialize a pointer with an unsigned long define,
 * representing an address.
 */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Table of base addresses for SEMA42 instances. */
static SEMA42_Type * const s_sema42Base[SEMA42_INSTANCE_COUNT] = SEMA42_BASE_PTRS;

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_Init
 * Description   : Initialize the SEMA42 module.
 * It resets all gate registers.
 * The caller must ensure that there is no overlap between any reset operations
 * executing from different cores - otherwise the reset may not succeed.
 *
 * Implements : SEMA42_DRV_Init_Activity
 *END*************************************************************************/
void SEMA42_DRV_Init(const uint32_t instance)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);

    SEMA42_Type * base = s_sema42Base[instance];
    /* Reset all gates */
    SEMA42_ResetAllGates(base);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_LockGate
 * Description   : Attempt to lock a selected gate and return the status of the operation.
 * This function attempts to lock a selected gate and checks if the operations was successful.
 *
 * Implements : SEMA42_DRV_LockGate_Activity
 *END*************************************************************************/
status_t SEMA42_DRV_LockGate(const uint32_t instance, const uint8_t gateIndex)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(gateIndex < SEMA42_GATE_COUNT);

    SEMA42_Type * base = s_sema42Base[instance];

    /* Initialize array to store number of master_id */
    uint8_t arr[NUMBER_OF_CORES] = FEATURE_SEMA42_MASTER;
    /* Get number of master_id */
    const uint8_t master_id = arr[GET_CORE_ID()];
    /* Check if gateIndex need to be inverted last bits */
    uint8_t invGateIndex = gateIndex;
#if defined (FEATURE_SEMA42_LAST_BITS_INVERTED)
    invGateIndex = FEATURE_SEMA42_CLEAR_LAST_BITS(gateIndex)|FEATURE_SEMA42_INVERT_LAST_BITS(gateIndex);
#endif /* defined (FEATURE_SEMA42_LAST_BITS_INVERTED) */
    /* Lock gate selected */
    status_t status = SEMA42_LockGate(base, invGateIndex, master_id);

    return status;
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_UnlockGate
 * Description   : Unlock a selected gate.
 * This function is used to unlock a selected gate. This operation is possible only
 * if the selected gate has been locked by the same processor that tries to unlock
 * the gate.
 *
 * Implements : SEMA42_DRV_UnlockGate_Activity
 *END*************************************************************************/
status_t SEMA42_DRV_UnlockGate(const uint32_t instance, const uint8_t gateIndex)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(gateIndex < SEMA42_GATE_COUNT);

    SEMA42_Type * base = s_sema42Base[instance];

    /* Initialize array to store number of master_id */
    uint8_t arr[NUMBER_OF_CORES] = FEATURE_SEMA42_MASTER;
    /* Get number of master_id */
    const uint8_t master_id = arr[GET_CORE_ID()];
    /* Check if gateIndex need to be inverted last bits */
    uint8_t invGateIndex = gateIndex;
#if defined (FEATURE_SEMA42_LAST_BITS_INVERTED)
    invGateIndex = FEATURE_SEMA42_CLEAR_LAST_BITS(gateIndex)|FEATURE_SEMA42_INVERT_LAST_BITS(gateIndex);
#endif /* defined (FEATURE_SEMA42_LAST_BITS_INVERTED) */
    /* Unlock gate selected */
    status_t status = SEMA42_UnlockGate(base, invGateIndex, master_id);

    return status;
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_GetGateLocker_Init
 * Description   : Get the index of the core locking the selected gate.
 * This function returns the index of the core which locked the selected gate
 * or a special value if the gate is not locked.
 *
 * Implements : SEMA42_DRV_GetGateLocker_Activity
 *END*************************************************************************/
uint8_t SEMA42_DRV_GetGateLocker(const uint32_t instance, const uint8_t gateIndex)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(gateIndex < SEMA42_GATE_COUNT);

    const SEMA42_Type * base = s_sema42Base[instance];
    /* Check if gateIndex need to be inverted last bits */
    uint8_t invGateIndex = gateIndex;
#if defined (FEATURE_SEMA42_LAST_BITS_INVERTED)
    invGateIndex = FEATURE_SEMA42_CLEAR_LAST_BITS(gateIndex)|FEATURE_SEMA42_INVERT_LAST_BITS(gateIndex);
#endif /* defined (FEATURE_SEMA42_LAST_BITS_INVERTED) */
    /* Get gate locked */
    return SEMA42_GetGateLocker(base, invGateIndex);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_ResetGate
 * Description   : Reset a selected gate register.
 * This function resets a selected gate register.
 * The caller must ensure that there is no overlap between any reset operations
 * executing from different cores - otherwise the reset may not succeed.
 *
 * Implements : SEMA42_DRV_ResetGate_Activity
 *END*************************************************************************/
void SEMA42_DRV_ResetGate(const uint32_t instance, const uint8_t gateIndex)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(gateIndex < SEMA42_GATE_COUNT);

    SEMA42_Type * base = s_sema42Base[instance];
    /* Reset gate index */
    SEMA42_ResetGate(base, gateIndex);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_IsResetGateStateIdle
 * Description   : Check if the current state of the reset gate state machine is idle.
 * This function retrieves the current state of the reset gate state machine.
 *
 * Implements : SEMA42_DRV_IsResetGateStateIdle_Activity
 *END*************************************************************************/
bool SEMA42_DRV_IsResetGateStateIdle(const uint32_t instance)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);

    const SEMA42_Type * base = s_sema42Base[instance];
    /* Check Reset gate state idle */
    return SEMA42_IsResetGateStateIdle(base);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_GetResetGateBusMaster
 * Description   : Get the bus master which initiated the most recent write
 * operation on the reset register.
 * This function returns the bus master index which initiated the most recent
 * write operation on the reset register.
 * A write operation on the reset register is equivalent with a reset attempt.
 *
 * Implements : SEMA42_DRV_GetResetGateBusMaster_Activity
 *END*************************************************************************/
uint8_t SEMA42_DRV_GetResetGateBusMaster(const uint32_t instance)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);

    const SEMA42_Type * base = s_sema42Base[instance];
    /* Get reset gate bus master */
    return SEMA42_GetResetGateBusMaster(base);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_GetResetGateIndex
 * Description   : Get the index of the gate targetted by the most recent reset attempt.
 * This function returns the index of the gate targetted by the most recent reset attempt
 * or a special value if all gates have been targetted.
 *
 * Implements : SEMA42_DRV_GetResetGateIndex_Activity
 *END*************************************************************************/
uint8_t SEMA42_DRV_GetResetGateIndex(const uint32_t instance)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);

    const SEMA42_Type * base = s_sema42Base[instance];
    /* Get reset gate index */
    return SEMA42_GetResetGateIndex(base);
}

#if defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE)

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_EnableNotificationInterrupt
 * Description   : Enable notification interrupts of selected core
 * This function use a mask to determine gates of which the notification interrupts will be enabled for the selected core.
 *
 * Implements : SEMA42_DRV_EnableNotificationInterrupt_Activity
 *END*************************************************************************/
void SEMA42_DRV_EnableNotificationInterrupt(const uint32_t instance, const uint32_t coreIndex, const uint32_t mask)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(coreIndex < NUMBER_OF_CORES);
    SEMA42_Type * base = s_sema42Base[instance];
    /* Swap mask because the CPINE bit enable for gate 0 is bit 15 */
    uint32_t swappedMask = 0U;
#if defined (FEATURE_SEMA42_INTERRUPT_ENABLE_SWAPPED)
    uint32_t index = 0U;
    for(index = 0U; index < SEMA42_CPINE_INE_WIDTH; index ++)
    {
        if((mask&((uint32_t)1U<<index)) > 0U)
        {
            swappedMask |= FEATURE_SEMA42_SWAP_MASK(index);
        }
        else
        {
            swappedMask &= FEATURE_SEMA42_INV_SWAP_MASK(index);
        }
    }
#else
    swappedMask = mask;
#endif
    /* Enable selected notification interrupts */
    SEMA42_EnableNotificationInterrupt(base,coreIndex,swappedMask);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_DisableNotificationInterrupt
 * Description   : Disable notification interrupts of selected core
 * This function use a mask to determine gates of which the notification interrupts will be disabled for the selected core.
 *
 * Implements : SEMA42_DRV_DisableNotificationInterrupt_Activity
 *END*************************************************************************/
void SEMA42_DRV_DisableNotificationInterrupt(const uint32_t instance, const uint32_t coreIndex, const uint32_t mask)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(coreIndex < NUMBER_OF_CORES);

    SEMA42_Type * base = s_sema42Base[instance];
    /* Swap mask because the CPINE bit enable for gate 0 is bit 15 */
    uint32_t swappedMask = 0U;
#if defined (FEATURE_SEMA42_INTERRUPT_ENABLE_SWAPPED)
    uint32_t index = 0U;
    for(index = 0U; index < SEMA42_CPINE_INE_WIDTH; index ++)
    {
        if((mask&((uint32_t)1U<<index)) > 0U)
        {
            swappedMask |= FEATURE_SEMA42_SWAP_MASK(index);
        }
        else
        {
            swappedMask &= FEATURE_SEMA42_INV_SWAP_MASK(index);
        }
    }
#else
    swappedMask = mask;
#endif
    /* Disable selected notification interrupts */
    SEMA42_DisableNotificationInterrupt(base,coreIndex,swappedMask);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_GetNotificationInterrupt
 * Description   : Get notification interrupts request of failed attempt to lock gates.
 * This function return a mask of the interrupt request notification from failed attempt to lock gates.
 *
 * Implements : SEMA42_DRV_GetNotificationInterrupt_Activity
 *END*************************************************************************/
uint32_t SEMA42_DRV_GetNotificationInterrupt(const uint32_t instance, const uint32_t coreIndex)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(coreIndex < NUMBER_OF_CORES);

    const SEMA42_Type * base = s_sema42Base[instance];
    /* Enable selected notification interrupts */
    return SEMA42_GetNotificationInterrupt(base,coreIndex);
}

/*FUNCTION*********************************************************************
 *
 * Function Name : SEMA42_DRV_ResetNotificationInterrupt
 * Description   : Reset selected notification interrupt request.
 * This function try to reset the selected notification interrupt.
 *
 * Implements : SEMA42_DRV_ResetNotificationInterrupt_Activity
 *END*************************************************************************/
void SEMA42_DRV_ResetNotificationInterrupt(const uint32_t instance, const uint8_t gateIndex)
{
    //DEV_ASSERT(instance < SEMA42_INSTANCE_COUNT);
    //DEV_ASSERT(gateIndex < SEMA42_GATE_COUNT);

    SEMA42_Type * base = s_sema42Base[instance];
    /* Enable selected notification interrupts */
    SEMA42_ResetNotificationInterrupt(base,gateIndex);
}

#endif      /* defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE) */

/*******************************************************************************
 * EOF
 ******************************************************************************/
