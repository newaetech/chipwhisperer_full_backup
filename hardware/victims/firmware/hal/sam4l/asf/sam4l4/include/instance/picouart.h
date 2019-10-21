/**
 * \file
 *
 * \brief Instance description for PICOUART
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _SAM4L_PICOUART_INSTANCE_
#define _SAM4L_PICOUART_INSTANCE_

/* ========== Register definition for PICOUART peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PICOUART_CR            (0x400F1400U) /**< \brief (PICOUART) Control Register */
#define REG_PICOUART_CFG           (0x400F1404U) /**< \brief (PICOUART) Configuration Register */
#define REG_PICOUART_SR            (0x400F1408U) /**< \brief (PICOUART) Status Register */
#define REG_PICOUART_RHR           (0x400F140CU) /**< \brief (PICOUART) Receive Holding Register */
#define REG_PICOUART_VERSION       (0x400F1420U) /**< \brief (PICOUART) Version Register */
#else
#define REG_PICOUART_CR            (*(WoReg  *)0x400F1400U) /**< \brief (PICOUART) Control Register */
#define REG_PICOUART_CFG           (*(RwReg  *)0x400F1404U) /**< \brief (PICOUART) Configuration Register */
#define REG_PICOUART_SR            (*(RoReg  *)0x400F1408U) /**< \brief (PICOUART) Status Register */
#define REG_PICOUART_RHR           (*(RoReg  *)0x400F140CU) /**< \brief (PICOUART) Receive Holding Register */
#define REG_PICOUART_VERSION       (*(RoReg  *)0x400F1420U) /**< \brief (PICOUART) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAM4L_PICOUART_INSTANCE_ */
