/*
 * Copyright 2017-2019 NXP
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
 * @file siul2_hw_access.c
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


#include "siul2_hw_access.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

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
    DEV_ASSERT(config != NULL);
    DEV_ASSERT(config->pinPortIdx < SIUL2_MSCR_COUNT);
    uint32_t pinsValues = 0U;
    uint32_t pinsValuesInput = 0U;
    uint8_t inputMuxIterator;
    uint32_t pinIntValue;

#ifdef FEATURE_SIUL2_HAS_DDR_PAD
    pinsValues |= SIUL2_MSCR_CRPOINT_TRIM(config->ddrConfiguration.crosspointAdjustment);
    pinsValues |= SIUL2_MSCR_DCYCLE_TRIM(config->ddrConfiguration.trimmingAdjustment);
    pinsValues |= SIUL2_MSCR_DDR_SEL(config->ddrConfiguration.ddrSelection);
    pinsValues |= SIUL2_MSCR_DDR_DO_TRIM(config->ddrConfiguration.trimmingDelay);
    pinsValues |= SIUL2_MSCR_DDR_ODT(config->ddrConfiguration.onDieTermination);
    pinsValues |= SIUL2_MSCR_DDR_INPUT(config->ddrConfiguration.ddrInputMode);
#endif
#ifdef FEATURE_PINS_HAS_DRIVE_STRENGTH_CONTROL
    pinsValues |= SIUL2_MSCR_DSE(config->driveSelect);
#endif
    if (config->pullConfig != PORT_INTERNAL_PULL_NOT_ENABLED)
    {
#ifdef FEATURE_SIUL2_HAS_WEAK_PULL_ENABLE
        if (config->pullConfig == PORT_INTERNAL_PULL_DOWN_ENABLED)
        {
            pinsValues |= SIUL2_MSCR_WPDE(1);
        }
        else
        {
            pinsValues |= SIUL2_MSCR_WPUE(1);
        }
#else
#ifdef FEATURE_SIUL2_HAS_PULL_KEEPER
        pinsValues |= SIUL2_MSCR_PKE(1);
#endif
        pinsValues |= SIUL2_MSCR_PUE(1);
        pinsValues |= SIUL2_MSCR_PUS(config->pullConfig);
#endif
    }
    else
    {
        /* default disable pull resistors */
    }

    pinsValues |= SIUL2_MSCR_SRC(config->slewRateCtrlSel);
#ifdef FEATURE_SIUL2_HAS_INVERT_DATA_OUTPUT
    pinsValues |= SIUL2_MSCR_INV(config->invertOutput);
#endif
    pinsValues |= SIUL2_MSCR_SMC(config->safeMode);
    pinsValues |= SIUL2_MSCR_HYS(config->hysteresisSelect);
#ifdef FEATURE_SIUL2_HAS_ANALOG_PAD
    pinsValues |= SIUL2_MSCR_APC(config->analogPadCtrlSel);
#endif
#ifdef FEATURE_SIUL2_HAS_OUTPUT_DRIVE_CONTROL
    pinsValues |= SIUL2_MSCR_ODC(config->outputDriveCtrl);
#else
    pinsValues |= SIUL2_MSCR_ODE(config->openDrain);
    pinsValues |= SIUL2_MSCR_OBE(config->outputBuffer);
#endif
#ifdef FEATURE_SIUL2_HAS_INPUT_LEVEL_SELECTION
    pinsValues |= SIUL2_MSCR_ILS(config->inputLevel);
#endif
    pinsValues |= SIUL2_MSCR_IBE(config->inputBuffer);
    pinsValues |= SIUL2_MSCR_SSS(config->mux);

    /* Configure initial value for GPIO pin in GPIO mux */
#ifdef FEATURE_SIUL2_HAS_OUTPUT_DRIVE_CONTROL
    if ((PORT_MUX_AS_GPIO == config->mux) && (PORT_OUTPUT_DRIVE_DISABLED != config->outputDriveCtrl))
#else
    if ((PORT_MUX_AS_GPIO == config->mux) && (PORT_OUTPUT_BUFFER_ENABLED == config->outputBuffer))
