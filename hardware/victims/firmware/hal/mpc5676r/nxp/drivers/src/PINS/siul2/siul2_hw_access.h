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
 * @file siul2_hw_access.h
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

#ifndef SIUL2_HW_ACCESS_H
#define SIUL2_HW_ACCESS_H

#include "pins_siul2_driver.h"
/*!
 * siul2_hw_access System Integration Unit Lite2 (SIUL2)
 * @brief This module covers the functionality of the SIUL2 peripheral.
 * <p>
 *  SIUL2 HW ACCESS provides the API for reading and writing register bit-fields belonging to the SIUL2 module.
 * </p>
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SIUL2_INTERRUPT_COUNT           32u
#define SIUL2_NUM_OF_PIN_PORT           16u

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Configuration
 * @{
 */

/*!
 * @brief Initializes the pins with the given configuration structure
 *
 * This function configures the pins with the options provided in the
 * given structure.
 *
 * @param[in] pinNumber the number of configured pins in structure
 * @param[in] config the configuration structure
  */
void PINS_Init(const pin_settings_config_t * config);

/*!
 * @brief Configures the internal resistor.
 *
 * This function configures the internal resistor.
 *
 * @param[in] base Port base pointer
 * @param[in] pin Port pin number
 * @param[in] pullConfig The pull configuration
 */
static inline void PINS_SetPullSel(PORT_Type * const base,
                                   uint16_t pin,
                                   port_pull_config_t pullConfig)
{
    //DEV_ASSERT(base != NULL);
    //DEV_ASSERT(pin < SIUL2_NUM_OF_PIN_PORT);
    uint32_t regVal;

    if (pullConfig == PORT_INTERNAL_PULL_NOT_ENABLED)
    {
#ifdef FEATURE_SIUL2_HAS_WEAK_PULL_ENABLE
        base->MSCR[pin] &= ~SIUL2_MSCR_WPUE_MASK;
        base->MSCR[pin] &= ~SIUL2_MSCR_WPDE_MASK;
#else
        base->MSCR[pin] &= ~SIUL2_MSCR_PUE_MASK;
#ifdef FEATURE_SIUL2_HAS_PULL_KEEPER
        base->MSCR[pin] &= ~SIUL2_MSCR_PKE_MASK;
#endif
#endif
    }
    else
    {
        regVal = base->MSCR[pin];
#ifdef FEATURE_SIUL2_HAS_WEAK_PULL_ENABLE
        if (pullConfig == PORT_INTERNAL_PULL_DOWN_ENABLED)
        {
            regVal |= SIUL2_MSCR_WPDE(1);
            regVal &= ~SIUL2_MSCR_WPUE_MASK;
        }
        else
        {
            regVal |= SIUL2_MSCR_WPUE(1);
            regVal &= ~SIUL2_MSCR_WPDE_MASK;
        }
#else
#ifdef FEATURE_SIUL2_HAS_PULL_KEEPER
        regVal |= SIUL2_MSCR_PKE_MASK;
#endif
        regVal |= SIUL2_MSCR_PUE_MASK;
        regVal &= ~SIUL2_MSCR_PUS_MASK;
        regVal |= SIUL2_MSCR_PUS(pullConfig);
#endif
        base->MSCR[pin] = regVal;
    }
}

#ifdef FEATURE_SIUL2_HAS_SPECIFIC_PULL_REGISTER
/*!
 * @brief Configures the internal specific pull resistor for GPI pins.
 *
 * @param[in] pin Port pin number is supported (PTY0, PTY1, PTY2, etc.)
 * @param[in] pullConfig The pull configuration
 */
void PINS_SetSpecificPullSel(uint32_t pin,
                             port_specific_pull_config_t pullConfig);
#endif

#ifdef FEATURE_SIUL2_HAS_OUTPUT_DRIVE_CONTROL
/*!
 * @brief Configures the output buffer.
 *
 * This function configures the output drive control.
 *
 * @param[in] base Port base pointer
 * @param[in] pin Port pin number
 * @param[in] driveCtrl the Output Drive control
 * @param[in] mux pin muxing slot selection
 */
static inline void PINS_SetOutputDriveControl(PORT_Type * const base,
                                              uint16_t pin,
                                              port_output_drive_control_t driveCtrl,
                                              port_mux_t mux)
{
    //DEV_ASSERT(base != NULL);
    //DEV_ASSERT(pin < SIUL2_NUM_OF_PIN_PORT);
    base->MSCR[pin] &= ~(SIUL2_MSCR_ODC_MASK | SIUL2_MSCR_SSS_MASK);
    base->MSCR[pin] |= (SIUL2_MSCR_ODC(driveCtrl) | SIUL2_MSCR_SSS(mux));
}
#else
/*!
 * @brief Configures the output buffer.
 *
 * This function configures the output buffer.
 *
 * @param[in] base Port base pointer
 * @param[in] pin Port pin number
 * @param[in] enable Enable output buffer
 * @param[in] mux pin muxing slot selection
 */
