/**
 * \file
 *
 * \brief Component description for HCACHE
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

#ifndef _SAM4L_HCACHE_COMPONENT_
#define _SAM4L_HCACHE_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR HCACHE */
/* ========================================================================== */
/** \addtogroup SAM4L_HCACHE Cortex M I&D Cache Controller */
/*@{*/

#define HCACHE_I8323
#define REV_HCACHE                  0x101

/* -------- HCACHE_CTRL : (HCACHE Offset: 0x08) ( /W 32) Control Register -------- */
#define HCACHE_CTRL_OFFSET          0x08         /**< \brief (HCACHE_CTRL offset) Control Register */

#define HCACHE_CTRL_CEN_Pos         0            /**< \brief (HCACHE_CTRL) Cache Enable */
#define HCACHE_CTRL_CEN             (0x1u << HCACHE_CTRL_CEN_Pos)
#define   HCACHE_CTRL_CEN_NO_Val          0x0u   /**< \brief (HCACHE_CTRL) Disable Cache Controller */
#define   HCACHE_CTRL_CEN_YES_Val         0x1u   /**< \brief (HCACHE_CTRL) Enable Cache Controller */
#define HCACHE_CTRL_CEN_NO          (HCACHE_CTRL_CEN_NO_Val        << HCACHE_CTRL_CEN_Pos)
#define HCACHE_CTRL_CEN_YES         (HCACHE_CTRL_CEN_YES_Val       << HCACHE_CTRL_CEN_Pos)
#define HCACHE_CTRL_MASK            0x00000001u  /**< \brief (HCACHE_CTRL) MASK Register */

/* -------- HCACHE_SR : (HCACHE Offset: 0x0C) (R/W 32) Status Register -------- */
#define HCACHE_SR_OFFSET            0x0C         /**< \brief (HCACHE_SR offset) Status Register */

#define HCACHE_SR_CSTS_Pos          0            /**< \brief (HCACHE_SR) Cache Controller Status */
#define HCACHE_SR_CSTS              (0x1u << HCACHE_SR_CSTS_Pos)
#define   HCACHE_SR_CSTS_DIS_Val          0x0u   /**< \brief (HCACHE_SR) Cache Controller Disabled */
#define   HCACHE_SR_CSTS_EN_Val           0x1u   /**< \brief (HCACHE_SR) Cache Controller Enabled */
#define HCACHE_SR_CSTS_DIS          (HCACHE_SR_CSTS_DIS_Val        << HCACHE_SR_CSTS_Pos)
#define HCACHE_SR_CSTS_EN           (HCACHE_SR_CSTS_EN_Val         << HCACHE_SR_CSTS_Pos)
#define HCACHE_SR_MASK              0x00000001u  /**< \brief (HCACHE_SR) MASK Register */

/* -------- HCACHE_MAINT0 : (HCACHE Offset: 0x20) ( /W 32) Maintenance Register 0 -------- */
#define HCACHE_MAINT0_OFFSET        0x20         /**< \brief (HCACHE_MAINT0 offset) Maintenance Register 0 */

#define HCACHE_MAINT0_INVALL_Pos    0            /**< \brief (HCACHE_MAINT0) Cache Controller Invalidate All */
#define HCACHE_MAINT0_INVALL        (0x1u << HCACHE_MAINT0_INVALL_Pos)
#define   HCACHE_MAINT0_INVALL_NO_Val     0x0u   /**< \brief (HCACHE_MAINT0) No effect */
#define   HCACHE_MAINT0_INVALL_YES_Val    0x1u   /**< \brief (HCACHE_MAINT0) Invalidate all cache entries */
#define HCACHE_MAINT0_INVALL_NO     (HCACHE_MAINT0_INVALL_NO_Val   << HCACHE_MAINT0_INVALL_Pos)
#define HCACHE_MAINT0_INVALL_YES    (HCACHE_MAINT0_INVALL_YES_Val  << HCACHE_MAINT0_INVALL_Pos)
#define HCACHE_MAINT0_MASK          0x00000001u  /**< \brief (HCACHE_MAINT0) MASK Register */

/* -------- HCACHE_MAINT1 : (HCACHE Offset: 0x24) ( /W 32) Maintenance Register 1 -------- */
#define HCACHE_MAINT1_OFFSET        0x24         /**< \brief (HCACHE_MAINT1 offset) Maintenance Register 1 */

