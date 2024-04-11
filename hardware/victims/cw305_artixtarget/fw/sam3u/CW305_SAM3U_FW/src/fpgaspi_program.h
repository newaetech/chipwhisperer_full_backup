/*
 * \brief SPI Stuff
 *
 * Copyright (c) 2020 NewAE Technology Inc.  All rights reserved.
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

#ifndef _FPGASPI_PROGRAM_H_
#define _FPGASPI_PROGRAM_H_


#define PIN_FPGASPI_CCLK_FLAGS	(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define FPGASPI_CCLK_LOW()		gpio_set_pin_low(PIN_FPGA_CCLK_GPIO)
#define FPGASPI_CCLK_HIGH()		gpio_set_pin_high(PIN_FPGA_CCLK_GPIO)
#define FPGASPI_CCLK_SETUP()	gpio_configure_pin(PIN_FPGA_CCLK_GPIO, PIN_FPGASPI_CCLK_FLAGS)
#define FPGASPI_CCLK_RELEASE()	gpio_configure_pin(PIN_FPGA_CCLK_GPIO, PIO_DEFAULT)

#define PIN_FPGASPI_DO_FLAGS		(PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define FPGASPI_DO_LOW()			gpio_set_pin_low(PIN_SPARE1)
#define FPGASPI_DO_HIGH()			gpio_set_pin_high(PIN_SPARE1)
#define FPGASPI_DO_SETUP()			gpio_configure_pin(PIN_SPARE1, PIN_FPGASPI_DO_FLAGS)
#define FPGASPI_DO_RELEASE()		gpio_configure_pin(PIN_SPARE1, PIO_DEFAULT)

#define PIN_FPGASPI_DI_FLAGS		(PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define FPGASPI_DI_SETUP()			gpio_configure_pin(PIN_FPGA_DO_GPIO, PIN_FPGASPI_DI_FLAGS)
#define FPGASPI_DI_STATUS()         pio_get_pin_value(PIN_FPGA_DO_GPIO)

#define PIN_FPGASPI_CS_FLAGS		(PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define FPGASPI_CS_LOW()			gpio_set_pin_low(PIN_SPARE2)
#define FPGASPI_CS_HIGH()			gpio_set_pin_high(PIN_SPARE2)
#define FPGASPI_CS_SETUP()			gpio_configure_pin(PIN_SPARE2, PIN_FPGASPI_CS_FLAGS)
#define FPGASPI_CS_RELEASE()		gpio_configure_pin(PIN_SPARE2, PIO_DEFAULT)

/**
 * \brief Send a byte to FPGA using CCLK/DO, FPGA must be in programming mode
 *
 * \param databyte Byte to send, LSB is shifted out first
 */
uint8_t fpgaspi_xferbyte(uint8_t databyte);

/**
 * \brief Setup the pins
 */
void fpgaspi_program_init(void);

/**
 * \brief Unsetup the pins
 */
void fpgaspi_program_deinit(void);

void fpgaspi_cs_low(void);

void fpgaspi_cs_high(void);

#endif