/**
 * \file
 *
 * \brief Instance description for IISC
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

#ifndef _SAM4L_IISC_INSTANCE_
#define _SAM4L_IISC_INSTANCE_

/* ========== Register definition for IISC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_IISC_CR                (0x40004000U) /**< \brief (IISC) Control Register */
#define REG_IISC_MR                (0x40004004U) /**< \brief (IISC) Mode Register */
#define REG_IISC_SR                (0x40004008U) /**< \brief (IISC) Status Register */
#define REG_IISC_SCR               (0x4000400CU) /**< \brief (IISC) Status Clear Register */
#define REG_IISC_SSR               (0x40004010U) /**< \brief (IISC) Status Set Register */
#define REG_IISC_IER               (0x40004014U) /**< \brief (IISC) Interrupt Enable Register */
#define REG_IISC_IDR               (0x40004018U) /**< \brief (IISC) Interrupt Disable Register */
#define REG_IISC_IMR               (0x4000401CU) /**< \brief (IISC) Interrupt Mask Register */
#define REG_IISC_RHR               (0x40004020U) /**< \brief (IISC) Receive Holding Register */
#define REG_IISC_THR               (0x40004024U) /**< \brief (IISC) Transmit Holding Register */
#define REG_IISC_VERSION           (0x40004028U) /**< \brief (IISC) Version Register */
#define REG_IISC_PARAMETER         (0x4000402CU) /**< \brief (IISC) Parameter Register */
#else
#define REG_IISC_CR                (*(WoReg  *)0x40004000U) /**< \brief (IISC) Control Register */
#define REG_IISC_MR                (*(RwReg  *)0x40004004U) /**< \brief (IISC) Mode Register */
#define REG_IISC_SR                (*(RoReg  *)0x40004008U) /**< \brief (IISC) Status Register */
#define REG_IISC_SCR               (*(WoReg  *)0x4000400CU) /**< \brief (IISC) Status Clear Register */
#define REG_IISC_SSR               (*(WoReg  *)0x40004010U) /**< \brief (IISC) Status Set Register */
#define REG_IISC_IER               (*(WoReg  *)0x40004014U) /**< \brief (IISC) Interrupt Enable Register */
#define REG_IISC_IDR               (*(WoReg  *)0x40004018U) /**< \brief (IISC) Interrupt Disable Register */
#define REG_IISC_IMR               (*(RoReg  *)0x4000401CU) /**< \brief (IISC) Interrupt Mask Register */
#define REG_IISC_RHR               (*(RoReg  *)0x40004020U) /**< \brief (IISC) Receive Holding Register */
#define REG_IISC_THR               (*(WoReg  *)0x40004024U) /**< \brief (IISC) Transmit Holding Register */
#define REG_IISC_VERSION           (*(RoReg  *)0x40004028U) /**< \brief (IISC) Version Register */
#define REG_IISC_PARAMETER         (*(RoReg  *)0x4000402CU) /**< \brief (IISC) Parameter Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for IISC peripheral ========== */
#define IISC_GCLK_NUM               6       
#define IISC_PDCA_ID_RX             14      
#define IISC_PDCA_ID_RX_1           15      
#define IISC_PDCA_ID_TX             33      
#define IISC_PDCA_ID_TX_1           34      

#endif /* _SAM4L_IISC_INSTANCE_ */
