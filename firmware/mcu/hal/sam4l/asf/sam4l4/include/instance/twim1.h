/**
 * \file
 *
 * \brief Instance description for TWIM1
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

#ifndef _SAM4L_TWIM1_INSTANCE_
#define _SAM4L_TWIM1_INSTANCE_

/* ========== Register definition for TWIM1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TWIM1_CR               (0x4001C000U) /**< \brief (TWIM1) Control Register */
#define REG_TWIM1_CWGR             (0x4001C004U) /**< \brief (TWIM1) Clock Waveform Generator Register */
#define REG_TWIM1_SMBTR            (0x4001C008U) /**< \brief (TWIM1) SMBus Timing Register */
#define REG_TWIM1_CMDR             (0x4001C00CU) /**< \brief (TWIM1) Command Register */
#define REG_TWIM1_NCMDR            (0x4001C010U) /**< \brief (TWIM1) Next Command Register */
#define REG_TWIM1_RHR              (0x4001C014U) /**< \brief (TWIM1) Receive Holding Register */
#define REG_TWIM1_THR              (0x4001C018U) /**< \brief (TWIM1) Transmit Holding Register */
#define REG_TWIM1_SR               (0x4001C01CU) /**< \brief (TWIM1) Status Register */
#define REG_TWIM1_IER              (0x4001C020U) /**< \brief (TWIM1) Interrupt Enable Register */
#define REG_TWIM1_IDR              (0x4001C024U) /**< \brief (TWIM1) Interrupt Disable Register */
#define REG_TWIM1_IMR              (0x4001C028U) /**< \brief (TWIM1) Interrupt Mask Register */
#define REG_TWIM1_SCR              (0x4001C02CU) /**< \brief (TWIM1) Status Clear Register */
#define REG_TWIM1_PR               (0x4001C030U) /**< \brief (TWIM1) Parameter Register */
#define REG_TWIM1_VR               (0x4001C034U) /**< \brief (TWIM1) Version Register */
#define REG_TWIM1_HSCWGR           (0x4001C038U) /**< \brief (TWIM1) HS-mode Clock Waveform Generator */
#define REG_TWIM1_SRR              (0x4001C03CU) /**< \brief (TWIM1) Slew Rate Register */
#define REG_TWIM1_HSSRR            (0x4001C040U) /**< \brief (TWIM1) HS-mode Slew Rate Register */
#else
#define REG_TWIM1_CR               (*(WoReg  *)0x4001C000U) /**< \brief (TWIM1) Control Register */
#define REG_TWIM1_CWGR             (*(RwReg  *)0x4001C004U) /**< \brief (TWIM1) Clock Waveform Generator Register */
#define REG_TWIM1_SMBTR            (*(RwReg  *)0x4001C008U) /**< \brief (TWIM1) SMBus Timing Register */
#define REG_TWIM1_CMDR             (*(RwReg  *)0x4001C00CU) /**< \brief (TWIM1) Command Register */
#define REG_TWIM1_NCMDR            (*(RwReg  *)0x4001C010U) /**< \brief (TWIM1) Next Command Register */
#define REG_TWIM1_RHR              (*(RoReg  *)0x4001C014U) /**< \brief (TWIM1) Receive Holding Register */
#define REG_TWIM1_THR              (*(WoReg  *)0x4001C018U) /**< \brief (TWIM1) Transmit Holding Register */
#define REG_TWIM1_SR               (*(RoReg  *)0x4001C01CU) /**< \brief (TWIM1) Status Register */
#define REG_TWIM1_IER              (*(WoReg  *)0x4001C020U) /**< \brief (TWIM1) Interrupt Enable Register */
#define REG_TWIM1_IDR              (*(WoReg  *)0x4001C024U) /**< \brief (TWIM1) Interrupt Disable Register */
#define REG_TWIM1_IMR              (*(RoReg  *)0x4001C028U) /**< \brief (TWIM1) Interrupt Mask Register */
#define REG_TWIM1_SCR              (*(WoReg  *)0x4001C02CU) /**< \brief (TWIM1) Status Clear Register */
#define REG_TWIM1_PR               (*(RoReg  *)0x4001C030U) /**< \brief (TWIM1) Parameter Register */
#define REG_TWIM1_VR               (*(RoReg  *)0x4001C034U) /**< \brief (TWIM1) Version Register */
#define REG_TWIM1_HSCWGR           (*(RwReg  *)0x4001C038U) /**< \brief (TWIM1) HS-mode Clock Waveform Generator */
#define REG_TWIM1_SRR              (*(RwReg  *)0x4001C03CU) /**< \brief (TWIM1) Slew Rate Register */
#define REG_TWIM1_HSSRR            (*(RwReg  *)0x4001C040U) /**< \brief (TWIM1) HS-mode Slew Rate Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TWIM1 peripheral ========== */
#define TWIM1_PDCA_ID_RX            6       
#define TWIM1_PDCA_ID_TX            24      

#endif /* _SAM4L_TWIM1_INSTANCE_ */
