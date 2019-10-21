/**
 * \file
 *
 * \brief Instance description for TC0
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

#ifndef _SAM4L_TC0_INSTANCE_
#define _SAM4L_TC0_INSTANCE_

/* ========== Register definition for TC0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC0_CCR0               (0x40010000U) /**< \brief (TC0) Channel Control Register Channel 0 */
#define REG_TC0_CMR0               (0x40010004U) /**< \brief (TC0) Channel Mode Register Channel 0 */
#define REG_TC0_SMMR0              (0x40010008U) /**< \brief (TC0) Stepper Motor Mode Register 0 */
#define REG_TC0_CV0                (0x40010010U) /**< \brief (TC0) Counter Value Channel 0 */
#define REG_TC0_RA0                (0x40010014U) /**< \brief (TC0) Register A Channel 0 */
#define REG_TC0_RB0                (0x40010018U) /**< \brief (TC0) Register B Channel 0 */
#define REG_TC0_RC0                (0x4001001CU) /**< \brief (TC0) Register C Channel 0 */
#define REG_TC0_SR0                (0x40010020U) /**< \brief (TC0) Status Register Channel 0 */
#define REG_TC0_IER0               (0x40010024U) /**< \brief (TC0) Interrupt Enable Register Channel 0 */
#define REG_TC0_IDR0               (0x40010028U) /**< \brief (TC0) Interrupt Disable Register Channel 0 */
#define REG_TC0_IMR0               (0x4001002CU) /**< \brief (TC0) Interrupt Mask Register Channel 0 */
#define REG_TC0_CCR1               (0x40010040U) /**< \brief (TC0) Channel Control Register Channel 1 */
#define REG_TC0_CMR1               (0x40010044U) /**< \brief (TC0) Channel Mode Register Channel 1 */
#define REG_TC0_SMMR1              (0x40010048U) /**< \brief (TC0) Stepper Motor Mode Register 1 */
#define REG_TC0_CV1                (0x40010050U) /**< \brief (TC0) Counter Value Channel 1 */
#define REG_TC0_RA1                (0x40010054U) /**< \brief (TC0) Register A Channel 1 */
#define REG_TC0_RB1                (0x40010058U) /**< \brief (TC0) Register B Channel 1 */
#define REG_TC0_RC1                (0x4001005CU) /**< \brief (TC0) Register C Channel 1 */
#define REG_TC0_SR1                (0x40010060U) /**< \brief (TC0) Status Register Channel 1 */
#define REG_TC0_IER1               (0x40010064U) /**< \brief (TC0) Interrupt Enable Register Channel 1 */
#define REG_TC0_IDR1               (0x40010068U) /**< \brief (TC0) Interrupt Disable Register Channel 1 */
#define REG_TC0_IMR1               (0x4001006CU) /**< \brief (TC0) Interrupt Mask Register Channel 1 */
#define REG_TC0_CCR2               (0x40010080U) /**< \brief (TC0) Channel Control Register Channel 2 */
#define REG_TC0_CMR2               (0x40010084U) /**< \brief (TC0) Channel Mode Register Channel 2 */
#define REG_TC0_SMMR2              (0x40010088U) /**< \brief (TC0) Stepper Motor Mode Register 2 */
#define REG_TC0_CV2                (0x40010090U) /**< \brief (TC0) Counter Value Channel 2 */
#define REG_TC0_RA2                (0x40010094U) /**< \brief (TC0) Register A Channel 2 */
#define REG_TC0_RB2                (0x40010098U) /**< \brief (TC0) Register B Channel 2 */
#define REG_TC0_RC2                (0x4001009CU) /**< \brief (TC0) Register C Channel 2 */
#define REG_TC0_SR2                (0x400100A0U) /**< \brief (TC0) Status Register Channel 2 */
#define REG_TC0_IER2               (0x400100A4U) /**< \brief (TC0) Interrupt Enable Register Channel 2 */
#define REG_TC0_IDR2               (0x400100A8U) /**< \brief (TC0) Interrupt Disable Register Channel 2 */
#define REG_TC0_IMR2               (0x400100ACU) /**< \brief (TC0) Interrupt Mask Register Channel 2 */
#define REG_TC0_BCR                (0x400100C0U) /**< \brief (TC0) TC Block Control Register */
#define REG_TC0_BMR                (0x400100C4U) /**< \brief (TC0) TC Block Mode Register */
#define REG_TC0_WPMR               (0x400100E4U) /**< \brief (TC0) Write Protect Mode Register */
#define REG_TC0_FEATURES           (0x400100F8U) /**< \brief (TC0) Features Register */
#define REG_TC0_VERSION            (0x400100FCU) /**< \brief (TC0) Version Register */
#else
#define REG_TC0_CCR0               (*(WoReg  *)0x40010000U) /**< \brief (TC0) Channel Control Register Channel 0 */
#define REG_TC0_CMR0               (*(RwReg  *)0x40010004U) /**< \brief (TC0) Channel Mode Register Channel 0 */
#define REG_TC0_SMMR0              (*(RwReg  *)0x40010008U) /**< \brief (TC0) Stepper Motor Mode Register 0 */
#define REG_TC0_CV0                (*(RoReg  *)0x40010010U) /**< \brief (TC0) Counter Value Channel 0 */
#define REG_TC0_RA0                (*(RwReg  *)0x40010014U) /**< \brief (TC0) Register A Channel 0 */
#define REG_TC0_RB0                (*(RwReg  *)0x40010018U) /**< \brief (TC0) Register B Channel 0 */
#define REG_TC0_RC0                (*(RwReg  *)0x4001001CU) /**< \brief (TC0) Register C Channel 0 */
#define REG_TC0_SR0                (*(RoReg  *)0x40010020U) /**< \brief (TC0) Status Register Channel 0 */
#define REG_TC0_IER0               (*(WoReg  *)0x40010024U) /**< \brief (TC0) Interrupt Enable Register Channel 0 */
#define REG_TC0_IDR0               (*(WoReg  *)0x40010028U) /**< \brief (TC0) Interrupt Disable Register Channel 0 */
#define REG_TC0_IMR0               (*(RoReg  *)0x4001002CU) /**< \brief (TC0) Interrupt Mask Register Channel 0 */
#define REG_TC0_CCR1               (*(WoReg  *)0x40010040U) /**< \brief (TC0) Channel Control Register Channel 1 */
#define REG_TC0_CMR1               (*(RwReg  *)0x40010044U) /**< \brief (TC0) Channel Mode Register Channel 1 */
#define REG_TC0_SMMR1              (*(RwReg  *)0x40010048U) /**< \brief (TC0) Stepper Motor Mode Register 1 */
#define REG_TC0_CV1                (*(RoReg  *)0x40010050U) /**< \brief (TC0) Counter Value Channel 1 */
#define REG_TC0_RA1                (*(RwReg  *)0x40010054U) /**< \brief (TC0) Register A Channel 1 */
#define REG_TC0_RB1                (*(RwReg  *)0x40010058U) /**< \brief (TC0) Register B Channel 1 */
#define REG_TC0_RC1                (*(RwReg  *)0x4001005CU) /**< \brief (TC0) Register C Channel 1 */
#define REG_TC0_SR1                (*(RoReg  *)0x40010060U) /**< \brief (TC0) Status Register Channel 1 */
#define REG_TC0_IER1               (*(WoReg  *)0x40010064U) /**< \brief (TC0) Interrupt Enable Register Channel 1 */
#define REG_TC0_IDR1               (*(WoReg  *)0x40010068U) /**< \brief (TC0) Interrupt Disable Register Channel 1 */
#define REG_TC0_IMR1               (*(RoReg  *)0x4001006CU) /**< \brief (TC0) Interrupt Mask Register Channel 1 */
#define REG_TC0_CCR2               (*(WoReg  *)0x40010080U) /**< \brief (TC0) Channel Control Register Channel 2 */
#define REG_TC0_CMR2               (*(RwReg  *)0x40010084U) /**< \brief (TC0) Channel Mode Register Channel 2 */
#define REG_TC0_SMMR2              (*(RwReg  *)0x40010088U) /**< \brief (TC0) Stepper Motor Mode Register 2 */
#define REG_TC0_CV2                (*(RoReg  *)0x40010090U) /**< \brief (TC0) Counter Value Channel 2 */
#define REG_TC0_RA2                (*(RwReg  *)0x40010094U) /**< \brief (TC0) Register A Channel 2 */
#define REG_TC0_RB2                (*(RwReg  *)0x40010098U) /**< \brief (TC0) Register B Channel 2 */
#define REG_TC0_RC2                (*(RwReg  *)0x4001009CU) /**< \brief (TC0) Register C Channel 2 */
#define REG_TC0_SR2                (*(RoReg  *)0x400100A0U) /**< \brief (TC0) Status Register Channel 2 */
#define REG_TC0_IER2               (*(WoReg  *)0x400100A4U) /**< \brief (TC0) Interrupt Enable Register Channel 2 */
#define REG_TC0_IDR2               (*(WoReg  *)0x400100A8U) /**< \brief (TC0) Interrupt Disable Register Channel 2 */
#define REG_TC0_IMR2               (*(RoReg  *)0x400100ACU) /**< \brief (TC0) Interrupt Mask Register Channel 2 */
#define REG_TC0_BCR                (*(WoReg  *)0x400100C0U) /**< \brief (TC0) TC Block Control Register */
#define REG_TC0_BMR                (*(RwReg  *)0x400100C4U) /**< \brief (TC0) TC Block Mode Register */
#define REG_TC0_WPMR               (*(RwReg  *)0x400100E4U) /**< \brief (TC0) Write Protect Mode Register */
#define REG_TC0_FEATURES           (*(RoReg  *)0x400100F8U) /**< \brief (TC0) Features Register */
#define REG_TC0_VERSION            (*(RoReg  *)0x400100FCU) /**< \brief (TC0) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TC0 peripheral ========== */
#define TC0_CLK_DIV1                gen_clk_tc0
#define TC0_CLK_DIV2                2       
#define TC0_CLK_DIV3                8       
#define TC0_CLK_DIV4                32      
#define TC0_CLK_DIV5                128     
#define TC0_GCLK_NUM                5       

#endif /* _SAM4L_TC0_INSTANCE_ */
