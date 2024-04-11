/*
 * Copyright 2017 NXP
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

#ifndef SEMA42_DRIVER_H
#define SEMA42_DRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include "status.h"
#include "../../device/MPC5676R.h"
#include "../../device/MPC5676R_features.h"

/* */
/* */
/* */

/*! @file */

/*!
 * @addtogroup sema42_driver Semaphores Driver
 * @ingroup sema42
 * @brief Semaphores Peripheral Driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SEMA42_RESET_GATE_ALL (SEMA42_RSTGT_W_RSTGTN_MASK)

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Reset all gate registers.
 *
 * This function resets all gate registers.
 * The caller must ensure that there is no overlap between any reset operations
 * executing from different cores - otherwise the reset may not succeed.
 *
 * @param[in] instance - instance number
 */
void SEMA42_DRV_Init(const uint32_t instance);

/*!
 * @brief Attempt to lock a selected gate and return the status of the operation.
 *
 * This function attempts to lock a selected gate and checks if the operations was successful.
 *
 * @param[in] instance - instance number
 * @param[in] gateIndex - index of selected gate
 * @return operation status
 *      - STATUS_SUCCESS: gate locked successfully
 *      - STATUS_ERROR: could not lock the gate (possible reset overlap with other core)
 */
status_t SEMA42_DRV_LockGate(const uint32_t instance, const uint8_t gateIndex);

/*!
 * @brief Unlock a selected gate.
 *
 * This function unlocks a selected gate.
 *
 * @param[in] instance - instance number
 * @param[in] gateIndex - index of selected gate
 */
status_t SEMA42_DRV_UnlockGate(const uint32_t instance, const uint8_t gateIndex);

/*!
 * @brief Get the index of the core locking the selected gate.
 *
 * This function returns the index of the core which locked the selected gate
 * or a special value if the gate is not locked.
 *
 * @param[in] instance - instance number
 * @param[in] gateIndex - index of selected gate
 * @return
 *      - index of the core locking the gate
 *      - SEMA42_GATE_NOT_LOCKED if gate is not locked
 */
uint8_t SEMA42_DRV_GetGateLocker(const uint32_t instance, const uint8_t gateIndex);

/*!
 * @brief Reset a selected gate register.
 *
 * This function resets a selected gate register.
 * The caller must ensure that there is no overlap between any reset operations
 * executing from different cores - otherwise the reset may not succeed.
 *
 * @param[in] instance - instance number
 * @param[in] gateIndex - index of selected gate
 */
void SEMA42_DRV_ResetGate(const uint32_t instance, const uint8_t gateIndex);

/*!
 * @brief Check if the current state of the reset gate state machine is idle.
 *
 * This function retrieves the current state of the reset gate state machine.
 *
 * @param[in] instance - instance number
 * @return
 *      - 'true'    if reset state is 'idle'
 *      - 'false'   if reset state is 'waiting for second write'
 */
bool SEMA42_DRV_IsResetGateStateIdle(const uint32_t instance);

/*!
 * @brief Get the bus master which initiated the most recent write operation on the reset register.
 *
 * This function returns the bus master index which initiated the most recent write operation on the reset register.
 * A write operation on the reset register is equivalent with a reset attempt.
 *
 * @param[in] instance - instance number
 * @return bus master index
 */
uint8_t SEMA42_DRV_GetResetGateBusMaster(const uint32_t instance);

/*!
 * @brief Get the index of the gate targetted by the most recent reset attempt.
 *
 * This function returns the index of the gate targetted by the most recent reset attempt
 * or a special value if all gates have been targetted.
 *
 * @param[in] instance - instance number
 * @return
 *      - index of gate targetted for reset
 *      - SEMA42_RESET_GATE_ALL if all gates have been targetted for reset
 */
uint8_t SEMA42_DRV_GetResetGateIndex(const uint32_t instance);

#if defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE)

/*!
 * @brief Enable notification interrupts of selected core.
 *
 * This function enable notification interrupts, which determine by a mask, of the selected core.
 * This mask define the gates of which the notification interrupt will be enabled.
 *
 * @param[in] instance - instance number
 * @param[in] coreIndex - index of selected core
 * @param[in] mask - mask of selected notification interrupts
 */
void SEMA42_DRV_EnableNotificationInterrupt(const uint32_t instance, const uint32_t coreIndex, const uint32_t mask);

/*!
 * @brief Disable notification interrupts of selected core.
 *
 * This function disable notification interrupts, which determine by a mask, of the selected core.
 * This mask define the gates of which the notification interrupt will be disabled.
 *
 * @param[in] instance - instance number
 * @param[in] coreIndex - index of selected core
 * @param[in] mask - mask of selected notification interrupts
 */
void SEMA42_DRV_DisableNotificationInterrupt(const uint32_t instance, const uint32_t coreIndex, const uint32_t mask);

/*!
 * @brief Get the notification interrupt requests of selected core.
 *
 * This function return a mask of notification interrupt requests of the selected core.
 * This mask define the state of the notification interrupt requests of gates.
 * @param[in] instance - instance number
 * @param[in] coreIndex - index of selected core
 * @return
 *      - a mask of notification interrupt requests
 */
uint32_t SEMA42_DRV_GetNotificationInterrupt(const uint32_t instance, const uint32_t coreIndex);

/*!
 * @brief Reset the notification interrupt requests of the selected gate.
 *
 * This function reset the notification interrupt requests of the selected gate.
 * The selected gate can be 1 single gate if gateIndex is in range of 1u to 15u or all gate if gateIndex is 16u
 *
 * @param[in] instance - instance number
 * @param[in] gateIndex - index of selected gate
 */
void SEMA42_DRV_ResetNotificationInterrupt(const uint32_t instance, const uint8_t gateIndex);

#endif		/* defined (FEATURE_SEMA42_NOTIFICATION_INTERRUPT_AVAILABLE) */

#if defined(__cplusplus)
}
#endif

/*!
 *@}
 */

#endif /* SEMA42_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/

