/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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

/*!
 * @file flexcan_driver.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 15.5, Return statement before end of function.
 * The return statement before end of function is used for simpler code structure
 * and better readability.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3, Taking address of near auto variable
 * The code is not dynamically linked. An absolute stack address is obtained when
 * taking the address of the near auto variable. A source of error in writing
 * dynamic code is that the stack segment may be different from the data segment.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, cast from unsigned char to pointer
 * The cast is needed for a function which has a generic parameter of type void*.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, conversion between a pointer and integer
 * type.
 * The cast is needed to obtain an address for a DMA call.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, cast from pointer to unsigned long
 * The cast is needed to obtain an address for a DMA call.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 2.2, Highest operation, function
 * 'FLEXCAN_UnlockRxMsgBuff', lacks side-effects.
 * The function is used to unlock the mailbox, which is done by reading the
 * free running timer.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.3, cast performed between a pointer to
 * object type and a pointer to a different object type
 * The cast is used for casting a bytes buffer into an words buffer in order to
 * optimize copying data to/from the message buffer.
 */


#include "flexcan_hw_access.h"
#include "flexcan_irq.h"

#include "interrupt_manager.h"
#if 1
#include "clock_manager.h"
#endif 

#include "MPC5676R.h"
#include "MPC5676R_features.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define FLEXCAN_MB_HANDLE_RXFIFO    0U

/* CAN bit timing values */
#define FLEXCAN_NUM_TQ_MIN     8U
#define FLEXCAN_NUM_TQ_MAX    26U
#define FLEXCAN_PRESDIV_MAX  256U
#define FLEXCAN_PSEG1_MAX      8U
#define FLEXCAN_PSEG2_MIN      1U
#define FLEXCAN_PSEG2_MAX      8U
#define FLEXCAN_PROPSEG_MAX    8U
#define FLEXCAN_TSEG1_MIN      2U
#define FLEXCAN_TSEG1_MAX     17U
#define FLEXCAN_TSEG2_MIN      2U
#define FLEXCAN_TSEG2_MAX      9U
#define FLEXCAN_RJW_MAX        3U
#define MB_OFFSET              4U

	struct canbuf {
            union {
                volatile uint32_t R;
                struct {
                    volatile uint32_t:4;
                    volatile uint32_t CODE:4;
                    volatile uint32_t:1;
                    volatile uint32_t SRR:1;
                    volatile uint32_t IDE:1;
                    volatile uint32_t RTR:1;
                    volatile uint32_t LENGTH:4;
                    volatile uint32_t TIMESTAMP:16;
                } B;
            } CS;
            
            union {
                volatile uint32_t R;
                struct {
                    volatile uint32_t PRIO:3;
                    volatile uint32_t STD_ID:11;
                    volatile uint32_t EXT_ID:18;
                } B;
            } ID;
            
            union {
                volatile uint8_t B[8];  /* Data buffer in Bytes (8 bits) */
                volatile uint16_t H[4]; /* Data buffer in Half-words (16 bits) */
                volatile uint32_t W[2]; /* Data buffer in words (32 bits) */
                volatile uint32_t R[2]; /* Data buffer in words (32 bits) */
            } DATA;
            
        };

    typedef struct canbuf canbuf_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Table of base addresses for CAN instances. */
static CAN_Type * const g_flexcanBase[] = CAN_BASE_PTRS;

/* Tables to save CAN IRQ enum numbers defined in CMSIS header file. */
#if FEATURE_CAN_HAS_WAKE_UP_IRQ
static const IRQn_Type g_flexcanWakeUpIrqId[] = CAN_Wake_Up_IRQS;
#endif
static const IRQn_Type g_flexcanErrorIrqId[] = CAN_Error_IRQS;
static const IRQn_Type g_flexcanBusOffIrqId[] = CAN_Bus_Off_IRQS;
static const IRQn_Type g_flexcanOredMessageBufferIrqId[FEATURE_CAN_MB_IRQS_MAX_COUNT][CAN_INSTANCE_COUNT]
    = FEATURE_CAN_MB_IRQS;
/* Pointer to runtime state structure.*/
static flexcan_state_t * g_flexcanStatePtr[CAN_INSTANCE_COUNT] = { NULL };

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static status_t FLEXCAN_StartSendData(
                    uint8_t instance,
                    uint8_t mb_idx,
                    const flexcan_data_info_t *tx_info,
                    uint32_t msg_id,
                    const uint8_t *mb_data,
                    bool isBlocking
                    );
static status_t FLEXCAN_StartRxMessageBufferData(
                    uint8_t instance,
                    uint8_t mb_idx,
                    flexcan_msgbuff_t *data,
                    bool isBlocking
                    );
static status_t FLEXCAN_StartRxMessageFifoData(
                    uint8_t instance,
                    flexcan_msgbuff_t *data,
                    bool isBlocking
                    );
static void FLEXCAN_CompleteTransfer(uint8_t instance, uint32_t mb_idx);
static void FLEXCAN_CompleteRxMessageFifoData(uint8_t instance);
#if FEATURE_CAN_HAS_DMA_ENABLE
static void FLEXCAN_CompleteRxFifoDataDMA(void *parameter,
                                          edma_chn_status_t status);
#endif
static void FLEXCAN_BitrateToTimeSeg(uint32_t bitrate,
                                     uint32_t clkFreq,
                                     flexcan_time_segment_t *timeSeg);
static inline void FLEXCAN_IRQHandlerRxFIFO(uint8_t instance, uint32_t mb_idx);
static void FLEXCAN_IRQHandlerRxMB(uint8_t instance, uint32_t mb_idx);
static inline void FLEXCAN_EnableIRQs(uint8_t instance);
#ifdef ERRATA_E10368
#if FEATURE_CAN_HAS_FD
static inline void FLEXCAN_Errata10368(uint8_t instance,
                                       const flexcan_user_config_t *data);
#endif /* FEATURE_CAN_HAS_FD */
#endif /* ERRATA_E10368 */
/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetBitrate
 * Description   : Set FlexCAN baudrate.
 * This function will set up all the time segment values for classical frames or the
 * extended time segments for the arbitration phase of FD frames. Those time segment
 * values are passed in by the user and are based on the required baudrate.
 *
 * Implements    : FLEXCAN_DRV_SetBitrate_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_SetBitrate(uint8_t instance, const flexcan_time_segment_t *bitrate)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(bitrate != NULL);

    CAN_Type * base = g_flexcanBase[instance];
#if FEATURE_CAN_HAS_FD
    bool fdEnabled = FLEXCAN_IsFDEnabled(base);
#endif

    FLEXCAN_EnterFreezeMode(base);

#if FEATURE_CAN_HAS_FD
    if (fdEnabled)
    {
        /* Set extended time segments*/
        FLEXCAN_SetExtendedTimeSegments(base, bitrate);
    }
    else
#endif
    {
        /* Set time segments*/
        FLEXCAN_SetTimeSegments(base, bitrate);
    }

    FLEXCAN_ExitFreezeMode(base);
}

#if FEATURE_CAN_HAS_FD
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetBitrateCbt
 * Description   : Set FlexCAN bitrate.
 * This function will set up all the time segment values for the data phase of
 * FD frames. Those time segment values are passed in by the user and are based
 * on the required baudrate.
 *
 * Implements    : FLEXCAN_DRV_SetBitrateCbt_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_SetBitrateCbt(uint8_t instance, const flexcan_time_segment_t *bitrate)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(bitrate != NULL);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    /* Set time segments*/
    FLEXCAN_SetFDTimeSegments(base, bitrate);

    FLEXCAN_ExitFreezeMode(base);
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetBitrate
 * Description   : Get FlexCAN baudrate.
 * This function will be return the current bit rate settings for classical frames
 * or the arbitration phase of FD frames.
 *
 * Implements    : FLEXCAN_DRV_GetBitrate_Activity
 *END**************************************************************************/
void  FLEXCAN_DRV_GetBitrate(uint8_t instance, flexcan_time_segment_t *bitrate)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(bitrate != NULL);

    const CAN_Type * base = g_flexcanBase[instance];

    /* Get the time segments*/
    FLEXCAN_GetTimeSegments(base, bitrate);

}

#if FEATURE_CAN_HAS_FD
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetBitrateFD
 * Description   : Get FlexCAN baudrate.
 * This function will be return the current bit rate settings for the data phase
 * of FD frames.
 *
 * Implements    : FLEXCAN_DRV_GetBitrateFD_Activity
 *END**************************************************************************/
void  FLEXCAN_DRV_GetBitrateFD(uint8_t instance, flexcan_time_segment_t *bitrate)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(bitrate != NULL);

    const CAN_Type * base = g_flexcanBase[instance];

    /* Get the time segments*/
    FLEXCAN_GetFDTimeSegments(base, bitrate);

}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetMasktype
 * Description   : Set RX masking type.
 * This function will set RX masking type as RX global mask or RX individual
 * mask.
 *
 * Implements    : FLEXCAN_DRV_SetRxMaskType_Activity
 *END**************************************************************************/
