/**
 * \file
 *
 * \brief Component description for FLASHCALW
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

#ifndef _SAM4L_FLASHCALW_COMPONENT_
#define _SAM4L_FLASHCALW_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR FLASHCALW */
/* ========================================================================== */
/** \addtogroup SAM4L_FLASHCALW Flash Controller */
/*@{*/

#define FLASHCALW_I8322
#define REV_FLASHCALW               0x110

/* -------- FLASHCALW_FCR : (FLASHCALW Offset: 0x00) (R/W 32) Flash Controller Control Register -------- */
#define FLASHCALW_FCR_OFFSET        0x00         /**< \brief (FLASHCALW_FCR offset) Flash Controller Control Register */
#define FLASHCALW_FCR_RESETVALUE    0x00000000   /**< \brief (FLASHCALW_FCR reset_value) Flash Controller Control Register */

#define FLASHCALW_FCR_FRDY_Pos      0            /**< \brief (FLASHCALW_FCR) Flash Ready Interrupt Enable */
#define FLASHCALW_FCR_FRDY          (0x1u << FLASHCALW_FCR_FRDY_Pos)
#define   FLASHCALW_FCR_FRDY_0_Val        0x0u   /**< \brief (FLASHCALW_FCR) Flash Ready does not generate an interrupt */
#define   FLASHCALW_FCR_FRDY_1_Val        0x1u   /**< \brief (FLASHCALW_FCR) Flash Ready generates an interrupt */
#define FLASHCALW_FCR_FRDY_0        (FLASHCALW_FCR_FRDY_0_Val      << FLASHCALW_FCR_FRDY_Pos)
#define FLASHCALW_FCR_FRDY_1        (FLASHCALW_FCR_FRDY_1_Val      << FLASHCALW_FCR_FRDY_Pos)
#define FLASHCALW_FCR_LOCKE_Pos     2            /**< \brief (FLASHCALW_FCR) Lock Error Interrupt Enable */
#define FLASHCALW_FCR_LOCKE         (0x1u << FLASHCALW_FCR_LOCKE_Pos)
#define   FLASHCALW_FCR_LOCKE_0_Val       0x0u   /**< \brief (FLASHCALW_FCR) Lock Error does not generate an interrupt */
#define   FLASHCALW_FCR_LOCKE_1_Val       0x1u   /**< \brief (FLASHCALW_FCR) Lock Error generates an interrupt */
#define FLASHCALW_FCR_LOCKE_0       (FLASHCALW_FCR_LOCKE_0_Val     << FLASHCALW_FCR_LOCKE_Pos)
#define FLASHCALW_FCR_LOCKE_1       (FLASHCALW_FCR_LOCKE_1_Val     << FLASHCALW_FCR_LOCKE_Pos)
#define FLASHCALW_FCR_PROGE_Pos     3            /**< \brief (FLASHCALW_FCR) Programming Error Interrupt Enable */
#define FLASHCALW_FCR_PROGE         (0x1u << FLASHCALW_FCR_PROGE_Pos)
#define   FLASHCALW_FCR_PROGE_0_Val       0x0u   /**< \brief (FLASHCALW_FCR) Programming Error does not generate an interrupt */
#define   FLASHCALW_FCR_PROGE_1_Val       0x1u   /**< \brief (FLASHCALW_FCR) Programming Error generates an interrupt */
#define FLASHCALW_FCR_PROGE_0       (FLASHCALW_FCR_PROGE_0_Val     << FLASHCALW_FCR_PROGE_Pos)
#define FLASHCALW_FCR_PROGE_1       (FLASHCALW_FCR_PROGE_1_Val     << FLASHCALW_FCR_PROGE_Pos)
#define FLASHCALW_FCR_FWS_Pos       6            /**< \brief (FLASHCALW_FCR) Flash Wait State */
#define FLASHCALW_FCR_FWS           (0x1u << FLASHCALW_FCR_FWS_Pos)
#define   FLASHCALW_FCR_FWS_0_Val         0x0u   /**< \brief (FLASHCALW_FCR) The flash is read with 0 wait states */
#define   FLASHCALW_FCR_FWS_1_Val         0x1u   /**< \brief (FLASHCALW_FCR) The flash is read with 1 wait states */
#define FLASHCALW_FCR_FWS_0         (FLASHCALW_FCR_FWS_0_Val       << FLASHCALW_FCR_FWS_Pos)
#define FLASHCALW_FCR_FWS_1         (FLASHCALW_FCR_FWS_1_Val       << FLASHCALW_FCR_FWS_Pos)
#define FLASHCALW_FCR_WS1OPT_Pos    7            /**< \brief (FLASHCALW_FCR) Wait State 1 Optimization */
#define FLASHCALW_FCR_WS1OPT        (0x1u << FLASHCALW_FCR_WS1OPT_Pos)
#define FLASHCALW_FCR_MASK          0x000000CDu  /**< \brief (FLASHCALW_FCR) MASK Register */

/* -------- FLASHCALW_FCMD : (FLASHCALW Offset: 0x04) (R/W 32) Flash Controller Command Register -------- */
#define FLASHCALW_FCMD_OFFSET       0x04         /**< \brief (FLASHCALW_FCMD offset) Flash Controller Command Register */
#define FLASHCALW_FCMD_RESETVALUE   0x00000000   /**< \brief (FLASHCALW_FCMD reset_value) Flash Controller Command Register */

