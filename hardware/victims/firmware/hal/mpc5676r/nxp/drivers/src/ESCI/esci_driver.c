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

/*!
 * @file esci_driver.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3, Taking address of near auto variable.
 * The code is not dynamically linked. An absolute stack address is obtained
 * when taking the address of the near auto variable. A source of error in
 * writing dynamic code is that the stack segment may be different from the data
 * segment.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
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
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 15.5, Return statement before end of function.
 * The return statement before end of function is used for simpler code structure
 * and better readability.
 */

#include "esci_driver.h"
#include "esci_irq.h"
#include "esci_hw_access.h"
//#include "clock_manager.h"
#include "interrupt_manager.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Pointer to ESCI runtime state structures */
static esci_state_t * s_esciState[eSCI_INSTANCE_COUNT];

/* Array with ESCI interrupt enum numbers defined in header files */
static const IRQn_Type s_esciIrqId[eSCI_INSTANCE_COUNT] = FEATURE_ESCI_IRQS;

/* Table to save ESCI clock names as defined in clock manager */
static const clock_names_t s_esciClkNames[eSCI_INSTANCE_COUNT] = FEATURE_ESCI_CLOCK_NAMES;

/* Tables for DMA configuration */
static const uint8_t s_esciDmaTxChannels[FEATURE_ESCI_TX_DMA_CHANNEL_COUNT] = FEATURE_ESCI_TX_DMA_CHANNEL;
static const uint8_t s_esciDmaRxChannels[FEATURE_ESCI_RX_DMA_CHANNEL_COUNT] = FEATURE_ESCI_RX_DMA_CHANNEL;

/*******************************************************************************
 * Private API
 ******************************************************************************/
static void ESCI_TxDisable(const uint32_t instNum);
static void ESCI_RxDisable(const uint32_t instNum);
static void ESCI_ClearState(esci_state_t * esciState);
#ifdef DMA_ENABLE
static void ESCI_TxDmaCallback(void * parameter, edma_chn_status_t status);
static void ESCI_RxDmaCallback(void * parameter, edma_chn_status_t status);
#endif
static void ESCI_CompleteSendData(const uint32_t instance);
static void ESCI_CompleteReceiveData(const uint32_t instance);

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_Init
 * Description   : Initializes the ESCI peripheral
 * Implements    : ESCI_DRV_Init_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_Init(const uint32_t instNum,
                       const esci_user_config_t * esciUserConfig,
                       esci_state_t * esciState)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(esciUserConfig != NULL);
    //DEV_ASSERT(esciState != NULL);

#if (defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT))
    /* Check if the instance supports DMA transmission */
    //bool s_esciDmaSupportEnabled[eSCI_INSTANCE_COUNT] = FEATURE_ESCI_DMA_SUPPORT;
    //DEV_ASSERT(s_esciDmaSupportEnabled[instNum] || (!(esciUserConfig->rxDmaTransfer || esciUserConfig->txDmaTransfer)));
