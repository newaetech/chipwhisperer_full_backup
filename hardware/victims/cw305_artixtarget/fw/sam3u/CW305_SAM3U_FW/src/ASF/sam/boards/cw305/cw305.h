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
#define BOARD_NAME "ChipWhisperer-CW305"
/** Board definition */
#define cw305
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
#define PIN_EBI_DATA_BUS_FLAG1   PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_DATA_BUS_FLAG2  PIO_PERIPH_B | PIO_PULLUP

#define PIN_EBI_ADDR_BUS_A0        PIO_PB7_IDX
#define PIN_EBI_ADDR_BUS_A1        PIO_PB8_IDX
#define PIN_EBI_ADDR_BUS_A2        PIO_PC0_IDX
#define PIN_EBI_ADDR_BUS_A3        PIO_PC1_IDX
#define PIN_EBI_ADDR_BUS_A4        PIO_PC2_IDX
#define PIN_EBI_ADDR_BUS_A5        PIO_PC3_IDX
#define PIN_EBI_ADDR_BUS_A6        PIO_PC4_IDX
#define PIN_EBI_ADDR_BUS_A7        PIO_PC5_IDX
#define PIN_EBI_ADDR_BUS_A8        PIO_PC6_IDX
#define PIN_EBI_ADDR_BUS_A9        PIO_PC7_IDX
#define PIN_EBI_ADDR_BUS_A10       PIO_PC8_IDX
#define PIN_EBI_ADDR_BUS_A11       PIO_PC9_IDX
#define PIN_EBI_ADDR_BUS_A12       PIO_PC10_IDX
#define PIN_EBI_ADDR_BUS_A13       PIO_PC11_IDX
#define PIN_EBI_ADDR_BUS_A14       PIO_PC20_IDX
#define PIN_EBI_ADDR_BUS_A15       PIO_PC21_IDX
#define PIN_EBI_ADDR_BUS_A16       PIO_PC22_IDX
#define PIN_EBI_ADDR_BUS_A17       PIO_PC23_IDX
#define PIN_EBI_ADDR_BUS_A18       PIO_PC24_IDX
#define PIN_EBI_ADDR_BUS_A19       PIO_PC25_IDX
#define PIN_EBI_ADDR_BUS_A20       PIO_PC26_IDX


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

#define FPGA_ADDR_PINS (PIO_PB0 | PIO_PB1 | PIO_PB2 | PIO_PB3 | PIO_PB4 | PIO_PB5 | PIO_PB6 | PIO_PB7 )
#define FPGA_ADDR_PORT PIOB

#define FPGA_ALE_GPIO		 (PIO_PB21_IDX)
#define FPGA_ALE_FLAGS		 (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)

#define FPGA_TRIGGER_GPIO	(PIO_PB17_IDX)
#define FPGA_TRIGGER_FLAGS	(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)

//! LED #0 pin definition
#define LED_0_NAME    "red LED1"
#define LED0_GPIO     (PIO_PC15_IDX)
#define LED0_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED0_ACTIVE_LEVEL 1

//! LED #1 pin definition
#define LED_1_NAME    "red LED2"
#define LED1_GPIO     (PIO_PC16_IDX)
#define LED1_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED1_ACTIVE_LEVEL 1

//! LED #1 pin definition
#define LED_2_NAME    "red LED3"
#define LED2_GPIO     (PIO_PC17_IDX)
#define LED2_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define LED2_ACTIVE_LEVEL 1

#define BOARD_NUM_OF_LED 3

//! General Pins
#define PIN_PWRON_GPIO	  PIO_PB31_IDX
#define PIN_PWRON_FLAGS  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT) 
#define board_power(a)	 if (a){gpio_set_pin_high(PIN_PWRON_GPIO);} else {gpio_set_pin_low(PIN_PWRON_GPIO);}
	
#define PIN_SWSTATE_GPIO  PIO_PB30_IDX
#define PIN_SWSTATE_FLAGS  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define board_get_powerstate()  gpio_pin_is_high(PIO_PB30_IDX)

//! FPGA Programming Pins
#define PIN_FPGA_PROGRAM_GPIO	PIO_PA8_IDX
#define PIN_FPGA_DONE_GPIO		PIO_PA7_IDX
#define PIN_FPGA_CCLK_GPIO		PIO_PA15_IDX
#define PIN_FPGA_DO_GPIO		PIO_PA14_IDX
#define PIN_FPGA_INITB_GPIO     PIO_PC29_IDX

