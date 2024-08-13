/**
 * \file
 *
 * \brief Instance description for DACC
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

#ifndef _SAM4L_DACC_INSTANCE_
#define _SAM4L_DACC_INSTANCE_

/* ========== Register definition for DACC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DACC_CR                (0x4003C000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR                (0x4003C004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CDR               (0x4003C008U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER               (0x4003C00CU) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR               (0x4003C010U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR               (0x4003C014U) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR               (0x4003C018U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_WPMR              (0x4003C0E4U) /**< \brief (DACC) Write Protect Mode Register */
#define REG_DACC_WPSR              (0x4003C0E8U) /**< \brief (DACC) Write Protect Status Register */
#define REG_DACC_VERSION           (0x4003C0FCU) /**< \brief (DACC) Version Register */
#else
#define REG_DACC_CR                (*(WoReg  *)0x4003C000U) /**< \brief (DACC) Control Register */
#define REG_DACC_MR                (*(RwReg  *)0x4003C004U) /**< \brief (DACC) Mode Register */
#define REG_DACC_CDR               (*(WoReg  *)0x4003C008U) /**< \brief (DACC) Conversion Data Register */
#define REG_DACC_IER               (*(WoReg  *)0x4003C00CU) /**< \brief (DACC) Interrupt Enable Register */
#define REG_DACC_IDR               (*(WoReg  *)0x4003C010U) /**< \brief (DACC) Interrupt Disable Register */
#define REG_DACC_IMR               (*(RoReg  *)0x4003C014U) /**< \brief (DACC) Interrupt Mask Register */
#define REG_DACC_ISR               (*(RoReg  *)0x4003C018U) /**< \brief (DACC) Interrupt Status Register */
#define REG_DACC_WPMR              (*(RwReg  *)0x4003C0E4U) /**< \brief (DACC) Write Protect Mode Register */
#define REG_DACC_WPSR              (*(RoReg  *)0x4003C0E8U) /**< \brief (DACC) Write Protect Status Register */
#define REG_DACC_VERSION           (*(RoReg  *)0x4003C0FCU) /**< \brief (DACC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DACC peripheral ========== */
#define DACC_DACC_EXT_TRIG_MSB      0       
#define DACC_DAC_RES_MSB            9       
#define DACC_PDCA_ID_TX             35      

#endif /* _SAM4L_DACC_INSTANCE_ */
