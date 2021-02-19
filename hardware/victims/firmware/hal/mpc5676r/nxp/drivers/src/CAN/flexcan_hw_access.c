/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
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
 * @file flexcan_hw_access.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 15.5, Return statement before end of function.
 * The return statement before end of function is used for simpler code structure
 * and better readability.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.3, cast performed between a pointer to
 * object type and a pointer to a different object type
 * The cast is used for casting a bytes buffer into an words buffer in order to
 * optimize copying data to/from the message buffer.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 2.2, Last value assigned to variable not
 * used
 * The values of the reported variables are changed by the REV_BYTES_32 macro,
 * which uses inline assembly.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be
 * performed between a pointer to object and an integer type.
 * The cast is required as CAN instance base addresses are defined as unsigned
 * integers in the header file, but the registers are accessed via pointers to
 * structures.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, A conversion should not be
 * performed between a pointer to object and an integer type.
 * The cast is required as CAN instance base addresses are defined as unsigned
 * integers in the header file, but the registers are accessed via pointers to
 * structures.
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
 * Violates MISRA 2012 Advisory Rule 8.7, Variable not defined with external linkage
 * The variables are defined in the driver header file to make transition to other
 * platforms easier.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Directive 4.9, Function-like macro
 * Function-like macros are used instead of inline functions in order to ensure
 * that the performance will not be decreased if the functions will not be
 * inlined by the compiler.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3, Taking address of near auto variable
 * The code is not dynamically linked. An absolute stack address is obtained when
 * taking the address of the near auto variable. A source of error in writing
 * dynamic code is that the stack segment may be different from the data segment.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 18.1, Possible creation of out-of-bounds pointer.
 * The function FlexcanSwapBytesInWordIndex guards the index to don't exceed the MB
 * space
 *
 */

#include "flexcan_hw_access.h"


/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
 
#define FLEXCAN_ALL_INT                                  (0x3B0006U)    /*!< Masks for wakeup, error, bus off*/
                                                                        /*! interrupts*/

/* CAN FD extended data length DLC encoding */
#define CAN_DLC_VALUE_12_BYTES                   9U
#define CAN_DLC_VALUE_16_BYTES                   10U
#define CAN_DLC_VALUE_20_BYTES                   11U
#define CAN_DLC_VALUE_24_BYTES                   12U
#define CAN_DLC_VALUE_32_BYTES                   13U
#define CAN_DLC_VALUE_48_BYTES                   14U
#define CAN_DLC_VALUE_64_BYTES                   15U

#define RxFifoFilterTableOffset         0x18U

#define FlexCanRxFifoAcceptRemoteFrame   1UL
#define FlexCanRxFifoAcceptExtFrame      1UL

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static uint8_t FLEXCAN_ComputeDLCValue(uint8_t payloadSize);
static uint8_t FLEXCAN_ComputePayloadSize(uint8_t dlcValue);
static void FLEXCAN_ClearRAM(CAN_Type * base);
#if (defined(CPU_S32K116) || defined(CPU_S32K118))
static uint32_t FLEXCAN_DeserializeUint32(const uint8_t *buffer);

static uint32_t FLEXCAN_DeserializeUint32(const uint8_t *buffer)
{
    uint32_t value = 0;

    #ifdef CORE_LITTLE_ENDIAN
        value |= (uint32_t)buffer[3] << 24U;
        value |= (uint32_t)buffer[2] << 16U;
        value |= (uint32_t)buffer[1] << 8U;
        value |= (uint32_t)buffer[0];
    #elif (defined CORE_BIG_ENDIAN)
        value |= (uint32_t)buffer[0] << 24U;
        value |= (uint32_t)buffer[1] << 16U;
        value |= (uint32_t)buffer[2] << 8U;
        value |= (uint32_t)buffer[3];
    #endif
        return value;
}
inline static uint32_t FLEXCAN_DataTransferTxMsgBuff(volatile uint32_t *flexcan_mb_data_32,
												 const flexcan_msgbuff_code_status_t *cs,
												 const uint8_t *msgData);
#endif

/* Determines the RxFIFO Filter element number */
#define RxFifoFilterElementNum(x) (((x) + 1U) * 8U)

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_GetRxFifoMask
 * Description   : Calculate the Global Mask as format ID type in FIFO Mode.
 * Based on the ID format type and Mask Type will calculate the and set accordingly
 * the Rx FIFO Individual\Global Mask register.
 *
 *END**************************************************************************/
uint32_t FLEXCAN_GetRxFifoMask(	 flexcan_msgbuff_id_type_t id_type,
                                 flexcan_rx_fifo_id_element_format_t formatType,
                                 uint32_t mask)
{
    uint32_t val=0;
    
    switch (formatType)
	    {
		case (FLEXCAN_RX_FIFO_ID_FORMAT_A):
			/* Set RTR bit encoded as bit 31 and IDE bit encoded as bit 30 in mask */
			val = mask & ((1UL<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT) | (1UL<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT));
			if (id_type == FLEXCAN_MSG_ID_STD)
			{
				/* Set standard global mask for RX FIFO and IDE will be 1 and check the FIFO filter ide */
				val |= ((mask << FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_STD_SHIFT) & FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_STD_MASK);

			}
			else if (id_type == FLEXCAN_MSG_ID_EXT)
			{
				/* Set extended global mask for RX FIFO and IDE will be 0 and don't check the FIFO filter ide */
				val |= ((mask << FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_EXT_SHIFT) & FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_EXT_MASK);
			}
			else {
				/* Should not get here */
			}
		break;
		case (FLEXCAN_RX_FIFO_ID_FORMAT_B):
			/* Set RTR bit encoded as bit 31 and IDE bit encoded as bit 30 in mask */
			val = mask & ((1UL<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT) | (1UL<<FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT));
			if (id_type == FLEXCAN_MSG_ID_STD)
			{
				/* Set standard global mask for RX FIFO  */
				val |= ((mask & FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_STD_MASK) << FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_STD_SHIFT1);

			}
			else if (id_type == FLEXCAN_MSG_ID_EXT)
			{
				/* Set extended global mask for RX FIFO  */
				val |= ((mask & FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_MASK1) <<  FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_SHIFT1);
			}
			else {
				/* Should not get here */
			}
		break;
		case (FLEXCAN_RX_FIFO_ID_FORMAT_C):
			if ((id_type == FLEXCAN_MSG_ID_EXT) || (id_type == FLEXCAN_MSG_ID_STD))
			{
				val |= ((mask & FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_MASK) << FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT1);
			}
			else
			{
				/* Should not get here */
			}
		break;
		case (FLEXCAN_RX_FIFO_ID_FORMAT_D):
				/* Format not supported.*/
		break;
        default:
            /* Should not get here */
        break;
	    }/* End Switch */

    return val;
}

/*FUNCTION**********************************************************************
 *
 * Function Name: FLEXCAN_ComputeDLCValue
 * Description  : Computes the DLC field value, given a payload size (in bytes).
 *
 *END**************************************************************************/
