/*
 * \brief Xilinx Spartan 6 FPGA Programming Routines Header File
 *
 * Copyright (c) 2014-2016 NewAE Technology Inc.  All rights reserved.
 *   Author: Colin O'Flynn, <coflynn@newae.com>
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
 * 3. The name of NewAE Technology Inc may not be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY NEWAE TECHNOLOGY INC "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL NEWAE TECHNOLOGY INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FPGA_PROGRAM_H_
#define _FPGA_PROGRAM_H_

/* Example of definitions you require:

// FPGA Programming Pins
#define PIN_FPGA_PROGRAM_GPIO	PIO_PA8_IDX
#define PIN_FPGA_INITB_GPIO		PIO_PA9_IDX
#define PIN_FPGA_DONE_GPIO		PIO_PA10_IDX

// If using bit-bang mode you need these definitions:
#define PIN_FPGA_CCLK_GPIO		PIO_PA15_IDX
#define PIN_FPGA_DO_GPIO		PIO_PA14_IDX

// If using SPI mode you need these definitions:
#define SPI_MOSI_GPIO       (PIO_PA14_IDX)
#define SPI_MOSI_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)
#define SPI_SPCK_GPIO       (PIO_PA15_IDX)
#define SPI_SPCK_FLAGS (PIO_PERIPH_A | PIO_DEFAULT)

*/


//! FPGA Programming Pins
#define PIN_FPGA_PROGRAM_FLAGS	(PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define FPGA_NPROG_LOW()		gpio_set_pin_low(PIN_FPGA_PROGRAM_GPIO)
#define FPGA_NPROG_HIGH()		gpio_set_pin_high(PIN_FPGA_PROGRAM_GPIO)
#define FPGA_NPROG_SETUP()		gpio_configure_pin(PIN_FPGA_PROGRAM_GPIO, PIN_FPGA_PROGRAM_FLAGS)

//#define PIN_FPGA_INITB_FLAGS	(PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT | PIO_OPENDRAIN)
//#define FPGA_INITB_LOW()		gpio_set_pin_low(PIN_FPGA_INITB_GPIO)
//#define FPGA_INITB_HIGH()		gpio_set_pin_high(PIN_FPGA_INITB_GPIO)
#define PIN_FPGA_INITB_FLAGS	(PIO_TYPE_PIO_INPUT)
#define FPGA_INITB_SETUP()		gpio_configure_pin(PIN_FPGA_INITB_GPIO, PIN_FPGA_INITB_FLAGS)
#define FPGA_INITB_STATUS()		gpio_pin_is_high(PIN_FPGA_INITB_GPIO)

#define PIN_FPGA_DONE_FLAGS     (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define	FPGA_ISDONE()			gpio_pin_is_high(PIN_FPGA_DONE_GPIO)

#define PIN_FPGA_CCLK_FLAGS		(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define FPGA_CCLK_LOW()			gpio_set_pin_low(PIN_FPGA_CCLK_GPIO)
#define FPGA_CCLK_HIGH()		gpio_set_pin_high(PIN_FPGA_CCLK_GPIO)
#define FPGA_CCLK_SETUP()		gpio_configure_pin(PIN_FPGA_CCLK_GPIO, PIN_FPGA_CCLK_FLAGS)

#define PIN_FPGA_DO_FLAGS		(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define FPGA_DO_LOW()			gpio_set_pin_low(PIN_FPGA_DO_GPIO)
#define FPGA_DO_HIGH()			gpio_set_pin_high(PIN_FPGA_DO_GPIO)
#define FPGA_DO_SETUP()			gpio_configure_pin(PIN_FPGA_DO_GPIO, PIN_FPGA_DO_FLAGS)

#define FPGA_USE_BITBANG 0

/**
 * \brief Send a byte to FPGA using CCLK/DO, FPGA must be in programming mode
 *
 * \param databyte Byte to send, LSB is shifted out first
 */
void fpga_program_sendbyte(uint8_t databyte);

/**
 * \brief Setup the 'NPROG' pin
 */
void fpga_program_init(void);

/**
 * \brief Setup peripherals, erase FPGA, must be followed by call to _setup2()
 */
void fpga_program_setup1(void);

/**
 * \brief Set NPROG to idle state in preperation for programming to commence
 */
void fpga_program_setup2(void);

#endif