static inline void PINS_SetOutputBuffer(PORT_Type * const base,
                                        uint16_t pin,
                                        bool enable,
                                        port_mux_t mux)
{
    //DEV_ASSERT(base != NULL);
    //DEV_ASSERT(pin < SIUL2_NUM_OF_PIN_PORT);
    base->MSCR[pin] &= ~SIUL2_MSCR_OBE_MASK;
    base->MSCR[pin] |= SIUL2_MSCR_OBE(enable ? 1UL : 0UL);
    base->MSCR[pin] &= ~SIUL2_MSCR_SSS_MASK;
    base->MSCR[pin] |= SIUL2_MSCR_SSS(mux);
}
#endif

/*!
 * @brief Configures the input buffer.
 *
 * This function configures the input buffer.
 *
 * @param[in] base Port base pointer
 * @param[in] pin Port pin number
 * @param[in] enable Enable input buffer
 * @param[in] inputMuxReg pin muxing register slot selection
 * @param[in] inputMux pin muxing slot selection
 */
static inline void PINS_SetInputBuffer(PORT_Type * const base,
                                       uint16_t pin,
                                       bool enable,
                                       uint32_t inputMuxReg,
                                       port_input_mux_t inputMux)
{
    //DEV_ASSERT(pin < SIUL2_NUM_OF_PIN_PORT);
    /* Check external pin for inputMux */
    if (base != NULL)
    {
        base->MSCR[pin] &= ~SIUL2_MSCR_IBE_MASK;
        base->MSCR[pin] |= SIUL2_MSCR_IBE(enable ? 1UL : 0UL);
    }
    /* Configure inputMux for input signal */
    if (inputMux != PORT_INPUT_MUX_NO_INIT)
    {
        SIUL2_IMCR_BASE[inputMuxReg] &= ~SIUL2_IMCR_SSS_MASK;
        SIUL2_IMCR_BASE[inputMuxReg] |= SIUL2_IMCR_SSS(inputMux);
    }
}

/*!
 * @brief Configures the interrupt filter.
 *
 * This function configures the interrupt filter clock prescaler.
 *
 * @param[in] prescaler The clock prescaler value
 */
static inline void PINS_ConfigIntFilterClock(uint8_t prescaler)
{
    SIUL2_Type * base = SIUL2;

    base->IFCPR = (uint32_t)((uint32_t)prescaler & SIUL2_IFCPR_IFCP_MASK);
}

/*!
 * @brief Configures the external interrupt.
 *
 * This function configures the external interrupt.
 *
 * @param[in] intConfig The configuration for interrupt pin
 */
void PINS_SetExInt(siul2_interrupt_config_t intConfig);

/*!
 * @brief Clears the individual pin external interrupt status flag.
 *
 * This function clears the individual pin external interrupt status flag.
 *
 * @param[in] eirqPinIdx IRQ pin
 */
static inline void PINS_ClearPinExIntFlag(uint32_t eirqPinIdx)
{
    //DEV_ASSERT(eirqPinIdx < SIUL2_INTERRUPT_COUNT);
    SIUL2_Type * base = SIUL2;

#ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
    base->DISR0 = 1UL << eirqPinIdx;
#else
    base->ISR0 = 1UL << eirqPinIdx;
#endif
}

/*!
 * @brief Gets the individual pin external interrupt status flag.
 *
 * This function gets the individual pin external interrupt status flag.
 *
 * @param[in] eirqPinIdx IRQ pin
 * @return Pin external status flag
 */
static inline bool PINS_GetPinExIntFlag(uint32_t eirqPinIdx)
{
    //DEV_ASSERT(eirqPinIdx < SIUL2_INTERRUPT_COUNT);
    const SIUL2_Type * base = SIUL2;

#ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
    return (base->DISR0 & (1UL << eirqPinIdx)) != 0U;
#else
    return (base->ISR0 & (1UL << eirqPinIdx)) != 0U;
#endif
}

/*!
 * @brief Clears the entire external interrupt status flag.
 *
 * This function clears the entire external interrupt status flag.
 *
 */
static inline void PINS_ClearExIntFlag(void)
{
    SIUL2_Type * base = SIUL2;

#ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
    base->DISR0 = 0xFFFFFFFFUL;
#else
    base->ISR0 = 0xFFFFFFFFUL;
#endif
}

/*!
 * @brief Reads the entire external interrupt status flag
 *
 * This function reads the entire external interrupt status flag.
 *
 * @return All 32 IRQ pins
 */
static inline uint32_t PINS_GetExIntFlag(void)
{
    const SIUL2_Type * base = SIUL2;

#ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
    return base->DISR0;
#else
    return base->ISR0;
#endif
}

/* Reverse bit order in each halfword independently */
static inline uint16_t REV_BIT_16(uint16_t value)
{
    uint8_t i;
    uint16_t ret = 0U;

    for (i = 0U; i < 8U; i++)
    {
        ret |= (uint16_t)((((value >> i) & 1U) << (15U - i)) | (((value << i) & 0x8000U) >> (15U - i)));
    }

    return ret;
}

/*!
 * @brief Write a pin of a port with a given value
 *
 * This function writes the given pin from a port, with the given value
 * ('0' represents LOW, '1' represents HIGH).
 *
 * @param base  GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param pin pin number to be written
 * @param value pin value to be written
 *        - 0: corresponding pin is set to LOW
 *        - 1: corresponding pin is set to HIGH
 */
