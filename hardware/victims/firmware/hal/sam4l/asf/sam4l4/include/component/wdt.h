/**
 * \file
 *
 * \brief Component description for WDT
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

#ifndef _SAM4L_WDT_COMPONENT_
#define _SAM4L_WDT_COMPONENT_

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR WDT */
/* ========================================================================== */
/** \addtogroup SAM4L_WDT Watchdog Timer */
/*@{*/

#define WDT_I7528
#define REV_WDT                     0x501

/* -------- WDT_CTRL : (WDT Offset: 0x000) (R/W 32) Control Register -------- */
#define WDT_CTRL_OFFSET             0x000        /**< \brief (WDT_CTRL offset) Control Register */
#define WDT_CTRL_RESETVALUE         0x00010080   /**< \brief (WDT_CTRL reset_value) Control Register */

#define WDT_CTRL_EN_Pos             0            /**< \brief (WDT_CTRL) WDT Enable */
#define WDT_CTRL_EN                 (0x1u << WDT_CTRL_EN_Pos)
#define   WDT_CTRL_EN_0_Val               0x0u   /**< \brief (WDT_CTRL) WDT is disabled. */
#define   WDT_CTRL_EN_1_Val               0x1u   /**< \brief (WDT_CTRL) WDT is enabled */
#define WDT_CTRL_EN_0               (WDT_CTRL_EN_0_Val             << WDT_CTRL_EN_Pos)
#define WDT_CTRL_EN_1               (WDT_CTRL_EN_1_Val             << WDT_CTRL_EN_Pos)
#define WDT_CTRL_DAR_Pos            1            /**< \brief (WDT_CTRL) WDT Disable After Reset */
#define WDT_CTRL_DAR                (0x1u << WDT_CTRL_DAR_Pos)
#define WDT_CTRL_MODE_Pos           2            /**< \brief (WDT_CTRL) WDT Mode */
#define WDT_CTRL_MODE               (0x1u << WDT_CTRL_MODE_Pos)
#define WDT_CTRL_SFV_Pos            3            /**< \brief (WDT_CTRL) WDT Store Final Value */
#define WDT_CTRL_SFV                (0x1u << WDT_CTRL_SFV_Pos)
#define WDT_CTRL_IM_Pos             4            /**< \brief (WDT_CTRL) WDT Interruput Mode */
#define WDT_CTRL_IM                 (0x1u << WDT_CTRL_IM_Pos)
#define WDT_CTRL_FCD_Pos            7            /**< \brief (WDT_CTRL) WDT Fuse Calibration Done */
#define WDT_CTRL_FCD                (0x1u << WDT_CTRL_FCD_Pos)
#define WDT_CTRL_PSEL_Pos           8            /**< \brief (WDT_CTRL) Timeout Prescale Select */
#define WDT_CTRL_PSEL_Msk           (0x1Fu << WDT_CTRL_PSEL_Pos)
#define WDT_CTRL_PSEL(value)        ((WDT_CTRL_PSEL_Msk & ((value) << WDT_CTRL_PSEL_Pos)))
#define WDT_CTRL_CSSEL1_Pos         14           /**< \brief (WDT_CTRL) Clock Source Selection1 */
#define WDT_CTRL_CSSEL1             (0x1u << WDT_CTRL_CSSEL1_Pos)
#define WDT_CTRL_CEN_Pos            16           /**< \brief (WDT_CTRL) Clock Enable */
#define WDT_CTRL_CEN                (0x1u << WDT_CTRL_CEN_Pos)
#define WDT_CTRL_CSSEL_Pos          17           /**< \brief (WDT_CTRL) Clock Source Selection0 */
#define WDT_CTRL_CSSEL              (0x1u << WDT_CTRL_CSSEL_Pos)
#define WDT_CTRL_TBAN_Pos           18           /**< \brief (WDT_CTRL) TBAN Prescale Select */
#define WDT_CTRL_TBAN_Msk           (0x1Fu << WDT_CTRL_TBAN_Pos)
#define WDT_CTRL_TBAN(value)        ((WDT_CTRL_TBAN_Msk & ((value) << WDT_CTRL_TBAN_Pos)))
#define WDT_CTRL_KEY_Pos            24           /**< \brief (WDT_CTRL) Key */
#define WDT_CTRL_KEY_Msk            (0xFFu << WDT_CTRL_KEY_Pos)
#define WDT_CTRL_KEY(value)         ((WDT_CTRL_KEY_Msk & ((value) << WDT_CTRL_KEY_Pos)))
#define WDT_CTRL_MASK               0xFF7F5F9Fu  /**< \brief (WDT_CTRL) MASK Register */

