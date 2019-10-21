/**
 * \file
 *
 * \brief Component description for HMATRIXB
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

#ifndef _SAM4L_HMATRIXB_COMPONENT_
#define _SAM4L_HMATRIXB_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR HMATRIXB */
/* ========================================================================== */
/** \addtogroup SAM4L_HMATRIXB HSB Matrix */
/*@{*/

#define HMATRIXB_I7638
#define REV_HMATRIXB                0x130

/* -------- HMATRIXB_MCFG : (HMATRIXB Offset: 0x000) (R/W 32) mcfg Master Configuration Register -------- */
#define HMATRIXB_MCFG_OFFSET        0x000        /**< \brief (HMATRIXB_MCFG offset) Master Configuration Register */
#define HMATRIXB_MCFG_RESETVALUE    0x00000002   /**< \brief (HMATRIXB_MCFG reset_value) Master Configuration Register */

#define HMATRIXB_MCFG_ULBT_Pos      0            /**< \brief (HMATRIXB_MCFG) Undefined Length Burst Type */
#define HMATRIXB_MCFG_ULBT_Msk      (0x7u << HMATRIXB_MCFG_ULBT_Pos)
#define HMATRIXB_MCFG_ULBT(value)   ((HMATRIXB_MCFG_ULBT_Msk & ((value) << HMATRIXB_MCFG_ULBT_Pos)))
#define   HMATRIXB_MCFG_ULBT_INFINITE_Val 0x0u   /**< \brief (HMATRIXB_MCFG) Infinite Length */
#define   HMATRIXB_MCFG_ULBT_SINGLE_Val   0x1u   /**< \brief (HMATRIXB_MCFG) Single Access */
#define   HMATRIXB_MCFG_ULBT_FOUR_BEAT_Val 0x2u   /**< \brief (HMATRIXB_MCFG) Four Beat Burst */
#define   HMATRIXB_MCFG_ULBT_EIGHT_BEAT_Val 0x3u   /**< \brief (HMATRIXB_MCFG) Eight Beat Burst */
#define   HMATRIXB_MCFG_ULBT_SIXTEEN_BEAT_Val 0x4u   /**< \brief (HMATRIXB_MCFG) Sixteen Beat Burst */
#define HMATRIXB_MCFG_ULBT_INFINITE (HMATRIXB_MCFG_ULBT_INFINITE_Val << HMATRIXB_MCFG_ULBT_Pos)
#define HMATRIXB_MCFG_ULBT_SINGLE   (HMATRIXB_MCFG_ULBT_SINGLE_Val << HMATRIXB_MCFG_ULBT_Pos)
#define HMATRIXB_MCFG_ULBT_FOUR_BEAT (HMATRIXB_MCFG_ULBT_FOUR_BEAT_Val << HMATRIXB_MCFG_ULBT_Pos)
#define HMATRIXB_MCFG_ULBT_EIGHT_BEAT (HMATRIXB_MCFG_ULBT_EIGHT_BEAT_Val << HMATRIXB_MCFG_ULBT_Pos)
#define HMATRIXB_MCFG_ULBT_SIXTEEN_BEAT (HMATRIXB_MCFG_ULBT_SIXTEEN_BEAT_Val << HMATRIXB_MCFG_ULBT_Pos)
#define HMATRIXB_MCFG_MASK          0x00000007u  /**< \brief (HMATRIXB_MCFG) MASK Register */

/* -------- HMATRIXB_SCFG : (HMATRIXB Offset: 0x040) (R/W 32) scfg Slave Configuration Register -------- */
#define HMATRIXB_SCFG_OFFSET        0x040        /**< \brief (HMATRIXB_SCFG offset) Slave Configuration Register */
#define HMATRIXB_SCFG_RESETVALUE    0x00000010   /**< \brief (HMATRIXB_SCFG reset_value) Slave Configuration Register */

