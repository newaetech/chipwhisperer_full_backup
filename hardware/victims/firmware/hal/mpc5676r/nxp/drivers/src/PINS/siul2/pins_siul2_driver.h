/*
 * Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
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

#ifndef PINS_SIUL2_DRIVER_H
#define PINS_SIUL2_DRIVER_H

#include <stddef.h>
#include "MPC5676R.h"
#include "MPC5676R_features.h"
#include "status.h"

/*!
 * @defgroup pins_siul2_driver PINS Driver
 * @ingroup pins
 * @details This section describes the programming interface of the PINS driver.
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @brief Type of a GPIO channel representation
 * Implements : pins_channel_type_t_Class
 */
typedef uint16_t pins_channel_type_t;

/*!
 * @brief Type of a port levels representation.
 * Implements : pins_level_type_t_Class
 */
typedef uint8_t pins_level_type_t;

/*!
 * @brief Configures the port data direction
 * Implements : port_data_direction_t_Class
 */
typedef enum
{
    GPIO_INPUT_DIRECTION                 = 0U,  /*!< General purpose input direction.       */
    GPIO_OUTPUT_DIRECTION                = 1U,  /*!< General purpose output direction.      */
    GPIO_UNSPECIFIED_DIRECTION           = 2U   /*!< General purpose unspecified direction. */
} port_data_direction_t;

/*!
 * @brief Internal resistor pull feature selection
 * Implements : port_pull_config_t_Class
 */
typedef enum
{
#ifdef FEATURE_SIUL2_HAS_PULL_KEEPER
    PORT_INTERNAL_PULL_DOWN_ENABLED      = 0U,  /*!< internal 100 kOhm pulldown enabled.         */
    PORT_INTERNAL_PULL_UP_MEDIUM         = 1U,  /*!< internal 50 kOhm pullup enabled.            */
    PORT_INTERNAL_PULL_UP_HIGH           = 2U,  /*!< internal 100 kOhm pullup enabled.           */
    PORT_INTERNAL_PULL_UP_LOW            = 3U,  /*!< internal 33 kOhm pullup enabled.            */
#else
    PORT_INTERNAL_PULL_DOWN_ENABLED      = 0U,  /*!< internal pull-down resistor is enabled.     */
    PORT_INTERNAL_PULL_UP_ENABLED        = 1U,  /*!< internal pull-up resistor is enabled.       */
#endif /* FEATURE_SIUL2_HAS_PULL_KEEPER */
    PORT_INTERNAL_PULL_NOT_ENABLED              /*!< internal pull-down/up resistor is disabled. */
} port_pull_config_t;

#ifdef FEATURE_SIUL2_HAS_DDR_PAD
/*!
 * @brief Configures the DDR type to:
 *      DDR3, DDR3L modes   - 00
 *      LPDDR2 mode         - 10
 * Implements : port_ddr_type_t_Class
 */
typedef enum
{
    PORT_DDR_DDR3_MODE           = 0x0U,  /*!< DDR3 / LPDDR3 mode  */
    PORT_DDR_LPDDR2_MODE         = 0x2U   /*!< LPDDR2 mode         */
} port_ddr_type_t;

/*!
 * @brief Configures the DDR trimming delay to:
 *      min delay           - 00
 *      +50ps delay         - 01
 *      +100ps delay        - 10
 *      +150ps delay        - 11
 * Implements : port_ddr_trim_delay_t_Class
 */
typedef enum
{
    PORT_DDR_MIN_DELAY           = 0x0U,  /*!< min delay     */
    PORT_DDR_50PS_DELAY          = 0x1U,  /*!< +50ps delay   */
    PORT_DDR_100PS_DELAY         = 0x2U,  /*!< +100ps delay  */
    PORT_DDR_150PS_DELAY         = 0x3U   /*!< +150ps delay  */
} port_ddr_trim_delay_t;

/*!
 * @brief Configures the DDR crosspoint adjusment of DDR CLK cell output signal to:
 *      No crosspoint change                    - 00
 *      Output crosspoint is +70mV higher       - 01
 *      Output crosspoint is -70mV lower        - 10
 *      Output crosspoint is +140mV higher      - 11
 * Implements : port_ddr_crpoint_t_Class
 */
