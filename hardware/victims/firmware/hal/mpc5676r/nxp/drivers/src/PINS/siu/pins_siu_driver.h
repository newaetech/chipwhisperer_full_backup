/*
 * Copyright 2018 NXP
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

#ifndef PINS_SIU_DRIVER_H
#define PINS_SIU_DRIVER_H

#include <stddef.h>
#include <stdbool.h>
#include "MPC5676R.h"
#include "MPC5676R_features.h"
#include "status.h" 
/*!
 * @defgroup pins_siu_driver PINS Driver
 * @ingroup pins
 * @details This section describes the programming interface of the PINS driver.
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The maximum of interrupt source can not be reached by SIU module */
#define SIU_INT_UNSUPPORTED           16u

/*!
 * @brief Type of a GPIO channel representation
 * Implements : pins_channel_type_t_Class
 */
typedef uint32_t pins_channel_type_t;

/*!
 * @brief Type of a port levels representation.
 * Implements : pins_level_type_t_Class
 */
typedef uint8_t pins_level_type_t;

/*!
 * @brief Internal resistor pull feature selection
 * Implements : port_pull_config_t_Class
 */
typedef enum
{
    PORT_INTERNAL_PULL_DOWN_ENABLED = 0U,  /*!< internal weak pull-down resistor is enabled     */
    PORT_INTERNAL_PULL_UP_ENABLED   = 1U,  /*!< internal weak pull-up resistor is enabled       */
    PORT_INTERNAL_PULL_NOT_ENABLED         /*!< internal weak pull-down/up resistor is disabled */
} port_pull_config_t;

/*!
 * @brief Configures the Output Buffer Enable field.
 * Implements : port_output_buffer_t_Class
 */
typedef enum
{
    PORT_OUTPUT_BUFFER_DISABLED     = 0U, /*!< Output buffer disabled */
    PORT_OUTPUT_BUFFER_ENABLED      = 1U  /*!< Output buffer enabled  */
} port_output_buffer_t;

/*!
 * @brief Configures the Input Buffer Enable field.
 * Implements : port_input_buffer_t_Class
 */
typedef enum
{
    PORT_INPUT_BUFFER_DISABLED      = 0U, /*!< Input buffer disabled */
    PORT_INPUT_BUFFER_ENABLED       = 1U  /*!< Input buffer enabled  */
} port_input_buffer_t;

/*!
 * @brief Configures the drive strength.
 * Implements : port_drive_strength_t_Class
 */
typedef enum
{
    PORT_MINIMUM_DRIVE_STRENGTH     = 0U, /*!< Minimum drive strength.     */
    PORT_MEDIUM_DRIVE_STRENGTH      = 1U, /*!< Medium drive strength.      */
    PORT_MEDIUM_HIGH_DRIVE_STRENGTH = 2U, /*!< Medium-high drive strength. */
    PORT_MAXIMUM_DRIVE_STRENGTH     = 3U  /*!< Maximum drive strength.     */
} port_drive_strength_t;

/*!
 * @brief Configures the Open Drain Enable field.
 * Implements : port_open_drain_t_Class
 */
typedef enum
{
    PORT_OPEN_DRAIN_DISABLED        = 0U, /*!< Output is CMOS       */
    PORT_OPEN_DRAIN_ENABLED         = 1U  /*!< Output is open drain */
} port_open_drain_t;

/*!
 * @brief Configures the Hysteresis Enable field.
 * Implements : port_hysteresis_t_Class
 */
typedef enum
{
    PORT_HYSTERESYS_DISABLED        = 0U, /*!< Input Hysteresis disabled */
    PORT_HYSTERESYS_ENABLED         = 1U  /*!< Input Hysteresis enabled  */
} port_hysteresis_t;

/*!
 * @brief Configures the slew rate control.
 * Implements : port_slew_rate_control_t_Class
 */
typedef enum
{
    HALF_STRENGTH_WITH_SLEWRATE_CONTROL     = 0U, /*!< Half Strength with Slewrate Control     */
    FULL_STRENGTH_WITH_SLEWRATE_CONTROL     = 1U, /*!< Full Strength with Slewrate Control     */
    HALF_STRENGTH_WITHOUT_SLEWRATE_CONTROL  = 2U, /*!< Half Strength without Slewrate Control  */
    FULL_STRENGTH_WITHOUT_SLEWRATE_CONTROL  = 3U  /*!< Full Strength without Slewrate Control  */
} port_slew_rate_control_t;

/*!
 * @brief Configures the Pin output muxing selection
 * Implements : port_mux_t_Class
 */