void  FLEXCAN_DRV_SetRxMaskType(uint8_t instance, flexcan_rx_mask_type_t type)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    FLEXCAN_SetRxMaskType(base, type);

    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetRxFifoGlobalMask
 * Description   : Set Rx FIFO global mask as the 11-bit standard mask or the
 * 29-bit extended mask.
 *
 * Implements    : FLEXCAN_DRV_SetRxFifoGlobalMask_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_SetRxFifoGlobalMask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    flexcan_rx_fifo_id_element_format_t formatType;
    CAN_Type * base = g_flexcanBase[instance];
    uint32_t calcMask = 0U;

    FLEXCAN_EnterFreezeMode(base);

    if (true == FLEXCAN_IsRxFifoEnabled(base))
    {
		formatType = FLEXCAN_GetRxFifoIdFormat(base);
		calcMask = FLEXCAN_GetRxFifoMask(id_type, formatType, mask);

		switch (formatType)
		{
			case FLEXCAN_RX_FIFO_ID_FORMAT_A :
				FLEXCAN_SetRxFifoGlobalMask(base, calcMask);
				break;
			case FLEXCAN_RX_FIFO_ID_FORMAT_B :
				FLEXCAN_SetRxFifoGlobalMask(base, (calcMask | (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_SHIFT1)));
				break;
			case FLEXCAN_RX_FIFO_ID_FORMAT_C :
				FLEXCAN_SetRxFifoGlobalMask(base, (calcMask | (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT1) |
												  	  	  	  (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT2) |
															  (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT3)));
				break;
			default :
				/* Will Enable all filter fields mask */
				FLEXCAN_SetRxFifoGlobalMask(base, 0xFFFFFFFFU);
				break;
		}
    }
    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetRxMbGlobalMask
 * Description   : Set Rx Message Buffer global mask as the 11-bit standard mask
 * or the 29-bit extended mask.
 *
 * Implements    : FLEXCAN_DRV_SetRxMbGlobalMask_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_SetRxMbGlobalMask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    if (id_type == FLEXCAN_MSG_ID_STD)
    {
        /* Set standard global mask for RX MB*/
        FLEXCAN_SetRxMsgBuffGlobalStdMask(base, mask);
    }
    else if (id_type == FLEXCAN_MSG_ID_EXT)
    {
        /* Set extended global mask for RX MB*/
        FLEXCAN_SetRxMsgBuffGlobalExtMask(base, mask);
    }
    else {
        /* Should not get here */
    }

    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetRxMb14Mask
 * Description   : Set Rx Message Buffer 14 mask as the 11-bit standard mask
 * or the 29-bit extended mask.
 *
 * Implements    : FLEXCAN_DRV_SetRxMb14Mask_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_SetRxMb14Mask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    if (id_type == FLEXCAN_MSG_ID_STD)
    {
        /* Set standard global mask for RX MB*/
        FLEXCAN_SetRxMsgBuff14StdMask(base, mask);
    }
    else if (id_type == FLEXCAN_MSG_ID_EXT)
    {
        /* Set extended global mask for RX MB*/
        FLEXCAN_SetRxMsgBuff14ExtMask(base, mask);
    }
    else {
        /* Should not get here */
    }

    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetRxMb15Mask
 * Description   : Set Rx Message Buffer 15 mask as the 11-bit standard mask
 * or the 29-bit extended mask.
 *
 * Implements    : FLEXCAN_DRV_SetRxMb15Mask_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_SetRxMb15Mask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint32_t mask)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    if (id_type == FLEXCAN_MSG_ID_STD)
    {
        /* Set standard global mask for RX MB*/
        FLEXCAN_SetRxMsgBuff15StdMask(base, mask);
    }
    else if (id_type == FLEXCAN_MSG_ID_EXT)
    {
        /* Set extended global mask for RX MB*/
        FLEXCAN_SetRxMsgBuff15ExtMask(base, mask);
    }
    else {
        /* Should not get here */
    }

    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetRxIndividualMask
 * Description   : Set Rx individual mask as the 11-bit standard mask or the
 * 29-bit extended mask.
 *
 * Implements    : FLEXCAN_DRV_SetRxIndividualMask_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_SetRxIndividualMask(
    uint8_t instance,
    flexcan_msgbuff_id_type_t id_type,
    uint8_t mb_idx,
    uint32_t mask)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    

    FLEXCAN_EnterFreezeMode(base);

    if ((mb_idx > FLEXCAN_GetMaxMsgBuffNum(base)) || (mb_idx >= CAN_RXIMR_COUNT))
    {
        FLEXCAN_ExitFreezeMode(base);
        return STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    if (false == FLEXCAN_IsRxFifoEnabled(base))
    {
		if (id_type == FLEXCAN_MSG_ID_STD)
		{
			/* Set standard individual mask*/
			FLEXCAN_SetRxIndividualStdMask(base, mb_idx, mask);
		}
		else if (id_type == FLEXCAN_MSG_ID_EXT)
		{
			/* Set extended individual mask*/
			FLEXCAN_SetRxIndividualExtMask(base, mb_idx, mask);
		}
		else
		{
			/* Should not get here */
		}
    }
    else
    {
		if (mb_idx <= FLEXCAN_GetNoOfIndividualMBsRxFIFO(base))
		{
			/* In FIFO Mode get the Id Filter Format already configured by FLEXCAN_DRV_ConfigRxFifo */
			flexcan_rx_fifo_id_element_format_t formatType = FLEXCAN_GetRxFifoIdFormat(base);
			uint32_t calcMask = FLEXCAN_GetRxFifoMask(id_type, formatType, mask);

			switch (formatType)
			{
				case FLEXCAN_RX_FIFO_ID_FORMAT_A :
					FLEXCAN_SetRxIndividualMask(base, mb_idx, calcMask);
					break;
				case FLEXCAN_RX_FIFO_ID_FORMAT_B :
					FLEXCAN_SetRxIndividualMask(base, mb_idx, (calcMask | (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_SHIFT1)));
					break;
				case FLEXCAN_RX_FIFO_ID_FORMAT_C :
					FLEXCAN_SetRxIndividualMask(base, mb_idx, (calcMask | (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT1) |
																  	  	  (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT2) |
																		  (calcMask >> FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT3)));
					break;
				default :
					/* Will Enable all filter fields mask */
					FLEXCAN_SetRxIndividualMask(base, mb_idx, 0xFFFFFFFFU);
					break;
			}
		}
    	else
    	{
    		if (id_type == FLEXCAN_MSG_ID_STD)
    		{
    			/* Set standard individual mask*/
    			FLEXCAN_SetRxIndividualStdMask(base, mb_idx, mask);
    		}
    		else if (id_type == FLEXCAN_MSG_ID_EXT)
    		{
    			/* Set extended individual mask*/
    			FLEXCAN_SetRxIndividualExtMask(base, mb_idx, mask);
    		}
    		else
    		{
    			/* Should not get here */
    		}
    	}
		
    }

    FLEXCAN_ExitFreezeMode(base);

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_EnableIRQs
 * Description   : This function enable FLEXCAN instance Interrupts
 *
 *END**************************************************************************/
static inline void FLEXCAN_EnableIRQs(uint8_t instance)
{
	uint8_t i;
#if FEATURE_CAN_HAS_WAKE_UP_IRQ
    if (g_flexcanWakeUpIrqId[instance] != NotAvail_IRQn)
    {
        INT_SYS_EnableIRQ(g_flexcanWakeUpIrqId[instance]);
    }
#endif
    INT_SYS_EnableIRQ(g_flexcanErrorIrqId[instance]);
    INT_SYS_EnableIRQ(g_flexcanBusOffIrqId[instance]);
    for (i = 0; i < FEATURE_CAN_MB_IRQS_MAX_COUNT; i++)
    {
        if (g_flexcanOredMessageBufferIrqId[i][instance] != NotAvail_IRQn)
        {
            INT_SYS_EnableIRQ(g_flexcanOredMessageBufferIrqId[i][instance]);
        }
    }
}

#ifdef ERRATA_E10368
#if FEATURE_CAN_HAS_FD
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_Errata10368
 * Description   : Implements errata 10368. This errata applies to mask 0N78S for MPC5748G CPU.
 *
 *END**************************************************************************/
static inline void FLEXCAN_Errata10368(uint8_t instance,
                                       const flexcan_user_config_t *data)
{
    CAN_Type * base = g_flexcanBase[instance];
    flexcan_time_segment_t bitrate;
    
	if (data->fd_enable)
	{
		flexcan_msgbuff_code_status_t messageBuff={
				.code = FLEXCAN_TX_REMOTE,
				.msgIdType = FLEXCAN_MSG_ID_STD,
				.dataLen = 1U,
				.fd_enable = false,
				.fd_padding = 0U,
				.enable_brs = false
		};
		uint8_t dummy_data[16];
		/* Enable Self Reception */
		FLEXCAN_SetSelfReception(base, true);

		/* Select Loopback mode */
		FLEXCAN_SetOperationMode(base, FLEXCAN_LOOPBACK_MODE);
		/* Dummy transmission on MB0 */
		(void)FLEXCAN_SetTxMsgBuff(base, 0U, &messageBuff, 0x1U, dummy_data, false);
		FLEXCAN_SetFDEnabled(base, true);
		bitrate = data->bitrate;
		FLEXCAN_SetExtendedTimeSegments(base, &bitrate);
		bitrate = data->bitrate_cbt;
		FLEXCAN_SetFDTimeSegments(base, &bitrate);
		FLEXCAN_ExitFreezeMode(base);
		/* Wait transmission of MB0 to complete */
		while((base->IFLAG1 & 0x1U) != 0x1U) {};
		/* Clear Transmission Complete */
		base->IFLAG1 = base->IFLAG1 | 0x1U;
		FLEXCAN_EnterFreezeMode(base);
	}    
}
#endif /* FEATURE_CAN_HAS_FD */
#endif /* ERRATA_E10368 */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_Init
 * Description   : Initialize FlexCAN driver.
 * This function will select a source clock, reset FlexCAN module, set maximum
 * number of message buffers, initialize all message buffers as inactive, enable
 * RX FIFO if needed, mask all mask bits, disable all MB interrupts, enable
 * FlexCAN normal mode, and enable all the error interrupts if needed.
 *
 * Implements    : FLEXCAN_DRV_Init_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_Init(

   uint8_t instance,
   flexcan_state_t *state,
   const flexcan_user_config_t *data)
{
#if 0 
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(state != NULL);
    //DEV_ASSERT(g_flexcanStatePtr[instance] == NULL);
#ifdef ERRATA_E10595
    if (instance != 0U)
    {
    	//DEV_ASSERT((MC_ME->GS & MC_ME_GS_S_FXOSC_MASK) != 0U);
    }
#endif

    status_t result;
    CAN_Type * base = g_flexcanBase[instance];
    flexcan_time_segment_t bitrate;
    status_t osifStat;
    uint32_t i, j;
    uint32_t mask = 0;
    if(FLEXCAN_IsEnabled(base))
    {
       /*Enable Freezemode to set clock*/
 //       
        FLEXCAN_Disable(base);

    }

    /* Enable the CAN clock */
    FLEXCAN_Enable(base);
    FLEXCAN_EnterFreezeMode(base);

    /* Initialize FLEXCAN device */
    FLEXCAN_Init(base);

#if FEATURE_CAN_HAS_MEM_ERR_DET
    /* Disable the Protection again because is enabled by soft reset */
    FLEXCAN_DisableMemErrorDetection(base);
#endif

#ifdef ERRATA_E10368
#if FEATURE_CAN_HAS_FD
    FLEXCAN_Errata10368(instance, data);
#endif /* FEATURE_CAN_HAS_FD */
#endif /* ERRATA_E10368 */

#if FEATURE_CAN_HAS_FD
    /* Enable/Disable FD and check FD was set as expected. Setting FD as enabled
     * might fail if the current CAN instance does not support FD. */
    FLEXCAN_SetFDEnabled(base, data->fd_enable);
    if (FLEXCAN_IsFDEnabled(base) != data->fd_enable)
    {
        return STATUS_ERROR;
    }

    /* If the FD feature is enabled, enable the Stuff Bit Count, in order to be
     * ISO-compliant. */
    FLEXCAN_SetStuffBitCount(base, data->fd_enable);
#endif

    /* Disable the self reception feature if FlexCAN is not in loopback mode. */
    if (data->flexcanMode != FLEXCAN_LOOPBACK_MODE)
    {
        FLEXCAN_SetSelfReception(base, false);
    }

    /* Enable RxFIFO feature, if requested. This might fail if the FD mode is
     * enabled. */
    if (data->is_rx_fifo_needed)
    {
        result = FLEXCAN_EnableRxFifo(base, (uint32_t)data->num_id_filters);
#if FEATURE_CAN_HAS_FD
        /* In case of non-fd capable devices this will be always false */
        if (result != STATUS_SUCCESS)
        {
            return result;
        }
#endif /* FEATURE_CAN_HAS_FD */
    }

#if FEATURE_CAN_HAS_DMA_ENABLE
    /* Enable DMA support for RxFIFO transfer, if requested. */
    if (data->transfer_type == FLEXCAN_RXFIFO_USING_DMA)
    {
        if (FLEXCAN_IsRxFifoEnabled(base))
        {
            FLEXCAN_SetRxFifoDMA(base, true);
        }
        else
        {
            return STATUS_ERROR;
        }
    }
    if (data->transfer_type == FLEXCAN_RXFIFO_USING_INTERRUPTS)
    {
    	FLEXCAN_SetRxFifoDMA(base, false);
    }
#endif

#if FEATURE_CAN_HAS_FD
    /* Set payload size. */
    FLEXCAN_SetPayloadSize(base, data->payload);
#endif

    result = FLEXCAN_SetMaxMsgBuffNum(base, data->max_num_mb);
    if (result != STATUS_SUCCESS)
    {
        return result;
    }

#if FEATURE_CAN_HAS_FD
    /* Set bit rate. */
    if (FLEXCAN_IsFDEnabled(base))
    {
        bitrate = data->bitrate;
        FLEXCAN_SetExtendedTimeSegments(base, &bitrate);
        bitrate = data->bitrate_cbt;
        FLEXCAN_SetFDTimeSegments(base, &bitrate);
    }
    else
#endif
    {
        bitrate = data->bitrate;
        FLEXCAN_SetTimeSegments(base, &bitrate);
    }

    /* Select mode */
    FLEXCAN_SetOperationMode(base, data->flexcanMode);

    if (data->flexcanMode != FLEXCAN_FREEZE_MODE)
    {
    	FLEXCAN_ExitFreezeMode(base);
    }

    /* Enable FlexCAN interrupts.*/
    FLEXCAN_EnableIRQs(instance);

    for (i = 0; i < FEATURE_CAN_MAX_MB_NUM; i++)
    {
        osifStat = OSIF_SemaCreate(&state->mbs[i].mbSema, 0U);
        if (osifStat != STATUS_SUCCESS)
        {
            for (j = 0; j < i; j++)
            {
                (void)OSIF_SemaDestroy(&state->mbs[j].mbSema);
            }
            return STATUS_ERROR;
        }
        state->mbs[i].isBlocking = false;
        state->mbs[i].mb_message = NULL;
        state->mbs[i].state = FLEXCAN_MB_IDLE;
    }

    /* Store transfer type and DMA channel number used in transfer */
    state->transferType = data->transfer_type;
#if FEATURE_CAN_HAS_DMA_ENABLE
    state->rxFifoDMAChannel = data->rxFifoDMAChannel;
#endif

    /* Clear Callbacks in case of autovariables garbage */
    state->callback = NULL;
    state->callbackParam = NULL;
    state->error_callback = NULL;
    state->errorCallbackParam = NULL;

    /* Save runtime structure pointers so irq handler can point to the correct state structure */
    g_flexcanStatePtr[instance] = state;
#endif 

    CAN_Type * base = g_flexcanBase[instance];

    uint16_t i = 0;
//TODO: Clean this up 
    for(i = 0; i< CAN_RAMn_COUNT; i+=MB_OFFSET)
    {   
        canbuf_t* mbuff = (canbuf_t *)&(base->RAMn[i]);
        (*mbuff).CS.B.CODE = 0;
    }


    base->MCR =  0x80000000UL;
    base->MCR =  0x02000000UL;
    while((base->MCR & CAN_MCR_FRZ_MASK) >> CAN_MCR_FRZ_SHIFT == 0)

    /* Set Final bit rate: 500kb/s @64MHz */
    /* Set prescaler, phase segments, clock source and propagation segment */

    base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_PRESDIV_MASK) | CAN_CTRL1_PRESDIV(7);
    base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_RJW_MASK) | CAN_CTRL1_RJW(1);
    base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_PSEG1_MASK)| CAN_CTRL1_PSEG1(6);
    base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_PSEG2_MASK)| CAN_CTRL1_PSEG2(6);
    base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_PROPSEG_MASK) | CAN_CTRL1_PROPSEG(0);
    base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_CLKSRC_MASK) | CAN_CTRL1_CLKSRC(1);


    base->RXMGMASK= 0x1FFFFFFFUL;     /* Set RX Global Mask */
    base->RX14MASK= 0x1FFFFFFFUL;    /* Set RX MB14 Mask */
    base->RX15MASK= 0x1FFFFFFFUL;    /* Set RX MB15 Mask */
    base->IFLAG2= 0xFFFFUL;            /* Clear all MB flags */
    base->IFLAG1= 0xFFFFUL;
    base->IMASK2= 0x0000UL;            /* Disable all MB interrupts */
    base->IMASK1= 0x0000UL;


    FLEXCAN_EnterFreezeMode(base);
    FLEXCAN_SetMaxMsgBuffNum(base,31);
    FLEXCAN_Disable(base);
    FLEXCAN_ExitFreezeMode(base);
    /* Enable FlexCAN interrupts.*/
    FLEXCAN_EnableIRQs(instance);
    #if 0 
    status_t osifStat;
    for (i = 0; i < FEATURE_CAN_MAX_MB_NUM; i++)
    {
        osifStat = OSIF_SemaCreate(&state->mbs[i].mbSema, 0U);
        if (osifStat != STATUS_SUCCESS)
        {
            for (j = 0; j < i; j++)
            {
                (void)OSIF_SemaDestroy(&state->mbs[j].mbSema);
            }
            return STATUS_ERROR;
        }
        state->mbs[i].isBlocking = false;
        state->mbs[i].mb_message = NULL;
        state->mbs[i].state = FLEXCAN_MB_IDLE;
    }
    #endif
    /* Store transfer type and DMA channel number used in transfer */
    state->transferType = data->transfer_type;

    /* Clear Callbacks in case of autovariables garbage */
    state->callback = NULL;
    state->callbackParam = NULL;
    state->error_callback = NULL;
    state->errorCallbackParam = NULL;

    /* Save runtime structure pointers so irq handler can point to the correct state structure */
    g_flexcanStatePtr[instance] = state;
    return (STATUS_SUCCESS);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_ConfigTxMb
 * Description   : Configure a Tx message buffer.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will set up the message buffer fields,
 * configure the message buffer code for Tx buffer as INACTIVE, and enable the
 * Message Buffer interrupt.
 *
 * Implements    : FLEXCAN_DRV_ConfigTxMb_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_ConfigTxMb(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(tx_info != NULL);

    flexcan_msgbuff_code_status_t cs;
    CAN_Type * base = g_flexcanBase[instance];

    /* Initialize transmit mb*/
    cs.dataLen = tx_info->data_length;
    cs.msgIdType = tx_info->msg_id_type;
#if FEATURE_CAN_HAS_FD
    cs.enable_brs = tx_info->enable_brs;
    cs.fd_enable = tx_info->fd_enable;
    cs.fd_padding = tx_info->fd_padding;
#endif

    cs.code = (uint32_t)FLEXCAN_TX_INACTIVE;

    return FLEXCAN_SetTxMsgBuff(base, mb_idx, &cs, msg_id, NULL, false);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_ConfigRemoteResponseMb
 * Description   : Configures a transmit message buffer for remote frame
 * response. This function will first check if RX FIFO is enabled. If RX FIFO is
 * enabled, the function will make sure if the MB requested is not occupied by
 * the RX FIFO and ID filter table. Then this function will set up the message
 * buffer fields, configure the message buffer code for Tx buffer as RX_RANSWER,
 * and enable the Message Buffer interrupt.
 *
 * Implements    : FLEXCAN_DRV_ConfigRemoteResponseMb_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_ConfigRemoteResponseMb(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id,
    const uint8_t *mb_data)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(tx_info != NULL);

    flexcan_msgbuff_code_status_t cs;
    CAN_Type * base = g_flexcanBase[instance];

    if (mb_idx > (((base->MCR) & CAN_MCR_MAXMB_MASK) >> CAN_MCR_MAXMB_SHIFT))
    {
        return STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    /* Initialize transmit mb*/
    cs.dataLen = tx_info->data_length;
    cs.msgIdType = tx_info->msg_id_type;
    cs.code = (uint32_t)FLEXCAN_RX_RANSWER;

    FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);

    return FLEXCAN_SetTxMsgBuff(base, mb_idx, &cs, msg_id, mb_data, tx_info->is_remote);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SendBlocking
 * Description   : This function sends a CAN frame using a configured message
 * buffer. The function blocks until either the frame was sent, or the specified
 * timeout expired.
 *
 * Implements    : FLEXCAN_DRV_SendBlocking_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_SendBlocking(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id,
    const uint8_t *mb_data,
    uint32_t timeout_ms)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(tx_info != NULL);

    status_t result;
    flexcan_state_t * state = g_flexcanStatePtr[instance];
    CAN_Type * base  = g_flexcanBase[instance];

    result = FLEXCAN_StartSendData(instance, mb_idx, tx_info, msg_id, mb_data, true);

    if (result == STATUS_SUCCESS)
    {
        status_t status;

        /* Enable message buffer interrupt*/
        (void)FLEXCAN_SetMsgBuffIntCmd(base, mb_idx, true);

        status = OSIF_SemaWait(&state->mbs[mb_idx].mbSema, timeout_ms);

        if (status == STATUS_TIMEOUT)
        {
        	if (state->mbs[mb_idx].state != FLEXCAN_MB_IDLE)
        	{
        	    /* Disable message buffer interrupt */
                (void)FLEXCAN_SetMsgBuffIntCmd(base, mb_idx, false);
                /* Clear message buffer flag */
                FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
				FLEXCAN_AbortTxMsgBuff(base, mb_idx);
				uint32_t flexcan_mb_config;

#ifndef ERRATA_E9527
				/* Wait to finish abort operation */
				while(FLEXCAN_GetBuffStatusFlag(base, mb_idx) == 0U)
				{
					/* Do Nothing wait for the flag */
				}
#endif
				volatile const uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, mb_idx);
				flexcan_mb_config = * flexcan_mb;
				/* Check if the MBs have been safely Inactivated */
				if (((flexcan_mb_config & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)FLEXCAN_TX_INACTIVE)
				{
#ifndef ERRATA_E9527
					/* Transmission have occurred */
					result = STATUS_SUCCESS;
#else
	     			/* Transmission have not occurred because it uses the inactivation operation */
					result = FLEXCAN_GetBuffStatus(base, mb_idx);
#endif
				}
#ifndef ERRATA_E9527
				if (((flexcan_mb_config & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)FLEXCAN_TX_ABORT)
				{
					/* Transmission have been aborted */
						result = STATUS_TIMEOUT;
				}
#endif
				/* Clear message buffer flag */
				FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
				state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;
			}
        }
    }
    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_Send
 * Description   : This function sends a CAN frame using a configured message
 * buffer. The function returns immediately. If a callback is installed, it will
 * be invoked after the frame was sent.
 *
 * Implements    : FLEXCAN_DRV_Send_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_Send(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *tx_info,
    uint32_t msg_id,
    const uint8_t *mb_data)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(tx_info != NULL);

    status_t result;
    CAN_Type * base = g_flexcanBase[instance];

    result = FLEXCAN_StartSendData(instance, mb_idx, tx_info, msg_id, mb_data, false);
    if(result == STATUS_SUCCESS)
    {
        /* Enable message buffer interrupt*/
        result = FLEXCAN_SetMsgBuffIntCmd(base, mb_idx, true);
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_ConfigMb
 * Description   : Configure a Rx message buffer.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will set up the message buffer fields,
 * configure the message buffer code for Rx message buffer as NOT_USED, enable
 * the Message Buffer interrupt, configure the message buffer code for Rx
 * message buffer as INACTIVE, copy user's buffer into the message buffer data
 * area, and configure the message buffer code for Rx message buffer as EMPTY.
 *
 * Implements    : FLEXCAN_DRV_ConfigRxMb_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_ConfigRxMb(
    uint8_t instance,
    uint8_t mb_idx,
    const flexcan_data_info_t *rx_info,
    uint32_t msg_id)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(rx_info != NULL);

    status_t result;
    flexcan_msgbuff_code_status_t cs;
    CAN_Type * base = g_flexcanBase[instance];

    cs.dataLen = rx_info->data_length;
    cs.msgIdType = rx_info->msg_id_type;
#if FEATURE_CAN_HAS_FD
    cs.fd_enable = rx_info->fd_enable;
#endif
    /* Initialize rx mb*/
    cs.code = (uint32_t)FLEXCAN_RX_NOT_USED;
    result = FLEXCAN_SetRxMsgBuff(base, mb_idx, &cs, msg_id);

    if (result == STATUS_SUCCESS)
    {
		/* Initialize receive MB*/
		cs.code = (uint32_t)FLEXCAN_RX_INACTIVE;
		result = FLEXCAN_SetRxMsgBuff(base, mb_idx, &cs, msg_id);
    }

    if (result == STATUS_SUCCESS)
    {
    	 /* Set up FlexCAN message buffer fields for receiving data*/
    	 cs.code = (uint32_t)FLEXCAN_RX_EMPTY;
    	 result = FLEXCAN_SetRxMsgBuff(base, mb_idx, &cs, msg_id);
    }
    if (result == STATUS_SUCCESS)
    {
        /* Clear the message buffer flag if previous remained triggered*/
        FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_ConfigRxFifo
 * Description   : Confgure RX FIFO ID filter table elements.
 * This function will confgure RX FIFO ID filter table elements, and enable RX
 * FIFO interrupts.
 *
 * Implements    : FLEXCAN_DRV_ConfigRxFifo_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_ConfigRxFifo(
    uint8_t instance,
    flexcan_rx_fifo_id_element_format_t id_format,
    const flexcan_id_table_t *id_filter_table)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    /* Initialize rx fifo*/
    FLEXCAN_SetRxFifoFilter(base, id_format, id_filter_table);

    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_ReceiveBlocking
 * Description   : This function receives a CAN frame into a configured message
 * buffer. The function blocks until either a frame was received, or the
 * specified timeout expired.
 *
 * Implements    : FLEXCAN_DRV_ReceiveBlocking_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_ReceiveBlocking(
    uint8_t instance,
    uint8_t mb_idx,
    flexcan_msgbuff_t *data,
    uint32_t timeout_ms)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    status_t result;
    flexcan_state_t * state = g_flexcanStatePtr[instance];
    CAN_Type * base = g_flexcanBase[instance];

    result = FLEXCAN_StartRxMessageBufferData(instance, mb_idx, data, true);

    if(result == STATUS_SUCCESS)
    {
        status_t status;

        status = OSIF_SemaWait(&state->mbs[mb_idx].mbSema, timeout_ms);

        if (status == STATUS_TIMEOUT)
        {
            /* If the flag is set Successful reception else report TimeOut */
			if(FLEXCAN_GetMsgBuffIntStatusFlag(base,mb_idx) == (uint8_t)0U)
			{
				result = STATUS_TIMEOUT;
			}
			/* Disable message buffer interrupt */
			(void)FLEXCAN_SetMsgBuffIntCmd(base, mb_idx, false);
        }
        /* Consider the MB state has been changed by interrupt as frame received */ 
        if (state->mbs[mb_idx].state == FLEXCAN_MB_IDLE)
        {
        	return STATUS_SUCCESS;
        }

        state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_Receive
 * Description   : This function receives a CAN frame into a configured message
 * buffer. The function returns immediately. If a callback is installed, it will
 * be invoked after the frame was received and read into the specified buffer.
 *
 * Implements    : FLEXCAN_DRV_Receive_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_Receive(
    uint8_t instance,
    uint8_t mb_idx,
    flexcan_msgbuff_t *data)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    status_t result;

    result = FLEXCAN_StartRxMessageBufferData(instance, mb_idx, data, false);

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_RxFifoBlocking
 * Description   : This function receives a CAN frame using the Rx FIFO. The
 * function blocks until either a frame was received, or the specified timeout
 * expired.
 *
 * Implements    : FLEXCAN_DRV_RxFifoBlocking_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_RxFifoBlocking(
    uint8_t instance,
    flexcan_msgbuff_t *data,
    uint32_t timeout_ms)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    status_t result;
    flexcan_state_t * state = g_flexcanStatePtr[instance];
    CAN_Type * base = g_flexcanBase[instance];

    result = FLEXCAN_StartRxMessageFifoData(instance, data, true);

    if (result == STATUS_SUCCESS)
    {
        result = OSIF_SemaWait(&state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].mbSema, timeout_ms);

        if (result == STATUS_TIMEOUT)
        {
        	 /* If the status is updated reception successful else report TimeOut */
			if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state == FLEXCAN_MB_IDLE)
			{
				result = STATUS_SUCCESS;
			}
            /* Disable RX FIFO interrupts*/
            (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_FRAME_AVAILABLE, false);
            (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_WARNING, false);
            (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_OVERFLOW, false);
            
#if FEATURE_CAN_HAS_DMA_ENABLE
            /* Check if transfer is done over DMA and stop transfer */
            if ((state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state == FLEXCAN_MB_RX_BUSY) && (state->transferType == FLEXCAN_RXFIFO_USING_DMA))
            {
                /* This function always return status success */
                (void)EDMA_DRV_StopChannel(state->rxFifoDMAChannel);
            }
#endif
        }
        state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_RxFifo
 * Description   : This function receives a CAN frame using the Rx FIFO. The
 * function returns immediately. If a callback is installed, it will be invoked
 * after the frame was received and read into the specified buffer.
 *
 * Implements    : FLEXCAN_DRV_RxFifo_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_RxFifo(
    uint8_t instance,
    flexcan_msgbuff_t *data)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    status_t result;

    result = FLEXCAN_StartRxMessageFifoData(instance, data, false);

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_Deinit
 * Description   : Shutdown a FlexCAN module.
 * This function will disable all FlexCAN interrupts, and disable the FlexCAN.
 *
 * Implements    : FLEXCAN_DRV_Deinit_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_Deinit(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    const flexcan_state_t * state = g_flexcanStatePtr[instance];
    status_t result = STATUS_SUCCESS;
    status_t osifStat;
    uint32_t i;


    /* Disable FlexCAN interrupts.*/
    if (state != NULL)
    {
		if (state->error_callback != NULL)
		{
			FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_ERR, false);
			FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_RX_WARNING, false);
			FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_TX_WARNING, false);
			FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_BUSOFF, false);
		}
    }

#if FEATURE_CAN_HAS_WAKE_UP_IRQ
    if (g_flexcanWakeUpIrqId[instance] != NotAvail_IRQn)
    {
        INT_SYS_DisableIRQ(g_flexcanWakeUpIrqId[instance]);
    }
#endif
    INT_SYS_DisableIRQ(g_flexcanErrorIrqId[instance]);
    INT_SYS_DisableIRQ(g_flexcanBusOffIrqId[instance]);
    for (i = 0; i < FEATURE_CAN_MB_IRQS_MAX_COUNT; i++)
    {
        if (g_flexcanOredMessageBufferIrqId[i][instance] != NotAvail_IRQn)
        {
            INT_SYS_DisableIRQ(g_flexcanOredMessageBufferIrqId[i][instance]);
        }
    }

    if(FLEXCAN_IsEnabled(g_flexcanBase[instance]))
    {
        /* Enter Freeze Mode Required before to enter Disabled Mode */
        FLEXCAN_EnterFreezeMode(g_flexcanBase[instance]);
        /* Disable FlexCAN.*/
        FLEXCAN_Disable(g_flexcanBase[instance]);
        /* Check if the state have been initialized */
    }

    if (state != NULL)
    {
		for (i = 0; i < FEATURE_CAN_MAX_MB_NUM; i++)
		{
			osifStat = OSIF_SemaDestroy(&state->mbs[i].mbSema);
			if (osifStat != STATUS_SUCCESS)
			{
				result = STATUS_ERROR;
			}
		}
    }

    if (result == STATUS_SUCCESS)
    {
    	/* Clear state pointer that is checked by FLEXCAN_DRV_Init */
    	g_flexcanStatePtr[instance] = NULL;
    }

    return result;
}

