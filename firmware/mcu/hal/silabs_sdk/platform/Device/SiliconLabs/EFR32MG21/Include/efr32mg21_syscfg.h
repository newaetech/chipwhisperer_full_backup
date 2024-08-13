/**************************************************************************//**
 * @file
 * @brief EFR32MG21 SYSCFG register and bit field definitions
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
#ifndef EFR32MG21_SYSCFG_H
#define EFR32MG21_SYSCFG_H
#define SYSCFG_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_SYSCFG SYSCFG
 * @{
 * @brief EFR32MG21 SYSCFG Register Declaration.
 *****************************************************************************/

/** SYSCFG Register Declaration. */
typedef struct {
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Enable Register                          */
  uint32_t       RESERVED0[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW;                     /**< Hardwired Chip Rev values                          */
  __IOM uint32_t CHIPREV;                       /**< Part Family and Revision values                    */
  uint32_t       RESERVED1[58U];                /**< Reserved for future use                            */
  uint32_t       RESERVED2[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED3[63U];                /**< Reserved for future use                            */
  __IOM uint32_t CTRL;                          /**< Memory System Control Register                     */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL;                 /**< DMEM retention Control Register                    */
  uint32_t       RESERVED5[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR;                  /**< DMEM ECC Error Address Register                    */
  __IOM uint32_t DMEM0ECCCTRL;                  /**< DMEM ECC Control Register                          */
  __IOM uint32_t DMEM0RAMCTRL;                  /**< DMEM Control enable Register                       */
  uint32_t       RESERVED6[121U];               /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL;              /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED7[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL;                  /**< RADIO RAM ECC Control Register                     */
  __IOM uint32_t RADIORAMCTRL;                  /**< RADIO RAM Control Register                         */
  __IM uint32_t  SEQRAMECCADDR;                 /**< SEQRAM ECC Error Address Register                  */
  __IM uint32_t  FRCRAMECCADDR;                 /**< FRCRAM ECC Error Address Register                  */
  uint32_t       RESERVED8[762U];               /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED9[2U];                 /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_SET;                 /**< Hardwired Chip Rev values                          */
  __IOM uint32_t CHIPREV_SET;                   /**< Part Family and Revision values                    */
  uint32_t       RESERVED10[58U];               /**< Reserved for future use                            */
  uint32_t       RESERVED11[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED12[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_SET;                      /**< Memory System Control Register                     */
  uint32_t       RESERVED13[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_SET;             /**< DMEM retention Control Register                    */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR_SET;              /**< DMEM ECC Error Address Register                    */
  __IOM uint32_t DMEM0ECCCTRL_SET;              /**< DMEM ECC Control Register                          */
  __IOM uint32_t DMEM0RAMCTRL_SET;              /**< DMEM Control enable Register                       */
  uint32_t       RESERVED15[121U];              /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL_SET;          /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED16[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL_SET;              /**< RADIO RAM ECC Control Register                     */
  __IOM uint32_t RADIORAMCTRL_SET;              /**< RADIO RAM Control Register                         */
  __IM uint32_t  SEQRAMECCADDR_SET;             /**< SEQRAM ECC Error Address Register                  */
  __IM uint32_t  FRCRAMECCADDR_SET;             /**< FRCRAM ECC Error Address Register                  */
  uint32_t       RESERVED17[762U];              /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED18[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_CLR;                 /**< Hardwired Chip Rev values                          */
  __IOM uint32_t CHIPREV_CLR;                   /**< Part Family and Revision values                    */
  uint32_t       RESERVED19[58U];               /**< Reserved for future use                            */
  uint32_t       RESERVED20[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED21[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_CLR;                      /**< Memory System Control Register                     */
  uint32_t       RESERVED22[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_CLR;             /**< DMEM retention Control Register                    */
  uint32_t       RESERVED23[1U];                /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR_CLR;              /**< DMEM ECC Error Address Register                    */
  __IOM uint32_t DMEM0ECCCTRL_CLR;              /**< DMEM ECC Control Register                          */
  __IOM uint32_t DMEM0RAMCTRL_CLR;              /**< DMEM Control enable Register                       */
  uint32_t       RESERVED24[121U];              /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL_CLR;          /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED25[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL_CLR;              /**< RADIO RAM ECC Control Register                     */
  __IOM uint32_t RADIORAMCTRL_CLR;              /**< RADIO RAM Control Register                         */
  __IM uint32_t  SEQRAMECCADDR_CLR;             /**< SEQRAM ECC Error Address Register                  */
  __IM uint32_t  FRCRAMECCADDR_CLR;             /**< FRCRAM ECC Error Address Register                  */
  uint32_t       RESERVED26[762U];              /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable Register                          */
  uint32_t       RESERVED27[2U];                /**< Reserved for future use                            */
  __IOM uint32_t CHIPREVHW_TGL;                 /**< Hardwired Chip Rev values                          */
  __IOM uint32_t CHIPREV_TGL;                   /**< Part Family and Revision values                    */
  uint32_t       RESERVED28[58U];               /**< Reserved for future use                            */
  uint32_t       RESERVED29[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED30[63U];               /**< Reserved for future use                            */
  __IOM uint32_t CTRL_TGL;                      /**< Memory System Control Register                     */
  uint32_t       RESERVED31[1U];                /**< Reserved for future use                            */
  __IOM uint32_t DMEM0RETNCTRL_TGL;             /**< DMEM retention Control Register                    */
  uint32_t       RESERVED32[1U];                /**< Reserved for future use                            */
  __IM uint32_t  DMEM0ECCADDR_TGL;              /**< DMEM ECC Error Address Register                    */
  __IOM uint32_t DMEM0ECCCTRL_TGL;              /**< DMEM ECC Control Register                          */
  __IOM uint32_t DMEM0RAMCTRL_TGL;              /**< DMEM Control enable Register                       */
  uint32_t       RESERVED33[121U];              /**< Reserved for future use                            */
  __IOM uint32_t RADIORAMRETNCTRL_TGL;          /**< RADIO RAM Retention Control Register               */
  uint32_t       RESERVED34[1U];                /**< Reserved for future use                            */
  __IOM uint32_t RADIOECCCTRL_TGL;              /**< RADIO RAM ECC Control Register                     */
  __IOM uint32_t RADIORAMCTRL_TGL;              /**< RADIO RAM Control Register                         */
  __IM uint32_t  SEQRAMECCADDR_TGL;             /**< SEQRAM ECC Error Address Register                  */
  __IM uint32_t  FRCRAMECCADDR_TGL;             /**< FRCRAM ECC Error Address Register                  */
} SYSCFG_TypeDef;
/** @} End of group EFR32MG21_SYSCFG */

/**************************************************************************//**
 * @addtogroup EFR32MG21_SYSCFG
 * @{
 * @defgroup EFR32MG21_SYSCFG_BitFields SYSCFG Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SYSCFG IF */
#define _SYSCFG_IF_RESETVALUE                              0x00000000UL                             /**< Default value for SYSCFG_IF                 */
#define _SYSCFG_IF_MASK                                    0x3303000FUL                             /**< Mask for SYSCFG_IF                          */
#define SYSCFG_IF_SW0                                      (0x1UL << 0)                             /**< Software Interrupt 0                        */
#define _SYSCFG_IF_SW0_SHIFT                               0                                        /**< Shift value for SYSCFG_SW0                  */
#define _SYSCFG_IF_SW0_MASK                                0x1UL                                    /**< Bit mask for SYSCFG_SW0                     */
#define _SYSCFG_IF_SW0_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW0_DEFAULT                              (_SYSCFG_IF_SW0_DEFAULT << 0)            /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW1                                      (0x1UL << 1)                             /**< Software Interrupt 1                        */
#define _SYSCFG_IF_SW1_SHIFT                               1                                        /**< Shift value for SYSCFG_SW1                  */
#define _SYSCFG_IF_SW1_MASK                                0x2UL                                    /**< Bit mask for SYSCFG_SW1                     */
#define _SYSCFG_IF_SW1_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW1_DEFAULT                              (_SYSCFG_IF_SW1_DEFAULT << 1)            /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW2                                      (0x1UL << 2)                             /**< Software Interrupt 2                        */
#define _SYSCFG_IF_SW2_SHIFT                               2                                        /**< Shift value for SYSCFG_SW2                  */
#define _SYSCFG_IF_SW2_MASK                                0x4UL                                    /**< Bit mask for SYSCFG_SW2                     */
#define _SYSCFG_IF_SW2_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW2_DEFAULT                              (_SYSCFG_IF_SW2_DEFAULT << 2)            /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SW3                                      (0x1UL << 3)                             /**< Software Interrupt 3                        */
#define _SYSCFG_IF_SW3_SHIFT                               3                                        /**< Shift value for SYSCFG_SW3                  */
#define _SYSCFG_IF_SW3_MASK                                0x8UL                                    /**< Bit mask for SYSCFG_SW3                     */
#define _SYSCFG_IF_SW3_DEFAULT                             0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SW3_DEFAULT                              (_SYSCFG_IF_SW3_DEFAULT << 3)            /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_RAMERR1B                                 (0x1UL << 16)                            /**< RAM 1-bit ECC Error Interrupt flag          */
#define _SYSCFG_IF_RAMERR1B_SHIFT                          16                                       /**< Shift value for SYSCFG_RAMERR1B             */
#define _SYSCFG_IF_RAMERR1B_MASK                           0x10000UL                                /**< Bit mask for SYSCFG_RAMERR1B                */
#define _SYSCFG_IF_RAMERR1B_DEFAULT                        0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_RAMERR1B_DEFAULT                         (_SYSCFG_IF_RAMERR1B_DEFAULT << 16)      /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_RAMERR2B                                 (0x1UL << 17)                            /**< RAM 2-bit ECC Error Interrupt flag          */
#define _SYSCFG_IF_RAMERR2B_SHIFT                          17                                       /**< Shift value for SYSCFG_RAMERR2B             */
#define _SYSCFG_IF_RAMERR2B_MASK                           0x20000UL                                /**< Bit mask for SYSCFG_RAMERR2B                */
#define _SYSCFG_IF_RAMERR2B_DEFAULT                        0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_RAMERR2B_DEFAULT                         (_SYSCFG_IF_RAMERR2B_DEFAULT << 17)      /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SEQRAMERR1B                              (0x1UL << 24)                            /**< SEQRAM 1-bit ECC Error Interrupt flag       */
#define _SYSCFG_IF_SEQRAMERR1B_SHIFT                       24                                       /**< Shift value for SYSCFG_SEQRAMERR1B          */
#define _SYSCFG_IF_SEQRAMERR1B_MASK                        0x1000000UL                              /**< Bit mask for SYSCFG_SEQRAMERR1B             */
#define _SYSCFG_IF_SEQRAMERR1B_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SEQRAMERR1B_DEFAULT                      (_SYSCFG_IF_SEQRAMERR1B_DEFAULT << 24)   /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_SEQRAMERR2B                              (0x1UL << 25)                            /**< SEQRAM 2-bit ECC Error Interrupt flag       */
#define _SYSCFG_IF_SEQRAMERR2B_SHIFT                       25                                       /**< Shift value for SYSCFG_SEQRAMERR2B          */
#define _SYSCFG_IF_SEQRAMERR2B_MASK                        0x2000000UL                              /**< Bit mask for SYSCFG_SEQRAMERR2B             */
#define _SYSCFG_IF_SEQRAMERR2B_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_SEQRAMERR2B_DEFAULT                      (_SYSCFG_IF_SEQRAMERR2B_DEFAULT << 25)   /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FRCRAMERR1BIF                            (0x1UL << 28)                            /**< FRCRAM 1-bit ECC Error Interrupt flag       */
#define _SYSCFG_IF_FRCRAMERR1BIF_SHIFT                     28                                       /**< Shift value for SYSCFG_FRCRAMERR1BIF        */
#define _SYSCFG_IF_FRCRAMERR1BIF_MASK                      0x10000000UL                             /**< Bit mask for SYSCFG_FRCRAMERR1BIF           */
#define _SYSCFG_IF_FRCRAMERR1BIF_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FRCRAMERR1BIF_DEFAULT                    (_SYSCFG_IF_FRCRAMERR1BIF_DEFAULT << 28) /**< Shifted mode DEFAULT for SYSCFG_IF          */
#define SYSCFG_IF_FRCRAMERR2BIF                            (0x1UL << 29)                            /**< FRCRAM 2-bit ECC Error Interrupt flag       */
#define _SYSCFG_IF_FRCRAMERR2BIF_SHIFT                     29                                       /**< Shift value for SYSCFG_FRCRAMERR2BIF        */
#define _SYSCFG_IF_FRCRAMERR2BIF_MASK                      0x20000000UL                             /**< Bit mask for SYSCFG_FRCRAMERR2BIF           */
#define _SYSCFG_IF_FRCRAMERR2BIF_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for SYSCFG_IF                  */
#define SYSCFG_IF_FRCRAMERR2BIF_DEFAULT                    (_SYSCFG_IF_FRCRAMERR2BIF_DEFAULT << 29) /**< Shifted mode DEFAULT for SYSCFG_IF          */

/* Bit fields for SYSCFG IEN */
#define _SYSCFG_IEN_RESETVALUE                             0x00000000UL                               /**< Default value for SYSCFG_IEN                */
#define _SYSCFG_IEN_MASK                                   0x3303000FUL                               /**< Mask for SYSCFG_IEN                         */
#define SYSCFG_IEN_SW0                                     (0x1UL << 0)                               /**< Software interrupt 0                        */
#define _SYSCFG_IEN_SW0_SHIFT                              0                                          /**< Shift value for SYSCFG_SW0                  */
#define _SYSCFG_IEN_SW0_MASK                               0x1UL                                      /**< Bit mask for SYSCFG_SW0                     */
#define _SYSCFG_IEN_SW0_DEFAULT                            0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW0_DEFAULT                             (_SYSCFG_IEN_SW0_DEFAULT << 0)             /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW1                                     (0x1UL << 1)                               /**< Software interrupt 1                        */
#define _SYSCFG_IEN_SW1_SHIFT                              1                                          /**< Shift value for SYSCFG_SW1                  */
#define _SYSCFG_IEN_SW1_MASK                               0x2UL                                      /**< Bit mask for SYSCFG_SW1                     */
#define _SYSCFG_IEN_SW1_DEFAULT                            0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW1_DEFAULT                             (_SYSCFG_IEN_SW1_DEFAULT << 1)             /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW2                                     (0x1UL << 2)                               /**< Software interrupt 2                        */
#define _SYSCFG_IEN_SW2_SHIFT                              2                                          /**< Shift value for SYSCFG_SW2                  */
#define _SYSCFG_IEN_SW2_MASK                               0x4UL                                      /**< Bit mask for SYSCFG_SW2                     */
#define _SYSCFG_IEN_SW2_DEFAULT                            0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW2_DEFAULT                             (_SYSCFG_IEN_SW2_DEFAULT << 2)             /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SW3                                     (0x1UL << 3)                               /**< Software interrupt 3                        */
#define _SYSCFG_IEN_SW3_SHIFT                              3                                          /**< Shift value for SYSCFG_SW3                  */
#define _SYSCFG_IEN_SW3_MASK                               0x8UL                                      /**< Bit mask for SYSCFG_SW3                     */
#define _SYSCFG_IEN_SW3_DEFAULT                            0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SW3_DEFAULT                             (_SYSCFG_IEN_SW3_DEFAULT << 3)             /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_RAMERR1B                                (0x1UL << 16)                              /**< RAM 1-bit ECC Error Interrupt enable        */
#define _SYSCFG_IEN_RAMERR1B_SHIFT                         16                                         /**< Shift value for SYSCFG_RAMERR1B             */
#define _SYSCFG_IEN_RAMERR1B_MASK                          0x10000UL                                  /**< Bit mask for SYSCFG_RAMERR1B                */
#define _SYSCFG_IEN_RAMERR1B_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_RAMERR1B_DEFAULT                        (_SYSCFG_IEN_RAMERR1B_DEFAULT << 16)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_RAMERR2B                                (0x1UL << 17)                              /**< RAM 2-bit ECC Error Interrupt enable        */
#define _SYSCFG_IEN_RAMERR2B_SHIFT                         17                                         /**< Shift value for SYSCFG_RAMERR2B             */
#define _SYSCFG_IEN_RAMERR2B_MASK                          0x20000UL                                  /**< Bit mask for SYSCFG_RAMERR2B                */
#define _SYSCFG_IEN_RAMERR2B_DEFAULT                       0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_RAMERR2B_DEFAULT                        (_SYSCFG_IEN_RAMERR2B_DEFAULT << 17)       /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SEQRAMERR1B                             (0x1UL << 24)                              /**< SEQRAM 1-bit ECC Error Interrupt enable     */
#define _SYSCFG_IEN_SEQRAMERR1B_SHIFT                      24                                         /**< Shift value for SYSCFG_SEQRAMERR1B          */
#define _SYSCFG_IEN_SEQRAMERR1B_MASK                       0x1000000UL                                /**< Bit mask for SYSCFG_SEQRAMERR1B             */
#define _SYSCFG_IEN_SEQRAMERR1B_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SEQRAMERR1B_DEFAULT                     (_SYSCFG_IEN_SEQRAMERR1B_DEFAULT << 24)    /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_SEQRAMERR2B                             (0x1UL << 25)                              /**< SEQRAM 2-bit ECC Error Interrupt enable     */
#define _SYSCFG_IEN_SEQRAMERR2B_SHIFT                      25                                         /**< Shift value for SYSCFG_SEQRAMERR2B          */
#define _SYSCFG_IEN_SEQRAMERR2B_MASK                       0x2000000UL                                /**< Bit mask for SYSCFG_SEQRAMERR2B             */
#define _SYSCFG_IEN_SEQRAMERR2B_DEFAULT                    0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_SEQRAMERR2B_DEFAULT                     (_SYSCFG_IEN_SEQRAMERR2B_DEFAULT << 25)    /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FRCRAMERR1BIEN                          (0x1UL << 28)                              /**< FRCRAM 1-bit ECC Error Interrupt enable     */
#define _SYSCFG_IEN_FRCRAMERR1BIEN_SHIFT                   28                                         /**< Shift value for SYSCFG_FRCRAMERR1BIEN       */
#define _SYSCFG_IEN_FRCRAMERR1BIEN_MASK                    0x10000000UL                               /**< Bit mask for SYSCFG_FRCRAMERR1BIEN          */
#define _SYSCFG_IEN_FRCRAMERR1BIEN_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FRCRAMERR1BIEN_DEFAULT                  (_SYSCFG_IEN_FRCRAMERR1BIEN_DEFAULT << 28) /**< Shifted mode DEFAULT for SYSCFG_IEN         */
#define SYSCFG_IEN_FRCRAMERR2BIEN                          (0x1UL << 29)                              /**< FRCRAM 2-bit ECC Error Interrupt enable     */
#define _SYSCFG_IEN_FRCRAMERR2BIEN_SHIFT                   29                                         /**< Shift value for SYSCFG_FRCRAMERR2BIEN       */
#define _SYSCFG_IEN_FRCRAMERR2BIEN_MASK                    0x20000000UL                               /**< Bit mask for SYSCFG_FRCRAMERR2BIEN          */
#define _SYSCFG_IEN_FRCRAMERR2BIEN_DEFAULT                 0x00000000UL                               /**< Mode DEFAULT for SYSCFG_IEN                 */
#define SYSCFG_IEN_FRCRAMERR2BIEN_DEFAULT                  (_SYSCFG_IEN_FRCRAMERR2BIEN_DEFAULT << 29) /**< Shifted mode DEFAULT for SYSCFG_IEN         */

/* Bit fields for SYSCFG CHIPREVHW */
#define _SYSCFG_CHIPREVHW_RESETVALUE                       0x00000000UL                            /**< Default value for SYSCFG_CHIPREVHW          */
#define _SYSCFG_CHIPREVHW_MASK                             0xFF0FFFFFUL                            /**< Mask for SYSCFG_CHIPREVHW                   */
#define _SYSCFG_CHIPREVHW_MAJOR_SHIFT                      0                                       /**< Shift value for SYSCFG_MAJOR                */
#define _SYSCFG_CHIPREVHW_MAJOR_MASK                       0x3FUL                                  /**< Bit mask for SYSCFG_MAJOR                   */
#define _SYSCFG_CHIPREVHW_MAJOR_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_MAJOR_DEFAULT                     (_SYSCFG_CHIPREVHW_MAJOR_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */
#define _SYSCFG_CHIPREVHW_FAMILY_SHIFT                     6                                       /**< Shift value for SYSCFG_FAMILY               */
#define _SYSCFG_CHIPREVHW_FAMILY_MASK                      0xFC0UL                                 /**< Bit mask for SYSCFG_FAMILY                  */
#define _SYSCFG_CHIPREVHW_FAMILY_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_FAMILY_DEFAULT                    (_SYSCFG_CHIPREVHW_FAMILY_DEFAULT << 6) /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */
#define _SYSCFG_CHIPREVHW_MINOR_SHIFT                      12                                      /**< Shift value for SYSCFG_MINOR                */
#define _SYSCFG_CHIPREVHW_MINOR_MASK                       0xFF000UL                               /**< Bit mask for SYSCFG_MINOR                   */
#define _SYSCFG_CHIPREVHW_MINOR_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for SYSCFG_CHIPREVHW           */
#define SYSCFG_CHIPREVHW_MINOR_DEFAULT                     (_SYSCFG_CHIPREVHW_MINOR_DEFAULT << 12) /**< Shifted mode DEFAULT for SYSCFG_CHIPREVHW   */

/* Bit fields for SYSCFG CHIPREV */
#define _SYSCFG_CHIPREV_RESETVALUE                         0x00000000UL                          /**< Default value for SYSCFG_CHIPREV            */
#define _SYSCFG_CHIPREV_MASK                               0x000FFFFFUL                          /**< Mask for SYSCFG_CHIPREV                     */
#define _SYSCFG_CHIPREV_MAJOR_SHIFT                        0                                     /**< Shift value for SYSCFG_MAJOR                */
#define _SYSCFG_CHIPREV_MAJOR_MASK                         0x3FUL                                /**< Bit mask for SYSCFG_MAJOR                   */
#define _SYSCFG_CHIPREV_MAJOR_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_MAJOR_DEFAULT                       (_SYSCFG_CHIPREV_MAJOR_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */
#define _SYSCFG_CHIPREV_FAMILY_SHIFT                       6                                     /**< Shift value for SYSCFG_FAMILY               */
#define _SYSCFG_CHIPREV_FAMILY_MASK                        0xFC0UL                               /**< Bit mask for SYSCFG_FAMILY                  */
#define _SYSCFG_CHIPREV_FAMILY_DEFAULT                     0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_FAMILY_DEFAULT                      (_SYSCFG_CHIPREV_FAMILY_DEFAULT << 6) /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */
#define _SYSCFG_CHIPREV_MINOR_SHIFT                        12                                    /**< Shift value for SYSCFG_MINOR                */
#define _SYSCFG_CHIPREV_MINOR_MASK                         0xFF000UL                             /**< Bit mask for SYSCFG_MINOR                   */
#define _SYSCFG_CHIPREV_MINOR_DEFAULT                      0x00000000UL                          /**< Mode DEFAULT for SYSCFG_CHIPREV             */
#define SYSCFG_CHIPREV_MINOR_DEFAULT                       (_SYSCFG_CHIPREV_MINOR_DEFAULT << 12) /**< Shifted mode DEFAULT for SYSCFG_CHIPREV     */

/* Bit fields for SYSCFG CTRL */
#define _SYSCFG_CTRL_RESETVALUE                            0x00000021UL                                 /**< Default value for SYSCFG_CTRL               */
#define _SYSCFG_CTRL_MASK                                  0x00000021UL                                 /**< Mask for SYSCFG_CTRL                        */
#define SYSCFG_CTRL_ADDRFAULTEN                            (0x1UL << 0)                                 /**< Invalid Address Bus Fault Response Enable   */
#define _SYSCFG_CTRL_ADDRFAULTEN_SHIFT                     0                                            /**< Shift value for SYSCFG_ADDRFAULTEN          */
#define _SYSCFG_CTRL_ADDRFAULTEN_MASK                      0x1UL                                        /**< Bit mask for SYSCFG_ADDRFAULTEN             */
#define _SYSCFG_CTRL_ADDRFAULTEN_DEFAULT                   0x00000001UL                                 /**< Mode DEFAULT for SYSCFG_CTRL                */
#define SYSCFG_CTRL_ADDRFAULTEN_DEFAULT                    (_SYSCFG_CTRL_ADDRFAULTEN_DEFAULT << 0)      /**< Shifted mode DEFAULT for SYSCFG_CTRL        */
#define SYSCFG_CTRL_RAMECCERRFAULTEN                       (0x1UL << 5)                                 /**< Two bit ECC Error Bus Fault Response Enable */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_SHIFT                5                                            /**< Shift value for SYSCFG_RAMECCERRFAULTEN     */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_MASK                 0x20UL                                       /**< Bit mask for SYSCFG_RAMECCERRFAULTEN        */
#define _SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT              0x00000001UL                                 /**< Mode DEFAULT for SYSCFG_CTRL                */
#define SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT               (_SYSCFG_CTRL_RAMECCERRFAULTEN_DEFAULT << 5) /**< Shifted mode DEFAULT for SYSCFG_CTRL        */

/* Bit fields for SYSCFG DMEM0RETNCTRL */
#define _SYSCFG_DMEM0RETNCTRL_RESETVALUE                   0x00000000UL                                     /**< Default value for SYSCFG_DMEM0RETNCTRL      */
#define _SYSCFG_DMEM0RETNCTRL_MASK                         0x0000001FUL                                     /**< Mask for SYSCFG_DMEM0RETNCTRL               */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_SHIFT            0                                                /**< Shift value for SYSCFG_RAMRETNCTRL          */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_MASK             0x1FUL                                           /**< Bit mask for SYSCFG_RAMRETNCTRL             */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SYSCFG_DMEM0RETNCTRL       */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON            0x00000000UL                                     /**< Mode ALLON for SYSCFG_DMEM0RETNCTRL         */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK5             0x00000010UL                                     /**< Mode BLK5 for SYSCFG_DMEM0RETNCTRL          */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK4TO5          0x00000018UL                                     /**< Mode BLK4TO5 for SYSCFG_DMEM0RETNCTRL       */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK3TO5          0x0000001CUL                                     /**< Mode BLK3TO5 for SYSCFG_DMEM0RETNCTRL       */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK2TO5          0x0000001EUL                                     /**< Mode BLK2TO5 for SYSCFG_DMEM0RETNCTRL       */
#define _SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1TO5          0x0000001FUL                                     /**< Mode BLK1TO5 for SYSCFG_DMEM0RETNCTRL       */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_DMEM0RETNCTRL*/
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON             (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_ALLON << 0)   /**< Shifted mode ALLON for SYSCFG_DMEM0RETNCTRL */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK5              (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK5 << 0)    /**< Shifted mode BLK5 for SYSCFG_DMEM0RETNCTRL  */
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK4TO5           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK4TO5 << 0) /**< Shifted mode BLK4TO5 for SYSCFG_DMEM0RETNCTRL*/
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK3TO5           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK3TO5 << 0) /**< Shifted mode BLK3TO5 for SYSCFG_DMEM0RETNCTRL*/
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK2TO5           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK2TO5 << 0) /**< Shifted mode BLK2TO5 for SYSCFG_DMEM0RETNCTRL*/
#define SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1TO5           (_SYSCFG_DMEM0RETNCTRL_RAMRETNCTRL_BLK1TO5 << 0) /**< Shifted mode BLK1TO5 for SYSCFG_DMEM0RETNCTRL*/

/* Bit fields for SYSCFG DMEM0ECCADDR */
#define _SYSCFG_DMEM0ECCADDR_RESETVALUE                    0x00000000UL                                     /**< Default value for SYSCFG_DMEM0ECCADDR       */
#define _SYSCFG_DMEM0ECCADDR_MASK                          0xFFFFFFFFUL                                     /**< Mask for SYSCFG_DMEM0ECCADDR                */
#define _SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_SHIFT            0                                                /**< Shift value for SYSCFG_DMEM0ECCADDR         */
#define _SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_MASK             0xFFFFFFFFUL                                     /**< Bit mask for SYSCFG_DMEM0ECCADDR            */
#define _SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_DEFAULT          0x00000000UL                                     /**< Mode DEFAULT for SYSCFG_DMEM0ECCADDR        */
#define SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_DEFAULT           (_SYSCFG_DMEM0ECCADDR_DMEM0ECCADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_DMEM0ECCADDR*/

/* Bit fields for SYSCFG DMEM0ECCCTRL */
#define _SYSCFG_DMEM0ECCCTRL_RESETVALUE                    0x00000000UL                                    /**< Default value for SYSCFG_DMEM0ECCCTRL       */
#define _SYSCFG_DMEM0ECCCTRL_MASK                          0x00000003UL                                    /**< Mask for SYSCFG_DMEM0ECCCTRL                */
#define SYSCFG_DMEM0ECCCTRL_RAMECCEWEN                     (0x1UL << 0)                                    /**< RAM ECC Write Enable                        */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_SHIFT              0                                               /**< Shift value for SYSCFG_RAMECCEWEN           */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_MASK               0x1UL                                           /**< Bit mask for SYSCFG_RAMECCEWEN              */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_DEFAULT            0x00000000UL                                    /**< Mode DEFAULT for SYSCFG_DMEM0ECCCTRL        */
#define SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_DEFAULT             (_SYSCFG_DMEM0ECCCTRL_RAMECCEWEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_DMEM0ECCCTRL*/
#define SYSCFG_DMEM0ECCCTRL_RAMECCCHKEN                    (0x1UL << 1)                                    /**< RAM ECC Check Enable                        */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCCHKEN_SHIFT             1                                               /**< Shift value for SYSCFG_RAMECCCHKEN          */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCCHKEN_MASK              0x2UL                                           /**< Bit mask for SYSCFG_RAMECCCHKEN             */
#define _SYSCFG_DMEM0ECCCTRL_RAMECCCHKEN_DEFAULT           0x00000000UL                                    /**< Mode DEFAULT for SYSCFG_DMEM0ECCCTRL        */
#define SYSCFG_DMEM0ECCCTRL_RAMECCCHKEN_DEFAULT            (_SYSCFG_DMEM0ECCCTRL_RAMECCCHKEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SYSCFG_DMEM0ECCCTRL*/

/* Bit fields for SYSCFG DMEM0RAMCTRL */
#define _SYSCFG_DMEM0RAMCTRL_RESETVALUE                    0x00000000UL                                      /**< Default value for SYSCFG_DMEM0RAMCTRL       */
#define _SYSCFG_DMEM0RAMCTRL_MASK                          0x00000007UL                                      /**< Mask for SYSCFG_DMEM0RAMCTRL                */
#define SYSCFG_DMEM0RAMCTRL_RAMCACHEEN                     (0x1UL << 0)                                      /**< RAM CACHE Enable                            */
#define _SYSCFG_DMEM0RAMCTRL_RAMCACHEEN_SHIFT              0                                                 /**< Shift value for SYSCFG_RAMCACHEEN           */
#define _SYSCFG_DMEM0RAMCTRL_RAMCACHEEN_MASK               0x1UL                                             /**< Bit mask for SYSCFG_RAMCACHEEN              */
#define _SYSCFG_DMEM0RAMCTRL_RAMCACHEEN_DEFAULT            0x00000000UL                                      /**< Mode DEFAULT for SYSCFG_DMEM0RAMCTRL        */
#define SYSCFG_DMEM0RAMCTRL_RAMCACHEEN_DEFAULT             (_SYSCFG_DMEM0RAMCTRL_RAMCACHEEN_DEFAULT << 0)    /**< Shifted mode DEFAULT for SYSCFG_DMEM0RAMCTRL*/
#define SYSCFG_DMEM0RAMCTRL_RAMWSEN                        (0x1UL << 1)                                      /**< RAM WAIT STATE Enable                       */
#define _SYSCFG_DMEM0RAMCTRL_RAMWSEN_SHIFT                 1                                                 /**< Shift value for SYSCFG_RAMWSEN              */
#define _SYSCFG_DMEM0RAMCTRL_RAMWSEN_MASK                  0x2UL                                             /**< Bit mask for SYSCFG_RAMWSEN                 */
#define _SYSCFG_DMEM0RAMCTRL_RAMWSEN_DEFAULT               0x00000000UL                                      /**< Mode DEFAULT for SYSCFG_DMEM0RAMCTRL        */
#define SYSCFG_DMEM0RAMCTRL_RAMWSEN_DEFAULT                (_SYSCFG_DMEM0RAMCTRL_RAMWSEN_DEFAULT << 1)       /**< Shifted mode DEFAULT for SYSCFG_DMEM0RAMCTRL*/
#define SYSCFG_DMEM0RAMCTRL_RAMPREFETCHEN                  (0x1UL << 2)                                      /**< RAM Prfetch Enable                          */
#define _SYSCFG_DMEM0RAMCTRL_RAMPREFETCHEN_SHIFT           2                                                 /**< Shift value for SYSCFG_RAMPREFETCHEN        */
#define _SYSCFG_DMEM0RAMCTRL_RAMPREFETCHEN_MASK            0x4UL                                             /**< Bit mask for SYSCFG_RAMPREFETCHEN           */
#define _SYSCFG_DMEM0RAMCTRL_RAMPREFETCHEN_DEFAULT         0x00000000UL                                      /**< Mode DEFAULT for SYSCFG_DMEM0RAMCTRL        */
#define SYSCFG_DMEM0RAMCTRL_RAMPREFETCHEN_DEFAULT          (_SYSCFG_DMEM0RAMCTRL_RAMPREFETCHEN_DEFAULT << 2) /**< Shifted mode DEFAULT for SYSCFG_DMEM0RAMCTRL*/

/* Bit fields for SYSCFG RADIORAMRETNCTRL */
#define _SYSCFG_RADIORAMRETNCTRL_RESETVALUE                0x00000000UL                                           /**< Default value for SYSCFG_RADIORAMRETNCTRL   */
#define _SYSCFG_RADIORAMRETNCTRL_MASK                      0x00000101UL                                           /**< Mask for SYSCFG_RADIORAMRETNCTRL            */
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL             (0x1UL << 0)                                           /**< SEQRAM Memory Shutdown Control Register     */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_SHIFT      0                                                      /**< Shift value for SYSCFG_SEQRAMRETNCTRL       */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_MASK       0x1UL                                                  /**< Bit mask for SYSCFG_SEQRAMRETNCTRL          */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_DEFAULT    0x00000000UL                                           /**< Mode DEFAULT for SYSCFG_RADIORAMRETNCTRL    */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLON      0x00000000UL                                           /**< Mode ALLON for SYSCFG_RADIORAMRETNCTRL      */
#define _SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_SEQBLK     0x00000001UL                                           /**< Mode SEQBLK for SYSCFG_RADIORAMRETNCTRL     */
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_DEFAULT     (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLON       (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_ALLON << 0)   /**< Shifted mode ALLON for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_SEQBLK      (_SYSCFG_RADIORAMRETNCTRL_SEQRAMRETNCTRL_SEQBLK << 0)  /**< Shifted mode SEQBLK for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL             (0x1UL << 8)                                           /**< FRCRAM Memory Shutdown Control Register     */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_SHIFT      8                                                      /**< Shift value for SYSCFG_FRCRAMRETNCTRL       */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_MASK       0x100UL                                                /**< Bit mask for SYSCFG_FRCRAMRETNCTRL          */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_DEFAULT    0x00000000UL                                           /**< Mode DEFAULT for SYSCFG_RADIORAMRETNCTRL    */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLON      0x00000000UL                                           /**< Mode ALLON for SYSCFG_RADIORAMRETNCTRL      */
#define _SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_FRCBLK     0x00000001UL                                           /**< Mode FRCBLK for SYSCFG_RADIORAMRETNCTRL     */
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_DEFAULT     (_SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_DEFAULT << 8) /**< Shifted mode DEFAULT for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLON       (_SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_ALLON << 8)   /**< Shifted mode ALLON for SYSCFG_RADIORAMRETNCTRL*/
#define SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_FRCBLK      (_SYSCFG_RADIORAMRETNCTRL_FRCRAMRETNCTRL_FRCBLK << 8)  /**< Shifted mode FRCBLK for SYSCFG_RADIORAMRETNCTRL*/

/* Bit fields for SYSCFG RADIOECCCTRL */
#define _SYSCFG_RADIOECCCTRL_RESETVALUE                    0x00000000UL                                       /**< Default value for SYSCFG_RADIOECCCTRL       */
#define _SYSCFG_RADIOECCCTRL_MASK                          0x00000303UL                                       /**< Mask for SYSCFG_RADIOECCCTRL                */
#define SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN                  (0x1UL << 0)                                       /**< SEQRAM ECC Write Enable                     */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_SHIFT           0                                                  /**< Shift value for SYSCFG_SEQRAMECCEWEN        */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_MASK            0x1UL                                              /**< Bit mask for SYSCFG_SEQRAMECCEWEN           */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_DEFAULT          (_SYSCFG_RADIOECCCTRL_SEQRAMECCEWEN_DEFAULT << 0)  /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/
#define SYSCFG_RADIOECCCTRL_SEQRAMECCCHKEN                 (0x1UL << 1)                                       /**< SEQRAM ECC Check Enable                     */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCCHKEN_SHIFT          1                                                  /**< Shift value for SYSCFG_SEQRAMECCCHKEN       */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCCHKEN_MASK           0x2UL                                              /**< Bit mask for SYSCFG_SEQRAMECCCHKEN          */
#define _SYSCFG_RADIOECCCTRL_SEQRAMECCCHKEN_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_SEQRAMECCCHKEN_DEFAULT         (_SYSCFG_RADIOECCCTRL_SEQRAMECCCHKEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/
#define SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN                  (0x1UL << 8)                                       /**< FRCRAM ECC Write Enable                     */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_SHIFT           8                                                  /**< Shift value for SYSCFG_FRCRAMECCEWEN        */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_MASK            0x100UL                                            /**< Bit mask for SYSCFG_FRCRAMECCEWEN           */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_DEFAULT         0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_DEFAULT          (_SYSCFG_RADIOECCCTRL_FRCRAMECCEWEN_DEFAULT << 8)  /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/
#define SYSCFG_RADIOECCCTRL_FRCRAMECCCHKEN                 (0x1UL << 9)                                       /**< FRCRAM ECC Check Enable                     */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCCHKEN_SHIFT          9                                                  /**< Shift value for SYSCFG_FRCRAMECCCHKEN       */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCCHKEN_MASK           0x200UL                                            /**< Bit mask for SYSCFG_FRCRAMECCCHKEN          */
#define _SYSCFG_RADIOECCCTRL_FRCRAMECCCHKEN_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_RADIOECCCTRL        */
#define SYSCFG_RADIOECCCTRL_FRCRAMECCCHKEN_DEFAULT         (_SYSCFG_RADIOECCCTRL_FRCRAMECCCHKEN_DEFAULT << 9) /**< Shifted mode DEFAULT for SYSCFG_RADIOECCCTRL*/

/* Bit fields for SYSCFG RADIORAMCTRL */
#define _SYSCFG_RADIORAMCTRL_RESETVALUE                    0x00000000UL                                          /**< Default value for SYSCFG_RADIORAMCTRL       */
#define _SYSCFG_RADIORAMCTRL_MASK                          0x01070007UL                                          /**< Mask for SYSCFG_RADIORAMCTRL                */
#define SYSCFG_RADIORAMCTRL_SEQRAMCACHEEN                  (0x1UL << 0)                                          /**< SEQRAM CACHE Enable                         */
#define _SYSCFG_RADIORAMCTRL_SEQRAMCACHEEN_SHIFT           0                                                     /**< Shift value for SYSCFG_SEQRAMCACHEEN        */
#define _SYSCFG_RADIORAMCTRL_SEQRAMCACHEEN_MASK            0x1UL                                                 /**< Bit mask for SYSCFG_SEQRAMCACHEEN           */
#define _SYSCFG_RADIORAMCTRL_SEQRAMCACHEEN_DEFAULT         0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_RADIORAMCTRL        */
#define SYSCFG_RADIORAMCTRL_SEQRAMCACHEEN_DEFAULT          (_SYSCFG_RADIORAMCTRL_SEQRAMCACHEEN_DEFAULT << 0)     /**< Shifted mode DEFAULT for SYSCFG_RADIORAMCTRL*/
#define SYSCFG_RADIORAMCTRL_SEQRAMWSEN                     (0x1UL << 1)                                          /**< SEQRAM WAIT STATE Enable                    */
#define _SYSCFG_RADIORAMCTRL_SEQRAMWSEN_SHIFT              1                                                     /**< Shift value for SYSCFG_SEQRAMWSEN           */
#define _SYSCFG_RADIORAMCTRL_SEQRAMWSEN_MASK               0x2UL                                                 /**< Bit mask for SYSCFG_SEQRAMWSEN              */
#define _SYSCFG_RADIORAMCTRL_SEQRAMWSEN_DEFAULT            0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_RADIORAMCTRL        */
#define SYSCFG_RADIORAMCTRL_SEQRAMWSEN_DEFAULT             (_SYSCFG_RADIORAMCTRL_SEQRAMWSEN_DEFAULT << 1)        /**< Shifted mode DEFAULT for SYSCFG_RADIORAMCTRL*/
#define SYSCFG_RADIORAMCTRL_SEQRAMPREFETCHEN               (0x1UL << 2)                                          /**< SEQRAM Prfetch Enable                       */
#define _SYSCFG_RADIORAMCTRL_SEQRAMPREFETCHEN_SHIFT        2                                                     /**< Shift value for SYSCFG_SEQRAMPREFETCHEN     */
#define _SYSCFG_RADIORAMCTRL_SEQRAMPREFETCHEN_MASK         0x4UL                                                 /**< Bit mask for SYSCFG_SEQRAMPREFETCHEN        */
#define _SYSCFG_RADIORAMCTRL_SEQRAMPREFETCHEN_DEFAULT      0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_RADIORAMCTRL        */
#define SYSCFG_RADIORAMCTRL_SEQRAMPREFETCHEN_DEFAULT       (_SYSCFG_RADIORAMCTRL_SEQRAMPREFETCHEN_DEFAULT << 2)  /**< Shifted mode DEFAULT for SYSCFG_RADIORAMCTRL*/
#define SYSCFG_RADIORAMCTRL_FRCRAMCACHEEN                  (0x1UL << 16)                                         /**< FRCRAM CACHE Enable                         */
#define _SYSCFG_RADIORAMCTRL_FRCRAMCACHEEN_SHIFT           16                                                    /**< Shift value for SYSCFG_FRCRAMCACHEEN        */
#define _SYSCFG_RADIORAMCTRL_FRCRAMCACHEEN_MASK            0x10000UL                                             /**< Bit mask for SYSCFG_FRCRAMCACHEEN           */
#define _SYSCFG_RADIORAMCTRL_FRCRAMCACHEEN_DEFAULT         0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_RADIORAMCTRL        */
#define SYSCFG_RADIORAMCTRL_FRCRAMCACHEEN_DEFAULT          (_SYSCFG_RADIORAMCTRL_FRCRAMCACHEEN_DEFAULT << 16)    /**< Shifted mode DEFAULT for SYSCFG_RADIORAMCTRL*/
#define SYSCFG_RADIORAMCTRL_FRCRAMWSEN                     (0x1UL << 17)                                         /**< FRCRAM WAIT STATE Enable                    */
#define _SYSCFG_RADIORAMCTRL_FRCRAMWSEN_SHIFT              17                                                    /**< Shift value for SYSCFG_FRCRAMWSEN           */
#define _SYSCFG_RADIORAMCTRL_FRCRAMWSEN_MASK               0x20000UL                                             /**< Bit mask for SYSCFG_FRCRAMWSEN              */
#define _SYSCFG_RADIORAMCTRL_FRCRAMWSEN_DEFAULT            0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_RADIORAMCTRL        */
#define SYSCFG_RADIORAMCTRL_FRCRAMWSEN_DEFAULT             (_SYSCFG_RADIORAMCTRL_FRCRAMWSEN_DEFAULT << 17)       /**< Shifted mode DEFAULT for SYSCFG_RADIORAMCTRL*/
#define SYSCFG_RADIORAMCTRL_FRCRAMPREFETCHEN               (0x1UL << 18)                                         /**< FRCRAM Prfetch Enable                       */
#define _SYSCFG_RADIORAMCTRL_FRCRAMPREFETCHEN_SHIFT        18                                                    /**< Shift value for SYSCFG_FRCRAMPREFETCHEN     */
#define _SYSCFG_RADIORAMCTRL_FRCRAMPREFETCHEN_MASK         0x40000UL                                             /**< Bit mask for SYSCFG_FRCRAMPREFETCHEN        */
#define _SYSCFG_RADIORAMCTRL_FRCRAMPREFETCHEN_DEFAULT      0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_RADIORAMCTRL        */
#define SYSCFG_RADIORAMCTRL_FRCRAMPREFETCHEN_DEFAULT       (_SYSCFG_RADIORAMCTRL_FRCRAMPREFETCHEN_DEFAULT << 18) /**< Shifted mode DEFAULT for SYSCFG_RADIORAMCTRL*/
#define SYSCFG_RADIORAMCTRL_DEMODRAMCACHEEN                (0x1UL << 24)                                         /**< DEMODRAM CACHE Enable                       */
#define _SYSCFG_RADIORAMCTRL_DEMODRAMCACHEEN_SHIFT         24                                                    /**< Shift value for SYSCFG_DEMODRAMCACHEEN      */
#define _SYSCFG_RADIORAMCTRL_DEMODRAMCACHEEN_MASK          0x1000000UL                                           /**< Bit mask for SYSCFG_DEMODRAMCACHEEN         */
#define _SYSCFG_RADIORAMCTRL_DEMODRAMCACHEEN_DEFAULT       0x00000000UL                                          /**< Mode DEFAULT for SYSCFG_RADIORAMCTRL        */
#define SYSCFG_RADIORAMCTRL_DEMODRAMCACHEEN_DEFAULT        (_SYSCFG_RADIORAMCTRL_DEMODRAMCACHEEN_DEFAULT << 24)  /**< Shifted mode DEFAULT for SYSCFG_RADIORAMCTRL*/

/* Bit fields for SYSCFG SEQRAMECCADDR */
#define _SYSCFG_SEQRAMECCADDR_RESETVALUE                   0x00000000UL                                       /**< Default value for SYSCFG_SEQRAMECCADDR      */
#define _SYSCFG_SEQRAMECCADDR_MASK                         0xFFFFFFFFUL                                       /**< Mask for SYSCFG_SEQRAMECCADDR               */
#define _SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_SHIFT          0                                                  /**< Shift value for SYSCFG_SEQRAMECCADDR        */
#define _SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_MASK           0xFFFFFFFFUL                                       /**< Bit mask for SYSCFG_SEQRAMECCADDR           */
#define _SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_SEQRAMECCADDR       */
#define SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_DEFAULT         (_SYSCFG_SEQRAMECCADDR_SEQRAMECCADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_SEQRAMECCADDR*/

/* Bit fields for SYSCFG FRCRAMECCADDR */
#define _SYSCFG_FRCRAMECCADDR_RESETVALUE                   0x00000000UL                                       /**< Default value for SYSCFG_FRCRAMECCADDR      */
#define _SYSCFG_FRCRAMECCADDR_MASK                         0xFFFFFFFFUL                                       /**< Mask for SYSCFG_FRCRAMECCADDR               */
#define _SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_SHIFT          0                                                  /**< Shift value for SYSCFG_FRCRAMECCADDR        */
#define _SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_MASK           0xFFFFFFFFUL                                       /**< Bit mask for SYSCFG_FRCRAMECCADDR           */
#define _SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_DEFAULT        0x00000000UL                                       /**< Mode DEFAULT for SYSCFG_FRCRAMECCADDR       */
#define SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_DEFAULT         (_SYSCFG_FRCRAMECCADDR_FRCRAMECCADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for SYSCFG_FRCRAMECCADDR*/

/** @} End of group EFR32MG21_SYSCFG_BitFields */
/** @} End of group EFR32MG21_SYSCFG */
/** @} End of group Parts */
#endif /* EFR32MG21_SYSCFG_H */
