/**************************************************************************//**
 * @file
 * @brief EFR32MG21 EMU register and bit field definitions
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
#ifndef EFR32MG21_EMU_H
#define EFR32MG21_EMU_H
#define EMU_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_EMU EMU
 * @{
 * @brief EFR32MG21 EMU Register Declaration.
 *****************************************************************************/

/** EMU Register Declaration. */
typedef struct {
  uint32_t       RESERVED0[4U];                 /**< Reserved for future use                            */
  __IOM uint32_t DECBOD;                        /**< DECOUPLE LVBOD  Control register                   */
  uint32_t       RESERVED1[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t BOD3SENSE;                     /**< BOD3SENSE Control register                         */
  uint32_t       RESERVED2[15U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< EMU Configuration lock register                    */
  __IOM uint32_t IF;                            /**< Interrupt Flags                                    */
  __IOM uint32_t IEN;                           /**< Interrupt Enables                                  */
  __IOM uint32_t EM4CTRL;                       /**< EM4 Control                                        */
  __IOM uint32_t CMD;                           /**< EMU Command register                               */
  __IOM uint32_t CTRL;                          /**< EMU Control register                               */
  __IOM uint32_t TEMPLIMITS;                    /**< EMU Temperature thresholds                         */
  uint32_t       RESERVED3[2U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< EMU Status register                                */
  __IM uint32_t  TEMP;                          /**< Temperature                                        */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t RSTCTRL;                       /**< Reset Management Control register                  */
  __IM uint32_t  RSTCAUSE;                      /**< Reset cause                                        */
  uint32_t       RESERVED5[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t DGIF;                          /**< Interrupt Flags Debug                              */
  __IOM uint32_t DGIEN;                         /**< Interrupt Enables Debug                            */
  __IOM uint32_t SEIF;                          /**< Interrupt Flags Secure Element                     */
  __IOM uint32_t SEIEN;                         /**< Interrupt Enables Secure Elements                  */
  uint32_t       RESERVED6[4U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED8[975U];               /**< Reserved for future use                            */
  uint32_t       RESERVED9[4U];                 /**< Reserved for future use                            */
  __IOM uint32_t DECBOD_SET;                    /**< DECOUPLE LVBOD  Control register                   */
  uint32_t       RESERVED10[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BOD3SENSE_SET;                 /**< BOD3SENSE Control register                         */
  uint32_t       RESERVED11[15U];               /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< EMU Configuration lock register                    */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enables                                  */
  __IOM uint32_t EM4CTRL_SET;                   /**< EM4 Control                                        */
  __IOM uint32_t CMD_SET;                       /**< EMU Command register                               */
  __IOM uint32_t CTRL_SET;                      /**< EMU Control register                               */
  __IOM uint32_t TEMPLIMITS_SET;                /**< EMU Temperature thresholds                         */
  uint32_t       RESERVED12[2U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< EMU Status register                                */
  __IM uint32_t  TEMP_SET;                      /**< Temperature                                        */
  uint32_t       RESERVED13[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RSTCTRL_SET;                   /**< Reset Management Control register                  */
  __IM uint32_t  RSTCAUSE_SET;                  /**< Reset cause                                        */
  uint32_t       RESERVED14[2U];                /**< Reserved for future use                            */
  __IOM uint32_t DGIF_SET;                      /**< Interrupt Flags Debug                              */
  __IOM uint32_t DGIEN_SET;                     /**< Interrupt Enables Debug                            */
  __IOM uint32_t SEIF_SET;                      /**< Interrupt Flags Secure Element                     */
  __IOM uint32_t SEIEN_SET;                     /**< Interrupt Enables Secure Elements                  */
  uint32_t       RESERVED15[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED16[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED17[975U];              /**< Reserved for future use                            */
  uint32_t       RESERVED18[4U];                /**< Reserved for future use                            */
  __IOM uint32_t DECBOD_CLR;                    /**< DECOUPLE LVBOD  Control register                   */
  uint32_t       RESERVED19[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BOD3SENSE_CLR;                 /**< BOD3SENSE Control register                         */
  uint32_t       RESERVED20[15U];               /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< EMU Configuration lock register                    */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enables                                  */
  __IOM uint32_t EM4CTRL_CLR;                   /**< EM4 Control                                        */
  __IOM uint32_t CMD_CLR;                       /**< EMU Command register                               */
  __IOM uint32_t CTRL_CLR;                      /**< EMU Control register                               */
  __IOM uint32_t TEMPLIMITS_CLR;                /**< EMU Temperature thresholds                         */
  uint32_t       RESERVED21[2U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< EMU Status register                                */
  __IM uint32_t  TEMP_CLR;                      /**< Temperature                                        */
  uint32_t       RESERVED22[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RSTCTRL_CLR;                   /**< Reset Management Control register                  */
  __IM uint32_t  RSTCAUSE_CLR;                  /**< Reset cause                                        */
  uint32_t       RESERVED23[2U];                /**< Reserved for future use                            */
  __IOM uint32_t DGIF_CLR;                      /**< Interrupt Flags Debug                              */
  __IOM uint32_t DGIEN_CLR;                     /**< Interrupt Enables Debug                            */
  __IOM uint32_t SEIF_CLR;                      /**< Interrupt Flags Secure Element                     */
  __IOM uint32_t SEIEN_CLR;                     /**< Interrupt Enables Secure Elements                  */
  uint32_t       RESERVED24[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED25[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED26[975U];              /**< Reserved for future use                            */
  uint32_t       RESERVED27[4U];                /**< Reserved for future use                            */
  __IOM uint32_t DECBOD_TGL;                    /**< DECOUPLE LVBOD  Control register                   */
  uint32_t       RESERVED28[3U];                /**< Reserved for future use                            */
  __IOM uint32_t BOD3SENSE_TGL;                 /**< BOD3SENSE Control register                         */
  uint32_t       RESERVED29[15U];               /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< EMU Configuration lock register                    */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enables                                  */
  __IOM uint32_t EM4CTRL_TGL;                   /**< EM4 Control                                        */
  __IOM uint32_t CMD_TGL;                       /**< EMU Command register                               */
  __IOM uint32_t CTRL_TGL;                      /**< EMU Control register                               */
  __IOM uint32_t TEMPLIMITS_TGL;                /**< EMU Temperature thresholds                         */
  uint32_t       RESERVED30[2U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< EMU Status register                                */
  __IM uint32_t  TEMP_TGL;                      /**< Temperature                                        */
  uint32_t       RESERVED31[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RSTCTRL_TGL;                   /**< Reset Management Control register                  */
  __IM uint32_t  RSTCAUSE_TGL;                  /**< Reset cause                                        */
  uint32_t       RESERVED32[2U];                /**< Reserved for future use                            */
  __IOM uint32_t DGIF_TGL;                      /**< Interrupt Flags Debug                              */
  __IOM uint32_t DGIEN_TGL;                     /**< Interrupt Enables Debug                            */
  __IOM uint32_t SEIF_TGL;                      /**< Interrupt Flags Secure Element                     */
  __IOM uint32_t SEIEN_TGL;                     /**< Interrupt Enables Secure Elements                  */
  uint32_t       RESERVED33[4U];                /**< Reserved for future use                            */
  uint32_t       RESERVED34[1U];                /**< Reserved for future use                            */
} EMU_TypeDef;
/** @} End of group EFR32MG21_EMU */

/**************************************************************************//**
 * @addtogroup EFR32MG21_EMU
 * @{
 * @defgroup EFR32MG21_EMU_BitFields EMU Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for EMU DECBOD */
#define _EMU_DECBOD_RESETVALUE                  0x00000022UL                             /**< Default value for EMU_DECBOD                */
#define _EMU_DECBOD_MASK                        0x00000033UL                             /**< Mask for EMU_DECBOD                         */
#define EMU_DECBOD_DECBODEN                     (0x1UL << 0)                             /**< DECBOD enable                               */
#define _EMU_DECBOD_DECBODEN_SHIFT              0                                        /**< Shift value for EMU_DECBODEN                */
#define _EMU_DECBOD_DECBODEN_MASK               0x1UL                                    /**< Bit mask for EMU_DECBODEN                   */
#define _EMU_DECBOD_DECBODEN_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for EMU_DECBOD                 */
#define EMU_DECBOD_DECBODEN_DEFAULT             (_EMU_DECBOD_DECBODEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for EMU_DECBOD         */
#define EMU_DECBOD_DECBODMASK                   (0x1UL << 1)                             /**< DECBOD Mask                                 */
#define _EMU_DECBOD_DECBODMASK_SHIFT            1                                        /**< Shift value for EMU_DECBODMASK              */
#define _EMU_DECBOD_DECBODMASK_MASK             0x2UL                                    /**< Bit mask for EMU_DECBODMASK                 */
#define _EMU_DECBOD_DECBODMASK_DEFAULT          0x00000001UL                             /**< Mode DEFAULT for EMU_DECBOD                 */
#define EMU_DECBOD_DECBODMASK_DEFAULT           (_EMU_DECBOD_DECBODMASK_DEFAULT << 1)    /**< Shifted mode DEFAULT for EMU_DECBOD         */
#define EMU_DECBOD_DECOVMBODEN                  (0x1UL << 4)                             /**< Over Voltage Monitor enable                 */
#define _EMU_DECBOD_DECOVMBODEN_SHIFT           4                                        /**< Shift value for EMU_DECOVMBODEN             */
#define _EMU_DECBOD_DECOVMBODEN_MASK            0x10UL                                   /**< Bit mask for EMU_DECOVMBODEN                */
#define _EMU_DECBOD_DECOVMBODEN_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for EMU_DECBOD                 */
#define EMU_DECBOD_DECOVMBODEN_DEFAULT          (_EMU_DECBOD_DECOVMBODEN_DEFAULT << 4)   /**< Shifted mode DEFAULT for EMU_DECBOD         */
#define EMU_DECBOD_DECOVMBODMASK                (0x1UL << 5)                             /**< Over Voltage Monitor Mask                   */
#define _EMU_DECBOD_DECOVMBODMASK_SHIFT         5                                        /**< Shift value for EMU_DECOVMBODMASK           */
#define _EMU_DECBOD_DECOVMBODMASK_MASK          0x20UL                                   /**< Bit mask for EMU_DECOVMBODMASK              */
#define _EMU_DECBOD_DECOVMBODMASK_DEFAULT       0x00000001UL                             /**< Mode DEFAULT for EMU_DECBOD                 */
#define EMU_DECBOD_DECOVMBODMASK_DEFAULT        (_EMU_DECBOD_DECOVMBODMASK_DEFAULT << 5) /**< Shifted mode DEFAULT for EMU_DECBOD         */

/* Bit fields for EMU BOD3SENSE */
#define _EMU_BOD3SENSE_RESETVALUE               0x00000000UL                              /**< Default value for EMU_BOD3SENSE             */
#define _EMU_BOD3SENSE_MASK                     0x00000077UL                              /**< Mask for EMU_BOD3SENSE                      */
#define EMU_BOD3SENSE_AVDDBODEN                 (0x1UL << 0)                              /**< AVDD BOD enable                             */
#define _EMU_BOD3SENSE_AVDDBODEN_SHIFT          0                                         /**< Shift value for EMU_AVDDBODEN               */
#define _EMU_BOD3SENSE_AVDDBODEN_MASK           0x1UL                                     /**< Bit mask for EMU_AVDDBODEN                  */
#define _EMU_BOD3SENSE_AVDDBODEN_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for EMU_BOD3SENSE              */
#define EMU_BOD3SENSE_AVDDBODEN_DEFAULT         (_EMU_BOD3SENSE_AVDDBODEN_DEFAULT << 0)   /**< Shifted mode DEFAULT for EMU_BOD3SENSE      */
#define EMU_BOD3SENSE_IOVDD0BODEN               (0x1UL << 1)                              /**< VDDIO0 BOD enable                           */
#define _EMU_BOD3SENSE_IOVDD0BODEN_SHIFT        1                                         /**< Shift value for EMU_IOVDD0BODEN             */
#define _EMU_BOD3SENSE_IOVDD0BODEN_MASK         0x2UL                                     /**< Bit mask for EMU_IOVDD0BODEN                */
#define _EMU_BOD3SENSE_IOVDD0BODEN_DEFAULT      0x00000000UL                              /**< Mode DEFAULT for EMU_BOD3SENSE              */
#define EMU_BOD3SENSE_IOVDD0BODEN_DEFAULT       (_EMU_BOD3SENSE_IOVDD0BODEN_DEFAULT << 1) /**< Shifted mode DEFAULT for EMU_BOD3SENSE      */
#define EMU_BOD3SENSE_IOVDD1BODEN               (0x1UL << 2)                              /**< VDDIO1 BOD enable                           */
#define _EMU_BOD3SENSE_IOVDD1BODEN_SHIFT        2                                         /**< Shift value for EMU_IOVDD1BODEN             */
#define _EMU_BOD3SENSE_IOVDD1BODEN_MASK         0x4UL                                     /**< Bit mask for EMU_IOVDD1BODEN                */
#define _EMU_BOD3SENSE_IOVDD1BODEN_DEFAULT      0x00000000UL                              /**< Mode DEFAULT for EMU_BOD3SENSE              */
#define EMU_BOD3SENSE_IOVDD1BODEN_DEFAULT       (_EMU_BOD3SENSE_IOVDD1BODEN_DEFAULT << 2) /**< Shifted mode DEFAULT for EMU_BOD3SENSE      */

/* Bit fields for EMU LOCK */
#define _EMU_LOCK_RESETVALUE                    0x0000ADE8UL                            /**< Default value for EMU_LOCK                  */
#define _EMU_LOCK_MASK                          0x0000FFFFUL                            /**< Mask for EMU_LOCK                           */
#define _EMU_LOCK_LOCKKEY_SHIFT                 0                                       /**< Shift value for EMU_LOCKKEY                 */
#define _EMU_LOCK_LOCKKEY_MASK                  0xFFFFUL                                /**< Bit mask for EMU_LOCKKEY                    */
#define _EMU_LOCK_LOCKKEY_DEFAULT               0x0000ADE8UL                            /**< Mode DEFAULT for EMU_LOCK                   */
#define _EMU_LOCK_LOCKKEY_UNLOCK                0x0000ADE8UL                            /**< Mode UNLOCK for EMU_LOCK                    */
#define EMU_LOCK_LOCKKEY_DEFAULT                (_EMU_LOCK_LOCKKEY_DEFAULT << 0)        /**< Shifted mode DEFAULT for EMU_LOCK           */
#define EMU_LOCK_LOCKKEY_UNLOCK                 (_EMU_LOCK_LOCKKEY_UNLOCK << 0)         /**< Shifted mode UNLOCK for EMU_LOCK            */

/* Bit fields for EMU IF */
#define _EMU_IF_RESETVALUE                      0x00000000UL                            /**< Default value for EMU_IF                    */
#define _EMU_IF_MASK                            0xE3070000UL                            /**< Mask for EMU_IF                             */
#define EMU_IF_AVDDBOD                          (0x1UL << 16)                           /**< AVDD BOD Interrupt flag                     */
#define _EMU_IF_AVDDBOD_SHIFT                   16                                      /**< Shift value for EMU_AVDDBOD                 */
#define _EMU_IF_AVDDBOD_MASK                    0x10000UL                               /**< Bit mask for EMU_AVDDBOD                    */
#define _EMU_IF_AVDDBOD_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EMU_IF                     */
#define EMU_IF_AVDDBOD_DEFAULT                  (_EMU_IF_AVDDBOD_DEFAULT << 16)         /**< Shifted mode DEFAULT for EMU_IF             */
#define EMU_IF_IOVDD0BOD                        (0x1UL << 17)                           /**< VDDIO0 BOD Interrupt flag                   */
#define _EMU_IF_IOVDD0BOD_SHIFT                 17                                      /**< Shift value for EMU_IOVDD0BOD               */
#define _EMU_IF_IOVDD0BOD_MASK                  0x20000UL                               /**< Bit mask for EMU_IOVDD0BOD                  */
#define _EMU_IF_IOVDD0BOD_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_IF                     */
#define EMU_IF_IOVDD0BOD_DEFAULT                (_EMU_IF_IOVDD0BOD_DEFAULT << 17)       /**< Shifted mode DEFAULT for EMU_IF             */
#define EMU_IF_EM23WAKEUP                       (0x1UL << 24)                           /**< EM23 Wake up Interrupt flag                 */
#define _EMU_IF_EM23WAKEUP_SHIFT                24                                      /**< Shift value for EMU_EM23WAKEUP              */
#define _EMU_IF_EM23WAKEUP_MASK                 0x1000000UL                             /**< Bit mask for EMU_EM23WAKEUP                 */
#define _EMU_IF_EM23WAKEUP_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EMU_IF                     */
#define EMU_IF_EM23WAKEUP_DEFAULT               (_EMU_IF_EM23WAKEUP_DEFAULT << 24)      /**< Shifted mode DEFAULT for EMU_IF             */
#define EMU_IF_TEMP                             (0x1UL << 29)                           /**< Temperature Interrupt flag                  */
#define _EMU_IF_TEMP_SHIFT                      29                                      /**< Shift value for EMU_TEMP                    */
#define _EMU_IF_TEMP_MASK                       0x20000000UL                            /**< Bit mask for EMU_TEMP                       */
#define _EMU_IF_TEMP_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for EMU_IF                     */
#define EMU_IF_TEMP_DEFAULT                     (_EMU_IF_TEMP_DEFAULT << 29)            /**< Shifted mode DEFAULT for EMU_IF             */
#define EMU_IF_TEMPLOW                          (0x1UL << 30)                           /**< Temperature low Interrupt flag              */
#define _EMU_IF_TEMPLOW_SHIFT                   30                                      /**< Shift value for EMU_TEMPLOW                 */
#define _EMU_IF_TEMPLOW_MASK                    0x40000000UL                            /**< Bit mask for EMU_TEMPLOW                    */
#define _EMU_IF_TEMPLOW_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EMU_IF                     */
#define EMU_IF_TEMPLOW_DEFAULT                  (_EMU_IF_TEMPLOW_DEFAULT << 30)         /**< Shifted mode DEFAULT for EMU_IF             */
#define EMU_IF_TEMPHIGH                         (0x1UL << 31)                           /**< Temperature high Interrupt flag             */
#define _EMU_IF_TEMPHIGH_SHIFT                  31                                      /**< Shift value for EMU_TEMPHIGH                */
#define _EMU_IF_TEMPHIGH_MASK                   0x80000000UL                            /**< Bit mask for EMU_TEMPHIGH                   */
#define _EMU_IF_TEMPHIGH_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EMU_IF                     */
#define EMU_IF_TEMPHIGH_DEFAULT                 (_EMU_IF_TEMPHIGH_DEFAULT << 31)        /**< Shifted mode DEFAULT for EMU_IF             */

/* Bit fields for EMU IEN */
#define _EMU_IEN_RESETVALUE                     0x00000000UL                            /**< Default value for EMU_IEN                   */
#define _EMU_IEN_MASK                           0xE3070000UL                            /**< Mask for EMU_IEN                            */
#define EMU_IEN_AVDDBOD                         (0x1UL << 16)                           /**< AVDD BOD Interrupt enable                   */
#define _EMU_IEN_AVDDBOD_SHIFT                  16                                      /**< Shift value for EMU_AVDDBOD                 */
#define _EMU_IEN_AVDDBOD_MASK                   0x10000UL                               /**< Bit mask for EMU_AVDDBOD                    */
#define _EMU_IEN_AVDDBOD_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EMU_IEN                    */
#define EMU_IEN_AVDDBOD_DEFAULT                 (_EMU_IEN_AVDDBOD_DEFAULT << 16)        /**< Shifted mode DEFAULT for EMU_IEN            */
#define EMU_IEN_IOVDD0BOD                       (0x1UL << 17)                           /**< VDDIO0 BOD Interrupt enable                 */
#define _EMU_IEN_IOVDD0BOD_SHIFT                17                                      /**< Shift value for EMU_IOVDD0BOD               */
#define _EMU_IEN_IOVDD0BOD_MASK                 0x20000UL                               /**< Bit mask for EMU_IOVDD0BOD                  */
#define _EMU_IEN_IOVDD0BOD_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EMU_IEN                    */
#define EMU_IEN_IOVDD0BOD_DEFAULT               (_EMU_IEN_IOVDD0BOD_DEFAULT << 17)      /**< Shifted mode DEFAULT for EMU_IEN            */
#define EMU_IEN_EM23WAKEUP                      (0x1UL << 24)                           /**< EM23 Wake up Interrupt enable               */
#define _EMU_IEN_EM23WAKEUP_SHIFT               24                                      /**< Shift value for EMU_EM23WAKEUP              */
#define _EMU_IEN_EM23WAKEUP_MASK                0x1000000UL                             /**< Bit mask for EMU_EM23WAKEUP                 */
#define _EMU_IEN_EM23WAKEUP_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EMU_IEN                    */
#define EMU_IEN_EM23WAKEUP_DEFAULT              (_EMU_IEN_EM23WAKEUP_DEFAULT << 24)     /**< Shifted mode DEFAULT for EMU_IEN            */
#define EMU_IEN_TEMP                            (0x1UL << 29)                           /**< Temperature Interrupt enable                */
#define _EMU_IEN_TEMP_SHIFT                     29                                      /**< Shift value for EMU_TEMP                    */
#define _EMU_IEN_TEMP_MASK                      0x20000000UL                            /**< Bit mask for EMU_TEMP                       */
#define _EMU_IEN_TEMP_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for EMU_IEN                    */
#define EMU_IEN_TEMP_DEFAULT                    (_EMU_IEN_TEMP_DEFAULT << 29)           /**< Shifted mode DEFAULT for EMU_IEN            */
#define EMU_IEN_TEMPLOW                         (0x1UL << 30)                           /**< Temperature low Interrupt enable            */
#define _EMU_IEN_TEMPLOW_SHIFT                  30                                      /**< Shift value for EMU_TEMPLOW                 */
#define _EMU_IEN_TEMPLOW_MASK                   0x40000000UL                            /**< Bit mask for EMU_TEMPLOW                    */
#define _EMU_IEN_TEMPLOW_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for EMU_IEN                    */
#define EMU_IEN_TEMPLOW_DEFAULT                 (_EMU_IEN_TEMPLOW_DEFAULT << 30)        /**< Shifted mode DEFAULT for EMU_IEN            */
#define EMU_IEN_TEMPHIGH                        (0x1UL << 31)                           /**< Temperature high Interrupt enable           */
#define _EMU_IEN_TEMPHIGH_SHIFT                 31                                      /**< Shift value for EMU_TEMPHIGH                */
#define _EMU_IEN_TEMPHIGH_MASK                  0x80000000UL                            /**< Bit mask for EMU_TEMPHIGH                   */
#define _EMU_IEN_TEMPHIGH_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_IEN                    */
#define EMU_IEN_TEMPHIGH_DEFAULT                (_EMU_IEN_TEMPHIGH_DEFAULT << 31)       /**< Shifted mode DEFAULT for EMU_IEN            */

/* Bit fields for EMU EM4CTRL */
#define _EMU_EM4CTRL_RESETVALUE                 0x00000000UL                               /**< Default value for EMU_EM4CTRL               */
#define _EMU_EM4CTRL_MASK                       0x00000033UL                               /**< Mask for EMU_EM4CTRL                        */
#define _EMU_EM4CTRL_EM4ENTRY_SHIFT             0                                          /**< Shift value for EMU_EM4ENTRY                */
#define _EMU_EM4CTRL_EM4ENTRY_MASK              0x3UL                                      /**< Bit mask for EMU_EM4ENTRY                   */
#define _EMU_EM4CTRL_EM4ENTRY_DEFAULT           0x00000000UL                               /**< Mode DEFAULT for EMU_EM4CTRL                */
#define EMU_EM4CTRL_EM4ENTRY_DEFAULT            (_EMU_EM4CTRL_EM4ENTRY_DEFAULT << 0)       /**< Shifted mode DEFAULT for EMU_EM4CTRL        */
#define _EMU_EM4CTRL_EM4IORETMODE_SHIFT         4                                          /**< Shift value for EMU_EM4IORETMODE            */
#define _EMU_EM4CTRL_EM4IORETMODE_MASK          0x30UL                                     /**< Bit mask for EMU_EM4IORETMODE               */
#define _EMU_EM4CTRL_EM4IORETMODE_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for EMU_EM4CTRL                */
#define _EMU_EM4CTRL_EM4IORETMODE_DISABLE       0x00000000UL                               /**< Mode DISABLE for EMU_EM4CTRL                */
#define _EMU_EM4CTRL_EM4IORETMODE_EM4EXIT       0x00000001UL                               /**< Mode EM4EXIT for EMU_EM4CTRL                */
#define _EMU_EM4CTRL_EM4IORETMODE_SWUNLATCH     0x00000002UL                               /**< Mode SWUNLATCH for EMU_EM4CTRL              */
#define EMU_EM4CTRL_EM4IORETMODE_DEFAULT        (_EMU_EM4CTRL_EM4IORETMODE_DEFAULT << 4)   /**< Shifted mode DEFAULT for EMU_EM4CTRL        */
#define EMU_EM4CTRL_EM4IORETMODE_DISABLE        (_EMU_EM4CTRL_EM4IORETMODE_DISABLE << 4)   /**< Shifted mode DISABLE for EMU_EM4CTRL        */
#define EMU_EM4CTRL_EM4IORETMODE_EM4EXIT        (_EMU_EM4CTRL_EM4IORETMODE_EM4EXIT << 4)   /**< Shifted mode EM4EXIT for EMU_EM4CTRL        */
#define EMU_EM4CTRL_EM4IORETMODE_SWUNLATCH      (_EMU_EM4CTRL_EM4IORETMODE_SWUNLATCH << 4) /**< Shifted mode SWUNLATCH for EMU_EM4CTRL      */

/* Bit fields for EMU CMD */
#define _EMU_CMD_RESETVALUE                     0x00000000UL                            /**< Default value for EMU_CMD                   */
#define _EMU_CMD_MASK                           0x00020E02UL                            /**< Mask for EMU_CMD                            */
#define EMU_CMD_EM4UNLATCH                      (0x1UL << 1)                            /**< EM4 unlatch                                 */
#define _EMU_CMD_EM4UNLATCH_SHIFT               1                                       /**< Shift value for EMU_EM4UNLATCH              */
#define _EMU_CMD_EM4UNLATCH_MASK                0x2UL                                   /**< Bit mask for EMU_EM4UNLATCH                 */
#define _EMU_CMD_EM4UNLATCH_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EMU_CMD                    */
#define EMU_CMD_EM4UNLATCH_DEFAULT              (_EMU_CMD_EM4UNLATCH_DEFAULT << 1)      /**< Shifted mode DEFAULT for EMU_CMD            */
#define EMU_CMD_RSTCAUSECLR                     (0x1UL << 17)                           /**< Reset Cause Clear                           */
#define _EMU_CMD_RSTCAUSECLR_SHIFT              17                                      /**< Shift value for EMU_RSTCAUSECLR             */
#define _EMU_CMD_RSTCAUSECLR_MASK               0x20000UL                               /**< Bit mask for EMU_RSTCAUSECLR                */
#define _EMU_CMD_RSTCAUSECLR_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for EMU_CMD                    */
#define EMU_CMD_RSTCAUSECLR_DEFAULT             (_EMU_CMD_RSTCAUSECLR_DEFAULT << 17)    /**< Shifted mode DEFAULT for EMU_CMD            */

/* Bit fields for EMU CTRL */
#define _EMU_CTRL_RESETVALUE                    0x0000A200UL                                 /**< Default value for EMU_CTRL                  */
#define _EMU_CTRL_MASK                          0x0001F303UL                                 /**< Mask for EMU_CTRL                           */
#define EMU_CTRL_EM2DBGEN                       (0x1UL << 0)                                 /**< Enable debugging in EM2                     */
#define _EMU_CTRL_EM2DBGEN_SHIFT                0                                            /**< Shift value for EMU_EM2DBGEN                */
#define _EMU_CTRL_EM2DBGEN_MASK                 0x1UL                                        /**< Bit mask for EMU_EM2DBGEN                   */
#define _EMU_CTRL_EM2DBGEN_DEFAULT              0x00000000UL                                 /**< Mode DEFAULT for EMU_CTRL                   */
#define EMU_CTRL_EM2DBGEN_DEFAULT               (_EMU_CTRL_EM2DBGEN_DEFAULT << 0)            /**< Shifted mode DEFAULT for EMU_CTRL           */
#define EMU_CTRL_FLASHPWRUPONDEMAND             (0x1UL << 16)                                /**< Enable flash on demand wakeup               */
#define _EMU_CTRL_FLASHPWRUPONDEMAND_SHIFT      16                                           /**< Shift value for EMU_FLASHPWRUPONDEMAND      */
#define _EMU_CTRL_FLASHPWRUPONDEMAND_MASK       0x10000UL                                    /**< Bit mask for EMU_FLASHPWRUPONDEMAND         */
#define _EMU_CTRL_FLASHPWRUPONDEMAND_DEFAULT    0x00000000UL                                 /**< Mode DEFAULT for EMU_CTRL                   */
#define EMU_CTRL_FLASHPWRUPONDEMAND_DEFAULT     (_EMU_CTRL_FLASHPWRUPONDEMAND_DEFAULT << 16) /**< Shifted mode DEFAULT for EMU_CTRL           */

/* Bit fields for EMU TEMPLIMITS */
#define _EMU_TEMPLIMITS_RESETVALUE              0x01FF0000UL                             /**< Default value for EMU_TEMPLIMITS            */
#define _EMU_TEMPLIMITS_MASK                    0x01FF01FFUL                             /**< Mask for EMU_TEMPLIMITS                     */
#define _EMU_TEMPLIMITS_TEMPLOW_SHIFT           0                                        /**< Shift value for EMU_TEMPLOW                 */
#define _EMU_TEMPLIMITS_TEMPLOW_MASK            0x1FFUL                                  /**< Bit mask for EMU_TEMPLOW                    */
#define _EMU_TEMPLIMITS_TEMPLOW_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for EMU_TEMPLIMITS             */
#define EMU_TEMPLIMITS_TEMPLOW_DEFAULT          (_EMU_TEMPLIMITS_TEMPLOW_DEFAULT << 0)   /**< Shifted mode DEFAULT for EMU_TEMPLIMITS     */
#define _EMU_TEMPLIMITS_TEMPHIGH_SHIFT          16                                       /**< Shift value for EMU_TEMPHIGH                */
#define _EMU_TEMPLIMITS_TEMPHIGH_MASK           0x1FF0000UL                              /**< Bit mask for EMU_TEMPHIGH                   */
#define _EMU_TEMPLIMITS_TEMPHIGH_DEFAULT        0x000001FFUL                             /**< Mode DEFAULT for EMU_TEMPLIMITS             */
#define EMU_TEMPLIMITS_TEMPHIGH_DEFAULT         (_EMU_TEMPLIMITS_TEMPHIGH_DEFAULT << 16) /**< Shifted mode DEFAULT for EMU_TEMPLIMITS     */

/* Bit fields for EMU STATUS */
#define _EMU_STATUS_RESETVALUE                  0x00000080UL                             /**< Default value for EMU_STATUS                */
#define _EMU_STATUS_MASK                        0x000054F7UL                             /**< Mask for EMU_STATUS                         */
#define EMU_STATUS_LOCK                         (0x1UL << 0)                             /**< Lock status                                 */
#define _EMU_STATUS_LOCK_SHIFT                  0                                        /**< Shift value for EMU_LOCK                    */
#define _EMU_STATUS_LOCK_MASK                   0x1UL                                    /**< Bit mask for EMU_LOCK                       */
#define _EMU_STATUS_LOCK_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for EMU_STATUS                 */
#define _EMU_STATUS_LOCK_UNLOCKED               0x00000000UL                             /**< Mode UNLOCKED for EMU_STATUS                */
#define _EMU_STATUS_LOCK_LOCKED                 0x00000001UL                             /**< Mode LOCKED for EMU_STATUS                  */
#define EMU_STATUS_LOCK_DEFAULT                 (_EMU_STATUS_LOCK_DEFAULT << 0)          /**< Shifted mode DEFAULT for EMU_STATUS         */
#define EMU_STATUS_LOCK_UNLOCKED                (_EMU_STATUS_LOCK_UNLOCKED << 0)         /**< Shifted mode UNLOCKED for EMU_STATUS        */
#define EMU_STATUS_LOCK_LOCKED                  (_EMU_STATUS_LOCK_LOCKED << 0)           /**< Shifted mode LOCKED for EMU_STATUS          */
#define EMU_STATUS_FIRSTTEMPDONE                (0x1UL << 1)                             /**< First Temp done                             */
#define _EMU_STATUS_FIRSTTEMPDONE_SHIFT         1                                        /**< Shift value for EMU_FIRSTTEMPDONE           */
#define _EMU_STATUS_FIRSTTEMPDONE_MASK          0x2UL                                    /**< Bit mask for EMU_FIRSTTEMPDONE              */
#define _EMU_STATUS_FIRSTTEMPDONE_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for EMU_STATUS                 */
#define EMU_STATUS_FIRSTTEMPDONE_DEFAULT        (_EMU_STATUS_FIRSTTEMPDONE_DEFAULT << 1) /**< Shifted mode DEFAULT for EMU_STATUS         */
#define EMU_STATUS_TEMPACTIVE                   (0x1UL << 2)                             /**< Temp active                                 */
#define _EMU_STATUS_TEMPACTIVE_SHIFT            2                                        /**< Shift value for EMU_TEMPACTIVE              */
#define _EMU_STATUS_TEMPACTIVE_MASK             0x4UL                                    /**< Bit mask for EMU_TEMPACTIVE                 */
#define _EMU_STATUS_TEMPACTIVE_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for EMU_STATUS                 */
#define EMU_STATUS_TEMPACTIVE_DEFAULT           (_EMU_STATUS_TEMPACTIVE_DEFAULT << 2)    /**< Shifted mode DEFAULT for EMU_STATUS         */
#define EMU_STATUS_RACACTIVE                    (0x1UL << 10)                            /**< RAC active                                  */
#define _EMU_STATUS_RACACTIVE_SHIFT             10                                       /**< Shift value for EMU_RACACTIVE               */
#define _EMU_STATUS_RACACTIVE_MASK              0x400UL                                  /**< Bit mask for EMU_RACACTIVE                  */
#define _EMU_STATUS_RACACTIVE_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for EMU_STATUS                 */
#define EMU_STATUS_RACACTIVE_DEFAULT            (_EMU_STATUS_RACACTIVE_DEFAULT << 10)    /**< Shifted mode DEFAULT for EMU_STATUS         */
#define EMU_STATUS_EM4IORET                     (0x1UL << 12)                            /**< EM4 IO retention status                     */
#define _EMU_STATUS_EM4IORET_SHIFT              12                                       /**< Shift value for EMU_EM4IORET                */
#define _EMU_STATUS_EM4IORET_MASK               0x1000UL                                 /**< Bit mask for EMU_EM4IORET                   */
#define _EMU_STATUS_EM4IORET_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for EMU_STATUS                 */
#define EMU_STATUS_EM4IORET_DEFAULT             (_EMU_STATUS_EM4IORET_DEFAULT << 12)     /**< Shifted mode DEFAULT for EMU_STATUS         */
#define EMU_STATUS_EM2ENTERED                   (0x1UL << 14)                            /**< EM2 entered                                 */
#define _EMU_STATUS_EM2ENTERED_SHIFT            14                                       /**< Shift value for EMU_EM2ENTERED              */
#define _EMU_STATUS_EM2ENTERED_MASK             0x4000UL                                 /**< Bit mask for EMU_EM2ENTERED                 */
#define _EMU_STATUS_EM2ENTERED_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for EMU_STATUS                 */
#define EMU_STATUS_EM2ENTERED_DEFAULT           (_EMU_STATUS_EM2ENTERED_DEFAULT << 14)   /**< Shifted mode DEFAULT for EMU_STATUS         */

/* Bit fields for EMU TEMP */
#define _EMU_TEMP_RESETVALUE                    0x00000000UL                            /**< Default value for EMU_TEMP                  */
#define _EMU_TEMP_MASK                          0x000007FFUL                            /**< Mask for EMU_TEMP                           */
#define _EMU_TEMP_TEMPLSB_SHIFT                 0                                       /**< Shift value for EMU_TEMPLSB                 */
#define _EMU_TEMP_TEMPLSB_MASK                  0x3UL                                   /**< Bit mask for EMU_TEMPLSB                    */
#define _EMU_TEMP_TEMPLSB_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_TEMP                   */
#define EMU_TEMP_TEMPLSB_DEFAULT                (_EMU_TEMP_TEMPLSB_DEFAULT << 0)        /**< Shifted mode DEFAULT for EMU_TEMP           */
#define _EMU_TEMP_TEMP_SHIFT                    2                                       /**< Shift value for EMU_TEMP                    */
#define _EMU_TEMP_TEMP_MASK                     0x7FCUL                                 /**< Bit mask for EMU_TEMP                       */
#define _EMU_TEMP_TEMP_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EMU_TEMP                   */
#define EMU_TEMP_TEMP_DEFAULT                   (_EMU_TEMP_TEMP_DEFAULT << 2)           /**< Shifted mode DEFAULT for EMU_TEMP           */

/* Bit fields for EMU RSTCTRL */
#define _EMU_RSTCTRL_RESETVALUE                 0x00004407UL                                /**< Default value for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_MASK                       0x0000C5CFUL                                /**< Mask for EMU_RSTCTRL                        */
#define EMU_RSTCTRL_WDOG0RMODE                  (0x1UL << 0)                                /**< Enable WDOG0 reset                          */
#define _EMU_RSTCTRL_WDOG0RMODE_SHIFT           0                                           /**< Shift value for EMU_WDOG0RMODE              */
#define _EMU_RSTCTRL_WDOG0RMODE_MASK            0x1UL                                       /**< Bit mask for EMU_WDOG0RMODE                 */
#define _EMU_RSTCTRL_WDOG0RMODE_DEFAULT         0x00000001UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_WDOG0RMODE_DISABLED        0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_WDOG0RMODE_ENABLED         0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_WDOG0RMODE_DEFAULT          (_EMU_RSTCTRL_WDOG0RMODE_DEFAULT << 0)      /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_WDOG0RMODE_DISABLED         (_EMU_RSTCTRL_WDOG0RMODE_DISABLED << 0)     /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_WDOG0RMODE_ENABLED          (_EMU_RSTCTRL_WDOG0RMODE_ENABLED << 0)      /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_WDOG1RMODE                  (0x1UL << 1)                                /**< Enable WDOG1 reset                          */
#define _EMU_RSTCTRL_WDOG1RMODE_SHIFT           1                                           /**< Shift value for EMU_WDOG1RMODE              */
#define _EMU_RSTCTRL_WDOG1RMODE_MASK            0x2UL                                       /**< Bit mask for EMU_WDOG1RMODE                 */
#define _EMU_RSTCTRL_WDOG1RMODE_DEFAULT         0x00000001UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_WDOG1RMODE_DISABLED        0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_WDOG1RMODE_ENABLED         0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_WDOG1RMODE_DEFAULT          (_EMU_RSTCTRL_WDOG1RMODE_DEFAULT << 1)      /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_WDOG1RMODE_DISABLED         (_EMU_RSTCTRL_WDOG1RMODE_DISABLED << 1)     /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_WDOG1RMODE_ENABLED          (_EMU_RSTCTRL_WDOG1RMODE_ENABLED << 1)      /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_SYSRMODE                    (0x1UL << 2)                                /**< Enable M33 System reset                     */
#define _EMU_RSTCTRL_SYSRMODE_SHIFT             2                                           /**< Shift value for EMU_SYSRMODE                */
#define _EMU_RSTCTRL_SYSRMODE_MASK              0x4UL                                       /**< Bit mask for EMU_SYSRMODE                   */
#define _EMU_RSTCTRL_SYSRMODE_DEFAULT           0x00000001UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_SYSRMODE_DISABLED          0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_SYSRMODE_ENABLED           0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_SYSRMODE_DEFAULT            (_EMU_RSTCTRL_SYSRMODE_DEFAULT << 2)        /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_SYSRMODE_DISABLED           (_EMU_RSTCTRL_SYSRMODE_DISABLED << 2)       /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_SYSRMODE_ENABLED            (_EMU_RSTCTRL_SYSRMODE_ENABLED << 2)        /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_LOCKUPRMODE                 (0x1UL << 3)                                /**< Enable M33 Lockup reset                     */
#define _EMU_RSTCTRL_LOCKUPRMODE_SHIFT          3                                           /**< Shift value for EMU_LOCKUPRMODE             */
#define _EMU_RSTCTRL_LOCKUPRMODE_MASK           0x8UL                                       /**< Bit mask for EMU_LOCKUPRMODE                */
#define _EMU_RSTCTRL_LOCKUPRMODE_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_LOCKUPRMODE_DISABLED       0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_LOCKUPRMODE_ENABLED        0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_LOCKUPRMODE_DEFAULT         (_EMU_RSTCTRL_LOCKUPRMODE_DEFAULT << 3)     /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_LOCKUPRMODE_DISABLED        (_EMU_RSTCTRL_LOCKUPRMODE_DISABLED << 3)    /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_LOCKUPRMODE_ENABLED         (_EMU_RSTCTRL_LOCKUPRMODE_ENABLED << 3)     /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_AVDDBODRMODE                (0x1UL << 6)                                /**< Enable AVDD BOD reset                       */
#define _EMU_RSTCTRL_AVDDBODRMODE_SHIFT         6                                           /**< Shift value for EMU_AVDDBODRMODE            */
#define _EMU_RSTCTRL_AVDDBODRMODE_MASK          0x40UL                                      /**< Bit mask for EMU_AVDDBODRMODE               */
#define _EMU_RSTCTRL_AVDDBODRMODE_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_AVDDBODRMODE_DISABLED      0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_AVDDBODRMODE_ENABLED       0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_AVDDBODRMODE_DEFAULT        (_EMU_RSTCTRL_AVDDBODRMODE_DEFAULT << 6)    /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_AVDDBODRMODE_DISABLED       (_EMU_RSTCTRL_AVDDBODRMODE_DISABLED << 6)   /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_AVDDBODRMODE_ENABLED        (_EMU_RSTCTRL_AVDDBODRMODE_ENABLED << 6)    /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_IOVDD0BODRMODE              (0x1UL << 7)                                /**< Enable VDDIO0 BOD reset                     */
#define _EMU_RSTCTRL_IOVDD0BODRMODE_SHIFT       7                                           /**< Shift value for EMU_IOVDD0BODRMODE          */
#define _EMU_RSTCTRL_IOVDD0BODRMODE_MASK        0x80UL                                      /**< Bit mask for EMU_IOVDD0BODRMODE             */
#define _EMU_RSTCTRL_IOVDD0BODRMODE_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_IOVDD0BODRMODE_DISABLED    0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_IOVDD0BODRMODE_ENABLED     0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_IOVDD0BODRMODE_DEFAULT      (_EMU_RSTCTRL_IOVDD0BODRMODE_DEFAULT << 7)  /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_IOVDD0BODRMODE_DISABLED     (_EMU_RSTCTRL_IOVDD0BODRMODE_DISABLED << 7) /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_IOVDD0BODRMODE_ENABLED      (_EMU_RSTCTRL_IOVDD0BODRMODE_ENABLED << 7)  /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_DECBODRMODE                 (0x1UL << 10)                               /**< Enable DECBOD reset                         */
#define _EMU_RSTCTRL_DECBODRMODE_SHIFT          10                                          /**< Shift value for EMU_DECBODRMODE             */
#define _EMU_RSTCTRL_DECBODRMODE_MASK           0x400UL                                     /**< Bit mask for EMU_DECBODRMODE                */
#define _EMU_RSTCTRL_DECBODRMODE_DEFAULT        0x00000001UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_DECBODRMODE_DISABLED       0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_DECBODRMODE_ENABLED        0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_DECBODRMODE_DEFAULT         (_EMU_RSTCTRL_DECBODRMODE_DEFAULT << 10)    /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_DECBODRMODE_DISABLED        (_EMU_RSTCTRL_DECBODRMODE_DISABLED << 10)   /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_DECBODRMODE_ENABLED         (_EMU_RSTCTRL_DECBODRMODE_ENABLED << 10)    /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_SESYSRMODE                  (0x1UL << 14)                               /**< Enable SE System reset                      */
#define _EMU_RSTCTRL_SESYSRMODE_SHIFT           14                                          /**< Shift value for EMU_SESYSRMODE              */
#define _EMU_RSTCTRL_SESYSRMODE_MASK            0x4000UL                                    /**< Bit mask for EMU_SESYSRMODE                 */
#define _EMU_RSTCTRL_SESYSRMODE_DEFAULT         0x00000001UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_SESYSRMODE_DISABLED        0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_SESYSRMODE_ENABLED         0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_SESYSRMODE_DEFAULT          (_EMU_RSTCTRL_SESYSRMODE_DEFAULT << 14)     /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_SESYSRMODE_DISABLED         (_EMU_RSTCTRL_SESYSRMODE_DISABLED << 14)    /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_SESYSRMODE_ENABLED          (_EMU_RSTCTRL_SESYSRMODE_ENABLED << 14)     /**< Shifted mode ENABLED for EMU_RSTCTRL        */
#define EMU_RSTCTRL_SELOCKUPRMODE               (0x1UL << 15)                               /**< Enable SE Lockup reset                      */
#define _EMU_RSTCTRL_SELOCKUPRMODE_SHIFT        15                                          /**< Shift value for EMU_SELOCKUPRMODE           */
#define _EMU_RSTCTRL_SELOCKUPRMODE_MASK         0x8000UL                                    /**< Bit mask for EMU_SELOCKUPRMODE              */
#define _EMU_RSTCTRL_SELOCKUPRMODE_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for EMU_RSTCTRL                */
#define _EMU_RSTCTRL_SELOCKUPRMODE_DISABLED     0x00000000UL                                /**< Mode DISABLED for EMU_RSTCTRL               */
#define _EMU_RSTCTRL_SELOCKUPRMODE_ENABLED      0x00000001UL                                /**< Mode ENABLED for EMU_RSTCTRL                */
#define EMU_RSTCTRL_SELOCKUPRMODE_DEFAULT       (_EMU_RSTCTRL_SELOCKUPRMODE_DEFAULT << 15)  /**< Shifted mode DEFAULT for EMU_RSTCTRL        */
#define EMU_RSTCTRL_SELOCKUPRMODE_DISABLED      (_EMU_RSTCTRL_SELOCKUPRMODE_DISABLED << 15) /**< Shifted mode DISABLED for EMU_RSTCTRL       */
#define EMU_RSTCTRL_SELOCKUPRMODE_ENABLED       (_EMU_RSTCTRL_SELOCKUPRMODE_ENABLED << 15)  /**< Shifted mode ENABLED for EMU_RSTCTRL        */

/* Bit fields for EMU RSTCAUSE */
#define _EMU_RSTCAUSE_RESETVALUE                0x00000000UL                            /**< Default value for EMU_RSTCAUSE              */
#define _EMU_RSTCAUSE_MASK                      0x0000FFFFUL                            /**< Mask for EMU_RSTCAUSE                       */
#define EMU_RSTCAUSE_POR                        (0x1UL << 0)                            /**< Power On Reset                              */
#define _EMU_RSTCAUSE_POR_SHIFT                 0                                       /**< Shift value for EMU_POR                     */
#define _EMU_RSTCAUSE_POR_MASK                  0x1UL                                   /**< Bit mask for EMU_POR                        */
#define _EMU_RSTCAUSE_POR_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_POR_DEFAULT                (_EMU_RSTCAUSE_POR_DEFAULT << 0)        /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_PIN                        (0x1UL << 1)                            /**< Pin Reset                                   */
#define _EMU_RSTCAUSE_PIN_SHIFT                 1                                       /**< Shift value for EMU_PIN                     */
#define _EMU_RSTCAUSE_PIN_MASK                  0x2UL                                   /**< Bit mask for EMU_PIN                        */
#define _EMU_RSTCAUSE_PIN_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_PIN_DEFAULT                (_EMU_RSTCAUSE_PIN_DEFAULT << 1)        /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_EM4                        (0x1UL << 2)                            /**< EM4 Wakeup Reset                            */
#define _EMU_RSTCAUSE_EM4_SHIFT                 2                                       /**< Shift value for EMU_EM4                     */
#define _EMU_RSTCAUSE_EM4_MASK                  0x4UL                                   /**< Bit mask for EMU_EM4                        */
#define _EMU_RSTCAUSE_EM4_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_EM4_DEFAULT                (_EMU_RSTCAUSE_EM4_DEFAULT << 2)        /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_WDOG0                      (0x1UL << 3)                            /**< Watchdog 0 Reset                            */
#define _EMU_RSTCAUSE_WDOG0_SHIFT               3                                       /**< Shift value for EMU_WDOG0                   */
#define _EMU_RSTCAUSE_WDOG0_MASK                0x8UL                                   /**< Bit mask for EMU_WDOG0                      */
#define _EMU_RSTCAUSE_WDOG0_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_WDOG0_DEFAULT              (_EMU_RSTCAUSE_WDOG0_DEFAULT << 3)      /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_WDOG1                      (0x1UL << 4)                            /**< Watchdog 1 Reset                            */
#define _EMU_RSTCAUSE_WDOG1_SHIFT               4                                       /**< Shift value for EMU_WDOG1                   */
#define _EMU_RSTCAUSE_WDOG1_MASK                0x10UL                                  /**< Bit mask for EMU_WDOG1                      */
#define _EMU_RSTCAUSE_WDOG1_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_WDOG1_DEFAULT              (_EMU_RSTCAUSE_WDOG1_DEFAULT << 4)      /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_LOCKUP                     (0x1UL << 5)                            /**< M33 Core Lockup Reset                       */
#define _EMU_RSTCAUSE_LOCKUP_SHIFT              5                                       /**< Shift value for EMU_LOCKUP                  */
#define _EMU_RSTCAUSE_LOCKUP_MASK               0x20UL                                  /**< Bit mask for EMU_LOCKUP                     */
#define _EMU_RSTCAUSE_LOCKUP_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_LOCKUP_DEFAULT             (_EMU_RSTCAUSE_LOCKUP_DEFAULT << 5)     /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_SYSREQ                     (0x1UL << 6)                            /**< M33 Core Sys Reset                          */
#define _EMU_RSTCAUSE_SYSREQ_SHIFT              6                                       /**< Shift value for EMU_SYSREQ                  */
#define _EMU_RSTCAUSE_SYSREQ_MASK               0x40UL                                  /**< Bit mask for EMU_SYSREQ                     */
#define _EMU_RSTCAUSE_SYSREQ_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_SYSREQ_DEFAULT             (_EMU_RSTCAUSE_SYSREQ_DEFAULT << 6)     /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_DVDDBOD                    (0x1UL << 7)                            /**< HVBOD Reset                                 */
#define _EMU_RSTCAUSE_DVDDBOD_SHIFT             7                                       /**< Shift value for EMU_DVDDBOD                 */
#define _EMU_RSTCAUSE_DVDDBOD_MASK              0x80UL                                  /**< Bit mask for EMU_DVDDBOD                    */
#define _EMU_RSTCAUSE_DVDDBOD_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_DVDDBOD_DEFAULT            (_EMU_RSTCAUSE_DVDDBOD_DEFAULT << 7)    /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_DVDDLEBOD                  (0x1UL << 8)                            /**< LEBOD Reset                                 */
#define _EMU_RSTCAUSE_DVDDLEBOD_SHIFT           8                                       /**< Shift value for EMU_DVDDLEBOD               */
#define _EMU_RSTCAUSE_DVDDLEBOD_MASK            0x100UL                                 /**< Bit mask for EMU_DVDDLEBOD                  */
#define _EMU_RSTCAUSE_DVDDLEBOD_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_DVDDLEBOD_DEFAULT          (_EMU_RSTCAUSE_DVDDLEBOD_DEFAULT << 8)  /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_DECBOD                     (0x1UL << 9)                            /**< LVBOD Reset                                 */
#define _EMU_RSTCAUSE_DECBOD_SHIFT              9                                       /**< Shift value for EMU_DECBOD                  */
#define _EMU_RSTCAUSE_DECBOD_MASK               0x200UL                                 /**< Bit mask for EMU_DECBOD                     */
#define _EMU_RSTCAUSE_DECBOD_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_DECBOD_DEFAULT             (_EMU_RSTCAUSE_DECBOD_DEFAULT << 9)     /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_AVDDBOD                    (0x1UL << 10)                           /**< LEBOD1 Reset                                */
#define _EMU_RSTCAUSE_AVDDBOD_SHIFT             10                                      /**< Shift value for EMU_AVDDBOD                 */
#define _EMU_RSTCAUSE_AVDDBOD_MASK              0x400UL                                 /**< Bit mask for EMU_AVDDBOD                    */
#define _EMU_RSTCAUSE_AVDDBOD_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_AVDDBOD_DEFAULT            (_EMU_RSTCAUSE_AVDDBOD_DEFAULT << 10)   /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_IOVDD0BOD                  (0x1UL << 11)                           /**< LEBOD2 Reset                                */
#define _EMU_RSTCAUSE_IOVDD0BOD_SHIFT           11                                      /**< Shift value for EMU_IOVDD0BOD               */
#define _EMU_RSTCAUSE_IOVDD0BOD_MASK            0x800UL                                 /**< Bit mask for EMU_IOVDD0BOD                  */
#define _EMU_RSTCAUSE_IOVDD0BOD_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_IOVDD0BOD_DEFAULT          (_EMU_RSTCAUSE_IOVDD0BOD_DEFAULT << 11) /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_SETAMPER                   (0x1UL << 13)                           /**< SE Tamper event Reset                       */
#define _EMU_RSTCAUSE_SETAMPER_SHIFT            13                                      /**< Shift value for EMU_SETAMPER                */
#define _EMU_RSTCAUSE_SETAMPER_MASK             0x2000UL                                /**< Bit mask for EMU_SETAMPER                   */
#define _EMU_RSTCAUSE_SETAMPER_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_SETAMPER_DEFAULT           (_EMU_RSTCAUSE_SETAMPER_DEFAULT << 13)  /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_SESYSREQ                   (0x1UL << 14)                           /**< SE System Reset                             */
#define _EMU_RSTCAUSE_SESYSREQ_SHIFT            14                                      /**< Shift value for EMU_SESYSREQ                */
#define _EMU_RSTCAUSE_SESYSREQ_MASK             0x4000UL                                /**< Bit mask for EMU_SESYSREQ                   */
#define _EMU_RSTCAUSE_SESYSREQ_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_SESYSREQ_DEFAULT           (_EMU_RSTCAUSE_SESYSREQ_DEFAULT << 14)  /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */
#define EMU_RSTCAUSE_SELOCKUP                   (0x1UL << 15)                           /**< SE Lockup Reset                             */
#define _EMU_RSTCAUSE_SELOCKUP_SHIFT            15                                      /**< Shift value for EMU_SELOCKUP                */
#define _EMU_RSTCAUSE_SELOCKUP_MASK             0x8000UL                                /**< Bit mask for EMU_SELOCKUP                   */
#define _EMU_RSTCAUSE_SELOCKUP_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for EMU_RSTCAUSE               */
#define EMU_RSTCAUSE_SELOCKUP_DEFAULT           (_EMU_RSTCAUSE_SELOCKUP_DEFAULT << 15)  /**< Shifted mode DEFAULT for EMU_RSTCAUSE       */

/* Bit fields for EMU DGIF */
#define _EMU_DGIF_RESETVALUE                    0x00000000UL                            /**< Default value for EMU_DGIF                  */
#define _EMU_DGIF_MASK                          0xE1000000UL                            /**< Mask for EMU_DGIF                           */
#define EMU_DGIF_EM23WAKEUP                     (0x1UL << 24)                           /**< EM23 Wake up Interrupt flag                 */
#define _EMU_DGIF_EM23WAKEUP_SHIFT              24                                      /**< Shift value for EMU_EM23WAKEUP              */
#define _EMU_DGIF_EM23WAKEUP_MASK               0x1000000UL                             /**< Bit mask for EMU_EM23WAKEUP                 */
#define _EMU_DGIF_EM23WAKEUP_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for EMU_DGIF                   */
#define EMU_DGIF_EM23WAKEUP_DEFAULT             (_EMU_DGIF_EM23WAKEUP_DEFAULT << 24)    /**< Shifted mode DEFAULT for EMU_DGIF           */
#define EMU_DGIF_TEMP                           (0x1UL << 29)                           /**< Temperature Interrupt flag                  */
#define _EMU_DGIF_TEMP_SHIFT                    29                                      /**< Shift value for EMU_TEMP                    */
#define _EMU_DGIF_TEMP_MASK                     0x20000000UL                            /**< Bit mask for EMU_TEMP                       */
#define _EMU_DGIF_TEMP_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EMU_DGIF                   */
#define EMU_DGIF_TEMP_DEFAULT                   (_EMU_DGIF_TEMP_DEFAULT << 29)          /**< Shifted mode DEFAULT for EMU_DGIF           */
#define EMU_DGIF_TEMPLOW                        (0x1UL << 30)                           /**< Temperature low Interrupt flag              */
#define _EMU_DGIF_TEMPLOW_SHIFT                 30                                      /**< Shift value for EMU_TEMPLOW                 */
#define _EMU_DGIF_TEMPLOW_MASK                  0x40000000UL                            /**< Bit mask for EMU_TEMPLOW                    */
#define _EMU_DGIF_TEMPLOW_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_DGIF                   */
#define EMU_DGIF_TEMPLOW_DEFAULT                (_EMU_DGIF_TEMPLOW_DEFAULT << 30)       /**< Shifted mode DEFAULT for EMU_DGIF           */
#define EMU_DGIF_TEMPHIGH                       (0x1UL << 31)                           /**< Temperature high Interrupt flag             */
#define _EMU_DGIF_TEMPHIGH_SHIFT                31                                      /**< Shift value for EMU_TEMPHIGH                */
#define _EMU_DGIF_TEMPHIGH_MASK                 0x80000000UL                            /**< Bit mask for EMU_TEMPHIGH                   */
#define _EMU_DGIF_TEMPHIGH_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EMU_DGIF                   */
#define EMU_DGIF_TEMPHIGH_DEFAULT               (_EMU_DGIF_TEMPHIGH_DEFAULT << 31)      /**< Shifted mode DEFAULT for EMU_DGIF           */

/* Bit fields for EMU DGIEN */
#define _EMU_DGIEN_RESETVALUE                   0x00000000UL                            /**< Default value for EMU_DGIEN                 */
#define _EMU_DGIEN_MASK                         0xE1000000UL                            /**< Mask for EMU_DGIEN                          */
#define EMU_DGIEN_EM23WAKEUP                    (0x1UL << 24)                           /**< EM23 Wake up Interrupt enable               */
#define _EMU_DGIEN_EM23WAKEUP_SHIFT             24                                      /**< Shift value for EMU_EM23WAKEUP              */
#define _EMU_DGIEN_EM23WAKEUP_MASK              0x1000000UL                             /**< Bit mask for EMU_EM23WAKEUP                 */
#define _EMU_DGIEN_EM23WAKEUP_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for EMU_DGIEN                  */
#define EMU_DGIEN_EM23WAKEUP_DEFAULT            (_EMU_DGIEN_EM23WAKEUP_DEFAULT << 24)   /**< Shifted mode DEFAULT for EMU_DGIEN          */
#define EMU_DGIEN_TEMP                          (0x1UL << 29)                           /**< Temperature Interrupt enable                */
#define _EMU_DGIEN_TEMP_SHIFT                   29                                      /**< Shift value for EMU_TEMP                    */
#define _EMU_DGIEN_TEMP_MASK                    0x20000000UL                            /**< Bit mask for EMU_TEMP                       */
#define _EMU_DGIEN_TEMP_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EMU_DGIEN                  */
#define EMU_DGIEN_TEMP_DEFAULT                  (_EMU_DGIEN_TEMP_DEFAULT << 29)         /**< Shifted mode DEFAULT for EMU_DGIEN          */
#define EMU_DGIEN_TEMPLOW                       (0x1UL << 30)                           /**< Temperature low Interrupt enable            */
#define _EMU_DGIEN_TEMPLOW_SHIFT                30                                      /**< Shift value for EMU_TEMPLOW                 */
#define _EMU_DGIEN_TEMPLOW_MASK                 0x40000000UL                            /**< Bit mask for EMU_TEMPLOW                    */
#define _EMU_DGIEN_TEMPLOW_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EMU_DGIEN                  */
#define EMU_DGIEN_TEMPLOW_DEFAULT               (_EMU_DGIEN_TEMPLOW_DEFAULT << 30)      /**< Shifted mode DEFAULT for EMU_DGIEN          */
#define EMU_DGIEN_TEMPHIGH                      (0x1UL << 31)                           /**< Temperature high Interrupt enable           */
#define _EMU_DGIEN_TEMPHIGH_SHIFT               31                                      /**< Shift value for EMU_TEMPHIGH                */
#define _EMU_DGIEN_TEMPHIGH_MASK                0x80000000UL                            /**< Bit mask for EMU_TEMPHIGH                   */
#define _EMU_DGIEN_TEMPHIGH_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EMU_DGIEN                  */
#define EMU_DGIEN_TEMPHIGH_DEFAULT              (_EMU_DGIEN_TEMPHIGH_DEFAULT << 31)     /**< Shifted mode DEFAULT for EMU_DGIEN          */

/* Bit fields for EMU SEIF */
#define _EMU_SEIF_RESETVALUE                    0x00000000UL                            /**< Default value for EMU_SEIF                  */
#define _EMU_SEIF_MASK                          0xE0000000UL                            /**< Mask for EMU_SEIF                           */
#define EMU_SEIF_TEMP                           (0x1UL << 29)                           /**< Temperature Interrupt flag                  */
#define _EMU_SEIF_TEMP_SHIFT                    29                                      /**< Shift value for EMU_TEMP                    */
#define _EMU_SEIF_TEMP_MASK                     0x20000000UL                            /**< Bit mask for EMU_TEMP                       */
#define _EMU_SEIF_TEMP_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for EMU_SEIF                   */
#define EMU_SEIF_TEMP_DEFAULT                   (_EMU_SEIF_TEMP_DEFAULT << 29)          /**< Shifted mode DEFAULT for EMU_SEIF           */
#define EMU_SEIF_TEMPLOW                        (0x1UL << 30)                           /**< Temperature Interrupt flag                  */
#define _EMU_SEIF_TEMPLOW_SHIFT                 30                                      /**< Shift value for EMU_TEMPLOW                 */
#define _EMU_SEIF_TEMPLOW_MASK                  0x40000000UL                            /**< Bit mask for EMU_TEMPLOW                    */
#define _EMU_SEIF_TEMPLOW_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for EMU_SEIF                   */
#define EMU_SEIF_TEMPLOW_DEFAULT                (_EMU_SEIF_TEMPLOW_DEFAULT << 30)       /**< Shifted mode DEFAULT for EMU_SEIF           */
#define EMU_SEIF_TEMPHIGH                       (0x1UL << 31)                           /**< Temperature low Interrupt flag              */
#define _EMU_SEIF_TEMPHIGH_SHIFT                31                                      /**< Shift value for EMU_TEMPHIGH                */
#define _EMU_SEIF_TEMPHIGH_MASK                 0x80000000UL                            /**< Bit mask for EMU_TEMPHIGH                   */
#define _EMU_SEIF_TEMPHIGH_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EMU_SEIF                   */
#define EMU_SEIF_TEMPHIGH_DEFAULT               (_EMU_SEIF_TEMPHIGH_DEFAULT << 31)      /**< Shifted mode DEFAULT for EMU_SEIF           */

/* Bit fields for EMU SEIEN */
#define _EMU_SEIEN_RESETVALUE                   0x00000000UL                            /**< Default value for EMU_SEIEN                 */
#define _EMU_SEIEN_MASK                         0xE0000000UL                            /**< Mask for EMU_SEIEN                          */
#define EMU_SEIEN_TEMP                          (0x1UL << 29)                           /**< Temperature Interrupt enable                */
#define _EMU_SEIEN_TEMP_SHIFT                   29                                      /**< Shift value for EMU_TEMP                    */
#define _EMU_SEIEN_TEMP_MASK                    0x20000000UL                            /**< Bit mask for EMU_TEMP                       */
#define _EMU_SEIEN_TEMP_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for EMU_SEIEN                  */
#define EMU_SEIEN_TEMP_DEFAULT                  (_EMU_SEIEN_TEMP_DEFAULT << 29)         /**< Shifted mode DEFAULT for EMU_SEIEN          */
#define EMU_SEIEN_TEMPLOW                       (0x1UL << 30)                           /**< Temperature low Interrupt enable            */
#define _EMU_SEIEN_TEMPLOW_SHIFT                30                                      /**< Shift value for EMU_TEMPLOW                 */
#define _EMU_SEIEN_TEMPLOW_MASK                 0x40000000UL                            /**< Bit mask for EMU_TEMPLOW                    */
#define _EMU_SEIEN_TEMPLOW_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for EMU_SEIEN                  */
#define EMU_SEIEN_TEMPLOW_DEFAULT               (_EMU_SEIEN_TEMPLOW_DEFAULT << 30)      /**< Shifted mode DEFAULT for EMU_SEIEN          */
#define EMU_SEIEN_TEMPHIGH                      (0x1UL << 31)                           /**< Temperature high Interrupt enable           */
#define _EMU_SEIEN_TEMPHIGH_SHIFT               31                                      /**< Shift value for EMU_TEMPHIGH                */
#define _EMU_SEIEN_TEMPHIGH_MASK                0x80000000UL                            /**< Bit mask for EMU_TEMPHIGH                   */
#define _EMU_SEIEN_TEMPHIGH_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for EMU_SEIEN                  */
#define EMU_SEIEN_TEMPHIGH_DEFAULT              (_EMU_SEIEN_TEMPHIGH_DEFAULT << 31)     /**< Shifted mode DEFAULT for EMU_SEIEN          */

/** @} End of group EFR32MG21_EMU_BitFields */
/** @} End of group EFR32MG21_EMU */
/** @} End of group Parts */
#endif /* EFR32MG21_EMU_H */
