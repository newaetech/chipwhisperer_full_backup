/**************************************************************************//**
 * @file
 * @brief EFR32MG21 CMU register and bit field definitions
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
#ifndef EFR32MG21_CMU_H
#define EFR32MG21_CMU_H
#define CMU_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_CMU CMU
 * @{
 * @brief EFR32MG21 CMU Register Declaration.
 *****************************************************************************/

/** CMU Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK;                      /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED2[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED3[10U];                /**< Reserved for future use                            */
  __IOM uint32_t CALCMD;                        /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL;                       /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT;                        /**< Calibration Result Counter Register                */
  uint32_t       RESERVED4[5U];                 /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL;                    /**< System Clock Control                               */
  uint32_t       RESERVED5[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL;                  /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED6[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL;                 /**< Export Clock Control                               */
  uint32_t       RESERVED7[27U];                /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL;                /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED8[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL;               /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED9[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL;               /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED10[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL;                /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED11[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL;                   /**< IADC Clock Control                                 */
  uint32_t       RESERVED12[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL;                  /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED13[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL;                  /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED14[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RTCCCLKCTRL;                   /**< RTCC Clock Control                                 */
  uint32_t       RESERVED15[1U];                /**< Reserved for future use                            */
  __IOM uint32_t PRORTCCLKCTRL;                 /**< Protocol RTC Clock Control                         */
  uint32_t       RESERVED16[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL;                  /**< Radio Clock Control                                */
  uint32_t       RESERVED17[863U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  uint32_t       RESERVED18[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  uint32_t       RESERVED19[1U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK_SET;                  /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED20[2U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED21[10U];               /**< Reserved for future use                            */
  __IOM uint32_t CALCMD_SET;                    /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL_SET;                   /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT_SET;                    /**< Calibration Result Counter Register                */
  uint32_t       RESERVED22[5U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL_SET;                /**< System Clock Control                               */
  uint32_t       RESERVED23[3U];                /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL_SET;              /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED24[3U];                /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL_SET;             /**< Export Clock Control                               */
  uint32_t       RESERVED25[27U];               /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL_SET;            /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED26[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL_SET;           /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED27[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL_SET;           /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED28[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL_SET;            /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED29[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL_SET;               /**< IADC Clock Control                                 */
  uint32_t       RESERVED30[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL_SET;              /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED31[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL_SET;              /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED32[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RTCCCLKCTRL_SET;               /**< RTCC Clock Control                                 */
  uint32_t       RESERVED33[1U];                /**< Reserved for future use                            */
  __IOM uint32_t PRORTCCLKCTRL_SET;             /**< Protocol RTC Clock Control                         */
  uint32_t       RESERVED34[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL_SET;              /**< Radio Clock Control                                */
  uint32_t       RESERVED35[863U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  uint32_t       RESERVED36[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  uint32_t       RESERVED37[1U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK_CLR;                  /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED38[2U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED39[10U];               /**< Reserved for future use                            */
  __IOM uint32_t CALCMD_CLR;                    /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL_CLR;                   /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT_CLR;                    /**< Calibration Result Counter Register                */
  uint32_t       RESERVED40[5U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL_CLR;                /**< System Clock Control                               */
  uint32_t       RESERVED41[3U];                /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL_CLR;              /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED42[3U];                /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL_CLR;             /**< Export Clock Control                               */
  uint32_t       RESERVED43[27U];               /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL_CLR;            /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED44[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL_CLR;           /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED45[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL_CLR;           /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED46[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL_CLR;            /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED47[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL_CLR;               /**< IADC Clock Control                                 */
  uint32_t       RESERVED48[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL_CLR;              /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED49[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL_CLR;              /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED50[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RTCCCLKCTRL_CLR;               /**< RTCC Clock Control                                 */
  uint32_t       RESERVED51[1U];                /**< Reserved for future use                            */
  __IOM uint32_t PRORTCCLKCTRL_CLR;             /**< Protocol RTC Clock Control                         */
  uint32_t       RESERVED52[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL_CLR;              /**< Radio Clock Control                                */
  uint32_t       RESERVED53[863U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  uint32_t       RESERVED54[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  uint32_t       RESERVED55[1U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
  __IOM uint32_t WDOGLOCK_TGL;                  /**< WDOG Configuration Lock Register                   */
  uint32_t       RESERVED56[2U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED57[10U];               /**< Reserved for future use                            */
  __IOM uint32_t CALCMD_TGL;                    /**< Calibration Command Register                       */
  __IOM uint32_t CALCTRL_TGL;                   /**< Calibration Control Register                       */
  __IM uint32_t  CALCNT_TGL;                    /**< Calibration Result Counter Register                */
  uint32_t       RESERVED58[5U];                /**< Reserved for future use                            */
  __IOM uint32_t SYSCLKCTRL_TGL;                /**< System Clock Control                               */
  uint32_t       RESERVED59[3U];                /**< Reserved for future use                            */
  __IOM uint32_t TRACECLKCTRL_TGL;              /**< Debug Trace Clock Control                          */
  uint32_t       RESERVED60[3U];                /**< Reserved for future use                            */
  __IOM uint32_t EXPORTCLKCTRL_TGL;             /**< Export Clock Control                               */
  uint32_t       RESERVED61[27U];               /**< Reserved for future use                            */
  __IOM uint32_t DPLLREFCLKCTRL_TGL;            /**< Digital PLL Reference Clock Control                */
  uint32_t       RESERVED62[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01GRPACLKCTRL_TGL;           /**< EM01 Peripheral Group A Clock Control              */
  uint32_t       RESERVED63[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM23GRPACLKCTRL_TGL;           /**< EM23 Peripheral Group A Clock Control              */
  uint32_t       RESERVED64[7U];                /**< Reserved for future use                            */
  __IOM uint32_t EM4GRPACLKCTRL_TGL;            /**< EM4 Peripheral Group A Clock Control               */
  uint32_t       RESERVED65[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IADCCLKCTRL_TGL;               /**< IADC Clock Control                                 */
  uint32_t       RESERVED66[31U];               /**< Reserved for future use                            */
  __IOM uint32_t WDOG0CLKCTRL_TGL;              /**< Watchdog0 Clock Control                            */
  uint32_t       RESERVED67[1U];                /**< Reserved for future use                            */
  __IOM uint32_t WDOG1CLKCTRL_TGL;              /**< Watchdog1 Clock Control                            */
  uint32_t       RESERVED68[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RTCCCLKCTRL_TGL;               /**< RTCC Clock Control                                 */
  uint32_t       RESERVED69[1U];                /**< Reserved for future use                            */
  __IOM uint32_t PRORTCCLKCTRL_TGL;             /**< Protocol RTC Clock Control                         */
  uint32_t       RESERVED70[13U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIOCLKCTRL_TGL;              /**< Radio Clock Control                                */
} CMU_TypeDef;
/** @} End of group EFR32MG21_CMU */

/**************************************************************************//**
 * @addtogroup EFR32MG21_CMU
 * @{
 * @defgroup EFR32MG21_CMU_BitFields CMU Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for CMU IPVERSION */
#define _CMU_IPVERSION_RESETVALUE                  0x00000000UL                            /**< Default value for CMU_IPVERSION             */
#define _CMU_IPVERSION_MASK                        0xFFFFFFFFUL                            /**< Mask for CMU_IPVERSION                      */
#define _CMU_IPVERSION_IPVERSION_SHIFT             0                                       /**< Shift value for CMU_IPVERSION               */
#define _CMU_IPVERSION_IPVERSION_MASK              0xFFFFFFFFUL                            /**< Bit mask for CMU_IPVERSION                  */
#define _CMU_IPVERSION_IPVERSION_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for CMU_IPVERSION              */
#define CMU_IPVERSION_IPVERSION_DEFAULT            (_CMU_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_IPVERSION      */

/* Bit fields for CMU STATUS */
#define _CMU_STATUS_RESETVALUE                     0x00000000UL                          /**< Default value for CMU_STATUS                */
#define _CMU_STATUS_MASK                           0xC0030101UL                          /**< Mask for CMU_STATUS                         */
#define CMU_STATUS_CALRDY                          (0x1UL << 0)                          /**< Calibration Ready                           */
#define _CMU_STATUS_CALRDY_SHIFT                   0                                     /**< Shift value for CMU_CALRDY                  */
#define _CMU_STATUS_CALRDY_MASK                    0x1UL                                 /**< Bit mask for CMU_CALRDY                     */
#define _CMU_STATUS_CALRDY_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for CMU_STATUS                 */
#define CMU_STATUS_CALRDY_DEFAULT                  (_CMU_STATUS_CALRDY_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_STATUS         */
#define CMU_STATUS_WDOGLOCK                        (0x1UL << 30)                         /**< Configuration Lock Status for WDOG          */
#define _CMU_STATUS_WDOGLOCK_SHIFT                 30                                    /**< Shift value for CMU_WDOGLOCK                */
#define _CMU_STATUS_WDOGLOCK_MASK                  0x40000000UL                          /**< Bit mask for CMU_WDOGLOCK                   */
#define _CMU_STATUS_WDOGLOCK_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for CMU_STATUS                 */
#define _CMU_STATUS_WDOGLOCK_UNLOCKED              0x00000000UL                          /**< Mode UNLOCKED for CMU_STATUS                */
#define _CMU_STATUS_WDOGLOCK_LOCKED                0x00000001UL                          /**< Mode LOCKED for CMU_STATUS                  */
#define CMU_STATUS_WDOGLOCK_DEFAULT                (_CMU_STATUS_WDOGLOCK_DEFAULT << 30)  /**< Shifted mode DEFAULT for CMU_STATUS         */
#define CMU_STATUS_WDOGLOCK_UNLOCKED               (_CMU_STATUS_WDOGLOCK_UNLOCKED << 30) /**< Shifted mode UNLOCKED for CMU_STATUS        */
#define CMU_STATUS_WDOGLOCK_LOCKED                 (_CMU_STATUS_WDOGLOCK_LOCKED << 30)   /**< Shifted mode LOCKED for CMU_STATUS          */
#define CMU_STATUS_LOCK                            (0x1UL << 31)                         /**< Configuration Lock Status                   */
#define _CMU_STATUS_LOCK_SHIFT                     31                                    /**< Shift value for CMU_LOCK                    */
#define _CMU_STATUS_LOCK_MASK                      0x80000000UL                          /**< Bit mask for CMU_LOCK                       */
#define _CMU_STATUS_LOCK_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for CMU_STATUS                 */
#define _CMU_STATUS_LOCK_UNLOCKED                  0x00000000UL                          /**< Mode UNLOCKED for CMU_STATUS                */
#define _CMU_STATUS_LOCK_LOCKED                    0x00000001UL                          /**< Mode LOCKED for CMU_STATUS                  */
#define CMU_STATUS_LOCK_DEFAULT                    (_CMU_STATUS_LOCK_DEFAULT << 31)      /**< Shifted mode DEFAULT for CMU_STATUS         */
#define CMU_STATUS_LOCK_UNLOCKED                   (_CMU_STATUS_LOCK_UNLOCKED << 31)     /**< Shifted mode UNLOCKED for CMU_STATUS        */
#define CMU_STATUS_LOCK_LOCKED                     (_CMU_STATUS_LOCK_LOCKED << 31)       /**< Shifted mode LOCKED for CMU_STATUS          */

/* Bit fields for CMU LOCK */
#define _CMU_LOCK_RESETVALUE                       0x000093F7UL                         /**< Default value for CMU_LOCK                  */
#define _CMU_LOCK_MASK                             0x0000FFFFUL                         /**< Mask for CMU_LOCK                           */
#define _CMU_LOCK_LOCKKEY_SHIFT                    0                                    /**< Shift value for CMU_LOCKKEY                 */
#define _CMU_LOCK_LOCKKEY_MASK                     0xFFFFUL                             /**< Bit mask for CMU_LOCKKEY                    */
#define _CMU_LOCK_LOCKKEY_DEFAULT                  0x000093F7UL                         /**< Mode DEFAULT for CMU_LOCK                   */
#define _CMU_LOCK_LOCKKEY_UNLOCK                   0x000093F7UL                         /**< Mode UNLOCK for CMU_LOCK                    */
#define CMU_LOCK_LOCKKEY_DEFAULT                   (_CMU_LOCK_LOCKKEY_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_LOCK           */
#define CMU_LOCK_LOCKKEY_UNLOCK                    (_CMU_LOCK_LOCKKEY_UNLOCK << 0)      /**< Shifted mode UNLOCK for CMU_LOCK            */

/* Bit fields for CMU WDOGLOCK */
#define _CMU_WDOGLOCK_RESETVALUE                   0x00005257UL                         /**< Default value for CMU_WDOGLOCK              */
#define _CMU_WDOGLOCK_MASK                         0x0000FFFFUL                         /**< Mask for CMU_WDOGLOCK                       */
#define _CMU_WDOGLOCK_LOCKKEY_SHIFT                0                                    /**< Shift value for CMU_LOCKKEY                 */
#define _CMU_WDOGLOCK_LOCKKEY_MASK                 0xFFFFUL                             /**< Bit mask for CMU_LOCKKEY                    */
#define _CMU_WDOGLOCK_LOCKKEY_DEFAULT              0x00005257UL                         /**< Mode DEFAULT for CMU_WDOGLOCK               */
#define _CMU_WDOGLOCK_LOCKKEY_UNLOCK               0x00005257UL                         /**< Mode UNLOCK for CMU_WDOGLOCK                */
#define CMU_WDOGLOCK_LOCKKEY_DEFAULT               (_CMU_WDOGLOCK_LOCKKEY_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_WDOGLOCK       */
#define CMU_WDOGLOCK_LOCKKEY_UNLOCK                (_CMU_WDOGLOCK_LOCKKEY_UNLOCK << 0)  /**< Shifted mode UNLOCK for CMU_WDOGLOCK        */

/* Bit fields for CMU IF */
#define _CMU_IF_RESETVALUE                         0x00000000UL                         /**< Default value for CMU_IF                    */
#define _CMU_IF_MASK                               0x00000003UL                         /**< Mask for CMU_IF                             */
#define CMU_IF_CALRDY                              (0x1UL << 0)                         /**< Calibration Ready Interrupt Flag            */
#define _CMU_IF_CALRDY_SHIFT                       0                                    /**< Shift value for CMU_CALRDY                  */
#define _CMU_IF_CALRDY_MASK                        0x1UL                                /**< Bit mask for CMU_CALRDY                     */
#define _CMU_IF_CALRDY_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for CMU_IF                     */
#define CMU_IF_CALRDY_DEFAULT                      (_CMU_IF_CALRDY_DEFAULT << 0)        /**< Shifted mode DEFAULT for CMU_IF             */
#define CMU_IF_CALOF                               (0x1UL << 1)                         /**< Calibration Overflow Interrupt Flag         */
#define _CMU_IF_CALOF_SHIFT                        1                                    /**< Shift value for CMU_CALOF                   */
#define _CMU_IF_CALOF_MASK                         0x2UL                                /**< Bit mask for CMU_CALOF                      */
#define _CMU_IF_CALOF_DEFAULT                      0x00000000UL                         /**< Mode DEFAULT for CMU_IF                     */
#define CMU_IF_CALOF_DEFAULT                       (_CMU_IF_CALOF_DEFAULT << 1)         /**< Shifted mode DEFAULT for CMU_IF             */

/* Bit fields for CMU IEN */
#define _CMU_IEN_RESETVALUE                        0x00000000UL                         /**< Default value for CMU_IEN                   */
#define _CMU_IEN_MASK                              0x00000003UL                         /**< Mask for CMU_IEN                            */
#define CMU_IEN_CALRDY                             (0x1UL << 0)                         /**< Calibration Ready Interrupt Enable          */
#define _CMU_IEN_CALRDY_SHIFT                      0                                    /**< Shift value for CMU_CALRDY                  */
#define _CMU_IEN_CALRDY_MASK                       0x1UL                                /**< Bit mask for CMU_CALRDY                     */
#define _CMU_IEN_CALRDY_DEFAULT                    0x00000000UL                         /**< Mode DEFAULT for CMU_IEN                    */
#define CMU_IEN_CALRDY_DEFAULT                     (_CMU_IEN_CALRDY_DEFAULT << 0)       /**< Shifted mode DEFAULT for CMU_IEN            */
#define CMU_IEN_CALOF                              (0x1UL << 1)                         /**< Calibration Overflow Interrupt Enable       */
#define _CMU_IEN_CALOF_SHIFT                       1                                    /**< Shift value for CMU_CALOF                   */
#define _CMU_IEN_CALOF_MASK                        0x2UL                                /**< Bit mask for CMU_CALOF                      */
#define _CMU_IEN_CALOF_DEFAULT                     0x00000000UL                         /**< Mode DEFAULT for CMU_IEN                    */
#define CMU_IEN_CALOF_DEFAULT                      (_CMU_IEN_CALOF_DEFAULT << 1)        /**< Shifted mode DEFAULT for CMU_IEN            */

/* Bit fields for CMU CALCMD */
#define _CMU_CALCMD_RESETVALUE                     0x00000000UL                         /**< Default value for CMU_CALCMD                */
#define _CMU_CALCMD_MASK                           0x00000003UL                         /**< Mask for CMU_CALCMD                         */
#define CMU_CALCMD_CALSTART                        (0x1UL << 0)                         /**< Calibration Start                           */
#define _CMU_CALCMD_CALSTART_SHIFT                 0                                    /**< Shift value for CMU_CALSTART                */
#define _CMU_CALCMD_CALSTART_MASK                  0x1UL                                /**< Bit mask for CMU_CALSTART                   */
#define _CMU_CALCMD_CALSTART_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for CMU_CALCMD                 */
#define CMU_CALCMD_CALSTART_DEFAULT                (_CMU_CALCMD_CALSTART_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_CALCMD         */
#define CMU_CALCMD_CALSTOP                         (0x1UL << 1)                         /**< Calibration Stop                            */
#define _CMU_CALCMD_CALSTOP_SHIFT                  1                                    /**< Shift value for CMU_CALSTOP                 */
#define _CMU_CALCMD_CALSTOP_MASK                   0x2UL                                /**< Bit mask for CMU_CALSTOP                    */
#define _CMU_CALCMD_CALSTOP_DEFAULT                0x00000000UL                         /**< Mode DEFAULT for CMU_CALCMD                 */
#define CMU_CALCMD_CALSTOP_DEFAULT                 (_CMU_CALCMD_CALSTOP_DEFAULT << 1)   /**< Shifted mode DEFAULT for CMU_CALCMD         */

/* Bit fields for CMU CALCTRL */
#define _CMU_CALCTRL_RESETVALUE                    0x00000000UL                           /**< Default value for CMU_CALCTRL               */
#define _CMU_CALCTRL_MASK                          0xFF8FFFFFUL                           /**< Mask for CMU_CALCTRL                        */
#define _CMU_CALCTRL_CALTOP_SHIFT                  0                                      /**< Shift value for CMU_CALTOP                  */
#define _CMU_CALCTRL_CALTOP_MASK                   0xFFFFFUL                              /**< Bit mask for CMU_CALTOP                     */
#define _CMU_CALCTRL_CALTOP_DEFAULT                0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define CMU_CALCTRL_CALTOP_DEFAULT                 (_CMU_CALCTRL_CALTOP_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define CMU_CALCTRL_CONT                           (0x1UL << 23)                          /**< Continuous Calibration                      */
#define _CMU_CALCTRL_CONT_SHIFT                    23                                     /**< Shift value for CMU_CONT                    */
#define _CMU_CALCTRL_CONT_MASK                     0x800000UL                             /**< Bit mask for CMU_CONT                       */
#define _CMU_CALCTRL_CONT_DEFAULT                  0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define CMU_CALCTRL_CONT_DEFAULT                   (_CMU_CALCTRL_CONT_DEFAULT << 23)      /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define _CMU_CALCTRL_UPSEL_SHIFT                   24                                     /**< Shift value for CMU_UPSEL                   */
#define _CMU_CALCTRL_UPSEL_MASK                    0xF000000UL                            /**< Bit mask for CMU_UPSEL                      */
#define _CMU_CALCTRL_UPSEL_DEFAULT                 0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define _CMU_CALCTRL_UPSEL_DISABLED                0x00000000UL                           /**< Mode DISABLED for CMU_CALCTRL               */
#define _CMU_CALCTRL_UPSEL_PRS                     0x00000001UL                           /**< Mode PRS for CMU_CALCTRL                    */
#define _CMU_CALCTRL_UPSEL_HFXO                    0x00000002UL                           /**< Mode HFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_UPSEL_LFXO                    0x00000003UL                           /**< Mode LFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_UPSEL_HFRCODPLL               0x00000004UL                           /**< Mode HFRCODPLL for CMU_CALCTRL              */
#define _CMU_CALCTRL_UPSEL_HFRCOEM23               0x00000005UL                           /**< Mode HFRCOEM23 for CMU_CALCTRL              */
#define _CMU_CALCTRL_UPSEL_FSRCO                   0x00000008UL                           /**< Mode FSRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_UPSEL_LFRCO                   0x00000009UL                           /**< Mode LFRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_UPSEL_ULFRCO                  0x0000000AUL                           /**< Mode ULFRCO for CMU_CALCTRL                 */
#define CMU_CALCTRL_UPSEL_DEFAULT                  (_CMU_CALCTRL_UPSEL_DEFAULT << 24)     /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define CMU_CALCTRL_UPSEL_DISABLED                 (_CMU_CALCTRL_UPSEL_DISABLED << 24)    /**< Shifted mode DISABLED for CMU_CALCTRL       */
#define CMU_CALCTRL_UPSEL_PRS                      (_CMU_CALCTRL_UPSEL_PRS << 24)         /**< Shifted mode PRS for CMU_CALCTRL            */
#define CMU_CALCTRL_UPSEL_HFXO                     (_CMU_CALCTRL_UPSEL_HFXO << 24)        /**< Shifted mode HFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_UPSEL_LFXO                     (_CMU_CALCTRL_UPSEL_LFXO << 24)        /**< Shifted mode LFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_UPSEL_HFRCODPLL                (_CMU_CALCTRL_UPSEL_HFRCODPLL << 24)   /**< Shifted mode HFRCODPLL for CMU_CALCTRL      */
#define CMU_CALCTRL_UPSEL_HFRCOEM23                (_CMU_CALCTRL_UPSEL_HFRCOEM23 << 24)   /**< Shifted mode HFRCOEM23 for CMU_CALCTRL      */
#define CMU_CALCTRL_UPSEL_FSRCO                    (_CMU_CALCTRL_UPSEL_FSRCO << 24)       /**< Shifted mode FSRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_UPSEL_LFRCO                    (_CMU_CALCTRL_UPSEL_LFRCO << 24)       /**< Shifted mode LFRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_UPSEL_ULFRCO                   (_CMU_CALCTRL_UPSEL_ULFRCO << 24)      /**< Shifted mode ULFRCO for CMU_CALCTRL         */
#define _CMU_CALCTRL_DOWNSEL_SHIFT                 28                                     /**< Shift value for CMU_DOWNSEL                 */
#define _CMU_CALCTRL_DOWNSEL_MASK                  0xF0000000UL                           /**< Bit mask for CMU_DOWNSEL                    */
#define _CMU_CALCTRL_DOWNSEL_DEFAULT               0x00000000UL                           /**< Mode DEFAULT for CMU_CALCTRL                */
#define _CMU_CALCTRL_DOWNSEL_DISABLED              0x00000000UL                           /**< Mode DISABLED for CMU_CALCTRL               */
#define _CMU_CALCTRL_DOWNSEL_HCLK                  0x00000001UL                           /**< Mode HCLK for CMU_CALCTRL                   */
#define _CMU_CALCTRL_DOWNSEL_PRS                   0x00000002UL                           /**< Mode PRS for CMU_CALCTRL                    */
#define _CMU_CALCTRL_DOWNSEL_HFXO                  0x00000003UL                           /**< Mode HFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_DOWNSEL_LFXO                  0x00000004UL                           /**< Mode LFXO for CMU_CALCTRL                   */
#define _CMU_CALCTRL_DOWNSEL_HFRCODPLL             0x00000005UL                           /**< Mode HFRCODPLL for CMU_CALCTRL              */
#define _CMU_CALCTRL_DOWNSEL_HFRCOEM23             0x00000006UL                           /**< Mode HFRCOEM23 for CMU_CALCTRL              */
#define _CMU_CALCTRL_DOWNSEL_FSRCO                 0x00000009UL                           /**< Mode FSRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_DOWNSEL_LFRCO                 0x0000000AUL                           /**< Mode LFRCO for CMU_CALCTRL                  */
#define _CMU_CALCTRL_DOWNSEL_ULFRCO                0x0000000BUL                           /**< Mode ULFRCO for CMU_CALCTRL                 */
#define CMU_CALCTRL_DOWNSEL_DEFAULT                (_CMU_CALCTRL_DOWNSEL_DEFAULT << 28)   /**< Shifted mode DEFAULT for CMU_CALCTRL        */
#define CMU_CALCTRL_DOWNSEL_DISABLED               (_CMU_CALCTRL_DOWNSEL_DISABLED << 28)  /**< Shifted mode DISABLED for CMU_CALCTRL       */
#define CMU_CALCTRL_DOWNSEL_HCLK                   (_CMU_CALCTRL_DOWNSEL_HCLK << 28)      /**< Shifted mode HCLK for CMU_CALCTRL           */
#define CMU_CALCTRL_DOWNSEL_PRS                    (_CMU_CALCTRL_DOWNSEL_PRS << 28)       /**< Shifted mode PRS for CMU_CALCTRL            */
#define CMU_CALCTRL_DOWNSEL_HFXO                   (_CMU_CALCTRL_DOWNSEL_HFXO << 28)      /**< Shifted mode HFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_DOWNSEL_LFXO                   (_CMU_CALCTRL_DOWNSEL_LFXO << 28)      /**< Shifted mode LFXO for CMU_CALCTRL           */
#define CMU_CALCTRL_DOWNSEL_HFRCODPLL              (_CMU_CALCTRL_DOWNSEL_HFRCODPLL << 28) /**< Shifted mode HFRCODPLL for CMU_CALCTRL      */
#define CMU_CALCTRL_DOWNSEL_HFRCOEM23              (_CMU_CALCTRL_DOWNSEL_HFRCOEM23 << 28) /**< Shifted mode HFRCOEM23 for CMU_CALCTRL      */
#define CMU_CALCTRL_DOWNSEL_FSRCO                  (_CMU_CALCTRL_DOWNSEL_FSRCO << 28)     /**< Shifted mode FSRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_DOWNSEL_LFRCO                  (_CMU_CALCTRL_DOWNSEL_LFRCO << 28)     /**< Shifted mode LFRCO for CMU_CALCTRL          */
#define CMU_CALCTRL_DOWNSEL_ULFRCO                 (_CMU_CALCTRL_DOWNSEL_ULFRCO << 28)    /**< Shifted mode ULFRCO for CMU_CALCTRL         */

/* Bit fields for CMU CALCNT */
#define _CMU_CALCNT_RESETVALUE                     0x00000000UL                         /**< Default value for CMU_CALCNT                */
#define _CMU_CALCNT_MASK                           0x000FFFFFUL                         /**< Mask for CMU_CALCNT                         */
#define _CMU_CALCNT_CALCNT_SHIFT                   0                                    /**< Shift value for CMU_CALCNT                  */
#define _CMU_CALCNT_CALCNT_MASK                    0xFFFFFUL                            /**< Bit mask for CMU_CALCNT                     */
#define _CMU_CALCNT_CALCNT_DEFAULT                 0x00000000UL                         /**< Mode DEFAULT for CMU_CALCNT                 */
#define CMU_CALCNT_CALCNT_DEFAULT                  (_CMU_CALCNT_CALCNT_DEFAULT << 0)    /**< Shifted mode DEFAULT for CMU_CALCNT         */

/* Bit fields for CMU SYSCLKCTRL */
#define _CMU_SYSCLKCTRL_RESETVALUE                 0x00000001UL                              /**< Default value for CMU_SYSCLKCTRL            */
#define _CMU_SYSCLKCTRL_MASK                       0x00003507UL                              /**< Mask for CMU_SYSCLKCTRL                     */
#define _CMU_SYSCLKCTRL_CLKSEL_SHIFT               0                                         /**< Shift value for CMU_CLKSEL                  */
#define _CMU_SYSCLKCTRL_CLKSEL_MASK                0x7UL                                     /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_SYSCLKCTRL_CLKSEL_DEFAULT             0x00000001UL                              /**< Mode DEFAULT for CMU_SYSCLKCTRL             */
#define _CMU_SYSCLKCTRL_CLKSEL_FSRCO               0x00000001UL                              /**< Mode FSRCO for CMU_SYSCLKCTRL               */
#define _CMU_SYSCLKCTRL_CLKSEL_HFRCODPLL           0x00000002UL                              /**< Mode HFRCODPLL for CMU_SYSCLKCTRL           */
#define _CMU_SYSCLKCTRL_CLKSEL_HFXO                0x00000003UL                              /**< Mode HFXO for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_CLKSEL_CLKIN0              0x00000004UL                              /**< Mode CLKIN0 for CMU_SYSCLKCTRL              */
#define CMU_SYSCLKCTRL_CLKSEL_DEFAULT              (_CMU_SYSCLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_SYSCLKCTRL     */
#define CMU_SYSCLKCTRL_CLKSEL_FSRCO                (_CMU_SYSCLKCTRL_CLKSEL_FSRCO << 0)       /**< Shifted mode FSRCO for CMU_SYSCLKCTRL       */
#define CMU_SYSCLKCTRL_CLKSEL_HFRCODPLL            (_CMU_SYSCLKCTRL_CLKSEL_HFRCODPLL << 0)   /**< Shifted mode HFRCODPLL for CMU_SYSCLKCTRL   */
#define CMU_SYSCLKCTRL_CLKSEL_HFXO                 (_CMU_SYSCLKCTRL_CLKSEL_HFXO << 0)        /**< Shifted mode HFXO for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_CLKSEL_CLKIN0               (_CMU_SYSCLKCTRL_CLKSEL_CLKIN0 << 0)      /**< Shifted mode CLKIN0 for CMU_SYSCLKCTRL      */
#define CMU_SYSCLKCTRL_PCLKPRESC                   (0x1UL << 10)                             /**< PCLK Prescaler                              */
#define _CMU_SYSCLKCTRL_PCLKPRESC_SHIFT            10                                        /**< Shift value for CMU_PCLKPRESC               */
#define _CMU_SYSCLKCTRL_PCLKPRESC_MASK             0x400UL                                   /**< Bit mask for CMU_PCLKPRESC                  */
#define _CMU_SYSCLKCTRL_PCLKPRESC_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for CMU_SYSCLKCTRL             */
#define _CMU_SYSCLKCTRL_PCLKPRESC_DIV1             0x00000000UL                              /**< Mode DIV1 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_PCLKPRESC_DIV2             0x00000001UL                              /**< Mode DIV2 for CMU_SYSCLKCTRL                */
#define CMU_SYSCLKCTRL_PCLKPRESC_DEFAULT           (_CMU_SYSCLKCTRL_PCLKPRESC_DEFAULT << 10) /**< Shifted mode DEFAULT for CMU_SYSCLKCTRL     */
#define CMU_SYSCLKCTRL_PCLKPRESC_DIV1              (_CMU_SYSCLKCTRL_PCLKPRESC_DIV1 << 10)    /**< Shifted mode DIV1 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_PCLKPRESC_DIV2              (_CMU_SYSCLKCTRL_PCLKPRESC_DIV2 << 10)    /**< Shifted mode DIV2 for CMU_SYSCLKCTRL        */
#define _CMU_SYSCLKCTRL_HCLKPRESC_SHIFT            12                                        /**< Shift value for CMU_HCLKPRESC               */
#define _CMU_SYSCLKCTRL_HCLKPRESC_MASK             0x3000UL                                  /**< Bit mask for CMU_HCLKPRESC                  */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for CMU_SYSCLKCTRL             */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV1             0x00000000UL                              /**< Mode DIV1 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV2             0x00000001UL                              /**< Mode DIV2 for CMU_SYSCLKCTRL                */
#define _CMU_SYSCLKCTRL_HCLKPRESC_DIV4             0x00000003UL                              /**< Mode DIV4 for CMU_SYSCLKCTRL                */
#define CMU_SYSCLKCTRL_HCLKPRESC_DEFAULT           (_CMU_SYSCLKCTRL_HCLKPRESC_DEFAULT << 12) /**< Shifted mode DEFAULT for CMU_SYSCLKCTRL     */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV1              (_CMU_SYSCLKCTRL_HCLKPRESC_DIV1 << 12)    /**< Shifted mode DIV1 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV2              (_CMU_SYSCLKCTRL_HCLKPRESC_DIV2 << 12)    /**< Shifted mode DIV2 for CMU_SYSCLKCTRL        */
#define CMU_SYSCLKCTRL_HCLKPRESC_DIV4              (_CMU_SYSCLKCTRL_HCLKPRESC_DIV4 << 12)    /**< Shifted mode DIV4 for CMU_SYSCLKCTRL        */

/* Bit fields for CMU TRACECLKCTRL */
#define _CMU_TRACECLKCTRL_RESETVALUE               0x00000001UL                              /**< Default value for CMU_TRACECLKCTRL          */
#define _CMU_TRACECLKCTRL_MASK                     0x00000003UL                              /**< Mask for CMU_TRACECLKCTRL                   */
#define _CMU_TRACECLKCTRL_CLKSEL_SHIFT             0                                         /**< Shift value for CMU_CLKSEL                  */
#define _CMU_TRACECLKCTRL_CLKSEL_MASK              0x3UL                                     /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_TRACECLKCTRL_CLKSEL_DEFAULT           0x00000001UL                              /**< Mode DEFAULT for CMU_TRACECLKCTRL           */
#define _CMU_TRACECLKCTRL_CLKSEL_HCLK              0x00000002UL                              /**< Mode HCLK for CMU_TRACECLKCTRL              */
#define _CMU_TRACECLKCTRL_CLKSEL_HFRCOEM23         0x00000003UL                              /**< Mode HFRCOEM23 for CMU_TRACECLKCTRL         */
#define CMU_TRACECLKCTRL_CLKSEL_DEFAULT            (_CMU_TRACECLKCTRL_CLKSEL_DEFAULT << 0)   /**< Shifted mode DEFAULT for CMU_TRACECLKCTRL   */
#define CMU_TRACECLKCTRL_CLKSEL_HCLK               (_CMU_TRACECLKCTRL_CLKSEL_HCLK << 0)      /**< Shifted mode HCLK for CMU_TRACECLKCTRL      */
#define CMU_TRACECLKCTRL_CLKSEL_HFRCOEM23          (_CMU_TRACECLKCTRL_CLKSEL_HFRCOEM23 << 0) /**< Shifted mode HFRCOEM23 for CMU_TRACECLKCTRL */

/* Bit fields for CMU EXPORTCLKCTRL */
#define _CMU_EXPORTCLKCTRL_RESETVALUE              0x00000000UL                                    /**< Default value for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_MASK                    0x1F0F0F0FUL                                    /**< Mask for CMU_EXPORTCLKCTRL                  */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_SHIFT        0                                               /**< Shift value for CMU_CLKOUTSEL0              */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_MASK         0xFUL                                           /**< Bit mask for CMU_CLKOUTSEL0                 */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_DEFAULT      0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_DISABLED     0x00000000UL                                    /**< Mode DISABLED for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HCLK         0x00000001UL                                    /**< Mode HCLK for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFEXPCLK     0x00000002UL                                    /**< Mode HFEXPCLK for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_ULFRCO       0x00000003UL                                    /**< Mode ULFRCO for CMU_EXPORTCLKCTRL           */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFRCO        0x00000004UL                                    /**< Mode LFRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFXO         0x00000005UL                                    /**< Mode LFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCODPLL    0x00000006UL                                    /**< Mode HFRCODPLL for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCOEM23    0x00000007UL                                    /**< Mode HFRCOEM23 for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFXO         0x00000008UL                                    /**< Mode HFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL0_FSRCO        0x00000009UL                                    /**< Mode FSRCO for CMU_EXPORTCLKCTRL            */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_DEFAULT       (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_DEFAULT << 0)    /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_DISABLED      (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_DISABLED << 0)   /**< Shifted mode DISABLED for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HCLK          (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HCLK << 0)       /**< Shifted mode HCLK for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFEXPCLK      (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFEXPCLK << 0)   /**< Shifted mode HFEXPCLK for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_ULFRCO        (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_ULFRCO << 0)     /**< Shifted mode ULFRCO for CMU_EXPORTCLKCTRL   */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFRCO << 0)      /**< Shifted mode LFRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_LFXO << 0)       /**< Shifted mode LFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCODPLL     (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCODPLL << 0)  /**< Shifted mode HFRCODPLL for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCOEM23     (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFRCOEM23 << 0)  /**< Shifted mode HFRCOEM23 for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_HFXO << 0)       /**< Shifted mode HFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL0_FSRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL0_FSRCO << 0)      /**< Shifted mode FSRCO for CMU_EXPORTCLKCTRL    */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_SHIFT        8                                               /**< Shift value for CMU_CLKOUTSEL1              */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_MASK         0xF00UL                                         /**< Bit mask for CMU_CLKOUTSEL1                 */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_DEFAULT      0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_DISABLED     0x00000000UL                                    /**< Mode DISABLED for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HCLK         0x00000001UL                                    /**< Mode HCLK for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFEXPCLK     0x00000002UL                                    /**< Mode HFEXPCLK for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_ULFRCO       0x00000003UL                                    /**< Mode ULFRCO for CMU_EXPORTCLKCTRL           */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFRCO        0x00000004UL                                    /**< Mode LFRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFXO         0x00000005UL                                    /**< Mode LFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCODPLL    0x00000006UL                                    /**< Mode HFRCODPLL for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCOEM23    0x00000007UL                                    /**< Mode HFRCOEM23 for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFXO         0x00000008UL                                    /**< Mode HFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL1_FSRCO        0x00000009UL                                    /**< Mode FSRCO for CMU_EXPORTCLKCTRL            */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_DEFAULT       (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_DEFAULT << 8)    /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_DISABLED      (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_DISABLED << 8)   /**< Shifted mode DISABLED for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HCLK          (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HCLK << 8)       /**< Shifted mode HCLK for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFEXPCLK      (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFEXPCLK << 8)   /**< Shifted mode HFEXPCLK for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_ULFRCO        (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_ULFRCO << 8)     /**< Shifted mode ULFRCO for CMU_EXPORTCLKCTRL   */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFRCO << 8)      /**< Shifted mode LFRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_LFXO << 8)       /**< Shifted mode LFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCODPLL     (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCODPLL << 8)  /**< Shifted mode HFRCODPLL for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCOEM23     (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFRCOEM23 << 8)  /**< Shifted mode HFRCOEM23 for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_HFXO << 8)       /**< Shifted mode HFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL1_FSRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL1_FSRCO << 8)      /**< Shifted mode FSRCO for CMU_EXPORTCLKCTRL    */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_SHIFT        16                                              /**< Shift value for CMU_CLKOUTSEL2              */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_MASK         0xF0000UL                                       /**< Bit mask for CMU_CLKOUTSEL2                 */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_DEFAULT      0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_DISABLED     0x00000000UL                                    /**< Mode DISABLED for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HCLK         0x00000001UL                                    /**< Mode HCLK for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFEXPCLK     0x00000002UL                                    /**< Mode HFEXPCLK for CMU_EXPORTCLKCTRL         */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_ULFRCO       0x00000003UL                                    /**< Mode ULFRCO for CMU_EXPORTCLKCTRL           */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFRCO        0x00000004UL                                    /**< Mode LFRCO for CMU_EXPORTCLKCTRL            */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFXO         0x00000005UL                                    /**< Mode LFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCODPLL    0x00000006UL                                    /**< Mode HFRCODPLL for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCOEM23    0x00000007UL                                    /**< Mode HFRCOEM23 for CMU_EXPORTCLKCTRL        */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFXO         0x00000008UL                                    /**< Mode HFXO for CMU_EXPORTCLKCTRL             */
#define _CMU_EXPORTCLKCTRL_CLKOUTSEL2_FSRCO        0x00000009UL                                    /**< Mode FSRCO for CMU_EXPORTCLKCTRL            */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_DEFAULT       (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_DEFAULT << 16)   /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_DISABLED      (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_DISABLED << 16)  /**< Shifted mode DISABLED for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HCLK          (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HCLK << 16)      /**< Shifted mode HCLK for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFEXPCLK      (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFEXPCLK << 16)  /**< Shifted mode HFEXPCLK for CMU_EXPORTCLKCTRL */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_ULFRCO        (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_ULFRCO << 16)    /**< Shifted mode ULFRCO for CMU_EXPORTCLKCTRL   */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFRCO << 16)     /**< Shifted mode LFRCO for CMU_EXPORTCLKCTRL    */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_LFXO << 16)      /**< Shifted mode LFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCODPLL     (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCODPLL << 16) /**< Shifted mode HFRCODPLL for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCOEM23     (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFRCOEM23 << 16) /**< Shifted mode HFRCOEM23 for CMU_EXPORTCLKCTRL*/
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFXO          (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_HFXO << 16)      /**< Shifted mode HFXO for CMU_EXPORTCLKCTRL     */
#define CMU_EXPORTCLKCTRL_CLKOUTSEL2_FSRCO         (_CMU_EXPORTCLKCTRL_CLKOUTSEL2_FSRCO << 16)     /**< Shifted mode FSRCO for CMU_EXPORTCLKCTRL    */
#define _CMU_EXPORTCLKCTRL_PRESC_SHIFT             24                                              /**< Shift value for CMU_PRESC                   */
#define _CMU_EXPORTCLKCTRL_PRESC_MASK              0x1F000000UL                                    /**< Bit mask for CMU_PRESC                      */
#define _CMU_EXPORTCLKCTRL_PRESC_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for CMU_EXPORTCLKCTRL          */
#define CMU_EXPORTCLKCTRL_PRESC_DEFAULT            (_CMU_EXPORTCLKCTRL_PRESC_DEFAULT << 24)        /**< Shifted mode DEFAULT for CMU_EXPORTCLKCTRL  */

/* Bit fields for CMU DPLLREFCLKCTRL */
#define _CMU_DPLLREFCLKCTRL_RESETVALUE             0x00000000UL                               /**< Default value for CMU_DPLLREFCLKCTRL        */
#define _CMU_DPLLREFCLKCTRL_MASK                   0x00000003UL                               /**< Mask for CMU_DPLLREFCLKCTRL                 */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_SHIFT           0                                          /**< Shift value for CMU_CLKSEL                  */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_MASK            0x3UL                                      /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_DEFAULT         0x00000000UL                               /**< Mode DEFAULT for CMU_DPLLREFCLKCTRL         */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_DISABLED        0x00000000UL                               /**< Mode DISABLED for CMU_DPLLREFCLKCTRL        */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_HFXO            0x00000001UL                               /**< Mode HFXO for CMU_DPLLREFCLKCTRL            */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_LFXO            0x00000002UL                               /**< Mode LFXO for CMU_DPLLREFCLKCTRL            */
#define _CMU_DPLLREFCLKCTRL_CLKSEL_CLKIN0          0x00000003UL                               /**< Mode CLKIN0 for CMU_DPLLREFCLKCTRL          */
#define CMU_DPLLREFCLKCTRL_CLKSEL_DEFAULT          (_CMU_DPLLREFCLKCTRL_CLKSEL_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_DPLLREFCLKCTRL */
#define CMU_DPLLREFCLKCTRL_CLKSEL_DISABLED         (_CMU_DPLLREFCLKCTRL_CLKSEL_DISABLED << 0) /**< Shifted mode DISABLED for CMU_DPLLREFCLKCTRL*/
#define CMU_DPLLREFCLKCTRL_CLKSEL_HFXO             (_CMU_DPLLREFCLKCTRL_CLKSEL_HFXO << 0)     /**< Shifted mode HFXO for CMU_DPLLREFCLKCTRL    */
#define CMU_DPLLREFCLKCTRL_CLKSEL_LFXO             (_CMU_DPLLREFCLKCTRL_CLKSEL_LFXO << 0)     /**< Shifted mode LFXO for CMU_DPLLREFCLKCTRL    */
#define CMU_DPLLREFCLKCTRL_CLKSEL_CLKIN0           (_CMU_DPLLREFCLKCTRL_CLKSEL_CLKIN0 << 0)   /**< Shifted mode CLKIN0 for CMU_DPLLREFCLKCTRL  */

/* Bit fields for CMU EM01GRPACLKCTRL */
#define _CMU_EM01GRPACLKCTRL_RESETVALUE            0x00000001UL                                 /**< Default value for CMU_EM01GRPACLKCTRL       */
#define _CMU_EM01GRPACLKCTRL_MASK                  0x00000007UL                                 /**< Mask for CMU_EM01GRPACLKCTRL                */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_SHIFT          0                                            /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_MASK           0x7UL                                        /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_DEFAULT        0x00000001UL                                 /**< Mode DEFAULT for CMU_EM01GRPACLKCTRL        */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLL      0x00000001UL                                 /**< Mode HFRCODPLL for CMU_EM01GRPACLKCTRL      */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFXO           0x00000002UL                                 /**< Mode HFXO for CMU_EM01GRPACLKCTRL           */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_HFRCOEM23      0x00000003UL                                 /**< Mode HFRCOEM23 for CMU_EM01GRPACLKCTRL      */
#define _CMU_EM01GRPACLKCTRL_CLKSEL_FSRCO          0x00000004UL                                 /**< Mode FSRCO for CMU_EM01GRPACLKCTRL          */
#define CMU_EM01GRPACLKCTRL_CLKSEL_DEFAULT         (_CMU_EM01GRPACLKCTRL_CLKSEL_DEFAULT << 0)   /**< Shifted mode DEFAULT for CMU_EM01GRPACLKCTRL*/
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLL       (_CMU_EM01GRPACLKCTRL_CLKSEL_HFRCODPLL << 0) /**< Shifted mode HFRCODPLL for CMU_EM01GRPACLKCTRL*/
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFXO            (_CMU_EM01GRPACLKCTRL_CLKSEL_HFXO << 0)      /**< Shifted mode HFXO for CMU_EM01GRPACLKCTRL   */
#define CMU_EM01GRPACLKCTRL_CLKSEL_HFRCOEM23       (_CMU_EM01GRPACLKCTRL_CLKSEL_HFRCOEM23 << 0) /**< Shifted mode HFRCOEM23 for CMU_EM01GRPACLKCTRL*/
#define CMU_EM01GRPACLKCTRL_CLKSEL_FSRCO           (_CMU_EM01GRPACLKCTRL_CLKSEL_FSRCO << 0)     /**< Shifted mode FSRCO for CMU_EM01GRPACLKCTRL  */

/* Bit fields for CMU EM23GRPACLKCTRL */
#define _CMU_EM23GRPACLKCTRL_RESETVALUE            0x00000001UL                               /**< Default value for CMU_EM23GRPACLKCTRL       */
#define _CMU_EM23GRPACLKCTRL_MASK                  0x00000003UL                               /**< Mask for CMU_EM23GRPACLKCTRL                */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_SHIFT          0                                          /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_MASK           0x3UL                                      /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_DEFAULT        0x00000001UL                               /**< Mode DEFAULT for CMU_EM23GRPACLKCTRL        */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_LFRCO          0x00000001UL                               /**< Mode LFRCO for CMU_EM23GRPACLKCTRL          */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_LFXO           0x00000002UL                               /**< Mode LFXO for CMU_EM23GRPACLKCTRL           */
#define _CMU_EM23GRPACLKCTRL_CLKSEL_ULFRCO         0x00000003UL                               /**< Mode ULFRCO for CMU_EM23GRPACLKCTRL         */
#define CMU_EM23GRPACLKCTRL_CLKSEL_DEFAULT         (_CMU_EM23GRPACLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_EM23GRPACLKCTRL*/
#define CMU_EM23GRPACLKCTRL_CLKSEL_LFRCO           (_CMU_EM23GRPACLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_EM23GRPACLKCTRL  */
#define CMU_EM23GRPACLKCTRL_CLKSEL_LFXO            (_CMU_EM23GRPACLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_EM23GRPACLKCTRL   */
#define CMU_EM23GRPACLKCTRL_CLKSEL_ULFRCO          (_CMU_EM23GRPACLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_EM23GRPACLKCTRL */

/* Bit fields for CMU EM4GRPACLKCTRL */
#define _CMU_EM4GRPACLKCTRL_RESETVALUE             0x00000001UL                              /**< Default value for CMU_EM4GRPACLKCTRL        */
#define _CMU_EM4GRPACLKCTRL_MASK                   0x00000003UL                              /**< Mask for CMU_EM4GRPACLKCTRL                 */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_SHIFT           0                                         /**< Shift value for CMU_CLKSEL                  */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_MASK            0x3UL                                     /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_DEFAULT         0x00000001UL                              /**< Mode DEFAULT for CMU_EM4GRPACLKCTRL         */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_LFRCO           0x00000001UL                              /**< Mode LFRCO for CMU_EM4GRPACLKCTRL           */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_LFXO            0x00000002UL                              /**< Mode LFXO for CMU_EM4GRPACLKCTRL            */
#define _CMU_EM4GRPACLKCTRL_CLKSEL_ULFRCO          0x00000003UL                              /**< Mode ULFRCO for CMU_EM4GRPACLKCTRL          */
#define CMU_EM4GRPACLKCTRL_CLKSEL_DEFAULT          (_CMU_EM4GRPACLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_EM4GRPACLKCTRL */
#define CMU_EM4GRPACLKCTRL_CLKSEL_LFRCO            (_CMU_EM4GRPACLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_EM4GRPACLKCTRL   */
#define CMU_EM4GRPACLKCTRL_CLKSEL_LFXO             (_CMU_EM4GRPACLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_EM4GRPACLKCTRL    */
#define CMU_EM4GRPACLKCTRL_CLKSEL_ULFRCO           (_CMU_EM4GRPACLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_EM4GRPACLKCTRL  */

/* Bit fields for CMU IADCCLKCTRL */
#define _CMU_IADCCLKCTRL_RESETVALUE                0x00000001UL                               /**< Default value for CMU_IADCCLKCTRL           */
#define _CMU_IADCCLKCTRL_MASK                      0x00000003UL                               /**< Mask for CMU_IADCCLKCTRL                    */
#define _CMU_IADCCLKCTRL_CLKSEL_SHIFT              0                                          /**< Shift value for CMU_CLKSEL                  */
#define _CMU_IADCCLKCTRL_CLKSEL_MASK               0x3UL                                      /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_IADCCLKCTRL_CLKSEL_DEFAULT            0x00000001UL                               /**< Mode DEFAULT for CMU_IADCCLKCTRL            */
#define _CMU_IADCCLKCTRL_CLKSEL_EM01GRPACLK        0x00000001UL                               /**< Mode EM01GRPACLK for CMU_IADCCLKCTRL        */
#define _CMU_IADCCLKCTRL_CLKSEL_HFRCOEM23          0x00000002UL                               /**< Mode HFRCOEM23 for CMU_IADCCLKCTRL          */
#define _CMU_IADCCLKCTRL_CLKSEL_FSRCO              0x00000003UL                               /**< Mode FSRCO for CMU_IADCCLKCTRL              */
#define CMU_IADCCLKCTRL_CLKSEL_DEFAULT             (_CMU_IADCCLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_IADCCLKCTRL    */
#define CMU_IADCCLKCTRL_CLKSEL_EM01GRPACLK         (_CMU_IADCCLKCTRL_CLKSEL_EM01GRPACLK << 0) /**< Shifted mode EM01GRPACLK for CMU_IADCCLKCTRL*/
#define CMU_IADCCLKCTRL_CLKSEL_HFRCOEM23           (_CMU_IADCCLKCTRL_CLKSEL_HFRCOEM23 << 0)   /**< Shifted mode HFRCOEM23 for CMU_IADCCLKCTRL  */
#define CMU_IADCCLKCTRL_CLKSEL_FSRCO               (_CMU_IADCCLKCTRL_CLKSEL_FSRCO << 0)       /**< Shifted mode FSRCO for CMU_IADCCLKCTRL      */

/* Bit fields for CMU WDOG0CLKCTRL */
#define _CMU_WDOG0CLKCTRL_RESETVALUE               0x00000001UL                                /**< Default value for CMU_WDOG0CLKCTRL          */
#define _CMU_WDOG0CLKCTRL_MASK                     0x00000007UL                                /**< Mask for CMU_WDOG0CLKCTRL                   */
#define _CMU_WDOG0CLKCTRL_CLKSEL_SHIFT             0                                           /**< Shift value for CMU_CLKSEL                  */
#define _CMU_WDOG0CLKCTRL_CLKSEL_MASK              0x7UL                                       /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_WDOG0CLKCTRL_CLKSEL_DEFAULT           0x00000001UL                                /**< Mode DEFAULT for CMU_WDOG0CLKCTRL           */
#define _CMU_WDOG0CLKCTRL_CLKSEL_LFRCO             0x00000001UL                                /**< Mode LFRCO for CMU_WDOG0CLKCTRL             */
#define _CMU_WDOG0CLKCTRL_CLKSEL_LFXO              0x00000002UL                                /**< Mode LFXO for CMU_WDOG0CLKCTRL              */
#define _CMU_WDOG0CLKCTRL_CLKSEL_ULFRCO            0x00000003UL                                /**< Mode ULFRCO for CMU_WDOG0CLKCTRL            */
#define _CMU_WDOG0CLKCTRL_CLKSEL_HCLKDIV1024       0x00000004UL                                /**< Mode HCLKDIV1024 for CMU_WDOG0CLKCTRL       */
#define CMU_WDOG0CLKCTRL_CLKSEL_DEFAULT            (_CMU_WDOG0CLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_WDOG0CLKCTRL   */
#define CMU_WDOG0CLKCTRL_CLKSEL_LFRCO              (_CMU_WDOG0CLKCTRL_CLKSEL_LFRCO << 0)       /**< Shifted mode LFRCO for CMU_WDOG0CLKCTRL     */
#define CMU_WDOG0CLKCTRL_CLKSEL_LFXO               (_CMU_WDOG0CLKCTRL_CLKSEL_LFXO << 0)        /**< Shifted mode LFXO for CMU_WDOG0CLKCTRL      */
#define CMU_WDOG0CLKCTRL_CLKSEL_ULFRCO             (_CMU_WDOG0CLKCTRL_CLKSEL_ULFRCO << 0)      /**< Shifted mode ULFRCO for CMU_WDOG0CLKCTRL    */
#define CMU_WDOG0CLKCTRL_CLKSEL_HCLKDIV1024        (_CMU_WDOG0CLKCTRL_CLKSEL_HCLKDIV1024 << 0) /**< Shifted mode HCLKDIV1024 for CMU_WDOG0CLKCTRL*/

/* Bit fields for CMU WDOG1CLKCTRL */
#define _CMU_WDOG1CLKCTRL_RESETVALUE               0x00000001UL                                /**< Default value for CMU_WDOG1CLKCTRL          */
#define _CMU_WDOG1CLKCTRL_MASK                     0x00000007UL                                /**< Mask for CMU_WDOG1CLKCTRL                   */
#define _CMU_WDOG1CLKCTRL_CLKSEL_SHIFT             0                                           /**< Shift value for CMU_CLKSEL                  */
#define _CMU_WDOG1CLKCTRL_CLKSEL_MASK              0x7UL                                       /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_WDOG1CLKCTRL_CLKSEL_DEFAULT           0x00000001UL                                /**< Mode DEFAULT for CMU_WDOG1CLKCTRL           */
#define _CMU_WDOG1CLKCTRL_CLKSEL_LFRCO             0x00000001UL                                /**< Mode LFRCO for CMU_WDOG1CLKCTRL             */
#define _CMU_WDOG1CLKCTRL_CLKSEL_LFXO              0x00000002UL                                /**< Mode LFXO for CMU_WDOG1CLKCTRL              */
#define _CMU_WDOG1CLKCTRL_CLKSEL_ULFRCO            0x00000003UL                                /**< Mode ULFRCO for CMU_WDOG1CLKCTRL            */
#define _CMU_WDOG1CLKCTRL_CLKSEL_HCLKDIV1024       0x00000004UL                                /**< Mode HCLKDIV1024 for CMU_WDOG1CLKCTRL       */
#define CMU_WDOG1CLKCTRL_CLKSEL_DEFAULT            (_CMU_WDOG1CLKCTRL_CLKSEL_DEFAULT << 0)     /**< Shifted mode DEFAULT for CMU_WDOG1CLKCTRL   */
#define CMU_WDOG1CLKCTRL_CLKSEL_LFRCO              (_CMU_WDOG1CLKCTRL_CLKSEL_LFRCO << 0)       /**< Shifted mode LFRCO for CMU_WDOG1CLKCTRL     */
#define CMU_WDOG1CLKCTRL_CLKSEL_LFXO               (_CMU_WDOG1CLKCTRL_CLKSEL_LFXO << 0)        /**< Shifted mode LFXO for CMU_WDOG1CLKCTRL      */
#define CMU_WDOG1CLKCTRL_CLKSEL_ULFRCO             (_CMU_WDOG1CLKCTRL_CLKSEL_ULFRCO << 0)      /**< Shifted mode ULFRCO for CMU_WDOG1CLKCTRL    */
#define CMU_WDOG1CLKCTRL_CLKSEL_HCLKDIV1024        (_CMU_WDOG1CLKCTRL_CLKSEL_HCLKDIV1024 << 0) /**< Shifted mode HCLKDIV1024 for CMU_WDOG1CLKCTRL*/

/* Bit fields for CMU RTCCCLKCTRL */
#define _CMU_RTCCCLKCTRL_RESETVALUE                0x00000001UL                           /**< Default value for CMU_RTCCCLKCTRL           */
#define _CMU_RTCCCLKCTRL_MASK                      0x00000003UL                           /**< Mask for CMU_RTCCCLKCTRL                    */
#define _CMU_RTCCCLKCTRL_CLKSEL_SHIFT              0                                      /**< Shift value for CMU_CLKSEL                  */
#define _CMU_RTCCCLKCTRL_CLKSEL_MASK               0x3UL                                  /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_RTCCCLKCTRL_CLKSEL_DEFAULT            0x00000001UL                           /**< Mode DEFAULT for CMU_RTCCCLKCTRL            */
#define _CMU_RTCCCLKCTRL_CLKSEL_LFRCO              0x00000001UL                           /**< Mode LFRCO for CMU_RTCCCLKCTRL              */
#define _CMU_RTCCCLKCTRL_CLKSEL_LFXO               0x00000002UL                           /**< Mode LFXO for CMU_RTCCCLKCTRL               */
#define _CMU_RTCCCLKCTRL_CLKSEL_ULFRCO             0x00000003UL                           /**< Mode ULFRCO for CMU_RTCCCLKCTRL             */
#define CMU_RTCCCLKCTRL_CLKSEL_DEFAULT             (_CMU_RTCCCLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_RTCCCLKCTRL    */
#define CMU_RTCCCLKCTRL_CLKSEL_LFRCO               (_CMU_RTCCCLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_RTCCCLKCTRL      */
#define CMU_RTCCCLKCTRL_CLKSEL_LFXO                (_CMU_RTCCCLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_RTCCCLKCTRL       */
#define CMU_RTCCCLKCTRL_CLKSEL_ULFRCO              (_CMU_RTCCCLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_RTCCCLKCTRL     */

/* Bit fields for CMU PRORTCCLKCTRL */
#define _CMU_PRORTCCLKCTRL_RESETVALUE              0x00000001UL                             /**< Default value for CMU_PRORTCCLKCTRL         */
#define _CMU_PRORTCCLKCTRL_MASK                    0x00000003UL                             /**< Mask for CMU_PRORTCCLKCTRL                  */
#define _CMU_PRORTCCLKCTRL_CLKSEL_SHIFT            0                                        /**< Shift value for CMU_CLKSEL                  */
#define _CMU_PRORTCCLKCTRL_CLKSEL_MASK             0x3UL                                    /**< Bit mask for CMU_CLKSEL                     */
#define _CMU_PRORTCCLKCTRL_CLKSEL_DEFAULT          0x00000001UL                             /**< Mode DEFAULT for CMU_PRORTCCLKCTRL          */
#define _CMU_PRORTCCLKCTRL_CLKSEL_LFRCO            0x00000001UL                             /**< Mode LFRCO for CMU_PRORTCCLKCTRL            */
#define _CMU_PRORTCCLKCTRL_CLKSEL_LFXO             0x00000002UL                             /**< Mode LFXO for CMU_PRORTCCLKCTRL             */
#define _CMU_PRORTCCLKCTRL_CLKSEL_ULFRCO           0x00000003UL                             /**< Mode ULFRCO for CMU_PRORTCCLKCTRL           */
#define CMU_PRORTCCLKCTRL_CLKSEL_DEFAULT           (_CMU_PRORTCCLKCTRL_CLKSEL_DEFAULT << 0) /**< Shifted mode DEFAULT for CMU_PRORTCCLKCTRL  */
#define CMU_PRORTCCLKCTRL_CLKSEL_LFRCO             (_CMU_PRORTCCLKCTRL_CLKSEL_LFRCO << 0)   /**< Shifted mode LFRCO for CMU_PRORTCCLKCTRL    */
#define CMU_PRORTCCLKCTRL_CLKSEL_LFXO              (_CMU_PRORTCCLKCTRL_CLKSEL_LFXO << 0)    /**< Shifted mode LFXO for CMU_PRORTCCLKCTRL     */
#define CMU_PRORTCCLKCTRL_CLKSEL_ULFRCO            (_CMU_PRORTCCLKCTRL_CLKSEL_ULFRCO << 0)  /**< Shifted mode ULFRCO for CMU_PRORTCCLKCTRL   */

/* Bit fields for CMU RADIOCLKCTRL */
#define _CMU_RADIOCLKCTRL_RESETVALUE               0x00000000UL                         /**< Default value for CMU_RADIOCLKCTRL          */
#define _CMU_RADIOCLKCTRL_MASK                     0x00000001UL                         /**< Mask for CMU_RADIOCLKCTRL                   */
#define CMU_RADIOCLKCTRL_EN                        (0x1UL << 0)                         /**< Enable                                      */
#define _CMU_RADIOCLKCTRL_EN_SHIFT                 0                                    /**< Shift value for CMU_EN                      */
#define _CMU_RADIOCLKCTRL_EN_MASK                  0x1UL                                /**< Bit mask for CMU_EN                         */
#define _CMU_RADIOCLKCTRL_EN_DEFAULT               0x00000000UL                         /**< Mode DEFAULT for CMU_RADIOCLKCTRL           */
#define CMU_RADIOCLKCTRL_EN_DEFAULT                (_CMU_RADIOCLKCTRL_EN_DEFAULT << 0)  /**< Shifted mode DEFAULT for CMU_RADIOCLKCTRL   */

/** @} End of group EFR32MG21_CMU_BitFields */
/** @} End of group EFR32MG21_CMU */
/** @} End of group Parts */
#endif /* EFR32MG21_CMU_H */