#define FLASHCALW_FCMD_CMD_Pos      0            /**< \brief (FLASHCALW_FCMD) Command */
#define FLASHCALW_FCMD_CMD_Msk      (0x3Fu << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD(value)   ((FLASHCALW_FCMD_CMD_Msk & ((value) << FLASHCALW_FCMD_CMD_Pos)))
#define   FLASHCALW_FCMD_CMD_NOP_Val      0x0u   /**< \brief (FLASHCALW_FCMD) No Operation */
#define   FLASHCALW_FCMD_CMD_WP_Val       0x1u   /**< \brief (FLASHCALW_FCMD) Write Page */
#define   FLASHCALW_FCMD_CMD_EP_Val       0x2u   /**< \brief (FLASHCALW_FCMD) Erase Page */
#define   FLASHCALW_FCMD_CMD_CPB_Val      0x3u   /**< \brief (FLASHCALW_FCMD) Clear Page Buffer */
#define   FLASHCALW_FCMD_CMD_LP_Val       0x4u   /**< \brief (FLASHCALW_FCMD) Lock Region containing page */
#define   FLASHCALW_FCMD_CMD_UP_Val       0x5u   /**< \brief (FLASHCALW_FCMD) Unlock Region containing page */
#define   FLASHCALW_FCMD_CMD_EA_Val       0x6u   /**< \brief (FLASHCALW_FCMD) Erase All, including secuity and fuse bits */
#define   FLASHCALW_FCMD_CMD_WGPB_Val     0x7u   /**< \brief (FLASHCALW_FCMD) Write General-Purpose fuse Bit */
#define   FLASHCALW_FCMD_CMD_EGPB_Val     0x8u   /**< \brief (FLASHCALW_FCMD) Erase General-Purpose fuse Bit */
#define   FLASHCALW_FCMD_CMD_SSB_Val      0x9u   /**< \brief (FLASHCALW_FCMD) Set Security Bit */
#define   FLASHCALW_FCMD_CMD_PGPFB_Val    0xAu   /**< \brief (FLASHCALW_FCMD) Program GPFuse Byte */
#define   FLASHCALW_FCMD_CMD_EAGPF_Val    0xBu   /**< \brief (FLASHCALW_FCMD) Erase All GP Fuses */
#define   FLASHCALW_FCMD_CMD_QPR_Val      0xCu   /**< \brief (FLASHCALW_FCMD) Quick Page Read */
#define   FLASHCALW_FCMD_CMD_WUP_Val      0xDu   /**< \brief (FLASHCALW_FCMD) Write User Page */
#define   FLASHCALW_FCMD_CMD_EUP_Val      0xEu   /**< \brief (FLASHCALW_FCMD) Erase User Page */
#define   FLASHCALW_FCMD_CMD_QPRUP_Val    0xFu   /**< \brief (FLASHCALW_FCMD) Quick Page Read User Page */
#define   FLASHCALW_FCMD_CMD_HSEN_Val     0x10u   /**< \brief (FLASHCALW_FCMD) High Speed Mode Enable */
#define   FLASHCALW_FCMD_CMD_HSDIS_Val    0x11u   /**< \brief (FLASHCALW_FCMD) High Speed Mode Disable */
#define FLASHCALW_FCMD_CMD_NOP      (FLASHCALW_FCMD_CMD_NOP_Val    << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_WP       (FLASHCALW_FCMD_CMD_WP_Val     << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_EP       (FLASHCALW_FCMD_CMD_EP_Val     << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_CPB      (FLASHCALW_FCMD_CMD_CPB_Val    << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_LP       (FLASHCALW_FCMD_CMD_LP_Val     << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_UP       (FLASHCALW_FCMD_CMD_UP_Val     << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_EA       (FLASHCALW_FCMD_CMD_EA_Val     << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_WGPB     (FLASHCALW_FCMD_CMD_WGPB_Val   << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_EGPB     (FLASHCALW_FCMD_CMD_EGPB_Val   << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_SSB      (FLASHCALW_FCMD_CMD_SSB_Val    << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_PGPFB    (FLASHCALW_FCMD_CMD_PGPFB_Val  << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_EAGPF    (FLASHCALW_FCMD_CMD_EAGPF_Val  << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_QPR      (FLASHCALW_FCMD_CMD_QPR_Val    << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_WUP      (FLASHCALW_FCMD_CMD_WUP_Val    << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_EUP      (FLASHCALW_FCMD_CMD_EUP_Val    << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_QPRUP    (FLASHCALW_FCMD_CMD_QPRUP_Val  << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_HSEN     (FLASHCALW_FCMD_CMD_HSEN_Val   << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_CMD_HSDIS    (FLASHCALW_FCMD_CMD_HSDIS_Val  << FLASHCALW_FCMD_CMD_Pos)
#define FLASHCALW_FCMD_PAGEN_Pos    8            /**< \brief (FLASHCALW_FCMD) Page number */
#define FLASHCALW_FCMD_PAGEN_Msk    (0xFFFFu << FLASHCALW_FCMD_PAGEN_Pos)
#define FLASHCALW_FCMD_PAGEN(value) ((FLASHCALW_FCMD_PAGEN_Msk & ((value) << FLASHCALW_FCMD_PAGEN_Pos)))
#define FLASHCALW_FCMD_KEY_Pos      24           /**< \brief (FLASHCALW_FCMD) Write protection key */
#define FLASHCALW_FCMD_KEY_Msk      (0xFFu << FLASHCALW_FCMD_KEY_Pos)
#define FLASHCALW_FCMD_KEY(value)   ((FLASHCALW_FCMD_KEY_Msk & ((value) << FLASHCALW_FCMD_KEY_Pos)))
#define   FLASHCALW_FCMD_KEY_KEY_Val      0xA5u   /**< \brief (FLASHCALW_FCMD)  */
#define FLASHCALW_FCMD_KEY_KEY      (FLASHCALW_FCMD_KEY_KEY_Val    << FLASHCALW_FCMD_KEY_Pos)
#define FLASHCALW_FCMD_MASK         0xFFFFFF3Fu  /**< \brief (FLASHCALW_FCMD) MASK Register */

