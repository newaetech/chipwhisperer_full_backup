/**
 * \file
 *
 * \brief Instance description for ABDACB
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

#ifndef _SAM4L_ABDACB_INSTANCE_
#define _SAM4L_ABDACB_INSTANCE_

/* ========== Register definition for ABDACB peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ABDACB_CR              (0x40064000U) /**< \brief (ABDACB) Control Register */
#define REG_ABDACB_SDR0            (0x40064004U) /**< \brief (ABDACB) Sample Data Register 0 */
#define REG_ABDACB_SDR1            (0x40064008U) /**< \brief (ABDACB) Sample Data Register 1 */
#define REG_ABDACB_VCR0            (0x4006400CU) /**< \brief (ABDACB) Volume Control Register 0 */
#define REG_ABDACB_VCR1            (0x40064010U) /**< \brief (ABDACB) Volume Control Register 1 */
#define REG_ABDACB_IER             (0x40064014U) /**< \brief (ABDACB) Interrupt Enable Register */
#define REG_ABDACB_IDR             (0x40064018U) /**< \brief (ABDACB) Interupt Disable Register */
#define REG_ABDACB_IMR             (0x4006401CU) /**< \brief (ABDACB) Interrupt Mask Register */
#define REG_ABDACB_SR              (0x40064020U) /**< \brief (ABDACB) Status Register */
#define REG_ABDACB_SCR             (0x40064024U) /**< \brief (ABDACB) Status Clear Register */
#define REG_ABDACB_PARAMETER       (0x40064028U) /**< \brief (ABDACB) Parameter Register */
#define REG_ABDACB_VERSION         (0x4006402CU) /**< \brief (ABDACB) Version Register */
#else
#define REG_ABDACB_CR              (*(RwReg  *)0x40064000U) /**< \brief (ABDACB) Control Register */
#define REG_ABDACB_SDR0            (*(RwReg  *)0x40064004U) /**< \brief (ABDACB) Sample Data Register 0 */
#define REG_ABDACB_SDR1            (*(RwReg  *)0x40064008U) /**< \brief (ABDACB) Sample Data Register 1 */
#define REG_ABDACB_VCR0            (*(RwReg  *)0x4006400CU) /**< \brief (ABDACB) Volume Control Register 0 */
#define REG_ABDACB_VCR1            (*(RwReg  *)0x40064010U) /**< \brief (ABDACB) Volume Control Register 1 */
#define REG_ABDACB_IER             (*(WoReg  *)0x40064014U) /**< \brief (ABDACB) Interrupt Enable Register */
#define REG_ABDACB_IDR             (*(WoReg  *)0x40064018U) /**< \brief (ABDACB) Interupt Disable Register */
#define REG_ABDACB_IMR             (*(RoReg  *)0x4006401CU) /**< \brief (ABDACB) Interrupt Mask Register */
#define REG_ABDACB_SR              (*(RoReg  *)0x40064020U) /**< \brief (ABDACB) Status Register */
#define REG_ABDACB_SCR             (*(WoReg  *)0x40064024U) /**< \brief (ABDACB) Status Clear Register */
#define REG_ABDACB_PARAMETER       (*(RoReg  *)0x40064028U) /**< \brief (ABDACB) Parameter Register */
#define REG_ABDACB_VERSION         (*(RoReg  *)0x4006402CU) /**< \brief (ABDACB) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for ABDACB peripheral ========== */
#define ABDACB_GCLK_NUM             6       
#define ABDACB_PDCA_ID_TX_CH0       31      
#define ABDACB_PDCA_ID_TX_CH1       32      

#endif /* _SAM4L_ABDACB_INSTANCE_ */
