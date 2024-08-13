/**
 * \file
 *
 * \brief Instance description for TWIM3
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

#ifndef _SAM4L_TWIM3_INSTANCE_
#define _SAM4L_TWIM3_INSTANCE_

/* ========== Register definition for TWIM3 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TWIM3_CR               (0x4007C000U) /**< \brief (TWIM3) Control Register */
#define REG_TWIM3_CWGR             (0x4007C004U) /**< \brief (TWIM3) Clock Waveform Generator Register */
#define REG_TWIM3_SMBTR            (0x4007C008U) /**< \brief (TWIM3) SMBus Timing Register */
#define REG_TWIM3_CMDR             (0x4007C00CU) /**< \brief (TWIM3) Command Register */
#define REG_TWIM3_NCMDR            (0x4007C010U) /**< \brief (TWIM3) Next Command Register */
#define REG_TWIM3_RHR              (0x4007C014U) /**< \brief (TWIM3) Receive Holding Register */
#define REG_TWIM3_THR              (0x4007C018U) /**< \brief (TWIM3) Transmit Holding Register */
#define REG_TWIM3_SR               (0x4007C01CU) /**< \brief (TWIM3) Status Register */
#define REG_TWIM3_IER              (0x4007C020U) /**< \brief (TWIM3) Interrupt Enable Register */
#define REG_TWIM3_IDR              (0x4007C024U) /**< \brief (TWIM3) Interrupt Disable Register */
#define REG_TWIM3_IMR              (0x4007C028U) /**< \brief (TWIM3) Interrupt Mask Register */
#define REG_TWIM3_SCR              (0x4007C02CU) /**< \brief (TWIM3) Status Clear Register */
#define REG_TWIM3_PR               (0x4007C030U) /**< \brief (TWIM3) Parameter Register */
#define REG_TWIM3_VR               (0x4007C034U) /**< \brief (TWIM3) Version Register */
#define REG_TWIM3_HSCWGR           (0x4007C038U) /**< \brief (TWIM3) HS-mode Clock Waveform Generator */
#define REG_TWIM3_SRR              (0x4007C03CU) /**< \brief (TWIM3) Slew Rate Register */
#define REG_TWIM3_HSSRR            (0x4007C040U) /**< \brief (TWIM3) HS-mode Slew Rate Register */
#else
#define REG_TWIM3_CR               (*(WoReg  *)0x4007C000U) /**< \brief (TWIM3) Control Register */
#define REG_TWIM3_CWGR             (*(RwReg  *)0x4007C004U) /**< \brief (TWIM3) Clock Waveform Generator Register */
#define REG_TWIM3_SMBTR            (*(RwReg  *)0x4007C008U) /**< \brief (TWIM3) SMBus Timing Register */
#define REG_TWIM3_CMDR             (*(RwReg  *)0x4007C00CU) /**< \brief (TWIM3) Command Register */
#define REG_TWIM3_NCMDR            (*(RwReg  *)0x4007C010U) /**< \brief (TWIM3) Next Command Register */
#define REG_TWIM3_RHR              (*(RoReg  *)0x4007C014U) /**< \brief (TWIM3) Receive Holding Register */
#define REG_TWIM3_THR              (*(WoReg  *)0x4007C018U) /**< \brief (TWIM3) Transmit Holding Register */
#define REG_TWIM3_SR               (*(RoReg  *)0x4007C01CU) /**< \brief (TWIM3) Status Register */
#define REG_TWIM3_IER              (*(WoReg  *)0x4007C020U) /**< \brief (TWIM3) Interrupt Enable Register */
#define REG_TWIM3_IDR              (*(WoReg  *)0x4007C024U) /**< \brief (TWIM3) Interrupt Disable Register */
#define REG_TWIM3_IMR              (*(RoReg  *)0x4007C028U) /**< \brief (TWIM3) Interrupt Mask Register */
#define REG_TWIM3_SCR              (*(WoReg  *)0x4007C02CU) /**< \brief (TWIM3) Status Clear Register */
#define REG_TWIM3_PR               (*(RoReg  *)0x4007C030U) /**< \brief (TWIM3) Parameter Register */
#define REG_TWIM3_VR               (*(RoReg  *)0x4007C034U) /**< \brief (TWIM3) Version Register */
#define REG_TWIM3_HSCWGR           (*(RwReg  *)0x4007C038U) /**< \brief (TWIM3) HS-mode Clock Waveform Generator */
#define REG_TWIM3_SRR              (*(RwReg  *)0x4007C03CU) /**< \brief (TWIM3) Slew Rate Register */
#define REG_TWIM3_HSSRR            (*(RwReg  *)0x4007C040U) /**< \brief (TWIM3) HS-mode Slew Rate Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for TWIM3 peripheral ========== */
#define TWIM3_PDCA_ID_RX            8       
#define TWIM3_PDCA_ID_TX            26      

#endif /* _SAM4L_TWIM3_INSTANCE_ */