#define HCACHE_MAINT1_INDEX_Pos     4            /**< \brief (HCACHE_MAINT1) Invalidate Index */
#define HCACHE_MAINT1_INDEX_Msk     (0xFu << HCACHE_MAINT1_INDEX_Pos)
#define HCACHE_MAINT1_INDEX(value)  ((HCACHE_MAINT1_INDEX_Msk & ((value) << HCACHE_MAINT1_INDEX_Pos)))
#define HCACHE_MAINT1_MASK          0x000000F0u  /**< \brief (HCACHE_MAINT1) MASK Register */

/* -------- HCACHE_MCFG : (HCACHE Offset: 0x28) (R/W 32) Monitor Configuration Register -------- */
#define HCACHE_MCFG_OFFSET          0x28         /**< \brief (HCACHE_MCFG offset) Monitor Configuration Register */

#define HCACHE_MCFG_MODE_Pos        0            /**< \brief (HCACHE_MCFG) Cache Controller Monitor Counter Mode */
#define HCACHE_MCFG_MODE_Msk        (0x3u << HCACHE_MCFG_MODE_Pos)
#define HCACHE_MCFG_MODE(value)     ((HCACHE_MCFG_MODE_Msk & ((value) << HCACHE_MCFG_MODE_Pos)))
#define   HCACHE_MCFG_MODE_CYCLE_Val      0x0u   /**< \brief (HCACHE_MCFG) Cycle Counter */
#define   HCACHE_MCFG_MODE_IHIT_Val       0x1u   /**< \brief (HCACHE_MCFG) Instruction Hit Counter */
#define   HCACHE_MCFG_MODE_DHIT_Val       0x2u   /**< \brief (HCACHE_MCFG) Data Hit Counter */
#define HCACHE_MCFG_MODE_CYCLE      (HCACHE_MCFG_MODE_CYCLE_Val    << HCACHE_MCFG_MODE_Pos)
#define HCACHE_MCFG_MODE_IHIT       (HCACHE_MCFG_MODE_IHIT_Val     << HCACHE_MCFG_MODE_Pos)
#define HCACHE_MCFG_MODE_DHIT       (HCACHE_MCFG_MODE_DHIT_Val     << HCACHE_MCFG_MODE_Pos)
#define HCACHE_MCFG_MASK            0x00000003u  /**< \brief (HCACHE_MCFG) MASK Register */

/* -------- HCACHE_MEN : (HCACHE Offset: 0x2C) (R/W 32) Monitor Enable Register -------- */
#define HCACHE_MEN_OFFSET           0x2C         /**< \brief (HCACHE_MEN offset) Monitor Enable Register */

#define HCACHE_MEN_MENABLE_Pos      0            /**< \brief (HCACHE_MEN) Monitor Enable */
#define HCACHE_MEN_MENABLE          (0x1u << HCACHE_MEN_MENABLE_Pos)
#define   HCACHE_MEN_MENABLE_DIS_Val      0x0u   /**< \brief (HCACHE_MEN) Disable Monitor Counter */
#define   HCACHE_MEN_MENABLE_EN_Val       0x1u   /**< \brief (HCACHE_MEN) Enable Monitor Counter */
#define HCACHE_MEN_MENABLE_DIS      (HCACHE_MEN_MENABLE_DIS_Val    << HCACHE_MEN_MENABLE_Pos)
#define HCACHE_MEN_MENABLE_EN       (HCACHE_MEN_MENABLE_EN_Val     << HCACHE_MEN_MENABLE_Pos)
#define HCACHE_MEN_MASK             0x00000001u  /**< \brief (HCACHE_MEN) MASK Register */

/* -------- HCACHE_MCTRL : (HCACHE Offset: 0x30) ( /W 32) Monitor Control Register -------- */
#define HCACHE_MCTRL_OFFSET         0x30         /**< \brief (HCACHE_MCTRL offset) Monitor Control Register */