/* -------- WDT_CLR : (WDT Offset: 0x004) ( /W 32) Clear Register -------- */
#define WDT_CLR_OFFSET              0x004        /**< \brief (WDT_CLR offset) Clear Register */
#define WDT_CLR_RESETVALUE          0x00000000   /**< \brief (WDT_CLR reset_value) Clear Register */

#define WDT_CLR_WDTCLR_Pos          0            /**< \brief (WDT_CLR) Clear WDT counter */
#define WDT_CLR_WDTCLR              (0x1u << WDT_CLR_WDTCLR_Pos)
#define WDT_CLR_KEY_Pos             24           /**< \brief (WDT_CLR) Key */
#define WDT_CLR_KEY_Msk             (0xFFu << WDT_CLR_KEY_Pos)
#define WDT_CLR_KEY(value)          ((WDT_CLR_KEY_Msk & ((value) << WDT_CLR_KEY_Pos)))
#define WDT_CLR_MASK                0xFF000001u  /**< \brief (WDT_CLR) MASK Register */

/* -------- WDT_SR : (WDT Offset: 0x008) (R/  32) Status Register -------- */
#define WDT_SR_OFFSET               0x008        /**< \brief (WDT_SR offset) Status Register */
#define WDT_SR_RESETVALUE           0x00000003   /**< \brief (WDT_SR reset_value) Status Register */

#define WDT_SR_WINDOW_Pos           0            /**< \brief (WDT_SR) WDT in window */
#define WDT_SR_WINDOW               (0x1u << WDT_SR_WINDOW_Pos)
#define WDT_SR_CLEARED_Pos          1            /**< \brief (WDT_SR) WDT cleared */
#define WDT_SR_CLEARED              (0x1u << WDT_SR_CLEARED_Pos)
#define WDT_SR_MASK                 0x00000003u  /**< \brief (WDT_SR) MASK Register */

/* -------- WDT_IER : (WDT Offset: 0x00C) ( /W 32) Interrupt Enable Register -------- */
#define WDT_IER_OFFSET              0x00C        /**< \brief (WDT_IER offset) Interrupt Enable Register */
#define WDT_IER_RESETVALUE          0x00000000   /**< \brief (WDT_IER reset_value) Interrupt Enable Register */

#define WDT_IER_WINT_Pos            2            /**< \brief (WDT_IER) Watchdog Interrupt */
#define WDT_IER_WINT                (0x1u << WDT_IER_WINT_Pos)
#define WDT_IER_MASK                0x00000004u  /**< \brief (WDT_IER) MASK Register */

/* -------- WDT_IDR : (WDT Offset: 0x010) ( /W 32) Interrupt Disable Register -------- */
#define WDT_IDR_OFFSET              0x010        /**< \brief (WDT_IDR offset) Interrupt Disable Register */
#define WDT_IDR_RESETVALUE          0x00000000   /**< \brief (WDT_IDR reset_value) Interrupt Disable Register */

#define WDT_IDR_WINT_Pos            2            /**< \brief (WDT_IDR) Watchdog Interrupt */
#define WDT_IDR_WINT                (0x1u << WDT_IDR_WINT_Pos)
#define WDT_IDR_MASK                0x00000004u  /**< \brief (WDT_IDR) MASK Register */

/* -------- WDT_IMR : (WDT Offset: 0x014) (R/  32) Interrupt Mask Register -------- */
#define WDT_IMR_OFFSET              0x014        /**< \brief (WDT_IMR offset) Interrupt Mask Register */
#define WDT_IMR_RESETVALUE          0x00000000   /**< \brief (WDT_IMR reset_value) Interrupt Mask Register */