#endif /*(defined(DEV_ERROR_DETECT) || defined(CUSTOM_DEVASSERT)) */

    status_t ret = STATUS_SUCCESS;
    status_t rxStatSem = STATUS_SUCCESS;
    status_t txStatSem = STATUS_SUCCESS;



    /* Check whether the instance has been already initialized */
    if (s_esciState[instNum] != NULL)
    {
        return STATUS_ERROR;
    }

    /* Save the runtime structure pointer */
    s_esciState[instNum] = esciState;

    /* Make sure every register is set to its reset value */
    ESCI_HW_ClearRegisters(instNum);

    /* Make sure the state structure is cleared */
    ESCI_ClearState(s_esciState[instNum]);

    /* Set the baud rate */
    (void) ESCI_DRV_SetBaudRate(instNum, esciUserConfig->baudRate);

    /* Configure receiver related registers */
    ret = ESCI_HW_ConfigureReceiver(instNum, esciUserConfig);
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }

    /* Set the frame format */
    esciState->bitCountPerChar = esciUserConfig->frameFormat.characterBits;
    ret = ESCI_HW_SetFrameFormat(instNum, &(esciUserConfig->frameFormat));
    if (ret != STATUS_SUCCESS)
    {
        return ret;
    }

    /* Setup DMA transmission */
    esciState->txDmaEnabled = esciUserConfig->txDmaTransfer;

    /* Setup DMA reception */
    esciState->rxDmaEnabled = esciUserConfig->rxDmaTransfer;

    /* Create the synchronization objects */
    rxStatSem = OSIF_SemaCreate(&esciState->rxComplete, 0U);
    txStatSem = OSIF_SemaCreate(&esciState->txComplete, 0U);
    if ((rxStatSem == STATUS_ERROR) || (txStatSem == STATUS_ERROR))
    {
        return STATUS_ERROR;
    }

    esciState->txBlockingTransfer = false;
    esciState->rxBlockingTransfer = false;

    esciState->txLastStatus = STATUS_SUCCESS;
    esciState->rxLastStatus = STATUS_SUCCESS;

    /* Enable the interrupt for the current core only */
    INT_SYS_EnableIRQ(s_esciIrqId[instNum]);

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_Deinit
 * Description   : De-initializes the ESCI peripheral
 * Implements    : ESCI_DRV_Deinit_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_Deinit(const uint32_t instance)
{
    //DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instance] != NULL);

    const esci_state_t * esciState = s_esciState[instance];
    status_t ret = STATUS_SUCCESS;

    /* Abort any ongoing receive operation */
    if (esciState->rxLastStatus == STATUS_BUSY)
    {
        ret = ESCI_DRV_AbortReceive(instance);
    }

    /* Abort any ongoing send operation */
    if (esciState->txLastStatus == STATUS_BUSY)
    {
        ret = ESCI_DRV_AbortSend(instance);
    }

    /* Destroy the synchronization objects */
    (void)OSIF_SemaDestroy(&esciState->rxComplete);
    (void)OSIF_SemaDestroy(&esciState->txComplete);

    /* Make sure every register is set to its reset value */
    ESCI_HW_ClearRegisters(instance);

    /* Delete the state structure pointer */
    s_esciState[instance] = NULL;

    /* Disable the interrupt */
    INT_SYS_DisableIRQ(s_esciIrqId[instance]);

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_GetDefaultConfig
 * Description   : Retrieves a default configuration.
 * Implements    : ESCI_DRV_GetDefaultConfig_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_GetDefaultConfig(esci_user_config_t * esciUserConfig)
{
    //DEV_ASSERT(esciUserConfig != NULL);

    esciUserConfig->baudRate = 24000U;
    esciUserConfig->functionalMode = ESCI_MODE_SCI;
    esciUserConfig->receiveInputMode = ESCI_RECV_MODE_DUAL_WIRE;
    esciUserConfig->frameFormat.characterBits = ESCI_CHARACTER_BITS_8;
    esciUserConfig->frameFormat.addressBits = false;
    esciUserConfig->frameFormat.extraStopBit = false;
    esciUserConfig->frameFormat.parityBits = false;
    esciUserConfig->frameFormat.oddParity = false;
    esciUserConfig->txDmaTransfer = false;
    esciUserConfig->rxDmaTransfer = false;
    esciUserConfig->rxdInvertedPolarity = false;
    esciUserConfig->rxWakeupCondition = ESCI_WAKEUP_IDLE_LINE;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_SetBaudRate
 * Description   : Configures a new baud rate value
 * Implements    : ESCI_DRV_SetBaudRate_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_SetBaudRate(const uint32_t instNum, uint32_t baudRate)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);
    //DEV_ASSERT(baudRate != 0U);

    const esci_state_t * esciState = s_esciState[instNum];
    uint32_t clockFrequency = 8000000;
    status_t ret = STATUS_SUCCESS;

    if ((esciState->txLastStatus == STATUS_BUSY) || (esciState->rxLastStatus == STATUS_BUSY))
    {
        return STATUS_BUSY;
    }

    /* Retrieve the Module Clock Frequency */
    ret = STATUS_SUCCESS;
    
    //CLOCK_SYS_GetFreq(s_esciClkNames[instNum], &clockFrequency);
    if (ret != STATUS_SUCCESS)
    {
        return STATUS_ERROR;
    }

    ESCI_HW_SetClockDiv(instNum, baudRate, clockFrequency);

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_GetBaudRate
 * Description   : Retrieves the actual baud rate used by the peripheral
 * Implements    : ESCI_DRV_GetBaudRate_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_GetBaudRate(const uint32_t instNum, uint32_t * baudRate)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);
    //DEV_ASSERT(baudRate != NULL);

    uint32_t clockFrequency = 8000000;
    status_t ret = STATUS_SUCCESS;

    /* Retrieve the Module Clock Frequency */
    ret = STATUS_SUCCESS;
    //CLOCK_SYS_GetFreq(s_esciClkNames[instNum], &clockFrequency);
    if (ret != STATUS_SUCCESS)
    {
        return STATUS_ERROR;
    }

    ESCI_HW_GetBaudRate(instNum, baudRate, clockFrequency);

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_Send
 * Description   : Starts a non-blocking send operation.
 * Implements    : ESCI_DRV_Send_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_Send(const uint32_t instNum, const uint8_t * data, const uint32_t count)
{

#if 0 
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);
    //DEV_ASSERT(data != NULL);
    //DEV_ASSERT(count > 0U);

    esci_state_t * esciState = s_esciState[instNum];

    /* Check whether there is another send operation ongoing */
    if (esciState->txLastStatus == STATUS_BUSY)
    {
        return STATUS_BUSY;
    }

    /*  9bit character transfer uses 2 bytes per character, buffer size should be even
     * 12bit character transfer is not supported by the TX frame format */
    if (((esciState->bitCountPerChar == ESCI_CHARACTER_BITS_9) && ((count % 2U) == 1U)) ||
         (esciState->bitCountPerChar == ESCI_CHARACTER_BITS_12))
    {

        esciState->txLastStatus = STATUS_ERROR;
        return STATUS_ERROR;
    }

    /* Setup the send buffer */
    esciState->sendBuffer = data;
    esciState->sendBufferIndex = 0U;
    esciState->sendBufferSize = count;

    esciState->txLastStatus = STATUS_BUSY;