typedef enum
{
    PORT_DDR_NO_CRPOINT          = 0x0U,  /*!< No crosspoint change                */
    PORT_DDR_MINUS_CRPOINT       = 0x1U,  /*!< Output crosspoint is +70mV to -70mV */
    PORT_DDR_PLUS_CRPOINT        = 0x2U,  /*!< Output crosspoint is -70mV to +70mV */
    PORT_DDR_DOUBLE_CRPOINT      = 0x3U   /*!< Output crosspoint is +140mV higher  */
} port_ddr_crpoint_t;

/*!
 * @brief Configures the DDR output signal duty cycle adjustment to:
 *      No duty cycle change            - 00
 *      Left side input pulse shrink    - 01
 *      Right side input pulse shrink   - 10
 *      Right side input pulse shrink   - 11
 * Implements : port_ddr_trim_t_Class
 */
typedef enum
{
    PORT_DDR_NO_TRIM             = 0x0U,  /*!< No duty cycle change             */
    PORT_DDR_LEFT_TRIM           = 0x1U,  /*!< Left side input pulse shrink     */
    PORT_DDR_RIGHT_TRIM          = 0x2U   /*!< Right side input pulse shrink    */
} port_ddr_trim_t;

/*!
 * @brief Configures DDR input to select differential or CMOS input receiver mode.
 * Implements : port_ddr_input_t_Class
 */
typedef enum
{
    PORT_DDR_INPUT_CMOS          = 0U, /*!< CMOS input receiver mode             */
    PORT_DDR_INPUT_DIFFERENTIAL  = 1U  /*!< differential DDR input receiver mode */
} port_ddr_input_t;

/*!
 * @brief Configures the On die Termination to select ODT strength in DDR3 mode.
 * Implements : port_ddr_termination_t_Class
 */
typedef enum
{
    PORT_DDR_NO_ODT              = 0U, /*!< Not applicable                         */
    PORT_DDR_120_OHM_ODT         = 1U, /*!< Termination resistor is set to 120 Ohm */
    PORT_DDR_60_OHM_ODT          = 2U, /*!< Termination resistor is set to 60 Ohm  */
    PORT_DDR_40_OHM_ODT          = 3U, /*!< Termination resistor is set to 40 Ohm  */
    PORT_DDR_30_OHM_ODT          = 4U, /*!< Termination resistor is set to 30 Ohm  */
    PORT_DDR_24_OHM_ODT          = 5U, /*!< Termination resistor is set to 24 Ohm  */
    PORT_DDR_20_OHM_ODT          = 6U, /*!< Termination resistor is set to 20 Ohm  */
    PORT_DDR_17_OHM_ODT          = 7U  /*!< Termination resistor is set to 17 Ohm  */
} port_ddr_termination_t;

/*!
 * @brief Defines the DDR configuration
 *
 * This structure is used to configure the pins for the DDR controller
 * Implements : pin_ddr_config_t_Class
 */
typedef struct
{
    port_ddr_input_t            ddrInputMode;           /*!< DDR input mode           */
    port_ddr_type_t             ddrSelection;           /*!< DDR type                 */
    port_ddr_trim_delay_t       trimmingDelay;          /*!< DDR trimming delay value */
    port_ddr_crpoint_t          crosspointAdjustment;   /*!< DDR crosspoint adjusment */
    port_ddr_trim_t             trimmingAdjustment;     /*!< DDR trimming type        */
    port_ddr_termination_t      onDieTermination;       /*!< DDR on die Termination   */
} pin_ddr_config_t;
#endif /* FEATURE_SIUL2_HAS_DDR_PAD */

/*!
 * @brief Configures the Pin output muxing selection
 * Implements : port_mux_t_Class
 */
