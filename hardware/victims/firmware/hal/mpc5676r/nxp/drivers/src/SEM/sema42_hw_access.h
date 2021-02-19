/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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

#ifndef SEMA42_HW_ACCESS_H
#define SEMA42_HW_ACCESS_H

/*! @file sema42_hw_access.h */
#include "sema42_driver.h"

/*!
 * sema42_hw_access Special hardware of C55 module
 * @details This section describes some access into register of hardware.
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

 #define SEMA42_UNLOCKED_GATE               ( 0U    )
 #define SEMA42_RSTGT_FIRST_WRITE_SEQUENCE  ( 0xE2U )
 #define SEMA42_RSTGT_SECOND_WRITE_SEQUENCE ( 0x1DU )
 
 #if defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE)
 #define SEMA42_RSTNTF_FIRST_WRITE_SEQUENCE  ( 0x47U )
 #define SEMA42_RSTNTF_SECOND_WRITE_SEQUENCE ( 0xB8U )
 #endif		/* defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE) */

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @name SEMA42 HARDWARE ACCESS API
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif


/*!
 * @brief Reset all the gate registers.
 *
 */
static inline void SEMA42_ResetAllGates(SEMA42_Type * const base)
{
    base->RSTGT.W = ((uint16_t) SEMA42_RSTGT_W_RSTGDP(SEMA42_RSTGT_FIRST_WRITE_SEQUENCE));
    base->RSTGT.W = (uint16_t) ((uint16_t) SEMA42_RSTGT_W_RSTGDP(SEMA42_RSTGT_SECOND_WRITE_SEQUENCE) |
                               ((uint16_t) SEMA42_RSTGT_W_RSTGTN(SEMA42_RESET_GATE_ALL)));
}

/*!
 * @brief Try to lock a gate.
 * return:
 *      STATUS_SUCCESS - if opperation succeded
 *      STATUS_ERROR - if opperation failed, gate is locked by some other core
 *
 */
static inline status_t SEMA42_LockGate(SEMA42_Type * const base, uint8_t const gateIndex, uint8_t const master_id)
{
    status_t return_value = STATUS_SUCCESS;
    if (base->GATE[gateIndex] == SEMA42_UNLOCKED_GATE)
    {
        base->GATE[gateIndex] = SEMA42_GATE_GTFSM(master_id + 1U);
    }
    else
    {
        if (base->GATE[gateIndex] != (master_id + 1U))
        {
            return_value = STATUS_ERROR;
        }
    }
    return return_value;
}

/*!
 * @brief Try to unlock a gate.
 * return:
 *      STATUS_SUCCESS - if opperation succeded
 *      STATUS_ERROR - if opperation failed, gate is locked by some other core
 *
 */
static inline status_t SEMA42_UnlockGate(SEMA42_Type * const base, uint8_t const gateIndex, uint8_t const master_id)
{
    status_t return_value = STATUS_SUCCESS;
    if (base->GATE[gateIndex] == SEMA42_UNLOCKED_GATE)
    {
        return_value = STATUS_SUCCESS;
    }
    else
    {
        if (base->GATE[gateIndex] == (master_id + 1U))
        {
            base->GATE[gateIndex] = ((uint8_t) SEMA42_GATE_GTFSM(SEMA42_UNLOCKED_GATE));
        }
        else
        {
            return_value = STATUS_ERROR;
        }
    }
    return return_value;
}

/*!
 * @brief Get gate locker.
 * return:
 *      Number of the core that locked the gate
 *
 */
static inline uint8_t SEMA42_GetGateLocker(const SEMA42_Type * const base, uint8_t const gateIndex)
{
    return (uint8_t)(base->GATE[gateIndex]);
}

/*!
 * @brief Reset the specified gate.
 *
 */