#define HMATRIXB_SCFG_SLOT_CYCLE_Pos 0            /**< \brief (HMATRIXB_SCFG) Maximum Number of Allowed Cycles for a Burst */
#define HMATRIXB_SCFG_SLOT_CYCLE_Msk (0xFFu << HMATRIXB_SCFG_SLOT_CYCLE_Pos)
#define HMATRIXB_SCFG_SLOT_CYCLE(value) ((HMATRIXB_SCFG_SLOT_CYCLE_Msk & ((value) << HMATRIXB_SCFG_SLOT_CYCLE_Pos)))
#define HMATRIXB_SCFG_DEFMSTR_TYPE_Pos 16           /**< \brief (HMATRIXB_SCFG) Default Master Type */
#define HMATRIXB_SCFG_DEFMSTR_TYPE_Msk (0x3u << HMATRIXB_SCFG_DEFMSTR_TYPE_Pos)
#define HMATRIXB_SCFG_DEFMSTR_TYPE(value) ((HMATRIXB_SCFG_DEFMSTR_TYPE_Msk & ((value) << HMATRIXB_SCFG_DEFMSTR_TYPE_Pos)))
#define   HMATRIXB_SCFG_DEFMSTR_TYPE_NO_DEFAULT_Val 0x0u   /**< \brief (HMATRIXB_SCFG) No Default Master. At the end of current slave access, if no other master request is pending, the slave is deconnected from all masters. This resusts in having a one cycle latency for the first transfer of a burst. */
#define   HMATRIXB_SCFG_DEFMSTR_TYPE_LAST_DEFAULT_Val 0x1u   /**< \brief (HMATRIXB_SCFG) Last Default Master At the end of current slave access, if no other master request is pending, the slave stay connected with the last master havingaccessed it.This resusts in not having the one cycle latency when the last master re-trying access on the slave. */
#define   HMATRIXB_SCFG_DEFMSTR_TYPE_FIXED_DEFAULT_Val 0x2u   /**< \brief (HMATRIXB_SCFG) Fixed Default Master At the end of current slave access, if no other master request is pending, the slave connects with fixed master which numberis in FIXED_DEFMSTR register.This resusts in not having the one cycle latency when the fixed master re-trying access on the slave. */
#define HMATRIXB_SCFG_DEFMSTR_TYPE_NO_DEFAULT (HMATRIXB_SCFG_DEFMSTR_TYPE_NO_DEFAULT_Val << HMATRIXB_SCFG_DEFMSTR_TYPE_Pos)
#define HMATRIXB_SCFG_DEFMSTR_TYPE_LAST_DEFAULT (HMATRIXB_SCFG_DEFMSTR_TYPE_LAST_DEFAULT_Val << HMATRIXB_SCFG_DEFMSTR_TYPE_Pos)
#define HMATRIXB_SCFG_DEFMSTR_TYPE_FIXED_DEFAULT (HMATRIXB_SCFG_DEFMSTR_TYPE_FIXED_DEFAULT_Val << HMATRIXB_SCFG_DEFMSTR_TYPE_Pos)
#define HMATRIXB_SCFG_FIXED_DEFMSTR_Pos 18           /**< \brief (HMATRIXB_SCFG) Fixed Index of Default Master */
#define HMATRIXB_SCFG_FIXED_DEFMSTR_Msk (0xFu << HMATRIXB_SCFG_FIXED_DEFMSTR_Pos)
#define HMATRIXB_SCFG_FIXED_DEFMSTR(value) ((HMATRIXB_SCFG_FIXED_DEFMSTR_Msk & ((value) << HMATRIXB_SCFG_FIXED_DEFMSTR_Pos)))
#define HMATRIXB_SCFG_ARBT_Pos      24           /**< \brief (HMATRIXB_SCFG) Arbitration Type */
#define HMATRIXB_SCFG_ARBT          (0x1u << HMATRIXB_SCFG_ARBT_Pos)
#define   HMATRIXB_SCFG_ARBT_ROUND_ROBIN_Val 0x0u   /**< \brief (HMATRIXB_SCFG) Round-Robin Arbitration */
#define   HMATRIXB_SCFG_ARBT_FIXED_PRIORITY_Val 0x1u   /**< \brief (HMATRIXB_SCFG) Fixed Priority Arbitration */
#define HMATRIXB_SCFG_ARBT_ROUND_ROBIN (HMATRIXB_SCFG_ARBT_ROUND_ROBIN_Val << HMATRIXB_SCFG_ARBT_Pos)
#define HMATRIXB_SCFG_ARBT_FIXED_PRIORITY (HMATRIXB_SCFG_ARBT_FIXED_PRIORITY_Val << HMATRIXB_SCFG_ARBT_Pos)
#define HMATRIXB_SCFG_MASK          0x013F00FFu  /**< \brief (HMATRIXB_SCFG) MASK Register */