#if FEATURE_CAN_HAS_FD
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_SetTDCOffset
 * Description   : Enables/Disables the Transceiver Delay Compensation feature and sets
 * the Transceiver Delay Compensation Offset.
 *
 * Implements    : FLEXCAN_DRV_SetTDCOffset_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_SetTDCOffset(uint8_t instance, bool enable, uint8_t offset)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    /* Enable/Disable TDC and set the TDC Offset */
    FLEXCAN_SetTDCOffset(base, enable, offset);

    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetTDCValue
 * Description   : Gets the value of the Transceiver Delay Compensation.
 *
 * Implements    : FLEXCAN_DRV_GetTDCValue_Activity
 *END**************************************************************************/
uint8_t FLEXCAN_DRV_GetTDCValue(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    const CAN_Type * base = g_flexcanBase[instance];

    return (uint8_t)((base->FDCTRL & CAN_FDCTRL_TDCVAL_MASK) >> CAN_FDCTRL_TDCVAL_SHIFT);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetTDCFail
 * Description   : Gets the value of the TDC Fail flag.
 *
 * Implements    : FLEXCAN_DRV_GetTDCFail_Activity
 *END**************************************************************************/
bool FLEXCAN_DRV_GetTDCFail(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    const CAN_Type * base = g_flexcanBase[instance];

    return (((base->FDCTRL & CAN_FDCTRL_TDCFAIL_MASK) >> CAN_FDCTRL_TDCFAIL_SHIFT) != 0U);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_ClearTDCFail
 * Description   : Clears the TDC Fail flag.
 *
 * Implements    : FLEXCAN_DRV_ClearTDCFail_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_ClearTDCFail(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    base->FDCTRL = base->FDCTRL | CAN_FDCTRL_TDCFAIL_MASK;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_IRQHandlerRxFIFO
 * Description   : Process IRQHandler in case of RxFIFO mode selection for CAN interface.
 *
 * Implements    : FLEXCAN_IRQHandlerRxFIFO_Activity
 *END**************************************************************************/
static inline void FLEXCAN_IRQHandlerRxFIFO(uint8_t instance, uint32_t mb_idx)
{
    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];
    
    if (mb_idx == FEATURE_CAN_RXFIFO_FRAME_AVAILABLE) 
            {
                if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state == FLEXCAN_MB_RX_BUSY)
                {
                    /* Get RX FIFO field values */
                    FLEXCAN_ReadRxFifo(base, state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].mb_message);

                    FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);

                    state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;

                    /* Invoke callback */
                    if (state->callback != NULL)
                    {
                        state->callback(instance,
                                        FLEXCAN_EVENT_RXFIFO_COMPLETE,
                                        FLEXCAN_MB_HANDLE_RXFIFO,
                                        state);
                    }

                    if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state == FLEXCAN_MB_IDLE)
                    {
                        /* Complete receive data */
                        FLEXCAN_CompleteRxMessageFifoData(instance);
                    }
                }
            }
            else if (mb_idx == FEATURE_CAN_RXFIFO_WARNING)
            {
                FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);

                /* Invoke callback */
                if (state->callback != NULL)
                {
                    state->callback(instance,
                                    FLEXCAN_EVENT_RXFIFO_WARNING,
                                    FLEXCAN_MB_HANDLE_RXFIFO,
                                    state);
                }
            }
            else if (mb_idx == FEATURE_CAN_RXFIFO_OVERFLOW)
            {
                FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);

                /* Invoke callback */
                if (state->callback != NULL)
                {
                    state->callback(instance,
                                    FLEXCAN_EVENT_RXFIFO_OVERFLOW,
                                    FLEXCAN_MB_HANDLE_RXFIFO,
                                    state);
                }
            }
            else
            {
                /* Do Nothing */
            }

}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_IRQHandlerRxMB
 * Description   : Process IRQHandler in case of Rx MessageBuffer selection
 * for CAN interface.
 *
 * This is not a public API as it is called whenever an interrupt and receive
 * individual MB occurs
 *END**************************************************************************/