#ifdef DMA_ENABLE
    if (esciState->txDmaEnabled)
    {
        /* DMA needs a pointer to the low byte of the eSCI_SDR register
         * to trigger the internal operation */
        uint32_t pSDR = ESCI_HW_GetDataRegLSBAddr(instNum);

        /* Configure the transfer control descriptor for the previously allocated channel */
        (void)EDMA_DRV_ConfigMultiBlockTransfer(s_esciDmaTxChannels[instNum],
                                                EDMA_TRANSFER_MEM2PERIPH,
                                                (uint32_t)data, (uint32_t)pSDR,
                                                EDMA_TRANSFER_SIZE_1B,
                                                1U, count, true);

        /* Call driver function to end the transmission when the DMA transfer is done */
        (void)EDMA_DRV_InstallCallback(s_esciDmaTxChannels[instNum],
                                        (edma_callback_t)(ESCI_TxDmaCallback),
                                        (void*)(instNum));

        /* Start the DMA channel */
        (void)EDMA_DRV_StartChannel(s_esciDmaTxChannels[instNum]);
    }
#endif
    ESCI_HW_TxEnable(instNum, esciState->txDmaEnabled);
#endif 

        for( int i = 0; i< count; i++)
    {
        ESCI_HW_SendCharacter(instNum, (uint16_t)data[i] );
    }

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_SendBlocking
 * Description   : Starts a blocking send operation.
 * Implements    : ESCI_DRV_SendBlocking_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_SendBlocking(const uint32_t instNum, const uint8_t * data,
                               const uint32_t count, const uint32_t timeout)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);
    //DEV_ASSERT(data != NULL);
    //DEV_ASSERT(count > 0U);

    esci_state_t * esciState = s_esciState[instNum];
    status_t ret = esciState->txLastStatus;
    status_t osifStatus = STATUS_SUCCESS;

    /* Check if there is another on-going send operation */
    if (ret != STATUS_BUSY)
    {
        /* Change TX state to blocking (to be used right after TX starts) */
        esciState->txBlockingTransfer = true;
        ret = ESCI_DRV_Send(instNum, data, count);
    }

    if (ret == STATUS_SUCCESS)
    {
        /* Wait until the transmission is completed */
        osifStatus = STATUS_SUCCESS;
        OSIF_TimeDelay(1000);

        if (osifStatus == STATUS_TIMEOUT)
        {
            ret = ESCI_DRV_AbortSend(instNum);
            if (ret == STATUS_SUCCESS)
            {
                esciState->txLastStatus = STATUS_TIMEOUT;
            }
        }

        /* The blocking transfer has finished, regardless of its result */
        esciState->txBlockingTransfer = false;
        ret = esciState->txLastStatus;
    }

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_GetSendStatus
 * Description   : Retrieves the status of the current send operation
 *                 or the last completed operation if there is none ongoing.
 *                 The parameter countRemain will return the number of
 *                 characters left to transfer.
 * Implements    : ESCI_DRV_GetSendStatus_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_GetSendStatus(const uint32_t instance, uint32_t * countRemain)
{
    //DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instance] != NULL);
    //DEV_ASSERT(countRemain != NULL);

    const esci_state_t * esciState = s_esciState[instance];

    switch(esciState->txLastStatus)
    {
        case STATUS_BUSY:
            if (esciState->txDmaEnabled == false)
            {
                *countRemain = esciState->sendBufferSize - esciState->sendBufferIndex;
            }
            else
            {
#ifdef DMA_ENABLE
                /* In DMA-based communication, the remaining bytes are retrieved
                 * from the current DMA major loop count */
                *countRemain = EDMA_DRV_GetRemainingMajorIterationsCount(s_esciDmaTxChannels[instance]);
#endif 
            }
            break;
        case STATUS_SUCCESS:
            *countRemain = 0U;
            break;
        default:
            /* For every other statuses, the countRemain value remains unchanged */
            break;
    }

    return esciState->txLastStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_AbortSend
 * Description   : Cancels an ongoing non-blocking send operation.
 * Implements    : ESCI_DRV_AbortSend_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_AbortSend(const uint32_t instNum)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];
    status_t ret = STATUS_SUCCESS;

    /* Make sure a send operation is ongoing */
    if (esciState->txLastStatus == STATUS_BUSY)
    {
#ifdef DMA_ENABLE
        if (esciState->txDmaEnabled)
        {
            (void)EDMA_DRV_StopChannel(s_esciDmaTxChannels[instNum]);
        }
#endif
        esciState->txLastStatus = STATUS_UART_ABORTED;
        ESCI_TxDisable(instNum);
    }
    else
    {
        ret = STATUS_ERROR;
    }

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_Receive
 * Description   : Starts a non-blocking receive operation.
 * Implements    : ESCI_DRV_Receive_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_Receive(const uint32_t instNum, uint8_t * data,
                          const uint32_t count)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);
    //DEV_ASSERT(data != NULL);
    //DEV_ASSERT(count > 0U);

    esci_state_t * esciState = s_esciState[instNum];

    /* Check whether there is another receive operation ongoing */
    if (esciState->rxLastStatus == STATUS_BUSY)
    {
        return STATUS_BUSY;
    }

    /* 9bit and 12bit reception uses 2 bytes for each character,
     * so the buffer size should be even */
    if (((esciState->bitCountPerChar == ESCI_CHARACTER_BITS_9) ||
         (esciState->bitCountPerChar == ESCI_CHARACTER_BITS_12)) && ((count % 2U) == 1U))
    {
        esciState->rxLastStatus = STATUS_ERROR;
        return STATUS_ERROR;
    }

    /* Setup the receive buffer */
    esciState->receiveBuffer = data;
    esciState->receiveBufferIndex = 0U;
    esciState->receiveBufferSize = count;

    esciState->rxLastStatus = STATUS_BUSY;
