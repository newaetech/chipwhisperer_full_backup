/**
 * \file
 *
 * \brief Instance description for ADCIFE
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

#ifndef _SAM4L_ADCIFE_INSTANCE_
#define _SAM4L_ADCIFE_INSTANCE_

/* ========== Register definition for ADCIFE peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ADCIFE_CR              (0x40038000U) /**< \brief (ADCIFE) Control Register */
#define REG_ADCIFE_CFG             (0x40038004U) /**< \brief (ADCIFE) Configuration Register */
#define REG_ADCIFE_SR              (0x40038008U) /**< \brief (ADCIFE) Status Register */
#define REG_ADCIFE_SCR             (0x4003800CU) /**< \brief (ADCIFE) Status Clear Register */
#define REG_ADCIFE_RTS             (0x40038010U) /**< \brief (ADCIFE) Resistive Touch Screen Register */
#define REG_ADCIFE_SEQCFG          (0x40038014U) /**< \brief (ADCIFE) Sequencer Configuration Register */
#define REG_ADCIFE_CDMA            (0x40038018U) /**< \brief (ADCIFE) Configuration Direct Memory Access Register */
#define REG_ADCIFE_TIM             (0x4003801CU) /**< \brief (ADCIFE) Timing Configuration Register */
#define REG_ADCIFE_ITIMER          (0x40038020U) /**< \brief (ADCIFE) Internal Timer Register */
#define REG_ADCIFE_WCFG            (0x40038024U) /**< \brief (ADCIFE) Window Monitor Configuration Register */
#define REG_ADCIFE_WTH             (0x40038028U) /**< \brief (ADCIFE) Window Monitor Threshold Configuration Register */
#define REG_ADCIFE_LCV             (0x4003802CU) /**< \brief (ADCIFE) Sequencer Last Converted Value Register */
#define REG_ADCIFE_IER             (0x40038030U) /**< \brief (ADCIFE) Interrupt Enable Register */
#define REG_ADCIFE_IDR             (0x40038034U) /**< \brief (ADCIFE) Interrupt Disable Register */
#define REG_ADCIFE_IMR             (0x40038038U) /**< \brief (ADCIFE) Interrupt Mask Register */
#define REG_ADCIFE_CALIB           (0x4003803CU) /**< \brief (ADCIFE) Calibration Register */
#define REG_ADCIFE_VERSION         (0x40038040U) /**< \brief (ADCIFE) Version Register */
#define REG_ADCIFE_PARAMETER       (0x40038044U) /**< \brief (ADCIFE) Parameter Register */
#else
#define REG_ADCIFE_CR              (*(WoReg  *)0x40038000U) /**< \brief (ADCIFE) Control Register */
#define REG_ADCIFE_CFG             (*(RwReg  *)0x40038004U) /**< \brief (ADCIFE) Configuration Register */
#define REG_ADCIFE_SR              (*(RoReg  *)0x40038008U) /**< \brief (ADCIFE) Status Register */
#define REG_ADCIFE_SCR             (*(WoReg  *)0x4003800CU) /**< \brief (ADCIFE) Status Clear Register */
#define REG_ADCIFE_RTS             (*(RwReg  *)0x40038010U) /**< \brief (ADCIFE) Resistive Touch Screen Register */
#define REG_ADCIFE_SEQCFG          (*(RwReg  *)0x40038014U) /**< \brief (ADCIFE) Sequencer Configuration Register */
#define REG_ADCIFE_CDMA            (*(WoReg  *)0x40038018U) /**< \brief (ADCIFE) Configuration Direct Memory Access Register */
#define REG_ADCIFE_TIM             (*(RwReg  *)0x4003801CU) /**< \brief (ADCIFE) Timing Configuration Register */
#define REG_ADCIFE_ITIMER          (*(RwReg  *)0x40038020U) /**< \brief (ADCIFE) Internal Timer Register */
#define REG_ADCIFE_WCFG            (*(RwReg  *)0x40038024U) /**< \brief (ADCIFE) Window Monitor Configuration Register */
#define REG_ADCIFE_WTH             (*(RwReg  *)0x40038028U) /**< \brief (ADCIFE) Window Monitor Threshold Configuration Register */
#define REG_ADCIFE_LCV             (*(RoReg  *)0x4003802CU) /**< \brief (ADCIFE) Sequencer Last Converted Value Register */
#define REG_ADCIFE_IER             (*(WoReg  *)0x40038030U) /**< \brief (ADCIFE) Interrupt Enable Register */
#define REG_ADCIFE_IDR             (*(WoReg  *)0x40038034U) /**< \brief (ADCIFE) Interrupt Disable Register */
#define REG_ADCIFE_IMR             (*(RoReg  *)0x40038038U) /**< \brief (ADCIFE) Interrupt Mask Register */
#define REG_ADCIFE_CALIB           (*(RwReg  *)0x4003803CU) /**< \brief (ADCIFE) Calibration Register */
#define REG_ADCIFE_VERSION         (*(RoReg  *)0x40038040U) /**< \brief (ADCIFE) Version Register */
#define REG_ADCIFE_PARAMETER       (*(RoReg  *)0x40038044U) /**< \brief (ADCIFE) Parameter Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for ADCIFE peripheral ========== */
#define ADCIFE_CHANNELS             16      
#define ADCIFE_CHANNEL_MSB          15      
#define ADCIFE_EXT_TRIGGERS_MSB     0       
#define ADCIFE_GCLK_NUM             10      
#define ADCIFE_PDCA_ID_RX           11      
#define ADCIFE_PDCA_ID_TX           29      
#define ADCIFE_REG_RX               LCV     
#define ADCIFE_REG_TX               CDMA    

#endif /* _SAM4L_ADCIFE_INSTANCE_ */