static inline void SEMA42_ResetGate(SEMA42_Type * const base, uint8_t const gateIndex)
{
    base->RSTGT.W = ((uint16_t) SEMA42_RSTGT_W_RSTGDP(SEMA42_RSTGT_FIRST_WRITE_SEQUENCE));
    base->RSTGT.W = (uint16_t)((uint16_t)SEMA42_RSTGT_W_RSTGDP(SEMA42_RSTGT_SECOND_WRITE_SEQUENCE) |
                              ((uint16_t)SEMA42_RSTGT_W_RSTGTN(gateIndex)));
}

/*!
 * @brief Reads the RSTGSM bitfield from the RSTGT register.
 * return:
 *      true    - if state is IDLE
 *      false   - for any other state
 *
 */
static inline bool SEMA42_IsResetGateStateIdle(const SEMA42_Type * const base)
{
    return (((base->RSTGT.R & SEMA42_RSTGT_R_RSTGSM_MASK) == 0U) ? true : false);
}

/*!
 * @brief Reads the Gate Master number from the RSTGT register
 *
 */
static inline uint8_t SEMA42_GetResetGateBusMaster(const SEMA42_Type * const base)
{
    return ((uint8_t)((base->RSTGT.R & ((uint16_t)SEMA42_RSTGT_R_RSTGMS_MASK)) >> SEMA42_RSTGT_R_RSTGMS_SHIFT));
}

/*!
 * @brief Reads the latest gate intended to be reset from the RSTGT register
 *
 */
static inline uint8_t SEMA42_GetResetGateIndex(const SEMA42_Type * const base)
{
    return ((uint8_t)((base->RSTGT.R & ((uint16_t)SEMA42_RSTGT_R_RSTGTN_MASK)) >> SEMA42_RSTGT_R_RSTGTN_SHIFT));
}

#if defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE)

/*!
 * @brief Set an enable mask to CPINE register
 *
 */
static inline void SEMA42_EnableNotificationInterrupt(SEMA42_Type * const base, uint32_t coreIndex, uint32_t mask)
{
	 base->CPINE[coreIndex].CPINE |= (uint16_t)(((uint16_t)mask & (((uint16_t)SEMA42_CPINE_INE_MASK) >> SEMA42_CPINE_INE_SHIFT)));
}

/*!
 * @brief Set a disable mask to CPINE register
 *
 */
static inline void SEMA42_DisableNotificationInterrupt(SEMA42_Type * const base, uint32_t coreIndex, uint32_t mask)
{
	 base->CPINE[coreIndex].CPINE &= (uint16_t)(~((uint16_t)mask & (((uint16_t)SEMA42_CPINE_INE_MASK) >> SEMA42_CPINE_INE_SHIFT)));
}

/*!
 * @brief get notification interrupt request CPNTF register
 *
 */
static inline uint32_t SEMA42_GetNotificationInterrupt(const SEMA42_Type * const base, uint32_t coreIndex)
{
	return ((uint32_t)(((uint32_t)base->CPNTF[coreIndex].CPNTF & ((uint32_t)SEMA42_CPNTF_GNn_MASK)) >> SEMA42_CPNTF_GNn_SHIFT));
}

/*!
 * @brief reset selected notification interrupt request in RSTNTF register
 *
 */
static inline void SEMA42_ResetNotificationInterrupt(SEMA42_Type * const base, uint8_t gateIndex)
{
	base->RSTNTF = ((uint16_t) SEMA42_RSTNTF_RSTNSM_RSTNMS_RSTNDP(SEMA42_RSTNTF_FIRST_WRITE_SEQUENCE));
	base->RSTNTF = (uint16_t)((uint16_t)SEMA42_RSTNTF_RSTNSM_RSTNMS_RSTNDP(SEMA42_RSTNTF_SECOND_WRITE_SEQUENCE) |
	                              ((uint16_t)SEMA42_RSTNTF_RSTNTN(gateIndex)));
}

#endif		/* defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE) */

/*! @} */
#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* SEMA42_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