#ifdef DMA_ENABLE
    if (esciState->rxDmaEnabled)
    {
        /* DMA needs a pointer to the low byte of the eSCI_SDR register
         * to trigger the internal operation */
        uint32_t pSDR = ESCI_HW_GetDataRegLSBAddr(instNum);

        /* Configure the transfer control descriptor for the previously allocated channel */
        (void)EDMA_DRV_ConfigMultiBlockTransfer(s_esciDmaRxChannels[instNum],
                                                EDMA_TRANSFER_PERIPH2MEM,
                                                (uint32_t)pSDR, (uint32_t)data,
                                                EDMA_TRANSFER_SIZE_1B,
                                                1U, count, true);

        /* Call driver function to end the transmission when the DMA transfer is done */
        (void)EDMA_DRV_InstallCallback(s_esciDmaRxChannels[instNum],
                                        (edma_callback_t)(ESCI_RxDmaCallback),
                                        (void*)(instNum));

        /* Start the DMA channel */
        (void)EDMA_DRV_StartChannel(s_esciDmaRxChannels[instNum]);
    }
#endif
    ESCI_HW_RxEnable(instNum, esciState->rxDmaEnabled);

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_ReceiveBlocking
 * Description   : Starts a blocking receive operation.
 * Implements    : ESCI_DRV_ReceiveBlocking_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_ReceiveBlocking(const uint32_t instNum, uint8_t * data,
                                  const uint32_t count, const uint32_t timeout)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);
    //DEV_ASSERT(data != NULL);
    //DEV_ASSERT(count > 0U);

    esci_state_t * esciState = s_esciState[instNum];
    status_t ret = esciState->rxLastStatus;
    status_t osifStatus = STATUS_SUCCESS;
