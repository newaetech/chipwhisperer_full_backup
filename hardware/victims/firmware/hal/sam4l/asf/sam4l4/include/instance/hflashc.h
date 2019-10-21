/**
 * \file
 *
 * \brief Instance description for HFLASHC
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

#ifndef _SAM4L_HFLASHC_INSTANCE_
#define _SAM4L_HFLASHC_INSTANCE_

/* ========== Register definition for HFLASHC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_HFLASHC_FCR            (0x400A0000U) /**< \brief (HFLASHC) Flash Controller Control Register */
#define REG_HFLASHC_FCMD           (0x400A0004U) /**< \brief (HFLASHC) Flash Controller Command Register */
#define REG_HFLASHC_FSR            (0x400A0008U) /**< \brief (HFLASHC) Flash Controller Status Register */
#define REG_HFLASHC_FPR            (0x400A000CU) /**< \brief (HFLASHC) Flash Controller Parameter Register */
#define REG_HFLASHC_VERSION        (0x400A0010U) /**< \brief (HFLASHC) Flash Controller Version Register */
#define REG_HFLASHC_FGPFRHI        (0x400A0014U) /**< \brief (HFLASHC) Flash Controller General Purpose Fuse Register High */
#define REG_HFLASHC_FGPFRLO        (0x400A0018U) /**< \brief (HFLASHC) Flash Controller General Purpose Fuse Register Low */
#else
#define REG_HFLASHC_FCR            (*(RwReg  *)0x400A0000U) /**< \brief (HFLASHC) Flash Controller Control Register */
#define REG_HFLASHC_FCMD           (*(RwReg  *)0x400A0004U) /**< \brief (HFLASHC) Flash Controller Command Register */
#define REG_HFLASHC_FSR            (*(RwReg  *)0x400A0008U) /**< \brief (HFLASHC) Flash Controller Status Register */
#define REG_HFLASHC_FPR            (*(RoReg  *)0x400A000CU) /**< \brief (HFLASHC) Flash Controller Parameter Register */
#define REG_HFLASHC_VERSION        (*(RoReg  *)0x400A0010U) /**< \brief (HFLASHC) Flash Controller Version Register */
#define REG_HFLASHC_FGPFRHI        (*(RwReg  *)0x400A0014U) /**< \brief (HFLASHC) Flash Controller General Purpose Fuse Register High */
#define REG_HFLASHC_FGPFRLO        (*(RwReg  *)0x400A0018U) /**< \brief (HFLASHC) Flash Controller General Purpose Fuse Register Low */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAM4L_HFLASHC_INSTANCE_ */
