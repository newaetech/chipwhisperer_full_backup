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
#if !defined(ESCI_DRIVER_H)
#define ESCI_DRIVER_H
/*!
 * @file esci_driver.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be performed
 * between a pointer to object and an integer type.
 * The address of hardware modules is provided as integer, therefore
 * a conversion between a pointer and an integer has to be performed.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, A cast shall not be performed
 * between pointer to void and an arithmetic type.
 * The address of hardware modules is provided as integer, therefore
 * a cast to pointer is necessary to be performed.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "MPC5676R.h"
#include "MPC5676R_features.h"
#include "status.h"
#include "uart_esci_callback.h"
#include "osif.h"

/*!
 * @addtogroup esci_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Operational mode of the peripheral.
 *  Implements : esci_functional_mode_t_Class
 */
typedef enum
{
    ESCI_MODE_SCI = 0U,
    ESCI_MODE_LIN = 1U
} esci_functional_mode_t;

/*! @brief Number of character bits present in frame payload.
 *  Implements : esci_character_bits_t_Class
 */
typedef enum
{
    ESCI_CHARACTER_BITS_7 = 0U,
    ESCI_CHARACTER_BITS_8 = 1U,
    ESCI_CHARACTER_BITS_9 = 2U,
    ESCI_CHARACTER_BITS_12 = 3U
} esci_character_bits_t;

/*! @brief Structure containing the frame format settings.
 *  Implements : esci_frame_format_t_Class
 */
typedef struct
{
    esci_character_bits_t characterBits;
    bool addressBits;                       /*!< Control whether an address bit is used to identify the frame as an address character */
    bool extraStopBit;                      /*!< Control whether an additional stop bit is used at the end of the frame */
    bool parityBits;                        /*!< Control whether a parity bit will be added to the outcoming and incoming characters */
    bool oddParity;                         /*!< Control whether to use odd or even parity, when parity bits are enabled */
} esci_frame_format_t;

/*! @brief Receive input mode of the peripheral.
 *  Implements : esci_receive_input_mode_t_Class
 */
typedef enum
{
    ESCI_RECV_MODE_DUAL_WIRE = 0U,
    ESCI_RECV_MODE_SINGLE_WIRE = 1U,
    ESCI_RECV_MODE_LOOP = 2U
} esci_receive_input_mode_t;

/*! @brief Wake-up condition of the receiver.
 *  Implements : esci_wakeup_condition_t_Class
 */
typedef enum
{
    ESCI_WAKEUP_ADDRESS_MARK = 0U,
    ESCI_WAKEUP_IDLE_LINE = 1U
} esci_wakeup_condition_t;

/*! @brief Structure containing the general configuration of the peripheral.
 *  Implements : esci_user_config_t_Class
 */
typedef struct
{
    esci_functional_mode_t functionalMode;
    esci_receive_input_mode_t receiveInputMode;
    esci_frame_format_t frameFormat;
    uint32_t baudRate;                          /*!< Targeted baud rate to be used for transfers */
    bool txDmaTransfer;                         /*!< Control whether the DMA operated transmission is enabled */
    bool rxDmaTransfer;                         /*!< Control whether the DMA operated reception is enabled */
    esci_wakeup_condition_t rxWakeupCondition;
    bool rxdInvertedPolarity;                   /*!< Control whether the RXD Pin Polarity is inverted */
} esci_user_config_t;

/*! @brief Structure containing all peripheral configuration settings
 *  and private data used during the lifetime of the driver instance.
 */