static void FLEXCAN_IRQHandlerRxMB(uint8_t instance, uint32_t mb_idx)
{
	 CAN_Type * base = g_flexcanBase[instance];
	 flexcan_state_t * state = g_flexcanStatePtr[instance];

	 /* Lock RX message buffer and RX FIFO*/
	 FLEXCAN_LockRxMsgBuff(base, mb_idx);

	 /* Get RX MB field values*/
	 FLEXCAN_GetMsgBuff(base, mb_idx, state->mbs[mb_idx].mb_message);

	 /* Unlock RX message buffer and RX FIFO*/
	 FLEXCAN_UnlockRxMsgBuff(base);

	 FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);

     state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;

	 /* Invoke callback */
	 if (state->callback != NULL)
	 {
		 state->callback(instance, FLEXCAN_EVENT_RX_COMPLETE, mb_idx, state);
	 }

     if (state->mbs[mb_idx].state == FLEXCAN_MB_IDLE)
     {
         /* Complete receive data */
         FLEXCAN_CompleteTransfer(instance, mb_idx);
     }
}


/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_IRQHandler
 * Description   : Interrupt handler for FLEXCAN.
 * This handler read data from MB or FIFO, and then clear the interrupt flags.
 * This is not a public API as it is called whenever an interrupt occurs.
 *
 *END**************************************************************************/