/* -------- FLASHCALW_FSR : (FLASHCALW Offset: 0x08) (R/W 32) Flash Controller Status Register -------- */
#define FLASHCALW_FSR_OFFSET        0x08         /**< \brief (FLASHCALW_FSR offset) Flash Controller Status Register */
#define FLASHCALW_FSR_RESETVALUE    0x00000000   /**< \brief (FLASHCALW_FSR reset_value) Flash Controller Status Register */

#define FLASHCALW_FSR_FRDY_Pos      0            /**< \brief (FLASHCALW_FSR) Flash Ready Status */
#define FLASHCALW_FSR_FRDY          (0x1u << FLASHCALW_FSR_FRDY_Pos)
#define FLASHCALW_FSR_LOCKE_Pos     2            /**< \brief (FLASHCALW_FSR) Lock Error Status */
#define FLASHCALW_FSR_LOCKE         (0x1u << FLASHCALW_FSR_LOCKE_Pos)
#define FLASHCALW_FSR_PROGE_Pos     3            /**< \brief (FLASHCALW_FSR) Programming Error Status */
#define FLASHCALW_FSR_PROGE         (0x1u << FLASHCALW_FSR_PROGE_Pos)
#define FLASHCALW_FSR_SECURITY_Pos  4            /**< \brief (FLASHCALW_FSR) Security Bit Status */
#define FLASHCALW_FSR_SECURITY      (0x1u << FLASHCALW_FSR_SECURITY_Pos)
#define FLASHCALW_FSR_QPRR_Pos      5            /**< \brief (FLASHCALW_FSR) Quick Page Read Result */
#define FLASHCALW_FSR_QPRR          (0x1u << FLASHCALW_FSR_QPRR_Pos)
#define FLASHCALW_FSR_HSMODE_Pos    6            /**< \brief (FLASHCALW_FSR) High Speed Mode */
#define FLASHCALW_FSR_HSMODE        (0x1u << FLASHCALW_FSR_HSMODE_Pos)
#define FLASHCALW_FSR_ECCERR_Pos    8            /**< \brief (FLASHCALW_FSR) ECC Error Status */
#define FLASHCALW_FSR_ECCERR_Msk    (0x3u << FLASHCALW_FSR_ECCERR_Pos)
#define FLASHCALW_FSR_ECCERR(value) ((FLASHCALW_FSR_ECCERR_Msk & ((value) << FLASHCALW_FSR_ECCERR_Pos)))
#define   FLASHCALW_FSR_ECCERR_NOERROR_Val 0x0u   /**< \brief (FLASHCALW_FSR) no error */
#define   FLASHCALW_FSR_ECCERR_ONEECCERR_Val 0x1u   /**< \brief (FLASHCALW_FSR) one ECC error detected */
#define   FLASHCALW_FSR_ECCERR_TWOECCERR_Val 0x2u   /**< \brief (FLASHCALW_FSR) two ECC errors detected */
#define FLASHCALW_FSR_ECCERR_NOERROR (FLASHCALW_FSR_ECCERR_NOERROR_Val << FLASHCALW_FSR_ECCERR_Pos)
#define FLASHCALW_FSR_ECCERR_ONEECCERR (FLASHCALW_FSR_ECCERR_ONEECCERR_Val << FLASHCALW_FSR_ECCERR_Pos)
#define FLASHCALW_FSR_ECCERR_TWOECCERR (FLASHCALW_FSR_ECCERR_TWOECCERR_Val << FLASHCALW_FSR_ECCERR_Pos)
#define FLASHCALW_FSR_LOCK0_Pos     16           /**< \brief (FLASHCALW_FSR) Lock Region 0 Lock Status */
#define FLASHCALW_FSR_LOCK0         (0x1u << FLASHCALW_FSR_LOCK0_Pos)
#define FLASHCALW_FSR_LOCK1_Pos     17           /**< \brief (FLASHCALW_FSR) Lock Region 1 Lock Status */
#define FLASHCALW_FSR_LOCK1         (0x1u << FLASHCALW_FSR_LOCK1_Pos)
#define FLASHCALW_FSR_LOCK2_Pos     18           /**< \brief (FLASHCALW_FSR) Lock Region 2 Lock Status */
#define FLASHCALW_FSR_LOCK2         (0x1u << FLASHCALW_FSR_LOCK2_Pos)
#define FLASHCALW_FSR_LOCK3_Pos     19           /**< \brief (FLASHCALW_FSR) Lock Region 3 Lock Status */
#define FLASHCALW_FSR_LOCK3         (0x1u << FLASHCALW_FSR_LOCK3_Pos)
#define FLASHCALW_FSR_LOCK4_Pos     20           /**< \brief (FLASHCALW_FSR) Lock Region 4 Lock Status */
#define FLASHCALW_FSR_LOCK4         (0x1u << FLASHCALW_FSR_LOCK4_Pos)
#define FLASHCALW_FSR_LOCK5_Pos     21           /**< \brief (FLASHCALW_FSR) Lock Region 5 Lock Status */
#define FLASHCALW_FSR_LOCK5         (0x1u << FLASHCALW_FSR_LOCK5_Pos)
#define FLASHCALW_FSR_LOCK6_Pos     22           /**< \brief (FLASHCALW_FSR) Lock Region 6 Lock Status */
#define FLASHCALW_FSR_LOCK6         (0x1u << FLASHCALW_FSR_LOCK6_Pos)
#define FLASHCALW_FSR_LOCK7_Pos     23           /**< \brief (FLASHCALW_FSR) Lock Region 7 Lock Status */
#define FLASHCALW_FSR_LOCK7         (0x1u << FLASHCALW_FSR_LOCK7_Pos)
#define FLASHCALW_FSR_LOCK8_Pos     24           /**< \brief (FLASHCALW_FSR) Lock Region 8 Lock Status */
#define FLASHCALW_FSR_LOCK8         (0x1u << FLASHCALW_FSR_LOCK8_Pos)
#define FLASHCALW_FSR_LOCK9_Pos     25           /**< \brief (FLASHCALW_FSR) Lock Region 9 Lock Status */
#define FLASHCALW_FSR_LOCK9         (0x1u << FLASHCALW_FSR_LOCK9_Pos)
#define FLASHCALW_FSR_LOCK10_Pos    26           /**< \brief (FLASHCALW_FSR) Lock Region 10 Lock Status */
#define FLASHCALW_FSR_LOCK10        (0x1u << FLASHCALW_FSR_LOCK10_Pos)
#define FLASHCALW_FSR_LOCK11_Pos    27           /**< \brief (FLASHCALW_FSR) Lock Region 11 Lock Status */
#define FLASHCALW_FSR_LOCK11        (0x1u << FLASHCALW_FSR_LOCK11_Pos)
#define FLASHCALW_FSR_LOCK12_Pos    28           /**< \brief (FLASHCALW_FSR) Lock Region 12 Lock Status */
#define FLASHCALW_FSR_LOCK12        (0x1u << FLASHCALW_FSR_LOCK12_Pos)
#define FLASHCALW_FSR_LOCK13_Pos    29           /**< \brief (FLASHCALW_FSR) Lock Region 13 Lock Status */
#define FLASHCALW_FSR_LOCK13        (0x1u << FLASHCALW_FSR_LOCK13_Pos)
#define FLASHCALW_FSR_LOCK14_Pos    30           /**< \brief (FLASHCALW_FSR) Lock Region 14 Lock Status */
#define FLASHCALW_FSR_LOCK14        (0x1u << FLASHCALW_FSR_LOCK14_Pos)
#define FLASHCALW_FSR_LOCK15_Pos    31           /**< \brief (FLASHCALW_FSR) Lock Region 15 Lock Status */
#define FLASHCALW_FSR_LOCK15        (0x1u << FLASHCALW_FSR_LOCK15_Pos)
#define FLASHCALW_FSR_MASK          0xFFFF037Du  /**< \brief (FLASHCALW_FSR) MASK Register */

