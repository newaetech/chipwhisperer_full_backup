/*
 * Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
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

#ifndef PINS_DRIVER_H
#define PINS_DRIVER_H

#include "MPC5676R.h"

/*
 * Include the cpu specific pins API header files.
 */

#if (defined(S32K14x_SERIES) || defined(S32K11x_SERIES) || defined(S32MTV_SERIES))
    /* Pins driver using PORT/GPIO header file */
    #include "../src/pins/port/pins_port_driver.h"
#elif (defined(MPC574x_SERIES) || defined(S32R_SERIES) || defined(S32V23x_SERIES))
    /* Pins driver using SIUL2 header file */
    #include "../src/pins/siul2/pins_siul2_driver.h"
#elif (defined(S32S247TV_SERIES))
    /* Pins driver using SIUL2 header file, support multiple instances */
    #include "../src/pins/siul2_multi_regions/pins_siul2_driver.h"
#elif (defined(MPC577x_SERIES)||defined(MPC5676R_SERIES))
    /* Pins driver using SIU header file */
    #include "../src/pins/siu/pins_siu_driver.h"
#else
    #error "No valid CPU defined!"
#endif

/*!
 * @defgroup pins_driver PINS Driver
 * @ingroup pins
 * @details This section describes the programming interface of the PINS driver.
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @name PINS DRIVER API.
 * @{
 */
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the pins with the given configuration structure
 *
 * This function configures the pins with the options provided in the
 * provided structure.
 *
 * @param[in] pinCount The number of configured pins in structure
 * @param[in] config The configuration structure
 * @return The status of the operation
 */
status_t PINS_DRV_Init(uint32_t pinCount,
                       const pin_settings_config_t config[]);

/*!
 * @brief Write a pin of a port with a given value
 *
 * This function writes the given pin from a port, with the given value
 * ('0' represents LOW, '1' represents HIGH).
 *
 * @param[in] base GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param[in] pin Pin number to be written
 * @param[in] value Pin value to be written
 *        - 0: corresponding pin is set to LOW
 *        - 1: corresponding pin is set to HIGH
 */
void PINS_DRV_WritePin(GPIO_Type * const base,
                       pins_channel_type_t pin,
                       pins_level_type_t value);

/*!
 * @brief Write all pins of a port
 *
 * This function writes all pins configured as output with the values given in
 * the parameter pins. '0' represents LOW, '1' represents HIGH.
 *
 * @param[in] base GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param[in] pins Pin mask to be written
 *        - 0: corresponding pin is set to LOW
 *        - 1: corresponding pin is set to HIGH
 */
void PINS_DRV_WritePins(GPIO_Type * const base,
                        pins_channel_type_t pins);

/*!
 * @brief Get the current output from a port
 *
 * This function returns the current output that is written to a port. Only pins
 * that are configured as output will have meaningful values.
 *
 * @param[in] base GPIO base pointer (PTA, PTB, PTC, etc.)
 * @return GPIO outputs. Each bit represents one pin (LSB is pin 0, MSB is pin
 * 31). For each bit:
 *        - 0: corresponding pin is set to LOW
 *        - 1: corresponding pin is set to HIGH
 */
pins_channel_type_t PINS_DRV_GetPinsOutput(const GPIO_Type * const base);

/*!
 * @brief Write pins with 'Set' value
 *
 * This function configures output pins listed in parameter pins (bits that are
 * '1') to have a value of 'set' (HIGH). Pins corresponding to '0' will be
 * unaffected.
 *
 * @param[in] base GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param[in] pins Pin mask of bits to be set. Each bit represents one pin (LSB is
 * pin 0, MSB is pin 31). For each bit:
 *        - 0: corresponding pin is unaffected
 *        - 1: corresponding pin is set to HIGH
 */
void PINS_DRV_SetPins(GPIO_Type * const base,
                      pins_channel_type_t pins);

/*!
 * @brief Write pins to 'Clear' value
 *
 * This function configures output pins listed in parameter pins (bits that are
 * '1') to have a 'cleared' value (LOW). Pins corresponding to '0' will be
 * unaffected.
 *
 * @param[in] base GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param[in] pins Pin mask of bits to be cleared. Each bit represents one pin (LSB
 * is pin 0, MSB is pin 31). For each bit:
 *        - 0: corresponding pin is unaffected
 *        - 1: corresponding pin is cleared(set to LOW)
 */
void PINS_DRV_ClearPins(GPIO_Type * const base,
                        pins_channel_type_t pins);

/*!
 * @brief Toggle pins value
 *
 * This function toggles output pins listed in parameter pins (bits that are
 * '1'). Pins corresponding to '0' will be unaffected.
 *
 * @param[in] base GPIO base pointer (PTA, PTB, PTC, etc.)
 * @param[in] pins Pin mask of bits to be toggled.  Each bit represents one pin (LSB
 * is pin 0, MSB is pin 31). For each bit:
 *        - 0: corresponding pin is unaffected
 *        - 1: corresponding pin is toggled
 */
void PINS_DRV_TogglePins(GPIO_Type * const base,
                         pins_channel_type_t pins);

/*!
 * @brief Read input pins
 *
 * This function returns the current input values from a port. Only pins
 * configured as input will have meaningful values.
 *
 * @param[in] base GPIO base pointer (PTA, PTB, PTC, etc.)
 * @return GPIO inputs. Each bit represents one pin (LSB is pin 0, MSB is pin
 * 31). For each bit:
 *        - 0: corresponding pin is read as LOW
 *        - 1: corresponding pin is read as HIGH
 */
pins_channel_type_t PINS_DRV_ReadPins(const GPIO_Type * const base);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* PINS_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/