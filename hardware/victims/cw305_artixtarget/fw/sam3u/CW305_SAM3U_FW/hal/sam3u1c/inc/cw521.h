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
#define BOARD_NAME "ChipWhisperer-CW521"
/** Board definition */
#define cw521
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

#define PIN_FPGA_PROGRAM_GPIO PIO_PA25_IDX
#define PIN_FPGA_INITB_GPIO PIO_PA26_IDX
#define PIN_FPGA_DONE_GPIO PIO_PA7_IDX
#define FPGA_PROG_USART USART0
#define PIN_FPGA_CCLK_GPIO PIO_PA17_IDX
#define PIN_FPGA_DO_GPIO PIO_PA18_IDX
#define PIN_FPGA_CCLK_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_FPGA_DO_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
#define FPGA_PROG_USART_ID ID_USART0

#define FPGA_ADDR_PINS (PIO_PB0 | PIO_PB1 | PIO_PB2 | PIO_PB3 | PIO_PB4 | PIO_PB5 | PIO_PB7 | PIO_PB8 )
#define FPGA_ADDR_PORT PIOB

#define CW_USE_USART0

#define PIN_USART0_RXD	         (PIO_PA19_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_USART0_TXD	        (PIO_PA18_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)

//! USART1 (XMEGA Programming)
#define PIN_USART1_RXD           (PIO_PA21_IDX)
#define PIN_USART1_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_USART1_TXD        (PIO_PA20_IDX)
#define PIN_USART1_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_USART1_SCK        (PIO_PA24_IDX)
#define PIN_USART1_SCK_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)

//! USART2 (Smartcard / Extra UART)
#define PIN_USART2_RXD        (PIO_PA23_IDX)
#define PIN_USART2_RXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_USART2_TXD        (PIO_PA22_IDX)
#define PIN_USART2_TXD_FLAGS      (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_USART2_SCK        (PIO_PA25_IDX)
#define PIN_USART2_SCK_FLAGS      (PIO_PERIPH_B | PIO_DEFAULT)


#define PIN_CDCI_SDA        (PIO_PA9_IDX)
#define PIN_CDCI_SDA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_CDCI_SCL        (PIO_PA10_IDX)
#define PIN_CDCI_SCL_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)

/** EBI Data Bus pins */
#define PIN_EBI_DATA_BUS_D0        PIO_PB9_IDX
#define PIN_EBI_DATA_BUS_D1        PIO_PB10_IDX
#define PIN_EBI_DATA_BUS_D2        PIO_PB11_IDX
#define PIN_EBI_DATA_BUS_D3        PIO_PB12_IDX
#define PIN_EBI_DATA_BUS_D4        PIO_PB13_IDX
#define PIN_EBI_DATA_BUS_D5        PIO_PB14_IDX
#define PIN_EBI_DATA_BUS_D6        PIO_PB15_IDX
#define PIN_EBI_DATA_BUS_D7        PIO_PB16_IDX
#define PIN_EBI_DATA_BUS_D8        PIO_PB25_IDX
#define PIN_EBI_DATA_BUS_D9        PIO_PB26_IDX
#define PIN_EBI_DATA_BUS_D10        PIO_PB27_IDX
#define PIN_EBI_DATA_BUS_D11        PIO_PB28_IDX
#define PIN_EBI_DATA_BUS_D12        PIO_PB29_IDX
#define PIN_EBI_DATA_BUS_D13        PIO_PB30_IDX
#define PIN_EBI_DATA_BUS_D14        PIO_PB31_IDX
#define PIN_EBI_DATA_BUS_D15        PIO_PB6_IDX

#define PIN_EBI_DATA_BUS_FLAG1   PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_DATA_BUS_FLAG2   PIO_PERIPH_B | PIO_PULLUP

#define USART_PDI                 USART1
#define USART_PDI_ID              ID_USART1

