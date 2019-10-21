/**
 * \file
 *
 * \brief Instance description for TWIS1
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

#ifndef _SAM4L_TWIS1_INSTANCE_
#define _SAM4L_TWIS1_INSTANCE_

/* ========== Register definition for TWIS1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TWIS1_CR               (0x4001C400U) /**< \brief (TWIS1) Control Register */
#define REG_TWIS1_NBYTES           (0x4001C404U) /**< \brief (TWIS1) NBYTES Register */
#define REG_TWIS1_TR               (0x4001C408U) /**< \brief (TWIS1) Timing Register */
#define REG_TWIS1_RHR              (0x4001C40CU) /**< \brief (TWIS1) Receive Holding Register */
#define REG_TWIS1_THR              (0x4001C410U) /**< \brief (TWIS1) Transmit Holding Register */
#define REG_TWIS1_PECR             (0x4001C414U) /**< \brief (TWIS1) Packet Error Check Register */
#define REG_TWIS1_SR               (0x4001C418U) /**< \brief (TWIS1) Status Register */
#define REG_TWIS1_IER              (0x4001C41CU) /**< \brief (TWIS1) Interrupt Enable Register */
#define REG_TWIS1_IDR              (0x4001C420U) /**< \brief (TWIS1) Interrupt Disable Register */
#define REG_TWIS1_IMR              (0x4001C424U) /**< \brief (TWIS1) Interrupt Mask Register */
#define REG_TWIS1_SCR              (0x4001C428U) /**< \brief (TWIS1) Status Clear Register */
#define REG_TWIS1_PR               (0x4001C42CU) /**< \brief (TWIS1) Parameter Register */
#define REG_TWIS1_VR               (0x4001C430U) /**< \brief (TWIS1) Version Register */
#define REG_TWIS1_HSTR             (0x4001C434U) /**< \brief (TWIS1) HS-mode Timing Register */
#define REG_TWIS1_SRR              (0x4001C438U) /**< \brief (TWIS1) Slew Rate Register */
#define REG_TWIS1_HSSRR            (0x4001C43CU) /**< \brief (TWIS1) HS-mode Slew Rate Register */
#else
#define REG_TWIS1_CR               (*(RwReg  *)0x4001C400U) /**< \brief (TWIS1) Control Register */
#define REG_TWIS1_NBYTES           (*(RwReg  *)0x4001C404U) /**< \brief (TWIS1) NBYTES Register */
#define REG_TWIS1_TR               (*(RwReg  *)0x4001C408U) /**< \brief (TWIS1) Timing Register */
#define REG_TWIS1_RHR              (*(RoReg  *)0x4001C40CU) /**< \brief (TWIS1) Receive Holding Register */
#define REG_TWIS1_THR              (*(WoReg  *)0x4001C410U) /**< \brief (TWIS1) Transmit Holding Register */
#define REG_TWIS1_PECR             (*(RoReg  *)0x4001C414U) /**< \brief (TWIS1) Packet Error Check Register */
#define REG_TWIS1_SR               (*(RoReg  *)0x4001C418U) /**< \brief (TWIS1) Status Register */
#define REG_TWIS1_IER              (*(WoReg  *)0x4001C41CU) /**< \brief (TWIS1) Interrupt Enable Register */
#define REG_TWIS1_IDR              (*(WoReg  *)0x4001C420U) /**< \brief (TWIS1) Interrupt Disable Register */
#define REG_TWIS1_IMR              (*(RoReg  *)0x4001C424U) /**< \brief (TWIS1) Interrupt Mask Register */
#define REG_TWIS1_SCR              (*(WoReg  *)0x4001C428U) /**< \brief (TWIS1) Status Clear Register */
#define REG_TWIS1_PR               (*(RoReg  *)0x4001C42CU) /**< \brief (TWIS1) Parameter Register */
#define REG_TWIS1_VR               (*(RoReg  *)0x4001C430U) /**< \brief (TWIS1) Version Register */
#define REG_TWIS1_HSTR             (*(RwReg  *)0x4001C434U) /**< \brief (TWIS1) HS-mode Timing Register */
#define REG_TWIS1_SRR              (*(RwReg  *)0x4001C438U) /**< \brief (TWIS1) Slew Rate Register */
#define REG_TWIS1_HSSRR            (*(RwReg  *)0x4001C43CU) /**< \brief (TWIS1) HS-mode Slew Rate Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TWIS1 peripheral ========== */
#define TWIS1_PDCA_ID_RX            10      
#define TWIS1_PDCA_ID_TX            28      

#endif /* _SAM4L_TWIS1_INSTANCE_ */
