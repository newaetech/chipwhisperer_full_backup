/**
 * \file
 *
 * \brief Instance description for ACIFC
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

#ifndef _SAM4L_ACIFC_INSTANCE_
#define _SAM4L_ACIFC_INSTANCE_

/* ========== Register definition for ACIFC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_ACIFC_CTRL             (0x40040000U) /**< \brief (ACIFC) Control Register */
#define REG_ACIFC_SR               (0x40040004U) /**< \brief (ACIFC) Status Register */
#define REG_ACIFC_IER              (0x40040010U) /**< \brief (ACIFC) Interrupt Enable Register */
#define REG_ACIFC_IDR              (0x40040014U) /**< \brief (ACIFC) Interrupt Disable Register */
#define REG_ACIFC_IMR              (0x40040018U) /**< \brief (ACIFC) Interrupt Mask Register */
#define REG_ACIFC_ISR              (0x4004001CU) /**< \brief (ACIFC) Interrupt Status Register */
#define REG_ACIFC_ICR              (0x40040020U) /**< \brief (ACIFC) Interrupt Status Clear Register */
#define REG_ACIFC_TR               (0x40040024U) /**< \brief (ACIFC) Test Register */
#define REG_ACIFC_PARAMETER        (0x40040030U) /**< \brief (ACIFC) Parameter Register */
#define REG_ACIFC_VERSION          (0x40040034U) /**< \brief (ACIFC) Version Register */
#define REG_ACIFC_CONFW0           (0x40040080U) /**< \brief (ACIFC) Window configuration Register 0 */
#define REG_ACIFC_CONFW1           (0x40040084U) /**< \brief (ACIFC) Window configuration Register 1 */
#define REG_ACIFC_CONFW2           (0x40040088U) /**< \brief (ACIFC) Window configuration Register 2 */
#define REG_ACIFC_CONFW3           (0x4004008CU) /**< \brief (ACIFC) Window configuration Register 3 */
#define REG_ACIFC_CONF0            (0x400400D0U) /**< \brief (ACIFC) AC Configuration Register 0 */
#define REG_ACIFC_CONF1            (0x400400D4U) /**< \brief (ACIFC) AC Configuration Register 1 */
#define REG_ACIFC_CONF2            (0x400400D8U) /**< \brief (ACIFC) AC Configuration Register 2 */
#define REG_ACIFC_CONF3            (0x400400DCU) /**< \brief (ACIFC) AC Configuration Register 3 */
#define REG_ACIFC_CONF4            (0x400400E0U) /**< \brief (ACIFC) AC Configuration Register 4 */
#define REG_ACIFC_CONF5            (0x400400E4U) /**< \brief (ACIFC) AC Configuration Register 5 */
#define REG_ACIFC_CONF6            (0x400400E8U) /**< \brief (ACIFC) AC Configuration Register 6 */
#define REG_ACIFC_CONF7            (0x400400ECU) /**< \brief (ACIFC) AC Configuration Register 7 */
#else
#define REG_ACIFC_CTRL             (*(RwReg  *)0x40040000U) /**< \brief (ACIFC) Control Register */
#define REG_ACIFC_SR               (*(RoReg  *)0x40040004U) /**< \brief (ACIFC) Status Register */
#define REG_ACIFC_IER              (*(WoReg  *)0x40040010U) /**< \brief (ACIFC) Interrupt Enable Register */
#define REG_ACIFC_IDR              (*(WoReg  *)0x40040014U) /**< \brief (ACIFC) Interrupt Disable Register */
#define REG_ACIFC_IMR              (*(RoReg  *)0x40040018U) /**< \brief (ACIFC) Interrupt Mask Register */
#define REG_ACIFC_ISR              (*(RoReg  *)0x4004001CU) /**< \brief (ACIFC) Interrupt Status Register */
#define REG_ACIFC_ICR              (*(WoReg  *)0x40040020U) /**< \brief (ACIFC) Interrupt Status Clear Register */
#define REG_ACIFC_TR               (*(RwReg  *)0x40040024U) /**< \brief (ACIFC) Test Register */
#define REG_ACIFC_PARAMETER        (*(RoReg  *)0x40040030U) /**< \brief (ACIFC) Parameter Register */
#define REG_ACIFC_VERSION          (*(RoReg  *)0x40040034U) /**< \brief (ACIFC) Version Register */
#define REG_ACIFC_CONFW0           (*(RwReg  *)0x40040080U) /**< \brief (ACIFC) Window configuration Register 0 */
#define REG_ACIFC_CONFW1           (*(RwReg  *)0x40040084U) /**< \brief (ACIFC) Window configuration Register 1 */
#define REG_ACIFC_CONFW2           (*(RwReg  *)0x40040088U) /**< \brief (ACIFC) Window configuration Register 2 */
#define REG_ACIFC_CONFW3           (*(RwReg  *)0x4004008CU) /**< \brief (ACIFC) Window configuration Register 3 */
#define REG_ACIFC_CONF0            (*(RwReg  *)0x400400D0U) /**< \brief (ACIFC) AC Configuration Register 0 */
#define REG_ACIFC_CONF1            (*(RwReg  *)0x400400D4U) /**< \brief (ACIFC) AC Configuration Register 1 */
#define REG_ACIFC_CONF2            (*(RwReg  *)0x400400D8U) /**< \brief (ACIFC) AC Configuration Register 2 */
#define REG_ACIFC_CONF3            (*(RwReg  *)0x400400DCU) /**< \brief (ACIFC) AC Configuration Register 3 */
#define REG_ACIFC_CONF4            (*(RwReg  *)0x400400E0U) /**< \brief (ACIFC) AC Configuration Register 4 */
#define REG_ACIFC_CONF5            (*(RwReg  *)0x400400E4U) /**< \brief (ACIFC) AC Configuration Register 5 */
#define REG_ACIFC_CONF6            (*(RwReg  *)0x400400E8U) /**< \brief (ACIFC) AC Configuration Register 6 */
#define REG_ACIFC_CONF7            (*(RwReg  *)0x400400ECU) /**< \brief (ACIFC) AC Configuration Register 7 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAM4L_ACIFC_INSTANCE_ */