#define PIN_TARG_NRST_GPIO      PIO_PA17_IDX
/** SPI MISO pin definition */
#define SPI_MISO_GPIO       (PIO_PA13_IDX)
#define SPI_MISO_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO       (PIO_PA14_IDX)
#define SPI_MOSI_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO       (PIO_PA15_IDX)
#define SPI_SPCK_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)

#define SPI_CS_GPIO (PIO_PA16_IDX)
#define SPI_CS_FLAGS (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
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
// #define PIN_EBI_ADDR_BUS_A8        PIO_PC6_IDX
// #define PIN_EBI_ADDR_BUS_A9        PIO_PC7_IDX
// #define PIN_EBI_ADDR_BUS_A10       PIO_PC8_IDX
// #define PIN_EBI_ADDR_BUS_A11       PIO_PC9_IDX
// #define PIN_EBI_ADDR_BUS_A12       PIO_PC10_IDX
// #define PIN_EBI_ADDR_BUS_A13       PIO_PC11_IDX
// #define PIN_EBI_ADDR_BUS_A14       PIO_PC20_IDX
// #define PIN_EBI_ADDR_BUS_A15       PIO_PC21_IDX
// #define PIN_EBI_ADDR_BUS_A16       PIO_PC22_IDX
// #define PIN_EBI_ADDR_BUS_A17       PIO_PC23_IDX
// #define PIN_EBI_ADDR_BUS_A18       PIO_PC24_IDX
// #define PIN_EBI_ADDR_BUS_A19       PIO_PC25_IDX
// #define PIN_EBI_ADDR_BUS_A20       PIO_PC26_IDX
// #define PIN_EBI_ADDR_BUS_A21       PIO_PB21_IDX

#define PIN_EBI_USB_SPARE0          PIO_PB6_IDX
#define PIN_EBI_USB_SPARE0_FLAGS    (PIO_TYPE_PIO_INPUT)
#define PIN_EBI_USB_SPARE0_PORT     PIOB
#define PIN_EBI_USB_SPARE0_PIN      PIO_PB6

#define PIN_EBI_USB_SPARE1          PIO_PB17_IDX

#define PIN_EBI_USB_SPARE1_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
//! XMEGA PDI Programming Pins
#define PIN_PDIDTX_GPIO			  (PIO_PA20_IDX)
#define PIN_PDIDTX_OUT_FLAGS	  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define PIN_PDIDTX_IN_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIDTX_USART_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_PDIDRX_GPIO			  (PIO_PA21_IDX)
#define PIN_PDIDRX_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIDRX_USART_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_PDIC_GPIO			  (PIO_PA24_IDX)
#define PIN_PDIC_OUT_FLAGS		  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define PIN_PDIC_IN_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIC_USART_FLAGS	  (PIO_PERIPH_B | PIO_DEFAULT)

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
#define LED_1_NAME    "red TX"
#define LED1_GPIO     (PIO_PA31_IDX)
#define LED1_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED1_ACTIVE_LEVEL 1

// //! LED #1 pin definition
// #define LED_2_NAME    "red RX"
// #define LED2_GPIO     (PIO_PC12_IDX)
// #define LED2_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
// #define LED2_ACTIVE_LEVEL 1

#define BOARD_NUM_OF_LED 2

//! General Pins
#define PIN_PWRON_GPIO	  PIO_PA29_IDX
#define PIN_PWRON_FLAGS  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT) 
#define board_power(a)	 if (a){gpio_set_pin_high(PIN_PWRON_GPIO);} else {gpio_set_pin_low(PIN_PWRON_GPIO);}
#define board_sram_pwron() gpio_set_pin_low(PIN_PWRON_GPIO)
#define board_sram_pwroff() gpio_set_pin_high(PIN_PWRON_GPIO)

	
/** Base address of PSRAM */
#define BOARD_PSRAM_ADDR         (0x60000000)

#define CONSOLE_UART               UART
#define CONSOLE_UART_ID            ID_UART

/** Address for transferring data bytes to the nandflash. */
#define BOARD_NF_DATA_ADDR      0x61000000

#endif  // _SAM3U_EK_H_