/* -------- FLASHCALW_FPR : (FLASHCALW Offset: 0x0C) (R/  32) Flash Controller Parameter Register -------- */
#define FLASHCALW_FPR_OFFSET        0x0C         /**< \brief (FLASHCALW_FPR offset) Flash Controller Parameter Register */

#define FLASHCALW_FPR_FSZ_Pos       0            /**< \brief (FLASHCALW_FPR) Flash Size */
#define FLASHCALW_FPR_FSZ_Msk       (0xFu << FLASHCALW_FPR_FSZ_Pos)
#define FLASHCALW_FPR_FSZ(value)    ((FLASHCALW_FPR_FSZ_Msk & ((value) << FLASHCALW_FPR_FSZ_Pos)))
#define FLASHCALW_FPR_PSZ_Pos       8            /**< \brief (FLASHCALW_FPR) Page Size */
#define FLASHCALW_FPR_PSZ_Msk       (0x7u << FLASHCALW_FPR_PSZ_Pos)
#define FLASHCALW_FPR_PSZ(value)    ((FLASHCALW_FPR_PSZ_Msk & ((value) << FLASHCALW_FPR_PSZ_Pos)))
#define FLASHCALW_FPR_MASK          0x0000070Fu  /**< \brief (FLASHCALW_FPR) MASK Register */

/* -------- FLASHCALW_VERSION : (FLASHCALW Offset: 0x10) (R/  32) Flash Controller Version Register -------- */
#define FLASHCALW_VERSION_OFFSET    0x10         /**< \brief (FLASHCALW_VERSION offset) Flash Controller Version Register */
#define FLASHCALW_VERSION_RESETVALUE 0x00000110   /**< \brief (FLASHCALW_VERSION reset_value) Flash Controller Version Register */