void FLEXCAN_IRQHandler(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    uint32_t flag_reg = 0;
    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];

    /* Get the interrupts that are enabled and ready */
    uint32_t mb_idx = 0;
    flag_reg = FLEXCAN_GetMsgBuffIntStatusFlag(base, mb_idx);

    while ((flag_reg & 1U) == 0U)
    {
        mb_idx++;
        flag_reg = FLEXCAN_GetMsgBuffIntStatusFlag(base, mb_idx);

        if (mb_idx >= FEATURE_CAN_MAX_MB_NUM)
        {
            break;
        }
    }

    /* Check Tx/Rx interrupt flag and clear the interrupt */
    if(flag_reg != 0U)
    {
        if (FLEXCAN_IsRxFifoEnabled(base) && (mb_idx <= FEATURE_CAN_RXFIFO_OVERFLOW))
        {
            FLEXCAN_IRQHandlerRxFIFO(instance, mb_idx);
        }
        else
        {
            /* Check mailbox completed reception */
            if (state->mbs[mb_idx].state == FLEXCAN_MB_RX_BUSY)
            {
            	FLEXCAN_IRQHandlerRxMB(instance, mb_idx);
            }
        }

        /* Check mailbox completed transmission */
        if (state->mbs[mb_idx].state == FLEXCAN_MB_TX_BUSY)
        {
            if (state->mbs[mb_idx].isRemote)
            {
                /* If the frame was a remote frame, clear the flag only if the response was
                 * not received yet. If the response was received, leave the flag set in order
                 * to be handled when the user calls FLEXCAN_DRV_RxMessageBuffer. */
                flexcan_msgbuff_t mb;
                FLEXCAN_LockRxMsgBuff(base, mb_idx);
                FLEXCAN_GetMsgBuff(base, mb_idx, &mb);
                FLEXCAN_UnlockRxMsgBuff(base);

                if (((mb.cs & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)FLEXCAN_RX_EMPTY)
                {
                    FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
                }
            }
            else
            {
                FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
            }

            state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;

            /* Invoke callback */
            if (state->callback != NULL)
            {
                state->callback(instance, FLEXCAN_EVENT_TX_COMPLETE, mb_idx, state);
            }

            if (state->mbs[mb_idx].state == FLEXCAN_MB_IDLE)
            {
                /* Complete transmit data */
                FLEXCAN_CompleteTransfer(instance, mb_idx);
            }
        }

        if (FLEXCAN_GetMsgBuffIntStatusFlag(base, mb_idx) != 0U)
        {
            /* In case of desynchronized status of the MB to avoid trapping in ISR
             * clear the MB flag */
            FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
        }
    }
#if (defined(CPU_S32K116) || defined(CPU_S32K118))
    else
    {
#if FEATURE_CAN_HAS_PRETENDED_NETWORKING
		/* The pretending Network Feature is present on all CPUs
		 * only on FLEXCAN Instance 0   */
		if (instance == 0U)
		{
			uint8_t wtof = FLEXCAN_GetWTOF(base);
			uint8_t wumf = FLEXCAN_GetWUMF(base);

			/* Check if wake-up event occurred */
			if ((wtof != 0U) || (wumf != 0U))
			{
				FLEXCAN_WakeUpHandler(instance);
			}
		}
#endif /* FEATURE_CAN_HAS_PRETENDED_NETWORKING */

        uint32_t ecr = FLEXCAN_GetErrorCounters(base);

        /* Check if any error occurred */
        if (ecr != 0U)
        {
            FLEXCAN_Error_IRQHandler(instance);
        }
    }
#endif /* (defined(CPU_S32K116) || defined(CPU_S32K118)) */
    return;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_Error_IRQHandler
 * Description   : Error interrupt handler for FLEXCAN.
 * This handler read data from MB or FIFO, and then clear the interrupt flags.
 * This is not a public API as it is called whenever an interrupt occurs.
 *
 *END**************************************************************************/
void FLEXCAN_Error_IRQHandler(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];

    /* Invoke callback */
    if (state->error_callback != NULL)
    {
        state->error_callback(instance, FLEXCAN_EVENT_ERROR, state);
    }

    /* Clear all other interrupts in ESR1 register (Error, Busoff, Wakeup) */
    FLEXCAN_ClearErrIntStatusFlag(base);

    return;
}

