/**************************************************************************//**
 * @file
 * @brief EFR32MG22 RFSENSE register and bit field definitions
 ******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *****************************************************************************/
#ifndef EFR32MG22_RFSENSE_H
#define EFR32MG22_RFSENSE_H
#define RFSENSE_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG22_RFSENSE RFSENSE
 * @{
 * @brief EFR32MG22 RFSENSE Register Declaration.
 *****************************************************************************/

/** RFSENSE Register Declaration. */
typedef struct {
  __IOM uint32_t EN;                            /**< Module Enable register                             */
  __IOM uint32_t EM4WUEN;                       /**< EM4 WakeUp Enable                                  */
  __IOM uint32_t CFG;                           /**< RFSENSE Digital Config                             */
  __IOM uint32_t SYNCWORD;                      /**< SYNCWORD                                           */
  __IOM uint32_t THDSEL;                        /**< RFSENSE Threshold Select                           */
  uint32_t       RESERVED0[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable register                          */
  __IOM uint32_t CALCFG;                        /**< Calibration Config register                        */
  __IOM uint32_t RFEN;                          /**< Config register                                    */
  __IOM uint32_t MODESEL;                       /**< Mode Select                                        */
  __IOM uint32_t CMPCONF;                       /**< Comparator Config                                  */
  __IOM uint32_t TRIMPTAT;                      /**< Trim PTAT                                          */
  __IOM uint32_t TRIMBG;                        /**< Trim BandGap                                       */
  __IOM uint32_t TRIMDAC;                       /**< Trim RFSENSE DAC                                   */
  __IOM uint32_t SPARE;                         /**< Spare register                                     */
  uint32_t       RESERVED1[6U];                 /**< Reserved for future use                            */
  __IOM uint32_t SWCTRL;                        /**< Software Control                                   */
  __IOM uint32_t DIAGCTRL;                      /**< Diagnostic Control                                 */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  uint32_t       RESERVED2[997U];               /**< Reserved for future use                            */
  __IOM uint32_t EN_SET;                        /**< Module Enable register                             */
  __IOM uint32_t EM4WUEN_SET;                   /**< EM4 WakeUp Enable                                  */
  __IOM uint32_t CFG_SET;                       /**< RFSENSE Digital Config                             */
  __IOM uint32_t SYNCWORD_SET;                  /**< SYNCWORD                                           */
  __IOM uint32_t THDSEL_SET;                    /**< RFSENSE Threshold Select                           */
  uint32_t       RESERVED3[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable register                          */
  __IOM uint32_t CALCFG_SET;                    /**< Calibration Config register                        */
  __IOM uint32_t RFEN_SET;                      /**< Config register                                    */
  __IOM uint32_t MODESEL_SET;                   /**< Mode Select                                        */
  __IOM uint32_t CMPCONF_SET;                   /**< Comparator Config                                  */
  __IOM uint32_t TRIMPTAT_SET;                  /**< Trim PTAT                                          */
  __IOM uint32_t TRIMBG_SET;                    /**< Trim BandGap                                       */
  __IOM uint32_t TRIMDAC_SET;                   /**< Trim RFSENSE DAC                                   */
  __IOM uint32_t SPARE_SET;                     /**< Spare register                                     */
  uint32_t       RESERVED4[6U];                 /**< Reserved for future use                            */
  __IOM uint32_t SWCTRL_SET;                    /**< Software Control                                   */
  __IOM uint32_t DIAGCTRL_SET;                  /**< Diagnostic Control                                 */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  uint32_t       RESERVED5[997U];               /**< Reserved for future use                            */
  __IOM uint32_t EN_CLR;                        /**< Module Enable register                             */
  __IOM uint32_t EM4WUEN_CLR;                   /**< EM4 WakeUp Enable                                  */
  __IOM uint32_t CFG_CLR;                       /**< RFSENSE Digital Config                             */
  __IOM uint32_t SYNCWORD_CLR;                  /**< SYNCWORD                                           */
  __IOM uint32_t THDSEL_CLR;                    /**< RFSENSE Threshold Select                           */
  uint32_t       RESERVED6[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable register                          */
  __IOM uint32_t CALCFG_CLR;                    /**< Calibration Config register                        */
  __IOM uint32_t RFEN_CLR;                      /**< Config register                                    */
  __IOM uint32_t MODESEL_CLR;                   /**< Mode Select                                        */
  __IOM uint32_t CMPCONF_CLR;                   /**< Comparator Config                                  */
  __IOM uint32_t TRIMPTAT_CLR;                  /**< Trim PTAT                                          */
  __IOM uint32_t TRIMBG_CLR;                    /**< Trim BandGap                                       */
  __IOM uint32_t TRIMDAC_CLR;                   /**< Trim RFSENSE DAC                                   */
  __IOM uint32_t SPARE_CLR;                     /**< Spare register                                     */
  uint32_t       RESERVED7[6U];                 /**< Reserved for future use                            */
  __IOM uint32_t SWCTRL_CLR;                    /**< Software Control                                   */
  __IOM uint32_t DIAGCTRL_CLR;                  /**< Diagnostic Control                                 */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  uint32_t       RESERVED8[997U];               /**< Reserved for future use                            */
  __IOM uint32_t EN_TGL;                        /**< Module Enable register                             */
  __IOM uint32_t EM4WUEN_TGL;                   /**< EM4 WakeUp Enable                                  */
  __IOM uint32_t CFG_TGL;                       /**< RFSENSE Digital Config                             */
  __IOM uint32_t SYNCWORD_TGL;                  /**< SYNCWORD                                           */
  __IOM uint32_t THDSEL_TGL;                    /**< RFSENSE Threshold Select                           */
  uint32_t       RESERVED9[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable register                          */
  __IOM uint32_t CALCFG_TGL;                    /**< Calibration Config register                        */
  __IOM uint32_t RFEN_TGL;                      /**< Config register                                    */
  __IOM uint32_t MODESEL_TGL;                   /**< Mode Select                                        */
  __IOM uint32_t CMPCONF_TGL;                   /**< Comparator Config                                  */
  __IOM uint32_t TRIMPTAT_TGL;                  /**< Trim PTAT                                          */
  __IOM uint32_t TRIMBG_TGL;                    /**< Trim BandGap                                       */
  __IOM uint32_t TRIMDAC_TGL;                   /**< Trim RFSENSE DAC                                   */
  __IOM uint32_t SPARE_TGL;                     /**< Spare register                                     */
  uint32_t       RESERVED10[6U];                /**< Reserved for future use                            */
  __IOM uint32_t SWCTRL_TGL;                    /**< Software Control                                   */
  __IOM uint32_t DIAGCTRL_TGL;                  /**< Diagnostic Control                                 */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
} RFSENSE_TypeDef;
/** @} End of group EFR32MG22_RFSENSE */

/**************************************************************************//**
 * @addtogroup EFR32MG22_RFSENSE
 * @{
 * @defgroup EFR32MG22_RFSENSE_BitFields RFSENSE Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for RFSENSE EN */
#define _RFSENSE_EN_RESETVALUE                   0x00000000UL                           /**< Default value for RFSENSE_EN                */
#define _RFSENSE_EN_MASK                         0x00000001UL                           /**< Mask for RFSENSE_EN                         */
#define RFSENSE_EN_EN                            (0x1UL << 0)                           /**< RFSENSE Enable                              */
#define _RFSENSE_EN_EN_SHIFT                     0                                      /**< Shift value for RFSENSE_EN                  */
#define _RFSENSE_EN_EN_MASK                      0x1UL                                  /**< Bit mask for RFSENSE_EN                     */
#define _RFSENSE_EN_EN_DEFAULT                   0x00000000UL                           /**< Mode DEFAULT for RFSENSE_EN                 */
#define RFSENSE_EN_EN_DEFAULT                    (_RFSENSE_EN_EN_DEFAULT << 0)          /**< Shifted mode DEFAULT for RFSENSE_EN         */

/* Bit fields for RFSENSE EM4WUEN */
#define _RFSENSE_EM4WUEN_RESETVALUE              0x00000000UL                            /**< Default value for RFSENSE_EM4WUEN           */
#define _RFSENSE_EM4WUEN_MASK                    0x00000001UL                            /**< Mask for RFSENSE_EM4WUEN                    */
#define RFSENSE_EM4WUEN_EM4WUEN                  (0x1UL << 0)                            /**< EM4 WakeUp Enable                           */
#define _RFSENSE_EM4WUEN_EM4WUEN_SHIFT           0                                       /**< Shift value for RFSENSE_EM4WUEN             */
#define _RFSENSE_EM4WUEN_EM4WUEN_MASK            0x1UL                                   /**< Bit mask for RFSENSE_EM4WUEN                */
#define _RFSENSE_EM4WUEN_EM4WUEN_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for RFSENSE_EM4WUEN            */
#define RFSENSE_EM4WUEN_EM4WUEN_DEFAULT          (_RFSENSE_EM4WUEN_EM4WUEN_DEFAULT << 0) /**< Shifted mode DEFAULT for RFSENSE_EM4WUEN    */

/* Bit fields for RFSENSE CFG */
#define _RFSENSE_CFG_RESETVALUE                  0x03800000UL                            /**< Default value for RFSENSE_CFG               */
#define _RFSENSE_CFG_MASK                        0x0780000DUL                            /**< Mask for RFSENSE_CFG                        */
#define RFSENSE_CFG_PREAMBLELEN                  (0x1UL << 0)                            /**< Number of Preamble bits                     */
#define _RFSENSE_CFG_PREAMBLELEN_SHIFT           0                                       /**< Shift value for RFSENSE_PREAMBLELEN         */
#define _RFSENSE_CFG_PREAMBLELEN_MASK            0x1UL                                   /**< Bit mask for RFSENSE_PREAMBLELEN            */
#define _RFSENSE_CFG_PREAMBLELEN_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for RFSENSE_CFG                */
#define RFSENSE_CFG_PREAMBLELEN_DEFAULT          (_RFSENSE_CFG_PREAMBLELEN_DEFAULT << 0) /**< Shifted mode DEFAULT for RFSENSE_CFG        */
#define _RFSENSE_CFG_SYNCWORDLEN_SHIFT           2                                       /**< Shift value for RFSENSE_SYNCWORDLEN         */
#define _RFSENSE_CFG_SYNCWORDLEN_MASK            0xCUL                                   /**< Bit mask for RFSENSE_SYNCWORDLEN            */
#define _RFSENSE_CFG_SYNCWORDLEN_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for RFSENSE_CFG                */
#define RFSENSE_CFG_SYNCWORDLEN_DEFAULT          (_RFSENSE_CFG_SYNCWORDLEN_DEFAULT << 2) /**< Shifted mode DEFAULT for RFSENSE_CFG        */
#define _RFSENSE_CFG_ENERGYDUR_SHIFT             23                                      /**< Shift value for RFSENSE_ENERGYDUR           */
#define _RFSENSE_CFG_ENERGYDUR_MASK              0x3800000UL                             /**< Bit mask for RFSENSE_ENERGYDUR              */
#define _RFSENSE_CFG_ENERGYDUR_DEFAULT           0x00000007UL                            /**< Mode DEFAULT for RFSENSE_CFG                */
#define _RFSENSE_CFG_ENERGYDUR_dur1ms            0x00000000UL                            /**< Mode dur1ms for RFSENSE_CFG                 */
#define _RFSENSE_CFG_ENERGYDUR_dur2ms            0x00000001UL                            /**< Mode dur2ms for RFSENSE_CFG                 */
#define _RFSENSE_CFG_ENERGYDUR_dur4ms            0x00000002UL                            /**< Mode dur4ms for RFSENSE_CFG                 */
#define _RFSENSE_CFG_ENERGYDUR_dur8ms            0x00000003UL                            /**< Mode dur8ms for RFSENSE_CFG                 */
#define _RFSENSE_CFG_ENERGYDUR_dur16ms           0x00000004UL                            /**< Mode dur16ms for RFSENSE_CFG                */
#define _RFSENSE_CFG_ENERGYDUR_dur32ms           0x00000005UL                            /**< Mode dur32ms for RFSENSE_CFG                */
#define _RFSENSE_CFG_ENERGYDUR_dur64ms           0x00000006UL                            /**< Mode dur64ms for RFSENSE_CFG                */
#define _RFSENSE_CFG_ENERGYDUR_dur128ms          0x00000007UL                            /**< Mode dur128ms for RFSENSE_CFG               */
#define RFSENSE_CFG_ENERGYDUR_DEFAULT            (_RFSENSE_CFG_ENERGYDUR_DEFAULT << 23)  /**< Shifted mode DEFAULT for RFSENSE_CFG        */
#define RFSENSE_CFG_ENERGYDUR_dur1ms             (_RFSENSE_CFG_ENERGYDUR_dur1ms << 23)   /**< Shifted mode dur1ms for RFSENSE_CFG         */
#define RFSENSE_CFG_ENERGYDUR_dur2ms             (_RFSENSE_CFG_ENERGYDUR_dur2ms << 23)   /**< Shifted mode dur2ms for RFSENSE_CFG         */
#define RFSENSE_CFG_ENERGYDUR_dur4ms             (_RFSENSE_CFG_ENERGYDUR_dur4ms << 23)   /**< Shifted mode dur4ms for RFSENSE_CFG         */
#define RFSENSE_CFG_ENERGYDUR_dur8ms             (_RFSENSE_CFG_ENERGYDUR_dur8ms << 23)   /**< Shifted mode dur8ms for RFSENSE_CFG         */
#define RFSENSE_CFG_ENERGYDUR_dur16ms            (_RFSENSE_CFG_ENERGYDUR_dur16ms << 23)  /**< Shifted mode dur16ms for RFSENSE_CFG        */
#define RFSENSE_CFG_ENERGYDUR_dur32ms            (_RFSENSE_CFG_ENERGYDUR_dur32ms << 23)  /**< Shifted mode dur32ms for RFSENSE_CFG        */
#define RFSENSE_CFG_ENERGYDUR_dur64ms            (_RFSENSE_CFG_ENERGYDUR_dur64ms << 23)  /**< Shifted mode dur64ms for RFSENSE_CFG        */
#define RFSENSE_CFG_ENERGYDUR_dur128ms           (_RFSENSE_CFG_ENERGYDUR_dur128ms << 23) /**< Shifted mode dur128ms for RFSENSE_CFG       */
#define RFSENSE_CFG_LEGACYMODE                   (0x1UL << 26)                           /**< RFSENSE Legacy Mode                         */
#define _RFSENSE_CFG_LEGACYMODE_SHIFT            26                                      /**< Shift value for RFSENSE_LEGACYMODE          */
#define _RFSENSE_CFG_LEGACYMODE_MASK             0x4000000UL                             /**< Bit mask for RFSENSE_LEGACYMODE             */
#define _RFSENSE_CFG_LEGACYMODE_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for RFSENSE_CFG                */
#define RFSENSE_CFG_LEGACYMODE_DEFAULT           (_RFSENSE_CFG_LEGACYMODE_DEFAULT << 26) /**< Shifted mode DEFAULT for RFSENSE_CFG        */

/* Bit fields for RFSENSE SYNCWORD */
#define _RFSENSE_SYNCWORD_RESETVALUE             0x00000000UL                              /**< Default value for RFSENSE_SYNCWORD          */
#define _RFSENSE_SYNCWORD_MASK                   0xFFFFFFFFUL                              /**< Mask for RFSENSE_SYNCWORD                   */
#define _RFSENSE_SYNCWORD_SYNCWORD_SHIFT         0                                         /**< Shift value for RFSENSE_SYNCWORD            */
#define _RFSENSE_SYNCWORD_SYNCWORD_MASK          0xFFFFFFFFUL                              /**< Bit mask for RFSENSE_SYNCWORD               */
#define _RFSENSE_SYNCWORD_SYNCWORD_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for RFSENSE_SYNCWORD           */
#define RFSENSE_SYNCWORD_SYNCWORD_DEFAULT        (_RFSENSE_SYNCWORD_SYNCWORD_DEFAULT << 0) /**< Shifted mode DEFAULT for RFSENSE_SYNCWORD   */

/* Bit fields for RFSENSE THDSEL */
#define _RFSENSE_THDSEL_RESETVALUE               0x00000080UL                           /**< Default value for RFSENSE_THDSEL            */
#define _RFSENSE_THDSEL_MASK                     0x000000FFUL                           /**< Mask for RFSENSE_THDSEL                     */
#define _RFSENSE_THDSEL_THDSEL_SHIFT             0                                      /**< Shift value for RFSENSE_THDSEL              */
#define _RFSENSE_THDSEL_THDSEL_MASK              0xFFUL                                 /**< Bit mask for RFSENSE_THDSEL                 */
#define _RFSENSE_THDSEL_THDSEL_DEFAULT           0x00000080UL                           /**< Mode DEFAULT for RFSENSE_THDSEL             */
#define RFSENSE_THDSEL_THDSEL_DEFAULT            (_RFSENSE_THDSEL_THDSEL_DEFAULT << 0)  /**< Shifted mode DEFAULT for RFSENSE_THDSEL     */

/* Bit fields for RFSENSE IF */
#define _RFSENSE_IF_RESETVALUE                   0x00000000UL                           /**< Default value for RFSENSE_IF                */
#define _RFSENSE_IF_MASK                         0x00000003UL                           /**< Mask for RFSENSE_IF                         */
#define RFSENSE_IF_RFSENSEIF                     (0x1UL << 0)                           /**< RFSENSE Interrupt Flag                      */
#define _RFSENSE_IF_RFSENSEIF_SHIFT              0                                      /**< Shift value for RFSENSE_RFSENSEIF           */
#define _RFSENSE_IF_RFSENSEIF_MASK               0x1UL                                  /**< Bit mask for RFSENSE_RFSENSEIF              */
#define _RFSENSE_IF_RFSENSEIF_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for RFSENSE_IF                 */
#define RFSENSE_IF_RFSENSEIF_DEFAULT             (_RFSENSE_IF_RFSENSEIF_DEFAULT << 0)   /**< Shifted mode DEFAULT for RFSENSE_IF         */
#define RFSENSE_IF_SEQIF                         (0x1UL << 1)                           /**< Sequencer Interrupt Flag                    */
#define _RFSENSE_IF_SEQIF_SHIFT                  1                                      /**< Shift value for RFSENSE_SEQIF               */
#define _RFSENSE_IF_SEQIF_MASK                   0x2UL                                  /**< Bit mask for RFSENSE_SEQIF                  */
#define _RFSENSE_IF_SEQIF_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for RFSENSE_IF                 */
#define RFSENSE_IF_SEQIF_DEFAULT                 (_RFSENSE_IF_SEQIF_DEFAULT << 1)       /**< Shifted mode DEFAULT for RFSENSE_IF         */

/* Bit fields for RFSENSE IEN */
#define _RFSENSE_IEN_RESETVALUE                  0x00000000UL                           /**< Default value for RFSENSE_IEN               */
#define _RFSENSE_IEN_MASK                        0x00000003UL                           /**< Mask for RFSENSE_IEN                        */
#define RFSENSE_IEN_RFSENSEIEN                   (0x1UL << 0)                           /**< RFSENSE Interrrupt Enable                   */
#define _RFSENSE_IEN_RFSENSEIEN_SHIFT            0                                      /**< Shift value for RFSENSE_RFSENSEIEN          */
#define _RFSENSE_IEN_RFSENSEIEN_MASK             0x1UL                                  /**< Bit mask for RFSENSE_RFSENSEIEN             */
#define _RFSENSE_IEN_RFSENSEIEN_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for RFSENSE_IEN                */
#define RFSENSE_IEN_RFSENSEIEN_DEFAULT           (_RFSENSE_IEN_RFSENSEIEN_DEFAULT << 0) /**< Shifted mode DEFAULT for RFSENSE_IEN        */
#define RFSENSE_IEN_SEQIEN                       (0x1UL << 1)                           /**< Sequencer Interrupt Enable                  */
#define _RFSENSE_IEN_SEQIEN_SHIFT                1                                      /**< Shift value for RFSENSE_SEQIEN              */
#define _RFSENSE_IEN_SEQIEN_MASK                 0x2UL                                  /**< Bit mask for RFSENSE_SEQIEN                 */
#define _RFSENSE_IEN_SEQIEN_DEFAULT              0x00000000UL                           /**< Mode DEFAULT for RFSENSE_IEN                */
#define RFSENSE_IEN_SEQIEN_DEFAULT               (_RFSENSE_IEN_SEQIEN_DEFAULT << 1)     /**< Shifted mode DEFAULT for RFSENSE_IEN        */

/* Bit fields for RFSENSE CALCFG */
#define _RFSENSE_CALCFG_RESETVALUE               0x01050010UL                                 /**< Default value for RFSENSE_CALCFG            */
#define _RFSENSE_CALCFG_MASK                     0x130F70FFUL                                 /**< Mask for RFSENSE_CALCFG                     */
#define _RFSENSE_CALCFG_CALPERIOD_SHIFT          0                                            /**< Shift value for RFSENSE_CALPERIOD           */
#define _RFSENSE_CALCFG_CALPERIOD_MASK           0xFFUL                                       /**< Bit mask for RFSENSE_CALPERIOD              */
#define _RFSENSE_CALCFG_CALPERIOD_DEFAULT        0x00000010UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define RFSENSE_CALCFG_CALPERIOD_DEFAULT         (_RFSENSE_CALCFG_CALPERIOD_DEFAULT << 0)     /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */
#define RFSENSE_CALCFG_DISCMCAL                  (0x1UL << 12)                                /**< Disable Common Mode Calibration             */
#define _RFSENSE_CALCFG_DISCMCAL_SHIFT           12                                           /**< Shift value for RFSENSE_DISCMCAL            */
#define _RFSENSE_CALCFG_DISCMCAL_MASK            0x1000UL                                     /**< Bit mask for RFSENSE_DISCMCAL               */
#define _RFSENSE_CALCFG_DISCMCAL_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define RFSENSE_CALCFG_DISCMCAL_DEFAULT          (_RFSENSE_CALCFG_DISCMCAL_DEFAULT << 12)     /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */
#define RFSENSE_CALCFG_DISPTATCAL                (0x1UL << 13)                                /**< Disable PTAT Calibration                    */
#define _RFSENSE_CALCFG_DISPTATCAL_SHIFT         13                                           /**< Shift value for RFSENSE_DISPTATCAL          */
#define _RFSENSE_CALCFG_DISPTATCAL_MASK          0x2000UL                                     /**< Bit mask for RFSENSE_DISPTATCAL             */
#define _RFSENSE_CALCFG_DISPTATCAL_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define RFSENSE_CALCFG_DISPTATCAL_DEFAULT        (_RFSENSE_CALCFG_DISPTATCAL_DEFAULT << 13)   /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */
#define RFSENSE_CALCFG_ENCALPUP                  (0x1UL << 14)                                /**< Enable Calibration at Power Up              */
#define _RFSENSE_CALCFG_ENCALPUP_SHIFT           14                                           /**< Shift value for RFSENSE_ENCALPUP            */
#define _RFSENSE_CALCFG_ENCALPUP_MASK            0x4000UL                                     /**< Bit mask for RFSENSE_ENCALPUP               */
#define _RFSENSE_CALCFG_ENCALPUP_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define RFSENSE_CALCFG_ENCALPUP_DEFAULT          (_RFSENSE_CALCFG_ENCALPUP_DEFAULT << 14)     /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */
#define _RFSENSE_CALCFG_PTATCALSTEPS_SHIFT       16                                           /**< Shift value for RFSENSE_PTATCALSTEPS        */
#define _RFSENSE_CALCFG_PTATCALSTEPS_MASK        0x30000UL                                    /**< Bit mask for RFSENSE_PTATCALSTEPS           */
#define _RFSENSE_CALCFG_PTATCALSTEPS_DEFAULT     0x00000001UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define _RFSENSE_CALCFG_PTATCALSTEPS_STEPS2      0x00000000UL                                 /**< Mode STEPS2 for RFSENSE_CALCFG              */
#define _RFSENSE_CALCFG_PTATCALSTEPS_STEPS4      0x00000001UL                                 /**< Mode STEPS4 for RFSENSE_CALCFG              */
#define _RFSENSE_CALCFG_PTATCALSTEPS_STEPS6      0x00000002UL                                 /**< Mode STEPS6 for RFSENSE_CALCFG              */
#define _RFSENSE_CALCFG_PTATCALSTEPS_STEPS8      0x00000003UL                                 /**< Mode STEPS8 for RFSENSE_CALCFG              */
#define RFSENSE_CALCFG_PTATCALSTEPS_DEFAULT      (_RFSENSE_CALCFG_PTATCALSTEPS_DEFAULT << 16) /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */
#define RFSENSE_CALCFG_PTATCALSTEPS_STEPS2       (_RFSENSE_CALCFG_PTATCALSTEPS_STEPS2 << 16)  /**< Shifted mode STEPS2 for RFSENSE_CALCFG      */
#define RFSENSE_CALCFG_PTATCALSTEPS_STEPS4       (_RFSENSE_CALCFG_PTATCALSTEPS_STEPS4 << 16)  /**< Shifted mode STEPS4 for RFSENSE_CALCFG      */
#define RFSENSE_CALCFG_PTATCALSTEPS_STEPS6       (_RFSENSE_CALCFG_PTATCALSTEPS_STEPS6 << 16)  /**< Shifted mode STEPS6 for RFSENSE_CALCFG      */
#define RFSENSE_CALCFG_PTATCALSTEPS_STEPS8       (_RFSENSE_CALCFG_PTATCALSTEPS_STEPS8 << 16)  /**< Shifted mode STEPS8 for RFSENSE_CALCFG      */
#define _RFSENSE_CALCFG_CMCALSTEPS_SHIFT         18                                           /**< Shift value for RFSENSE_CMCALSTEPS          */
#define _RFSENSE_CALCFG_CMCALSTEPS_MASK          0xC0000UL                                    /**< Bit mask for RFSENSE_CMCALSTEPS             */
#define _RFSENSE_CALCFG_CMCALSTEPS_DEFAULT       0x00000001UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define _RFSENSE_CALCFG_CMCALSTEPS_STEPS2        0x00000000UL                                 /**< Mode STEPS2 for RFSENSE_CALCFG              */
#define _RFSENSE_CALCFG_CMCALSTEPS_STEPS4        0x00000001UL                                 /**< Mode STEPS4 for RFSENSE_CALCFG              */
#define _RFSENSE_CALCFG_CMCALSTEPS_STEPS6        0x00000002UL                                 /**< Mode STEPS6 for RFSENSE_CALCFG              */
#define _RFSENSE_CALCFG_CMCALSTEPS_STEPS8        0x00000003UL                                 /**< Mode STEPS8 for RFSENSE_CALCFG              */
#define RFSENSE_CALCFG_CMCALSTEPS_DEFAULT        (_RFSENSE_CALCFG_CMCALSTEPS_DEFAULT << 18)   /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */
#define RFSENSE_CALCFG_CMCALSTEPS_STEPS2         (_RFSENSE_CALCFG_CMCALSTEPS_STEPS2 << 18)    /**< Shifted mode STEPS2 for RFSENSE_CALCFG      */
#define RFSENSE_CALCFG_CMCALSTEPS_STEPS4         (_RFSENSE_CALCFG_CMCALSTEPS_STEPS4 << 18)    /**< Shifted mode STEPS4 for RFSENSE_CALCFG      */
#define RFSENSE_CALCFG_CMCALSTEPS_STEPS6         (_RFSENSE_CALCFG_CMCALSTEPS_STEPS6 << 18)    /**< Shifted mode STEPS6 for RFSENSE_CALCFG      */
#define RFSENSE_CALCFG_CMCALSTEPS_STEPS8         (_RFSENSE_CALCFG_CMCALSTEPS_STEPS8 << 18)    /**< Shifted mode STEPS8 for RFSENSE_CALCFG      */
#define _RFSENSE_CALCFG_CALSTEPCLKS_SHIFT        24                                           /**< Shift value for RFSENSE_CALSTEPCLKS         */
#define _RFSENSE_CALCFG_CALSTEPCLKS_MASK         0x3000000UL                                  /**< Bit mask for RFSENSE_CALSTEPCLKS            */
#define _RFSENSE_CALCFG_CALSTEPCLKS_DEFAULT      0x00000001UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define RFSENSE_CALCFG_CALSTEPCLKS_DEFAULT       (_RFSENSE_CALCFG_CALSTEPCLKS_DEFAULT << 24)  /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */
#define RFSENSE_CALCFG_PTATCALMODE               (0x1UL << 28)                                /**< PTAT Calibration mode                       */
#define _RFSENSE_CALCFG_PTATCALMODE_SHIFT        28                                           /**< Shift value for RFSENSE_PTATCALMODE         */
#define _RFSENSE_CALCFG_PTATCALMODE_MASK         0x10000000UL                                 /**< Bit mask for RFSENSE_PTATCALMODE            */
#define _RFSENSE_CALCFG_PTATCALMODE_DEFAULT      0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_CALCFG             */
#define RFSENSE_CALCFG_PTATCALMODE_DEFAULT       (_RFSENSE_CALCFG_PTATCALMODE_DEFAULT << 28)  /**< Shifted mode DEFAULT for RFSENSE_CALCFG     */

/* Bit fields for RFSENSE RFEN */
#define _RFSENSE_RFEN_RESETVALUE                 0x00000060UL                           /**< Default value for RFSENSE_RFEN              */
#define _RFSENSE_RFEN_MASK                       0x0000007FUL                           /**< Mask for RFSENSE_RFEN                       */
#define RFSENSE_RFEN_ENBG                        (0x1UL << 0)                           /**< Enable Bandgap                              */
#define _RFSENSE_RFEN_ENBG_SHIFT                 0                                      /**< Shift value for RFSENSE_ENBG                */
#define _RFSENSE_RFEN_ENBG_MASK                  0x1UL                                  /**< Bit mask for RFSENSE_ENBG                   */
#define _RFSENSE_RFEN_ENBG_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for RFSENSE_RFEN               */
#define RFSENSE_RFEN_ENBG_DEFAULT                (_RFSENSE_RFEN_ENBG_DEFAULT << 0)      /**< Shifted mode DEFAULT for RFSENSE_RFEN       */
#define RFSENSE_RFEN_BGSTART                     (0x1UL << 1)                           /**< Bandgap Startup Signal                      */
#define _RFSENSE_RFEN_BGSTART_SHIFT              1                                      /**< Shift value for RFSENSE_BGSTART             */
#define _RFSENSE_RFEN_BGSTART_MASK               0x2UL                                  /**< Bit mask for RFSENSE_BGSTART                */
#define _RFSENSE_RFEN_BGSTART_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for RFSENSE_RFEN               */
#define RFSENSE_RFEN_BGSTART_DEFAULT             (_RFSENSE_RFEN_BGSTART_DEFAULT << 1)   /**< Shifted mode DEFAULT for RFSENSE_RFEN       */
#define RFSENSE_RFEN_PTATISO                     (0x1UL << 2)                           /**< Isolate PTAT core                           */
#define _RFSENSE_RFEN_PTATISO_SHIFT              2                                      /**< Shift value for RFSENSE_PTATISO             */
#define _RFSENSE_RFEN_PTATISO_MASK               0x4UL                                  /**< Bit mask for RFSENSE_PTATISO                */
#define _RFSENSE_RFEN_PTATISO_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for RFSENSE_RFEN               */
#define RFSENSE_RFEN_PTATISO_DEFAULT             (_RFSENSE_RFEN_PTATISO_DEFAULT << 2)   /**< Shifted mode DEFAULT for RFSENSE_RFEN       */
#define RFSENSE_RFEN_PTATSTART                   (0x1UL << 3)                           /**< PTAT Startup signal                         */
#define _RFSENSE_RFEN_PTATSTART_SHIFT            3                                      /**< Shift value for RFSENSE_PTATSTART           */
#define _RFSENSE_RFEN_PTATSTART_MASK             0x8UL                                  /**< Bit mask for RFSENSE_PTATSTART              */
#define _RFSENSE_RFEN_PTATSTART_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for RFSENSE_RFEN               */
#define RFSENSE_RFEN_PTATSTART_DEFAULT           (_RFSENSE_RFEN_PTATSTART_DEFAULT << 3) /**< Shifted mode DEFAULT for RFSENSE_RFEN       */
#define RFSENSE_RFEN_SUPFLTN                     (0x1UL << 4)                           /**< Disable supply filtering                    */
#define _RFSENSE_RFEN_SUPFLTN_SHIFT              4                                      /**< Shift value for RFSENSE_SUPFLTN             */
#define _RFSENSE_RFEN_SUPFLTN_MASK               0x10UL                                 /**< Bit mask for RFSENSE_SUPFLTN                */
#define _RFSENSE_RFEN_SUPFLTN_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for RFSENSE_RFEN               */
#define RFSENSE_RFEN_SUPFLTN_DEFAULT             (_RFSENSE_RFEN_SUPFLTN_DEFAULT << 4)   /**< Shifted mode DEFAULT for RFSENSE_RFEN       */
#define RFSENSE_RFEN_DCEN                        (0x1UL << 5)                           /**< DC Bias Enable                              */
#define _RFSENSE_RFEN_DCEN_SHIFT                 5                                      /**< Shift value for RFSENSE_DCEN                */
#define _RFSENSE_RFEN_DCEN_MASK                  0x20UL                                 /**< Bit mask for RFSENSE_DCEN                   */
#define _RFSENSE_RFEN_DCEN_DEFAULT               0x00000001UL                           /**< Mode DEFAULT for RFSENSE_RFEN               */
#define RFSENSE_RFEN_DCEN_DEFAULT                (_RFSENSE_RFEN_DCEN_DEFAULT << 5)      /**< Shifted mode DEFAULT for RFSENSE_RFEN       */
#define RFSENSE_RFEN_RESETN                      (0x1UL << 6)                           /**< Reset RFSENSE data flop                     */
#define _RFSENSE_RFEN_RESETN_SHIFT               6                                      /**< Shift value for RFSENSE_RESETN              */
#define _RFSENSE_RFEN_RESETN_MASK                0x40UL                                 /**< Bit mask for RFSENSE_RESETN                 */
#define _RFSENSE_RFEN_RESETN_DEFAULT             0x00000001UL                           /**< Mode DEFAULT for RFSENSE_RFEN               */
#define RFSENSE_RFEN_RESETN_DEFAULT              (_RFSENSE_RFEN_RESETN_DEFAULT << 6)    /**< Shifted mode DEFAULT for RFSENSE_RFEN       */

/* Bit fields for RFSENSE MODESEL */
#define _RFSENSE_MODESEL_RESETVALUE              0x00000000UL                               /**< Default value for RFSENSE_MODESEL           */
#define _RFSENSE_MODESEL_MASK                    0x00000007UL                               /**< Mask for RFSENSE_MODESEL                    */
#define _RFSENSE_MODESEL_MODESEL_SHIFT           0                                          /**< Shift value for RFSENSE_MODESEL             */
#define _RFSENSE_MODESEL_MODESEL_MASK            0x7UL                                      /**< Bit mask for RFSENSE_MODESEL                */
#define _RFSENSE_MODESEL_MODESEL_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for RFSENSE_MODESEL            */
#define _RFSENSE_MODESEL_MODESEL_NORMAL_OP       0x00000000UL                               /**< Mode NORMAL_OP for RFSENSE_MODESEL          */
#define _RFSENSE_MODESEL_MODESEL_CM_CAL          0x00000001UL                               /**< Mode CM_CAL for RFSENSE_MODESEL             */
#define _RFSENSE_MODESEL_MODESEL_REF_CAL         0x00000002UL                               /**< Mode REF_CAL for RFSENSE_MODESEL            */
#define _RFSENSE_MODESEL_MODESEL_NORMAL_OP2      0x00000003UL                               /**< Mode NORMAL_OP2 for RFSENSE_MODESEL         */
#define _RFSENSE_MODESEL_MODESEL_DM_A_CAL        0x00000004UL                               /**< Mode DM_A_CAL for RFSENSE_MODESEL           */
#define _RFSENSE_MODESEL_MODESEL_DM_B_CAL        0x00000005UL                               /**< Mode DM_B_CAL for RFSENSE_MODESEL           */
#define _RFSENSE_MODESEL_MODESEL_DM_A_CAL2       0x00000006UL                               /**< Mode DM_A_CAL2 for RFSENSE_MODESEL          */
#define _RFSENSE_MODESEL_MODESEL_DM_B_CAL2       0x00000007UL                               /**< Mode DM_B_CAL2 for RFSENSE_MODESEL          */
#define RFSENSE_MODESEL_MODESEL_DEFAULT          (_RFSENSE_MODESEL_MODESEL_DEFAULT << 0)    /**< Shifted mode DEFAULT for RFSENSE_MODESEL    */
#define RFSENSE_MODESEL_MODESEL_NORMAL_OP        (_RFSENSE_MODESEL_MODESEL_NORMAL_OP << 0)  /**< Shifted mode NORMAL_OP for RFSENSE_MODESEL  */
#define RFSENSE_MODESEL_MODESEL_CM_CAL           (_RFSENSE_MODESEL_MODESEL_CM_CAL << 0)     /**< Shifted mode CM_CAL for RFSENSE_MODESEL     */
#define RFSENSE_MODESEL_MODESEL_REF_CAL          (_RFSENSE_MODESEL_MODESEL_REF_CAL << 0)    /**< Shifted mode REF_CAL for RFSENSE_MODESEL    */
#define RFSENSE_MODESEL_MODESEL_NORMAL_OP2       (_RFSENSE_MODESEL_MODESEL_NORMAL_OP2 << 0) /**< Shifted mode NORMAL_OP2 for RFSENSE_MODESEL */
#define RFSENSE_MODESEL_MODESEL_DM_A_CAL         (_RFSENSE_MODESEL_MODESEL_DM_A_CAL << 0)   /**< Shifted mode DM_A_CAL for RFSENSE_MODESEL   */
#define RFSENSE_MODESEL_MODESEL_DM_B_CAL         (_RFSENSE_MODESEL_MODESEL_DM_B_CAL << 0)   /**< Shifted mode DM_B_CAL for RFSENSE_MODESEL   */
#define RFSENSE_MODESEL_MODESEL_DM_A_CAL2        (_RFSENSE_MODESEL_MODESEL_DM_A_CAL2 << 0)  /**< Shifted mode DM_A_CAL2 for RFSENSE_MODESEL  */
#define RFSENSE_MODESEL_MODESEL_DM_B_CAL2        (_RFSENSE_MODESEL_MODESEL_DM_B_CAL2 << 0)  /**< Shifted mode DM_B_CAL2 for RFSENSE_MODESEL  */

/* Bit fields for RFSENSE CMPCONF */
#define _RFSENSE_CMPCONF_RESETVALUE              0x00000000UL                           /**< Default value for RFSENSE_CMPCONF           */
#define _RFSENSE_CMPCONF_MASK                    0x00000007UL                           /**< Mask for RFSENSE_CMPCONF                    */
#define RFSENSE_CMPCONF_CHP                      (0x1UL << 0)                           /**< Invert Comparator IO                        */
#define _RFSENSE_CMPCONF_CHP_SHIFT               0                                      /**< Shift value for RFSENSE_CHP                 */
#define _RFSENSE_CMPCONF_CHP_MASK                0x1UL                                  /**< Bit mask for RFSENSE_CHP                    */
#define _RFSENSE_CMPCONF_CHP_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for RFSENSE_CMPCONF            */
#define RFSENSE_CMPCONF_CHP_DEFAULT              (_RFSENSE_CMPCONF_CHP_DEFAULT << 0)    /**< Shifted mode DEFAULT for RFSENSE_CMPCONF    */
#define RFSENSE_CMPCONF_HYS                      (0x1UL << 1)                           /**< Enable Hysteresis                           */
#define _RFSENSE_CMPCONF_HYS_SHIFT               1                                      /**< Shift value for RFSENSE_HYS                 */
#define _RFSENSE_CMPCONF_HYS_MASK                0x2UL                                  /**< Bit mask for RFSENSE_HYS                    */
#define _RFSENSE_CMPCONF_HYS_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for RFSENSE_CMPCONF            */
#define RFSENSE_CMPCONF_HYS_DEFAULT              (_RFSENSE_CMPCONF_HYS_DEFAULT << 1)    /**< Shifted mode DEFAULT for RFSENSE_CMPCONF    */
#define RFSENSE_CMPCONF_PLSN                     (0x1UL << 2)                           /**< Disable Comparator reset pulse              */
#define _RFSENSE_CMPCONF_PLSN_SHIFT              2                                      /**< Shift value for RFSENSE_PLSN                */
#define _RFSENSE_CMPCONF_PLSN_MASK               0x4UL                                  /**< Bit mask for RFSENSE_PLSN                   */
#define _RFSENSE_CMPCONF_PLSN_DEFAULT            0x00000000UL                           /**< Mode DEFAULT for RFSENSE_CMPCONF            */
#define RFSENSE_CMPCONF_PLSN_DEFAULT             (_RFSENSE_CMPCONF_PLSN_DEFAULT << 2)   /**< Shifted mode DEFAULT for RFSENSE_CMPCONF    */

/* Bit fields for RFSENSE TRIMPTAT */
#define _RFSENSE_TRIMPTAT_RESETVALUE             0x00000420UL                              /**< Default value for RFSENSE_TRIMPTAT          */
#define _RFSENSE_TRIMPTAT_MASK                   0x0000073FUL                              /**< Mask for RFSENSE_TRIMPTAT                   */
#define _RFSENSE_TRIMPTAT_TRIMRES_SHIFT          0                                         /**< Shift value for RFSENSE_TRIMRES             */
#define _RFSENSE_TRIMPTAT_TRIMRES_MASK           0x3FUL                                    /**< Bit mask for RFSENSE_TRIMRES                */
#define _RFSENSE_TRIMPTAT_TRIMRES_DEFAULT        0x00000020UL                              /**< Mode DEFAULT for RFSENSE_TRIMPTAT           */
#define RFSENSE_TRIMPTAT_TRIMRES_DEFAULT         (_RFSENSE_TRIMPTAT_TRIMRES_DEFAULT << 0)  /**< Shifted mode DEFAULT for RFSENSE_TRIMPTAT   */
#define _RFSENSE_TRIMPTAT_TRIMCURR_SHIFT         8                                         /**< Shift value for RFSENSE_TRIMCURR            */
#define _RFSENSE_TRIMPTAT_TRIMCURR_MASK          0x700UL                                   /**< Bit mask for RFSENSE_TRIMCURR               */
#define _RFSENSE_TRIMPTAT_TRIMCURR_DEFAULT       0x00000004UL                              /**< Mode DEFAULT for RFSENSE_TRIMPTAT           */
#define _RFSENSE_TRIMPTAT_TRIMCURR_m80pct        0x00000000UL                              /**< Mode m80pct for RFSENSE_TRIMPTAT            */
#define _RFSENSE_TRIMPTAT_TRIMCURR_m60pct        0x00000001UL                              /**< Mode m60pct for RFSENSE_TRIMPTAT            */
#define _RFSENSE_TRIMPTAT_TRIMCURR_m40pct        0x00000002UL                              /**< Mode m40pct for RFSENSE_TRIMPTAT            */
#define _RFSENSE_TRIMPTAT_TRIMCURR_m20pct        0x00000003UL                              /**< Mode m20pct for RFSENSE_TRIMPTAT            */
#define _RFSENSE_TRIMPTAT_TRIMCURR_default       0x00000004UL                              /**< Mode default for RFSENSE_TRIMPTAT           */
#define _RFSENSE_TRIMPTAT_TRIMCURR_p20pct        0x00000005UL                              /**< Mode p20pct for RFSENSE_TRIMPTAT            */
#define _RFSENSE_TRIMPTAT_TRIMCURR_p40pct        0x00000006UL                              /**< Mode p40pct for RFSENSE_TRIMPTAT            */
#define _RFSENSE_TRIMPTAT_TRIMCURR_p60pct        0x00000007UL                              /**< Mode p60pct for RFSENSE_TRIMPTAT            */
#define RFSENSE_TRIMPTAT_TRIMCURR_DEFAULT        (_RFSENSE_TRIMPTAT_TRIMCURR_DEFAULT << 8) /**< Shifted mode DEFAULT for RFSENSE_TRIMPTAT   */
#define RFSENSE_TRIMPTAT_TRIMCURR_m80pct         (_RFSENSE_TRIMPTAT_TRIMCURR_m80pct << 8)  /**< Shifted mode m80pct for RFSENSE_TRIMPTAT    */
#define RFSENSE_TRIMPTAT_TRIMCURR_m60pct         (_RFSENSE_TRIMPTAT_TRIMCURR_m60pct << 8)  /**< Shifted mode m60pct for RFSENSE_TRIMPTAT    */
#define RFSENSE_TRIMPTAT_TRIMCURR_m40pct         (_RFSENSE_TRIMPTAT_TRIMCURR_m40pct << 8)  /**< Shifted mode m40pct for RFSENSE_TRIMPTAT    */
#define RFSENSE_TRIMPTAT_TRIMCURR_m20pct         (_RFSENSE_TRIMPTAT_TRIMCURR_m20pct << 8)  /**< Shifted mode m20pct for RFSENSE_TRIMPTAT    */
#define RFSENSE_TRIMPTAT_TRIMCURR_default        (_RFSENSE_TRIMPTAT_TRIMCURR_default << 8) /**< Shifted mode default for RFSENSE_TRIMPTAT   */
#define RFSENSE_TRIMPTAT_TRIMCURR_p20pct         (_RFSENSE_TRIMPTAT_TRIMCURR_p20pct << 8)  /**< Shifted mode p20pct for RFSENSE_TRIMPTAT    */
#define RFSENSE_TRIMPTAT_TRIMCURR_p40pct         (_RFSENSE_TRIMPTAT_TRIMCURR_p40pct << 8)  /**< Shifted mode p40pct for RFSENSE_TRIMPTAT    */
#define RFSENSE_TRIMPTAT_TRIMCURR_p60pct         (_RFSENSE_TRIMPTAT_TRIMCURR_p60pct << 8)  /**< Shifted mode p60pct for RFSENSE_TRIMPTAT    */

/* Bit fields for RFSENSE TRIMBG */
#define _RFSENSE_TRIMBG_RESETVALUE               0x00000088UL                           /**< Default value for RFSENSE_TRIMBG            */
#define _RFSENSE_TRIMBG_MASK                     0x000000FFUL                           /**< Mask for RFSENSE_TRIMBG                     */
#define _RFSENSE_TRIMBG_TRIMREF_SHIFT            0                                      /**< Shift value for RFSENSE_TRIMREF             */
#define _RFSENSE_TRIMBG_TRIMREF_MASK             0xFUL                                  /**< Bit mask for RFSENSE_TRIMREF                */
#define _RFSENSE_TRIMBG_TRIMREF_DEFAULT          0x00000008UL                           /**< Mode DEFAULT for RFSENSE_TRIMBG             */
#define RFSENSE_TRIMBG_TRIMREF_DEFAULT           (_RFSENSE_TRIMBG_TRIMREF_DEFAULT << 0) /**< Shifted mode DEFAULT for RFSENSE_TRIMBG     */
#define _RFSENSE_TRIMBG_TRIMTC_SHIFT             4                                      /**< Shift value for RFSENSE_TRIMTC              */
#define _RFSENSE_TRIMBG_TRIMTC_MASK              0xF0UL                                 /**< Bit mask for RFSENSE_TRIMTC                 */
#define _RFSENSE_TRIMBG_TRIMTC_DEFAULT           0x00000008UL                           /**< Mode DEFAULT for RFSENSE_TRIMBG             */
#define RFSENSE_TRIMBG_TRIMTC_DEFAULT            (_RFSENSE_TRIMBG_TRIMTC_DEFAULT << 4)  /**< Shifted mode DEFAULT for RFSENSE_TRIMBG     */

/* Bit fields for RFSENSE TRIMDAC */
#define _RFSENSE_TRIMDAC_RESETVALUE              0x00007F7FUL                             /**< Default value for RFSENSE_TRIMDAC           */
#define _RFSENSE_TRIMDAC_MASK                    0x03FFFFFFUL                             /**< Mask for RFSENSE_TRIMDAC                    */
#define _RFSENSE_TRIMDAC_TRIMCM_SHIFT            0                                        /**< Shift value for RFSENSE_TRIMCM              */
#define _RFSENSE_TRIMDAC_TRIMCM_MASK             0xFFUL                                   /**< Bit mask for RFSENSE_TRIMCM                 */
#define _RFSENSE_TRIMDAC_TRIMCM_DEFAULT          0x0000007FUL                             /**< Mode DEFAULT for RFSENSE_TRIMDAC            */
#define RFSENSE_TRIMDAC_TRIMCM_DEFAULT           (_RFSENSE_TRIMDAC_TRIMCM_DEFAULT << 0)   /**< Shifted mode DEFAULT for RFSENSE_TRIMDAC    */
#define _RFSENSE_TRIMDAC_CMPOSCAL_SHIFT          8                                        /**< Shift value for RFSENSE_CMPOSCAL            */
#define _RFSENSE_TRIMDAC_CMPOSCAL_MASK           0xFF00UL                                 /**< Bit mask for RFSENSE_CMPOSCAL               */
#define _RFSENSE_TRIMDAC_CMPOSCAL_DEFAULT        0x0000007FUL                             /**< Mode DEFAULT for RFSENSE_TRIMDAC            */
#define RFSENSE_TRIMDAC_CMPOSCAL_DEFAULT         (_RFSENSE_TRIMDAC_CMPOSCAL_DEFAULT << 8) /**< Shifted mode DEFAULT for RFSENSE_TRIMDAC    */
#define _RFSENSE_TRIMDAC_TRIMDM_SHIFT            16                                       /**< Shift value for RFSENSE_TRIMDM              */
#define _RFSENSE_TRIMDAC_TRIMDM_MASK             0x1FF0000UL                              /**< Bit mask for RFSENSE_TRIMDM                 */
#define _RFSENSE_TRIMDAC_TRIMDM_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for RFSENSE_TRIMDAC            */
#define RFSENSE_TRIMDAC_TRIMDM_DEFAULT           (_RFSENSE_TRIMDAC_TRIMDM_DEFAULT << 16)  /**< Shifted mode DEFAULT for RFSENSE_TRIMDAC    */
#define RFSENSE_TRIMDAC_INVDM                    (0x1UL << 25)                            /**< Invert Differential offset polarity         */
#define _RFSENSE_TRIMDAC_INVDM_SHIFT             25                                       /**< Shift value for RFSENSE_INVDM               */
#define _RFSENSE_TRIMDAC_INVDM_MASK              0x2000000UL                              /**< Bit mask for RFSENSE_INVDM                  */
#define _RFSENSE_TRIMDAC_INVDM_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for RFSENSE_TRIMDAC            */
#define RFSENSE_TRIMDAC_INVDM_DEFAULT            (_RFSENSE_TRIMDAC_INVDM_DEFAULT << 25)   /**< Shifted mode DEFAULT for RFSENSE_TRIMDAC    */

/* Bit fields for RFSENSE SPARE */
#define _RFSENSE_SPARE_RESETVALUE                0x00000000UL                           /**< Default value for RFSENSE_SPARE             */
#define _RFSENSE_SPARE_MASK                      0x000000F1UL                           /**< Mask for RFSENSE_SPARE                      */
#define RFSENSE_SPARE_SPARE                      (0x1UL << 0)                           /**< Spare register                              */
#define _RFSENSE_SPARE_SPARE_SHIFT               0                                      /**< Shift value for RFSENSE_SPARE               */
#define _RFSENSE_SPARE_SPARE_MASK                0x1UL                                  /**< Bit mask for RFSENSE_SPARE                  */
#define _RFSENSE_SPARE_SPARE_DEFAULT             0x00000000UL                           /**< Mode DEFAULT for RFSENSE_SPARE              */
#define RFSENSE_SPARE_SPARE_DEFAULT              (_RFSENSE_SPARE_SPARE_DEFAULT << 0)    /**< Shifted mode DEFAULT for RFSENSE_SPARE      */
#define _RFSENSE_SPARE_DIGSPARE_SHIFT            4                                      /**< Shift value for RFSENSE_DIGSPARE            */
#define _RFSENSE_SPARE_DIGSPARE_MASK             0xF0UL                                 /**< Bit mask for RFSENSE_DIGSPARE               */
#define _RFSENSE_SPARE_DIGSPARE_DEFAULT          0x00000000UL                           /**< Mode DEFAULT for RFSENSE_SPARE              */
#define RFSENSE_SPARE_DIGSPARE_DEFAULT           (_RFSENSE_SPARE_DIGSPARE_DEFAULT << 4) /**< Shifted mode DEFAULT for RFSENSE_SPARE      */

/* Bit fields for RFSENSE SWCTRL */
#define _RFSENSE_SWCTRL_RESETVALUE               0x00000010UL                             /**< Default value for RFSENSE_SWCTRL            */
#define _RFSENSE_SWCTRL_MASK                     0x0000001FUL                             /**< Mask for RFSENSE_SWCTRL                     */
#define _RFSENSE_SWCTRL_SWCTRL_SHIFT             0                                        /**< Shift value for RFSENSE_SWCTRL              */
#define _RFSENSE_SWCTRL_SWCTRL_MASK              0xFUL                                    /**< Bit mask for RFSENSE_SWCTRL                 */
#define _RFSENSE_SWCTRL_SWCTRL_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for RFSENSE_SWCTRL             */
#define RFSENSE_SWCTRL_SWCTRL_DEFAULT            (_RFSENSE_SWCTRL_SWCTRL_DEFAULT << 0)    /**< Shifted mode DEFAULT for RFSENSE_SWCTRL     */
#define RFSENSE_SWCTRL_ANACLKINV                 (0x1UL << 4)                             /**< Invert Analog clock                         */
#define _RFSENSE_SWCTRL_ANACLKINV_SHIFT          4                                        /**< Shift value for RFSENSE_ANACLKINV           */
#define _RFSENSE_SWCTRL_ANACLKINV_MASK           0x10UL                                   /**< Bit mask for RFSENSE_ANACLKINV              */
#define _RFSENSE_SWCTRL_ANACLKINV_DEFAULT        0x00000001UL                             /**< Mode DEFAULT for RFSENSE_SWCTRL             */
#define RFSENSE_SWCTRL_ANACLKINV_DEFAULT         (_RFSENSE_SWCTRL_ANACLKINV_DEFAULT << 4) /**< Shifted mode DEFAULT for RFSENSE_SWCTRL     */

/* Bit fields for RFSENSE DIAGCTRL */
#define _RFSENSE_DIAGCTRL_RESETVALUE             0x00000000UL                                 /**< Default value for RFSENSE_DIAGCTRL          */
#define _RFSENSE_DIAGCTRL_MASK                   0x00000003UL                                 /**< Mask for RFSENSE_DIAGCTRL                   */
#define RFSENSE_DIAGCTRL_DIAGABYPN               (0x1UL << 0)                                 /**< Diag buffer bypass                          */
#define _RFSENSE_DIAGCTRL_DIAGABYPN_SHIFT        0                                            /**< Shift value for RFSENSE_DIAGABYPN           */
#define _RFSENSE_DIAGCTRL_DIAGABYPN_MASK         0x1UL                                        /**< Bit mask for RFSENSE_DIAGABYPN              */
#define _RFSENSE_DIAGCTRL_DIAGABYPN_DEFAULT      0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_DIAGCTRL           */
#define _RFSENSE_DIAGCTRL_DIAGABYPN_ENBYPASS     0x00000000UL                                 /**< Mode ENBYPASS for RFSENSE_DIAGCTRL          */
#define _RFSENSE_DIAGCTRL_DIAGABYPN_DISBYPASS    0x00000001UL                                 /**< Mode DISBYPASS for RFSENSE_DIAGCTRL         */
#define RFSENSE_DIAGCTRL_DIAGABYPN_DEFAULT       (_RFSENSE_DIAGCTRL_DIAGABYPN_DEFAULT << 0)   /**< Shifted mode DEFAULT for RFSENSE_DIAGCTRL   */
#define RFSENSE_DIAGCTRL_DIAGABYPN_ENBYPASS      (_RFSENSE_DIAGCTRL_DIAGABYPN_ENBYPASS << 0)  /**< Shifted mode ENBYPASS for RFSENSE_DIAGCTRL  */
#define RFSENSE_DIAGCTRL_DIAGABYPN_DISBYPASS     (_RFSENSE_DIAGCTRL_DIAGABYPN_DISBYPASS << 0) /**< Shifted mode DISBYPASS for RFSENSE_DIAGCTRL */
#define RFSENSE_DIAGCTRL_DIAGACHP                (0x1UL << 1)                                 /**< Diag buffer chop                            */
#define _RFSENSE_DIAGCTRL_DIAGACHP_SHIFT         1                                            /**< Shift value for RFSENSE_DIAGACHP            */
#define _RFSENSE_DIAGCTRL_DIAGACHP_MASK          0x2UL                                        /**< Bit mask for RFSENSE_DIAGACHP               */
#define _RFSENSE_DIAGCTRL_DIAGACHP_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_DIAGCTRL           */
#define _RFSENSE_DIAGCTRL_DIAGACHP_DISABLE       0x00000000UL                                 /**< Mode DISABLE for RFSENSE_DIAGCTRL           */
#define _RFSENSE_DIAGCTRL_DIAGACHP_ENABLE        0x00000001UL                                 /**< Mode ENABLE for RFSENSE_DIAGCTRL            */
#define RFSENSE_DIAGCTRL_DIAGACHP_DEFAULT        (_RFSENSE_DIAGCTRL_DIAGACHP_DEFAULT << 1)    /**< Shifted mode DEFAULT for RFSENSE_DIAGCTRL   */
#define RFSENSE_DIAGCTRL_DIAGACHP_DISABLE        (_RFSENSE_DIAGCTRL_DIAGACHP_DISABLE << 1)    /**< Shifted mode DISABLE for RFSENSE_DIAGCTRL   */
#define RFSENSE_DIAGCTRL_DIAGACHP_ENABLE         (_RFSENSE_DIAGCTRL_DIAGACHP_ENABLE << 1)     /**< Shifted mode ENABLE for RFSENSE_DIAGCTRL    */

/* Bit fields for RFSENSE STATUS */
#define _RFSENSE_STATUS_RESETVALUE               0x00000000UL                                 /**< Default value for RFSENSE_STATUS            */
#define _RFSENSE_STATUS_MASK                     0x0FF73F1FUL                                 /**< Mask for RFSENSE_STATUS                     */
#define RFSENSE_STATUS_RFSENSEDATA               (0x1UL << 0)                                 /**< RFSENSE data input from analog              */
#define _RFSENSE_STATUS_RFSENSEDATA_SHIFT        0                                            /**< Shift value for RFSENSE_RFSENSEDATA         */
#define _RFSENSE_STATUS_RFSENSEDATA_MASK         0x1UL                                        /**< Bit mask for RFSENSE_RFSENSEDATA            */
#define _RFSENSE_STATUS_RFSENSEDATA_DEFAULT      0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_RFSENSEDATA_DEFAULT       (_RFSENSE_STATUS_RFSENSEDATA_DEFAULT << 0)   /**< Shifted mode DEFAULT for RFSENSE_STATUS     */
#define RFSENSE_STATUS_FIRSTTRIPDONE             (0x1UL << 1)                                 /**< First RFSENSE trip done                     */
#define _RFSENSE_STATUS_FIRSTTRIPDONE_SHIFT      1                                            /**< Shift value for RFSENSE_FIRSTTRIPDONE       */
#define _RFSENSE_STATUS_FIRSTTRIPDONE_MASK       0x2UL                                        /**< Bit mask for RFSENSE_FIRSTTRIPDONE          */
#define _RFSENSE_STATUS_FIRSTTRIPDONE_DEFAULT    0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_FIRSTTRIPDONE_DEFAULT     (_RFSENSE_STATUS_FIRSTTRIPDONE_DEFAULT << 1) /**< Shifted mode DEFAULT for RFSENSE_STATUS     */
#define RFSENSE_STATUS_PREAMBLEDET               (0x1UL << 2)                                 /**< Preamble Detected                           */
#define _RFSENSE_STATUS_PREAMBLEDET_SHIFT        2                                            /**< Shift value for RFSENSE_PREAMBLEDET         */
#define _RFSENSE_STATUS_PREAMBLEDET_MASK         0x4UL                                        /**< Bit mask for RFSENSE_PREAMBLEDET            */
#define _RFSENSE_STATUS_PREAMBLEDET_DEFAULT      0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_PREAMBLEDET_DEFAULT       (_RFSENSE_STATUS_PREAMBLEDET_DEFAULT << 2)   /**< Shifted mode DEFAULT for RFSENSE_STATUS     */
#define RFSENSE_STATUS_SYNCDET                   (0x1UL << 3)                                 /**< Syncword Detected                           */
#define _RFSENSE_STATUS_SYNCDET_SHIFT            3                                            /**< Shift value for RFSENSE_SYNCDET             */
#define _RFSENSE_STATUS_SYNCDET_MASK             0x8UL                                        /**< Bit mask for RFSENSE_SYNCDET                */
#define _RFSENSE_STATUS_SYNCDET_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_SYNCDET_DEFAULT           (_RFSENSE_STATUS_SYNCDET_DEFAULT << 3)       /**< Shifted mode DEFAULT for RFSENSE_STATUS     */
#define RFSENSE_STATUS_CALRUNNING                (0x1UL << 4)                                 /**< Calibration Running                         */
#define _RFSENSE_STATUS_CALRUNNING_SHIFT         4                                            /**< Shift value for RFSENSE_CALRUNNING          */
#define _RFSENSE_STATUS_CALRUNNING_MASK          0x10UL                                       /**< Bit mask for RFSENSE_CALRUNNING             */
#define _RFSENSE_STATUS_CALRUNNING_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_CALRUNNING_DEFAULT        (_RFSENSE_STATUS_CALRUNNING_DEFAULT << 4)    /**< Shifted mode DEFAULT for RFSENSE_STATUS     */
#define _RFSENSE_STATUS_CALTRIMRES_SHIFT         8                                            /**< Shift value for RFSENSE_CALTRIMRES          */
#define _RFSENSE_STATUS_CALTRIMRES_MASK          0x3F00UL                                     /**< Bit mask for RFSENSE_CALTRIMRES             */
#define _RFSENSE_STATUS_CALTRIMRES_DEFAULT       0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_CALTRIMRES_DEFAULT        (_RFSENSE_STATUS_CALTRIMRES_DEFAULT << 8)    /**< Shifted mode DEFAULT for RFSENSE_STATUS     */
#define _RFSENSE_STATUS_CALTRIMCURR_SHIFT        16                                           /**< Shift value for RFSENSE_CALTRIMCURR         */
#define _RFSENSE_STATUS_CALTRIMCURR_MASK         0x70000UL                                    /**< Bit mask for RFSENSE_CALTRIMCURR            */
#define _RFSENSE_STATUS_CALTRIMCURR_DEFAULT      0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_CALTRIMCURR_DEFAULT       (_RFSENSE_STATUS_CALTRIMCURR_DEFAULT << 16)  /**< Shifted mode DEFAULT for RFSENSE_STATUS     */
#define _RFSENSE_STATUS_CALTRIMCM_SHIFT          20                                           /**< Shift value for RFSENSE_CALTRIMCM           */
#define _RFSENSE_STATUS_CALTRIMCM_MASK           0xFF00000UL                                  /**< Bit mask for RFSENSE_CALTRIMCM              */
#define _RFSENSE_STATUS_CALTRIMCM_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for RFSENSE_STATUS             */
#define RFSENSE_STATUS_CALTRIMCM_DEFAULT         (_RFSENSE_STATUS_CALTRIMCM_DEFAULT << 20)    /**< Shifted mode DEFAULT for RFSENSE_STATUS     */

/** @} End of group EFR32MG22_RFSENSE_BitFields */
/** @} End of group EFR32MG22_RFSENSE */
/** @} End of group Parts */
#endif /* EFR32MG22_RFSENSE_H */