typedef struct
{
    status_t txLastStatus;                  /*!< Status of the send operation */
    status_t rxLastStatus;                  /*!< Status of the receive operation */
    bool txBlockingTransfer;                /*!< Whether the transmitter is doing a blocking send operation */
    bool rxBlockingTransfer;                /*!< Whether the receiver is doing a blocking receive operation */
    semaphore_t txComplete;                 /*!< Provides synchronization for the blocking send operation */
    semaphore_t rxComplete;                 /*!< Provides synchronization for the blocking receive operation */
    uart_callback_t txCallbackMethod;       /*!< Callback method to be invoked for a send operation */
    void * txCallbackParameter;             /*!< Callback parameter pointer for a send operation */
    uart_callback_t rxCallbackMethod;       /*!< Callback method to be invoked for a receive operation */
    void * rxCallbackParameter;             /*!< Callback parameter pointer for a receive operation */
    bool txDmaEnabled;                      /*!< Whether the DMA operated transmission is enabled */
    bool rxDmaEnabled;                      /*!< Whether the DMA operated reception is enabled */
    esci_character_bits_t bitCountPerChar;  /*!< Number of bits in a character */
    const uint8_t * sendBuffer;             /*!< Buffer of sent data */
    uint32_t sendBufferIndex;               /*!< Index of the next byte to be sent */
    uint32_t sendBufferSize;                /*!< Size of the send buffer */
    uint8_t * receiveBuffer;                /*!< Buffer of received data */
    uint32_t receiveBufferIndex;            /*!< Index where the next received byte will be saved */
    uint32_t receiveBufferSize;             /*!< Size of the receive buffer */
} esci_state_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus*/

/*! @name ESCI Driver APIs*/
/*@{*/


/*! @brief  Initialize the ESCI peripheral.
 *
 *   The method initalizes the ESCI peripheral.
 *
 *   @param  instNum         Instance number.
 *   @param  esciUserConfig  User configuration structure.
 *   @param  esciState       Pointer to the driver state structure.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_Init(const uint32_t instNum,
                       const esci_user_config_t * esciUserConfig,
                       esci_state_t * esciState);

/*! @brief  De-initialize the ESCI peripheral.
 *
 *   The method de-initializes the ESCI peripheral.
 *
 *   @param  instNum         Instance number.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_Deinit(const uint32_t instance);

/*! @brief  Returns a default configuration for the ESCI peripheral.
 *
 *   The method provides a structure containing default configuration parameters
 *   for the ESCI peripheral.
 *
 *   @param  esciUserConfig  Pointer to the user configuration structure.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_GetDefaultConfig(esci_user_config_t * esciUserConfig);

/*! @brief  Configures the data buffer for the transmitter.
 *
 *   The method configures the data buffer for the transmitter.
 *   It can be called from the callback method to provide
 *   a different buffer for continuous transmission.
 *
 *   @param  instNum         Instance number.
 *   @param  data            Pointer to the new TX data buffer.
 *   @param  count           Size of the new TX data buffer.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_SetTxBuffer(const uint32_t instNum,
                              const uint8_t * data,
                              const uint32_t count);

/*! @brief  Configures the data buffer for the receiver.
 *
 *   The method configures the data buffer for the receiver.
 *   It can be called from the callback method to provide
 *   a different buffer for continuous reception.
 *
 *   @param  instNum         Instance number.
 *   @param  data            Pointer to the new RX data buffer.
 *   @param  count           Size of the new RX data buffer.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_SetRxBuffer(const uint32_t instNum,
                              uint8_t * data,
                              const uint32_t count);

/*! @brief  Configures a new baud rate value.
 *
 *   The method configures a new value for the baud rate used for transfers.
 *
 *   @param  instNum         Instance number.
 *   @param  baudRate        Baud rate value.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_SetBaudRate(const uint32_t instNum, uint32_t baudRate);

/*! @brief  Returns the currently configured baud rate value.
 *
 *   The method provides the currently configured baud rate value used for transfers.
 *
 *   @param  instNum         Instance number.
 *   @param  baudRate        Pointer to the variable to be filled with the baud rate value.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_GetBaudRate(const uint32_t instNum, uint32_t * baudRate);

/*! @brief  Starts a non-blocking send operation.
 *
 *   The method starts a non-blocking send operation using the provided data buffer.
 *
 *   @param  instNum         Instance number.
 *   @param  data            Pointer to the TX data buffer.
 *   @param  count           Size of the TX data buffer.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_Send(const uint32_t instNum,
                       const uint8_t * data,
                       const uint32_t count);

/*! @brief  Starts a blocking send operation.
 *
 *   The method starts a blocking send operation using the provided data buffer
 *   and timeout period. After the timeout period passes, the method will exit
 *   and return STATUS_TIMEOUT.
 *
 *   @param  instNum         Instance number.
 *   @param  data            Pointer to the TX data buffer.
 *   @param  count           Size of the TX data buffer.
 *   @param  timeout         Timeout period.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_SendBlocking(const uint32_t instNum,
                               const uint8_t * data,
                               const uint32_t count,
                               const uint32_t timeout);

/*! @brief  Retrieves the status of the current send operation.
 *
 *   The method retrieves the status of the current send operation
 *   and the number of remaining characters to be sent if there is
 *   an ongoing transfer.
 *
 *   @param  instance        Instance number.
 *   @param  countRemain     Number of characters left to send.
 *   @return status_t        The status of the current send operation.
 */