typedef enum
{
    PORT_MUX_AS_GPIO            = 0U,   /*!< corresponding pin is configured as GPIO */
    PORT_MUX_ALT1               = 1U,   /*!< chip-specific                           */
    PORT_MUX_ALT2               = 2U,   /*!< chip-specific                           */
    PORT_MUX_ALT3               = 3U,   /*!< chip-specific                           */
    PORT_MUX_ALT4               = 4U,   /*!< chip-specific                           */
    PORT_MUX_ALT5               = 5U,   /*!< chip-specific                           */
    PORT_MUX_ALT6               = 6U,   /*!< chip-specific                           */
    PORT_MUX_ALT7               = 7U,   /*!< chip-specific                           */
    PORT_MUX_ALT8               = 8U,   /*!< chip-specific                           */
    PORT_MUX_ALT9               = 9U,   /*!< chip-specific                           */
    PORT_MUX_ALT10              = 10U,  /*!< chip-specific                           */
    PORT_MUX_ALT11              = 11U,  /*!< chip-specific                           */
    PORT_MUX_ALT12              = 12U,  /*!< chip-specific                           */
    PORT_MUX_ALT13              = 13U,  /*!< chip-specific                           */
    PORT_MUX_ALT14              = 14U,  /*!< chip-specific                           */
    PORT_MUX_ALT15              = 15U   /*!< chip-specific                           */
} port_mux_t;

/*!
 * @brief Configures the interrupt generation condition.
 * Implements : siul2_interrupt_type_t_Class
 */
typedef enum
{
    SIUL2_INT_DISABLE           = 0x0U,  /*!< Interrupt disable.         */
    SIUL2_INT_RISING_EDGE       = 0x1U,  /*!< Interrupt on rising edge.  */
    SIUL2_INT_FALLING_EDGE      = 0x2U,  /*!< Interrupt on falling edge. */
    SIUL2_INT_EITHER_EDGE       = 0x3U   /*!< Interrupt on either edge.  */
} siul2_interrupt_type_t;

#ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
/*!
 * @brief Configures the interrupt DMA select.
 * Implements : siul2_interrupt_dma_select_t_Class
 */
typedef enum
{
    SIUL2_INT_USING_INTERUPT    = 0x0U, /*!< Executived by an interrupt request */
    SIUL2_INT_USING_DMA         = 0x1U  /*!< Executived by an DMA request       */
} siul2_interrupt_dma_select_t;
#endif /* FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA */

/*!
 * @brief Interrupt configuration structure.
 * Implements : siul2_interrupt_config_t_Class
 */
typedef struct
{
    uint8_t                      eirqPinIdx;    /*!< The interrupt pin index                  */
    siul2_interrupt_type_t       intEdgeSel;    /*!< The type of edge event                   */
    bool                         digitalFilter; /*!< Enables digital filter                   */
    uint8_t                      maxCnt;        /*!< Maximum interrupt filter value           */
#ifdef FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA
    siul2_interrupt_dma_select_t intExeSel;     /*!< Switch between DMA and interrupt request */
#endif /* FEATURE_SIUL2_EXTERNAL_INT_SUPPORT_DMA */
} siul2_interrupt_config_t;

#ifdef FEATURE_SIUL2_HAS_OUTPUT_DRIVE_CONTROL
/*!
 * @brief Configures the output drive control.
 * Implements : port_output_drive_control_t_Class
 */
typedef enum
{
    PORT_OUTPUT_DRIVE_DISABLED    = 0U,  /*!< Output drive control disabled    */
    PORT_OUTPUT_DRIVE_OPEN_DRAIN  = 1U,  /*!< Output drive control Open-Drain  */
    PORT_OUTPUT_DRIVE_PUSH_PULL   = 2U,  /*!< Output drive control Push-Pull   */
    PORT_OUTPUT_DRIVE_OPEN_SOURCE = 3U   /*!< Output drive control Open-Source */
} port_output_drive_control_t;
#else
/*!
 * @brief Configures the Open Drain Enable field.
 * Implements : port_open_drain_t_Class
 */
typedef enum
{
    PORT_OPEN_DRAIN_DISABLED     = 0U, /*!< Output is CMOS       */
    PORT_OPEN_DRAIN_ENABLED      = 1U  /*!< Output is open drain */
} port_open_drain_t;

/*!
 * @brief Configures the Output Buffer Enable field.
 * Implements : port_output_buffer_t_Class
 */
typedef enum
{
    PORT_OUTPUT_BUFFER_DISABLED  = 0U, /*!< Output buffer disabled */
    PORT_OUTPUT_BUFFER_ENABLED   = 1U  /*!< Output buffer enabled  */
} port_output_buffer_t;
#endif /* FEATURE_SIUL2_HAS_OUTPUT_DRIVE_CONTROL */