/* -------- HMATRIXB_PRAS : (HMATRIXB Offset: 0x080) (R/W 32) prs Priority Register A for Slave -------- */
#define HMATRIXB_PRAS_OFFSET        0x080        /**< \brief (HMATRIXB_PRAS offset) Priority Register A for Slave */
#define HMATRIXB_PRAS_RESETVALUE    0x00000000   /**< \brief (HMATRIXB_PRAS reset_value) Priority Register A for Slave */

#define HMATRIXB_PRAS_M0PR_Pos      0            /**< \brief (HMATRIXB_PRAS) Master 0 Priority */
#define HMATRIXB_PRAS_M0PR_Msk      (0xFu << HMATRIXB_PRAS_M0PR_Pos)
#define HMATRIXB_PRAS_M0PR(value)   ((HMATRIXB_PRAS_M0PR_Msk & ((value) << HMATRIXB_PRAS_M0PR_Pos)))
#define HMATRIXB_PRAS_M1PR_Pos      4            /**< \brief (HMATRIXB_PRAS) Master 1 Priority */
#define HMATRIXB_PRAS_M1PR_Msk      (0xFu << HMATRIXB_PRAS_M1PR_Pos)
#define HMATRIXB_PRAS_M1PR(value)   ((HMATRIXB_PRAS_M1PR_Msk & ((value) << HMATRIXB_PRAS_M1PR_Pos)))
#define HMATRIXB_PRAS_M2PR_Pos      8            /**< \brief (HMATRIXB_PRAS) Master 2 Priority */
#define HMATRIXB_PRAS_M2PR_Msk      (0xFu << HMATRIXB_PRAS_M2PR_Pos)
#define HMATRIXB_PRAS_M2PR(value)   ((HMATRIXB_PRAS_M2PR_Msk & ((value) << HMATRIXB_PRAS_M2PR_Pos)))
#define HMATRIXB_PRAS_M3PR_Pos      12           /**< \brief (HMATRIXB_PRAS) Master 3 Priority */
#define HMATRIXB_PRAS_M3PR_Msk      (0xFu << HMATRIXB_PRAS_M3PR_Pos)
#define HMATRIXB_PRAS_M3PR(value)   ((HMATRIXB_PRAS_M3PR_Msk & ((value) << HMATRIXB_PRAS_M3PR_Pos)))
#define HMATRIXB_PRAS_M4PR_Pos      16           /**< \brief (HMATRIXB_PRAS) Master 4 Priority */
#define HMATRIXB_PRAS_M4PR_Msk      (0xFu << HMATRIXB_PRAS_M4PR_Pos)
#define HMATRIXB_PRAS_M4PR(value)   ((HMATRIXB_PRAS_M4PR_Msk & ((value) << HMATRIXB_PRAS_M4PR_Pos)))
#define HMATRIXB_PRAS_M5PR_Pos      20           /**< \brief (HMATRIXB_PRAS) Master 5 Priority */
#define HMATRIXB_PRAS_M5PR_Msk      (0xFu << HMATRIXB_PRAS_M5PR_Pos)
#define HMATRIXB_PRAS_M5PR(value)   ((HMATRIXB_PRAS_M5PR_Msk & ((value) << HMATRIXB_PRAS_M5PR_Pos)))
#define HMATRIXB_PRAS_M6PR_Pos      24           /**< \brief (HMATRIXB_PRAS) Master 6 Priority */
#define HMATRIXB_PRAS_M6PR_Msk      (0xFu << HMATRIXB_PRAS_M6PR_Pos)
#define HMATRIXB_PRAS_M6PR(value)   ((HMATRIXB_PRAS_M6PR_Msk & ((value) << HMATRIXB_PRAS_M6PR_Pos)))
#define HMATRIXB_PRAS_M7PR_Pos      28           /**< \brief (HMATRIXB_PRAS) Master 7 Priority */
#define HMATRIXB_PRAS_M7PR_Msk      (0xFu << HMATRIXB_PRAS_M7PR_Pos)
#define HMATRIXB_PRAS_M7PR(value)   ((HMATRIXB_PRAS_M7PR_Msk & ((value) << HMATRIXB_PRAS_M7PR_Pos)))
#define HMATRIXB_PRAS_MASK          0xFFFFFFFFu  /**< \brief (HMATRIXB_PRAS) MASK Register */