static inline void PINS_WritePin(GPIO_Type * const base,
                                 pins_channel_type_t pin,
                                 pins_level_type_t value)
{
    //DEV_ASSERT(base != NULL);
    //DEV_ASSERT(pin < SIUL2_NUM_OF_PIN_PORT);
    //DEV_ASSERT(value <= 1U);
    pins_channel_type_t pinsValues = (pins_channel_type_t)base->PGPDO;
    uint16_t bitf = (uint16_t)((SIUL2_NUM_OF_PIN_PORT - 1U) - pin);
    pinsValues &= (pins_channel_type_t)(~((pins_channel_type_t)1U << bitf));
    pinsValues |= (pins_channel_type_t)(((pins_channel_type_t)value & 1U) << bitf);
    base->PGPDO = pinsValues;
}

/*!
 * @brief Write all pins of a port
 *
 * This function writes all pins configured as output with the values given in
 * the parameter pins. '0' represents LOW, '1' represents HIGH.
 *
 * @param base  GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param pins pin mask to be written
 *        - 0: corresponding pin is set to LOW
 *        - 1: corresponding pin is set to HIGH
 */
static inline void PINS_WritePins(GPIO_Type * const base,
                                  pins_channel_type_t pins)
{
    //DEV_ASSERT(base != NULL);
    base->PGPDO = REV_BIT_16(pins);
}

/*!
 * @brief Get the current output from a port
 *
 * This function returns the current output that is written to a port. Only pins
 * that are configured as output will have meaningful values.
 *
 * @param base  GPIO base pointer (PTA, PTB, PTC, etc.)
 * @return GPIO outputs. Each bit represents one pin (LSB is pin 0, MSB is pin
 * 31). For each bit:
 *        - 0: corresponding pin is set to LOW
 *        - 1: corresponding pin is set to HIGH
 */
static inline pins_channel_type_t PINS_GetPinsOutput(const GPIO_Type * const base)
{
    //DEV_ASSERT(base != NULL);
    pins_channel_type_t returnValue = (pins_channel_type_t)REV_BIT_16(base->PGPDO);
    return returnValue;
}

/*!
 * @brief Write pins with 'Set' value
 *
 * This function configures output pins listed in parameter pins (bits that are
 * '1') to have a value of 'set' (HIGH). Pins corresponding to '0' will be
 * unaffected.
 *
 * @param base  GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param pins pin mask of bits to be set.  Each bit represents one pin (LSB is
 * pin 0, MSB is pin 31). For each bit:
 *        - 0: corresponding pin is unaffected
 *        - 1: corresponding pin is set to HIGH
 */
static inline void PINS_SetPins(GPIO_Type * const base,
                                pins_channel_type_t pins)
{
    //DEV_ASSERT(base != NULL);
    base->PGPDO |= REV_BIT_16(pins);
}

/*!
 * @brief Write pins to 'Clear' value
 *
 * This function configures output pins listed in parameter pins (bits that are
 * '1') to have a 'cleared' value (LOW). Pins corresponding to '0' will be
 * unaffected.
 *
 * @param base  GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param pins pin mask of bits to be cleared.  Each bit represents one pin (LSB
 * is pin 0, MSB is pin 31). For each bit:
 *        - 0: corresponding pin is unaffected
 *        - 1: corresponding pin is cleared(set to LOW)
 */
static inline void PINS_ClearPins(GPIO_Type * const base,
                                  pins_channel_type_t pins)
{
    //DEV_ASSERT(base != NULL);
    base->PGPDO &= (pins_channel_type_t)(~REV_BIT_16(pins));
}

/*!
 * @brief Toggle pins value
 *
 * This function toggles output pins listed in parameter pins (bits that are
 * '1'). Pins corresponding to '0' will be unaffected.
 *
 * @param base  GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param pins pin mask of bits to be toggled.  Each bit represents one pin (LSB
 * is pin 0, MSB is pin 31). For each bit:
 *        - 0: corresponding pin is unaffected
 *        - 1: corresponding pin is toggled
 */
static inline void PINS_TogglePins(GPIO_Type * const base,
                                   pins_channel_type_t pins)
{
    //DEV_ASSERT(base != NULL);
    base->PGPDO ^= REV_BIT_16(pins);
}

/*!
 * @brief Read input pins
 *
 * This function returns the current input values from a port. Only pins
 * configured as input will have meaningful values.
 *
 * @param base  GPIO base pointer (PTA, PTB, PTC, etc.)
 * @return GPIO inputs. Each bit represents one pin (LSB is pin 0, MSB is pin
 * 31). For each bit:
 *        - 0: corresponding pin is read as LOW
 *        - 1: corresponding pin is read as HIGH
 */
static inline pins_channel_type_t PINS_ReadPins(const GPIO_Type * const base)
{
    //DEV_ASSERT(base != NULL);
    pins_channel_type_t returnValue = (pins_channel_type_t)REV_BIT_16(base->PGPDI);
    return returnValue;
}

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* SIUL2_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
