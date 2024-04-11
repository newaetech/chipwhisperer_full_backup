/*
 * Copyright 2018-2019 NXP
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
#if !defined(ESCI_HW_ACCESS_H)
#define ESCI_HW_ACCESS_H

#include "../../inc/esci_driver.h"

/*!
 * @file esci_hw_access.h
 *
 * @page misra_violations MISRA-C:2012 violations
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*! @brief Resets all hardware registers to their reset values.
 *
 *   The method resets all hardware registers to their known
 *   reset values.
 *
 *   @param  instance           Instance number.
 */
void ESCI_HW_ClearRegisters(const uint32_t instance);

/*! @brief Configures receiver related registers.
 *
 *   The method configures the receiver of the peripheral
 *   with settings for receive input mode, pin polarity
 *   and wake-up condition.
 *
 *   @param  instance           Instance number.
 *   @param  esciUserConfig     User configuration structure.
 *   @return status_t           The status of the operation.
 */
status_t ESCI_HW_ConfigureReceiver(const uint32_t instance,
                                   const esci_user_config_t * esciUserConfig);

/*! @brief Configures the frame format.
 *
 *   The method configures the frame format related registers
 *   based on the user configuration.
 *
 *   @param  instance           Instance number.
 *   @param  frameFormat        User configured frame format.
 *   @return status_t           The status of the operation.
 */
status_t ESCI_HW_SetFrameFormat(const uint32_t instance,
                                const esci_frame_format_t * frameFormat);

/*! @brief Configures the clock divider.
 *
 *   The method configures the clock divider of the peripheral
 *   based on input clock frequency and target baud rate.
 *   The actual configured baud rate will not be equal
 *   to the target value.
 *
 *   @param  instance           Instance number.
 *   @param  baudRate           User configured baud rate.
 *   @param  clockFrequency     Configured clock frequency.
 */
void ESCI_HW_SetClockDiv(const uint32_t instance,
                             const uint32_t baudRate,
                             const uint32_t clockFrequency);

/*! @brief Returns the configured baud rate.
 *
 *   The method calculates and returns the configured baud rate value.
 *
 *   @param  instance           Instance number.
 *   @param  baudRate           Calculated baud rate.
 *   @param  clockFrequency     Configured clock frequency.
 */
void ESCI_HW_GetBaudRate(const uint32_t instance,
                             uint32_t * baudRate,
                             uint32_t clockFrequency);

/*! @brief Enables the transmitter.
 *
 *   The method enables the transmitter including the related interrupts,
 *   DMA functionality and flags.
 *
 *   @param  instance        Instance number.
 *   @param  dmaEnable       Specify whether the transmitter will use DMA or interrupts.
 */
void ESCI_HW_TxEnable(const uint32_t instNum,
                      const bool dmaEnable);

/*! @brief Disables the transmitter.
 *
 *   The method disables the transmitter including the related interrupts,
 *   DMA functionality and flags.
 *
 *   @param  instance        Instance number.
 */
void ESCI_HW_TxDisable(const uint32_t instNum);

/*! @brief Enables the receiver.
 *
 *   The method enables the receiver including the related interrupts,
 *   DMA functionality and flags.
 *
 *   @param  instance        Instance number.
 *   @param  dmaEnable       Specify whether the receiver will use DMA or interrupts.
 */
void ESCI_HW_RxEnable(const uint32_t instNum,
                      const bool dmaEnable);

/*! @brief Disables the receiver.
 *
 *   The method disables the receiver including the related interrupts,
 *   DMA functionality and flags.
 *
 *   @param  instance        Instance number.
 */
void ESCI_HW_RxDisable(const uint32_t instNum);

/*! @brief Retrieves the address of the LSB of the data register.
 *
 *   The method retrives the address of the least significant byte
 *   of the data register which is used for DMA configuration.
 *
 *   @param  instance        Instance number.
 *   @return uint32_t        Address of the LSB of the data register.
 */
uint32_t ESCI_HW_GetDataRegLSBAddr(const uint32_t instNum);

/*! @brief Checks if the RDRF flag is set.
 *
 *   The method checks whether the Receive Data Register Full flag is set.
 *
 *   @param  instance        Instance number.
 *   @return bool            Whether the RDRF flag is set.
 */
bool ESCI_HW_GetReceiveReadyFlag(const uint32_t instance);

/*! @brief Clears the RDRF flag.
 *
 *   The method clears the Receive Data Register Full flag.
 *
 *   @param  instance        Instance number.
 */
void ESCI_HW_ClearReceiveReadyFlag(const uint32_t instance);

/*! @brief Checks if the TDRE or TC flags have been set.
 *
 *   The method checks whether the Transmit Data Register Empty or
 *   Transmit Complete flags have been set.
 *
 *   @param  instance        Instance number.
 *   @return bool            Whether the TDRE or TC flags have been set.
 */
bool ESCI_HW_GetSendReadyFlag(const uint32_t instance);

/*! @brief Clears the TDRE and TC flags.
 *
 *   The method clears the Transmit Data Register Empty
 *   and Transmit Complete flags.
 *
 *   @param  instance        Instance number.
 */
void ESCI_HW_ClearSendReadyFlag(const uint32_t instance);

/*! @brief Sends a character.
 *
 *   The method sends a character of up to 9 bits.
 *
 *   @param  instance        Instance number.
 *   @param  data            Character to be send.
 */
void ESCI_HW_SendCharacter(const uint32_t instance, uint16_t data);

/*! @brief Receive a character.
 *
 *   The method receives a character of up to 12 bits.
 *
 *   @param  instance          Instance number.
 *   @param  bitCountPerChar   Number of bits expected to be received.
 *   @return uint16_t          Received character.
 */
uint16_t ESCI_HW_ReceiveCharacter(const uint32_t instance,
                                  esci_character_bits_t bitCountPerChar);

#if defined(__cplusplus)
}
#endif /* __cplusplus*/


#endif /* ESCI_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