/*!
 * @brief Configures the Input Buffer Enable field.
 * Implements : port_input_buffer_t_Class
 */
typedef enum
{
    PORT_INPUT_BUFFER_DISABLED   = 0U, /*!< Input buffer disabled */
    PORT_INPUT_BUFFER_ENABLED    = 1U  /*!< Input buffer enabled  */
} port_input_buffer_t;

/*!
 * @brief Configures the Hysteresis Enable field.
 * Implements : port_hysteresis_t_Class
 */
typedef enum
{
#ifdef FEATURE_SIUL2_HAS_HYSTERESYS_CMOS_SCHMITT
    PORT_HYSTERESYS_CMOS         = 0U, /*!< CMOS Input            */
    PORT_HYSTERESYS_SCHMITT      = 1U  /*!< Schmitt trigger input */
#else
    PORT_HYSTERESYS_DISABLED     = 0U, /*!< Input Hysteresis disabled */
    PORT_HYSTERESYS_ENABLED      = 1U  /*!< Input Hysteresis enabled  */
#endif
} port_hysteresis_t;

#ifdef FEATURE_SIUL2_HAS_INVERT_DATA_OUTPUT
/*!
 * @brief Configures the Invert Data Output field.
 * Implements : port_invert_output_t_Class
 */
typedef enum
{
    PORT_INVERT_OUTPUT_DISABLED  = 0U, /*!< Invert Data Output disabled */
    PORT_INVERT_OUTPUT_ENABLED   = 1U  /*!< Invert Data Output enabled  */
} port_invert_output_t;
#endif /* FEATURE_SIUL2_HAS_INVERT_DATA_OUTPUT */

#ifdef FEATURE_SIUL2_HAS_INVERT_DATA_INPUT
/*!
 * @brief Configures the Invert Data Input field.
 * Implements : port_invert_input_t_Class
 */
typedef enum
{
    PORT_INVERT_INPUT_DISABLED   = 0U, /*!< Invert Data Input disabled  */
    PORT_INVERT_INPUT_ENABLED    = 1U  /*!< Invert Data Input enabled   */
} port_invert_input_t;
#endif /* FEATURE_SIUL2_HAS_INVERT_DATA_INPUT */

#ifdef FEATURE_SIUL2_HAS_ANALOG_PAD
/*!
 * @brief Configures the Analog Pad Control.
 * Implements : port_analog_pad_t_Class
 */
typedef enum
{
    PORT_ANALOG_PAD_CONTROL_DISABLED = 0U, /*!< Disable (the switch is off)                                 */
    PORT_ANALOG_PAD_CONTROL_ENABLED  = 1U  /*!< Enable (another module can control the state of the switch) */
} port_analog_pad_t;
#endif /* FEATURE_SIUL2_HAS_ANALOG_PAD */

/*!
 * @brief Configures the Pin input muxing selection
 * Implements : port_input_mux_t_Class
 */
typedef enum
{
    PORT_INPUT_MUX_ALT0      = 0U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT1      = 1U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT2      = 2U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT3      = 3U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT4      = 4U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT5      = 5U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT6      = 6U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT7      = 7U,   /*!< Chip-specific     */
#if (FEATURE_SIUL2_INPUT_SOURCE_SELECT_WIDTH >= 4U)
    PORT_INPUT_MUX_ALT8      = 8U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT9      = 9U,   /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT10     = 10U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT11     = 11U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT12     = 12U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT13     = 13U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT14     = 14U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT15     = 15U,  /*!< Chip-specific     */
#endif
#if (FEATURE_SIUL2_INPUT_SOURCE_SELECT_WIDTH >= 8U)
    PORT_INPUT_MUX_ALT16     = 16U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT17     = 17U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT18     = 18U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT19     = 19U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT20     = 20U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT21     = 21U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT22     = 22U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT23     = 23U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT24     = 24U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT25     = 25U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT26     = 26U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT48     = 48U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT49     = 49U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT50     = 50U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT51     = 51U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT52     = 52U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT53     = 53U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT54     = 54U,  /*!< Chip-specific     */
    PORT_INPUT_MUX_ALT55     = 55U,  /*!< Chip-specific     */
#endif
    PORT_INPUT_MUX_NO_INIT           /*!< No initialization */
} port_input_mux_t;