#define FLASHCALW_VERSION_VERSION_Pos 0            /**< \brief (FLASHCALW_VERSION) Version Number */
#define FLASHCALW_VERSION_VERSION_Msk (0xFFFu << FLASHCALW_VERSION_VERSION_Pos)
#define FLASHCALW_VERSION_VERSION(value) ((FLASHCALW_VERSION_VERSION_Msk & ((value) << FLASHCALW_VERSION_VERSION_Pos)))
#define FLASHCALW_VERSION_VARIANT_Pos 16           /**< \brief (FLASHCALW_VERSION) Variant Number */
#define FLASHCALW_VERSION_VARIANT_Msk (0xFu << FLASHCALW_VERSION_VARIANT_Pos)
#define FLASHCALW_VERSION_VARIANT(value) ((FLASHCALW_VERSION_VARIANT_Msk & ((value) << FLASHCALW_VERSION_VARIANT_Pos)))
#define FLASHCALW_VERSION_MASK      0x000F0FFFu  /**< \brief (FLASHCALW_VERSION) MASK Register */

/* -------- FLASHCALW_FGPFRHI : (FLASHCALW Offset: 0x14) (R/W 32) Flash Controller General Purpose Fuse Register High -------- */
#define FLASHCALW_FGPFRHI_OFFSET    0x14         /**< \brief (FLASHCALW_FGPFRHI offset) Flash Controller General Purpose Fuse Register High */

#define FLASHCALW_FGPFRHI_GPF32_Pos 0            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 32 */
#define FLASHCALW_FGPFRHI_GPF32     (0x1u << FLASHCALW_FGPFRHI_GPF32_Pos)
#define FLASHCALW_FGPFRHI_GPF33_Pos 1            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 33 */
#define FLASHCALW_FGPFRHI_GPF33     (0x1u << FLASHCALW_FGPFRHI_GPF33_Pos)
#define FLASHCALW_FGPFRHI_GPF34_Pos 2            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 34 */
#define FLASHCALW_FGPFRHI_GPF34     (0x1u << FLASHCALW_FGPFRHI_GPF34_Pos)
#define FLASHCALW_FGPFRHI_GPF35_Pos 3            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 35 */
#define FLASHCALW_FGPFRHI_GPF35     (0x1u << FLASHCALW_FGPFRHI_GPF35_Pos)
#define FLASHCALW_FGPFRHI_GPF36_Pos 4            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 36 */
#define FLASHCALW_FGPFRHI_GPF36     (0x1u << FLASHCALW_FGPFRHI_GPF36_Pos)
#define FLASHCALW_FGPFRHI_GPF37_Pos 5            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 37 */
#define FLASHCALW_FGPFRHI_GPF37     (0x1u << FLASHCALW_FGPFRHI_GPF37_Pos)
#define FLASHCALW_FGPFRHI_GPF38_Pos 6            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 38 */
#define FLASHCALW_FGPFRHI_GPF38     (0x1u << FLASHCALW_FGPFRHI_GPF38_Pos)
#define FLASHCALW_FGPFRHI_GPF39_Pos 7            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 39 */
#define FLASHCALW_FGPFRHI_GPF39     (0x1u << FLASHCALW_FGPFRHI_GPF39_Pos)
#define FLASHCALW_FGPFRHI_GPF40_Pos 8            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 40 */
#define FLASHCALW_FGPFRHI_GPF40     (0x1u << FLASHCALW_FGPFRHI_GPF40_Pos)
#define FLASHCALW_FGPFRHI_GPF41_Pos 9            /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 41 */
#define FLASHCALW_FGPFRHI_GPF41     (0x1u << FLASHCALW_FGPFRHI_GPF41_Pos)
#define FLASHCALW_FGPFRHI_GPF42_Pos 10           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 42 */
#define FLASHCALW_FGPFRHI_GPF42     (0x1u << FLASHCALW_FGPFRHI_GPF42_Pos)
#define FLASHCALW_FGPFRHI_GPF43_Pos 11           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 43 */
#define FLASHCALW_FGPFRHI_GPF43     (0x1u << FLASHCALW_FGPFRHI_GPF43_Pos)
#define FLASHCALW_FGPFRHI_GPF44_Pos 12           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 44 */
#define FLASHCALW_FGPFRHI_GPF44     (0x1u << FLASHCALW_FGPFRHI_GPF44_Pos)
#define FLASHCALW_FGPFRHI_GPF45_Pos 13           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 45 */
#define FLASHCALW_FGPFRHI_GPF45     (0x1u << FLASHCALW_FGPFRHI_GPF45_Pos)
#define FLASHCALW_FGPFRHI_GPF46_Pos 14           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 46 */
#define FLASHCALW_FGPFRHI_GPF46     (0x1u << FLASHCALW_FGPFRHI_GPF46_Pos)
#define FLASHCALW_FGPFRHI_GPF47_Pos 15           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 47 */
#define FLASHCALW_FGPFRHI_GPF47     (0x1u << FLASHCALW_FGPFRHI_GPF47_Pos)
#define FLASHCALW_FGPFRHI_GPF48_Pos 16           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 48 */
#define FLASHCALW_FGPFRHI_GPF48     (0x1u << FLASHCALW_FGPFRHI_GPF48_Pos)
#define FLASHCALW_FGPFRHI_GPF49_Pos 17           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 49 */
#define FLASHCALW_FGPFRHI_GPF49     (0x1u << FLASHCALW_FGPFRHI_GPF49_Pos)
#define FLASHCALW_FGPFRHI_GPF50_Pos 18           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 50 */
#define FLASHCALW_FGPFRHI_GPF50     (0x1u << FLASHCALW_FGPFRHI_GPF50_Pos)
#define FLASHCALW_FGPFRHI_GPF51_Pos 19           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 51 */
#define FLASHCALW_FGPFRHI_GPF51     (0x1u << FLASHCALW_FGPFRHI_GPF51_Pos)
#define FLASHCALW_FGPFRHI_GPF52_Pos 20           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 52 */
#define FLASHCALW_FGPFRHI_GPF52     (0x1u << FLASHCALW_FGPFRHI_GPF52_Pos)
#define FLASHCALW_FGPFRHI_GPF53_Pos 21           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 53 */
#define FLASHCALW_FGPFRHI_GPF53     (0x1u << FLASHCALW_FGPFRHI_GPF53_Pos)
#define FLASHCALW_FGPFRHI_GPF54_Pos 22           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 54 */
#define FLASHCALW_FGPFRHI_GPF54     (0x1u << FLASHCALW_FGPFRHI_GPF54_Pos)
#define FLASHCALW_FGPFRHI_GPF55_Pos 23           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 55 */
#define FLASHCALW_FGPFRHI_GPF55     (0x1u << FLASHCALW_FGPFRHI_GPF55_Pos)
#define FLASHCALW_FGPFRHI_GPF56_Pos 24           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 56 */
#define FLASHCALW_FGPFRHI_GPF56     (0x1u << FLASHCALW_FGPFRHI_GPF56_Pos)
#define FLASHCALW_FGPFRHI_GPF57_Pos 25           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 57 */
#define FLASHCALW_FGPFRHI_GPF57     (0x1u << FLASHCALW_FGPFRHI_GPF57_Pos)
#define FLASHCALW_FGPFRHI_GPF58_Pos 26           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 58 */
#define FLASHCALW_FGPFRHI_GPF58     (0x1u << FLASHCALW_FGPFRHI_GPF58_Pos)
#define FLASHCALW_FGPFRHI_GPF59_Pos 27           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 59 */
#define FLASHCALW_FGPFRHI_GPF59     (0x1u << FLASHCALW_FGPFRHI_GPF59_Pos)
#define FLASHCALW_FGPFRHI_GPF60_Pos 28           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 60 */
#define FLASHCALW_FGPFRHI_GPF60     (0x1u << FLASHCALW_FGPFRHI_GPF60_Pos)
#define FLASHCALW_FGPFRHI_GPF61_Pos 29           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 61 */
#define FLASHCALW_FGPFRHI_GPF61     (0x1u << FLASHCALW_FGPFRHI_GPF61_Pos)
#define FLASHCALW_FGPFRHI_GPF62_Pos 30           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 62 */
#define FLASHCALW_FGPFRHI_GPF62     (0x1u << FLASHCALW_FGPFRHI_GPF62_Pos)
#define FLASHCALW_FGPFRHI_GPF63_Pos 31           /**< \brief (FLASHCALW_FGPFRHI) General Purpose Fuse 63 */
#define FLASHCALW_FGPFRHI_GPF63     (0x1u << FLASHCALW_FGPFRHI_GPF63_Pos)
#define FLASHCALW_FGPFRHI_MASK      0xFFFFFFFFu  /**< \brief (FLASHCALW_FGPFRHI) MASK Register */

