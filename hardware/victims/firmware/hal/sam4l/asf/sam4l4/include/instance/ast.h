/**
 * \file
 *
 * \brief Instance description for AST
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

#ifndef _SAM4L_AST_INSTANCE_
#define _SAM4L_AST_INSTANCE_

/* ========== Register definition for AST peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_AST_CR                 (0x400F0800U) /**< \brief (AST) Control Register */
#define REG_AST_CV                 (0x400F0804U) /**< \brief (AST) Counter Value */
#define REG_AST_SR                 (0x400F0808U) /**< \brief (AST) Status Register */
#define REG_AST_SCR                (0x400F080CU) /**< \brief (AST) Status Clear Register */
#define REG_AST_IER                (0x400F0810U) /**< \brief (AST) Interrupt Enable Register */
#define REG_AST_IDR                (0x400F0814U) /**< \brief (AST) Interrupt Disable Register */
#define REG_AST_IMR                (0x400F0818U) /**< \brief (AST) Interrupt Mask Register */
#define REG_AST_WER                (0x400F081CU) /**< \brief (AST) Wake Enable Register */
#define REG_AST_AR0                (0x400F0820U) /**< \brief (AST) Alarm Register 0 */
#define REG_AST_AR1                (0x400F0824U) /**< \brief (AST) Alarm Register 1 */
#define REG_AST_PIR0               (0x400F0830U) /**< \brief (AST) Periodic Interval Register 0 */
#define REG_AST_PIR1               (0x400F0834U) /**< \brief (AST) Periodic Interval Register 1 */
#define REG_AST_CLOCK              (0x400F0840U) /**< \brief (AST) Clock Control Register */
#define REG_AST_DTR                (0x400F0844U) /**< \brief (AST) Digital Tuner Register */
#define REG_AST_EVE                (0x400F0848U) /**< \brief (AST) Event Enable Register */
#define REG_AST_EVD                (0x400F084CU) /**< \brief (AST) Event Disable Register */
#define REG_AST_EVM                (0x400F0850U) /**< \brief (AST) Event Mask Register */
#define REG_AST_CALV               (0x400F0854U) /**< \brief (AST) Calendar Value */
#define REG_AST_PARAMETER          (0x400F08F0U) /**< \brief (AST) Parameter Register */
#define REG_AST_VERSION            (0x400F08FCU) /**< \brief (AST) Version Register */
#else
#define REG_AST_CR                 (*(RwReg  *)0x400F0800U) /**< \brief (AST) Control Register */
#define REG_AST_CV                 (*(RwReg  *)0x400F0804U) /**< \brief (AST) Counter Value */
#define REG_AST_SR                 (*(RoReg  *)0x400F0808U) /**< \brief (AST) Status Register */
#define REG_AST_SCR                (*(WoReg  *)0x400F080CU) /**< \brief (AST) Status Clear Register */
#define REG_AST_IER                (*(WoReg  *)0x400F0810U) /**< \brief (AST) Interrupt Enable Register */
#define REG_AST_IDR                (*(WoReg  *)0x400F0814U) /**< \brief (AST) Interrupt Disable Register */
#define REG_AST_IMR                (*(RoReg  *)0x400F0818U) /**< \brief (AST) Interrupt Mask Register */
#define REG_AST_WER                (*(RwReg  *)0x400F081CU) /**< \brief (AST) Wake Enable Register */
#define REG_AST_AR0                (*(RwReg  *)0x400F0820U) /**< \brief (AST) Alarm Register 0 */
#define REG_AST_AR1                (*(RwReg  *)0x400F0824U) /**< \brief (AST) Alarm Register 1 */
#define REG_AST_PIR0               (*(RwReg  *)0x400F0830U) /**< \brief (AST) Periodic Interval Register 0 */
#define REG_AST_PIR1               (*(RwReg  *)0x400F0834U) /**< \brief (AST) Periodic Interval Register 1 */
#define REG_AST_CLOCK              (*(RwReg  *)0x400F0840U) /**< \brief (AST) Clock Control Register */
#define REG_AST_DTR                (*(RwReg  *)0x400F0844U) /**< \brief (AST) Digital Tuner Register */
#define REG_AST_EVE                (*(WoReg  *)0x400F0848U) /**< \brief (AST) Event Enable Register */
#define REG_AST_EVD                (*(WoReg  *)0x400F084CU) /**< \brief (AST) Event Disable Register */
#define REG_AST_EVM                (*(RoReg  *)0x400F0850U) /**< \brief (AST) Event Mask Register */
#define REG_AST_CALV               (*(RwReg  *)0x400F0854U) /**< \brief (AST) Calendar Value */
#define REG_AST_PARAMETER          (*(RoReg  *)0x400F08F0U) /**< \brief (AST) Parameter Register */
#define REG_AST_VERSION            (*(RoReg  *)0x400F08FCU) /**< \brief (AST) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for AST peripheral ========== */
#define AST_CLK1K                   4       
#define AST_CLK32                   1       
#define AST_GCLK_NUM                2       
#define AST_GENCLK                  3       
#define AST_PB                      2       
#define AST_RCOSC                   0       

#endif /* _SAM4L_AST_INSTANCE_ */
