/**
 * \file
 *
 * \brief Instance description for PARC
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

#ifndef _SAM4L_PARC_INSTANCE_
#define _SAM4L_PARC_INSTANCE_

/* ========== Register definition for PARC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PARC_CFG               (0x4006C000U) /**< \brief (PARC) Configuration Register */
#define REG_PARC_CR                (0x4006C004U) /**< \brief (PARC) Control Register */
#define REG_PARC_IER               (0x4006C008U) /**< \brief (PARC) Interrupt Enable Register */
#define REG_PARC_IDR               (0x4006C00CU) /**< \brief (PARC) Interrupt Disable Register */
#define REG_PARC_IMR               (0x4006C010U) /**< \brief (PARC) Interrupt Mask Register */
#define REG_PARC_SR                (0x4006C014U) /**< \brief (PARC) Status Register */
#define REG_PARC_ICR               (0x4006C018U) /**< \brief (PARC) Interrupt Status Clear Register */
#define REG_PARC_RHR               (0x4006C01CU) /**< \brief (PARC) Receive Holding Register */
#define REG_PARC_VERSION           (0x4006C020U) /**< \brief (PARC) Version Register */
#else
#define REG_PARC_CFG               (*(RwReg  *)0x4006C000U) /**< \brief (PARC) Configuration Register */
#define REG_PARC_CR                (*(RwReg  *)0x4006C004U) /**< \brief (PARC) Control Register */
#define REG_PARC_IER               (*(WoReg  *)0x4006C008U) /**< \brief (PARC) Interrupt Enable Register */
#define REG_PARC_IDR               (*(WoReg  *)0x4006C00CU) /**< \brief (PARC) Interrupt Disable Register */
#define REG_PARC_IMR               (*(RoReg  *)0x4006C010U) /**< \brief (PARC) Interrupt Mask Register */
#define REG_PARC_SR                (*(RoReg  *)0x4006C014U) /**< \brief (PARC) Status Register */
#define REG_PARC_ICR               (*(WoReg  *)0x4006C018U) /**< \brief (PARC) Interrupt Status Clear Register */
#define REG_PARC_RHR               (*(RoReg  *)0x4006C01CU) /**< \brief (PARC) Receive Holding Register */
#define REG_PARC_VERSION           (*(RoReg  *)0x4006C020U) /**< \brief (PARC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PARC peripheral ========== */
#define PARC_PDCA_ID_RX             16      

#endif /* _SAM4L_PARC_INSTANCE_ */