/* -------- FLASHCALW_FGPFRLO : (FLASHCALW Offset: 0x18) (R/W 32) Flash Controller General Purpose Fuse Register Low -------- */
#define FLASHCALW_FGPFRLO_OFFSET    0x18         /**< \brief (FLASHCALW_FGPFRLO offset) Flash Controller General Purpose Fuse Register Low */

#define FLASHCALW_FGPFRLO_LOCK0_Pos 0            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 0 */
#define FLASHCALW_FGPFRLO_LOCK0     (0x1u << FLASHCALW_FGPFRLO_LOCK0_Pos)
#define FLASHCALW_FGPFRLO_LOCK1_Pos 1            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 1 */
#define FLASHCALW_FGPFRLO_LOCK1     (0x1u << FLASHCALW_FGPFRLO_LOCK1_Pos)
#define FLASHCALW_FGPFRLO_LOCK2_Pos 2            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 2 */
#define FLASHCALW_FGPFRLO_LOCK2     (0x1u << FLASHCALW_FGPFRLO_LOCK2_Pos)
#define FLASHCALW_FGPFRLO_LOCK3_Pos 3            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 3 */
#define FLASHCALW_FGPFRLO_LOCK3     (0x1u << FLASHCALW_FGPFRLO_LOCK3_Pos)
#define FLASHCALW_FGPFRLO_LOCK4_Pos 4            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 4 */
#define FLASHCALW_FGPFRLO_LOCK4     (0x1u << FLASHCALW_FGPFRLO_LOCK4_Pos)
#define FLASHCALW_FGPFRLO_LOCK5_Pos 5            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 5 */
#define FLASHCALW_FGPFRLO_LOCK5     (0x1u << FLASHCALW_FGPFRLO_LOCK5_Pos)
#define FLASHCALW_FGPFRLO_LOCK6_Pos 6            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 6 */
#define FLASHCALW_FGPFRLO_LOCK6     (0x1u << FLASHCALW_FGPFRLO_LOCK6_Pos)
#define FLASHCALW_FGPFRLO_LOCK7_Pos 7            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 7 */
#define FLASHCALW_FGPFRLO_LOCK7     (0x1u << FLASHCALW_FGPFRLO_LOCK7_Pos)
#define FLASHCALW_FGPFRLO_LOCK8_Pos 8            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 8 */
#define FLASHCALW_FGPFRLO_LOCK8     (0x1u << FLASHCALW_FGPFRLO_LOCK8_Pos)
#define FLASHCALW_FGPFRLO_LOCK9_Pos 9            /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 9 */
#define FLASHCALW_FGPFRLO_LOCK9     (0x1u << FLASHCALW_FGPFRLO_LOCK9_Pos)
#define FLASHCALW_FGPFRLO_LOCK10_Pos 10           /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 10 */
#define FLASHCALW_FGPFRLO_LOCK10    (0x1u << FLASHCALW_FGPFRLO_LOCK10_Pos)
#define FLASHCALW_FGPFRLO_LOCK11_Pos 11           /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 11 */
#define FLASHCALW_FGPFRLO_LOCK11    (0x1u << FLASHCALW_FGPFRLO_LOCK11_Pos)
#define FLASHCALW_FGPFRLO_LOCK12_Pos 12           /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 12 */
#define FLASHCALW_FGPFRLO_LOCK12    (0x1u << FLASHCALW_FGPFRLO_LOCK12_Pos)
#define FLASHCALW_FGPFRLO_LOCK13_Pos 13           /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 13 */
#define FLASHCALW_FGPFRLO_LOCK13    (0x1u << FLASHCALW_FGPFRLO_LOCK13_Pos)
#define FLASHCALW_FGPFRLO_LOCK14_Pos 14           /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 14 */
#define FLASHCALW_FGPFRLO_LOCK14    (0x1u << FLASHCALW_FGPFRLO_LOCK14_Pos)
#define FLASHCALW_FGPFRLO_LOCK15_Pos 15           /**< \brief (FLASHCALW_FGPFRLO) Lock Bit 15 */
#define FLASHCALW_FGPFRLO_LOCK15    (0x1u << FLASHCALW_FGPFRLO_LOCK15_Pos)
#define FLASHCALW_FGPFRLO_GPF16_Pos 16           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 16 */
#define FLASHCALW_FGPFRLO_GPF16     (0x1u << FLASHCALW_FGPFRLO_GPF16_Pos)
#define FLASHCALW_FGPFRLO_GPF17_Pos 17           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 17 */
#define FLASHCALW_FGPFRLO_GPF17     (0x1u << FLASHCALW_FGPFRLO_GPF17_Pos)
#define FLASHCALW_FGPFRLO_GPF18_Pos 18           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 18 */
#define FLASHCALW_FGPFRLO_GPF18     (0x1u << FLASHCALW_FGPFRLO_GPF18_Pos)
#define FLASHCALW_FGPFRLO_GPF19_Pos 19           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 19 */
#define FLASHCALW_FGPFRLO_GPF19     (0x1u << FLASHCALW_FGPFRLO_GPF19_Pos)
#define FLASHCALW_FGPFRLO_GPF20_Pos 20           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 20 */
#define FLASHCALW_FGPFRLO_GPF20     (0x1u << FLASHCALW_FGPFRLO_GPF20_Pos)
#define FLASHCALW_FGPFRLO_GPF21_Pos 21           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 21 */
#define FLASHCALW_FGPFRLO_GPF21     (0x1u << FLASHCALW_FGPFRLO_GPF21_Pos)
#define FLASHCALW_FGPFRLO_GPF22_Pos 22           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 22 */
#define FLASHCALW_FGPFRLO_GPF22     (0x1u << FLASHCALW_FGPFRLO_GPF22_Pos)
#define FLASHCALW_FGPFRLO_GPF23_Pos 23           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 23 */
#define FLASHCALW_FGPFRLO_GPF23     (0x1u << FLASHCALW_FGPFRLO_GPF23_Pos)
#define FLASHCALW_FGPFRLO_GPF24_Pos 24           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 24 */
#define FLASHCALW_FGPFRLO_GPF24     (0x1u << FLASHCALW_FGPFRLO_GPF24_Pos)
#define FLASHCALW_FGPFRLO_GPF25_Pos 25           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 25 */
#define FLASHCALW_FGPFRLO_GPF25     (0x1u << FLASHCALW_FGPFRLO_GPF25_Pos)
#define FLASHCALW_FGPFRLO_GPF26_Pos 26           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 26 */
#define FLASHCALW_FGPFRLO_GPF26     (0x1u << FLASHCALW_FGPFRLO_GPF26_Pos)
#define FLASHCALW_FGPFRLO_GPF27_Pos 27           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 27 */
#define FLASHCALW_FGPFRLO_GPF27     (0x1u << FLASHCALW_FGPFRLO_GPF27_Pos)
#define FLASHCALW_FGPFRLO_GPF28_Pos 28           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 28 */
#define FLASHCALW_FGPFRLO_GPF28     (0x1u << FLASHCALW_FGPFRLO_GPF28_Pos)
#define FLASHCALW_FGPFRLO_GPF29_Pos 29           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 29 */
#define FLASHCALW_FGPFRLO_GPF29     (0x1u << FLASHCALW_FGPFRLO_GPF29_Pos)
#define FLASHCALW_FGPFRLO_GPF30_Pos 30           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 30 */
#define FLASHCALW_FGPFRLO_GPF30     (0x1u << FLASHCALW_FGPFRLO_GPF30_Pos)
#define FLASHCALW_FGPFRLO_GPF31_Pos 31           /**< \brief (FLASHCALW_FGPFRLO) General Purpose Fuse 31 */
#define FLASHCALW_FGPFRLO_GPF31     (0x1u << FLASHCALW_FGPFRLO_GPF31_Pos)
#define FLASHCALW_FGPFRLO_MASK      0xFFFFFFFFu  /**< \brief (FLASHCALW_FGPFRLO) MASK Register */