#define HCACHE_MCTRL_SWRST_Pos      0            /**< \brief (HCACHE_MCTRL) Monitor Software Reset */
#define HCACHE_MCTRL_SWRST          (0x1u << HCACHE_MCTRL_SWRST_Pos)
#define   HCACHE_MCTRL_SWRST_NO_Val       0x0u   /**< \brief (HCACHE_MCTRL) No effect */
#define   HCACHE_MCTRL_SWRST_YES_Val      0x1u   /**< \brief (HCACHE_MCTRL) Reset event counter register */
#define HCACHE_MCTRL_SWRST_NO       (HCACHE_MCTRL_SWRST_NO_Val     << HCACHE_MCTRL_SWRST_Pos)
#define HCACHE_MCTRL_SWRST_YES      (HCACHE_MCTRL_SWRST_YES_Val    << HCACHE_MCTRL_SWRST_Pos)
#define HCACHE_MCTRL_MASK           0x00000001u  /**< \brief (HCACHE_MCTRL) MASK Register */

/* -------- HCACHE_MSR : (HCACHE Offset: 0x34) (R/  32) Monitor Status Register -------- */
#define HCACHE_MSR_OFFSET           0x34         /**< \brief (HCACHE_MSR offset) Monitor Status Register */

#define HCACHE_MSR_EVENTCNT_Pos     0            /**< \brief (HCACHE_MSR) Monitor Event Counter */
#define HCACHE_MSR_EVENTCNT_Msk     (0xFFFFFFFFu << HCACHE_MSR_EVENTCNT_Pos)
#define HCACHE_MSR_EVENTCNT(value)  ((HCACHE_MSR_EVENTCNT_Msk & ((value) << HCACHE_MSR_EVENTCNT_Pos)))
#define HCACHE_MSR_MASK             0xFFFFFFFFu  /**< \brief (HCACHE_MSR) MASK Register */

/* -------- HCACHE_VERSION : (HCACHE Offset: 0xFC) (R/  32) Version Register -------- */
#define HCACHE_VERSION_OFFSET       0xFC         /**< \brief (HCACHE_VERSION offset) Version Register */
#define HCACHE_VERSION_RESETVALUE   0x00000101   /**< \brief (HCACHE_VERSION reset_value) Version Register */

#define HCACHE_VERSION_VERSION_Pos  0            /**< \brief (HCACHE_VERSION) VERSION */
#define HCACHE_VERSION_VERSION_Msk  (0xFFFu << HCACHE_VERSION_VERSION_Pos)
#define HCACHE_VERSION_VERSION(value) ((HCACHE_VERSION_VERSION_Msk & ((value) << HCACHE_VERSION_VERSION_Pos)))
#define HCACHE_VERSION_MFN_Pos      16           /**< \brief (HCACHE_VERSION) MFN */
#define HCACHE_VERSION_MFN_Msk      (0xFu << HCACHE_VERSION_MFN_Pos)
#define HCACHE_VERSION_MFN(value)   ((HCACHE_VERSION_MFN_Msk & ((value) << HCACHE_VERSION_MFN_Pos)))
#define HCACHE_VERSION_MASK         0x000F0FFFu  /**< \brief (HCACHE_VERSION) MASK Register */

/** \brief HCACHE hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RoReg8  Reserved1[0x8];
  WoReg   HCACHE_CTRL;        /**< \brief (HCACHE Offset: 0x08) Control Register */
  RwReg   HCACHE_SR;          /**< \brief (HCACHE Offset: 0x0C) Status Register */
  RoReg8  Reserved2[0x10];
  WoReg   HCACHE_MAINT0;      /**< \brief (HCACHE Offset: 0x20) Maintenance Register 0 */
  WoReg   HCACHE_MAINT1;      /**< \brief (HCACHE Offset: 0x24) Maintenance Register 1 */
  RwReg   HCACHE_MCFG;        /**< \brief (HCACHE Offset: 0x28) Monitor Configuration Register */
  RwReg   HCACHE_MEN;         /**< \brief (HCACHE Offset: 0x2C) Monitor Enable Register */
  WoReg   HCACHE_MCTRL;       /**< \brief (HCACHE Offset: 0x30) Monitor Control Register */
  RoReg   HCACHE_MSR;         /**< \brief (HCACHE Offset: 0x34) Monitor Status Register */
  RoReg8  Reserved3[0xC4];
  RoReg   HCACHE_VERSION;     /**< \brief (HCACHE Offset: 0xFC) Version Register */
} Hcache;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAM4L_HCACHE_COMPONENT_ */
