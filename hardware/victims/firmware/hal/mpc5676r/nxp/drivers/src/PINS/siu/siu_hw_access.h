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
 * @file siu_hw_access.h
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
 * This is required by the conversion of a MARCO and number into address number
 * to increase source address.
 */

#ifndef SIU_HW_ACCESS_H
#define SIU_HW_ACCESS_H

#include "pins_siu_driver.h"
#include <stdbool.h>
/*!
 * siu_hw_access System Integration Unit (SIU)
 * @brief This module covers the functionality of the SIU peripheral.
 * <p>
 *  SIU HW ACCESS provides the API for reading and writing register bit-fields belonging to the SIU module.
 * </p>
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The number of possible interrupt source for external pin */
#define SIU_INTERRUPT_COUNT           16u
/* The maximum number of pins on a port */
#define SIU_NUM_OF_PIN_PORT           32u

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
                                   uint32_t pin,
                                   port_pull_config_t pullConfig)
{
    //DEV_ASSERT(base != NULL);
    //DEV_ASSERT(pin < SIU_NUM_OF_PIN_PORT);
    uint16_t regVal;
    if (pullConfig == PORT_INTERNAL_PULL_NOT_ENABLED)
    {
        base->PCR[pin] &= ~SIU_PCR_WPE_MASK;
    }
    else
    {
        regVal = base->PCR[pin];
        regVal |= SIU_PCR_WPE_MASK;
        regVal &= ~SIU_PCR_WPS_MASK;
        regVal |= SIU_PCR_WPS(pullConfig);
        base->PCR[pin] = regVal;
    }
}

/*!
 * @brief Configures the output muxing and gate.
 *
 * This function configures the output muxing and gate.
 *
 * @param[in] base Port base pointer
 * @param[in] pin Port pin number
 * @param[in] enable Enable output buffer
 * @param[in] mux pin muxing slot selection
 */
static inline void PINS_ConfigOutputMux(PORT_Type * const base,
                                        uint32_t pin,
                                        bool enable,
                                        port_mux_t mux)
{
    //DEV_ASSERT(base != NULL);
    //DEV_ASSERT(pin < SIU_NUM_OF_PIN_PORT);
    uint16_t regVal;
    regVal = base->PCR[pin];
    regVal &= ~SIU_PCR_OBE_MASK;
    regVal |= SIU_PCR_OBE(enable ? 1UL : 0UL);
    regVal &= ~SIU_PCR_PA_MASK;
    regVal |= SIU_PCR_PA(mux);
    base->PCR[pin] = regVal;
}

/*!
 * @brief Configures the the input muxing and gate.
 *
 * This function configures the the input muxing and gate.
 *
 * @param[in] base Port base pointer
 * @param[in] pin Port pin number
 * @param[in] enable Enable input buffer
 * @param[in] mux Pin muxing slot selection
 * @param[in] inputConfig structure configuration for input signal
 */
void PINS_ConfigInputMux(PORT_Type * const base,
                         uint32_t pin,
                         bool enable,
                         port_mux_t mux,
                         siu_input_config_t inputConfig);

/*!
 * @brief Configures the digital filter length.
 *
 * This function configures the digital filter length..
 *
 * @param[in] length The digital filter length
 */
static inline void PINS_ConfigDigitalFilterLength(uint8_t length)
{
    SIU->IDFR = (uint32_t)((uint32_t)length & SIU_IDFR_DFL_MASK);
}

/*!
 * @brief Configures the external interrupt.
 *
 * This function configures the external interrupt.
 *
 * @param[in] intConfig The configuration for interrupt pin
 */
void PINS_SetExInt(siu_interrupt_config_t intConfig);

/*!
 * @brief Clears the individual pin external interrupt status flag.
 *
 * This function clears the individual pin external interrupt status flag.
 *
 * @param[in] eirqPinIdx IRQ pin
 */
static inline void PINS_ClearPinExIntFlag(uint32_t eirqPinIdx)
{
    //DEV_ASSERT(eirqPinIdx < SIU_INTERRUPT_COUNT);
    //DEV_ASSERT(0UL != ((1UL << eirqPinIdx) & 0xC000FFFFUL));

    SIU->EISR = 1UL << eirqPinIdx;
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
    //DEV_ASSERT(eirqPinIdx < SIU_INTERRUPT_COUNT);
    //DEV_ASSERT(0UL != ((1UL << eirqPinIdx) & 0xC000FFFFUL));

    return (SIU->EISR & (1UL << eirqPinIdx)) != 0U;
}

/*!
 * @brief Clears the entire external interrupt status flag.
 *
 * This function clears the entire external interrupt status flag.
 *
 */
static inline void PINS_ClearExIntFlag(void)
{
    SIU->EISR = 0xC000FFFFUL;
}

/*!
 * @brief Reads the entire external interrupt status flag
 *
 * This function reads the entire external interrupt status flag.
 *
 * @return All IRQ pins
 */
static inline uint32_t PINS_GetExIntFlag(void)
{
    return (uint32_t)SIU->EISR;
}

/*!
 * @brief Clears the entire overrun interrupt status flag.
 *
 * This function clears the entire overrun interrupt status flag.
 *
 */
static inline void PINS_ClearOverrunIntFlag(void)
{
    SIU->OSR = 0x0000FFFFUL;
}

/*!
 * @brief Reads the entire overrun interrupt status flag
 *
 * This function reads the entire overrun interrupt status flag.
 *
 * @return All IRQ pins
 */
static inline uint32_t PINS_GetOverrunIntFlag(void)
{
    return (uint32_t)SIU->OSR;
}

/*!
 * @brief Gets the entire filter input values.
 *
 * This function gets the entire filter input values of IRQ and NMI pins.
 *
 * @return All IRQ and NMI pins
 */
static inline uint32_t PINS_GetFilterInputValue(void)
{
    return (uint32_t)SIU->IFIR;
}

/* Reverse bit order in each halfword independently */
static inline uint32_t REV_BIT_32(uint32_t value)
{
    uint8_t i;
    uint32_t ret = 0U;

    for (i = 0U; i < 16U; i++)
    {
        ret |= (uint32_t)((((value >> i) & 1U) << (31U - i)) | (((value << i) & 0x80000000U) >> (31U - i)));
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
    //DEV_ASSERT(pin < SIU_NUM_OF_PIN_PORT);
    //DEV_ASSERT(value <= 1u);
    pins_channel_type_t pinsValues = (pins_channel_type_t)base->PGPDO;
    uint16_t bitf = (uint16_t)((SIU_NUM_OF_PIN_PORT - 1U) - pin);
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
    base->PGPDO = REV_BIT_32(pins);
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
    pins_channel_type_t returnValue = (pins_channel_type_t)REV_BIT_32(base->PGPDO);
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
    base->PGPDO |= REV_BIT_32(pins);
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
    base->PGPDO &= (pins_channel_type_t)(~REV_BIT_32(pins));
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
    base->PGPDO ^= REV_BIT_32(pins);
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
    pins_channel_type_t returnValue = (pins_channel_type_t)REV_BIT_32(base->PGPDI);
    return returnValue;
}

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* SIU_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/