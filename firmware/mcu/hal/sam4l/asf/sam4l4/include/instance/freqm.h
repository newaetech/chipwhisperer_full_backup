/**
 * \file
 *
 * \brief Instance description for FREQM
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

#ifndef _SAM4L_FREQM_INSTANCE_
#define _SAM4L_FREQM_INSTANCE_

/* ========== Register definition for FREQM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_FREQM_CTRL             (0x400E0C00U) /**< \brief (FREQM) Control register */
#define REG_FREQM_MODE             (0x400E0C04U) /**< \brief (FREQM) Mode  register */
#define REG_FREQM_STATUS           (0x400E0C08U) /**< \brief (FREQM) Status  register */
#define REG_FREQM_VALUE            (0x400E0C0CU) /**< \brief (FREQM) Value register */
#define REG_FREQM_IER              (0x400E0C10U) /**< \brief (FREQM) Interrupt Enable Register */
#define REG_FREQM_IDR              (0x400E0C14U) /**< \brief (FREQM) Interrupt Diable Register */
#define REG_FREQM_IMR              (0x400E0C18U) /**< \brief (FREQM) Interrupt Mask Register */
#define REG_FREQM_ISR              (0x400E0C1CU) /**< \brief (FREQM) Interrupt Status Register */
#define REG_FREQM_ICR              (0x400E0C20U) /**< \brief (FREQM) Interrupt Clear Register */
#define REG_FREQM_VERSION          (0x400E0FFCU) /**< \brief (FREQM) Version Register */
#else
#define REG_FREQM_CTRL             (*(WoReg  *)0x400E0C00U) /**< \brief (FREQM) Control register */
#define REG_FREQM_MODE             (*(RwReg  *)0x400E0C04U) /**< \brief (FREQM) Mode  register */
#define REG_FREQM_STATUS           (*(RoReg  *)0x400E0C08U) /**< \brief (FREQM) Status  register */
#define REG_FREQM_VALUE            (*(RoReg  *)0x400E0C0CU) /**< \brief (FREQM) Value register */
#define REG_FREQM_IER              (*(WoReg  *)0x400E0C10U) /**< \brief (FREQM) Interrupt Enable Register */
#define REG_FREQM_IDR              (*(WoReg  *)0x400E0C14U) /**< \brief (FREQM) Interrupt Diable Register */
#define REG_FREQM_IMR              (*(RoReg  *)0x400E0C18U) /**< \brief (FREQM) Interrupt Mask Register */
#define REG_FREQM_ISR              (*(RoReg  *)0x400E0C1CU) /**< \brief (FREQM) Interrupt Status Register */
#define REG_FREQM_ICR              (*(WoReg  *)0x400E0C20U) /**< \brief (FREQM) Interrupt Clear Register */
#define REG_FREQM_VERSION          (*(RoReg  *)0x400E0FFCU) /**< \brief (FREQM) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for FREQM peripheral ========== */
#define FREQM_CPU                   0       
#define FREQM_CRIPOSC               10      
#define FREQM_DFLL0                 9       
#define FREQM_FLO_JITTER            27      
#define FREQM_GENCLK0               11      
#define FREQM_GENCLK1               12      
#define FREQM_GENCLK2               13      
#define FREQM_GENCLK3               14      
#define FREQM_GENCLK4               15      
#define FREQM_GENCLK5               16      
#define FREQM_GENCLK6               17      
#define FREQM_GENCLK7               18      
#define FREQM_GENCLK8               19      
#define FREQM_GENCLK9               20      
#define FREQM_GENCLK10              21      
#define FREQM_HSB                   1       
#define FREQM_NUM_CLK               28      
#define FREQM_NUM_REF_CLK           4       
#define FREQM_OSC0                  6       
#define FREQM_OSC32                 7       
#define FREQM_PBA                   2       
#define FREQM_PBB                   3       
#define FREQM_PBC                   4       
#define FREQM_PLL0                  26      
#define FREQM_RCFAST                24      
#define FREQM_RCOSC                 8       
#define FREQM_RC1M                  25      
#define FREQM_RC32K                 7       
#define FREQM_RC80M                 23      
#define FREQM_REFSEL_BITS           2       
#define FREQM_REF_OSC32             1       
#define FREQM_REF_RCOSC             0       

#endif /* _SAM4L_FREQM_INSTANCE_ */
