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
 * @file esci_hw_access.c
 *
 * @page misra_violations MISRA-C:2012 violations
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

#include "esci_hw_access.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Table of base addresses for ESCI instances */
static eSCI_Type * const s_esciBase[eSCI_INSTANCE_COUNT] = eSCI_BASE_PTRS;

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_DRV_ClearRegisters
 * Description   : Sets the configuration registers to reset value
 *
 *END**************************************************************************/
void ESCI_HW_ClearRegisters(const uint32_t instance) {
	//DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instance];
	base->CR2 = 0x2000;
	base->BRR = 78;
	base->CR1 = 0b1100;
	base->CR3 = 0;

}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_ConfigureReceiver
 * Description   : Configures receiver related registers
 *
 *END**************************************************************************/
status_t ESCI_HW_ConfigureReceiver(const uint32_t instance,
		const esci_user_config_t * esciUserConfig) {
	//DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instance];
	status_t ret = STATUS_SUCCESS;

	/* Setup RXD pin polarity */
	base->CR2 |= (uint16_t) eSCI_CR2_RXPOL(
			esciUserConfig->rxdInvertedPolarity ? 1UL : 0UL);

	/* Select the receiver wake-up condition */
	base->CR1 |= (uint16_t) eSCI_CR1_WAKE(
			(esciUserConfig->rxWakeupCondition == ESCI_WAKEUP_ADDRESS_MARK) ?
					1UL : 0UL);

	/* Set the receiver input mode */
	switch (esciUserConfig->receiveInputMode) {
	case ESCI_RECV_MODE_DUAL_WIRE:
		base->CR1 |= (uint16_t) (eSCI_CR1_LOOPS(0U) | eSCI_CR1_RSRC(0U));
		break;
	case ESCI_RECV_MODE_LOOP:
		base->CR1 |= (uint16_t) (eSCI_CR1_LOOPS(1U) | eSCI_CR1_RSRC(0U));
		break;
	case ESCI_RECV_MODE_SINGLE_WIRE:
		base->CR1 |= (uint16_t) (eSCI_CR1_LOOPS(1U) | eSCI_CR1_RSRC(1U));
		break;
	default:
		ret = STATUS_ESCI_FRAME_FORMAT_ERROR;
		break;
	}

	return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_SetFrameFormat
 * Description   : Configures the frame format related registers
 *                 based on the user configuration
 *
 *END**************************************************************************/
status_t ESCI_HW_SetFrameFormat(const uint32_t instance,
		const esci_frame_format_t * frameFormat) {
	//DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instance];
	status_t ret = STATUS_SUCCESS;

	/* Explicit cast from boolean to uint16_t will trigger a MISRA warning */
	uint16_t parityBits = (
			(frameFormat->parityBits == true) ? (uint16_t) 1U : (uint16_t) 0U);
	uint16_t addressBits = (
			(frameFormat->addressBits == true) ? (uint16_t) 1U : (uint16_t) 0U);

	switch (frameFormat->characterBits) {
	case ESCI_CHARACTER_BITS_7:
		/* 7-bit characters require the parity bit or address bit to be enabled, but not both */
		if ((frameFormat->parityBits != frameFormat->addressBits)
				&& (!frameFormat->extraStopBit)) {
			/* 8 payload bits in total with either parity or address bit */
			base->CR1 |= (uint16_t) ((uint16_t) eSCI_CR1_M(0U)
					| (uint16_t) eSCI_CR1_PE((uint16_t )parityBits)
					| (uint16_t) eSCI_CR1_WAKE((uint16_t )addressBits));
		} else {
			/* frame format does not support 7 payload bits */
			ret = STATUS_ESCI_FRAME_FORMAT_ERROR;
		}
		break;
	case ESCI_CHARACTER_BITS_8:
		if (frameFormat->extraStopBit) {
			if ((!frameFormat->addressBits) && frameFormat->parityBits) {
				/* frame format with 2 stop bits with parity and no address bits */
				base->CR1 |= (uint16_t) (eSCI_CR1_M(0U) | eSCI_CR1_PE(1U)
						| eSCI_CR1_WAKE(0U));
				base->CR3 |= (uint16_t) eSCI_CR3_M2(1U);
			} else {
				/* invalid frame format with 2 stop bits */
				ret = STATUS_ESCI_FRAME_FORMAT_ERROR;
			}
		} else {
			if (frameFormat->parityBits && frameFormat->addressBits) {
				/* having both address and parity bits enabled is not supported */
				ret = STATUS_ESCI_FRAME_FORMAT_ERROR;
			} else if (frameFormat->parityBits || frameFormat->addressBits) {
				/* frame format with 9 payload bits */
				base->CR1 |= (uint16_t) ((uint16_t) eSCI_CR1_M(1U)
						| (uint16_t) eSCI_CR1_PE(parityBits)
						| (uint16_t) eSCI_CR1_WAKE(addressBits));
			} else {
				/* 8 payload bits - do nothing - leave registers to their reset values */
			}
		}
		break;
	case ESCI_CHARACTER_BITS_9:
		if (frameFormat->parityBits || frameFormat->addressBits
				|| frameFormat->extraStopBit) {
			/* frame format does not support 10 payload bits */
			ret = STATUS_ESCI_FRAME_FORMAT_ERROR;
		}
		/* frame format with 9 payload bits */
		base->CR1 |= (uint16_t) (eSCI_CR1_M(1U) | eSCI_CR1_PE(0U)
				| eSCI_CR1_WAKE(0U));
		break;
	case ESCI_CHARACTER_BITS_12:
		if (frameFormat->addressBits || (!frameFormat->parityBits)
				|| (!frameFormat->extraStopBit)) {
			/* invalid frame format with 2 stop bits */
			ret = STATUS_ESCI_FRAME_FORMAT_ERROR;
		}
		/* frame format with 2 stop bits */
		base->CR1 |= (uint16_t) (eSCI_CR1_M(1U) | eSCI_CR1_PE(1U)
				| eSCI_CR1_WAKE(0U));
		base->CR3 |= (uint16_t) eSCI_CR3_M2(1U);
		break;
	default:
		ret = STATUS_ESCI_FRAME_FORMAT_ERROR;
		break;
	}
	/* set parity type */
	base->CR1 |= (uint16_t) eSCI_CR1_PT(frameFormat->oddParity ? 1UL : 0UL);

	return ret;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_SetClockDiv
 * Description   : Calculates the clock divider and
 *                 configures the related registers
 *
 *END**************************************************************************/
void ESCI_HW_SetClockDiv(const uint32_t instance, uint32_t baudRate,
		uint32_t clockFrequency) {
	//DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instance];

	uint32_t clockDivider = clockFrequency / baudRate;
	/* Add 0.5 then divide by 16, this rounds the final value to be closer to target value */
	clockDivider += 8U;
	clockDivider >>= 4U;

	/* Configure the new clock divider */
	base->BRR = eSCI_BRR_SBR(clockDivider);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_GetBaudRate
 * Description   : Calculates the configured baud rate
 *
 *END**************************************************************************/
void ESCI_HW_GetBaudRate(const uint32_t instance, uint32_t * baudRate,
		uint32_t clockFrequency) {
	//DEV_ASSERT(instance < eSCI_INSTANCE_COUNT);
	//DEV_ASSERT(baudRate != NULL);

	const eSCI_Type * base = s_esciBase[instance];
	uint32_t clockDivider = ((uint32_t) base->BRR) & eSCI_BRR_SBR_MASK;
	//DEV_ASSERT(clockDivider != 0U);
	/* Calculate the baud rate using the module frequency and clock divider */
	*baudRate = clockFrequency / (16U * clockDivider);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_TxEnable
 * Description   : Enables the ESCI transmitter and related interrupts
 *
 *END**************************************************************************/
void ESCI_HW_TxEnable(const uint32_t instNum, const bool dmaEnable) {
	//DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instNum];

	/* Enable the transmitter */
	base->CR1 |= (uint16_t) eSCI_CR1_TE(1U);

	if (dmaEnable) {
		/* Enable DMA transmission */
		base->CR2 |= (uint16_t) eSCI_CR2_TXDMA(1U);
	} else {
		/* Enable transmitter related interrupts */
		base->CR1 |= (uint16_t) (eSCI_CR1_TIE(1U) | eSCI_CR1_TCIE(1U));
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_TxDisable
 * Description   : Disables the ESCI transmitter and related interrupts
 *
 *END**************************************************************************/
void ESCI_HW_TxDisable(const uint32_t instNum) {
	//DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instNum];

	/* Disable DMA transmission */
	base->CR2 &= (uint16_t) (~eSCI_CR2_TXDMA_MASK);

	/* Disable the transmitter and related interrupts */
	base->CR1 &= (uint16_t) (~(eSCI_CR1_TE_MASK | eSCI_CR1_TIE_MASK
			| eSCI_CR1_TCIE_MASK));

	/* Clear TDRE and TC bits - w1c */
	base->IFSR1 = (uint16_t) (eSCI_IFSR1_TDRE_MASK | eSCI_IFSR1_TC_MASK);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_RxEnable
 * Description   : Enables the ESCI receiver and related interrupts
 *
 *END**************************************************************************/
void ESCI_HW_RxEnable(const uint32_t instNum, const bool dmaEnable) {
	//DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instNum];

	/* Enable the receiver */
	base->CR1 |= (uint16_t) eSCI_CR1_RE(1U);

	if (dmaEnable) {
		/* Enable DMA transmission */
		base->CR2 |= (uint16_t) eSCI_CR2_RXDMA(1U);
	} else {
		/* Enable receiver related interrupts */
		base->CR1 |= (uint16_t) eSCI_CR1_RIE(1U);
	}
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_RxDisable
 * Description   : Disables the ESCI receiver and related interrupts
 *
 *END**************************************************************************/
void ESCI_HW_RxDisable(const uint32_t instNum) {
	//DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);

	eSCI_Type * base = s_esciBase[instNum];

	/* Disable DMA reception */
	base->CR2 &= (uint16_t) (~eSCI_CR2_RXDMA_MASK);

	/* Disable the receiver and related interrupts */
	base->CR1 &= (uint16_t) (~(eSCI_CR1_RE_MASK | eSCI_CR1_RIE_MASK));

	/* Clear the RDRF bit - w1c */
	base->IFSR1 = eSCI_IFSR1_RDRF_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_GetDataRegLSBAddr
 * Description   : Retrieves the address of the LSB of the
 *                 data register. This is used for DMA configuration.
 *
 *END**************************************************************************/
uint32_t ESCI_HW_GetDataRegLSBAddr(const uint32_t instNum) {
	//DEV_ASSERT(instNum < eSCI_INSTANCE_COUNT);

	const eSCI_Type * base = s_esciBase[instNum];

	return (uint32_t) (&(base->SDR)) + 1U;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_GetReceiveReadyFlag
 * Description   : Checks if the Receive Data Register Full flag is set
 *
 *END**************************************************************************/
bool ESCI_HW_GetReceiveReadyFlag(const uint32_t instance) {
	const eSCI_Type * base = s_esciBase[instance];

	return (base->IFSR1 & eSCI_IFSR1_RDRF_MASK) != 0U;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_ClearReceiveReadyFlag
 * Description   : Clears the Receive Data Register Full flag
 *
 *END**************************************************************************/
void ESCI_HW_ClearReceiveReadyFlag(const uint32_t instance) {
	eSCI_Type * base = s_esciBase[instance];

	/* Clear the RDRF bit - w1c */
	base->IFSR1 = eSCI_IFSR1_RDRF_MASK;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_GetSendReadyFlag
 * Description   : Checks if the Transmit Data Register Empty or
 *                 Transmit Complete flags have been set
 *
 *END**************************************************************************/
bool ESCI_HW_GetSendReadyFlag(const uint32_t instance) {
	const eSCI_Type * base = s_esciBase[instance];

	return (base->IFSR1 & (eSCI_IFSR1_TDRE_MASK | eSCI_IFSR1_TC_MASK)) != 0U;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_ClearSendReadyFlag
 * Description   : Clears the Transmit Data Register Empty and
 *                 Transmit Complete flags
 *
 *END**************************************************************************/
void ESCI_HW_ClearSendReadyFlag(const uint32_t instance) {
	eSCI_Type * base = s_esciBase[instance];

	/* Clear TDRE and TC bits - w1c */
	base->IFSR1 = (uint16_t) (eSCI_IFSR1_TDRE_MASK | eSCI_IFSR1_TC_MASK);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_SendCharacter
 * Description   : Sends a character
 *
 *END**************************************************************************/
void ESCI_HW_SendCharacter(const uint32_t instance, uint16_t data) {
	eSCI_Type * base = s_esciBase[instance];
	/* Retrieve the 9th bit of the next character that has to be send */
	uint8_t msb_data = (uint8_t) ((data >> 8U) & 0x1U);
	data &= 0xFFU;
	/* Send the character */
	while ((base->IFSR1 & 0x8000) == 0) {}
	base->IFSR1 = 0x8000;              /* Clear TDRE flag */
	base->SDR = eSCI_SDR_RDTD(data) | eSCI_SDR_TN(msb_data);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : ESCI_HW_ReceiveCharacter
 * Description   : Receives a character
 *
 *END**************************************************************************/
uint16_t ESCI_HW_ReceiveCharacter(const uint32_t instance,
		esci_character_bits_t bitCountPerChar) {
	const eSCI_Type * base = s_esciBase[instance];
	uint8_t msb_data = 0U;

	/* Retrieve the data character */
	uint16_t word_data = base->SDR;

	switch (bitCountPerChar) {
	case ESCI_CHARACTER_BITS_7:
		/* Trim the character to 7 bits */
		word_data &= (uint16_t) 0x7FU;
		break;
	case ESCI_CHARACTER_BITS_8:
		/* Trim the character to 8 bits */
		word_data &= (uint16_t) 0xFFU;
		break;
	case ESCI_CHARACTER_BITS_9:
		/* Retrieve the 9th bit */
		msb_data = (uint8_t) ((word_data & eSCI_SDR_RN_MASK)
				>> eSCI_SDR_RN_SHIFT);
		break;
	case ESCI_CHARACTER_BITS_12:
		/* Extract the data bitfield from the register */
		word_data &= eSCI_SDR_RDTD_MASK;
		/* Retrieve the 4 MSB bits (11-8) */
		msb_data = (uint8_t) (word_data >> 8U);
		break;
	default:
		/* Return 0 for any other input value */
		word_data = 0U;
		msb_data = 0U;
		break;
	}

	/* Clear any MSB from the original register */
	word_data &= (uint16_t) 0xFFU;

	/* Add the correctly shifted MSB to the register */
	word_data |= (uint16_t) (((uint16_t) msb_data) << 8U);

	return word_data;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