#if FEATURE_CAN_HAS_WAKE_UP_IRQ

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_WakeUpHandler
 * Description   : Wake up handler for FLEXCAN.
 * This handler verifies the event which caused the wake up and invokes the
 * user callback, if configured.
 * This is not a public API as it is called whenever an wake up event occurs.
 *
 *END**************************************************************************/
void FLEXCAN_WakeUpHandler(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];


#if FEATURE_CAN_HAS_PRETENDED_NETWORKING
        if (FLEXCAN_IsPNEnabled(base))
        {
            if (FLEXCAN_GetWTOF(base) != 0U)
            {
                FLEXCAN_ClearWTOF(base);
                /* Invoke callback */
                if (state->callback != NULL)
                {
                	state->callback(instance, FLEXCAN_EVENT_WAKEUP_TIMEOUT, 0U, state);
                }
            }
            if (FLEXCAN_GetWUMF(base) != 0U)
            {
                FLEXCAN_ClearWUMF(base);
                /* Invoke callback */
                if (state->callback != NULL)
                {
                	state->callback(instance, FLEXCAN_EVENT_WAKEUP_MATCH, 0U, state);
                }
            }
        }
#endif
}
#endif /* FEATURE_CAN_HAS_WAKE_UP_IRQ */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetTransferStatus
 * Description   : This function returns whether the previous FLEXCAN receive is
 *                 completed.
 * When performing a non-blocking receive, the user can call this function to
 * ascertain the state of the current receive progress: in progress (or busy)
 * or complete (success).
 *
 * Implements    : FLEXCAN_DRV_GetTransferStatus_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_GetTransferStatus(uint8_t instance, uint8_t mb_idx)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    const flexcan_state_t * state = g_flexcanStatePtr[instance];
    status_t status;

    if (state->mbs[mb_idx].state == FLEXCAN_MB_IDLE)
    {
        status = STATUS_SUCCESS;
    }
#if FEATURE_CAN_HAS_DMA_ENABLE
    else if (state->mbs[mb_idx].state == FLEXCAN_MB_DMA_ERROR)
    {
        status = STATUS_ERROR;
    }
#endif
    else
    {
       	status = STATUS_BUSY;
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetErrorStatus
 * Description   : Reports various error conditions detected in the reception and
 *                 transmission of a CAN frame and some general status of the device.
 *
 * Implements    : FLEXCAN_DRV_GetErrorStatus_Activity
 *END**************************************************************************/
uint32_t FLEXCAN_DRV_GetErrorStatus(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    const CAN_Type * base = g_flexcanBase[instance];

    return ((uint32_t)(base->ESR1));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_AbortTransfer
 * Description   : This function shuts down the FLEXCAN by disabling interrupts and
 *                 the transmitter/receiver.
 * This function disables the FLEXCAN interrupts, disables the transmitter and
 * receiver.
 *
 * Implements    : FLEXCAN_DRV_AbortTransfer_Activity
 *END**************************************************************************/
status_t FLEXCAN_DRV_AbortTransfer(uint8_t instance, uint8_t mb_idx)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    flexcan_state_t * state = g_flexcanStatePtr[instance];
    CAN_Type * base = g_flexcanBase[instance];

    /* Check if a transfer is running. */
    if (state->mbs[mb_idx].state == FLEXCAN_MB_IDLE)
    {
        return STATUS_CAN_NO_TRANSFER_IN_PROGRESS;
    }

    if (state->mbs[mb_idx].state == FLEXCAN_MB_TX_BUSY)
    {
    	status_t result = STATUS_SUCCESS;
		/* Stop the running transfer. */
		FLEXCAN_CompleteTransfer(instance, mb_idx);
    	/* Clear message buffer flag */
    	FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
        FLEXCAN_AbortTxMsgBuff(base, mb_idx);
		uint32_t flexcan_mb_config;

#ifndef ERRATA_E9527
		/* Wait to finish abort operation */
		while(FLEXCAN_GetBuffStatusFlag(base, mb_idx) == 0U)
		{
			/* Do Nothing wait for the flag */
		}
#endif
		volatile const uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, mb_idx);
		flexcan_mb_config = * flexcan_mb;
		/* Check if the MBs have been safely Inactivated */
		if (((flexcan_mb_config & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)FLEXCAN_TX_INACTIVE)
		{
#ifndef ERRATA_E9527
			/* Transmission have occurred */
			result = STATUS_CAN_NO_TRANSFER_IN_PROGRESS;
#else
 			/* Transmission have not occurred because it uses the inactivation operation */
			if (FLEXCAN_GetBuffStatusFlag(base, mb_idx) == 0U)
			{
				result = STATUS_SUCCESS;
			}
			else
			{
				result = STATUS_CAN_NO_TRANSFER_IN_PROGRESS;
			}
#endif
		}
#ifndef ERRATA_E9527
		if (((flexcan_mb_config & CAN_CS_CODE_MASK) >> CAN_CS_CODE_SHIFT) == (uint32_t)FLEXCAN_TX_ABORT)
		{
			/* Transmission have been aborted */
				result = STATUS_SUCCESS;
		}
#endif
		/* Clear message buffer flag */
		FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
		state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;
		return result;
    }

    if (state->mbs[mb_idx].state == FLEXCAN_MB_RX_BUSY)
    {
		/* Stop the running transfer. */
		FLEXCAN_CompleteTransfer(instance, mb_idx);

    	if(FLEXCAN_IsRxFifoEnabled(base) == true)
        {

    		/* Get the number of RX FIFO Filters*/
    		uint32_t val1 = (((base->CTRL2) & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
    		/* Get the number if MBs occupied by RX FIFO and ID filter table*/
    		/* the Rx FIFO occupies the memory space originally reserved for MB0-5*/
			/* Every number of RFFN means 8 number of RX FIFO filters*/
			/* and every 4 number of RX FIFO filters occupied one MB*/
    		uint32_t val2 = RxFifoOcuppiedLastMsgBuff(val1);
			if(mb_idx > val2)
			{
				/* This operation is not allowed for MB that are part of RxFIFO */
				FLEXCAN_ResetRxMsgBuff(base, mb_idx);
			}
			if (mb_idx == FLEXCAN_MB_HANDLE_RXFIFO)
			{
		        /* Disable RX FIFO interrupts*/
		        (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_FRAME_AVAILABLE, false);
#if FEATURE_CAN_HAS_DMA_ENABLE
		        if (state->transferType == FLEXCAN_RXFIFO_USING_DMA)
		        {
		        	(void) EDMA_DRV_StopChannel(state->rxFifoDMAChannel);
		        }
#endif
			}
        }
    	else
    	{
    		/* This operation is not allowed for MB that are part of RxFIFO */
    		FLEXCAN_ResetRxMsgBuff(base, mb_idx);
    	}
    }

    if (FLEXCAN_GetMsgBuffIntStatus(base, mb_idx) == 1U)
    {
		/* Stop the running transfer. */
		FLEXCAN_CompleteTransfer(instance, mb_idx);
		/* Clear message buffer flag */
		FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);
    }

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_StartSendData
 * Description   : Initiate (start) a transmit by beginning the process of
 * sending data.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static status_t FLEXCAN_StartSendData(
                    uint8_t instance,
                    uint8_t mb_idx,
                    const flexcan_data_info_t *tx_info,
                    uint32_t msg_id,
                    const uint8_t *mb_data,
                    bool isBlocking
                    )
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(tx_info != NULL);

    status_t result;
    flexcan_msgbuff_code_status_t cs;
    flexcan_state_t * state = g_flexcanStatePtr[instance];
    CAN_Type * base = g_flexcanBase[instance];

    if(mb_idx >= FLEXCAN_GetMaxMbNum(base))
    {
    	return STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    if (state->mbs[mb_idx].state != FLEXCAN_MB_IDLE)
    {
        return STATUS_BUSY;
    }

    /* Clear message buffer flag */
	FLEXCAN_ClearMsgBuffIntStatusFlag(base, mb_idx);

    state->mbs[mb_idx].state = FLEXCAN_MB_TX_BUSY;
    state->mbs[mb_idx].isBlocking = isBlocking;
    state->mbs[mb_idx].isRemote = tx_info->is_remote;

    cs.dataLen = tx_info->data_length;
    cs.msgIdType = tx_info->msg_id_type;

#if FEATURE_CAN_HAS_FD
    cs.fd_enable = tx_info->fd_enable;
    cs.fd_padding = tx_info->fd_padding;
    cs.enable_brs = tx_info->enable_brs;
#endif

    if (tx_info->is_remote)
    {
        cs.code = (uint32_t)FLEXCAN_TX_REMOTE;
    }
    else
    {
        cs.code = (uint32_t)FLEXCAN_TX_DATA;
    }
    result = FLEXCAN_SetTxMsgBuff(base, mb_idx, &cs, msg_id, mb_data, false);

    if (result != STATUS_SUCCESS)
    {
        state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_StartRxMessageBufferData
 * Description   : Initiate (start) a receive by beginning the process of
 * receiving data and enabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static status_t FLEXCAN_StartRxMessageBufferData(
                    uint8_t instance,
                    uint8_t mb_idx,
                    flexcan_msgbuff_t *data,
                    bool isBlocking
                    )
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    status_t result = STATUS_SUCCESS;
    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];

    /* Check if the MB index is in range */
    if(mb_idx >= FLEXCAN_GetMaxMbNum(base))
    {
    	return STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    /* Check if RX FIFO is enabled*/
    if (FLEXCAN_IsRxFifoEnabled(base) == true)
    {
    	uint32_t val1, val2;
        /* Get the number of RX FIFO Filters*/
        val1 = (((base->CTRL2) & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        /* Get the number if MBs occupied by RX FIFO and ID filter table*/
        /* the Rx FIFO occupies the memory space originally reserved for MB0-5*/
        /* Every number of RFFN means 8 number of RX FIFO filters*/
        /* and every 4 number of RX FIFO filters occupied one MB*/
        val2 = RxFifoOcuppiedLastMsgBuff(val1);

        if (mb_idx <= val2)
        {
        	return STATUS_CAN_BUFF_OUT_OF_RANGE;
        }
    }

    /* Start receiving mailbox */
    if(state->mbs[mb_idx].state != FLEXCAN_MB_IDLE)
    {
        return STATUS_BUSY;
    }
    state->mbs[mb_idx].state = FLEXCAN_MB_RX_BUSY;
    state->mbs[mb_idx].mb_message = data;
    state->mbs[mb_idx].isBlocking = isBlocking;

    /* Enable MB interrupt*/
    result = FLEXCAN_SetMsgBuffIntCmd(base, mb_idx, true);

    if (result != STATUS_SUCCESS)
    {
        state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;
    }

    return result;
}


/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_StartRxMessageFifoData
 * Description   : Initiate (start) a receive by beginning the process of
 * receiving data and enabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static status_t FLEXCAN_StartRxMessageFifoData(
                    uint8_t instance,
                    flexcan_msgbuff_t *data,
                    bool isBlocking
                    )
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];
#if FEATURE_CAN_HAS_DMA_ENABLE
    status_t edmaStat;

    if (state->transferType == FLEXCAN_RXFIFO_USING_DMA)
    {
    	if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state == FLEXCAN_MB_DMA_ERROR)
    	{
			/* Check if FIFO has Pending Request that generated error,
			 * the RxFIFO need to be empty to activate DMA */
			if (FLEXCAN_GetMsgBuffIntStatusFlag(base,FEATURE_CAN_RXFIFO_FRAME_AVAILABLE) == (uint8_t)1U)
			{
				/* Enter CAN in freeze Mode to allow FIFO Clear */
				FLEXCAN_EnterFreezeMode(base);

				FLEXCAN_ClearFIFO(base);
				do
				{	/* Read Offset 0x8C to clear DMA pending request */
					(void) base->RAMn[3];
				}
				while (FLEXCAN_GetMsgBuffIntStatusFlag(base,FEATURE_CAN_RXFIFO_FRAME_AVAILABLE) == (uint8_t)1U);

				/* Return CAN to normal Mode */
				FLEXCAN_ExitFreezeMode(base);
			}
			/* Change status of MB to be reconfigured with DMA transfer */
			state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;
    	}
    }
#endif /* FEATURE_CAN_HAS_DMA_ENABLE */

    /* Start receiving fifo */
    if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state != FLEXCAN_MB_IDLE)
    {
        return STATUS_BUSY;
    }
    /* Check if RxFIFO feature is enabled */
    if (!FLEXCAN_IsRxFifoEnabled(base))
    {
        return STATUS_ERROR;
    }

    state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_RX_BUSY;

    state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].isBlocking = isBlocking;

    /* This will get filled by the interrupt handler */
    state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].mb_message = data;

    /* Enable RX FIFO interrupts*/
    (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_WARNING, true);
    (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_OVERFLOW, true);
    (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_FRAME_AVAILABLE, true);

#if FEATURE_CAN_HAS_DMA_ENABLE
    if (state->transferType == FLEXCAN_RXFIFO_USING_DMA)
    {
        status_t edmaStatus;

        edmaStatus = EDMA_DRV_InstallCallback(state->rxFifoDMAChannel,
                                              FLEXCAN_CompleteRxFifoDataDMA,
                                              (void *)((uint32_t)instance));

        if (edmaStatus != STATUS_SUCCESS)
        {
            state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;
            return STATUS_ERROR;
        }

        edmaStatus = EDMA_DRV_ConfigSingleBlockTransfer(state->rxFifoDMAChannel,
                                                        EDMA_TRANSFER_MEM2MEM,
                                                        (uint32_t)(base->RAMn),
                                                        (uint32_t)(state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].mb_message),
                                                        EDMA_TRANSFER_SIZE_4B,
                                                        16U);

        if (edmaStatus != STATUS_SUCCESS)
        {
            state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;
            return STATUS_ERROR;
        }

        EDMA_DRV_DisableRequestsOnTransferComplete(state->rxFifoDMAChannel, true);

        edmaStat = EDMA_DRV_StartChannel(state->rxFifoDMAChannel);
        if (edmaStat != STATUS_SUCCESS)
        {
            state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;
            return STATUS_ERROR;
        }
    }