#endif
    {
        PINS_WritePin(config->gpioBase, (pins_channel_type_t)(config->pinPortIdx % SIUL2_NUM_OF_PIN_PORT), config->initValue);
    }

    /* Write to Multiplexed Signal Configuration Register register */
    config->base->MSCR[config->pinPortIdx] = pinsValues;

    if (PORT_INPUT_BUFFER_ENABLED == config->inputBuffer)
    {
        for (inputMuxIterator = 0U; inputMuxIterator < FEATURE_SIUL2_INPUT_MUX_WIDTH; inputMuxIterator++)
        {
            /* Check if input mux information needs to be configured */
            if (PORT_INPUT_MUX_NO_INIT != config->inputMux[inputMuxIterator])
            {
                /* Input mux value */
                pinsValuesInput = SIUL2_IMCR_SSS(config->inputMux[inputMuxIterator]);

                /* Input inversion feature */
            #ifdef FEATURE_SIUL2_HAS_INVERT_DATA_INPUT
                pinsValuesInput |= SIUL2_IMCR_INV(config->inputInvert[inputMuxIterator]);
            #endif
                /* Write to Input Mux register */
                config->base->IMCR[config->inputMuxReg[inputMuxIterator]] = (uint32_t)pinsValuesInput;
            }
        }

        DEV_ASSERT(config->intConfig.eirqPinIdx < SIUL2_INTERRUPT_COUNT);
        /* Get the appropriate value follow index of pin interrupt */
        pinIntValue = (1UL << config->intConfig.eirqPinIdx);
        /* External interrupt initialization */
        if (config->intConfig.intEdgeSel != SIUL2_INT_DISABLE)
        {
            /* Setting the appropriate IREEnbits in IREER0 */
            if ((config->intConfig.intEdgeSel == SIUL2_INT_RISING_EDGE) || (config->intConfig.intEdgeSel == SIUL2_INT_EITHER_EDGE))
            {
                config->base->IREER0 |= pinIntValue;
            }
            else
            {
                config->base->IREER0 &= ~pinIntValue;
            }
            /* Setting the appropriate IREEnbits in IFEER0 */
            if ((config->intConfig.intEdgeSel == SIUL2_INT_FALLING_EDGE) || (config->intConfig.intEdgeSel == SIUL2_INT_EITHER_EDGE))
            {
                config->base->IFEER0 |= pinIntValue;
            }
            else
            {
                config->base->IFEER0 &= ~pinIntValue;
            }

        #ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
            /* Select the request desired between DMA or Interrupt */
            if (SIUL2_INT_USING_DMA == config->intConfig.intExeSel)
            {
                config->base->DIRSR0 |= pinIntValue;
            }
            else
            {
                config->base->DIRSR0 &= ~pinIntValue;
            }
            /* Write to EIFn bits in DISR0  to clear any flags */
            config->base->DISR0  |= pinIntValue;
            /* Enable the interrupt pins */
            config->base->DIRER0 |= pinIntValue;
        #else  /* if FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA */
            /* Select the request desired Interrupt */
            config->base->IRSR0 &= ~pinIntValue;
            /* Write to EIFn bits in ISR0  to clear any flags */
            config->base->ISR0 = pinIntValue;
            /* Enable the interrupt pins */
            config->base->IRER0 |= pinIntValue;
        #endif /* if FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA */
        }
        else
        {
        #ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
            /* Mask interrupts by clearing the EIREn bits in DIRER0 */
            config->base->DIRER0 &= ~pinIntValue;
        #else
            /* Mask interrupts by clearing the EIREn bits in IRER0 */
            config->base->IRER0 &= ~pinIntValue;
        #endif
        }
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : PINS_SetExInt
 * Description   : This function configures the external interrupt.
 *
 *END**************************************************************************/
void PINS_SetExInt(siul2_interrupt_config_t intConfig)
{
    uint32_t pinIntValue;
    DEV_ASSERT(intConfig.eirqPinIdx < SIUL2_INTERRUPT_COUNT);
    SIUL2_Type * const base = SIUL2;

    /* Get the appropriate value follow index of pin interrupt */
    pinIntValue = (1UL << intConfig.eirqPinIdx);
    /* External interrupt initialization */
    if (intConfig.intEdgeSel != SIUL2_INT_DISABLE)
    {
        /* Set maximum interrupt filter counter value */
        base->IFMCR[intConfig.eirqPinIdx] = (uint32_t)((uint32_t)intConfig.maxCnt & SIUL2_IFMCR_MAXCNT_MASK);
        /* Setting the appropriate IFEn bits in IFER0 */
        if (intConfig.digitalFilter == true)
        {
            base->IFER0 |= pinIntValue;
        }
        else
        {
            base->IFER0 &= ~pinIntValue;
        }

        /* Setting the appropriate IREEn bits in IREER0 */
        if ((intConfig.intEdgeSel == SIUL2_INT_RISING_EDGE) || (intConfig.intEdgeSel == SIUL2_INT_EITHER_EDGE))
        {
            base->IREER0 |= pinIntValue;
        }
        else
        {
            base->IREER0 &= ~pinIntValue;
        }
        /* Setting the appropriate IREEn bits in IFEER0 */
        if ((intConfig.intEdgeSel == SIUL2_INT_FALLING_EDGE) || (intConfig.intEdgeSel == SIUL2_INT_EITHER_EDGE))
        {
            base->IFEER0 |= pinIntValue;
        }
        else
        {
            base->IFEER0 &= ~pinIntValue;
        }

    #ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
        /* Select the request desired between DMA or Interrupt */
        if (SIUL2_INT_USING_DMA == intConfig.intExeSel)
        {
            base->DIRSR0 |= pinIntValue;
        }
        else
        {
            base->DIRSR0 &= ~pinIntValue;
        }

        /* Write to EIFn bits in DISR0  to clear any flags */
        base->DISR0  |= pinIntValue;
        /* Enable the interrupt pins */
        base->DIRER0 |= pinIntValue;
    #else /* if FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA */
        /* Select the request desired Interrupt */
        base->IRSR0 &= ~pinIntValue;
        /* Write to EIFn bits in ISR0  to clear any flags  */
        base->ISR0 = pinIntValue;
        /* Enable the interrupt pins */
        base->IRER0 |= pinIntValue;
    #endif /* if FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA */
    }
    else
    {
    #ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
        /* Mask interrupts by clearing the EIREn bits in DIRER0 */
        base->DIRER0 &= ~pinIntValue;
    #else
        /* Mask interrupts by clearing the EIREn bits in IRER0 */
        base->IRER0 &= ~pinIntValue;
    #endif
    }
}

#ifdef FEATURE_SIUL2_HAS_SPECIFIC_PULL_REGISTER
/*FUNCTION**********************************************************************
 *
 * Function Name : PINS_SetSpecificPullSel
 * Description   : This function configures the internal specific pull resistor
 * for GPI pins.
 *
 *END**************************************************************************/
void PINS_SetSpecificPullSel(uint32_t pin,
                             port_specific_pull_config_t pullConfig)
{
    SIUL2_Type * const base = SIUL2;
    uint32_t regVal;
    switch (pin / 8u)
    {
        case 0u:
        {
            regVal  = base->GPR0;
            regVal &= ~((uint32_t)((uint32_t)(0xFu) << (pin % 8u)));
            regVal |= (uint32_t)pullConfig << (pin % 8u);
            base->GPR0 = regVal;
            break;
        }
        case 1u:
        {
            regVal  = base->GPR1;
            regVal &= ~((uint32_t)((uint32_t)(0xFu) << (pin % 8u)));
            regVal |= (uint32_t)pullConfig << (pin % 8u);
            base->GPR1 = regVal;
            break;
        }
        case 2u:
        {
            regVal  = base->GPR2;
            regVal &= ~((uint32_t)((uint32_t)(0xFu) << (pin % 8u)));
            regVal |= (uint32_t)pullConfig << (pin % 8u);
            base->GPR2 = regVal;
            break;
        }
        default:
            /* invalid command */
            DEV_ASSERT(false);
            break;
    }
}
#endif

/*******************************************************************************
 * EOF
 ******************************************************************************/