/** \brief FLASHCALW APB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RwReg   FLASHCALW_FCR;      /**< \brief (FLASHCALW Offset: 0x00) Flash Controller Control Register */
  RwReg   FLASHCALW_FCMD;     /**< \brief (FLASHCALW Offset: 0x04) Flash Controller Command Register */
  RwReg   FLASHCALW_FSR;      /**< \brief (FLASHCALW Offset: 0x08) Flash Controller Status Register */
  RoReg   FLASHCALW_FPR;      /**< \brief (FLASHCALW Offset: 0x0C) Flash Controller Parameter Register */
  RoReg   FLASHCALW_VERSION;  /**< \brief (FLASHCALW Offset: 0x10) Flash Controller Version Register */
  RwReg   FLASHCALW_FGPFRHI;  /**< \brief (FLASHCALW Offset: 0x14) Flash Controller General Purpose Fuse Register High */
  RwReg   FLASHCALW_FGPFRLO;  /**< \brief (FLASHCALW Offset: 0x18) Flash Controller General Purpose Fuse Register Low */
} Flashcalw;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
#define SECTION_FLASHCALW_FROW
#define SECTION_FLASHCALW_USER

/*@}*/

/* ************************************************************************** */
/**  SOFTWARE PERIPHERAL API DEFINITION FOR NON-VOLATILE FUSES */
/* ************************************************************************** */
/** \addtogroup fuses_api Peripheral Software API */
/*@{*/