/*!
 * @brief Configures the Safe Mode Control.
 * Implements : port_safe_mode_t_Class
 */
typedef enum
{
    PORT_SAFE_MODE_DISABLED     = 0U, /*!< To drive pad in hi-z state using OBE = 0, when FCCU in fault state.
                                       *   The OBE will be driven by IP/SIUL when FCCU leaves the fault state. */
    PORT_SAFE_MODE_ENABLED      = 1U  /*!< No effect on IP/SIUL driven OBE value */
} port_safe_mode_t;

/*!
 * @brief Configures the slew rate control.
 * Implements : port_slew_rate_control_t_Class
 */
typedef enum
{
#ifdef FEATURE_SIUL2_HAS_SLEW_RATE_WITH_DRIVE_STRENGTH
    HALF_STRENGTH_WITH_SLEWRATE_CONTROL     = 0u, /*!< Half Strength with Slewrate Control     */
    FULL_STRENGTH_WITH_SLEWRATE_CONTROL     = 1u, /*!< Full Strength with Slewrate Control     */
    HALF_STRENGTH_WITHOUT_SLEWRATE_CONTROL  = 2u, /*!< Half Strength without Slewrate Control  */
    FULL_STRENGTH_WITHOUT_SLEWRATE_CONTROL  = 3u  /*!< Full Strength without Slewrate Control  */
#else
    PORT_LOW_SLEW_RATE                      = 0U, /*!< Low    frequency slew rate (50 Mhz)     */
    PORT_MEDIUM_SLEW_RATE                   = 1U, /*!< Medium frequency slew rate (100Mhz)     */
    PORT_MEDIUM_SLEW_RATE2                  = 2U, /*!< Medium frequency slew rate (100Mhz)     */
    PORT_HIGH_SLEW_RATE                     = 3U  /*!< High   frequency slew rate (200Mhz)     */
#endif
} port_slew_rate_control_t;

#ifdef FEATURE_PINS_HAS_DRIVE_STRENGTH_CONTROL
/*!
 * @brief Configures the drive strength.
 * Implements : port_drive_strength_t_Class
 */
typedef enum
{
    PORT_STRENGTH_DISABLED      = 0U, /*!< Output driver disabled                   */
    PORT_STR1_DRIVE_STRENGTH    = 1U, /*!< Resistor is set to 240 Ohm               */
    PORT_STR2_DRIVE_STRENGTH    = 2U, /*!< Resistor is set to 240 / 2 Ohm = 120 Ohm */
    PORT_STR3_DRIVE_STRENGTH    = 3U, /*!< Resistor is set to 240 / 3 Ohm = 80 Ohm  */
    PORT_STR4_DRIVE_STRENGTH    = 4U, /*!< Resistor is set to 240 / 4 Ohm = 60 Ohm  */
    PORT_STR5_DRIVE_STRENGTH    = 5U, /*!< Resistor is set to 240 / 5 Ohm = 48 Ohm  */
    PORT_STR6_DRIVE_STRENGTH    = 6U, /*!< Resistor is set to 240 / 6 Ohm = 40 Ohm  */
    PORT_STR7_DRIVE_STRENGTH    = 7U  /*!< Resistor is set to 240 / 7 Ohm = 34 Ohm  */
} port_drive_strength_t;
#endif /* FEATURE_PINS_HAS_DRIVE_STRENGTH_CONTROL */

#ifdef FEATURE_SIUL2_HAS_SPECIFIC_PULL_REGISTER
/*!
 * @brief The internal specific pull resistor for GPI pins
 * Implements : port_specific_pull_config_t_Class
 */
