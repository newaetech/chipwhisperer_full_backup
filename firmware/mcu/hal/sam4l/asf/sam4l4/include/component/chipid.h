/**
 * \file
 *
 * \brief Component description for CHIPID
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

#ifndef _SAM4L_CHIPID_COMPONENT_
#define _SAM4L_CHIPID_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR CHIPID */
/* ========================================================================== */
/** \addtogroup SAM4L_CHIPID Chip ID Registers */
/*@{*/

#define CHIPID_
#define REV_CHIPID                  0x100

/* -------- CHIPID_CIDR : (CHIPID Offset: 0x340) (R/  32) Chip ID Register -------- */
#define CHIPID_CIDR_OFFSET          0x340        /**< \brief (CHIPID_CIDR offset) Chip ID Register */
#define CHIPID_CIDR_RESETVALUE      0xAB0A09E0   /**< \brief (CHIPID_CIDR reset_value) Chip ID Register */
#define CHIPID_CIDR_MASK            0xFFFFFFFFu  /**< \brief (CHIPID_CIDR) MASK Register */

/* -------- CHIPID_EXID : (CHIPID Offset: 0x344) (R/  32) Chip ID Extension Register -------- */
#define CHIPID_EXID_OFFSET          0x344        /**< \brief (CHIPID_EXID offset) Chip ID Extension Register */
#define CHIPID_EXID_RESETVALUE      0x0400000F   /**< \brief (CHIPID_EXID reset_value) Chip ID Extension Register */
#define CHIPID_EXID_MASK            0xFFFFFFFFu  /**< \brief (CHIPID_EXID) MASK Register */

/** \brief CHIPID hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RoReg8  Reserved1[0x340];
  RoReg   CHIPID_CIDR;        /**< \brief (CHIPID Offset: 0x340) Chip ID Register */
  RoReg   CHIPID_EXID;        /**< \brief (CHIPID Offset: 0x344) Chip ID Extension Register */
} Chipid;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAM4L_CHIPID_COMPONENT_ */