status_t ESCI_DRV_GetSendStatus(const uint32_t instance,
                                uint32_t * countRemain);

/*! @brief  Aborts the current send operation.
 *
 *   The method aborts the current send operation. It has no effect
 *   if there is no ongoing send operation. After a successful
 *   execution of the ESCI_DRV_AbortSend method, the driver
 *   shall be available for a new transfer.
 *
 *   @param  instNum         Instance number.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_AbortSend(const uint32_t instNum);

/*! @brief  Registers the TX callback function and its parameter.
 *
 *   The method registers the callback function and its parameter for a send operation.
 *
 *   @param  instNum         Instance number.
 *   @param  callback        Pointer to the TX callback function.
 *   @param  parameter       Pointer to the TX callback function's parameter.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_InstallTxCallback(const uint32_t instNum,
                                    const uart_callback_t callback,
                                    void * parameter);

/*! @brief  Registers the RX callback function and its parameter.
 *
 *   The method registers the callback function and its parameter for a receive operation.
 *
 *   @param  instNum         Instance number.
 *   @param  callback        Pointer to the RX callback function.
 *   @param  parameter       Pointer to the RX callback function's parameter.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_InstallRxCallback(const uint32_t instNum,
                                    const uart_callback_t callback,
                                    void * parameter);

/*! @brief  Starts a non-blocking receive operation.
 *
 *   The method starts a non-blocking receive operation using the provided data buffer.
 *
 *   @param  instNum         Instance number.
 *   @param  data            Pointer to the RX data buffer.
 *   @param  count           Size of the RX data buffer.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_Receive(const uint32_t instNum,
                          uint8_t * data,
                          const uint32_t count);

/*! @brief  Starts a blocking receive operation.
 *
 *   The method starts a blocking receive operation using the provided data buffer
 *   and timeout period. After the timeout passes, the method will exit
 *   and return STATUS_TIMEOUT.
 *
 *   @param  instNum         Instance number.
 *   @param  data            Pointer to the RX data buffer.
 *   @param  count           Size of the RX data buffer.
 *   @param  timeout         Timeout period.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_ReceiveBlocking(const uint32_t instNum,
                                  uint8_t * data,
                                  const uint32_t count,
                                  const uint32_t timeout);

/*! @brief  Retrieves the status of the current receive operation.
 *
 *   The method retrieves the status of the current receive operation
 *   and the number of remaining characters to be received if there is
 *   an ongoing transfer.
 *
 *   @param  instance        Instance number.
 *   @param  countRemain     Number of characters left to receive.
 *   @return status_t        The status of the current receive operation.
 */
status_t ESCI_DRV_GetReceiveStatus(const uint32_t instance,
                                   uint32_t * countRemain);

/*! @brief  Aborts the current receive operation.
 *
 *   The method aborts the current receive operation. It has no effect
 *   if there is no ongoing receive operation. After a successful
 *   execution of the ESCI_DRV_AbortReceive method, the driver
 *   shall be available for a new transfer.
 *
 *   @param  instNum         Instance number.
 *   @return status_t        The status of the operation.
 */
status_t ESCI_DRV_AbortReceive(const uint32_t instNum);

/*@}*/

#if defined(__cplusplus)
}
#endif /* __cplusplus*/

/*! @}*/

#endif /* ESCI_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