typedef enum
{
    PORT_INTERNAL_PULL_DISABLED       = 0x0U,  /*!< internal pull up-down both disabled.         */
    PORT_INTERNAL_200K_PULL_UP        = 0x5U,  /*!< internal 100 kOhm pull up enabled.           */
    PORT_INTERNAL_100K_PULL_UP        = 0x9U,  /*!< internal 50  kOhm pull up enabled.           */
    PORT_INTERNAL_5K_PULL_UP          = 0xDU,  /*!< internal 100 kOhm pull up enabled.           */
    PORT_INTERNAL_200K_PULL_DOWN      = 0x6U,  /*!< internal 100 kOhm pull down enabled.         */
    PORT_INTERNAL_100K_PULL_DOWN      = 0xAU,  /*!< internal 50  kOhm pull down enabled.         */
    PORT_INTERNAL_5K_PULL_DOWN        = 0xEU,  /*!< internal 100 kOhm pull down enabled.         */
    PORT_INTERNAL_200K_PULL_UP_DOWN   = 0x7U,  /*!< internal 100 kOhm pull up-down both enabled. */
    PORT_INTERNAL_100K_PULL_UP_DOWN   = 0xBU,  /*!< internal 50  kOhm pull up-down both enabled. */
    PORT_INTERNAL_5K_PULL_UP_DOWN     = 0xFU   /*!< internal 100 kOhm pull up-down both enabled. */
} port_specific_pull_config_t;
#endif

#ifdef FEATURE_SIUL2_HAS_INPUT_LEVEL_SELECTION
/*!
 * @brief Configures the Pin input level selection
 * Implements : port_input_level_select_t_Class
 */
typedef enum
{
    PORT_INPUT_LEVEL_SELECT_RESERVED  = 0x0U,  /*!< Reserved                     */
    PORT_INPUT_LEVEL_SELECT_TTL       = 0x1U,  /*!< Input level select TTL  mode */
    PORT_INPUT_LEVEL_SELECT_LVDS      = 0x2U,  /*!< Input level select LVDS mode */
    PORT_INPUT_LEVEL_SELECT_CMOS      = 0x3U   /*!< Input level select CMOS mode */
} port_input_level_select_t;
#endif

/*!
 * @brief Defines the converter configuration
 *
 * This structure is used to configure the pins
 * Implements : pin_settings_config_t_Class
 */
typedef struct
{
    SIUL2_Type        *         base;              /*!< SIUL2 base pointer.                       */
    uint32_t                    pinPortIdx;        /*!< Port pin number.                          */
    port_pull_config_t          pullConfig;        /*!< Internal resistor pull feature selection. */
#ifdef FEATURE_PINS_HAS_DRIVE_STRENGTH_CONTROL
    port_drive_strength_t       driveSelect;       /*!< Configures the drive strength.            */
#endif
    port_mux_t                  mux;               /*!< Pin output muxing selection.              */
    GPIO_Type         *         gpioBase;          /*!< GPIO base pointer.                        */
#ifdef FEATURE_SIUL2_HAS_ANALOG_PAD
    port_analog_pad_t           analogPadCtrlSel;  /*!< Configures the Analog Pad Control         */
#endif
    port_safe_mode_t            safeMode;          /*!< Configures the Safe Mode Control.         */
    port_slew_rate_control_t    slewRateCtrlSel;   /*!< Configures the Slew Rate Control field.   */
    port_hysteresis_t           hysteresisSelect;  /*!< Configures the Hysteresis Enable.         */
#ifdef FEATURE_SIUL2_HAS_DDR_PAD
    pin_ddr_config_t            ddrConfiguration;  /*!< Structure that configures the DDR         */
#endif
#ifdef FEATURE_SIUL2_HAS_OUTPUT_DRIVE_CONTROL
    port_output_drive_control_t outputDriveCtrl;   /*!< Configures the Output Drive control.      */
#else
    port_open_drain_t           openDrain;         /*!< Configures open drain.                    */
    port_output_buffer_t        outputBuffer;      /*!< Configures the Output Buffer Enable.      */
#endif
#ifdef FEATURE_SIUL2_HAS_INVERT_DATA_OUTPUT
    port_invert_output_t        invertOutput;      /*!< Configures the Invert Data Output.        */
#endif
#ifdef FEATURE_SIUL2_HAS_INPUT_LEVEL_SELECTION
    port_input_level_select_t   inputLevel;        /*!< Configures the Input Level Selection.     */
#endif
    port_input_buffer_t         inputBuffer;       /*!< Configures the Input Buffer Enable.       */
    port_input_mux_t            inputMux[FEATURE_SIUL2_INPUT_MUX_WIDTH];
                                                   /*!< Configures the input muxing selection     */
#ifdef FEATURE_SIUL2_HAS_INVERT_DATA_INPUT
    port_invert_input_t         inputInvert[FEATURE_SIUL2_INPUT_MUX_WIDTH];
                                                   /*!< Configures the Invert Data Input.         */
#endif
    uint32_t                    inputMuxReg[FEATURE_SIUL2_INPUT_MUX_WIDTH];
                                                   /*!< Configures the input muxing register      */
    siul2_interrupt_config_t    intConfig;         /*!< Interrupt generation condition.           */
    port_data_direction_t       direction;         /*!< Configures the port data direction.       */
    pins_level_type_t           initValue;         /*!< Initial value                             */
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
                         uint16_t pin,
                         port_pull_config_t pullConfig);