/* -------- HMATRIXB_PRBS : (HMATRIXB Offset: 0x084) (R/W 32) prs Priority Register B for Slave -------- */
#define HMATRIXB_PRBS_OFFSET        0x084        /**< \brief (HMATRIXB_PRBS offset) Priority Register B for Slave */
#define HMATRIXB_PRBS_RESETVALUE    0x00000000   /**< \brief (HMATRIXB_PRBS reset_value) Priority Register B for Slave */

#define HMATRIXB_PRBS_M8PR_Pos      0            /**< \brief (HMATRIXB_PRBS) Master 8 Priority */
#define HMATRIXB_PRBS_M8PR_Msk      (0xFu << HMATRIXB_PRBS_M8PR_Pos)
#define HMATRIXB_PRBS_M8PR(value)   ((HMATRIXB_PRBS_M8PR_Msk & ((value) << HMATRIXB_PRBS_M8PR_Pos)))
#define HMATRIXB_PRBS_M9PR_Pos      4            /**< \brief (HMATRIXB_PRBS) Master 9 Priority */
#define HMATRIXB_PRBS_M9PR_Msk      (0xFu << HMATRIXB_PRBS_M9PR_Pos)
#define HMATRIXB_PRBS_M9PR(value)   ((HMATRIXB_PRBS_M9PR_Msk & ((value) << HMATRIXB_PRBS_M9PR_Pos)))
#define HMATRIXB_PRBS_M10PR_Pos     8            /**< \brief (HMATRIXB_PRBS) Master 10 Priority */
#define HMATRIXB_PRBS_M10PR_Msk     (0xFu << HMATRIXB_PRBS_M10PR_Pos)
#define HMATRIXB_PRBS_M10PR(value)  ((HMATRIXB_PRBS_M10PR_Msk & ((value) << HMATRIXB_PRBS_M10PR_Pos)))
#define HMATRIXB_PRBS_M11PR_Pos     12           /**< \brief (HMATRIXB_PRBS) Master 11 Priority */
#define HMATRIXB_PRBS_M11PR_Msk     (0xFu << HMATRIXB_PRBS_M11PR_Pos)
#define HMATRIXB_PRBS_M11PR(value)  ((HMATRIXB_PRBS_M11PR_Msk & ((value) << HMATRIXB_PRBS_M11PR_Pos)))
#define HMATRIXB_PRBS_M12PR_Pos     16           /**< \brief (HMATRIXB_PRBS) Master 12 Priority */
#define HMATRIXB_PRBS_M12PR_Msk     (0xFu << HMATRIXB_PRBS_M12PR_Pos)
#define HMATRIXB_PRBS_M12PR(value)  ((HMATRIXB_PRBS_M12PR_Msk & ((value) << HMATRIXB_PRBS_M12PR_Pos)))
#define HMATRIXB_PRBS_M13PR_Pos     20           /**< \brief (HMATRIXB_PRBS) Master 13 Priority */
#define HMATRIXB_PRBS_M13PR_Msk     (0xFu << HMATRIXB_PRBS_M13PR_Pos)
#define HMATRIXB_PRBS_M13PR(value)  ((HMATRIXB_PRBS_M13PR_Msk & ((value) << HMATRIXB_PRBS_M13PR_Pos)))
#define HMATRIXB_PRBS_M14PR_Pos     24           /**< \brief (HMATRIXB_PRBS) Master 14 Priority */
#define HMATRIXB_PRBS_M14PR_Msk     (0xFu << HMATRIXB_PRBS_M14PR_Pos)
#define HMATRIXB_PRBS_M14PR(value)  ((HMATRIXB_PRBS_M14PR_Msk & ((value) << HMATRIXB_PRBS_M14PR_Pos)))
#define HMATRIXB_PRBS_M15PR_Pos     28           /**< \brief (HMATRIXB_PRBS) Master 15 Priority */
#define HMATRIXB_PRBS_M15PR_Msk     (0xFu << HMATRIXB_PRBS_M15PR_Pos)
#define HMATRIXB_PRBS_M15PR(value)  ((HMATRIXB_PRBS_M15PR_Msk & ((value) << HMATRIXB_PRBS_M15PR_Pos)))
#define HMATRIXB_PRBS_MASK          0xFFFFFFFFu  /**< \brief (HMATRIXB_PRBS) MASK Register */