#warning This code isn't tested at all here"
#if 0
    /* Check if there is another on-going receive operation */
    if (ret != STATUS_BUSY)
    {
        /* Change RX state to blocking (to be used right after RX starts) */
        esciState->rxBlockingTransfer = true;
        ret = ESCI_DRV_Receive(instNum, data, count);
    }

    if (ret == STATUS_SUCCESS)
    {
        /* Wait until the transmission is completed */
        osifStatus = OSIF_SemaWait(&esciState->rxComplete, timeout);

        if (osifStatus == STATUS_TIMEOUT)
        {
            ret = ESCI_DRV_AbortReceive(instNum);
            if (ret == STATUS_SUCCESS)
            {
                esciState->rxLastStatus = STATUS_TIMEOUT;
            }
        }

        /* The blocking transfer has finished, regardless of its result */
        esciState->rxBlockingTransfer = false;
        ret = esciState->rxLastStatus;
    }
#endif 
            for( int i = 0; i< count; i++)
    {
        (*data) = (uint8_t)ESCI_HW_ReceiveCharacter(instNum,ESCI_CHARACTER_BITS_8);
    }

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_GetReceiveStatus
 * Description   : Retrieves the status of the current receive operation
 *                 or the last completed operation if there is none ongoing.
 *                 The parameter countRemain will return the number of
 *                 characters left to receive.
 * Implements    : ESCI_DRV_GetReceiveStatus_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_GetReceiveStatus(const uint32_t instance, uint32_t * countRemain)
{
    //DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instance] != NULL);
    //DEV_ASSERT(countRemain != NULL);

    const esci_state_t * esciState = s_esciState[instance];

    switch(esciState->rxLastStatus)
    {
        case STATUS_BUSY:
            if (esciState->rxDmaEnabled == false)
            {
                *countRemain = esciState->receiveBufferSize - esciState->receiveBufferIndex;
            }
            else
            {
#ifdef DMA_ENABLE
                /* In DMA-based communication, the remaining bytes are retrieved
                 * from the current DMA major loop count */
                *countRemain = EDMA_DRV_GetRemainingMajorIterationsCount(s_esciDmaRxChannels[instance]);
#endif             
            }
            break;
        case STATUS_SUCCESS:
            *countRemain = 0U;
            break;
        default:
            /* For every other statuses, the countRemain value remains unchanged */
            break;
    }

    return esciState->rxLastStatus;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_AbortReceive
 * Description   : Cancels an ongoing non-blocking receive operation.
 * Implements    : ESCI_DRV_AbortReceive_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_AbortReceive(const uint32_t instNum)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];
    status_t ret = STATUS_SUCCESS;

    /* Make sure a receive operation is ongoing */
    if (esciState->rxLastStatus == STATUS_BUSY)
    {
        #if DMA_ENABdefLE
        if (esciState->rxDmaEnabled)
        {
            (void)EDMA_DRV_StopChannel(s_esciDmaRxChannels[instNum]);
        }
        #endif 
        esciState->rxLastStatus = STATUS_UART_ABORTED;
        ESCI_RxDisable(instNum);
    }
    else
    {
        ret = STATUS_ERROR;
    }

    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_InstallTxCallback
 * Description   : Installs a new TX callback method
 * Implements    : ESCI_DRV_InstallTxCallback_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_InstallTxCallback(const uint32_t instNum,
                                    const uart_callback_t callback,
                                    void * parameter)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];

    esciState->txCallbackMethod = callback;
    esciState->txCallbackParameter = parameter;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_InstallRxCallback
 * Description   : Installs a new RX callback method.
 * Implements    : ESCI_DRV_InstallRxCallback_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_InstallRxCallback(const uint32_t instNum,
                                    const uart_callback_t callback,
                                    void * parameter)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];

    esciState->rxCallbackMethod = callback;
    esciState->rxCallbackParameter = parameter;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_SetTxBuffer
 * Description   : Configures a new buffer for the transmitter.
 *                 Can be called from the callback method to provide
 *                 a different buffer for continuous transmission.
 * Implements    : ESCI_DRV_SetTxBuffer_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_SetTxBuffer(const uint32_t instNum, const uint8_t * data, const uint32_t count)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];

    /* Configure the new buffer */
    esciState->sendBuffer = data;
    esciState->sendBufferSize = count;
    esciState->sendBufferIndex = 0U;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_SetRxBuffer
 * Description   : Configures a new buffer for the receiver.
 *                 Can be called from the callback method to provide
 *                 a different buffer for continuous reception.
 * Implements    : ESCI_DRV_SetRxBuffer_Activity
 *
 *END**************************************************************************/