#define WDT_IMR_WINT_Pos            2            /**< \brief (WDT_IMR) Watchdog Interrupt */
#define WDT_IMR_WINT                (0x1u << WDT_IMR_WINT_Pos)
#define WDT_IMR_MASK                0x00000004u  /**< \brief (WDT_IMR) MASK Register */

/* -------- WDT_ISR : (WDT Offset: 0x018) (R/  32) Interrupt Status Register -------- */
#define WDT_ISR_OFFSET              0x018        /**< \brief (WDT_ISR offset) Interrupt Status Register */
#define WDT_ISR_RESETVALUE          0x00000000   /**< \brief (WDT_ISR reset_value) Interrupt Status Register */

#define WDT_ISR_WINT_Pos            2            /**< \brief (WDT_ISR) Watchdog Interrupt */
#define WDT_ISR_WINT                (0x1u << WDT_ISR_WINT_Pos)
#define WDT_ISR_MASK                0x00000004u  /**< \brief (WDT_ISR) MASK Register */

/* -------- WDT_ICR : (WDT Offset: 0x01C) ( /W 32) Interrupt Clear Register -------- */
#define WDT_ICR_OFFSET              0x01C        /**< \brief (WDT_ICR offset) Interrupt Clear Register */
#define WDT_ICR_RESETVALUE          0x00000000   /**< \brief (WDT_ICR reset_value) Interrupt Clear Register */

#define WDT_ICR_WINT_Pos            2            /**< \brief (WDT_ICR) Watchdog Interrupt */
#define WDT_ICR_WINT                (0x1u << WDT_ICR_WINT_Pos)
#define WDT_ICR_MASK                0x00000004u  /**< \brief (WDT_ICR) MASK Register */

/* -------- WDT_VERSION : (WDT Offset: 0x3FC) (R/  32) Version Register -------- */
#define WDT_VERSION_OFFSET          0x3FC        /**< \brief (WDT_VERSION offset) Version Register */
#define WDT_VERSION_RESETVALUE      0x00000501   /**< \brief (WDT_VERSION reset_value) Version Register */

#define WDT_VERSION_VERSION_Pos     0            /**< \brief (WDT_VERSION) Version number */
#define WDT_VERSION_VERSION_Msk     (0xFFFu << WDT_VERSION_VERSION_Pos)
#define WDT_VERSION_VERSION(value)  ((WDT_VERSION_VERSION_Msk & ((value) << WDT_VERSION_VERSION_Pos)))
#define WDT_VERSION_VARIANT_Pos     16           /**< \brief (WDT_VERSION) Variant number */
#define WDT_VERSION_VARIANT_Msk     (0xFu << WDT_VERSION_VARIANT_Pos)
#define WDT_VERSION_VARIANT(value)  ((WDT_VERSION_VARIANT_Msk & ((value) << WDT_VERSION_VARIANT_Pos)))
#define WDT_VERSION_MASK            0x000F0FFFu  /**< \brief (WDT_VERSION) MASK Register */

/** \brief WDT hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  RwReg   WDT_CTRL;           /**< \brief (WDT Offset: 0x000) Control Register */
  WoReg   WDT_CLR;            /**< \brief (WDT Offset: 0x004) Clear Register */
  RoReg   WDT_SR;             /**< \brief (WDT Offset: 0x008) Status Register */
  WoReg   WDT_IER;            /**< \brief (WDT Offset: 0x00C) Interrupt Enable Register */
  WoReg   WDT_IDR;            /**< \brief (WDT Offset: 0x010) Interrupt Disable Register */
  RoReg   WDT_IMR;            /**< \brief (WDT Offset: 0x014) Interrupt Mask Register */
  RoReg   WDT_ISR;            /**< \brief (WDT Offset: 0x018) Interrupt Status Register */
  WoReg   WDT_ICR;            /**< \brief (WDT Offset: 0x01C) Interrupt Clear Register */
  RoReg8  Reserved1[0x3DC];
  RoReg   WDT_VERSION;        /**< \brief (WDT Offset: 0x3FC) Version Register */
} Wdt;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAM4L_WDT_COMPONENT_ */
