/**
 * \file
 *
 * \brief SAM3U-EK Board Definition.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAM3U_EK_H_
#define _SAM3U_EK_H_

#include "compiler.h"
#include "system_sam3u.h"
#include "exceptions.h"

/*----------------------------------------------------------------------------*/
/**
 *  \page sam3u_ek_opfreq SAM3U-EK - Operating frequencies
 *  This page lists several definition related to the board operating frequency
 *
 *  \section Definitions
 *  - \ref BOARD_FREQ_*
 *  - \ref BOARD_MCK
 */

/** Board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL        (0U)
#define BOARD_FREQ_SLCK_BYPASS      (0U)
#define BOARD_FREQ_MAINCK_XTAL      (12000000U)
#define BOARD_FREQ_MAINCK_BYPASS    (12000000U)

/** Master clock frequency */
#define BOARD_MCK                   CHIP_FREQ_CPU_MAX

/** board main clock xtal startup time */
#define BOARD_OSC_STARTUP_US   15625

/*----------------------------------------------------------------------------*/
/**
 * \page sam3u_ek_board_info SAM3U-EK - Board informations
 * This page lists several definition related to the board description.
 *
 * \section Definitions
 * - \ref BOARD_NAME
 * - \ref sam3uek
 * - \ref sam3u
 * - \ref cortexm3
 */

/** Name of the board */
#define BOARD_NAME "ChipWhisperer-Husky"
/** Board definition */
#define cw1190
/** Family definition (already defined) */
#define sam3u
/** Core definition */
#define cortexm3

/*----------------------------------------------------------------------------*/
//! Pins ADC
#define PINS_ADC12B_TRIG  PIO_PA2_IDX
#define PINS_ADC12B_TRIG_FLAG  (PIO_PERIPH_B | PIO_DEFAULT)
#define PINS_ADC_TRIG  PIO_PA17_IDX
#define PINS_ADC_TRIG_FLAG  (PIO_PERIPH_B | PIO_DEFAULT)

//! UART pins (UTXD0 and URXD0) definitions, PA9,10.
#define PINS_UART       (PIO_PA11A_URXD | PIO_PA12A_UTXD)
#define PINS_UART_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)

#define PINS_UART_MASK PIO_PA11A_URXD|PIO_PA12A_UTXD
#define PINS_UART_PIO PIOA
#define PINS_UART_ID ID_PIOA
#define PINS_UART_TYPE PIO_PERIPH_A
#define PINS_UART_ATTR PIO_DEFAULT

/** EBI Data Bus pins */
#define PIN_EBI_DATA_BUS_D0        PIO_PB9_IDX
#define PIN_EBI_DATA_BUS_D1        PIO_PB10_IDX
#define PIN_EBI_DATA_BUS_D2        PIO_PB11_IDX
#define PIN_EBI_DATA_BUS_D3        PIO_PB12_IDX
#define PIN_EBI_DATA_BUS_D4        PIO_PB13_IDX
#define PIN_EBI_DATA_BUS_D5        PIO_PB14_IDX
#define PIN_EBI_DATA_BUS_D6        PIO_PB15_IDX
#define PIN_EBI_DATA_BUS_D7        PIO_PB16_IDX

#define PIN_EBI_USB_SPARE0          PIO_PB6_IDX
#define PIN_EBI_USB_SPARE1          PIO_PB17_IDX

#define PIN_EBI_USB_SPARE0_FLAGS    (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define PIN_EBI_USB_SPARE1_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)

#define PIN_EBI_DATA_BUS_FLAG1   PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_DATA_BUS_FLAG2   PIO_PERIPH_B | PIO_PULLUP

/* NBS0 = A0 = LB# */
/* NBS1      = UB# */
#define PIN_EBI_ADDR_BUS_NBS0 PIO_PB7_IDX
#define PIN_EBI_ADDR_BUS_NBS1 PIO_PC15_IDX

#define PIN_EBI_ADDR_BUS_FLAG1  PIO_PERIPH_B | PIO_PULLUP
#define PIN_EBI_ADDR_BUS_FLAG2  PIO_PERIPH_A | PIO_PULLUP

#define PIN_EBI_ADDR_BUS_A0        PIO_PB7_IDX
#define PIN_EBI_ADDR_BUS_A1        PIO_PB8_IDX
#define PIN_EBI_ADDR_BUS_A2        PIO_PB0_IDX
#define PIN_EBI_ADDR_BUS_A3        PIO_PB1_IDX
#define PIN_EBI_ADDR_BUS_A4        PIO_PB2_IDX
#define PIN_EBI_ADDR_BUS_A5        PIO_PB3_IDX
#define PIN_EBI_ADDR_BUS_A6        PIO_PB4_IDX
#define PIN_EBI_ADDR_BUS_A7        PIO_PB5_IDX

/** EBI NRD pin */
#define PIN_EBI_NRD                 PIO_PB19_IDX
#define PIN_EBI_NRD_FLAGS       PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_NRD_MASK  1 << 19
#define PIN_EBI_NRD_PIO  PIOB
#define PIN_EBI_NRD_ID  ID_PIOB
#define PIN_EBI_NRD_TYPE PIO_PERIPH_A
#define PIN_EBI_NRD_ATTR PIO_PULLUP
/** EBI NWE pin */
#define PIN_EBI_NWE                  PIO_PB23_IDX
#define PIN_EBI_NWE_FLAGS       PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_NWE_MASK  1 << 23
#define PIN_EBI_NWE_PIO  PIOB
#define PIN_EBI_NWE_ID  ID_PIOB
#define PIN_EBI_NWE_TYPE PIO_PERIPH_A
#define PIN_EBI_NWE_ATTR PIO_PULLUP
/** EBI NCS0 pin */
#define PIN_EBI_NCS0                PIO_PB20_IDX
#define PIN_EBI_NCS0_FLAGS     PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_NCS0_MASK  1 << 20
#define PIN_EBI_NCS0_PIO  PIOB
#define PIN_EBI_NCS0_ID  ID_PIOB
#define PIN_EBI_NCS0_TYPE PIO_PERIPH_A
#define PIN_EBI_NCS0_ATTR PIO_PULLUP

//! LED #0 pin definition
#define LED_0_NAME    "green LEDHB"
#define LED0_GPIO     (PIO_PA29_IDX)
#define LED0_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED0_ACTIVE_LEVEL 1

//! LED #1 pin definition
#define LED_1_NAME    "red Error"
#define LED1_GPIO     (PIO_PA31_IDX)
#define LED1_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED1_ACTIVE_LEVEL 1

#define BOARD_NUM_OF_LED 2

//! General Pins
#define PIN_PWRON_GPIO	  PIO_PB18_IDX
#define PIN_PWRON_FLAGS  (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT) 
#define board_pwron()     gpio_set_pin_high(PIN_PWRON_GPIO)
#define board_pwroff()    gpio_set_pin_low(PIN_PWRON_GPIO)

	
/** Base address of PSRAM */
#define BOARD_PSRAM_ADDR         (0x60000000)

#define CONSOLE_UART               UART
#define CONSOLE_UART_ID            ID_UART

/** Address for transferring data bytes to the nandflash. */
#define BOARD_NF_DATA_ADDR      0x61000000

#endif  // _SAM3U_EK_H_