/* -------- HMATRIXB_MRCR : (HMATRIXB Offset: 0x100) (R/W 32) Master Remap Control Register -------- */
#define HMATRIXB_MRCR_OFFSET        0x100        /**< \brief (HMATRIXB_MRCR offset) Master Remap Control Register */
#define HMATRIXB_MRCR_RESETVALUE    0x00000000   /**< \brief (HMATRIXB_MRCR reset_value) Master Remap Control Register */

#define HMATRIXB_MRCR_RCB0_Pos      0            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 0 */
#define HMATRIXB_MRCR_RCB0          (0x1u << HMATRIXB_MRCR_RCB0_Pos)
#define   HMATRIXB_MRCR_RCB0_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB0_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB0_0        (HMATRIXB_MRCR_RCB0_0_Val      << HMATRIXB_MRCR_RCB0_Pos)
#define HMATRIXB_MRCR_RCB0_1        (HMATRIXB_MRCR_RCB0_1_Val      << HMATRIXB_MRCR_RCB0_Pos)
#define HMATRIXB_MRCR_RCB1_Pos      1            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 1 */
#define HMATRIXB_MRCR_RCB1          (0x1u << HMATRIXB_MRCR_RCB1_Pos)
#define   HMATRIXB_MRCR_RCB1_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB1_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB1_0        (HMATRIXB_MRCR_RCB1_0_Val      << HMATRIXB_MRCR_RCB1_Pos)
#define HMATRIXB_MRCR_RCB1_1        (HMATRIXB_MRCR_RCB1_1_Val      << HMATRIXB_MRCR_RCB1_Pos)
#define HMATRIXB_MRCR_RCB2_Pos      2            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 2 */
#define HMATRIXB_MRCR_RCB2          (0x1u << HMATRIXB_MRCR_RCB2_Pos)
#define   HMATRIXB_MRCR_RCB2_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB2_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB2_0        (HMATRIXB_MRCR_RCB2_0_Val      << HMATRIXB_MRCR_RCB2_Pos)
#define HMATRIXB_MRCR_RCB2_1        (HMATRIXB_MRCR_RCB2_1_Val      << HMATRIXB_MRCR_RCB2_Pos)
#define HMATRIXB_MRCR_RCB3_Pos      3            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 3 */
#define HMATRIXB_MRCR_RCB3          (0x1u << HMATRIXB_MRCR_RCB3_Pos)
#define   HMATRIXB_MRCR_RCB3_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB3_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB3_0        (HMATRIXB_MRCR_RCB3_0_Val      << HMATRIXB_MRCR_RCB3_Pos)
#define HMATRIXB_MRCR_RCB3_1        (HMATRIXB_MRCR_RCB3_1_Val      << HMATRIXB_MRCR_RCB3_Pos)
#define HMATRIXB_MRCR_RCB4_Pos      4            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 4 */
#define HMATRIXB_MRCR_RCB4          (0x1u << HMATRIXB_MRCR_RCB4_Pos)
#define   HMATRIXB_MRCR_RCB4_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB4_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB4_0        (HMATRIXB_MRCR_RCB4_0_Val      << HMATRIXB_MRCR_RCB4_Pos)
#define HMATRIXB_MRCR_RCB4_1        (HMATRIXB_MRCR_RCB4_1_Val      << HMATRIXB_MRCR_RCB4_Pos)
#define HMATRIXB_MRCR_RCB5_Pos      5            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 5 */
#define HMATRIXB_MRCR_RCB5          (0x1u << HMATRIXB_MRCR_RCB5_Pos)
#define   HMATRIXB_MRCR_RCB5_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB5_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB5_0        (HMATRIXB_MRCR_RCB5_0_Val      << HMATRIXB_MRCR_RCB5_Pos)
#define HMATRIXB_MRCR_RCB5_1        (HMATRIXB_MRCR_RCB5_1_Val      << HMATRIXB_MRCR_RCB5_Pos)
#define HMATRIXB_MRCR_RCB6_Pos      6            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 6 */
#define HMATRIXB_MRCR_RCB6          (0x1u << HMATRIXB_MRCR_RCB6_Pos)
#define   HMATRIXB_MRCR_RCB6_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB6_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB6_0        (HMATRIXB_MRCR_RCB6_0_Val      << HMATRIXB_MRCR_RCB6_Pos)
#define HMATRIXB_MRCR_RCB6_1        (HMATRIXB_MRCR_RCB6_1_Val      << HMATRIXB_MRCR_RCB6_Pos)
#define HMATRIXB_MRCR_RCB7_Pos      7            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 7 */
#define HMATRIXB_MRCR_RCB7          (0x1u << HMATRIXB_MRCR_RCB7_Pos)
#define   HMATRIXB_MRCR_RCB7_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB7_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB7_0        (HMATRIXB_MRCR_RCB7_0_Val      << HMATRIXB_MRCR_RCB7_Pos)
#define HMATRIXB_MRCR_RCB7_1        (HMATRIXB_MRCR_RCB7_1_Val      << HMATRIXB_MRCR_RCB7_Pos)
#define HMATRIXB_MRCR_RCB8_Pos      8            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 8 */
#define HMATRIXB_MRCR_RCB8          (0x1u << HMATRIXB_MRCR_RCB8_Pos)
#define   HMATRIXB_MRCR_RCB8_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB8_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB8_0        (HMATRIXB_MRCR_RCB8_0_Val      << HMATRIXB_MRCR_RCB8_Pos)
#define HMATRIXB_MRCR_RCB8_1        (HMATRIXB_MRCR_RCB8_1_Val      << HMATRIXB_MRCR_RCB8_Pos)
#define HMATRIXB_MRCR_RCB9_Pos      9            /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 9 */
#define HMATRIXB_MRCR_RCB9          (0x1u << HMATRIXB_MRCR_RCB9_Pos)
#define   HMATRIXB_MRCR_RCB9_0_Val        0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB9_1_Val        0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB9_0        (HMATRIXB_MRCR_RCB9_0_Val      << HMATRIXB_MRCR_RCB9_Pos)
#define HMATRIXB_MRCR_RCB9_1        (HMATRIXB_MRCR_RCB9_1_Val      << HMATRIXB_MRCR_RCB9_Pos)
#define HMATRIXB_MRCR_RCB10_Pos     10           /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 10 */
#define HMATRIXB_MRCR_RCB10         (0x1u << HMATRIXB_MRCR_RCB10_Pos)
#define   HMATRIXB_MRCR_RCB10_0_Val       0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB10_1_Val       0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB10_0       (HMATRIXB_MRCR_RCB10_0_Val     << HMATRIXB_MRCR_RCB10_Pos)
#define HMATRIXB_MRCR_RCB10_1       (HMATRIXB_MRCR_RCB10_1_Val     << HMATRIXB_MRCR_RCB10_Pos)
#define HMATRIXB_MRCR_RCB11_Pos     11           /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 11 */
#define HMATRIXB_MRCR_RCB11         (0x1u << HMATRIXB_MRCR_RCB11_Pos)
#define   HMATRIXB_MRCR_RCB11_0_Val       0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB11_1_Val       0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB11_0       (HMATRIXB_MRCR_RCB11_0_Val     << HMATRIXB_MRCR_RCB11_Pos)
#define HMATRIXB_MRCR_RCB11_1       (HMATRIXB_MRCR_RCB11_1_Val     << HMATRIXB_MRCR_RCB11_Pos)
#define HMATRIXB_MRCR_RCB12_Pos     12           /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 12 */
#define HMATRIXB_MRCR_RCB12         (0x1u << HMATRIXB_MRCR_RCB12_Pos)
#define   HMATRIXB_MRCR_RCB12_0_Val       0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB12_1_Val       0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB12_0       (HMATRIXB_MRCR_RCB12_0_Val     << HMATRIXB_MRCR_RCB12_Pos)
#define HMATRIXB_MRCR_RCB12_1       (HMATRIXB_MRCR_RCB12_1_Val     << HMATRIXB_MRCR_RCB12_Pos)
#define HMATRIXB_MRCR_RCB13_Pos     13           /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 13 */
#define HMATRIXB_MRCR_RCB13         (0x1u << HMATRIXB_MRCR_RCB13_Pos)
#define   HMATRIXB_MRCR_RCB13_0_Val       0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB13_1_Val       0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB13_0       (HMATRIXB_MRCR_RCB13_0_Val     << HMATRIXB_MRCR_RCB13_Pos)
#define HMATRIXB_MRCR_RCB13_1       (HMATRIXB_MRCR_RCB13_1_Val     << HMATRIXB_MRCR_RCB13_Pos)
#define HMATRIXB_MRCR_RCB14_Pos     14           /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 14 */
#define HMATRIXB_MRCR_RCB14         (0x1u << HMATRIXB_MRCR_RCB14_Pos)
#define   HMATRIXB_MRCR_RCB14_0_Val       0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB14_1_Val       0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB14_0       (HMATRIXB_MRCR_RCB14_0_Val     << HMATRIXB_MRCR_RCB14_Pos)
#define HMATRIXB_MRCR_RCB14_1       (HMATRIXB_MRCR_RCB14_1_Val     << HMATRIXB_MRCR_RCB14_Pos)
#define HMATRIXB_MRCR_RCB15_Pos     15           /**< \brief (HMATRIXB_MRCR) Remap Command bit for Master 15 */
#define HMATRIXB_MRCR_RCB15         (0x1u << HMATRIXB_MRCR_RCB15_Pos)
#define   HMATRIXB_MRCR_RCB15_0_Val       0x0u   /**< \brief (HMATRIXB_MRCR) Disable remapped address decoding for master */
#define   HMATRIXB_MRCR_RCB15_1_Val       0x1u   /**< \brief (HMATRIXB_MRCR) Enable remapped address decoding for master */
#define HMATRIXB_MRCR_RCB15_0       (HMATRIXB_MRCR_RCB15_0_Val     << HMATRIXB_MRCR_RCB15_Pos)
#define HMATRIXB_MRCR_RCB15_1       (HMATRIXB_MRCR_RCB15_1_Val     << HMATRIXB_MRCR_RCB15_Pos)
#define HMATRIXB_MRCR_MASK          0x0000FFFFu  /**< \brief (HMATRIXB_MRCR) MASK Register */

