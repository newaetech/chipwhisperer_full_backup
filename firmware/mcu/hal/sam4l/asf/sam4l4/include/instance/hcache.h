/**
 * \file
 *
 * \brief Instance description for HCACHE
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

#ifndef _SAM4L_HCACHE_INSTANCE_
#define _SAM4L_HCACHE_INSTANCE_

/* ========== Register definition for HCACHE peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_HCACHE_CTRL            (0x400A0408U) /**< \brief (HCACHE) Control Register */
#define REG_HCACHE_SR              (0x400A040CU) /**< \brief (HCACHE) Status Register */
#define REG_HCACHE_MAINT0          (0x400A0420U) /**< \brief (HCACHE) Maintenance Register 0 */
#define REG_HCACHE_MAINT1          (0x400A0424U) /**< \brief (HCACHE) Maintenance Register 1 */
#define REG_HCACHE_MCFG            (0x400A0428U) /**< \brief (HCACHE) Monitor Configuration Register */
#define REG_HCACHE_MEN             (0x400A042CU) /**< \brief (HCACHE) Monitor Enable Register */
#define REG_HCACHE_MCTRL           (0x400A0430U) /**< \brief (HCACHE) Monitor Control Register */
#define REG_HCACHE_MSR             (0x400A0434U) /**< \brief (HCACHE) Monitor Status Register */
#define REG_HCACHE_VERSION         (0x400A04FCU) /**< \brief (HCACHE) Version Register */
#else
#define REG_HCACHE_CTRL            (*(WoReg  *)0x400A0408U) /**< \brief (HCACHE) Control Register */
#define REG_HCACHE_SR              (*(RwReg  *)0x400A040CU) /**< \brief (HCACHE) Status Register */
#define REG_HCACHE_MAINT0          (*(WoReg  *)0x400A0420U) /**< \brief (HCACHE) Maintenance Register 0 */
#define REG_HCACHE_MAINT1          (*(WoReg  *)0x400A0424U) /**< \brief (HCACHE) Maintenance Register 1 */
#define REG_HCACHE_MCFG            (*(RwReg  *)0x400A0428U) /**< \brief (HCACHE) Monitor Configuration Register */
#define REG_HCACHE_MEN             (*(RwReg  *)0x400A042CU) /**< \brief (HCACHE) Monitor Enable Register */
#define REG_HCACHE_MCTRL           (*(WoReg  *)0x400A0430U) /**< \brief (HCACHE) Monitor Control Register */
#define REG_HCACHE_MSR             (*(RoReg  *)0x400A0434U) /**< \brief (HCACHE) Monitor Status Register */
#define REG_HCACHE_VERSION         (*(RoReg  *)0x400A04FCU) /**< \brief (HCACHE) Version Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */


#endif /* _SAM4L_HCACHE_INSTANCE_ */
