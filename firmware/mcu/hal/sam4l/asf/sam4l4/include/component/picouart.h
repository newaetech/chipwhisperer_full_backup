/**
 * \file
 *
 * \brief Component description for PICOUART
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

#ifndef _SAM4L_PICOUART_COMPONENT_
#define _SAM4L_PICOUART_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PICOUART */
/* ========================================================================== */
/** \addtogroup SAM4L_PICOUART Pico UART */
/*@{*/

#define PICOUART_I8403
#define REV_PICOUART                0x101

/* -------- PICOUART_CR : (PICOUART Offset: 0x00) ( /W 32) Control Register -------- */
#define PICOUART_CR_OFFSET          0x00         /**< \brief (PICOUART_CR offset) Control Register */
#define PICOUART_CR_RESETVALUE      0x00000000   /**< \brief (PICOUART_CR reset_value) Control Register */

#define PICOUART_CR_EN_Pos          0            /**< \brief (PICOUART_CR) Enable */
#define PICOUART_CR_EN              (0x1u << PICOUART_CR_EN_Pos)
#define PICOUART_CR_DIS_Pos         1            /**< \brief (PICOUART_CR) Disable */
#define PICOUART_CR_DIS             (0x1u << PICOUART_CR_DIS_Pos)
#define PICOUART_CR_MASK            0x00000003u  /**< \brief (PICOUART_CR) MASK Register */

/* -------- PICOUART_CFG : (PICOUART Offset: 0x04) (R/W 32) Configuration Register -------- */
#define PICOUART_CFG_OFFSET         0x04         /**< \brief (PICOUART_CFG offset) Configuration Register */
#define PICOUART_CFG_RESETVALUE     0x00000000   /**< \brief (PICOUART_CFG reset_value) Configuration Register */

#define PICOUART_CFG_SOURCE_Pos     0            /**< \brief (PICOUART_CFG) Source Enable Mode */
#define PICOUART_CFG_SOURCE_Msk     (0x3u << PICOUART_CFG_SOURCE_Pos)
#define PICOUART_CFG_SOURCE(value)  ((PICOUART_CFG_SOURCE_Msk & ((value) << PICOUART_CFG_SOURCE_Pos)))
#define PICOUART_CFG_ACTION_Pos     2            /**< \brief (PICOUART_CFG) Action to perform */
#define PICOUART_CFG_ACTION         (0x1u << PICOUART_CFG_ACTION_Pos)
#define PICOUART_CFG_MATCH_Pos      8            /**< \brief (PICOUART_CFG) Data Match */
#define PICOUART_CFG_MATCH_Msk      (0xFFu << PICOUART_CFG_MATCH_Pos)
#define PICOUART_CFG_MATCH(value)   ((PICOUART_CFG_MATCH_Msk & ((value) << PICOUART_CFG_MATCH_Pos)))
#define PICOUART_CFG_MASK           0x0000FF07u  /**< \brief (PICOUART_CFG) MASK Register */

/* -------- PICOUART_SR : (PICOUART Offset: 0x08) (R/  32) Status Register -------- */
#define PICOUART_SR_OFFSET          0x08         /**< \brief (PICOUART_SR offset) Status Register */
#define PICOUART_SR_RESETVALUE      0x00000000   /**< \brief (PICOUART_SR reset_value) Status Register */

#define PICOUART_SR_EN_Pos          0            /**< \brief (PICOUART_SR) Enable Interrupt Status */
#define PICOUART_SR_EN              (0x1u << PICOUART_SR_EN_Pos)
#define PICOUART_SR_DRDY_Pos        1            /**< \brief (PICOUART_SR) Data Ready Interrupt Status */
#define PICOUART_SR_DRDY            (0x1u << PICOUART_SR_DRDY_Pos)
#define PICOUART_SR_MASK            0x00000003u  /**< \brief (PICOUART_SR) MASK Register */

/* -------- PICOUART_RHR : (PICOUART Offset: 0x0C) (R/  32) Receive Holding Register -------- */
#define PICOUART_RHR_OFFSET         0x0C         /**< \brief (PICOUART_RHR offset) Receive Holding Register */
#define PICOUART_RHR_RESETVALUE     0x00000000   /**< \brief (PICOUART_RHR reset_value) Receive Holding Register */

#define PICOUART_RHR_CDATA_Pos      0            /**< \brief (PICOUART_RHR) Received Data */
#define PICOUART_RHR_CDATA_Msk      (0xFFFFFFFFu << PICOUART_RHR_CDATA_Pos)
#define PICOUART_RHR_CDATA(value)   ((PICOUART_RHR_CDATA_Msk & ((value) << PICOUART_RHR_CDATA_Pos)))
#define PICOUART_RHR_MASK           0xFFFFFFFFu  /**< \brief (PICOUART_RHR) MASK Register */

/* -------- PICOUART_VERSION : (PICOUART Offset: 0x20) (R/  32) Version Register -------- */
#define PICOUART_VERSION_OFFSET     0x20         /**< \brief (PICOUART_VERSION offset) Version Register */
#define PICOUART_VERSION_RESETVALUE 0x00000101   /**< \brief (PICOUART_VERSION reset_value) Version Register */

#define PICOUART_VERSION_VERSION_Pos 0            /**< \brief (PICOUART_VERSION) Version Number */
#define PICOUART_VERSION_VERSION_Msk (0xFFFu << PICOUART_VERSION_VERSION_Pos)
#define PICOUART_VERSION_VERSION(value) ((PICOUART_VERSION_VERSION_Msk & ((value) << PICOUART_VERSION_VERSION_Pos)))
#define PICOUART_VERSION_VARIANT_Pos 16           /**< \brief (PICOUART_VERSION) Variant Number */
#define PICOUART_VERSION_VARIANT_Msk (0xFu << PICOUART_VERSION_VARIANT_Pos)
#define PICOUART_VERSION_VARIANT(value) ((PICOUART_VERSION_VARIANT_Msk & ((value) << PICOUART_VERSION_VARIANT_Pos)))
#define PICOUART_VERSION_MASK       0x000F0FFFu  /**< \brief (PICOUART_VERSION) MASK Register */

/** \brief PICOUART hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  WoReg   PICOUART_CR;        /**< \brief (PICOUART Offset: 0x00) Control Register */
  RwReg   PICOUART_CFG;       /**< \brief (PICOUART Offset: 0x04) Configuration Register */
  RoReg   PICOUART_SR;        /**< \brief (PICOUART Offset: 0x08) Status Register */
  RoReg   PICOUART_RHR;       /**< \brief (PICOUART Offset: 0x0C) Receive Holding Register */
  RoReg8  Reserved1[0x10];
  RoReg   PICOUART_VERSION;   /**< \brief (PICOUART Offset: 0x20) Version Register */
} Picouart;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAM4L_PICOUART_COMPONENT_ */