#define FUSES_BOD18ACTION_ADDR      HFLASHC_USER
#define FUSES_BOD18ACTION_Pos       18           /**< \brief (HFLASHC_USER) BOD18 Action */
#define FUSES_BOD18ACTION_Msk       (0x3u << FUSES_BOD18ACTION_Pos)
#define FUSES_BOD18ACTION(value)    ((FUSES_BOD18ACTION_Msk & ((value) << FUSES_BOD18ACTION_Pos)))

#define FUSES_BOD18EN_ADDR          HFLASHC_USER
#define FUSES_BOD18EN_Pos           17           /**< \brief (HFLASHC_USER) BOD18 Enable */
#define FUSES_BOD18EN_Msk           (0x1u << FUSES_BOD18EN_Pos)

#define FUSES_BOD18HYST_ADDR        HFLASHC_USER
#define FUSES_BOD18HYST_Pos         20           /**< \brief (HFLASHC_USER) BOD18 Hysteresis */
#define FUSES_BOD18HYST_Msk         (0x1u << FUSES_BOD18HYST_Pos)

#define FUSES_BOD18LV_ADDR          HFLASHC_USER
#define FUSES_BOD18LV_Pos           11           /**< \brief (HFLASHC_USER) BOD18 Level */
#define FUSES_BOD18LV_Msk           (0x3Fu << FUSES_BOD18LV_Pos)
#define FUSES_BOD18LV(value)        ((FUSES_BOD18LV_Msk & ((value) << FUSES_BOD18LV_Pos)))

#define FUSES_BOD33ACTION_ADDR      HFLASHC_USER
#define FUSES_BOD33ACTION_Pos       8            /**< \brief (HFLASHC_USER) BOD33 Action */
#define FUSES_BOD33ACTION_Msk       (0x3u << FUSES_BOD33ACTION_Pos)
#define FUSES_BOD33ACTION(value)    ((FUSES_BOD33ACTION_Msk & ((value) << FUSES_BOD33ACTION_Pos)))

#define FUSES_BOD33EN_ADDR          HFLASHC_USER
#define FUSES_BOD33EN_Pos           7            /**< \brief (HFLASHC_USER) BOD33 Enable */
#define FUSES_BOD33EN_Msk           (0x1u << FUSES_BOD33EN_Pos)

#define FUSES_BOD33LEVEL_ADDR       HFLASHC_USER
#define FUSES_BOD33LEVEL_Pos        1            /**< \brief (HFLASHC_USER) BOD33 Level */
#define FUSES_BOD33LEVEL_Msk        (0x3Fu << FUSES_BOD33LEVEL_Pos)
#define FUSES_BOD33LEVEL(value)     ((FUSES_BOD33LEVEL_Msk & ((value) << FUSES_BOD33LEVEL_Pos)))

#define FUSES_BOD33_HYST_ADDR       HFLASHC_USER
#define FUSES_BOD33_HYST_Pos        10           /**< \brief (HFLASHC_USER) BOD33 Hysteresis */
#define FUSES_BOD33_HYST_Msk        (0x1u << FUSES_BOD33_HYST_Pos)

#define WDT_FUSES_WDTAUTO_ADDR      HFLASHC_USER
#define WDT_FUSES_WDTAUTO_Pos       0            /**< \brief (HFLASHC_USER) Watchdog Enabled at Reset */
#define WDT_FUSES_WDTAUTO_Msk       (0x1u << WDT_FUSES_WDTAUTO_Pos)

/*@}*/

#endif /* _SAM4L_FLASHCALW_COMPONENT_ */