status_t ESCI_DRV_SetRxBuffer(const uint32_t instNum, uint8_t * data, const uint32_t count)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];

    /* Configure the new buffer */
    esciState->receiveBuffer = data;
    esciState->receiveBufferSize = count;
    esciState->receiveBufferIndex = 0U;

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_IRQ_Handler
 * Description   : General interrupt handler.
 *                 It is called by all installed interrupt handlers.
 *
 *END**************************************************************************/
void ESCI_IRQ_Handler(const uint32_t instNum)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];

    /* Check if a character has been received */
    if (ESCI_HW_GetReceiveReadyFlag(instNum))
    {
        /* Only process a received character if there is an operation ongoing */
        if (esciState->rxLastStatus == STATUS_BUSY)
        {
            uint8_t msb_data = 0U;

            /* Retrieve the data character */
            uint16_t data = ESCI_HW_ReceiveCharacter(instNum, esciState->bitCountPerChar);

            if ((esciState->bitCountPerChar == ESCI_CHARACTER_BITS_9) ||
                (esciState->bitCountPerChar == ESCI_CHARACTER_BITS_12))
            {
                /* Retrieve the MSB */
                msb_data = (uint8_t) (data >> 8U);

                /* Save the MSB in the buffer */
                esciState->receiveBuffer[esciState->receiveBufferIndex] = msb_data;
                ++esciState->receiveBufferIndex;
            }

            /* Save the received character (LSB) in the buffer */
            esciState->receiveBuffer[esciState->receiveBufferIndex] = (uint8_t) data;
            ++esciState->receiveBufferIndex;

            /* Check if the receive operation was completed and notify the user */
            if (esciState->receiveBufferIndex >= esciState->receiveBufferSize)
            {
                ESCI_CompleteReceiveData(instNum);
            }
        }
        /* Clear receive ready flag */
        ESCI_HW_ClearReceiveReadyFlag(instNum);
    }

    /* Check if a character can be sent */
    if (ESCI_HW_GetSendReadyFlag(instNum))
    {
        /* Clear send ready flag */
        ESCI_HW_ClearSendReadyFlag(instNum);

        /* Try to send a character if there is a send operation ongoing */
        if (esciState->txLastStatus == STATUS_BUSY)
        {
            uint16_t data = 0U;

            if (esciState->bitCountPerChar == ESCI_CHARACTER_BITS_9)
            {
                /* Retrieve the MSB of the next character that has to be send */
                data = (uint16_t)(((uint16_t)esciState->sendBuffer[esciState->sendBufferIndex]) << 8U);
                ++esciState->sendBufferIndex;
            }

            /* Retrieve the next character that has to be send */
            data |= (uint16_t)esciState->sendBuffer[esciState->sendBufferIndex];
            ++esciState->sendBufferIndex;

            if (esciState->bitCountPerChar == ESCI_CHARACTER_BITS_7)
            {
                /* Truncate the character to 7 bits */
                data &= (uint16_t) 0x7FU;
            }

            /* Send the character */
            ESCI_HW_SendCharacter(instNum, data);

            /* Check if the send operation was completed and notify the user */
            if (esciState->sendBufferIndex >= esciState->sendBufferSize)
            {
                ESCI_CompleteSendData(instNum);
            }
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_ClearState
 * Description   : Clears the state structure
 *
 *END**************************************************************************/
static void ESCI_ClearState(esci_state_t * esciState)
{
    uint32_t i;
    uint8_t *ptr = (uint8_t *)esciState;
    /* Clear the state structure */
    for (i = 0U; i < sizeof(esci_state_t); i++)
    {
        ptr[i] = (uint8_t)0U;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_TxDisable
 * Description   : Disables the ESCI transmitter
 *
 *END**************************************************************************/
static void ESCI_TxDisable(const uint32_t instNum)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];

    /* Erase the send buffer information */
    esciState->sendBuffer = NULL;
    esciState->sendBufferIndex = 0U;
    esciState->sendBufferSize = 0U;

    /* Disable transmitter hardware registers */
    ESCI_HW_TxDisable(instNum);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_RxDisable
 * Description   : Disables the ESCI receiver and receiver related interrupts
 *
 *END**************************************************************************/
static void ESCI_RxDisable(const uint32_t instNum)
{
    //DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instNum] != NULL);

    esci_state_t * esciState = s_esciState[instNum];

    /* Erase the receive buffer information */
    esciState->receiveBuffer = NULL;
    esciState->receiveBufferIndex = 0U;
    esciState->receiveBufferSize = 0U;

    /* Disable receiver hardware registers */
    ESCI_HW_RxDisable(instNum);
}

#ifdef DMA_ENABLE
/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_TxDmaCallback
 * Description   : Callback for the DMA-related events of the transmitter
 *
 *END**************************************************************************/
static void ESCI_TxDmaCallback(void * parameter, edma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter;
    esci_state_t * esciState = s_esciState[instance];

    /* Check if the DMA transfer completed with errors */
    if (status == EDMA_CHN_ERROR)
    {
        /* Abort the send operation */
        (void)ESCI_DRV_AbortSend(instance);

        /* Update the status */
        esciState->txLastStatus = STATUS_ERROR;

        /* Signal the synchronous completion object, skip timeout if DMA transfer encountered error */
        if (esciState->txBlockingTransfer)
        {
            (void)OSIF_SemaPost(&esciState->txComplete);
            esciState->txBlockingTransfer = false;
        }

        /* Notify the application that an error occurred */
        if (esciState->txCallbackMethod != NULL)
        {
            esciState->txCallbackMethod(esciState, UART_EVENT_ERROR, esciState->txCallbackParameter);
        }
    }
    else
    {
        ESCI_CompleteSendData(instance);
    }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_RxDmaCallback
 * Description   : Callback for the DMA-related events of the receiver
 *
 *END**************************************************************************/
static void ESCI_RxDmaCallback(void * parameter, edma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter;
    esci_state_t * esciState = s_esciState[instance];

    /* Check if the DMA transfer completed with errors */
    if (status == EDMA_CHN_ERROR)
    {
        /* Abort the receive operation */
        (void)ESCI_DRV_AbortReceive(instance);

        /* Update the status */
        esciState->rxLastStatus = STATUS_ERROR;

        /* Signal the synchronous completion object, skip timeout if DMA transfer encountered error */
        if (esciState->rxBlockingTransfer)
        {
            (void)OSIF_SemaPost(&esciState->rxComplete);
            esciState->rxBlockingTransfer = false;
        }

        /* Notify the application that an error occurred */
        if (esciState->rxCallbackMethod != NULL)
        {
            esciState->rxCallbackMethod(esciState, UART_EVENT_ERROR, esciState->rxCallbackParameter);
        }
    }
    else
    {
        ESCI_CompleteReceiveData(instance);
    }
}

#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_CompleteSendData
 * Description   : Approach the finish of the current send operation.
 *                 Run the user configured callback and continue the transfer
 *                 only if the user provided a new send buffer. Otherwise,
 *                 finish the transfer.
 *
 *END**************************************************************************/
static void ESCI_CompleteSendData(const uint32_t instance)
{
    //DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instance] != NULL);

    esci_state_t * esciState = s_esciState[instance];

    esciState->sendBuffer = NULL;
    esciState->sendBufferIndex = 0U;
    esciState->sendBufferSize = 0U;

    /* Notify the user about the send buffer empty event */
    if (esciState->txCallbackMethod != NULL)
    {
        esciState->txCallbackMethod(esciState, UART_EVENT_TX_EMPTY, esciState->txCallbackParameter);
    }

    /* Check whether the user provided a new buffer for continuous transmission */
    if (esciState->sendBuffer == NULL)
    {
        /* The user did not provide a new buffer, the send operation was completed */
        esciState->txLastStatus = STATUS_SUCCESS;

        /* Disable the transmitter and transmitter related interrupts */
        ESCI_TxDisable(instance);

        /* Signal the synchronous completion object */
        if (esciState->txBlockingTransfer)
        {
            (void)OSIF_SemaPost(&esciState->txComplete);
        }

        /* Notify the user about the send complete event */
        if (esciState->txCallbackMethod != NULL)
        {
            esciState->txCallbackMethod(esciState, UART_EVENT_END_TRANSFER, esciState->txCallbackParameter);
        }
    }
    else if (esciState->txDmaEnabled && ((instance < FEATURE_ESCI_TX_DMA_CHANNEL_COUNT)))
    {
        #if DMA_ENABdefLE
        /* Set the source address and the number of minor loops (bytes to be transfered) */
        EDMA_DRV_SetSrcAddr(s_esciDmaTxChannels[instance], (uint32_t)(esciState->sendBuffer));
        EDMA_DRV_SetMajorLoopIterationCount(s_esciDmaTxChannels[instance], esciState->sendBufferSize);

        /* Re-start the channel */
        (void)EDMA_DRV_StartChannel(s_esciDmaTxChannels[instance]);
        #endif
    }
    else
    {
        /* Final else branch needed for MISRA compliance */
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_CompleteReceiveData
 * Description   : Approach the finish of the current receive operation.
 *                 Run the user configured callback and continue the transfer
 *                 only if the user provided a new receive buffer. Otherwise,
 *                 finish the transfer.
 *
 *END**************************************************************************/
static void ESCI_CompleteReceiveData(const uint32_t instance)
{
    //DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);
    //DEV_ASSERT(s_esciState[instance] != NULL);

    esci_state_t * esciState = s_esciState[instance];

    esciState->receiveBuffer = NULL;
    esciState->receiveBufferIndex = 0U;
    esciState->receiveBufferSize = 0U;

    /* Notify the user about the receive buffer full event */
    if (esciState->rxCallbackMethod != NULL)
    {
        esciState->rxCallbackMethod(esciState, UART_EVENT_RX_FULL, esciState->rxCallbackParameter);
    }

    /* Check whether the user provided a new buffer for continuous reception */
    if (esciState->receiveBuffer == NULL)
    {
        /* The user did not provide a new buffer, the receive operation was completed */
        esciState->rxLastStatus = STATUS_SUCCESS;

        /* Disable the receiver and receiver related interrupts */
        ESCI_RxDisable(instance);

        /* Signal the synchronous completion object */
        if (esciState->rxBlockingTransfer)
        {
            (void)OSIF_SemaPost(&esciState->rxComplete);
        }

        /* Notify the user about the receive complete event */
        if (esciState->rxCallbackMethod != NULL)
        {
            esciState->rxCallbackMethod(esciState, UART_EVENT_END_TRANSFER, esciState->rxCallbackParameter);
        }
    }
    else if (esciState->rxDmaEnabled && ((instance < FEATURE_ESCI_RX_DMA_CHANNEL_COUNT)))
    {
    #if DMA_defENABLE
        /* Set the destination address and the number of minor loops (bytes to be transfered) */
        EDMA_DRV_SetDestAddr(s_esciDmaRxChannels[instance], (uint32_t)(esciState->receiveBuffer));
        EDMA_DRV_SetMajorLoopIterationCount(s_esciDmaRxChannels[instance], esciState->receiveBufferSize);

        /* Re-start the channel */
        (void)EDMA_DRV_StartChannel(s_esciDmaRxChannels[instance]);
    #endif
    }
    else
    {
        /* Final else branch needed for MISRA compliance */
    }
}

/******************************************************************************/
/* EOF */
/******************************************************************************/