/* -------- HMATRIXB_SFR : (HMATRIXB Offset: 0x110) (R/W 32) sfr Special Function Register -------- */
#define HMATRIXB_SFR_OFFSET         0x110        /**< \brief (HMATRIXB_SFR offset) Special Function Register */
#define HMATRIXB_SFR_RESETVALUE     0x00000000   /**< \brief (HMATRIXB_SFR reset_value) Special Function Register */

#define HMATRIXB_SFR_SFR_Pos        0            /**< \brief (HMATRIXB_SFR) Special Function Register */
#define HMATRIXB_SFR_SFR_Msk        (0xFFFFFFFFu << HMATRIXB_SFR_SFR_Pos)
#define HMATRIXB_SFR_SFR(value)     ((HMATRIXB_SFR_SFR_Msk & ((value) << HMATRIXB_SFR_SFR_Pos)))
#define HMATRIXB_SFR_MASK           0xFFFFFFFFu  /**< \brief (HMATRIXB_SFR) MASK Register */

/** \brief HmatrixbMcfg hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RwReg   HMATRIXB_MCFG;      /**< \brief (HMATRIXB Offset: 0x000) Master Configuration Register */
} HmatrixbMcfg;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief HmatrixbPrs hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RwReg   HMATRIXB_PRAS;      /**< \brief (HMATRIXB Offset: 0x000) Priority Register A for Slave */
  RwReg   HMATRIXB_PRBS;      /**< \brief (HMATRIXB Offset: 0x004) Priority Register B for Slave */
} HmatrixbPrs;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief HmatrixbScfg hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RwReg   HMATRIXB_SCFG;      /**< \brief (HMATRIXB Offset: 0x000) Slave Configuration Register */
} HmatrixbScfg;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief HmatrixbSfr hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RwReg   HMATRIXB_SFR;       /**< \brief (HMATRIXB Offset: 0x000) Special Function Register */
} HmatrixbSfr;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief HMATRIXB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  HmatrixbMcfg HMATRIXB_MCFG[16];  /**< \brief (HMATRIXB Offset: 0x000) HmatrixbMcfg groups */
  HmatrixbScfg HMATRIXB_SCFG[16];  /**< \brief (HMATRIXB Offset: 0x040) HmatrixbScfg groups */
  HmatrixbPrs HMATRIXB_PRS[16];   /**< \brief (HMATRIXB Offset: 0x080) HmatrixbPrs groups */
  RwReg   HMATRIXB_MRCR;      /**< \brief (HMATRIXB Offset: 0x100) Master Remap Control Register */
  RoReg8  Reserved1[0xC];
  HmatrixbSfr HMATRIXB_SFR[16];   /**< \brief (HMATRIXB Offset: 0x110) HmatrixbSfr groups */
} Hmatrixb;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAM4L_HMATRIXB_COMPONENT_ */