typedef enum
{
    PORT_MUX_AS_GPIO            = 0U, /*!< corresponding pin is configured as GPIO             */
    PORT_MUX_PRIMARY            = 1U, /*!< corresponding pin is configured as primary function */
    PORT_MUX_ALT1               = 2U, /*!< chip-specific                                       */
    PORT_MUX_ALT2               = 3U, /*!< chip-specific                                       */
    PORT_MUX_ALT3               = 4U  /*!< chip-specific                                       */
} port_mux_t;

/*!
 * @brief Configures the interrupt event condition.
 * Implements : siu_interrupt_edge_t_Class
 */
typedef enum
{
    SIU_INT_DISABLE             = 0x0U,  /*!< Interrupt disable.         */
    SIU_INT_RISING_EDGE         = 0x1U,  /*!< Interrupt on rising edge.  */
    SIU_INT_FALLING_EDGE        = 0x2U,  /*!< Interrupt on falling edge. */
    SIU_INT_EITHER_EDGE         = 0x3U   /*!< Interrupt on either edge.  */
} siu_interrupt_edge_t;

/*!
 * @brief Configures the type of interrupt request.
 * Implements : siu_interrupt_type_t_Class
 */
typedef enum
{
    SIU_INT_DMA_IRQ_RQUEST      = 0x0U,  /*!< DMA/Interrupt request enabled.              */
    SIU_INT_OVERRUN_REQUEST     = 0x1U,  /*!< Overrun request enabled.                    */
    SIU_INT_ALL_REQUEST         = 0x2U   /*!< DMA/Interrupt and Overrun requests enabled. */
} siu_interrupt_type_t;

/*!
 * @brief Configures the interrupt DMA select.
 * Implements : siu_interrupt_dma_select_t_Class
 */
typedef enum
{
    SIU_INT_USING_INTERRUPT     = 0x0U, /*!< Executived by an interrupt request */
    SIU_INT_USING_DMA           = 0x1U  /*!< Executived by an DMA request       */
} siu_interrupt_dma_select_t;

/*!
 * @brief Configures the source for the external interrupt/DMA.
 * Implements : siu_interrupt_source_t_Class
 */
typedef enum
{
    SIU_INT_SOURCE_IRQ          = 0x0U,  /*!< Source is IRQ input pin */
    SIU_INT_SOURCE_1            = 0x1U,  /*!< Chip-specific.          */
    SIU_INT_SOURCE_2            = 0x2U,  /*!< Chip-specific.          */
    SIU_INT_SOURCE_3            = 0x3U   /*!< Chip-specific.          */
} siu_interrupt_source_t;

/*!
 * @brief Interrupt configuration structure.
 * Implements : siu_interrupt_config_t_Class
 */
typedef struct
{
    uint32_t                   eirqPinIdx;    /*!< The interrupt pin index                   */
    siu_interrupt_type_t       intTypeSel;    /*!< The type of interrupt request             */
    siu_interrupt_edge_t       intEdgeSel;    /*!< The type of edge event                    */
    siu_interrupt_source_t     intSource;     /*!< The source for the external interrupt/DMA */
    siu_interrupt_dma_select_t intExeSel;     /*!< Switch between DMA and interrupt request  */
} siu_interrupt_config_t;

/*!
 * @brief Configures the Pin input muxing selection
 * Implements : port_input_mux_t_Class
 */
typedef enum
{
    PORT_INPUT_MUX_ALT0         = 0U,   /*!< Chip-specific.    */
    PORT_INPUT_MUX_ALT1         = 1U,   /*!< Chip-specific.    */
    PORT_INPUT_MUX_ALT2         = 2U,   /*!< Chip-specific.    */
    PORT_INPUT_MUX_ALT3         = 3U,   /*!< Chip-specific.    */
    PORT_INPUT_MUX_NO_INIT              /*!< No initialization */
} port_input_mux_t;

/*!
 * @brief Configuration structure for input signal.
 * Implements : siu_input_config_t_Class
 */
typedef struct
{
    port_input_mux_t            muxSrc; /*!< Configures the input muxing source    */
    uint8_t                     muxSel; /*!< Configures the input muxing selection */
    uint32_t                    muxReg; /*!< Configures the input muxing register  */
} siu_input_config_t;

/*!
 * @brief Defines the converter configuration
 *
 * This structure is used to configure the pins
 * Implements : pin_settings_config_t_Class
 */
