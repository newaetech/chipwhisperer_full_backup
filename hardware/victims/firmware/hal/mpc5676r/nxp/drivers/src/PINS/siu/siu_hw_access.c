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
 * @file siu_hw_access.c
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
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 10.8, Impermissible cast of composite expression
 * (different essential type categories).
 * This is required by the conversion of a marco and number into address number
 * to increase source address.
 */


#include "siu_hw_access.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The number of possible mux selection source in IMUX registers */
#define SIU_MUXSEL_COUNT              16u

/*******************************************************************************
 * Code
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : PINS_Init
 * Description   : This function configures the pin feature with the options
 * provided in the given structure.
 *
 *END**************************************************************************/
void PINS_Init(const pin_settings_config_t * config)
{
    //DEV_ASSERT(config != NULL);
    //DEV_ASSERT(config->pinPortIdx < SIU_PCR_COUNT);
    uint16_t pinsValues = 0U;
    uint32_t pinsValuesInput = 0U;

    if (config->pullConfig != PORT_INTERNAL_PULL_NOT_ENABLED)
    {
        pinsValues |= SIU_PCR_WPE_MASK;
        pinsValues |= SIU_PCR_WPS(config->pullConfig);
    }
    pinsValues |= SIU_PCR_DSC(config->driveSelect);
    pinsValues |= SIU_PCR_ODE(config->openDrain);
    pinsValues |= SIU_PCR_HYS(config->hysteresis);
    pinsValues |= SIU_PCR_SRC(config->slewRateCtrl);
    pinsValues |= SIU_PCR_OBE(config->outputBuffer);
    pinsValues |= SIU_PCR_IBE(config->inputBuffer);
    pinsValues |= SIU_PCR_PA(config->mux);

    /* Configure initial value for GPIO pin in GPIO mux */
    if ((PORT_MUX_AS_GPIO == config->mux) && (PORT_OUTPUT_BUFFER_ENABLED == config->outputBuffer))
    {
        //DEV_ASSERT(config->initValue <= 1u);
        config->base->GPDO[config->pinPortIdx] = SIU_GPDO_PDOn(config->initValue & 1u);
    }

    /* Write to Pad Configuration Register register */
    config->base->PCR[config->pinPortIdx] = pinsValues;

    if (PORT_INPUT_BUFFER_ENABLED == config->inputBuffer)
    {
        /* Configures the external interrupt */
        //DEV_ASSERT(config->intConfig.eirqPinIdx <= SIU_INTERRUPT_COUNT);
        if (config->intConfig.eirqPinIdx != SIU_INT_UNSUPPORTED)
        {
            PINS_SetExInt(config->intConfig);
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : PINS_SetExInt
 * Description   : This function configures the external interrupt.
 *
 *END**************************************************************************/
void PINS_SetExInt(siu_interrupt_config_t intConfig)
{
    uint32_t pinIntValue;
    //DEV_ASSERT(intConfig.eirqPinIdx < SIU_INTERRUPT_COUNT);
    SIU_Type * const base = SIU;

    /* Get the appropriate value follow index of pin interrupt */
    pinIntValue = (1UL << intConfig.eirqPinIdx);
    /* External interrupt initialization */
    if (intConfig.intEdgeSel != SIU_INT_DISABLE)
    {
        /* Selects the source for the external interrupt or DMA inputs */
        base->EIISR &= ~((uint32_t)SIU_EIISR_ESEL0_MASK << (2u * intConfig.eirqPinIdx));
        base->EIISR |= (SIU_EIISR_ESEL0(intConfig.intSource) << (2u * intConfig.eirqPinIdx));

        /* Setting the appropriate IREEn/IFEEn bits in IREER/IFEER */
        switch (intConfig.intEdgeSel)
        {
            case SIU_INT_RISING_EDGE:
            {
                base->IREER |= pinIntValue;
                base->IFEER &= ~pinIntValue;
                break;
            }
            case SIU_INT_FALLING_EDGE:
            {
                base->IREER &= ~pinIntValue;
                base->IFEER |= pinIntValue;
                break;
            }
            case SIU_INT_EITHER_EDGE:
            {
                base->IREER |= pinIntValue;
                base->IFEER |= pinIntValue;
                break;
            }
            default:
            {
                /* Not supported */
                break;
            }
        }

        /* Configure the type of interrupt request */
        switch (intConfig.intTypeSel)
        {
            case SIU_INT_DMA_IRQ_RQUEST:
            {
                base->EISR  |= pinIntValue;
                base->DIRER |= pinIntValue;
                base->ORER  &= ~pinIntValue;
                break;
            }
            case SIU_INT_OVERRUN_REQUEST:
            {
                base->DIRER &= ~pinIntValue;
                base->OSR   |= pinIntValue;
                base->ORER  |= pinIntValue;
                break;
            }
            case SIU_INT_ALL_REQUEST:
            {
                base->EISR  |= pinIntValue;
                base->DIRER |= pinIntValue;
                base->OSR   |= pinIntValue;
                base->ORER  |= pinIntValue;
                break;
            }
            default:
            {
                /* Not supported */
                //DEV_ASSERT(false);
                break;
            }
        }

        /* Select the request desired between DMA or Interrupt */
        if (SIU_INT_USING_DMA == intConfig.intExeSel)
        {
            base->DIRSR |= pinIntValue;
        }
        else
        {
            base->DIRSR &= ~pinIntValue;
        }
    }
    else
    {
        /* Mask interrupts by clearing the EIREn bits in DIRER */
        base->DIRER &= ~pinIntValue;
        /* Disable overrun request pin */
        base->ORER &= ~pinIntValue;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : PINS_SetInputBuffer
 * Description   : This function configures the input buffer.
 *
 *END**************************************************************************/
void PINS_ConfigInputMux(PORT_Type * const base,
                         uint32_t pin,
                         bool enable,
                         port_mux_t mux,
                         siu_input_config_t inputConfig)
{
    //DEV_ASSERT(base != NULL);
    //DEV_ASSERT(pin < SIU_NUM_OF_PIN_PORT);
    uint16_t pcrRegVal;

    pcrRegVal = base->PCR[pin];
    pcrRegVal &= ~SIU_PCR_IBE_MASK;
    pcrRegVal |= SIU_PCR_IBE(enable ? 1UL : 0UL);
    pcrRegVal &= ~SIU_PCR_PA_MASK;
    pcrRegVal |= SIU_PCR_PA(mux);
    base->PCR[pin] = pcrRegVal;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