#endif

    return STATUS_SUCCESS;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_CompleteTransfer
 * Description   : Finish up a transmit by completing the process of sending
 * data and disabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void FLEXCAN_CompleteTransfer(uint8_t instance, uint32_t mb_idx)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];

    /* Disable the transmitter data register empty interrupt */
    (void)FLEXCAN_SetMsgBuffIntCmd(base, mb_idx, false);

    /* Update the information of the module driver state */
    if (state->mbs[mb_idx].isBlocking)
    {
        (void)OSIF_SemaPost(&state->mbs[mb_idx].mbSema);
    }
    state->mbs[mb_idx].state = FLEXCAN_MB_IDLE;
}

#if FEATURE_CAN_HAS_DMA_ENABLE
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_CompleteRxFifoDataDMA
 * Description   : Finish up a DMA transfer (this is just a wrapper over
 * FLEXCAN_DRV_CompleteRxMessageFifoData).
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void FLEXCAN_CompleteRxFifoDataDMA(void *parameter, edma_chn_status_t status)
{
    uint32_t instance = (uint32_t)parameter;

    if(status == EDMA_CHN_ERROR)
    {
    	flexcan_state_t * state = g_flexcanStatePtr[instance];
    	state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_DMA_ERROR;

        if	(state->callback != NULL)
    	{
        	state->callback((uint8_t)instance,
        					FLEXCAN_EVENT_DMA_ERROR,
							FLEXCAN_MB_HANDLE_RXFIFO,
							state);
    	}
    }
    FLEXCAN_CompleteRxMessageFifoData((uint8_t)instance);
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_CompleteRxMessageFifoData
 * Description   : Finish up a receive by completing the process of receiving
 * data and disabling the interrupt.
 * This is not a public API as it is called from other driver functions.
 *
 *END**************************************************************************/
static void FLEXCAN_CompleteRxMessageFifoData(uint8_t instance)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];

    if (state->transferType == FLEXCAN_RXFIFO_USING_INTERRUPTS)
    {
        /* Disable RX FIFO interrupts*/
        (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_FRAME_AVAILABLE, false);
        (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_WARNING, false);
        (void)FLEXCAN_SetMsgBuffIntCmd(base, FEATURE_CAN_RXFIFO_OVERFLOW, false);
    }
#if FEATURE_CAN_HAS_DMA_ENABLE
    else
    {
    	if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state != FLEXCAN_MB_DMA_ERROR)
    	{
			flexcan_msgbuff_t *fifo_message = state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].mb_message;
			uint32_t *msgData_32 = (uint32_t *)fifo_message->data;

			(void) EDMA_DRV_StopChannel(state->rxFifoDMAChannel);
			/* Adjust the ID if it is not extended */
			if (((fifo_message->cs) & CAN_CS_IDE_MASK) == 0U)
			{
				fifo_message->msgId = fifo_message->msgId  >> CAN_ID_STD_SHIFT;
			}
			/* Extract the data length */
			fifo_message->dataLen = (uint8_t)((fifo_message->cs & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT);
			/* Reverse the endianness */
			FlexcanSwapBytesInWord(msgData_32[0], msgData_32[0]);
			FlexcanSwapBytesInWord(msgData_32[1], msgData_32[1]);
    	}
    }
#endif
    /* Clear fifo message*/
    state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].mb_message = NULL;

#if FEATURE_CAN_HAS_DMA_ENABLE
    if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state != FLEXCAN_MB_DMA_ERROR)
    {
    	state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;
    	if ((state->callback != NULL) && (state->transferType == FLEXCAN_RXFIFO_USING_DMA))
        {
            state->callback(instance,
                            FLEXCAN_EVENT_DMA_COMPLETE,
                            FLEXCAN_MB_HANDLE_RXFIFO,
                            state);
        }
    }
#else
    state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state = FLEXCAN_MB_IDLE;
#endif

    /* Update status for receive by using fifo*/
    if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].isBlocking == true)
    {
        if (state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].state == FLEXCAN_MB_IDLE)
        {
            status_t status = OSIF_SemaPost(&state->mbs[FLEXCAN_MB_HANDLE_RXFIFO].mbSema);
            //DEV_ASSERT(status == STATUS_SUCCESS);
            (void)status;
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_InstallEventCallback
 * Description   : Installs a callback function for the IRQ handler.
 *
 * Implements    : FLEXCAN_DRV_InstallEventCallback_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_InstallEventCallback(uint8_t instance,
                                      flexcan_callback_t callback,
                                      void *callbackParam)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    flexcan_state_t * state = g_flexcanStatePtr[instance];

    state->callback = callback;
    state->callbackParam = callbackParam;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_InstallErrorCallback
 * Description   : Installs an error callback function for the IRQ handler and enables/disables
 *                 error interrupts.
 *
 * Implements    : FLEXCAN_DRV_InstallErrorCallback_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_InstallErrorCallback(uint8_t instance,
                                      flexcan_error_callback_t callback,
                                      void *callbackParam)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];
    flexcan_state_t * state = g_flexcanStatePtr[instance];

    state->error_callback = callback;
    state->errorCallbackParam = callbackParam;
    FLEXCAN_EnterFreezeMode(base);
    if (callback != NULL)
    {
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_ERR, true);
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_RX_WARNING, true);
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_TX_WARNING, true);
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_BUSOFF, true);
    }
    else
    {
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_ERR, false);
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_RX_WARNING, false);
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_TX_WARNING, false);
        FLEXCAN_SetErrIntCmd(base, FLEXCAN_INT_BUSOFF, false);
    }
    FLEXCAN_ExitFreezeMode(base);
}