#ifdef FEATURE_SIUL2_HAS_SPECIFIC_PULL_REGISTER
/*!
 * @brief Configures the internal specific pull resistor for GPI pins.
 *
 * This function configures the internal specific pull resistor for GPI pins.
 * Supports only for some GPI pin (general purpose input)
 * with specific pull resistors
 *
 * @param[in] pin Port pin number is supported (PTY0, PTY1, PTY2, etc.)
 * @param[in] pullConfig The pull configuration
 */
void PINS_DRV_SetSpecificPullSel(uint32_t pin,
                                 port_specific_pull_config_t pullConfig);
#endif

#ifdef FEATURE_SIUL2_HAS_OUTPUT_DRIVE_CONTROL
/*!
 * @brief Configures the output drive control and output signal
 *
 * This function configures the output drive control for the pin
 * and the path for output signal from module to pin
 *
 * @param[in] base Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin Port pin number
 * @param[in] driveCtrl the Output Drive control
 * @param[in] mux Pin muxing slot selection
 */
void PINS_DRV_SetOutputDriveControl(PORT_Type * const base,
                                    uint16_t pin,
                                    port_output_drive_control_t driveCtrl,
                                    port_mux_t mux);
#else
/*!
 * @brief Configures the output buffer and output signal.
 *
 * This function configures the output buffer for the pin
 * and the path for output signal from module to pin
 *
 * @param[in] base Port base pointer (PORTA, PORTB, PORTC, etc.)
 * @param[in] pin Port pin number
 * @param[in] enable Enable output buffer
 * @param[in] mux Pin muxing slot selection
 */
void PINS_DRV_SetOutputBuffer(PORT_Type * const base,
                              uint16_t pin,
                              bool enable,
                              port_mux_t mux);
#endif

/*!
 * @brief Configures the input buffer and input signal.
 *
 * This function configures the input buffer for the pin
 * and the path for input signal from pin to module
 *
 * @param[in] base Port base pointer (PORTA, PORTB, PORTC, etc.), NULL if inputMux
 *            does not connect to external pin or disable inputMux only
 * @param[in] pin Port pin number
 * @param[in] enable Enable input buffer
 * @param[in] inputMuxReg Pin muxing register slot selection
 * @param[in] inputMux Pin muxing slot selection
 */
void PINS_DRV_SetInputBuffer(PORT_Type * const base,
                             uint16_t pin,
                             bool enable,
                             uint32_t inputMuxReg,
                             port_input_mux_t inputMux);

/*!
 * @brief Configures the interrupt filter clock prescaler.
 *
 * This function configures the interrupt filter clock prescaler.
 *
 * @param[in] prescaler The clock prescaler value
 */
void PINS_DRV_ConfigIntFilterClock(uint8_t prescaler);

/*!
 * @brief Configures the external interrupt.
 *
 * This function configures the external interrupt.
 *
 * @param[in] intConfig The configuration for interrupt pin
 */
void PINS_DRV_SetExInt(siul2_interrupt_config_t intConfig);

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
 * @brief Reads the entire external interrupt status flag
 *
 * This function reads the entire external interrupt status flag.
 *
 * @return All 32 IRQ pins
 */
uint32_t PINS_DRV_GetExIntFlag(void);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* PINS_SIUL2_DRIVER_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
