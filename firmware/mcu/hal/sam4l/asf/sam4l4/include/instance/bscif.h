/**
 * \file
 *
 * \brief Instance description for BSCIF
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

#ifndef _SAM4L_BSCIF_INSTANCE_
#define _SAM4L_BSCIF_INSTANCE_

/* ========== Register definition for BSCIF peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_BSCIF_IER              (0x400F0400U) /**< \brief (BSCIF) Interrupt Enable Register */
#define REG_BSCIF_IDR              (0x400F0404U) /**< \brief (BSCIF) Interrupt Disable Register */
#define REG_BSCIF_IMR              (0x400F0408U) /**< \brief (BSCIF) Interrupt Mask Register */
#define REG_BSCIF_ISR              (0x400F040CU) /**< \brief (BSCIF) Interrupt Status Register */
#define REG_BSCIF_ICR              (0x400F0410U) /**< \brief (BSCIF) Interrupt Clear Register */
#define REG_BSCIF_PCLKSR           (0x400F0414U) /**< \brief (BSCIF) Power and Clocks Status Register */
#define REG_BSCIF_UNLOCK           (0x400F0418U) /**< \brief (BSCIF) Unlock Register */
#define REG_BSCIF_CSCR             (0x400F041CU) /**< \brief (BSCIF) Chip Specific Configuration Register */
#define REG_BSCIF_OSCCTRL32        (0x400F0420U) /**< \brief (BSCIF) Oscillator 32 Control Register */
#define REG_BSCIF_RC32KCR          (0x400F0424U) /**< \brief (BSCIF) 32 kHz RC Oscillator Control Register */
#define REG_BSCIF_RC32KTUNE        (0x400F0428U) /**< \brief (BSCIF) 32kHz RC Oscillator Tuning Register */
#define REG_BSCIF_BOD33CTRL        (0x400F042CU) /**< \brief (BSCIF) BOD33 Control Register */
#define REG_BSCIF_BOD33LEVEL       (0x400F0430U) /**< \brief (BSCIF) BOD33 Level Register */
#define REG_BSCIF_BOD33SAMPLING    (0x400F0434U) /**< \brief (BSCIF) BOD33 Sampling Control Register */
#define REG_BSCIF_BOD18CTRL        (0x400F0438U) /**< \brief (BSCIF) BOD18 Control Register */
#define REG_BSCIF_BOD18LEVEL       (0x400F043CU) /**< \brief (BSCIF) BOD18 Level Register */
#define REG_BSCIF_VREGCR           (0x400F0444U) /**< \brief (BSCIF) Voltage Regulator Configuration Register */
#define REG_BSCIF_VREGNCSR         (0x400F044CU) /**< \brief (BSCIF) Normal Mode Control and Status Register */
#define REG_BSCIF_VREGLPCSR        (0x400F0450U) /**< \brief (BSCIF) LP Mode Control and Status Register */
#define REG_BSCIF_RC1MCR           (0x400F0458U) /**< \brief (BSCIF) 1MHz RC Clock Configuration Register */
#define REG_BSCIF_BGCR             (0x400F045CU) /**< \brief (BSCIF) Bandgap Calibration Register */
#define REG_BSCIF_BGCTRL           (0x400F0460U) /**< \brief (BSCIF) Bandgap Control Register */
#define REG_BSCIF_BGSR             (0x400F0464U) /**< \brief (BSCIF) Bandgap Status Register */
#define REG_BSCIF_BR0              (0x400F0478U) /**< \brief (BSCIF) Backup Register 0 */
#define REG_BSCIF_BR1              (0x400F047CU) /**< \brief (BSCIF) Backup Register 1 */
#define REG_BSCIF_BR2              (0x400F0480U) /**< \brief (BSCIF) Backup Register 2 */
#define REG_BSCIF_BR3              (0x400F0484U) /**< \brief (BSCIF) Backup Register 3 */
#define REG_BSCIF_BRIFBVERSION     (0x400F07E4U) /**< \brief (BSCIF) Backup Register Interface Version Register */
#define REG_BSCIF_BGREFIFBVERSION  (0x400F07E8U) /**< \brief (BSCIF) BGREFIFB Version Register */
#define REG_BSCIF_VREGIFGVERSION   (0x400F07ECU) /**< \brief (BSCIF) VREGIFA Version Register */
#define REG_BSCIF_BODIFCVERSION    (0x400F07F0U) /**< \brief (BSCIF) BODIFC Version Register */
#define REG_BSCIF_RC32KIFBVERSION  (0x400F07F4U) /**< \brief (BSCIF) 32 kHz RC Oscillator Version Register */
#define REG_BSCIF_OSC32IFAVERSION  (0x400F07F8U) /**< \brief (BSCIF) 32 KHz Oscillator Version Register */
#define REG_BSCIF_VERSION          (0x400F07FCU) /**< \brief (BSCIF) BSCIF Version Register */
#else
#define REG_BSCIF_IER              (*(WoReg  *)0x400F0400U) /**< \brief (BSCIF) Interrupt Enable Register */
#define REG_BSCIF_IDR              (*(WoReg  *)0x400F0404U) /**< \brief (BSCIF) Interrupt Disable Register */
#define REG_BSCIF_IMR              (*(RoReg  *)0x400F0408U) /**< \brief (BSCIF) Interrupt Mask Register */
#define REG_BSCIF_ISR              (*(RoReg  *)0x400F040CU) /**< \brief (BSCIF) Interrupt Status Register */
#define REG_BSCIF_ICR              (*(WoReg  *)0x400F0410U) /**< \brief (BSCIF) Interrupt Clear Register */
#define REG_BSCIF_PCLKSR           (*(RoReg  *)0x400F0414U) /**< \brief (BSCIF) Power and Clocks Status Register */
#define REG_BSCIF_UNLOCK           (*(WoReg  *)0x400F0418U) /**< \brief (BSCIF) Unlock Register */
#define REG_BSCIF_CSCR             (*(RwReg  *)0x400F041CU) /**< \brief (BSCIF) Chip Specific Configuration Register */
#define REG_BSCIF_OSCCTRL32        (*(RwReg  *)0x400F0420U) /**< \brief (BSCIF) Oscillator 32 Control Register */
#define REG_BSCIF_RC32KCR          (*(RwReg  *)0x400F0424U) /**< \brief (BSCIF) 32 kHz RC Oscillator Control Register */
#define REG_BSCIF_RC32KTUNE        (*(RwReg  *)0x400F0428U) /**< \brief (BSCIF) 32kHz RC Oscillator Tuning Register */
#define REG_BSCIF_BOD33CTRL        (*(RwReg  *)0x400F042CU) /**< \brief (BSCIF) BOD33 Control Register */
#define REG_BSCIF_BOD33LEVEL       (*(RwReg  *)0x400F0430U) /**< \brief (BSCIF) BOD33 Level Register */
#define REG_BSCIF_BOD33SAMPLING    (*(RwReg  *)0x400F0434U) /**< \brief (BSCIF) BOD33 Sampling Control Register */
#define REG_BSCIF_BOD18CTRL        (*(RwReg  *)0x400F0438U) /**< \brief (BSCIF) BOD18 Control Register */
#define REG_BSCIF_BOD18LEVEL       (*(RwReg  *)0x400F043CU) /**< \brief (BSCIF) BOD18 Level Register */
#define REG_BSCIF_VREGCR           (*(RwReg  *)0x400F0444U) /**< \brief (BSCIF) Voltage Regulator Configuration Register */
#define REG_BSCIF_VREGNCSR         (*(RwReg  *)0x400F044CU) /**< \brief (BSCIF) Normal Mode Control and Status Register */
#define REG_BSCIF_VREGLPCSR        (*(RwReg  *)0x400F0450U) /**< \brief (BSCIF) LP Mode Control and Status Register */
#define REG_BSCIF_RC1MCR           (*(RwReg  *)0x400F0458U) /**< \brief (BSCIF) 1MHz RC Clock Configuration Register */
#define REG_BSCIF_BGCR             (*(RwReg  *)0x400F045CU) /**< \brief (BSCIF) Bandgap Calibration Register */
#define REG_BSCIF_BGCTRL           (*(RwReg  *)0x400F0460U) /**< \brief (BSCIF) Bandgap Control Register */
#define REG_BSCIF_BGSR             (*(RoReg  *)0x400F0464U) /**< \brief (BSCIF) Bandgap Status Register */
#define REG_BSCIF_BR0              (*(RwReg  *)0x400F0478U) /**< \brief (BSCIF) Backup Register 0 */
#define REG_BSCIF_BR1              (*(RwReg  *)0x400F047CU) /**< \brief (BSCIF) Backup Register 1 */
#define REG_BSCIF_BR2              (*(RwReg  *)0x400F0480U) /**< \brief (BSCIF) Backup Register 2 */
#define REG_BSCIF_BR3              (*(RwReg  *)0x400F0484U) /**< \brief (BSCIF) Backup Register 3 */
#define REG_BSCIF_BRIFBVERSION     (*(RoReg  *)0x400F07E4U) /**< \brief (BSCIF) Backup Register Interface Version Register */
#define REG_BSCIF_BGREFIFBVERSION  (*(RoReg  *)0x400F07E8U) /**< \brief (BSCIF) BGREFIFB Version Register */
#define REG_BSCIF_VREGIFGVERSION   (*(RoReg  *)0x400F07ECU) /**< \brief (BSCIF) VREGIFA Version Register */
#define REG_BSCIF_BODIFCVERSION    (*(RoReg  *)0x400F07F0U) /**< \brief (BSCIF) BODIFC Version Register */
#define REG_BSCIF_RC32KIFBVERSION  (*(RoReg  *)0x400F07F4U) /**< \brief (BSCIF) 32 kHz RC Oscillator Version Register */
#define REG_BSCIF_OSC32IFAVERSION  (*(RoReg  *)0x400F07F8U) /**< \brief (BSCIF) 32 KHz Oscillator Version Register */
#define REG_BSCIF_VERSION          (*(RoReg  *)0x400F07FCU) /**< \brief (BSCIF) BSCIF Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for BSCIF peripheral ========== */
#define BSCIF_BGBUF_NUM             6       
#define BSCIF_BOD18_IMPLEMENTED     0       
#define BSCIF_BOD33_IMPLEMENTED     0       
#define BSCIF_BOD50_IMPLEMENTED     0       
#define BSCIF_BR_NUM                4       
#define BSCIF_BOD_OFF               0       
#define BSCIF_BOD_ON                1       
#define BSCIF_BOD_ON_NORESET        2       
#define BSCIF_GC_DIV_CLOCK          1       
#define BSCIF_GC_NO_DIV_CLOCK       0       

#endif /* _SAM4L_BSCIF_INSTANCE_ */