#if FEATURE_CAN_HAS_PRETENDED_NETWORKING

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_ConfigPN
 * Description   : Configures Pretended Networking settings.
 *
 * Implements    : FLEXCAN_DRV_ConfigPN_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_ConfigPN(uint8_t instance, bool enable, const flexcan_pn_config_t *pnConfig)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);

    CAN_Type * base = g_flexcanBase[instance];

    FLEXCAN_EnterFreezeMode(base);

    if (enable)
    {
        FLEXCAN_ConfigPN(base, pnConfig);
#if FEATURE_CAN_HAS_SELF_WAKE_UP
        FLEXCAN_SetSelfWakeUp(base, false);
#endif
    }

    FLEXCAN_SetPN(base, enable);

    FLEXCAN_ExitFreezeMode(base);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetWMB
 * Description   : Extracts one of the frames which triggered the wake up event.
 *
 * Implements    : FLEXCAN_DRV_GetWMB_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_GetWMB(uint8_t instance, uint8_t wmbIndex, flexcan_msgbuff_t *wmb)
{
    //DEV_ASSERT(instance < CAN_INSTANCE_COUNT);
    //DEV_ASSERT(wmb != NULL);
    const CAN_Type *base  =  g_flexcanBase[instance];
    uint32_t *tmp, wmbData;

    tmp = (uint32_t *)&wmb->data[0];
    wmbData = base->WMB[wmbIndex].WMBn_D03;
    FlexcanSwapBytesInWord(wmbData, *tmp);

    tmp = (uint32_t *)&wmb->data[4];
    wmbData = base->WMB[wmbIndex].WMBn_D47;
    FlexcanSwapBytesInWord(wmbData, *tmp);

    wmb->cs = base->WMB[wmbIndex].WMBn_CS;

    if ((wmb->cs & CAN_CS_IDE_MASK) != 0U)
    {
        wmb->msgId = base->WMB[wmbIndex].WMBn_ID;
    }
    else
    {
        wmb->msgId = base->WMB[wmbIndex].WMBn_ID >> CAN_ID_STD_SHIFT;
    }

    wmb->dataLen = (uint8_t)((wmb->cs & CAN_CS_DLC_MASK) >> 16);
}
#endif /* FEATURE_CAN_HAS_PRETENDED_NETWORKING */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_CheckDsample
 * Description   : Check the Sample value
 *
 *END**************************************************************************/
static inline uint32_t FLEXCAN_CheckDsample(uint32_t tmpSample, uint32_t samplePoint)
{
    if (tmpSample > samplePoint)
    {
        return (tmpSample - samplePoint);
    }
    return (samplePoint - tmpSample);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_CheckdBitrate
 * Description   : Check the Bitrate value
 *
 *END**************************************************************************/
static inline uint32_t FLEXCAN_CheckdBitrate(uint32_t tmpBitrate, uint32_t bitrate)
{
    if (tmpBitrate > bitrate)
    {
        return (tmpBitrate - bitrate);
    }
    return (bitrate - tmpBitrate);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_CheckJumpwidth
 * Description   : Check the JumpWidth value
 *
 *END**************************************************************************/
static inline uint32_t FLEXCAN_CheckJumpwidth(uint32_t pseg1)
{
    if (pseg1 < FLEXCAN_RJW_MAX)
    {
        return pseg1;
    }
    return FLEXCAN_RJW_MAX;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ProccessTSeg
 * Description   : Calculate Tseg value
 *
 *END**************************************************************************/
static inline void FLEXCAN_ProccessTSeg(uint32_t * tSeg1, uint32_t * tSeg2)
{
    /* Adjust time segment 1 and time segment 2 */
    while ((*tSeg1 >= FLEXCAN_TSEG1_MAX) || (*tSeg2 < FLEXCAN_TSEG2_MIN))
    {
        *tSeg2 = *tSeg2 + 1U;
        *tSeg1 = *tSeg1 - 1U;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ProccessPSeg
 * Description   : Calculate Pseg value
 *
 *END**************************************************************************/
static inline void FLEXCAN_ProccessPSeg(uint32_t * tmpPropseg, uint32_t * tmpPseg1)
{
    while (*tmpPropseg <= 0U)
    {
        *tmpPropseg = *tmpPropseg + 1U;
        *tmpPseg1 = *tmpPseg1 - 1U;
    }
    
    while (*tmpPropseg >= FLEXCAN_PROPSEG_MAX)
    {
        *tmpPropseg = *tmpPropseg - 1U;
        *tmpPseg1 = *tmpPseg1 + 1U;
    }
}    
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_BitrateToTimeSeg
 * Description   : Converts a bitrate (kbit/s) in time segment values for
 *                 standard CAN frame.
 *
 *END**************************************************************************/
static void FLEXCAN_BitrateToTimeSeg(uint32_t bitrate,
                                     uint32_t clkFreq,
                                     flexcan_time_segment_t *timeSeg)
{
    uint32_t dBitrate, tmpBitrate, dBitrateMin, tmpPresdiv, tmpSample,
             dSampleMin, dSample, samplePoint, numTq, pseg1, pseg2, propseg,
             presdiv, tSeg1, tSeg2, tmpPseg1, tmpPseg2, tmpPropseg;

    presdiv = 0U;
    propseg = 0U;
    pseg1 = 0U;
    pseg2 = 0U;

    dSampleMin = 100U;
    dBitrateMin = 1000000U;
    samplePoint = 88U;

    for (tmpPresdiv = 0U; tmpPresdiv < FLEXCAN_PRESDIV_MAX; tmpPresdiv++) {

        /* Compute the number of time quanta in 1 bit time */
        numTq = clkFreq / ((tmpPresdiv + 1U) * bitrate);
        /* Compute the real bitrate resulted */
        tmpBitrate = clkFreq / ((tmpPresdiv + 1U) * numTq);

        /* The number of time quanta in 1 bit time must be lower than the one supported */
        if ((numTq >= FLEXCAN_NUM_TQ_MIN) && (numTq < FLEXCAN_NUM_TQ_MAX))
        {
            /* Compute time segments based on the value of the sampling point */
            tSeg1 = (numTq * samplePoint / 100U) - 1U;
            tSeg2 = numTq - 1U - tSeg1;

            /* Adjust time segment 1 and time segment 2 */
            FLEXCAN_ProccessTSeg(&tSeg1, &tSeg2);

            tmpPseg2 = tSeg2 - 1U;

            /* Start from pseg1 = pseg2 and adjust until propseg is valid */
            tmpPseg1 = tmpPseg2;
            tmpPropseg = tSeg1 - tmpPseg1 - 2U;

            FLEXCAN_ProccessPSeg(&tmpPropseg, &tmpPseg1);
            

            if (((tSeg1 >= FLEXCAN_TSEG1_MAX) || (tSeg2 >= FLEXCAN_TSEG2_MAX) || (tSeg2 < FLEXCAN_TSEG2_MIN) || (tSeg1 < FLEXCAN_TSEG1_MIN)) ||
               ((tmpPropseg >= FLEXCAN_PROPSEG_MAX) || (tmpPseg1 >= FLEXCAN_PSEG1_MAX) || (tmpPseg2 < FLEXCAN_PSEG2_MIN) || (tmpPseg2 >= FLEXCAN_PSEG2_MAX)))
            {
                continue;
            }

            tmpSample = ((tSeg1 + 1U) * 100U) / numTq;
            dSample = FLEXCAN_CheckDsample(tmpSample , samplePoint);
            dBitrate = FLEXCAN_CheckdBitrate(tmpBitrate , bitrate);

            if ((dBitrate < dBitrateMin) ||
                ((dBitrate == dBitrateMin) && (dSample < dSampleMin)))
            {
                dSampleMin = dSample;
                dBitrateMin = dBitrate;
                pseg1 = tmpPseg1;
                pseg2 = tmpPseg2;
                presdiv = tmpPresdiv;
                propseg = tmpPropseg;

                if ((dBitrate == 0U) && (dSample <= 1U))
                {
                    break;
                }
            }
        }
    }

    timeSeg->phaseSeg1 = pseg1;
    timeSeg->phaseSeg2 = pseg2;
    timeSeg->preDivider = presdiv;
    timeSeg->propSeg = propseg;
    timeSeg->rJumpwidth = FLEXCAN_CheckJumpwidth(pseg1);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DRV_GetDefaultConfig
 * Description   : Gets the default configuration structure
 *
 * Implements    : FLEXCAN_DRV_GetDefaultConfig_Activity
 *END**************************************************************************/
void FLEXCAN_DRV_GetDefaultConfig(flexcan_user_config_t *config)
{
    /* Checks input parameter. */
    //DEV_ASSERT(config != NULL);

    uint32_t clkFreq;
    flexcan_time_segment_t timeSeg;

    /* Get the PE clock frequency */
    (void) CLOCK_SYS_GetFreq(FEATURE_CAN_PE_OSC_CLK_NAME, &clkFreq);
    /* Time segments computed for PE bitrate = 500 Kbit/s, sample point = 87.5 */
    FLEXCAN_BitrateToTimeSeg(500000U, clkFreq, &timeSeg);

    /* Maximum number of message buffers */
    config->max_num_mb = 16;
    /* Rx FIFO is disabled */
    config->is_rx_fifo_needed = false;
    /* Number of Rx FIFO ID filters */
    config->num_id_filters = FLEXCAN_RX_FIFO_ID_FILTERS_8;
    /* Normal operation mode */
    config->flexcanMode = FLEXCAN_NORMAL_MODE;
#if FEATURE_CAN_HAS_PE_CLKSRC_SELECT
    /* Protocol engine clock is System Oscillator div 2 */
    config->pe_clock = FLEXCAN_CLK_SOURCE_OSC;
#endif
    /* Time segments for the arbitration phase */
    config->bitrate = timeSeg;
#if FEATURE_CAN_HAS_FD
    /* Payload size */
    config->payload = FLEXCAN_PAYLOAD_SIZE_8;
    /* Flexible data rate is disabled */
    config->fd_enable = false;
    /* Time segments for the data phase of FD frames */
    config->bitrate_cbt = timeSeg;
#endif
    /* Rx FIFO transfer type */
    config->transfer_type = FLEXCAN_RXFIFO_USING_INTERRUPTS;
#if FEATURE_CAN_HAS_DMA_ENABLE
    /* Rx FIFO DMA channel */
    config->rxFifoDMAChannel = 0U;
#endif
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
