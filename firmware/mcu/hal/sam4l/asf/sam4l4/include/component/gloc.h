/**
 * \file
 *
 * \brief Component description for GLOC
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

#ifndef _SAM4L_GLOC_COMPONENT_
#define _SAM4L_GLOC_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR GLOC */
/* ========================================================================== */
/** \addtogroup SAM4L_GLOC Glue Logic Controller */
/*@{*/

#define GLOC_I7551
#define REV_GLOC                    0x102

/* -------- GLOC_CR : (GLOC Offset: 0x00) (R/W 32) LUT Control Register -------- */
#define GLOC_CR_OFFSET              0x00         /**< \brief (GLOC_CR offset) Control Register */
#define GLOC_CR_RESETVALUE          0x00000000   /**< \brief (GLOC_CR reset_value) Control Register */

#define GLOC_CR_AEN_Pos             0            /**< \brief (GLOC_CR) Input mask */
#define GLOC_CR_AEN_Msk             (0xFu << GLOC_CR_AEN_Pos)
#define GLOC_CR_AEN(value)          ((GLOC_CR_AEN_Msk & ((value) << GLOC_CR_AEN_Pos)))
#define GLOC_CR_FILTEN_Pos          31           /**< \brief (GLOC_CR) Filter enable */
#define GLOC_CR_FILTEN              (0x1u << GLOC_CR_FILTEN_Pos)
#define GLOC_CR_MASK                0x8000000Fu  /**< \brief (GLOC_CR) MASK Register */

/* -------- GLOC_TRUTH : (GLOC Offset: 0x04) (R/W 32) LUT Truth Register -------- */
#define GLOC_TRUTH_OFFSET           0x04         /**< \brief (GLOC_TRUTH offset) Truth Register */
#define GLOC_TRUTH_RESETVALUE       0x00000000   /**< \brief (GLOC_TRUTH reset_value) Truth Register */

#define GLOC_TRUTH_TRUTH_Pos        0            /**< \brief (GLOC_TRUTH) Truth */
#define GLOC_TRUTH_TRUTH_Msk        (0xFFFFu << GLOC_TRUTH_TRUTH_Pos)
#define GLOC_TRUTH_TRUTH(value)     ((GLOC_TRUTH_TRUTH_Msk & ((value) << GLOC_TRUTH_TRUTH_Pos)))
#define GLOC_TRUTH_MASK             0x0000FFFFu  /**< \brief (GLOC_TRUTH) MASK Register */

/* -------- GLOC_PARAMETER : (GLOC Offset: 0x38) (R/  32) Parameter Register -------- */
#define GLOC_PARAMETER_OFFSET       0x38         /**< \brief (GLOC_PARAMETER offset) Parameter Register */

#define GLOC_PARAMETER_LUTS_Pos     0            /**< \brief (GLOC_PARAMETER) LUTs */
#define GLOC_PARAMETER_LUTS_Msk     (0xFFu << GLOC_PARAMETER_LUTS_Pos)
#define GLOC_PARAMETER_LUTS(value)  ((GLOC_PARAMETER_LUTS_Msk & ((value) << GLOC_PARAMETER_LUTS_Pos)))
#define GLOC_PARAMETER_MASK         0x000000FFu  /**< \brief (GLOC_PARAMETER) MASK Register */

/* -------- GLOC_VERSION : (GLOC Offset: 0x3C) (R/  32) Version Register -------- */
#define GLOC_VERSION_OFFSET         0x3C         /**< \brief (GLOC_VERSION offset) Version Register */
#define GLOC_VERSION_RESETVALUE     0x00000102   /**< \brief (GLOC_VERSION reset_value) Version Register */

#define GLOC_VERSION_VERSION_Pos    0            /**< \brief (GLOC_VERSION) Version */
#define GLOC_VERSION_VERSION_Msk    (0xFFFu << GLOC_VERSION_VERSION_Pos)
#define GLOC_VERSION_VERSION(value) ((GLOC_VERSION_VERSION_Msk & ((value) << GLOC_VERSION_VERSION_Pos)))
#define GLOC_VERSION_VARIANT_Pos    16           /**< \brief (GLOC_VERSION) Variant */
#define GLOC_VERSION_VARIANT_Msk    (0xFu << GLOC_VERSION_VARIANT_Pos)
#define GLOC_VERSION_VARIANT(value) ((GLOC_VERSION_VARIANT_Msk & ((value) << GLOC_VERSION_VARIANT_Pos)))
#define GLOC_VERSION_MASK           0x000F0FFFu  /**< \brief (GLOC_VERSION) MASK Register */

/** \brief GlocLut hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RwReg   GLOC_CR;            /**< \brief (GLOC Offset: 0x00) Control Register */
  RwReg   GLOC_TRUTH;         /**< \brief (GLOC Offset: 0x04) Truth Register */
} GlocLut;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief GLOC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  GlocLut GLOC_LUT[2];        /**< \brief (GLOC Offset: 0x00) GlocLut groups [LUTS] */
  RoReg8  Reserved1[0x28];
  RoReg   GLOC_PARAMETER;     /**< \brief (GLOC Offset: 0x38) Parameter Register */
  RoReg   GLOC_VERSION;       /**< \brief (GLOC Offset: 0x3C) Version Register */
} Gloc;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAM4L_GLOC_COMPONENT_ */
