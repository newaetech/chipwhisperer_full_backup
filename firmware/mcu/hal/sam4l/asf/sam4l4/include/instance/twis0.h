/**
 * \file
 *
 * \brief Instance description for TWIS0
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

#ifndef _SAM4L_TWIS0_INSTANCE_
#define _SAM4L_TWIS0_INSTANCE_

/* ========== Register definition for TWIS0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TWIS0_CR               (0x40018400U) /**< \brief (TWIS0) Control Register */
#define REG_TWIS0_NBYTES           (0x40018404U) /**< \brief (TWIS0) NBYTES Register */
#define REG_TWIS0_TR               (0x40018408U) /**< \brief (TWIS0) Timing Register */
#define REG_TWIS0_RHR              (0x4001840CU) /**< \brief (TWIS0) Receive Holding Register */
#define REG_TWIS0_THR              (0x40018410U) /**< \brief (TWIS0) Transmit Holding Register */
#define REG_TWIS0_PECR             (0x40018414U) /**< \brief (TWIS0) Packet Error Check Register */
#define REG_TWIS0_SR               (0x40018418U) /**< \brief (TWIS0) Status Register */
#define REG_TWIS0_IER              (0x4001841CU) /**< \brief (TWIS0) Interrupt Enable Register */
#define REG_TWIS0_IDR              (0x40018420U) /**< \brief (TWIS0) Interrupt Disable Register */
#define REG_TWIS0_IMR              (0x40018424U) /**< \brief (TWIS0) Interrupt Mask Register */
#define REG_TWIS0_SCR              (0x40018428U) /**< \brief (TWIS0) Status Clear Register */
#define REG_TWIS0_PR               (0x4001842CU) /**< \brief (TWIS0) Parameter Register */
#define REG_TWIS0_VR               (0x40018430U) /**< \brief (TWIS0) Version Register */
#define REG_TWIS0_HSTR             (0x40018434U) /**< \brief (TWIS0) HS-mode Timing Register */
#define REG_TWIS0_SRR              (0x40018438U) /**< \brief (TWIS0) Slew Rate Register */
#define REG_TWIS0_HSSRR            (0x4001843CU) /**< \brief (TWIS0) HS-mode Slew Rate Register */
#else
#define REG_TWIS0_CR               (*(RwReg  *)0x40018400U) /**< \brief (TWIS0) Control Register */
#define REG_TWIS0_NBYTES           (*(RwReg  *)0x40018404U) /**< \brief (TWIS0) NBYTES Register */
#define REG_TWIS0_TR               (*(RwReg  *)0x40018408U) /**< \brief (TWIS0) Timing Register */
#define REG_TWIS0_RHR              (*(RoReg  *)0x4001840CU) /**< \brief (TWIS0) Receive Holding Register */
#define REG_TWIS0_THR              (*(WoReg  *)0x40018410U) /**< \brief (TWIS0) Transmit Holding Register */
#define REG_TWIS0_PECR             (*(RoReg  *)0x40018414U) /**< \brief (TWIS0) Packet Error Check Register */
#define REG_TWIS0_SR               (*(RoReg  *)0x40018418U) /**< \brief (TWIS0) Status Register */
#define REG_TWIS0_IER              (*(WoReg  *)0x4001841CU) /**< \brief (TWIS0) Interrupt Enable Register */
#define REG_TWIS0_IDR              (*(WoReg  *)0x40018420U) /**< \brief (TWIS0) Interrupt Disable Register */
#define REG_TWIS0_IMR              (*(RoReg  *)0x40018424U) /**< \brief (TWIS0) Interrupt Mask Register */
#define REG_TWIS0_SCR              (*(WoReg  *)0x40018428U) /**< \brief (TWIS0) Status Clear Register */
#define REG_TWIS0_PR               (*(RoReg  *)0x4001842CU) /**< \brief (TWIS0) Parameter Register */
#define REG_TWIS0_VR               (*(RoReg  *)0x40018430U) /**< \brief (TWIS0) Version Register */
#define REG_TWIS0_HSTR             (*(RwReg  *)0x40018434U) /**< \brief (TWIS0) HS-mode Timing Register */
#define REG_TWIS0_SRR              (*(RwReg  *)0x40018438U) /**< \brief (TWIS0) Slew Rate Register */
#define REG_TWIS0_HSSRR            (*(RwReg  *)0x4001843CU) /**< \brief (TWIS0) HS-mode Slew Rate Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TWIS0 peripheral ========== */
#define TWIS0_PDCA_ID_RX            9       
#define TWIS0_PDCA_ID_TX            27      

#endif /* _SAM4L_TWIS0_INSTANCE_ */
