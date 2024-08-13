/**
 * \file
 *
 * \brief Instance description for LCDCA
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

#ifndef _SAM4L_LCDCA_INSTANCE_
#define _SAM4L_LCDCA_INSTANCE_

/* ========== Register definition for LCDCA peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_LCDCA_CR               (0x40080000U) /**< \brief (LCDCA) Control Register */
#define REG_LCDCA_CFG              (0x40080004U) /**< \brief (LCDCA) Configuration Register */
#define REG_LCDCA_TIM              (0x40080008U) /**< \brief (LCDCA) Timing Register */
#define REG_LCDCA_SR               (0x4008000CU) /**< \brief (LCDCA) Status Register */
#define REG_LCDCA_SCR              (0x40080010U) /**< \brief (LCDCA) Status Clear Register */
#define REG_LCDCA_DRL0             (0x40080014U) /**< \brief (LCDCA) Data Register Low 0 */
#define REG_LCDCA_DRH0             (0x40080018U) /**< \brief (LCDCA) Data Register High 0 */
#define REG_LCDCA_DRL1             (0x4008001CU) /**< \brief (LCDCA) Data Register Low 1 */
#define REG_LCDCA_DRH1             (0x40080020U) /**< \brief (LCDCA) Data Register High 1 */
#define REG_LCDCA_DRL2             (0x40080024U) /**< \brief (LCDCA) Data Register Low 2 */
#define REG_LCDCA_DRH2             (0x40080028U) /**< \brief (LCDCA) Data Register High 2 */
#define REG_LCDCA_DRL3             (0x4008002CU) /**< \brief (LCDCA) Data Register Low 3 */
#define REG_LCDCA_DRH3             (0x40080030U) /**< \brief (LCDCA) Data Register High 3 */
#define REG_LCDCA_IADR             (0x40080034U) /**< \brief (LCDCA) Indirect Access Data Register */
#define REG_LCDCA_BCFG             (0x40080038U) /**< \brief (LCDCA) Blink Configuration Register */
#define REG_LCDCA_CSRCFG           (0x4008003CU) /**< \brief (LCDCA) Circular Shift Register Configuration */
#define REG_LCDCA_CMCFG            (0x40080040U) /**< \brief (LCDCA) Character Mapping Configuration Register */
#define REG_LCDCA_CMDR             (0x40080044U) /**< \brief (LCDCA) Character Mapping Data Register */
#define REG_LCDCA_ACMCFG           (0x40080048U) /**< \brief (LCDCA) Automated Character Mapping Configuration Register */
#define REG_LCDCA_ACMDR            (0x4008004CU) /**< \brief (LCDCA) Automated Character Mapping Data Register */
#define REG_LCDCA_ABMCFG           (0x40080050U) /**< \brief (LCDCA) Automated Bit Mapping Configuration Register */
#define REG_LCDCA_ABMDR            (0x40080054U) /**< \brief (LCDCA) Automated Bit Mapping Data Register */
#define REG_LCDCA_IER              (0x40080058U) /**< \brief (LCDCA) Interrupt Enable Register */
#define REG_LCDCA_IDR              (0x4008005CU) /**< \brief (LCDCA) Interrupt Disable Register */
#define REG_LCDCA_IMR              (0x40080060U) /**< \brief (LCDCA) Interrupt Mask Register */
#define REG_LCDCA_VERSION          (0x40080064U) /**< \brief (LCDCA) Version Register */
#else
#define REG_LCDCA_CR               (*(WoReg  *)0x40080000U) /**< \brief (LCDCA) Control Register */
#define REG_LCDCA_CFG              (*(RwReg  *)0x40080004U) /**< \brief (LCDCA) Configuration Register */
#define REG_LCDCA_TIM              (*(RwReg  *)0x40080008U) /**< \brief (LCDCA) Timing Register */
#define REG_LCDCA_SR               (*(RoReg  *)0x4008000CU) /**< \brief (LCDCA) Status Register */
#define REG_LCDCA_SCR              (*(WoReg  *)0x40080010U) /**< \brief (LCDCA) Status Clear Register */
#define REG_LCDCA_DRL0             (*(RwReg  *)0x40080014U) /**< \brief (LCDCA) Data Register Low 0 */
#define REG_LCDCA_DRH0             (*(RwReg  *)0x40080018U) /**< \brief (LCDCA) Data Register High 0 */
#define REG_LCDCA_DRL1             (*(RwReg  *)0x4008001CU) /**< \brief (LCDCA) Data Register Low 1 */
#define REG_LCDCA_DRH1             (*(RwReg  *)0x40080020U) /**< \brief (LCDCA) Data Register High 1 */
#define REG_LCDCA_DRL2             (*(RwReg  *)0x40080024U) /**< \brief (LCDCA) Data Register Low 2 */
#define REG_LCDCA_DRH2             (*(RwReg  *)0x40080028U) /**< \brief (LCDCA) Data Register High 2 */
#define REG_LCDCA_DRL3             (*(RwReg  *)0x4008002CU) /**< \brief (LCDCA) Data Register Low 3 */
#define REG_LCDCA_DRH3             (*(RwReg  *)0x40080030U) /**< \brief (LCDCA) Data Register High 3 */
#define REG_LCDCA_IADR             (*(WoReg  *)0x40080034U) /**< \brief (LCDCA) Indirect Access Data Register */
#define REG_LCDCA_BCFG             (*(RwReg  *)0x40080038U) /**< \brief (LCDCA) Blink Configuration Register */
#define REG_LCDCA_CSRCFG           (*(RwReg  *)0x4008003CU) /**< \brief (LCDCA) Circular Shift Register Configuration */
#define REG_LCDCA_CMCFG            (*(RwReg  *)0x40080040U) /**< \brief (LCDCA) Character Mapping Configuration Register */
#define REG_LCDCA_CMDR             (*(WoReg  *)0x40080044U) /**< \brief (LCDCA) Character Mapping Data Register */
#define REG_LCDCA_ACMCFG           (*(RwReg  *)0x40080048U) /**< \brief (LCDCA) Automated Character Mapping Configuration Register */
#define REG_LCDCA_ACMDR            (*(WoReg  *)0x4008004CU) /**< \brief (LCDCA) Automated Character Mapping Data Register */
#define REG_LCDCA_ABMCFG           (*(RwReg  *)0x40080050U) /**< \brief (LCDCA) Automated Bit Mapping Configuration Register */
#define REG_LCDCA_ABMDR            (*(WoReg  *)0x40080054U) /**< \brief (LCDCA) Automated Bit Mapping Data Register */
#define REG_LCDCA_IER              (*(WoReg  *)0x40080058U) /**< \brief (LCDCA) Interrupt Enable Register */
#define REG_LCDCA_IDR              (*(WoReg  *)0x4008005CU) /**< \brief (LCDCA) Interrupt Disable Register */
#define REG_LCDCA_IMR              (*(RoReg  *)0x40080060U) /**< \brief (LCDCA) Interrupt Mask Register */
#define REG_LCDCA_VERSION          (*(RoReg  *)0x40080064U) /**< \brief (LCDCA) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for LCDCA peripheral ========== */
#define LCDCA_PDCA_ID_TX_ABM        38      
#define LCDCA_PDCA_ID_TX_ACM        37      

#endif /* _SAM4L_LCDCA_INSTANCE_ */
