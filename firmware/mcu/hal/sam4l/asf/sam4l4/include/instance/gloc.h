/**
 * \file
 *
 * \brief Instance description for GLOC
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

#ifndef _SAM4L_GLOC_INSTANCE_
#define _SAM4L_GLOC_INSTANCE_

/* ========== Register definition for GLOC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_GLOC_CR0               (0x40060000U) /**< \brief (GLOC) Control Register 0 */
#define REG_GLOC_TRUTH0            (0x40060004U) /**< \brief (GLOC) Truth Register 0 */
#define REG_GLOC_CR1               (0x40060008U) /**< \brief (GLOC) Control Register 1 */
#define REG_GLOC_TRUTH1            (0x4006000CU) /**< \brief (GLOC) Truth Register 1 */
#define REG_GLOC_PARAMETER         (0x40060038U) /**< \brief (GLOC) Parameter Register */
#define REG_GLOC_VERSION           (0x4006003CU) /**< \brief (GLOC) Version Register */
#else
#define REG_GLOC_CR0               (*(RwReg  *)0x40060000U) /**< \brief (GLOC) Control Register 0 */
#define REG_GLOC_TRUTH0            (*(RwReg  *)0x40060004U) /**< \brief (GLOC) Truth Register 0 */
#define REG_GLOC_CR1               (*(RwReg  *)0x40060008U) /**< \brief (GLOC) Control Register 1 */
#define REG_GLOC_TRUTH1            (*(RwReg  *)0x4006000CU) /**< \brief (GLOC) Truth Register 1 */
#define REG_GLOC_PARAMETER         (*(RoReg  *)0x40060038U) /**< \brief (GLOC) Parameter Register */
#define REG_GLOC_VERSION           (*(RoReg  *)0x4006003CU) /**< \brief (GLOC) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for GLOC peripheral ========== */
#define GLOC_GCLK_NUM               5       
#define GLOC_LUTS                   2       

#endif /* _SAM4L_GLOC_INSTANCE_ */
