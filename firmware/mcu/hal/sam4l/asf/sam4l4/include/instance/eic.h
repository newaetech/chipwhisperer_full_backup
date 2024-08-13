/**
 * \file
 *
 * \brief Instance description for EIC
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

#ifndef _SAM4L_EIC_INSTANCE_
#define _SAM4L_EIC_INSTANCE_

/* ========== Register definition for EIC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EIC_IER                (0x400F1000U) /**< \brief (EIC) Interrupt Enable Register */
#define REG_EIC_IDR                (0x400F1004U) /**< \brief (EIC) Interrupt Disable Register */
#define REG_EIC_IMR                (0x400F1008U) /**< \brief (EIC) Interrupt Mask Register */
#define REG_EIC_ISR                (0x400F100CU) /**< \brief (EIC) Interrupt Status Register */
#define REG_EIC_ICR                (0x400F1010U) /**< \brief (EIC) Interrupt Clear Register */
#define REG_EIC_MODE               (0x400F1014U) /**< \brief (EIC) Mode Register */
#define REG_EIC_EDGE               (0x400F1018U) /**< \brief (EIC) Edge Register */
#define REG_EIC_LEVEL              (0x400F101CU) /**< \brief (EIC) Level Register */
#define REG_EIC_FILTER             (0x400F1020U) /**< \brief (EIC) Filter Register */
#define REG_EIC_ASYNC              (0x400F1028U) /**< \brief (EIC) Asynchronous Register */
#define REG_EIC_EN                 (0x400F1030U) /**< \brief (EIC) Enable Register */
#define REG_EIC_DIS                (0x400F1034U) /**< \brief (EIC) Disable Register */
#define REG_EIC_CTRL               (0x400F1038U) /**< \brief (EIC) Control Register */
#define REG_EIC_VERSION            (0x400F13FCU) /**< \brief (EIC) Version Register */
#else
#define REG_EIC_IER                (*(WoReg  *)0x400F1000U) /**< \brief (EIC) Interrupt Enable Register */
#define REG_EIC_IDR                (*(WoReg  *)0x400F1004U) /**< \brief (EIC) Interrupt Disable Register */
#define REG_EIC_IMR                (*(RoReg  *)0x400F1008U) /**< \brief (EIC) Interrupt Mask Register */
#define REG_EIC_ISR                (*(RoReg  *)0x400F100CU) /**< \brief (EIC) Interrupt Status Register */
#define REG_EIC_ICR                (*(WoReg  *)0x400F1010U) /**< \brief (EIC) Interrupt Clear Register */
#define REG_EIC_MODE               (*(RwReg  *)0x400F1014U) /**< \brief (EIC) Mode Register */
#define REG_EIC_EDGE               (*(RwReg  *)0x400F1018U) /**< \brief (EIC) Edge Register */
#define REG_EIC_LEVEL              (*(RwReg  *)0x400F101CU) /**< \brief (EIC) Level Register */
#define REG_EIC_FILTER             (*(RwReg  *)0x400F1020U) /**< \brief (EIC) Filter Register */
#define REG_EIC_ASYNC              (*(RwReg  *)0x400F1028U) /**< \brief (EIC) Asynchronous Register */
#define REG_EIC_EN                 (*(WoReg  *)0x400F1030U) /**< \brief (EIC) Enable Register */
#define REG_EIC_DIS                (*(WoReg  *)0x400F1034U) /**< \brief (EIC) Disable Register */
#define REG_EIC_CTRL               (*(RoReg  *)0x400F1038U) /**< \brief (EIC) Control Register */
#define REG_EIC_VERSION            (*(RoReg  *)0x400F13FCU) /**< \brief (EIC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EIC peripheral ========== */
#define EIC_STD_NUM                 8       
#define EIC_EIC_EDGE_IRQ            0       
#define EIC_EIC_FALLING_EDGE        0       
#define EIC_EIC_FILTER_OFF          0       
#define EIC_EIC_FILTER_ON           1       
#define EIC_EIC_HIGH_LEVEL          1       
#define EIC_EIC_LEVEL_IRQ           1       
#define EIC_EIC_LOW_LEVEL           0       
#define EIC_EIC_RISING_EDGE         1       
#define EIC_EIC_SYNC                0       
#define EIC_EIC_USE_ASYNC           1       

#endif /* _SAM4L_EIC_INSTANCE_ */
