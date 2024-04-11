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

#ifndef _FPGAUTILIO_H_
#define _FPGAUTILIO_H_

#include <stdint.h>
#include "fpgautil_io.h"

extern int pin_spi1_mosi;
extern int pin_spi1_miso;
extern int pin_spi1_sck;
extern int pin_spi1_cs;

/**
 * \brief Send a byte to FPGA
 *
 * \param databyte Byte to send, LSB is shifted out first
 */
uint8_t spi1util_xferbyte(uint8_t databyte);

/**
 * \brief Setup the pins
 */
void spi1util_init(void);

/**
 * \brief Unsetup the pins
 */
void spi1util_deinit(void);

void spi1util_cs_low(void);

void spi1util_cs_high(void);

#endif