static uint8_t FLEXCAN_ComputeDLCValue(
        uint8_t payloadSize)
{
    uint32_t ret = 0xFFU;     				/* 0,  1,  2,  3,  4,  5,  6,  7,  8, */
    static const uint8_t payload_code[65] = { 0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U,
    						/* 9 to 12 payload have DLC Code 12 Bytes */
    					CAN_DLC_VALUE_12_BYTES, CAN_DLC_VALUE_12_BYTES, CAN_DLC_VALUE_12_BYTES, CAN_DLC_VALUE_12_BYTES,
							/* 13 to 16 payload have DLC Code 16 Bytes */
						CAN_DLC_VALUE_16_BYTES, CAN_DLC_VALUE_16_BYTES, CAN_DLC_VALUE_16_BYTES, CAN_DLC_VALUE_16_BYTES,
    						/* 17 to 20 payload have DLC Code 20 Bytes */
						CAN_DLC_VALUE_20_BYTES, CAN_DLC_VALUE_20_BYTES, CAN_DLC_VALUE_20_BYTES, CAN_DLC_VALUE_20_BYTES,
							/* 21 to 24 payload have DLC Code 24 Bytes */
						CAN_DLC_VALUE_24_BYTES, CAN_DLC_VALUE_24_BYTES, CAN_DLC_VALUE_24_BYTES, CAN_DLC_VALUE_24_BYTES,
							/* 25 to 32 payload have DLC Code 32 Bytes */
						CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES,
						CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES, CAN_DLC_VALUE_32_BYTES,
							/* 33 to 48 payload have DLC Code 48 Bytes */
						CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
						CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
						CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
						CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES, CAN_DLC_VALUE_48_BYTES,
							/* 49 to 64 payload have DLC Code 64 Bytes */
						CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES,
						CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES,
						CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES,
						CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES, CAN_DLC_VALUE_64_BYTES };

    if (payloadSize <= 64U)
    {
    	ret = payload_code[payloadSize];
    }
    else
    {
        /* The argument is not a valid payload size will return 0xFF*/
    }

    return (uint8_t)ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ComputePayloadSize
 * Description   : Computes the maximum payload size (in bytes), given a DLC
 * field value.
 *
 *END**************************************************************************/
static uint8_t FLEXCAN_ComputePayloadSize(
        uint8_t dlcValue)
{
    uint8_t ret=0U;

    if (dlcValue <= 8U)
    {
        ret = dlcValue;
    }
#if FEATURE_CAN_HAS_FD
    else
    {
        switch (dlcValue) {
        case CAN_DLC_VALUE_12_BYTES:
            ret = 12U;
            break;
        case CAN_DLC_VALUE_16_BYTES:
            ret = 16U;
            break;
        case CAN_DLC_VALUE_20_BYTES:
            ret = 20U;
            break;
        case CAN_DLC_VALUE_24_BYTES:
            ret = 24U;
            break;
        case CAN_DLC_VALUE_32_BYTES:
            ret = 32U;
            break;
        case CAN_DLC_VALUE_48_BYTES:
            ret = 48U;
            break;
        case CAN_DLC_VALUE_64_BYTES:
            ret = 64U;
            break;
        default:
            /* The argument is not a valid DLC size */
            break;
        }
    }
#endif /* FEATURE_CAN_HAS_FD */
    return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_GetMaxMbNum
 * Description   : Computes the maximum RAM size occupied by MBs.
 *
 *END**************************************************************************/
uint32_t FLEXCAN_GetMaxMbNum(const CAN_Type * base)
{
    #if 0 
    uint32_t i, ret = 0;
    static CAN_Type * const flexcanBase[] = CAN_BASE_PTRS;
    static const uint32_t maxMbNum[] = FEATURE_CAN_MAX_MB_NUM_ARRAY;

    for (i = 0; i < CAN_INSTANCE_COUNT; i++)
    {
        if (base == flexcanBase[i])
        {
            ret = maxMbNum[i];
        }
    }
    #endif
    return 64u;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ClearRAM
 * Description   : Clears FlexCAN memory positions that require initialization.
 *
 *END**************************************************************************/
static void FLEXCAN_ClearRAM(CAN_Type * base)
{
    uint32_t databyte;
    uint32_t RAM_size = 64u * 4U;
    uint32_t RXIMR_size = 64u;
    volatile uint32_t *RAM = base->RAMn;

    /* Clear MB region */
    for (databyte = 0; databyte < RAM_size; databyte+=4) {
        RAM[databyte] = 0x0;
    }

    RAM = base->RXIMR;

    /* Clear RXIMR region */
    for (databyte = 0; databyte < RXIMR_size; databyte++) {
        RAM[databyte] = 0x0;
    }

#if FEATURE_CAN_HAS_MEM_ERR_DET
    /* Set WRMFRZ bit in CTRL2 Register to grant write access to memory */
    base->CTRL2 = (base->CTRL2 & ~CAN_CTRL2_WRMFRZ_MASK) | CAN_CTRL2_WRMFRZ(1U);

    uint32_t ram_addr = (uint32_t)base + (uint32_t)FEATURE_CAN_RAM_OFFSET;
    RAM = (volatile uint32_t *)ram_addr;

#if FEATURE_CAN_ECC_CLEAR_RXMASKS
    /* Clear RXMGMASK, RXFGMASK, RX14MASK, RX15MASK RAM mapping */
    base->RXMGMASK = 0U;
    base->RXFGMASK = 0U;
    base->RX14MASK = 0U;
    base->RX15MASK = 0U;
#endif

#if FEATURE_CAN_HAS_FD
    /* Clear SMB FD region */
    for (databyte = FEATURE_CAN_SMB_FD_START_ADDRESS_OFFSET; databyte < (uint32_t)FEATURE_CAN_SMB_FD_END_ADDRESS_OFFSET; databyte++) {
            RAM[databyte] = 0;
    }
#endif

    /* Clear WRMFRZ bit in CTRL2 Register to restrict write access to memory */
    base->CTRL2 = (base->CTRL2 & ~CAN_CTRL2_WRMFRZ_MASK) | CAN_CTRL2_WRMFRZ(0U);
#endif
}

/*******************************************************************************
 * Code
 ******************************************************************************/
 
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_GetMsgBuffRegion
 * Description   : Returns the start of a MB area, based on its index.
 *
 *END**************************************************************************/
volatile uint32_t* FLEXCAN_GetMsgBuffRegion(
        CAN_Type * base,
        uint32_t msgBuffIdx)
{
#if FEATURE_CAN_HAS_FD
    uint8_t payload_size = FLEXCAN_GetPayloadSize(base);
#else
    uint8_t payload_size = 8U;
#endif

    uint8_t arbitration_field_size = 8U;
    uint32_t ramBlockSize = 512U;
    uint32_t ramBlockOffset;

    uint8_t mb_size = (uint8_t)(payload_size + arbitration_field_size);
    uint8_t maxMbNum = (uint8_t)(ramBlockSize / mb_size);

    ramBlockOffset = 128U * (msgBuffIdx / (uint32_t)maxMbNum);

    /* Multiply the MB index by the MB size (in words) */
    uint32_t mb_index = ramBlockOffset + ((msgBuffIdx % (uint32_t)maxMbNum) * ((uint32_t)mb_size >> 2U));

    return &(base->RAMn[mb_index]);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_Enable
 * Description   : Enable FlexCAN module.
 * This function will enable FlexCAN module.
 *
 *END**************************************************************************/
void FLEXCAN_Enable(CAN_Type * base)
{
    /* Check for low power mode */
    if(((base->MCR & CAN_MCR_LPMACK_MASK) >> CAN_MCR_LPMACK_SHIFT) == 1U)
    {
        /* Enable clock */
        base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_CLKSRC_MASK) | CAN_CTRL1_CLKSRC(0u);
        base->MCR = (base->MCR & ~CAN_MCR_MDIS_MASK) | CAN_MCR_MDIS(0U);
        base->MCR = (base->MCR & ~CAN_MCR_FRZ_MASK) | CAN_MCR_FRZ(0U);
        base->MCR = (base->MCR & ~CAN_MCR_HALT_MASK) | CAN_MCR_HALT(0U);
        /* Wait until enabled */
        while (((base->MCR & CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT) != 0U) {}

    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_Disable
 * Description   : Disable FlexCAN module.
 * This function will disable FlexCAN module.
 *
 *END**************************************************************************/
void FLEXCAN_Disable(CAN_Type * base)
{
    /* To access the memory mapped registers */
    /* Entre disable mode (hard reset). */
    if(((base->MCR & CAN_MCR_MDIS_MASK) >> CAN_MCR_MDIS_SHIFT) == 0U)
    {
        /* Clock disable (module) */
        base->MCR = (base->MCR & ~CAN_MCR_MDIS_MASK) | CAN_MCR_MDIS(1U);

        /* Wait until disable mode acknowledged */
        while (((base->MCR & CAN_MCR_LPMACK_MASK) >> CAN_MCR_LPMACK_SHIFT) == 0U) {}
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_Init
 * Description   : Initialize FlexCAN module.
 * This function will reset FlexCAN module, set maximum number of message
 * buffers, initialize all message buffers as inactive, enable RX FIFO
 * if needed, mask all mask bits, and disable all MB interrupts.
 *
 *END**************************************************************************/
void FLEXCAN_Init(CAN_Type * base)
{
    /* Reset the FLEXCAN */
    base->MCR = (base->MCR & ~CAN_MCR_SOFTRST_MASK) | CAN_MCR_SOFTRST(1U);

    /* Wait for reset cycle to complete */
    while (((base->MCR & CAN_MCR_SOFTRST_MASK) >> CAN_MCR_SOFTRST_SHIFT) != 0U) {}

    /* Enable abort */
#ifndef ERRATA_E9527
    /* Avoid Abort Transmission, use Inactive MB */
    base->MCR = (base->MCR & ~CAN_MCR_AEN_MASK) | CAN_MCR_AEN(1U);
#endif

    /* Clear FlexCAN memory */
    FLEXCAN_ClearRAM(base);

    /* Rx global mask*/
    (base->RXMGMASK) = (uint32_t)(CAN_RXMGMASK_MG_MASK);

    /* Rx reg 14 mask*/
    (base->RX14MASK) =  (uint32_t)(CAN_RX14MASK_RX14M_MASK);

    /* Rx reg 15 mask*/
    (base->RX15MASK) = (uint32_t)(CAN_RX15MASK_RX15M_MASK);

    /* Disable all MB interrupts */
    (base->IMASK1) = 0x0;
    /* Clear all MB interrupt flags */
    (base->IFLAG1) = CAN_IMASK1_BUF31TO0M_MASK;
#if FEATURE_CAN_MAX_MB_NUM > 32U
    if (FLEXCAN_GetMaxMbNum(base) > 32U)
    {
		(base->IMASK2) = 0x0;
		(base->IFLAG2) = CAN_IMASK2_BUF63TO32M_MASK;
	}
#endif	
#if FEATURE_CAN_MAX_MB_NUM > 64U
    if (FLEXCAN_GetMaxMbNum(base) > 64U)
    {
    	(base->IMASK3) = 0x0;
    	(base->IFLAG3) = CAN_IMASK3_BUF95TO64M_MASK;
    }
#endif	
    /* Clear all error interrupt flags */
    (base->ESR1) = FLEXCAN_ALL_INT;
}

#if (defined(CPU_S32K116) || defined(CPU_S32K118))
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_DataTransferTxMsgBuff
 * Description   : Transfer Payload data in message buffer, in case of unaligned
 * buffer it makes a byte alignment.
 * This function is private.
 *
 *END**************************************************************************/
inline static uint32_t FLEXCAN_DataTransferTxMsgBuff(volatile uint32_t *flexcan_mb_data_32,
												 const flexcan_msgbuff_code_status_t *cs,
												 const uint8_t *msgData)
{
    uint32_t databyte;
    const uint32_t *msgData_32 = (const uint32_t *)msgData;

    /* Check if the buffer address is aligned */
    if (((uint32_t)msgData_32&0x3U) != 0U)
    {
        for (databyte = 0U; databyte < (cs->dataLen & ~3U); databyte += 4U)
        {
            uint32_t x = FLEXCAN_DeserializeUint32(&msgData[databyte]);
            FlexcanSwapBytesInWord(x, flexcan_mb_data_32[databyte >> 2U]);
        }
    }
    else
    {
        for (databyte = 0U; databyte < (cs->dataLen & ~3U); databyte += 4U)
        {
            FlexcanSwapBytesInWord(msgData_32[databyte >> 2U], flexcan_mb_data_32[databyte >> 2U]);
        }
    }
    return databyte;
}
#endif /* defined(CPU_S32K116) || defined(CPU_S32K118) */

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetTxMsgBuff
 * Description   : Configure a message buffer for transmission.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will copy user's buffer into the
 * message buffer data area and configure the message buffer as required for
 * transmission.
 *
 *END**************************************************************************/
status_t FLEXCAN_SetTxMsgBuff(
    CAN_Type * base,
    uint32_t msgBuffIdx,
    const flexcan_msgbuff_code_status_t *cs,
    uint32_t msgId,
    const uint8_t *msgData,
    const bool isRemote)
{
    //DEV_ASSERT(cs != NULL);

    uint32_t val1, val2 = 1;
    uint32_t flexcan_mb_config = 0;
    uint32_t databyte;
    uint8_t dlc_value;
    status_t stat = STATUS_SUCCESS;

    volatile uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, msgBuffIdx);

    volatile uint32_t *flexcan_mb_id   = &flexcan_mb[1];
    volatile uint8_t  *flexcan_mb_data = (volatile uint8_t *)(&flexcan_mb[2]);
    volatile uint32_t *flexcan_mb_data_32 = &flexcan_mb[2];
    const uint32_t *msgData_32 = (const uint32_t *)msgData;

    if (msgBuffIdx > (((base->MCR) & CAN_MCR_MAXMB_MASK) >> CAN_MCR_MAXMB_SHIFT) )
    {
        stat = STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    /* Check if RX FIFO is enabled*/
    if (((base->MCR & CAN_MCR_RFEN_MASK) >> CAN_MCR_RFEN_SHIFT) != 0U)
    {
        /* Get the number of RX FIFO Filters*/
        val1 = (((base->CTRL2) & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        /* Get the number if MBs occupied by RX FIFO and ID filter table*/
        /* the Rx FIFO occupies the memory space originally reserved for MB0-5*/
        /* Every number of RFFN means 8 number of RX FIFO filters*/
        /* and every 4 number of RX FIFO filters occupied one MB*/
        val2 = RxFifoOcuppiedLastMsgBuff(val1);

        if (msgBuffIdx <= val2) {
            stat =  STATUS_CAN_BUFF_OUT_OF_RANGE;
        }
    }

    if (stat == STATUS_SUCCESS)
    {
#if FEATURE_CAN_HAS_FD
        /* Make sure the BRS bit will not be ignored */
        if (FLEXCAN_IsFDEnabled(base) && cs->enable_brs)
        {
            base->FDCTRL = (base->FDCTRL & ~CAN_FDCTRL_FDRATE_MASK) | CAN_FDCTRL_FDRATE(1U);
        }
        /* Check if the Payload Size is smaller than the payload configured */
        //DEV_ASSERT((uint8_t)cs->dataLen <= FLEXCAN_GetPayloadSize(base));
#else
        //DEV_ASSERT((uint8_t)cs->dataLen <= 8U);
#endif
        /* Compute the value of the DLC field */
        dlc_value = FLEXCAN_ComputeDLCValue((uint8_t)cs->dataLen);

        /* Copy user's buffer into the message buffer data area */
        if (msgData != NULL)
        {
#if (defined(CPU_S32K116) || defined(CPU_S32K118))
            (void) msgData_32;
            databyte = FLEXCAN_DataTransferTxMsgBuff( flexcan_mb_data_32, cs, msgData);
#else
            for (databyte = 0; databyte < (cs->dataLen & ~3U); databyte += 4U)
			{
				FlexcanSwapBytesInWord(msgData_32[databyte >> 2U], flexcan_mb_data_32[databyte >> 2U]);
			}
#endif
            for ( ; databyte < cs->dataLen; databyte++)
            {
                flexcan_mb_data[FlexcanSwapBytesInWordIndex(databyte)] =  msgData[databyte];
            }
#if FEATURE_CAN_HAS_FD
            uint8_t payload_size = FLEXCAN_ComputePayloadSize(dlc_value);
            /* Add padding, if needed */
            for (databyte = cs->dataLen; databyte < payload_size; databyte++)
            {
                flexcan_mb_data[FlexcanSwapBytesInWordIndex(databyte)] = cs->fd_padding;
            }
#endif /* FEATURE_CAN_HAS_FD */
        }

        /* Clean up the arbitration field area */
        *flexcan_mb = 0;
        *flexcan_mb_id = 0;

        /* Set the ID according the format structure */
        if (cs->msgIdType == FLEXCAN_MSG_ID_EXT)
        {
            /* ID [28-0] */
            *flexcan_mb_id &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
            *flexcan_mb_id |= (msgId & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));

            /* Set IDE */
            flexcan_mb_config |= CAN_CS_IDE_MASK;

            /* Clear SRR bit */
            flexcan_mb_config &= ~CAN_CS_SRR_MASK;
        }
        if(cs->msgIdType == FLEXCAN_MSG_ID_STD)
        {
            /* ID[28-18] */
            *flexcan_mb_id &= ~CAN_ID_STD_MASK;
            *flexcan_mb_id |= (msgId << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK;

            /* make sure IDE and SRR are not set */
            flexcan_mb_config &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);
        }

        /* Set the length of data in bytes */
        flexcan_mb_config &= ~CAN_CS_DLC_MASK;
        flexcan_mb_config |= ((uint32_t)dlc_value << CAN_CS_DLC_SHIFT) & CAN_CS_DLC_MASK;

        /* Set MB CODE */
        if (cs->code != (uint32_t)FLEXCAN_TX_NOT_USED)
        {
            if (cs->code == (uint32_t)FLEXCAN_TX_REMOTE)
            {
                /* Set RTR bit */
                flexcan_mb_config |= CAN_CS_RTR_MASK;
            }
            else
            {
                if (isRemote == true)
                {
                    /* Set RTR bit */
                    flexcan_mb_config |= CAN_CS_RTR_MASK;
                }
            }

            /* Reset the code */
            flexcan_mb_config &= ~CAN_CS_CODE_MASK;

            /* Set the code */
            if (cs->fd_enable)
            {
                flexcan_mb_config |= ((cs->code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK) | CAN_MB_EDL_MASK;
            }
            else
            {
                flexcan_mb_config |= (cs->code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
            }

            if (cs->enable_brs)
            {
                flexcan_mb_config |= CAN_MB_BRS_MASK;
            }

            *flexcan_mb |= flexcan_mb_config;
        }
    }

    return stat;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_AbortTxMsgBuff
 * Description   : Writes the abort code into the CODE field of the requested
 * Tx message buffer.
 *
 *END**************************************************************************/
void FLEXCAN_AbortTxMsgBuff(CAN_Type * base, uint32_t msgBuffIdx)
{
    uint32_t flexcan_mb_config = 0;
    uint32_t code = FLEXCAN_TX_ABORT;

    volatile uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, msgBuffIdx);
    flexcan_mb_config = *flexcan_mb;

    /* Reset the code */
    flexcan_mb_config &= (~CAN_CS_CODE_MASK);

    /* Write the abort code */
#ifdef ERRATA_E9527
    /* Use instead Abort Code the Inactive Code */
    code = FLEXCAN_TX_INACTIVE;
#endif
    flexcan_mb_config |= (code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;

    *flexcan_mb = flexcan_mb_config;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ResetRxMsgBuff
 * Description   : Writes the Inactive Rx code into the CODE field of the requested
 * Rx message buffer and restore the MB to active Rx. This will force even the unlock of the RxMB.
 *
 *END**************************************************************************/
void FLEXCAN_ResetRxMsgBuff(CAN_Type * base, uint32_t msgBuffIdx)
{
	uint32_t flexcan_mb_config = 0;
	uint32_t code = FLEXCAN_RX_INACTIVE;

	volatile uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, msgBuffIdx);
	flexcan_mb_config = *flexcan_mb;

	/* Reset the code and unlock the MB */
	flexcan_mb_config &= (~CAN_CS_CODE_MASK);
    flexcan_mb_config |= (code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
    *flexcan_mb = flexcan_mb_config;

    /* Reconfigure The MB as left by RxMBconfig */
    code = FLEXCAN_RX_EMPTY;
    flexcan_mb_config &= (~CAN_CS_CODE_MASK);
    flexcan_mb_config |= (code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
    *flexcan_mb = flexcan_mb_config;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetRxMsgBuff
 * Description   : Configure a message buffer for receiving.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will configure the message buffer as
 * required for receiving.
 *
 *END**************************************************************************/
status_t FLEXCAN_SetRxMsgBuff(
    CAN_Type * base,
    uint32_t msgBuffIdx,
    const flexcan_msgbuff_code_status_t *cs,
    uint32_t msgId)
{
    //DEV_ASSERT(cs != NULL);

    uint32_t val1, val2 = 1;

    volatile uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, msgBuffIdx);
    volatile uint32_t *flexcan_mb_id = &flexcan_mb[1];
    status_t stat = STATUS_SUCCESS;

    if (msgBuffIdx > (((base->MCR) & CAN_MCR_MAXMB_MASK) >> CAN_MCR_MAXMB_SHIFT))
    {
        stat = STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    /* Check if RX FIFO is enabled */
    if (((base->MCR & CAN_MCR_RFEN_MASK) >> CAN_MCR_RFEN_SHIFT) != 0U)
    {
        /* Get the number of RX FIFO Filters*/
        val1 = (((base->CTRL2) & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);
        /* Get the number if MBs occupied by RX FIFO and ID filter table*/
        /* the Rx FIFO occupies the memory space originally reserved for MB0-5*/
        /* Every number of RFFN means 8 number of RX FIFO filters*/
        /* and every 4 number of RX FIFO filters occupied one MB*/
        val2 = RxFifoOcuppiedLastMsgBuff(val1);

        if (msgBuffIdx <= val2) {
            stat =  STATUS_CAN_BUFF_OUT_OF_RANGE;
        }
    }

    if (stat == STATUS_SUCCESS)
    {
        /* Clean up the arbitration field area */
        *flexcan_mb = 0;
        *flexcan_mb_id = 0;

        /* Set the ID according the format structure */
        if (cs->msgIdType == FLEXCAN_MSG_ID_EXT)
        {
            /* Set IDE */
            *flexcan_mb |= CAN_CS_IDE_MASK;

            /* Clear SRR bit */
            *flexcan_mb &= ~CAN_CS_SRR_MASK;

            /* ID [28-0] */
            *flexcan_mb_id &= ~(CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
            *flexcan_mb_id |= (msgId & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));
        }
        if (cs->msgIdType == FLEXCAN_MSG_ID_STD)
        {
            /* Make sure IDE and SRR are not set */
            *flexcan_mb &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK);

            /* ID[28-18] */
            *flexcan_mb_id &= ~CAN_ID_STD_MASK;
            *flexcan_mb_id |= (msgId << CAN_ID_STD_SHIFT) & CAN_ID_STD_MASK;
        }

        /* Set MB CODE */
        if (cs->code != (uint32_t)FLEXCAN_RX_NOT_USED)
        {
             *flexcan_mb &= ~CAN_CS_CODE_MASK;
             *flexcan_mb |= (cs->code << CAN_CS_CODE_SHIFT) & CAN_CS_CODE_MASK;
        }
    }

    return stat;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_GetMsgBuff
 * Description   : Get a message buffer field values.
 * This function will first check if RX FIFO is enabled. If RX FIFO is enabled,
 * the function will make sure if the MB requested is not occupied by RX FIFO
 * and ID filter table. Then this function will get the message buffer field
 * values and copy the MB data field into user's buffer.
 *
 *END**************************************************************************/
void FLEXCAN_GetMsgBuff(
    CAN_Type * base,
    uint32_t msgBuffIdx,
    flexcan_msgbuff_t *msgBuff)
{
    //DEV_ASSERT(msgBuff != NULL);

    uint8_t i;

    volatile const uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, msgBuffIdx);
    volatile const uint32_t *flexcan_mb_id   = &flexcan_mb[1];
    volatile const uint8_t  *flexcan_mb_data = (volatile const uint8_t *)(&flexcan_mb[2]);
    volatile const uint32_t *flexcan_mb_data_32 = &flexcan_mb[2];
    uint32_t *msgBuff_data_32 = (uint32_t *)(msgBuff->data);
    uint32_t mbWord;

    uint8_t flexcan_mb_dlc_value = (uint8_t)(((*flexcan_mb) & CAN_CS_DLC_MASK) >> 16);
    uint8_t payload_size = FLEXCAN_ComputePayloadSize(flexcan_mb_dlc_value);

#if FEATURE_CAN_HAS_FD
    if (payload_size > FLEXCAN_GetPayloadSize(base))
    {
        payload_size = FLEXCAN_GetPayloadSize(base);
    }
#endif /* FEATURE_CAN_HAS_FD */

	msgBuff->dataLen = payload_size;

    /* Get a MB field values */
    msgBuff->cs = *flexcan_mb;
    if ((msgBuff->cs & CAN_CS_IDE_MASK) != 0U)
    {
        msgBuff->msgId = (*flexcan_mb_id);
    }
    else
    {
        msgBuff->msgId = (*flexcan_mb_id) >> CAN_ID_STD_SHIFT;
    }
#if (defined(CPU_S32K116) || defined(CPU_S32K118))
    /* Check if the buffer address is aligned */
    if (((uint32_t)msgBuff_data_32&0x3U) != 0U)
    {
        /* Copy MB data field into user's buffer */
        for (i = 0U ; i < (payload_size & ~3U); i += 4U)
        {
            mbWord = flexcan_mb_data_32[i >> 2U];
            uint8_t index;
            uint32_t x;
            const uint8_t * p;
            FlexcanSwapBytesInWord(mbWord, x);
            p = (uint8_t *)&x;
            for (index=0; index < 4U; index++)
            {
                msgBuff->data[i+index]= p[index];
            }
        }
    }
    else
    {
        for (i = 0U ; i < (payload_size & ~3U); i += 4U)
        {
            mbWord = flexcan_mb_data_32[i >> 2U];
            FlexcanSwapBytesInWord(mbWord, msgBuff_data_32[i >> 2U]);
        }
    }
#else
    for (i = 0U ; i < (payload_size & ~3U); i += 4U)
    {
        mbWord = flexcan_mb_data_32[i >> 2U];
        FlexcanSwapBytesInWord(mbWord, msgBuff_data_32[i >> 2U]);
    }
#endif
    for ( ; i < payload_size ; i++)
    {	/* Max allowed value for index is 63 */
        msgBuff->data[i] = flexcan_mb_data[FlexcanSwapBytesInWordIndex(i)];
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_LockRxMsgBuff
 * Description   : Lock the RX message buffer.
 * This function will lock the RX message buffer.
 *
 *END**************************************************************************/
void FLEXCAN_LockRxMsgBuff(
    CAN_Type * base,
    uint32_t msgBuffIdx)
{
    volatile const uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, msgBuffIdx);

    /* Lock the mailbox by reading it */
    (void)*flexcan_mb;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_EnableRxFifo
 * Description   : Enable Rx FIFO feature.
 * This function will enable the Rx FIFO feature.
 *
 *END**************************************************************************/
status_t FLEXCAN_EnableRxFifo(CAN_Type * base, uint32_t numOfFilters)
{
    uint32_t i;
    uint16_t noOfMbx= (uint16_t)FLEXCAN_GetMaxMbNum(base);
    status_t stat = STATUS_SUCCESS;

#if FEATURE_CAN_HAS_FD
    /* RxFIFO cannot be enabled if FD is enabled */
    if (FLEXCAN_IsFDEnabled(base))
    {
        stat = STATUS_ERROR;
    }

    if (stat == STATUS_SUCCESS)
#endif
    {
        /* Enable RX FIFO */
        base->MCR = (base->MCR & ~CAN_MCR_RFEN_MASK) | CAN_MCR_RFEN(1U);
        /* Set the number of the RX FIFO filters needed */
        base->CTRL2 = (base->CTRL2 & ~CAN_CTRL2_RFFN_MASK) | ((numOfFilters << CAN_CTRL2_RFFN_SHIFT) & CAN_CTRL2_RFFN_MASK);
        /* RX FIFO global mask, take in consideration all filter fields*/
        //(base->RXFGMASK) = CAN_RXFGMASK_FGM_MASK;

        for (i = 0; i < noOfMbx; i++)
        {
            /* RX individual mask */
            base->RXIMR[i] = (CAN_RXIMR_MI_MASK << CAN_ID_EXT_SHIFT) & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK);
        }
    }

    return stat;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetMaxMsgBuffNum
 * Description   : Set the number of the last Message Buffers.
 * This function will define the number of the last Message Buffers
 *
 *END**************************************************************************/
status_t FLEXCAN_SetMaxMsgBuffNum(
    CAN_Type * base,
    uint32_t maxMsgBuffNum)
{
    uint8_t msgBuffIdx;
    uint32_t databyte;

    //DEV_ASSERT(maxMsgBuffNum>0U);
#if FEATURE_CAN_HAS_FD
    uint8_t can_real_payload = FLEXCAN_GetPayloadSize(base);
#else
    uint8_t can_real_payload = 8U;
#endif
    status_t status = STATUS_SUCCESS;

#if FEATURE_CAN_HAS_FD
    uint8_t arbitration_field_size = 8U;
    /* Check that the number of MBs is supported based on the payload size*/
    volatile uint32_t * valEndMbPointer =FLEXCAN_GetMsgBuffRegion(base, (maxMsgBuffNum-1U));
    uint32_t valEndMb = (uint32_t)valEndMbPointer + can_real_payload + arbitration_field_size;
    if ((valEndMb > (uint32_t)&base->RAMn[CAN_RAMn_COUNT]) || (maxMsgBuffNum > FLEXCAN_GetMaxMbNum(base)))
#else
    /* Check that the number of MBs is supported based on features defined */
    if (maxMsgBuffNum > FLEXCAN_GetMaxMbNum(base))
#endif
    {
    	status = STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    if (status == STATUS_SUCCESS)
    {
        /* Set the maximum number of MBs*/
        base->MCR = (base->MCR & ~CAN_MCR_MAXMB_MASK) | (((maxMsgBuffNum-1U) << CAN_MCR_MAXMB_SHIFT) & CAN_MCR_MAXMB_MASK);

        if (!FLEXCAN_IsRxFifoEnabled(base))
        {
            /* Initialize all message buffers as inactive */
            for (msgBuffIdx = 0; msgBuffIdx < maxMsgBuffNum; msgBuffIdx++)
            {
                volatile uint32_t *flexcan_mb = FLEXCAN_GetMsgBuffRegion(base, msgBuffIdx);
                volatile uint32_t *flexcan_mb_id   = &flexcan_mb[1];
                volatile uint8_t  *flexcan_mb_data = (volatile uint8_t *)(&flexcan_mb[2]);

                *flexcan_mb = 0x0;
                *flexcan_mb_id = 0x0;
                for (databyte = 0; databyte < can_real_payload; databyte++)
                {
                   flexcan_mb_data[databyte] = 0x0;
                }
            }
        }
    }

    return status;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetRxFifoFilter
 * Description   : Configure RX FIFO ID filter table elements.
 *
 *END**************************************************************************/
void FLEXCAN_SetRxFifoFilter(
    CAN_Type * base,
    flexcan_rx_fifo_id_element_format_t idFormat,
    const flexcan_id_table_t *idFilterTable)
{
    //DEV_ASSERT(idFilterTable != NULL);

    /* Set RX FIFO ID filter table elements*/
    uint32_t i, j, numOfFilters;
    uint32_t val1 = 0, val2 = 0, val = 0;

    volatile uint32_t *filterTable = &base->RAMn[RxFifoFilterTableOffset];

    numOfFilters = (((base->CTRL2) & CAN_CTRL2_RFFN_MASK) >> CAN_CTRL2_RFFN_SHIFT);

    switch(idFormat)
    {
        case (FLEXCAN_RX_FIFO_ID_FORMAT_A):
            /* One full ID (standard and extended) per ID Filter Table element.*/
            (base->MCR) = (((base->MCR) & ~(CAN_MCR_IDAM_MASK)) | ( (((uint32_t)(((uint32_t)(FLEXCAN_RX_FIFO_ID_FORMAT_A))<<CAN_MCR_IDAM_SHIFT))&CAN_MCR_IDAM_MASK)));

            for (i = 0; i < RxFifoFilterElementNum(numOfFilters); i++)
            {
                val = 0;

                if (idFilterTable[i].isRemoteFrame)
                {
                    val = FlexCanRxFifoAcceptRemoteFrame << FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT;
                }
                if(idFilterTable[i].isExtendedFrame)
                {
                    val |= FlexCanRxFifoAcceptExtFrame << FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT;
                    filterTable[i] = val + ((idFilterTable[i].id <<
                                             FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_EXT_SHIFT) &
                                             FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_EXT_MASK);
                }
                else
                {
                    filterTable[i] = val + ((idFilterTable[i].id <<
                                             FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_STD_SHIFT) &
                                             FLEXCAN_RX_FIFO_ID_FILTER_FORMATA_STD_MASK);
                }
            }
            break;
        case (FLEXCAN_RX_FIFO_ID_FORMAT_B):
            /* Two full standard IDs or two partial 14-bit (standard and extended) IDs*/
            /* per ID Filter Table element.*/
           (base->MCR) = (((base->MCR) & ~(CAN_MCR_IDAM_MASK)) | ( (((uint32_t)(((uint32_t)(FLEXCAN_RX_FIFO_ID_FORMAT_B))<<CAN_MCR_IDAM_SHIFT))&CAN_MCR_IDAM_MASK)));

            j = 0;
            for (i = 0; i < RxFifoFilterElementNum(numOfFilters); i++)
            {
                val1 = 0;
                val2 = 0;

                if (idFilterTable[j].isRemoteFrame)
                {
                    val1 = FlexCanRxFifoAcceptRemoteFrame << FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_RTR_SHIFT;
                }
                if (idFilterTable[j + 1U].isRemoteFrame)
                {
                    val2 = FlexCanRxFifoAcceptRemoteFrame << FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_RTR_SHIFT;
                }
                if (idFilterTable[j].isExtendedFrame)
                {
                    val1 |= FlexCanRxFifoAcceptExtFrame << FLEXCAN_RX_FIFO_ID_FILTER_FORMATAB_IDE_SHIFT;

                    filterTable[i] = val1 + (((idFilterTable[j].id &
                                               FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_MASK) >>
                                               FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_CMP_SHIFT) <<
                                               FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_SHIFT1);
                }
                else
                {
                    filterTable[i] = val1 + ((idFilterTable[j].id &
                                              FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_STD_MASK) <<
                                              FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_STD_SHIFT1);
                }
                if (idFilterTable[j + 1U].isExtendedFrame)
                {
                    val2 |= FlexCanRxFifoAcceptExtFrame << FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_IDE_SHIFT;

                    filterTable[i] |= val2 + (((idFilterTable[j + 1U].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_EXT_SHIFT2);
                }
                else
                {
                    filterTable[i] |= val2 + ((idFilterTable[j + 1U].id &
                                               FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_STD_MASK) <<
                                               FLEXCAN_RX_FIFO_ID_FILTER_FORMATB_STD_SHIFT2);
                }
                j = j + 2U;
            }
            break;
        case (FLEXCAN_RX_FIFO_ID_FORMAT_C):
            /* Four partial 8-bit Standard IDs per ID Filter Table element.*/
            (base->MCR) = (((base->MCR) & ~(CAN_MCR_IDAM_MASK)) | ( (((uint32_t)(((uint32_t)(FLEXCAN_RX_FIFO_ID_FORMAT_C))<<CAN_MCR_IDAM_SHIFT))&CAN_MCR_IDAM_MASK)));
            j = 0;
            for (i = 0; i < RxFifoFilterElementNum(numOfFilters); i++)
            {
                if (idFilterTable[j].isExtendedFrame)
                {
                    filterTable[i] |= val1 + (((idFilterTable[j].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT1);
                }
                else
                {
                    filterTable[i] |= val1 + (((idFilterTable[j].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT1);
                }
                if (idFilterTable[j + 1U].isExtendedFrame)
                {
                    filterTable[i] |= val1 + (((idFilterTable[j + 1U].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT2);
                }
                else
                {
                    filterTable[i] |= val1 + (((idFilterTable[j + 1U].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT2);
                }
                if (idFilterTable[j + 2U].isExtendedFrame)
                {
                    filterTable[i] |= val1 + (((idFilterTable[j + 2U].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT3);
                }
                else
                {
                    filterTable[i] |= val1 + (((idFilterTable[j + 2U].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT3);
                }
                if (idFilterTable[j + 3U].isExtendedFrame)
                {
                    filterTable[i] |= val1 + (((idFilterTable[j + 3U].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_EXT_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT4);
                }
                else
                {
                    filterTable[i] |= val1 + (((idFilterTable[j + 3U].id &
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_MASK) >>
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_STD_CMP_SHIFT) <<
                                                FLEXCAN_RX_FIFO_ID_FILTER_FORMATC_SHIFT4);
                }
                j = j + 4U;
            }
            break;
        case (FLEXCAN_RX_FIFO_ID_FORMAT_D):
            /* All frames rejected.*/
            (base->MCR) = (((base->MCR) & ~(CAN_MCR_IDAM_MASK)) | ( (((uint32_t)(((uint32_t)(FLEXCAN_RX_FIFO_ID_FORMAT_D))<<CAN_MCR_IDAM_SHIFT))&CAN_MCR_IDAM_MASK)));
            break;
        default:
            /* Should not get here */
            break;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetMsgBuffIntCmd
 * Description   : Enable/Disable the corresponding Message Buffer interrupt.
 *
 *END**************************************************************************/
status_t FLEXCAN_SetMsgBuffIntCmd(
    CAN_Type * base,
    uint32_t msgBuffIdx, bool enable)
{
    uint32_t temp;
    status_t stat = STATUS_SUCCESS;

    if (msgBuffIdx > (((base->MCR) & CAN_MCR_MAXMB_MASK) >> CAN_MCR_MAXMB_SHIFT))
    {
        stat = STATUS_CAN_BUFF_OUT_OF_RANGE;
    }

    if (stat == STATUS_SUCCESS)
    {
        /* Enable the corresponding message buffer Interrupt */
        temp = 1UL << (msgBuffIdx % 32U);
        if (msgBuffIdx  < 32U)
        {
            if (enable)
            {
                (base->IMASK1) = ((base ->IMASK1) | (temp));
            }
            else
            {
                (base->IMASK1) = ((base->IMASK1) & ~(temp));
            }
        }
#if FEATURE_CAN_MAX_MB_NUM > 32U
        if ((msgBuffIdx >= 32U) && (msgBuffIdx < 64U))
        {
            if (enable)
            {
                (base->IMASK2) = ((base ->IMASK2) | (temp));
            }
            else
            {
                (base->IMASK2) = ((base->IMASK2) & ~(temp));
            }
        }
#endif
#if FEATURE_CAN_MAX_MB_NUM > 64U
        if (msgBuffIdx >= 64U)
        {
            if (enable)
            {
                (base->IMASK3) = ((base ->IMASK3) | (temp));
            }
            else
            {
                (base->IMASK3) = ((base->IMASK3) & ~(temp));
            }
        }
#endif
    }

    return stat;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetErrIntCmd
 * Description   : Enable the error interrupts.
 * This function will enable Error interrupt.
 *
 *END**************************************************************************/
void FLEXCAN_SetErrIntCmd(CAN_Type * base, flexcan_int_type_t errType, bool enable)
{
    uint32_t temp = (uint32_t)errType;
    if (enable)
    {
        if((errType == FLEXCAN_INT_RX_WARNING) || (errType == FLEXCAN_INT_TX_WARNING))
        {
           base->MCR = (base->MCR & ~CAN_MCR_WRNEN_MASK) | CAN_MCR_WRNEN(1U);
        }
#if FEATURE_CAN_HAS_FD
        if (errType == FLEXCAN_INT_ERR)
        {
           base->CTRL2 = (base->CTRL2 & ~CAN_CTRL2_ERRMSK_FAST_MASK) | CAN_CTRL2_ERRMSK_FAST(1U);
        }
#endif
        (base->CTRL1) = ((base->CTRL1) | (temp));
    }
    else
    {
    	(base->CTRL1) = ((base->CTRL1) & ~(temp));
#if FEATURE_CAN_HAS_FD
                if (errType == FLEXCAN_INT_ERR)
                {
                   base->CTRL2 = (base->CTRL2 & ~CAN_CTRL2_ERRMSK_FAST_MASK) | CAN_CTRL2_ERRMSK_FAST(0U);
                }
#endif
        temp = base->CTRL1;
		if (((temp & (uint32_t)FLEXCAN_INT_RX_WARNING) == 0U) && ((temp & (uint32_t)FLEXCAN_INT_TX_WARNING) == 0U))
		{
		   /* If WRNEN disabled then both FLEXCAN_INT_RX_WARNING and FLEXCAN_INT_TX_WARNING will be disabled */
		   base->MCR = (base->MCR & ~CAN_MCR_WRNEN_MASK) | CAN_MCR_WRNEN(0U);
		}
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ExitFreezeMode
 * Description   : Exit of freeze mode.
 *
 *END**************************************************************************/
void FLEXCAN_ExitFreezeMode(CAN_Type * base)
{
    base->MCR = (base->MCR & ~CAN_MCR_HALT_MASK) | CAN_MCR_HALT(0U);
    base->MCR = (base->MCR & ~CAN_MCR_FRZ_MASK) | CAN_MCR_FRZ(0U);

    /* Wait till exit freeze mode */
    while (((base->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT) != 0U) {}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_EnterFreezeMode
 * Description   : Enter the freeze mode.
 *
 *END**************************************************************************/
void FLEXCAN_EnterFreezeMode(CAN_Type * base)
{
	bool enabled = false;

    base->MCR = CAN_MCR_MDIS(1); /* Disable FlexCAN module*/
    base->MCR = CAN_MCR_SOFTRST(1); /*Enable & SW Reset*/
    /* Check if enabled */
    if (((base->MCR & CAN_MCR_MDIS_MASK) >> CAN_MCR_MDIS_SHIFT) == 0U)
	{
		enabled = true;
	}
	else
	{
		base->MCR &= ~CAN_MCR_MDIS_MASK;
	}
    
#if 0 
#if defined(ERRATA_E9595)
    /* Check Low-Power Mode Acknowledge Cleared */
    while (((base->MCR & CAN_MCR_LPMACK_MASK) >> CAN_MCR_LPMACK_SHIFT) == 1U) {}
    /* Check if is a Bus-Off Error corresponding to 1x */
    if ((((base->ESR1 & CAN_ESR1_FLTCONF_MASK) >> CAN_ESR1_FLTCONF_SHIFT) & 2U) != 0U)
    {
    	/* Save registers before Soft Reset */
    	uint32_t tempIMSK[2],tempMCR;
    	tempIMSK[0] = base->IMASK1;
    	tempIMSK[1] = base->IMASK2;
    	tempMCR = base->MCR;
    	/* Soft Reset FlexCan */
    	base->MCR |= CAN_MCR_SOFTRST(1U);
    	while (((base->MCR & CAN_MCR_SOFTRST_MASK) >> CAN_MCR_SOFTRST_SHIFT) != 0U) {}
    	/* Restore registers after Soft Reset */
    	base->IMASK1 = tempIMSK[0];
    	base->IMASK2 = tempIMSK[1];
    	base->MCR = tempMCR;
#if FEATURE_CAN_HAS_MEM_ERR_DET
    /* Disable the Protection again because is enabled by soft reset */
    FLEXCAN_DisableMemErrorDetection(base);
#endif
    }
    else
    {
    	base->MCR = (base->MCR & ~CAN_MCR_HALT_MASK) | CAN_MCR_HALT(1U);
    }
#endif
#if defined(ERRATA_E8341)
    base->TIMER = 0U;
    uint32_t aux=0U;
    while ((((base->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT) == 0U) && (aux < 730U))
    {
    	/* Wait until finish counting 730 bit times and exit*/
    	aux = (uint32_t)base->TIMER;
    }
    if (((base->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT) == 0U)
    {
    	/* Save registers before Soft Reset */
    	uint32_t tempIMSK[2],tempMCR;
    	tempIMSK[0] = base->IMASK1;
    	tempIMSK[1] = base->IMASK2;
    	tempMCR = base->MCR;
    	/* Soft Reset FlexCan */
    	base->MCR |= CAN_MCR_SOFTRST(1U);
    	while (((base->MCR & CAN_MCR_SOFTRST_MASK) >> CAN_MCR_SOFTRST_SHIFT) != 0U) {}
    	/* Restore registers after Soft Reset */
    	base->IMASK1 = tempIMSK[0];
    	base->IMASK2 = tempIMSK[1];
    	base->MCR = tempMCR;
#if FEATURE_CAN_HAS_MEM_ERR_DET
    /* Disable the Protection again because is enabled by soft reset */
    FLEXCAN_DisableMemErrorDetection(base);
#endif
    }
#endif
#endif

    /* Wait for entering the freeze mode */
    while (((base->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT) == 0U) {}
    if (false == enabled)
    {
    	base->MCR |= CAN_MCR_MDIS_MASK;
    	/* Wait until disable mode acknowledged */
        while (((base->MCR & CAN_MCR_LPMACK_MASK) >> CAN_MCR_LPMACK_SHIFT) == 0U) {}
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ClearErrIntStatusFlag
 * Description   : Clear all error interrupt status.
 *
 *END**************************************************************************/
void FLEXCAN_ClearErrIntStatusFlag(CAN_Type * base)
{
    if((base->ESR1 & FLEXCAN_ALL_INT) != 0U)
    {
        (base->ESR1) = FLEXCAN_ALL_INT;
#ifdef ERRATA_E9005
        /* Dummy read as a workaround for errata e9005 to ensure the flags are
        cleared before continuing. */
        (void)(base->ESR1);
#endif
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ReadRxFifo
 * Description   : Read Rx FIFO data.
 * This function will copy MB[0] data field into user's buffer.
 *
 *END**************************************************************************/
void FLEXCAN_ReadRxFifo(
    const CAN_Type * base,
    flexcan_msgbuff_t *rxFifo)
{
    //DEV_ASSERT(rxFifo != NULL);

    uint32_t databyte;
    uint32_t mbWord;

    volatile const uint32_t *flexcan_mb = base->RAMn;
    volatile const uint32_t *flexcan_mb_id = &base->RAMn[1];
    volatile const uint32_t *flexcan_mb_data_32 = &flexcan_mb[2];
    uint32_t *msgData_32 = (uint32_t *)(rxFifo->data);

    uint8_t flexcan_mb_dlc_value = (uint8_t)(((*flexcan_mb) & CAN_CS_DLC_MASK) >> 16);
    uint8_t can_real_payload = FLEXCAN_ComputePayloadSize(flexcan_mb_dlc_value);

    rxFifo->dataLen = can_real_payload;
    rxFifo->cs = *flexcan_mb;

    if ((rxFifo->cs & CAN_CS_IDE_MASK) != 0U)
    {
        rxFifo->msgId = *flexcan_mb_id;
    }
    else
    {
        rxFifo->msgId = (*flexcan_mb_id) >> CAN_ID_STD_SHIFT;
    }
#if (defined(CPU_S32K116) || defined(CPU_S32K118))
    /* Check if the buffer address is aligned */
    if (((uint32_t)msgData_32&0x3U) != 0U)
	{
		/* Copy MB[0] data field into user's buffer */
		for (databyte = 0U; databyte < can_real_payload; databyte += 4U)
		{
			mbWord = flexcan_mb_data_32[databyte >> 2U];
			uint32_t x;
			uint8_t index;
			const uint8_t * p;
			FlexcanSwapBytesInWord(mbWord, x);
			p = (uint8_t *)&x;
			for (index = 0U; index < 4U; index++)
			{
				rxFifo->data[databyte+index] = p[index];
			}
		}
	}
	else
	{
		/* Copy MB[0] data field into user's buffer */
		for (databyte = 0U; databyte < can_real_payload; databyte += 4U)
		{
			mbWord = flexcan_mb_data_32[databyte >> 2U];
			FlexcanSwapBytesInWord(mbWord, msgData_32[databyte >> 2U]);
		}
	}
#else
      /* Copy MB[0] data field into user's buffer */
      for (databyte = 0U; databyte < can_real_payload; databyte += 4U)
      {
          mbWord = flexcan_mb_data_32[databyte >> 2U];
          FlexcanSwapBytesInWord(mbWord, msgData_32[databyte >> 2U]);
      }
#endif
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetOperationMode
 * Description   : Enable a FlexCAN operation mode.
 * This function will enable one of the modes listed in flexcan_operation_modes_t.
 *
 *END**************************************************************************/
void FLEXCAN_SetOperationMode(
    CAN_Type * base,
    flexcan_operation_modes_t mode)
{

	switch (mode)
	{
	case FLEXCAN_FREEZE_MODE:
        /* Debug mode, Halt and Freeze*/
        FLEXCAN_EnterFreezeMode(base);
	break;
	case FLEXCAN_DISABLE_MODE:
		/* Debug mode, Halt and Freeze */
		base->MCR = (base->MCR & ~CAN_MCR_MDIS_MASK) | CAN_MCR_MDIS(1U);
	break;
	case FLEXCAN_NORMAL_MODE:
#if FEATURE_CAN_HAS_SUPV
        base->MCR = (base->MCR & ~CAN_MCR_SUPV_MASK) | CAN_MCR_SUPV(0U);
#endif
        base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_LOM_MASK) | CAN_CTRL1_LOM(0U);
        base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_LPB_MASK) | CAN_CTRL1_LPB(0U);
    break;
	case FLEXCAN_LISTEN_ONLY_MODE:
        base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_LOM_MASK) | CAN_CTRL1_LOM(1U);
    break;
	case FLEXCAN_LOOPBACK_MODE:
        base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_LPB_MASK) | CAN_CTRL1_LPB(1U);
        base->CTRL1 = (base->CTRL1 & ~CAN_CTRL1_LOM_MASK) | CAN_CTRL1_LOM(0U);
        /* Enable Self Reception */
        FLEXCAN_SetSelfReception(base, true);
    break;
	default :
		 /* Should not get here */
	break;
	}
}

#if FEATURE_CAN_HAS_FD
/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_SetPayloadSize
 * Description   : Sets the payload size of the MBs.
 *
 *END**************************************************************************/
void FLEXCAN_SetPayloadSize(
    CAN_Type * base,
    flexcan_fd_payload_size_t payloadSize)
{
    uint32_t tmp;

    //DEV_ASSERT(FLEXCAN_IsFDEnabled(base) || (payloadSize == FLEXCAN_PAYLOAD_SIZE_8));

    /* If FD is not enabled, only 8 bytes payload is supported */
    if (FLEXCAN_IsFDEnabled(base))
    {
        tmp = base->FDCTRL;
        tmp &= ~(CAN_FDCTRL_MBDSR0_MASK);
        tmp |= ((uint32_t)payloadSize) << CAN_FDCTRL_MBDSR0_SHIFT;
#if FEATURE_CAN_HAS_MBDSR1
        tmp &= ~(CAN_FDCTRL_MBDSR1_MASK);
        tmp |= ((uint32_t)payloadSize) << CAN_FDCTRL_MBDSR1_SHIFT;
#endif
#if FEATURE_CAN_HAS_MBDSR2
        tmp &= ~(CAN_FDCTRL_MBDSR2_MASK);
        tmp |= ((uint32_t)payloadSize) << CAN_FDCTRL_MBDSR2_SHIFT;
#endif

        base->FDCTRL = tmp;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_GetPayloadSize
 * Description   : Returns the payload size of the MBs (in bytes).
 *
 *END**************************************************************************/
uint8_t FLEXCAN_GetPayloadSize(const CAN_Type * base)
{
    uint32_t payloadSize;

    /* The standard payload size is 8 bytes */
    if (!FLEXCAN_IsFDEnabled(base))
    {
        payloadSize = 8U;
    }
    else
    {
        payloadSize = 1UL << (((base->FDCTRL & CAN_FDCTRL_MBDSR0_MASK) >> CAN_FDCTRL_MBDSR0_SHIFT) + 3U);
    }

    return (uint8_t)payloadSize;
}
#endif  /* End FEATURE_CAN_HAS_FD */

#if FEATURE_CAN_HAS_PRETENDED_NETWORKING

/*FUNCTION**********************************************************************
 *
 * Function Name : FLEXCAN_ConfigPN
 * Description   : Configures the Pretended Networking mode.
 *
 *END**************************************************************************/
void FLEXCAN_ConfigPN(CAN_Type * base, const flexcan_pn_config_t *pnConfig)
{
    //DEV_ASSERT(pnConfig != NULL);

    /* Configure specific pretended networking settings */
    FLEXCAN_SetPNFilteringSelection(base,
                                    pnConfig->wakeUpTimeout,
                                    pnConfig->wakeUpMatch,
                                    pnConfig->numMatches,
                                    pnConfig->filterComb,
                                    pnConfig->idFilterType,
                                    pnConfig->payloadFilterType);

    FLEXCAN_SetPNTimeoutValue(base, pnConfig->matchTimeout);

    /* Configure ID filtering */
    FLEXCAN_SetPNIdFilter1(base, pnConfig->idFilter1);

    /* Configure the second ID, if needed (as mask for exact matching or higher limit for range matching) */
    if ((pnConfig->idFilterType == FLEXCAN_FILTER_MATCH_EXACT) || (pnConfig->idFilterType == FLEXCAN_FILTER_MATCH_RANGE))
    {
        FLEXCAN_SetPNIdFilter2(base, pnConfig);
    }

    /* Configure payload filtering, if requested */
    if ((pnConfig->filterComb == FLEXCAN_FILTER_ID_PAYLOAD) || (pnConfig->filterComb == FLEXCAN_FILTER_ID_PAYLOAD_NTIMES))
    {
        FLEXCAN_SetPNDlcFilter(base,
                               pnConfig->payloadFilter.dlcLow,
                               pnConfig->payloadFilter.dlcHigh);

        FLEXCAN_SetPNPayloadHighFilter1(base, pnConfig->payloadFilter.payload1);
        FLEXCAN_SetPNPayloadLowFilter1(base, pnConfig->payloadFilter.payload1);

        /* Configure the second payload, if needed (as mask for exact matching or higher limit for range matching) */
        if ((pnConfig->payloadFilterType == FLEXCAN_FILTER_MATCH_EXACT) || (pnConfig->payloadFilterType == FLEXCAN_FILTER_MATCH_RANGE))
        {
            FLEXCAN_SetPNPayloadHighFilter2(base, pnConfig->payloadFilter.payload2);
            FLEXCAN_SetPNPayloadLowFilter2(base, pnConfig->payloadFilter.payload2);
        }
    }
}

#endif /* FEATURE_CAN_HAS_PRETENDED_NETWORKING */

/*******************************************************************************
 * EOF
 ******************************************************************************/