typedef struct
{
    SIU_Type          *         base;         /*!< SIU base pointer.                         */
    uint32_t                    pinPortIdx;   /*!< Port pin number.                          */
    port_pull_config_t          pullConfig;   /*!< Internal resistor pull feature selection. */
    port_open_drain_t           openDrain;    /*!< Configures open drain.                    */
    port_drive_strength_t       driveSelect;  /*!< Configures the drive strength.            */
    port_mux_t                  mux;          /*!< Pin output muxing selection.              */
    port_slew_rate_control_t    slewRateCtrl; /*!< Configures the Slew Rate Control field.   */
    port_hysteresis_t           hysteresis;   /*!< Configures the Hysteresis Enable.         */
    port_output_buffer_t        outputBuffer; /*!< Configures the Output Buffer Enable.      */
    port_input_buffer_t         inputBuffer;  /*!< Configures the Input Buffer Enable.       */
    siu_input_config_t          inputConfig;  /*!< Configures for the input signal.          */
    siu_interrupt_config_t      intConfig;    /*!< Interrupt generation condition.           */
    pins_level_type_t           initValue;    /*!< Initial value.                            */
} pin_settings_config_t;

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
 * @brief Configures the internal resistor.
 *
 * This function configures the internal resistor.
 *
 * @param[in] base Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin Port pin number
 * @param[in] pullConfig The pull configuration
 */
void PINS_DRV_SetPullSel(PORT_Type * const base,
                         uint32_t pin,
                         port_pull_config_t pullConfig);

/*!
 * @brief Configures the output muxing and gate.
 *
 * This function configures the output muxing and gate.
 *
 * @param[in] base Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin Port pin number
 * @param[in] enable Enable output buffer
 * @param[in] mux Pin muxing slot selection
 */
void PINS_DRV_ConfigOutputMux(PORT_Type * const base,
                              uint32_t pin,
                              bool enable,
                              port_mux_t mux);

/*!
 * @brief Configures the input muxing and gate.
 *
 * This function configures the input muxing and gate.
 *
 * @param[in] base Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin Port pin number
 * @param[in] enable Enable input buffer
 * @param[in] mux Pin muxing slot selection
 * @param[in] inputConfig structure configuration for input signal
 */
void PINS_DRV_ConfigInputMux(PORT_Type * const base,
                             uint32_t pin,
                             bool enable,
                             port_mux_t mux,
                             siu_input_config_t inputConfig);

/*!
 * @brief Configures the digital filter length.
 *
 * This function configures the digital filter length specifies the number
 * system clocks that define the period of the digital filter and minimum
 * time an IRQ and NMI signal must hold.
 *
 * @param[in] length The digital filter length
 */
void PINS_DRV_ConfigDigitalFilterLength(uint8_t length);

/*!
 * @brief Configures the external interrupt.
 *
 * This function configures the external interrupt.
 *
 * @param[in] intConfig The configuration for interrupt pin
 */
void PINS_DRV_SetExInt(siu_interrupt_config_t intConfig);

/*!
 * @brief Clears the individual pin external interrupt status flag.
 *
 * This function clears the individual pin external interrupt status flag.
 *
 * @param[in] eirqPinIdx IRQ pin
 */
void PINS_DRV_ClearPinExIntFlag(uint32_t eirqPinIdx);

/*!
 * @brief Gets the individual pin external interrupt status flag.
 *
 * This function gets the individual pin external interrupt status flag.
 *
 * @param[in] eirqPinIdx IRQ pin
 * @return Pin external status flag
 */
bool PINS_DRV_GetPinExIntFlag(uint32_t eirqPinIdx);

/*!
 * @brief Clears the entire external interrupt status flag.
 *
 * This function clears the entire external interrupt status flag.
 */
void PINS_DRV_ClearExIntFlag(void);

/*!
 * @brief Reads the entire external interrupt status flag.
 *
 * This function reads the entire external interrupt status flag.
 *
 * @return All 16 IRQ pins
 */
uint32_t PINS_DRV_GetExIntFlag(void);

/*!
 * @brief Clears the entire overrun interrupt status flag.
 *
 * This function clears the entire overrun interrupt status flag.
 */
void PINS_DRV_ClearOverrunIntFlag(void);

/*!
 * @brief Reads the entire overrun interrupt status flag.
 *
 * This function reads the entire overrun interrupt status flag.
 *
 * @return All 16 IRQ pins
 */
uint32_t PINS_DRV_GetOverrunIntFlag(void);

/*!
 * @brief Gets the entire filter input values of IRQ and NMI pins.
 *
 * This function gets the entire filter input values of IRQ and NMI pins.
 *
 * @return All 16 IRQ and NMI pins
 */
uint32_t PINS_DRV_GetFilterInputValue(void);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* PINS_SIU_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/