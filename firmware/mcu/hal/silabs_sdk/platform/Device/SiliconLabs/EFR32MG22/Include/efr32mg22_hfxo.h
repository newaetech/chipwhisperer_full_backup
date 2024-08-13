/**************************************************************************//**
 * @file
 * @brief EFR32MG22 HFXO register and bit field definitions
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
#ifndef EFR32MG22_HFXO_H
#define EFR32MG22_HFXO_H
#define HFXO_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG22_HFXO HFXO
 * @{
 * @brief EFR32MG22 HFXO Register Declaration.
 *****************************************************************************/

/** HFXO Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP version ID                                      */
  uint32_t       RESERVED0[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG;                       /**< Crystal Configuration Register                     */
  uint32_t       RESERVED1[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL;                      /**< Crystal Control Register                           */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CFG;                           /**< Configuration Register                             */
  uint32_t       RESERVED3[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  uint32_t       RESERVED4[9U];                 /**< Reserved for future use                            */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  uint32_t       RESERVED5[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  uint32_t       RESERVED6[5U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED7[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Configuration Lock Register                        */
  uint32_t       RESERVED8[991U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP version ID                                      */
  uint32_t       RESERVED9[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG_SET;                   /**< Crystal Configuration Register                     */
  uint32_t       RESERVED10[1U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL_SET;                  /**< Crystal Control Register                           */
  uint32_t       RESERVED11[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CFG_SET;                       /**< Configuration Register                             */
  uint32_t       RESERVED12[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  uint32_t       RESERVED13[9U];                /**< Reserved for future use                            */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  uint32_t       RESERVED15[5U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED16[2U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED17[991U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP version ID                                      */
  uint32_t       RESERVED18[3U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG_CLR;                   /**< Crystal Configuration Register                     */
  uint32_t       RESERVED19[1U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL_CLR;                  /**< Crystal Control Register                           */
  uint32_t       RESERVED20[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CFG_CLR;                       /**< Configuration Register                             */
  uint32_t       RESERVED21[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  uint32_t       RESERVED22[9U];                /**< Reserved for future use                            */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  uint32_t       RESERVED23[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  uint32_t       RESERVED24[5U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED25[2U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Configuration Lock Register                        */
  uint32_t       RESERVED26[991U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP version ID                                      */
  uint32_t       RESERVED27[3U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCFG_TGL;                   /**< Crystal Configuration Register                     */
  uint32_t       RESERVED28[1U];                /**< Reserved for future use                            */
  __IOM uint32_t XTALCTRL_TGL;                  /**< Crystal Control Register                           */
  uint32_t       RESERVED29[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CFG_TGL;                       /**< Configuration Register                             */
  uint32_t       RESERVED30[1U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  uint32_t       RESERVED31[9U];                /**< Reserved for future use                            */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  uint32_t       RESERVED32[1U];                /**< Reserved for future use                            */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  uint32_t       RESERVED33[5U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED34[2U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Configuration Lock Register                        */
} HFXO_TypeDef;
/** @} End of group EFR32MG22_HFXO */

/**************************************************************************//**
 * @addtogroup EFR32MG22_HFXO
 * @{
 * @defgroup EFR32MG22_HFXO_BitFields HFXO Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for HFXO IPVERSION */
#define _HFXO_IPVERSION_RESETVALUE                0x00000002UL                             /**< Default value for HFXO_IPVERSION            */
#define _HFXO_IPVERSION_MASK                      0xFFFFFFFFUL                             /**< Mask for HFXO_IPVERSION                     */
#define _HFXO_IPVERSION_IPVERSION_SHIFT           0                                        /**< Shift value for HFXO_IPVERSION              */
#define _HFXO_IPVERSION_IPVERSION_MASK            0xFFFFFFFFUL                             /**< Bit mask for HFXO_IPVERSION                 */
#define _HFXO_IPVERSION_IPVERSION_DEFAULT         0x00000002UL                             /**< Mode DEFAULT for HFXO_IPVERSION             */
#define HFXO_IPVERSION_IPVERSION_DEFAULT          (_HFXO_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_IPVERSION     */

/* Bit fields for HFXO XTALCFG */
#define _HFXO_XTALCFG_RESETVALUE                  0x044334CBUL                                  /**< Default value for HFXO_XTALCFG              */
#define _HFXO_XTALCFG_MASK                        0x0FFFFFFFUL                                  /**< Mask for HFXO_XTALCFG                       */
#define _HFXO_XTALCFG_COREBIASSTARTUPI_SHIFT      0                                             /**< Shift value for HFXO_COREBIASSTARTUPI       */
#define _HFXO_XTALCFG_COREBIASSTARTUPI_MASK       0x3FUL                                        /**< Bit mask for HFXO_COREBIASSTARTUPI          */
#define _HFXO_XTALCFG_COREBIASSTARTUPI_DEFAULT    0x0000000BUL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_COREBIASSTARTUPI_DEFAULT     (_HFXO_XTALCFG_COREBIASSTARTUPI_DEFAULT << 0) /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_COREBIASSTARTUP_SHIFT       6                                             /**< Shift value for HFXO_COREBIASSTARTUP        */
#define _HFXO_XTALCFG_COREBIASSTARTUP_MASK        0xFC0UL                                       /**< Bit mask for HFXO_COREBIASSTARTUP           */
#define _HFXO_XTALCFG_COREBIASSTARTUP_DEFAULT     0x00000013UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_COREBIASSTARTUP_DEFAULT      (_HFXO_XTALCFG_COREBIASSTARTUP_DEFAULT << 6)  /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_CTUNEXISTARTUP_SHIFT        12                                            /**< Shift value for HFXO_CTUNEXISTARTUP         */
#define _HFXO_XTALCFG_CTUNEXISTARTUP_MASK         0xF000UL                                      /**< Bit mask for HFXO_CTUNEXISTARTUP            */
#define _HFXO_XTALCFG_CTUNEXISTARTUP_DEFAULT      0x00000003UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_CTUNEXISTARTUP_DEFAULT       (_HFXO_XTALCFG_CTUNEXISTARTUP_DEFAULT << 12)  /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_CTUNEXOSTARTUP_SHIFT        16                                            /**< Shift value for HFXO_CTUNEXOSTARTUP         */
#define _HFXO_XTALCFG_CTUNEXOSTARTUP_MASK         0xF0000UL                                     /**< Bit mask for HFXO_CTUNEXOSTARTUP            */
#define _HFXO_XTALCFG_CTUNEXOSTARTUP_DEFAULT      0x00000003UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define HFXO_XTALCFG_CTUNEXOSTARTUP_DEFAULT       (_HFXO_XTALCFG_CTUNEXOSTARTUP_DEFAULT << 16)  /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_SHIFT         20                                            /**< Shift value for HFXO_TIMEOUTSTEADY          */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_MASK          0xF00000UL                                    /**< Bit mask for HFXO_TIMEOUTSTEADY             */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_DEFAULT       0x00000004UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T16US         0x00000000UL                                  /**< Mode T16US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T41US         0x00000001UL                                  /**< Mode T41US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T83US         0x00000002UL                                  /**< Mode T83US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T125US        0x00000003UL                                  /**< Mode T125US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T166US        0x00000004UL                                  /**< Mode T166US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T208US        0x00000005UL                                  /**< Mode T208US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T250US        0x00000006UL                                  /**< Mode T250US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T333US        0x00000007UL                                  /**< Mode T333US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T416US        0x00000008UL                                  /**< Mode T416US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T500US        0x00000009UL                                  /**< Mode T500US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T666US        0x0000000AUL                                  /**< Mode T666US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T833US        0x0000000BUL                                  /**< Mode T833US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T1666US       0x0000000CUL                                  /**< Mode T1666US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T2500US       0x0000000DUL                                  /**< Mode T2500US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T4166US       0x0000000EUL                                  /**< Mode T4166US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTSTEADY_T7500US       0x0000000FUL                                  /**< Mode T7500US for HFXO_XTALCFG               */
#define HFXO_XTALCFG_TIMEOUTSTEADY_DEFAULT        (_HFXO_XTALCFG_TIMEOUTSTEADY_DEFAULT << 20)   /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T16US          (_HFXO_XTALCFG_TIMEOUTSTEADY_T16US << 20)     /**< Shifted mode T16US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T41US          (_HFXO_XTALCFG_TIMEOUTSTEADY_T41US << 20)     /**< Shifted mode T41US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T83US          (_HFXO_XTALCFG_TIMEOUTSTEADY_T83US << 20)     /**< Shifted mode T83US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T125US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T125US << 20)    /**< Shifted mode T125US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T166US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T166US << 20)    /**< Shifted mode T166US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T208US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T208US << 20)    /**< Shifted mode T208US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T250US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T250US << 20)    /**< Shifted mode T250US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T333US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T333US << 20)    /**< Shifted mode T333US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T416US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T416US << 20)    /**< Shifted mode T416US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T500US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T500US << 20)    /**< Shifted mode T500US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T666US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T666US << 20)    /**< Shifted mode T666US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T833US         (_HFXO_XTALCFG_TIMEOUTSTEADY_T833US << 20)    /**< Shifted mode T833US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T1666US        (_HFXO_XTALCFG_TIMEOUTSTEADY_T1666US << 20)   /**< Shifted mode T1666US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T2500US        (_HFXO_XTALCFG_TIMEOUTSTEADY_T2500US << 20)   /**< Shifted mode T2500US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T4166US        (_HFXO_XTALCFG_TIMEOUTSTEADY_T4166US << 20)   /**< Shifted mode T4166US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTSTEADY_T7500US        (_HFXO_XTALCFG_TIMEOUTSTEADY_T7500US << 20)   /**< Shifted mode T7500US for HFXO_XTALCFG       */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_SHIFT          24                                            /**< Shift value for HFXO_TIMEOUTCBLSB           */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_MASK           0xF000000UL                                   /**< Bit mask for HFXO_TIMEOUTCBLSB              */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_DEFAULT        0x00000004UL                                  /**< Mode DEFAULT for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T8US           0x00000000UL                                  /**< Mode T8US for HFXO_XTALCFG                  */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T20US          0x00000001UL                                  /**< Mode T20US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T41US          0x00000002UL                                  /**< Mode T41US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T62US          0x00000003UL                                  /**< Mode T62US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T83US          0x00000004UL                                  /**< Mode T83US for HFXO_XTALCFG                 */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T104US         0x00000005UL                                  /**< Mode T104US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T125US         0x00000006UL                                  /**< Mode T125US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T166US         0x00000007UL                                  /**< Mode T166US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T208US         0x00000008UL                                  /**< Mode T208US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T250US         0x00000009UL                                  /**< Mode T250US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T333US         0x0000000AUL                                  /**< Mode T333US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T416US         0x0000000BUL                                  /**< Mode T416US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T833US         0x0000000CUL                                  /**< Mode T833US for HFXO_XTALCFG                */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T1250US        0x0000000DUL                                  /**< Mode T1250US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T2083US        0x0000000EUL                                  /**< Mode T2083US for HFXO_XTALCFG               */
#define _HFXO_XTALCFG_TIMEOUTCBLSB_T3750US        0x0000000FUL                                  /**< Mode T3750US for HFXO_XTALCFG               */
#define HFXO_XTALCFG_TIMEOUTCBLSB_DEFAULT         (_HFXO_XTALCFG_TIMEOUTCBLSB_DEFAULT << 24)    /**< Shifted mode DEFAULT for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T8US            (_HFXO_XTALCFG_TIMEOUTCBLSB_T8US << 24)       /**< Shifted mode T8US for HFXO_XTALCFG          */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T20US           (_HFXO_XTALCFG_TIMEOUTCBLSB_T20US << 24)      /**< Shifted mode T20US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T41US           (_HFXO_XTALCFG_TIMEOUTCBLSB_T41US << 24)      /**< Shifted mode T41US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T62US           (_HFXO_XTALCFG_TIMEOUTCBLSB_T62US << 24)      /**< Shifted mode T62US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T83US           (_HFXO_XTALCFG_TIMEOUTCBLSB_T83US << 24)      /**< Shifted mode T83US for HFXO_XTALCFG         */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T104US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T104US << 24)     /**< Shifted mode T104US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T125US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T125US << 24)     /**< Shifted mode T125US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T166US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T166US << 24)     /**< Shifted mode T166US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T208US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T208US << 24)     /**< Shifted mode T208US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T250US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T250US << 24)     /**< Shifted mode T250US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T333US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T333US << 24)     /**< Shifted mode T333US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T416US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T416US << 24)     /**< Shifted mode T416US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T833US          (_HFXO_XTALCFG_TIMEOUTCBLSB_T833US << 24)     /**< Shifted mode T833US for HFXO_XTALCFG        */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T1250US         (_HFXO_XTALCFG_TIMEOUTCBLSB_T1250US << 24)    /**< Shifted mode T1250US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T2083US         (_HFXO_XTALCFG_TIMEOUTCBLSB_T2083US << 24)    /**< Shifted mode T2083US for HFXO_XTALCFG       */
#define HFXO_XTALCFG_TIMEOUTCBLSB_T3750US         (_HFXO_XTALCFG_TIMEOUTCBLSB_T3750US << 24)    /**< Shifted mode T3750US for HFXO_XTALCFG       */

/* Bit fields for HFXO XTALCTRL */
#define _HFXO_XTALCTRL_RESETVALUE                 0x0F8C8C10UL                                   /**< Default value for HFXO_XTALCTRL             */
#define _HFXO_XTALCTRL_MASK                       0x8FFFFFFFUL                                   /**< Mask for HFXO_XTALCTRL                      */
#define _HFXO_XTALCTRL_COREBIASANA_SHIFT          0                                              /**< Shift value for HFXO_COREBIASANA            */
#define _HFXO_XTALCTRL_COREBIASANA_MASK           0xFFUL                                         /**< Bit mask for HFXO_COREBIASANA               */
#define _HFXO_XTALCTRL_COREBIASANA_DEFAULT        0x00000010UL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_COREBIASANA_DEFAULT         (_HFXO_XTALCTRL_COREBIASANA_DEFAULT << 0)      /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define _HFXO_XTALCTRL_CTUNEXIANA_SHIFT           8                                              /**< Shift value for HFXO_CTUNEXIANA             */
#define _HFXO_XTALCTRL_CTUNEXIANA_MASK            0xFF00UL                                       /**< Bit mask for HFXO_CTUNEXIANA                */
#define _HFXO_XTALCTRL_CTUNEXIANA_DEFAULT         0x0000008CUL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_CTUNEXIANA_DEFAULT          (_HFXO_XTALCTRL_CTUNEXIANA_DEFAULT << 8)       /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define _HFXO_XTALCTRL_CTUNEXOANA_SHIFT           16                                             /**< Shift value for HFXO_CTUNEXOANA             */
#define _HFXO_XTALCTRL_CTUNEXOANA_MASK            0xFF0000UL                                     /**< Bit mask for HFXO_CTUNEXOANA                */
#define _HFXO_XTALCTRL_CTUNEXOANA_DEFAULT         0x0000008CUL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_CTUNEXOANA_DEFAULT          (_HFXO_XTALCTRL_CTUNEXOANA_DEFAULT << 16)      /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define _HFXO_XTALCTRL_CTUNEFIXANA_SHIFT          24                                             /**< Shift value for HFXO_CTUNEFIXANA            */
#define _HFXO_XTALCTRL_CTUNEFIXANA_MASK           0x3000000UL                                    /**< Bit mask for HFXO_CTUNEFIXANA               */
#define _HFXO_XTALCTRL_CTUNEFIXANA_DEFAULT        0x00000003UL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define _HFXO_XTALCTRL_CTUNEFIXANA_NONE           0x00000000UL                                   /**< Mode NONE for HFXO_XTALCTRL                 */
#define _HFXO_XTALCTRL_CTUNEFIXANA_XI             0x00000001UL                                   /**< Mode XI for HFXO_XTALCTRL                   */
#define _HFXO_XTALCTRL_CTUNEFIXANA_XO             0x00000002UL                                   /**< Mode XO for HFXO_XTALCTRL                   */
#define _HFXO_XTALCTRL_CTUNEFIXANA_BOTH           0x00000003UL                                   /**< Mode BOTH for HFXO_XTALCTRL                 */
#define HFXO_XTALCTRL_CTUNEFIXANA_DEFAULT         (_HFXO_XTALCTRL_CTUNEFIXANA_DEFAULT << 24)     /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define HFXO_XTALCTRL_CTUNEFIXANA_NONE            (_HFXO_XTALCTRL_CTUNEFIXANA_NONE << 24)        /**< Shifted mode NONE for HFXO_XTALCTRL         */
#define HFXO_XTALCTRL_CTUNEFIXANA_XI              (_HFXO_XTALCTRL_CTUNEFIXANA_XI << 24)          /**< Shifted mode XI for HFXO_XTALCTRL           */
#define HFXO_XTALCTRL_CTUNEFIXANA_XO              (_HFXO_XTALCTRL_CTUNEFIXANA_XO << 24)          /**< Shifted mode XO for HFXO_XTALCTRL           */
#define HFXO_XTALCTRL_CTUNEFIXANA_BOTH            (_HFXO_XTALCTRL_CTUNEFIXANA_BOTH << 24)        /**< Shifted mode BOTH for HFXO_XTALCTRL         */
#define _HFXO_XTALCTRL_COREDGENANA_SHIFT          26                                             /**< Shift value for HFXO_COREDGENANA            */
#define _HFXO_XTALCTRL_COREDGENANA_MASK           0xC000000UL                                    /**< Bit mask for HFXO_COREDGENANA               */
#define _HFXO_XTALCTRL_COREDGENANA_DEFAULT        0x00000003UL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define _HFXO_XTALCTRL_COREDGENANA_NONE           0x00000000UL                                   /**< Mode NONE for HFXO_XTALCTRL                 */
#define _HFXO_XTALCTRL_COREDGENANA_DGEN33         0x00000001UL                                   /**< Mode DGEN33 for HFXO_XTALCTRL               */
#define _HFXO_XTALCTRL_COREDGENANA_DGEN50         0x00000002UL                                   /**< Mode DGEN50 for HFXO_XTALCTRL               */
#define _HFXO_XTALCTRL_COREDGENANA_DGEN100        0x00000003UL                                   /**< Mode DGEN100 for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_COREDGENANA_DEFAULT         (_HFXO_XTALCTRL_COREDGENANA_DEFAULT << 26)     /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */
#define HFXO_XTALCTRL_COREDGENANA_NONE            (_HFXO_XTALCTRL_COREDGENANA_NONE << 26)        /**< Shifted mode NONE for HFXO_XTALCTRL         */
#define HFXO_XTALCTRL_COREDGENANA_DGEN33          (_HFXO_XTALCTRL_COREDGENANA_DGEN33 << 26)      /**< Shifted mode DGEN33 for HFXO_XTALCTRL       */
#define HFXO_XTALCTRL_COREDGENANA_DGEN50          (_HFXO_XTALCTRL_COREDGENANA_DGEN50 << 26)      /**< Shifted mode DGEN50 for HFXO_XTALCTRL       */
#define HFXO_XTALCTRL_COREDGENANA_DGEN100         (_HFXO_XTALCTRL_COREDGENANA_DGEN100 << 26)     /**< Shifted mode DGEN100 for HFXO_XTALCTRL      */
#define HFXO_XTALCTRL_SKIPCOREBIASOPT             (0x1UL << 31)                                  /**< Skip Core Bias Optimization                 */
#define _HFXO_XTALCTRL_SKIPCOREBIASOPT_SHIFT      31                                             /**< Shift value for HFXO_SKIPCOREBIASOPT        */
#define _HFXO_XTALCTRL_SKIPCOREBIASOPT_MASK       0x80000000UL                                   /**< Bit mask for HFXO_SKIPCOREBIASOPT           */
#define _HFXO_XTALCTRL_SKIPCOREBIASOPT_DEFAULT    0x00000000UL                                   /**< Mode DEFAULT for HFXO_XTALCTRL              */
#define HFXO_XTALCTRL_SKIPCOREBIASOPT_DEFAULT     (_HFXO_XTALCTRL_SKIPCOREBIASOPT_DEFAULT << 31) /**< Shifted mode DEFAULT for HFXO_XTALCTRL      */

/* Bit fields for HFXO CFG */
#define _HFXO_CFG_RESETVALUE                      0x10000000UL                            /**< Default value for HFXO_CFG                  */
#define _HFXO_CFG_MASK                            0xF000000DUL                            /**< Mask for HFXO_CFG                           */
#define HFXO_CFG_MODE                             (0x1UL << 0)                            /**< Crystal Oscillator Mode                     */
#define _HFXO_CFG_MODE_SHIFT                      0                                       /**< Shift value for HFXO_MODE                   */
#define _HFXO_CFG_MODE_MASK                       0x1UL                                   /**< Bit mask for HFXO_MODE                      */
#define _HFXO_CFG_MODE_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for HFXO_CFG                   */
#define _HFXO_CFG_MODE_XTAL                       0x00000000UL                            /**< Mode XTAL for HFXO_CFG                      */
#define _HFXO_CFG_MODE_EXTCLK                     0x00000001UL                            /**< Mode EXTCLK for HFXO_CFG                    */
#define HFXO_CFG_MODE_DEFAULT                     (_HFXO_CFG_MODE_DEFAULT << 0)           /**< Shifted mode DEFAULT for HFXO_CFG           */
#define HFXO_CFG_MODE_XTAL                        (_HFXO_CFG_MODE_XTAL << 0)              /**< Shifted mode XTAL for HFXO_CFG              */
#define HFXO_CFG_MODE_EXTCLK                      (_HFXO_CFG_MODE_EXTCLK << 0)            /**< Shifted mode EXTCLK for HFXO_CFG            */
#define HFXO_CFG_ENXIDCBIASANA                    (0x1UL << 2)                            /**< Enable XI Internal DC Bias                  */
#define _HFXO_CFG_ENXIDCBIASANA_SHIFT             2                                       /**< Shift value for HFXO_ENXIDCBIASANA          */
#define _HFXO_CFG_ENXIDCBIASANA_MASK              0x4UL                                   /**< Bit mask for HFXO_ENXIDCBIASANA             */
#define _HFXO_CFG_ENXIDCBIASANA_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for HFXO_CFG                   */
#define HFXO_CFG_ENXIDCBIASANA_DEFAULT            (_HFXO_CFG_ENXIDCBIASANA_DEFAULT << 2)  /**< Shifted mode DEFAULT for HFXO_CFG           */
#define HFXO_CFG_SQBUFSCHTRGANA                   (0x1UL << 3)                            /**< Squaring Buffer Schmitt Trigger             */
#define _HFXO_CFG_SQBUFSCHTRGANA_SHIFT            3                                       /**< Shift value for HFXO_SQBUFSCHTRGANA         */
#define _HFXO_CFG_SQBUFSCHTRGANA_MASK             0x8UL                                   /**< Bit mask for HFXO_SQBUFSCHTRGANA            */
#define _HFXO_CFG_SQBUFSCHTRGANA_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for HFXO_CFG                   */
#define _HFXO_CFG_SQBUFSCHTRGANA_DISABLE          0x00000000UL                            /**< Mode DISABLE for HFXO_CFG                   */
#define _HFXO_CFG_SQBUFSCHTRGANA_ENABLE           0x00000001UL                            /**< Mode ENABLE for HFXO_CFG                    */
#define HFXO_CFG_SQBUFSCHTRGANA_DEFAULT           (_HFXO_CFG_SQBUFSCHTRGANA_DEFAULT << 3) /**< Shifted mode DEFAULT for HFXO_CFG           */
#define HFXO_CFG_SQBUFSCHTRGANA_DISABLE           (_HFXO_CFG_SQBUFSCHTRGANA_DISABLE << 3) /**< Shifted mode DISABLE for HFXO_CFG           */
#define HFXO_CFG_SQBUFSCHTRGANA_ENABLE            (_HFXO_CFG_SQBUFSCHTRGANA_ENABLE << 3)  /**< Shifted mode ENABLE for HFXO_CFG            */

/* Bit fields for HFXO CTRL */
#define _HFXO_CTRL_RESETVALUE                     0x00000002UL                             /**< Default value for HFXO_CTRL                 */
#define _HFXO_CTRL_MASK                           0x80000037UL                             /**< Mask for HFXO_CTRL                          */
#define HFXO_CTRL_FORCEEN                         (0x1UL << 0)                             /**< Force Enable                                */
#define _HFXO_CTRL_FORCEEN_SHIFT                  0                                        /**< Shift value for HFXO_FORCEEN                */
#define _HFXO_CTRL_FORCEEN_MASK                   0x1UL                                    /**< Bit mask for HFXO_FORCEEN                   */
#define _HFXO_CTRL_FORCEEN_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_FORCEEN_DEFAULT                 (_HFXO_CTRL_FORCEEN_DEFAULT << 0)        /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_DISONDEMAND                     (0x1UL << 1)                             /**< Disable On-demand Mode                      */
#define _HFXO_CTRL_DISONDEMAND_SHIFT              1                                        /**< Shift value for HFXO_DISONDEMAND            */
#define _HFXO_CTRL_DISONDEMAND_MASK               0x2UL                                    /**< Bit mask for HFXO_DISONDEMAND               */
#define _HFXO_CTRL_DISONDEMAND_DEFAULT            0x00000001UL                             /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_DISONDEMAND_DEFAULT             (_HFXO_CTRL_DISONDEMAND_DEFAULT << 1)    /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_KEEPWARM                        (0x1UL << 2)                             /**< Keep Warm                                   */
#define _HFXO_CTRL_KEEPWARM_SHIFT                 2                                        /**< Shift value for HFXO_KEEPWARM               */
#define _HFXO_CTRL_KEEPWARM_MASK                  0x4UL                                    /**< Bit mask for HFXO_KEEPWARM                  */
#define _HFXO_CTRL_KEEPWARM_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for HFXO_CTRL                  */
#define HFXO_CTRL_KEEPWARM_DEFAULT                (_HFXO_CTRL_KEEPWARM_DEFAULT << 2)       /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXI2GNDANA                  (0x1UL << 4)                             /**< Force XI Pin to Ground                      */
#define _HFXO_CTRL_FORCEXI2GNDANA_SHIFT           4                                        /**< Shift value for HFXO_FORCEXI2GNDANA         */
#define _HFXO_CTRL_FORCEXI2GNDANA_MASK            0x10UL                                   /**< Bit mask for HFXO_FORCEXI2GNDANA            */
#define _HFXO_CTRL_FORCEXI2GNDANA_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXI2GNDANA_DISABLE         0x00000000UL                             /**< Mode DISABLE for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXI2GNDANA_ENABLE          0x00000001UL                             /**< Mode ENABLE for HFXO_CTRL                   */
#define HFXO_CTRL_FORCEXI2GNDANA_DEFAULT          (_HFXO_CTRL_FORCEXI2GNDANA_DEFAULT << 4) /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXI2GNDANA_DISABLE          (_HFXO_CTRL_FORCEXI2GNDANA_DISABLE << 4) /**< Shifted mode DISABLE for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXI2GNDANA_ENABLE           (_HFXO_CTRL_FORCEXI2GNDANA_ENABLE << 4)  /**< Shifted mode ENABLE for HFXO_CTRL           */
#define HFXO_CTRL_FORCEXO2GNDANA                  (0x1UL << 5)                             /**< Force XO Pin to Ground                      */
#define _HFXO_CTRL_FORCEXO2GNDANA_SHIFT           5                                        /**< Shift value for HFXO_FORCEXO2GNDANA         */
#define _HFXO_CTRL_FORCEXO2GNDANA_MASK            0x20UL                                   /**< Bit mask for HFXO_FORCEXO2GNDANA            */
#define _HFXO_CTRL_FORCEXO2GNDANA_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXO2GNDANA_DISABLE         0x00000000UL                             /**< Mode DISABLE for HFXO_CTRL                  */
#define _HFXO_CTRL_FORCEXO2GNDANA_ENABLE          0x00000001UL                             /**< Mode ENABLE for HFXO_CTRL                   */
#define HFXO_CTRL_FORCEXO2GNDANA_DEFAULT          (_HFXO_CTRL_FORCEXO2GNDANA_DEFAULT << 5) /**< Shifted mode DEFAULT for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXO2GNDANA_DISABLE          (_HFXO_CTRL_FORCEXO2GNDANA_DISABLE << 5) /**< Shifted mode DISABLE for HFXO_CTRL          */
#define HFXO_CTRL_FORCEXO2GNDANA_ENABLE           (_HFXO_CTRL_FORCEXO2GNDANA_ENABLE << 5)  /**< Shifted mode ENABLE for HFXO_CTRL           */

/* Bit fields for HFXO CMD */
#define _HFXO_CMD_RESETVALUE                      0x00000000UL                            /**< Default value for HFXO_CMD                  */
#define _HFXO_CMD_MASK                            0x00000003UL                            /**< Mask for HFXO_CMD                           */
#define HFXO_CMD_COREBIASOPT                      (0x1UL << 0)                            /**< Core Bias Optimizaton                       */
#define _HFXO_CMD_COREBIASOPT_SHIFT               0                                       /**< Shift value for HFXO_COREBIASOPT            */
#define _HFXO_CMD_COREBIASOPT_MASK                0x1UL                                   /**< Bit mask for HFXO_COREBIASOPT               */
#define _HFXO_CMD_COREBIASOPT_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for HFXO_CMD                   */
#define HFXO_CMD_COREBIASOPT_DEFAULT              (_HFXO_CMD_COREBIASOPT_DEFAULT << 0)    /**< Shifted mode DEFAULT for HFXO_CMD           */
#define HFXO_CMD_MANUALOVERRIDE                   (0x1UL << 1)                            /**< Manual Override                             */
#define _HFXO_CMD_MANUALOVERRIDE_SHIFT            1                                       /**< Shift value for HFXO_MANUALOVERRIDE         */
#define _HFXO_CMD_MANUALOVERRIDE_MASK             0x2UL                                   /**< Bit mask for HFXO_MANUALOVERRIDE            */
#define _HFXO_CMD_MANUALOVERRIDE_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for HFXO_CMD                   */
#define HFXO_CMD_MANUALOVERRIDE_DEFAULT           (_HFXO_CMD_MANUALOVERRIDE_DEFAULT << 1) /**< Shifted mode DEFAULT for HFXO_CMD           */

/* Bit fields for HFXO STATUS */
#define _HFXO_STATUS_RESETVALUE                   0x00000000UL                               /**< Default value for HFXO_STATUS               */
#define _HFXO_STATUS_MASK                         0xC00F0003UL                               /**< Mask for HFXO_STATUS                        */
#define HFXO_STATUS_RDY                           (0x1UL << 0)                               /**< Ready Status                                */
#define _HFXO_STATUS_RDY_SHIFT                    0                                          /**< Shift value for HFXO_RDY                    */
#define _HFXO_STATUS_RDY_MASK                     0x1UL                                      /**< Bit mask for HFXO_RDY                       */
#define _HFXO_STATUS_RDY_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_RDY_DEFAULT                   (_HFXO_STATUS_RDY_DEFAULT << 0)            /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_COREBIASOPTRDY                (0x1UL << 1)                               /**< Core Bias Optimization Ready                */
#define _HFXO_STATUS_COREBIASOPTRDY_SHIFT         1                                          /**< Shift value for HFXO_COREBIASOPTRDY         */
#define _HFXO_STATUS_COREBIASOPTRDY_MASK          0x2UL                                      /**< Bit mask for HFXO_COREBIASOPTRDY            */
#define _HFXO_STATUS_COREBIASOPTRDY_DEFAULT       0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_COREBIASOPTRDY_DEFAULT        (_HFXO_STATUS_COREBIASOPTRDY_DEFAULT << 1) /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_ENS                           (0x1UL << 16)                              /**< Enabled Status                              */
#define _HFXO_STATUS_ENS_SHIFT                    16                                         /**< Shift value for HFXO_ENS                    */
#define _HFXO_STATUS_ENS_MASK                     0x10000UL                                  /**< Bit mask for HFXO_ENS                       */
#define _HFXO_STATUS_ENS_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_ENS_DEFAULT                   (_HFXO_STATUS_ENS_DEFAULT << 16)           /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_HWREQ                         (0x1UL << 17)                              /**< Oscillator Requested by Hardware            */
#define _HFXO_STATUS_HWREQ_SHIFT                  17                                         /**< Shift value for HFXO_HWREQ                  */
#define _HFXO_STATUS_HWREQ_MASK                   0x20000UL                                  /**< Bit mask for HFXO_HWREQ                     */
#define _HFXO_STATUS_HWREQ_DEFAULT                0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_HWREQ_DEFAULT                 (_HFXO_STATUS_HWREQ_DEFAULT << 17)         /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_ISWARM                        (0x1UL << 19)                              /**< Oscillator Is Kept Warm                     */
#define _HFXO_STATUS_ISWARM_SHIFT                 19                                         /**< Shift value for HFXO_ISWARM                 */
#define _HFXO_STATUS_ISWARM_MASK                  0x80000UL                                  /**< Bit mask for HFXO_ISWARM                    */
#define _HFXO_STATUS_ISWARM_DEFAULT               0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define HFXO_STATUS_ISWARM_DEFAULT                (_HFXO_STATUS_ISWARM_DEFAULT << 19)        /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_FSMLOCK                       (0x1UL << 30)                              /**< FSM Lock Status                             */
#define _HFXO_STATUS_FSMLOCK_SHIFT                30                                         /**< Shift value for HFXO_FSMLOCK                */
#define _HFXO_STATUS_FSMLOCK_MASK                 0x40000000UL                               /**< Bit mask for HFXO_FSMLOCK                   */
#define _HFXO_STATUS_FSMLOCK_DEFAULT              0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define _HFXO_STATUS_FSMLOCK_UNLOCKED             0x00000000UL                               /**< Mode UNLOCKED for HFXO_STATUS               */
#define _HFXO_STATUS_FSMLOCK_LOCKED               0x00000001UL                               /**< Mode LOCKED for HFXO_STATUS                 */
#define HFXO_STATUS_FSMLOCK_DEFAULT               (_HFXO_STATUS_FSMLOCK_DEFAULT << 30)       /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_FSMLOCK_UNLOCKED              (_HFXO_STATUS_FSMLOCK_UNLOCKED << 30)      /**< Shifted mode UNLOCKED for HFXO_STATUS       */
#define HFXO_STATUS_FSMLOCK_LOCKED                (_HFXO_STATUS_FSMLOCK_LOCKED << 30)        /**< Shifted mode LOCKED for HFXO_STATUS         */
#define HFXO_STATUS_LOCK                          (0x1UL << 31)                              /**< Configuration Lock Status                   */
#define _HFXO_STATUS_LOCK_SHIFT                   31                                         /**< Shift value for HFXO_LOCK                   */
#define _HFXO_STATUS_LOCK_MASK                    0x80000000UL                               /**< Bit mask for HFXO_LOCK                      */
#define _HFXO_STATUS_LOCK_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for HFXO_STATUS                */
#define _HFXO_STATUS_LOCK_UNLOCKED                0x00000000UL                               /**< Mode UNLOCKED for HFXO_STATUS               */
#define _HFXO_STATUS_LOCK_LOCKED                  0x00000001UL                               /**< Mode LOCKED for HFXO_STATUS                 */
#define HFXO_STATUS_LOCK_DEFAULT                  (_HFXO_STATUS_LOCK_DEFAULT << 31)          /**< Shifted mode DEFAULT for HFXO_STATUS        */
#define HFXO_STATUS_LOCK_UNLOCKED                 (_HFXO_STATUS_LOCK_UNLOCKED << 31)         /**< Shifted mode UNLOCKED for HFXO_STATUS       */
#define HFXO_STATUS_LOCK_LOCKED                   (_HFXO_STATUS_LOCK_LOCKED << 31)           /**< Shifted mode LOCKED for HFXO_STATUS         */

/* Bit fields for HFXO IF */
#define _HFXO_IF_RESETVALUE                       0x00000000UL                            /**< Default value for HFXO_IF                   */
#define _HFXO_IF_MASK                             0xE0000003UL                            /**< Mask for HFXO_IF                            */
#define HFXO_IF_RDY                               (0x1UL << 0)                            /**< Ready Interrupt                             */
#define _HFXO_IF_RDY_SHIFT                        0                                       /**< Shift value for HFXO_RDY                    */
#define _HFXO_IF_RDY_MASK                         0x1UL                                   /**< Bit mask for HFXO_RDY                       */
#define _HFXO_IF_RDY_DEFAULT                      0x00000000UL                            /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_RDY_DEFAULT                       (_HFXO_IF_RDY_DEFAULT << 0)             /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_COREBIASOPTRDY                    (0x1UL << 1)                            /**< Core Bias Optimization Ready Interrupt      */
#define _HFXO_IF_COREBIASOPTRDY_SHIFT             1                                       /**< Shift value for HFXO_COREBIASOPTRDY         */
#define _HFXO_IF_COREBIASOPTRDY_MASK              0x2UL                                   /**< Bit mask for HFXO_COREBIASOPTRDY            */
#define _HFXO_IF_COREBIASOPTRDY_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_COREBIASOPTRDY_DEFAULT            (_HFXO_IF_COREBIASOPTRDY_DEFAULT << 1)  /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_DNSERR                            (0x1UL << 29)                           /**< Did Not Start Error Interrupt               */
#define _HFXO_IF_DNSERR_SHIFT                     29                                      /**< Shift value for HFXO_DNSERR                 */
#define _HFXO_IF_DNSERR_MASK                      0x20000000UL                            /**< Bit mask for HFXO_DNSERR                    */
#define _HFXO_IF_DNSERR_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_DNSERR_DEFAULT                    (_HFXO_IF_DNSERR_DEFAULT << 29)         /**< Shifted mode DEFAULT for HFXO_IF            */
#define HFXO_IF_COREBIASOPTERR                    (0x1UL << 31)                           /**< Core Bias Optimization Error Interrupt      */
#define _HFXO_IF_COREBIASOPTERR_SHIFT             31                                      /**< Shift value for HFXO_COREBIASOPTERR         */
#define _HFXO_IF_COREBIASOPTERR_MASK              0x80000000UL                            /**< Bit mask for HFXO_COREBIASOPTERR            */
#define _HFXO_IF_COREBIASOPTERR_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for HFXO_IF                    */
#define HFXO_IF_COREBIASOPTERR_DEFAULT            (_HFXO_IF_COREBIASOPTERR_DEFAULT << 31) /**< Shifted mode DEFAULT for HFXO_IF            */

/* Bit fields for HFXO IEN */
#define _HFXO_IEN_RESETVALUE                      0x00000000UL                             /**< Default value for HFXO_IEN                  */
#define _HFXO_IEN_MASK                            0xE0000003UL                             /**< Mask for HFXO_IEN                           */
#define HFXO_IEN_RDY                              (0x1UL << 0)                             /**< Ready Interrupt                             */
#define _HFXO_IEN_RDY_SHIFT                       0                                        /**< Shift value for HFXO_RDY                    */
#define _HFXO_IEN_RDY_MASK                        0x1UL                                    /**< Bit mask for HFXO_RDY                       */
#define _HFXO_IEN_RDY_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_RDY_DEFAULT                      (_HFXO_IEN_RDY_DEFAULT << 0)             /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_COREBIASOPTRDY                   (0x1UL << 1)                             /**< Core Bias Optimization Ready Interrupt      */
#define _HFXO_IEN_COREBIASOPTRDY_SHIFT            1                                        /**< Shift value for HFXO_COREBIASOPTRDY         */
#define _HFXO_IEN_COREBIASOPTRDY_MASK             0x2UL                                    /**< Bit mask for HFXO_COREBIASOPTRDY            */
#define _HFXO_IEN_COREBIASOPTRDY_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_COREBIASOPTRDY_DEFAULT           (_HFXO_IEN_COREBIASOPTRDY_DEFAULT << 1)  /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_DNSERR                           (0x1UL << 29)                            /**< Did Not Start Error Interrupt               */
#define _HFXO_IEN_DNSERR_SHIFT                    29                                       /**< Shift value for HFXO_DNSERR                 */
#define _HFXO_IEN_DNSERR_MASK                     0x20000000UL                             /**< Bit mask for HFXO_DNSERR                    */
#define _HFXO_IEN_DNSERR_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_DNSERR_DEFAULT                   (_HFXO_IEN_DNSERR_DEFAULT << 29)         /**< Shifted mode DEFAULT for HFXO_IEN           */
#define HFXO_IEN_COREBIASOPTERR                   (0x1UL << 31)                            /**< Core Bias Optimization Error Interrupt      */
#define _HFXO_IEN_COREBIASOPTERR_SHIFT            31                                       /**< Shift value for HFXO_COREBIASOPTERR         */
#define _HFXO_IEN_COREBIASOPTERR_MASK             0x80000000UL                             /**< Bit mask for HFXO_COREBIASOPTERR            */
#define _HFXO_IEN_COREBIASOPTERR_DEFAULT          0x00000000UL                             /**< Mode DEFAULT for HFXO_IEN                   */
#define HFXO_IEN_COREBIASOPTERR_DEFAULT           (_HFXO_IEN_COREBIASOPTERR_DEFAULT << 31) /**< Shifted mode DEFAULT for HFXO_IEN           */

/* Bit fields for HFXO LOCK */
#define _HFXO_LOCK_RESETVALUE                     0x0000580EUL                          /**< Default value for HFXO_LOCK                 */
#define _HFXO_LOCK_MASK                           0x0000FFFFUL                          /**< Mask for HFXO_LOCK                          */
#define _HFXO_LOCK_LOCKKEY_SHIFT                  0                                     /**< Shift value for HFXO_LOCKKEY                */
#define _HFXO_LOCK_LOCKKEY_MASK                   0xFFFFUL                              /**< Bit mask for HFXO_LOCKKEY                   */
#define _HFXO_LOCK_LOCKKEY_DEFAULT                0x0000580EUL                          /**< Mode DEFAULT for HFXO_LOCK                  */
#define _HFXO_LOCK_LOCKKEY_UNLOCK                 0x0000580EUL                          /**< Mode UNLOCK for HFXO_LOCK                   */
#define HFXO_LOCK_LOCKKEY_DEFAULT                 (_HFXO_LOCK_LOCKKEY_DEFAULT << 0)     /**< Shifted mode DEFAULT for HFXO_LOCK          */
#define HFXO_LOCK_LOCKKEY_UNLOCK                  (_HFXO_LOCK_LOCKKEY_UNLOCK << 0)      /**< Shifted mode UNLOCK for HFXO_LOCK           */

/** @} End of group EFR32MG22_HFXO_BitFields */
/** @} End of group EFR32MG22_HFXO */
/** @} End of group Parts */
#endif /* EFR32MG22_HFXO_H */
