/**
 * \file
 *
 * \brief Instance description for BPM
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

#ifndef _SAM4L_BPM_INSTANCE_
#define _SAM4L_BPM_INSTANCE_

/* ========== Register definition for BPM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_BPM_IER                (0x400F0000U) /**< \brief (BPM) Interrupt Enable Register */
#define REG_BPM_IDR                (0x400F0004U) /**< \brief (BPM) Interrupt Disable Register */
#define REG_BPM_IMR                (0x400F0008U) /**< \brief (BPM) Interrupt Mask Register */
#define REG_BPM_ISR                (0x400F000CU) /**< \brief (BPM) Interrupt Status Register */
#define REG_BPM_ICR                (0x400F0010U) /**< \brief (BPM) Interrupt Clear Register */
#define REG_BPM_SR                 (0x400F0014U) /**< \brief (BPM) Status Register */
#define REG_BPM_UNLOCK             (0x400F0018U) /**< \brief (BPM) Unlock Register */
#define REG_BPM_PMCON              (0x400F001CU) /**< \brief (BPM) Power Mode Control Register */
#define REG_BPM_BKUPWCAUSE         (0x400F0028U) /**< \brief (BPM) Backup Wake up Cause Register */
#define REG_BPM_BKUPWEN            (0x400F002CU) /**< \brief (BPM) Backup Wake up Enable Register */
#define REG_BPM_BKUPPMUX           (0x400F0030U) /**< \brief (BPM) Backup Pin Muxing Register */
#define REG_BPM_IORET              (0x400F0034U) /**< \brief (BPM) Input Output Retention Register */
#define REG_BPM_BPR                (0x400F0040U) /**< \brief (BPM) Bypass Register */
#define REG_BPM_FWRUNPS            (0x400F0044U) /**< \brief (BPM) Factory Word Run PS Register */
#define REG_BPM_FWPSAVEPS          (0x400F0048U) /**< \brief (BPM) Factory Word Power Save PS Register */
#define REG_BPM_VERSION            (0x400F00FCU) /**< \brief (BPM) Version Register */
#else
#define REG_BPM_IER                (*(WoReg  *)0x400F0000U) /**< \brief (BPM) Interrupt Enable Register */
#define REG_BPM_IDR                (*(WoReg  *)0x400F0004U) /**< \brief (BPM) Interrupt Disable Register */
#define REG_BPM_IMR                (*(RoReg  *)0x400F0008U) /**< \brief (BPM) Interrupt Mask Register */
#define REG_BPM_ISR                (*(RoReg  *)0x400F000CU) /**< \brief (BPM) Interrupt Status Register */
#define REG_BPM_ICR                (*(WoReg  *)0x400F0010U) /**< \brief (BPM) Interrupt Clear Register */
#define REG_BPM_SR                 (*(RoReg  *)0x400F0014U) /**< \brief (BPM) Status Register */
#define REG_BPM_UNLOCK             (*(WoReg  *)0x400F0018U) /**< \brief (BPM) Unlock Register */
#define REG_BPM_PMCON              (*(RwReg  *)0x400F001CU) /**< \brief (BPM) Power Mode Control Register */
#define REG_BPM_BKUPWCAUSE         (*(RoReg  *)0x400F0028U) /**< \brief (BPM) Backup Wake up Cause Register */
#define REG_BPM_BKUPWEN            (*(RwReg  *)0x400F002CU) /**< \brief (BPM) Backup Wake up Enable Register */
#define REG_BPM_BKUPPMUX           (*(RwReg  *)0x400F0030U) /**< \brief (BPM) Backup Pin Muxing Register */
#define REG_BPM_IORET              (*(RwReg  *)0x400F0034U) /**< \brief (BPM) Input Output Retention Register */
#define REG_BPM_BPR                (*(RwReg  *)0x400F0040U) /**< \brief (BPM) Bypass Register */
#define REG_BPM_FWRUNPS            (*(RoReg  *)0x400F0044U) /**< \brief (BPM) Factory Word Run PS Register */
#define REG_BPM_FWPSAVEPS          (*(RoReg  *)0x400F0048U) /**< \brief (BPM) Factory Word Power Save PS Register */
#define REG_BPM_VERSION            (*(RoReg  *)0x400F00FCU) /**< \brief (BPM) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for BPM peripheral ========== */
#define BPM_BKUPPMUX_MSB            9       
#define BPM_BKUPWEN_AST             1       
#define BPM_BKUPWEN_BOD18           4       
#define BPM_BKUPWEN_BOD33           3       
#define BPM_BKUPWEN_EIC             0       
#define BPM_BKUPWEN_MSB             5       
#define BPM_BKUPWEN_PICOUART        5       
#define BPM_BKUPWEN_WDT             2       

#endif /* _SAM4L_BPM_INSTANCE_ */