/* SPARE0, 1, 2 - SPARE 0/1 used to program SPI flash chip on-board*/
#define PIN_SPARE0                 PIO_PB17_IDX
#define PIN_SPARE1                 PIO_PB18_IDX
#define PIN_SPARE2                 PIO_PB22_IDX

//#define PIN_TARG_NRST_GPIO      PIO_PA17_IDX

/** SPI MISO pin definition */
#define SPI_MISO_GPIO       (PIO_PA13_IDX)
#define SPI_MISO_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO       (PIO_PA14_IDX)
#define SPI_MOSI_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO       (PIO_PA15_IDX)
#define SPI_SPCK_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_PWD_SDA        (PIO_PA9_IDX)
#define PIN_PWD_SDA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_PWD_SCL        (PIO_PA10_IDX)
#define PIN_PWD_SCL_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_CDCE_SDA        (PIO_PA24_IDX)
#define PIN_CDCE_SDA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_CDCE_SCL        (PIO_PA25_IDX)
#define PIN_CDCE_SCL_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)

//! PCK0
#define PIN_PCK0        (PIO_PA27_IDX)
#define PIN_PCK0_MUX    (IOPORT_MODE_MUX_B)
#define PIN_PCK0_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_PCK_0_MASK PIO_PA27
#define PIN_PCK_0_PIO PIOA
#define PIN_PCK_0_ID ID_PIOA
#define PIN_PCK_0_TYPE PIO_PERIPH_B
#define PIN_PCK_0_ATTR PIO_DEFAULT

//! PCK1
#define PIN_PCK1        (PIO_PB24_IDX)
#define PIN_PCK1_MUX    (IOPORT_MODE_MUX_B)
#define PIN_PCK1_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_PCK_1_MASK PIO_PB24
#define PIN_PCK_1_PIO PIOB
#define PIN_PCK_1_ID ID_PIOB
#define PIN_PCK_1_TYPE PIO_PERIPH_B
#define PIN_PCK_1_ATTR PIO_DEFAULT

//! XMEGA PDI Programming Pins
#define PIN_PDIDTX_GPIO			  (PIO_PA18_IDX)
#define PIN_PDIDTX_OUT_FLAGS	  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define PIN_PDIDTX_IN_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIDTX_USART_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_PDIDRX_GPIO			  (PIO_PA19_IDX)
#define PIN_PDIDRX_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIDRX_USART_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)

#define PIN_PDIC_GPIO			  (PIO_PA17_IDX)
#define PIN_PDIC_OUT_FLAGS		  (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define PIN_PDIC_IN_FLAGS		  (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define PIN_PDIC_USART_FLAGS	  (PIO_PERIPH_A | PIO_DEFAULT)

#define USART_PDI                 USART0
#define USART_PDI_ID              ID_USART0

/** TWI0 pins definition */
#define TWI0_DATA_GPIO   PIO_PA9_IDX
#define TWI0_DATA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI0_CLK_GPIO    PIO_PA10_IDX
#define TWI0_CLK_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)

/** TWI1 pins definition */
/*
#define TWI1_DATA_GPIO   PIO_PA24_IDX
#define TWI1_DATA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI1_CLK_GPIO    PIO_PA25_IDX
#define TWI1_CLK_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)
*/

//! USART0 (Target Communication)
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

//! USB VBus monitoring pin definition.
//The following are NOT used, as you may want to power board without USB attached.
//If you enable the VBUS Monitor it will turn off everything when USB is not connected.
#ifdef USE_VBUS_MONITOR
#define PIN_USB_VBUS    {1 << 30, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define USB_VBUS_FLAGS    (PIO_INPUT | PIO_DEBOUNCE | PIO_IT_EDGE)
#define USB_VBUS_PIN_IRQn (PIOA_IRQn)
#define USB_VBUS_PIN      (PIO_PA30_IDX)
#define USB_VBUS_PIO_ID   (ID_PIOA)
#define USB_VBUS_PIO_MASK (PIO_PA30)
#endif
/* This pin can not be used as fast wakeup source such as
 * USB_VBUS_WKUP PMC_FSMR_FSTT7 */

/** Base address of PSRAM */
#define BOARD_PSRAM_ADDR         (0x60000000)

#define CONSOLE_UART               UART
#define CONSOLE_UART_ID            ID_UART

/** Address for transferring data bytes to the nandflash. */
#define BOARD_NF_DATA_ADDR      0x61000000

#endif  // _SAM3U_EK_H_
