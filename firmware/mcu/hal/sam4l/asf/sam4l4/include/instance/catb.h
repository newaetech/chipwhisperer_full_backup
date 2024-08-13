/**
 * \file
 *
 * \brief Instance description for CATB
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

#ifndef _SAM4L_CATB_INSTANCE_
#define _SAM4L_CATB_INSTANCE_

/* ========== Register definition for CATB peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_CATB_CR                (0x40070000U) /**< \brief (CATB) Control Register */
#define REG_CATB_CNTCR             (0x40070004U) /**< \brief (CATB) Counter Control Register */
#define REG_CATB_IDLE              (0x40070008U) /**< \brief (CATB) Sensor Idle Level */
#define REG_CATB_LEVEL             (0x4007000CU) /**< \brief (CATB) Sensor Relative Level */
#define REG_CATB_RAW               (0x40070010U) /**< \brief (CATB) Sensor Raw Value */
#define REG_CATB_TIMING            (0x40070014U) /**< \brief (CATB) Filter Timing Register */
#define REG_CATB_THRESH            (0x40070018U) /**< \brief (CATB) Threshold Register */
#define REG_CATB_PINSEL            (0x4007001CU) /**< \brief (CATB) Pin Selection Register */
#define REG_CATB_DMA               (0x40070020U) /**< \brief (CATB) Direct Memory Access Register */
#define REG_CATB_ISR               (0x40070024U) /**< \brief (CATB) Interrupt Status Register */
#define REG_CATB_IER               (0x40070028U) /**< \brief (CATB) Interrupt Enable Register */
#define REG_CATB_IDR               (0x4007002CU) /**< \brief (CATB) Interrupt Disable Register */
#define REG_CATB_IMR               (0x40070030U) /**< \brief (CATB) Interrupt Mask Register */
#define REG_CATB_SCR               (0x40070034U) /**< \brief (CATB) Status Clear Register */
#define REG_CATB_INTCH0            (0x40070040U) /**< \brief (CATB) In-Touch Status Register 0 */
#define REG_CATB_INTCHCLR0         (0x40070050U) /**< \brief (CATB) In-Touch Status Clear Register 0 */
#define REG_CATB_OUTTCH0           (0x40070060U) /**< \brief (CATB) Out-of-Touch Status Register 0 */
#define REG_CATB_OUTTCHCLR0        (0x40070070U) /**< \brief (CATB) Out-of-Touch Status Clear Register 0 */
#define REG_CATB_PARAMETER         (0x400700F8U) /**< \brief (CATB) Parameter Register */
#define REG_CATB_VERSION           (0x400700FCU) /**< \brief (CATB) Version Register */
#else
#define REG_CATB_CR                (*(RwReg  *)0x40070000U) /**< \brief (CATB) Control Register */
#define REG_CATB_CNTCR             (*(RwReg  *)0x40070004U) /**< \brief (CATB) Counter Control Register */
#define REG_CATB_IDLE              (*(RwReg  *)0x40070008U) /**< \brief (CATB) Sensor Idle Level */
#define REG_CATB_LEVEL             (*(RoReg  *)0x4007000CU) /**< \brief (CATB) Sensor Relative Level */
#define REG_CATB_RAW               (*(RoReg  *)0x40070010U) /**< \brief (CATB) Sensor Raw Value */
#define REG_CATB_TIMING            (*(RwReg  *)0x40070014U) /**< \brief (CATB) Filter Timing Register */
#define REG_CATB_THRESH            (*(RwReg  *)0x40070018U) /**< \brief (CATB) Threshold Register */
#define REG_CATB_PINSEL            (*(RwReg  *)0x4007001CU) /**< \brief (CATB) Pin Selection Register */
#define REG_CATB_DMA               (*(RwReg  *)0x40070020U) /**< \brief (CATB) Direct Memory Access Register */
#define REG_CATB_ISR               (*(RoReg  *)0x40070024U) /**< \brief (CATB) Interrupt Status Register */
#define REG_CATB_IER               (*(WoReg  *)0x40070028U) /**< \brief (CATB) Interrupt Enable Register */
#define REG_CATB_IDR               (*(WoReg  *)0x4007002CU) /**< \brief (CATB) Interrupt Disable Register */
#define REG_CATB_IMR               (*(RoReg  *)0x40070030U) /**< \brief (CATB) Interrupt Mask Register */
#define REG_CATB_SCR               (*(WoReg  *)0x40070034U) /**< \brief (CATB) Status Clear Register */
#define REG_CATB_INTCH0            (*(RoReg  *)0x40070040U) /**< \brief (CATB) In-Touch Status Register 0 */
#define REG_CATB_INTCHCLR0         (*(WoReg  *)0x40070050U) /**< \brief (CATB) In-Touch Status Clear Register 0 */
#define REG_CATB_OUTTCH0           (*(RoReg  *)0x40070060U) /**< \brief (CATB) Out-of-Touch Status Register 0 */
#define REG_CATB_OUTTCHCLR0        (*(WoReg  *)0x40070070U) /**< \brief (CATB) Out-of-Touch Status Clear Register 0 */
#define REG_CATB_PARAMETER         (*(RoReg  *)0x400700F8U) /**< \brief (CATB) Parameter Register */
#define REG_CATB_VERSION           (*(RoReg  *)0x400700FCU) /**< \brief (CATB) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for CATB peripheral ========== */
#define CATB_GCLK_NUM               3       
#define CATB_PDCA_ID_RX             12      
#define CATB_PDCA_ID_TX             30      
#define CATB_SENSORS_MSB            31      
#define CATB_STATUS_REG_NUMBER      1       

#endif /* _SAM4L_CATB_INSTANCE_ */
