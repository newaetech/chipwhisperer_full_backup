/**************************************************************************//**
 * @file
 * @brief EFR32MG22 DCDC register and bit field definitions
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
#ifndef EFR32MG22_DCDC_H
#define EFR32MG22_DCDC_H
#define DCDC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG22_DCDC DCDC
 * @{
 * @brief EFR32MG22 DCDC Register Declaration.
 *****************************************************************************/

/** DCDC Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IPVERSION                                          */
  __IOM uint32_t EN;                            /**< Enable                                             */
  __IOM uint32_t CTRL;                          /**< Control                                            */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0;                     /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0;                     /**< EM23 Control                                       */
  uint32_t       RESERVED1[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flags                                    */
  __IOM uint32_t IEN;                           /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  uint32_t       RESERVED2[4U];                 /**< Reserved for future use                            */
  __IOM uint32_t LOCK;                          /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS;                    /**< Lock Status Register                               */
  uint32_t       RESERVED3[2U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED5[7U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED6[1U];                 /**< Reserved for future use                            */
  uint32_t       RESERVED7[995U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_SET;                        /**< Enable                                             */
  __IOM uint32_t CTRL_SET;                      /**< Control                                            */
  uint32_t       RESERVED8[1U];                 /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0_SET;                 /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0_SET;                 /**< EM23 Control                                       */
  uint32_t       RESERVED9[3U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  uint32_t       RESERVED10[4U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_SET;                      /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS_SET;                /**< Lock Status Register                               */
  uint32_t       RESERVED11[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED12[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED13[7U];                /**< Reserved for future use                            */
  uint32_t       RESERVED14[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED15[995U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_CLR;                        /**< Enable                                             */
  __IOM uint32_t CTRL_CLR;                      /**< Control                                            */
  uint32_t       RESERVED16[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0_CLR;                 /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0_CLR;                 /**< EM23 Control                                       */
  uint32_t       RESERVED17[3U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  uint32_t       RESERVED18[4U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_CLR;                      /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS_CLR;                /**< Lock Status Register                               */
  uint32_t       RESERVED19[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED20[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED21[7U];                /**< Reserved for future use                            */
  uint32_t       RESERVED22[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED23[995U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IPVERSION                                          */
  __IOM uint32_t EN_TGL;                        /**< Enable                                             */
  __IOM uint32_t CTRL_TGL;                      /**< Control                                            */
  uint32_t       RESERVED24[1U];                /**< Reserved for future use                            */
  __IOM uint32_t EM01CTRL0_TGL;                 /**< EM01 Control                                       */
  __IOM uint32_t EM23CTRL0_TGL;                 /**< EM23 Control                                       */
  uint32_t       RESERVED25[3U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flags                                    */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Enable                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  uint32_t       RESERVED26[4U];                /**< Reserved for future use                            */
  __IOM uint32_t LOCK_TGL;                      /**< Lock Register                                      */
  __IM uint32_t  LOCKSTATUS_TGL;                /**< Lock Status Register                               */
  uint32_t       RESERVED27[2U];                /**< Reserved for future use                            */
  uint32_t       RESERVED28[1U];                /**< Reserved for future use                            */
  uint32_t       RESERVED29[7U];                /**< Reserved for future use                            */
  uint32_t       RESERVED30[1U];                /**< Reserved for future use                            */
} DCDC_TypeDef;
/** @} End of group EFR32MG22_DCDC */

/**************************************************************************//**
 * @addtogroup EFR32MG22_DCDC
 * @{
 * @defgroup EFR32MG22_DCDC_BitFields DCDC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for DCDC IPVERSION */
#define _DCDC_IPVERSION_RESETVALUE                  0x00000000UL                             /**< Default value for DCDC_IPVERSION            */
#define _DCDC_IPVERSION_MASK                        0xFFFFFFFFUL                             /**< Mask for DCDC_IPVERSION                     */
#define _DCDC_IPVERSION_IPVERSION_SHIFT             0                                        /**< Shift value for DCDC_IPVERSION              */
#define _DCDC_IPVERSION_IPVERSION_MASK              0xFFFFFFFFUL                             /**< Bit mask for DCDC_IPVERSION                 */
#define _DCDC_IPVERSION_IPVERSION_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for DCDC_IPVERSION             */
#define DCDC_IPVERSION_IPVERSION_DEFAULT            (_DCDC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for DCDC_IPVERSION     */

/* Bit fields for DCDC EN */
#define _DCDC_EN_RESETVALUE                         0x00000000UL                        /**< Default value for DCDC_EN                   */
#define _DCDC_EN_MASK                               0x00000001UL                        /**< Mask for DCDC_EN                            */
#define DCDC_EN_EN                                  (0x1UL << 0)                        /**< Enable                                      */
#define _DCDC_EN_EN_SHIFT                           0                                   /**< Shift value for DCDC_EN                     */
#define _DCDC_EN_EN_MASK                            0x1UL                               /**< Bit mask for DCDC_EN                        */
#define _DCDC_EN_EN_DEFAULT                         0x00000000UL                        /**< Mode DEFAULT for DCDC_EN                    */
#define _DCDC_EN_EN_DISABLE                         0x00000000UL                        /**< Mode DISABLE for DCDC_EN                    */
#define _DCDC_EN_EN_ENABLE                          0x00000001UL                        /**< Mode ENABLE for DCDC_EN                     */
#define DCDC_EN_EN_DEFAULT                          (_DCDC_EN_EN_DEFAULT << 0)          /**< Shifted mode DEFAULT for DCDC_EN            */
#define DCDC_EN_EN_DISABLE                          (_DCDC_EN_EN_DISABLE << 0)          /**< Shifted mode DISABLE for DCDC_EN            */
#define DCDC_EN_EN_ENABLE                           (_DCDC_EN_EN_ENABLE << 0)           /**< Shifted mode ENABLE for DCDC_EN             */

/* Bit fields for DCDC CTRL */
#define _DCDC_CTRL_RESETVALUE                       0x00000044UL                              /**< Default value for DCDC_CTRL                 */
#define _DCDC_CTRL_MASK                             0x00000077UL                              /**< Mask for DCDC_CTRL                          */
#define DCDC_CTRL_MODE                              (0x1UL << 0)                              /**< DCDC/Bypass Mode Control                    */
#define _DCDC_CTRL_MODE_SHIFT                       0                                         /**< Shift value for DCDC_MODE                   */
#define _DCDC_CTRL_MODE_MASK                        0x1UL                                     /**< Bit mask for DCDC_MODE                      */
#define _DCDC_CTRL_MODE_DEFAULT                     0x00000000UL                              /**< Mode DEFAULT for DCDC_CTRL                  */
#define _DCDC_CTRL_MODE_BYPASS                      0x00000000UL                              /**< Mode BYPASS for DCDC_CTRL                   */
#define _DCDC_CTRL_MODE_DCDCREGULATION              0x00000001UL                              /**< Mode DCDCREGULATION for DCDC_CTRL           */
#define DCDC_CTRL_MODE_DEFAULT                      (_DCDC_CTRL_MODE_DEFAULT << 0)            /**< Shifted mode DEFAULT for DCDC_CTRL          */
#define DCDC_CTRL_MODE_BYPASS                       (_DCDC_CTRL_MODE_BYPASS << 0)             /**< Shifted mode BYPASS for DCDC_CTRL           */
#define DCDC_CTRL_MODE_DCDCREGULATION               (_DCDC_CTRL_MODE_DCDCREGULATION << 0)     /**< Shifted mode DCDCREGULATION for DCDC_CTRL   */
#define DCDC_CTRL_DCMONLYEN                         (0x1UL << 2)                              /**< DCDC DCM Only Enable                        */
#define _DCDC_CTRL_DCMONLYEN_SHIFT                  2                                         /**< Shift value for DCDC_DCMONLYEN              */
#define _DCDC_CTRL_DCMONLYEN_MASK                   0x4UL                                     /**< Bit mask for DCDC_DCMONLYEN                 */
#define _DCDC_CTRL_DCMONLYEN_DEFAULT                0x00000001UL                              /**< Mode DEFAULT for DCDC_CTRL                  */
#define _DCDC_CTRL_DCMONLYEN_DUALMODE               0x00000000UL                              /**< Mode DUALMODE for DCDC_CTRL                 */
#define _DCDC_CTRL_DCMONLYEN_DCMONLYEN              0x00000001UL                              /**< Mode DCMONLYEN for DCDC_CTRL                */
#define DCDC_CTRL_DCMONLYEN_DEFAULT                 (_DCDC_CTRL_DCMONLYEN_DEFAULT << 2)       /**< Shifted mode DEFAULT for DCDC_CTRL          */
#define DCDC_CTRL_DCMONLYEN_DUALMODE                (_DCDC_CTRL_DCMONLYEN_DUALMODE << 2)      /**< Shifted mode DUALMODE for DCDC_CTRL         */
#define DCDC_CTRL_DCMONLYEN_DCMONLYEN               (_DCDC_CTRL_DCMONLYEN_DCMONLYEN << 2)     /**< Shifted mode DCMONLYEN for DCDC_CTRL        */
#define _DCDC_CTRL_IPKTMAXCTRL_SHIFT                4                                         /**< Shift value for DCDC_IPKTMAXCTRL            */
#define _DCDC_CTRL_IPKTMAXCTRL_MASK                 0x70UL                                    /**< Bit mask for DCDC_IPKTMAXCTRL               */
#define _DCDC_CTRL_IPKTMAXCTRL_DEFAULT              0x00000004UL                              /**< Mode DEFAULT for DCDC_CTRL                  */
#define _DCDC_CTRL_IPKTMAXCTRL_OFF                  0x00000000UL                              /**< Mode OFF for DCDC_CTRL                      */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P35us          0x00000001UL                              /**< Mode TMAX_0P35us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P63us          0x00000002UL                              /**< Mode TMAX_0P63us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_0P91us          0x00000003UL                              /**< Mode TMAX_0P91us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P19us          0x00000004UL                              /**< Mode TMAX_1P19us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P47us          0x00000005UL                              /**< Mode TMAX_1P47us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_1P75us          0x00000006UL                              /**< Mode TMAX_1P75us for DCDC_CTRL              */
#define _DCDC_CTRL_IPKTMAXCTRL_TMAX_2P03us          0x00000007UL                              /**< Mode TMAX_2P03us for DCDC_CTRL              */
#define DCDC_CTRL_IPKTMAXCTRL_DEFAULT               (_DCDC_CTRL_IPKTMAXCTRL_DEFAULT << 4)     /**< Shifted mode DEFAULT for DCDC_CTRL          */
#define DCDC_CTRL_IPKTMAXCTRL_OFF                   (_DCDC_CTRL_IPKTMAXCTRL_OFF << 4)         /**< Shifted mode OFF for DCDC_CTRL              */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_0P35us           (_DCDC_CTRL_IPKTMAXCTRL_TMAX_0P35us << 4) /**< Shifted mode TMAX_0P35us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_0P63us           (_DCDC_CTRL_IPKTMAXCTRL_TMAX_0P63us << 4) /**< Shifted mode TMAX_0P63us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_0P91us           (_DCDC_CTRL_IPKTMAXCTRL_TMAX_0P91us << 4) /**< Shifted mode TMAX_0P91us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_1P19us           (_DCDC_CTRL_IPKTMAXCTRL_TMAX_1P19us << 4) /**< Shifted mode TMAX_1P19us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_1P47us           (_DCDC_CTRL_IPKTMAXCTRL_TMAX_1P47us << 4) /**< Shifted mode TMAX_1P47us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_1P75us           (_DCDC_CTRL_IPKTMAXCTRL_TMAX_1P75us << 4) /**< Shifted mode TMAX_1P75us for DCDC_CTRL      */
#define DCDC_CTRL_IPKTMAXCTRL_TMAX_2P03us           (_DCDC_CTRL_IPKTMAXCTRL_TMAX_2P03us << 4) /**< Shifted mode TMAX_2P03us for DCDC_CTRL      */

/* Bit fields for DCDC EM01CTRL0 */
#define _DCDC_EM01CTRL0_RESETVALUE                  0x00000109UL                                    /**< Default value for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_MASK                        0x0000030FUL                                    /**< Mask for DCDC_EM01CTRL0                     */
#define _DCDC_EM01CTRL0_IPKVAL_SHIFT                0                                               /**< Shift value for DCDC_IPKVAL                 */
#define _DCDC_EM01CTRL0_IPKVAL_MASK                 0xFUL                                           /**< Bit mask for DCDC_IPKVAL                    */
#define _DCDC_EM01CTRL0_IPKVAL_DEFAULT              0x00000009UL                                    /**< Mode DEFAULT for DCDC_EM01CTRL0             */
#define _DCDC_EM01CTRL0_IPKVAL_Load36mA             0x00000003UL                                    /**< Mode Load36mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load40mA             0x00000004UL                                    /**< Mode Load40mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load44mA             0x00000005UL                                    /**< Mode Load44mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load48mA             0x00000006UL                                    /**< Mode Load48mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load52mA             0x00000007UL                                    /**< Mode Load52mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load56mA             0x00000008UL                                    /**< Mode Load56mA for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_IPKVAL_Load60mA             0x00000009UL                                    /**< Mode Load60mA for DCDC_EM01CTRL0            */
#define DCDC_EM01CTRL0_IPKVAL_DEFAULT               (_DCDC_EM01CTRL0_IPKVAL_DEFAULT << 0)           /**< Shifted mode DEFAULT for DCDC_EM01CTRL0     */
#define DCDC_EM01CTRL0_IPKVAL_Load36mA              (_DCDC_EM01CTRL0_IPKVAL_Load36mA << 0)          /**< Shifted mode Load36mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load40mA              (_DCDC_EM01CTRL0_IPKVAL_Load40mA << 0)          /**< Shifted mode Load40mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load44mA              (_DCDC_EM01CTRL0_IPKVAL_Load44mA << 0)          /**< Shifted mode Load44mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load48mA              (_DCDC_EM01CTRL0_IPKVAL_Load48mA << 0)          /**< Shifted mode Load48mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load52mA              (_DCDC_EM01CTRL0_IPKVAL_Load52mA << 0)          /**< Shifted mode Load52mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load56mA              (_DCDC_EM01CTRL0_IPKVAL_Load56mA << 0)          /**< Shifted mode Load56mA for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_IPKVAL_Load60mA              (_DCDC_EM01CTRL0_IPKVAL_Load60mA << 0)          /**< Shifted mode Load60mA for DCDC_EM01CTRL0    */
#define _DCDC_EM01CTRL0_DRVSPEED_SHIFT              8                                               /**< Shift value for DCDC_DRVSPEED               */
#define _DCDC_EM01CTRL0_DRVSPEED_MASK               0x300UL                                         /**< Bit mask for DCDC_DRVSPEED                  */
#define _DCDC_EM01CTRL0_DRVSPEED_DEFAULT            0x00000001UL                                    /**< Mode DEFAULT for DCDC_EM01CTRL0             */
#define _DCDC_EM01CTRL0_DRVSPEED_BEST_EMI           0x00000000UL                                    /**< Mode BEST_EMI for DCDC_EM01CTRL0            */
#define _DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING    0x00000001UL                                    /**< Mode DEFAULT_SETTING for DCDC_EM01CTRL0     */
#define _DCDC_EM01CTRL0_DRVSPEED_INTERMEDIATE       0x00000002UL                                    /**< Mode INTERMEDIATE for DCDC_EM01CTRL0        */
#define _DCDC_EM01CTRL0_DRVSPEED_BEST_EFFICIENCY    0x00000003UL                                    /**< Mode BEST_EFFICIENCY for DCDC_EM01CTRL0     */
#define DCDC_EM01CTRL0_DRVSPEED_DEFAULT             (_DCDC_EM01CTRL0_DRVSPEED_DEFAULT << 8)         /**< Shifted mode DEFAULT for DCDC_EM01CTRL0     */
#define DCDC_EM01CTRL0_DRVSPEED_BEST_EMI            (_DCDC_EM01CTRL0_DRVSPEED_BEST_EMI << 8)        /**< Shifted mode BEST_EMI for DCDC_EM01CTRL0    */
#define DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING     (_DCDC_EM01CTRL0_DRVSPEED_DEFAULT_SETTING << 8) /**< Shifted mode DEFAULT_SETTING for DCDC_EM01CTRL0*/
#define DCDC_EM01CTRL0_DRVSPEED_INTERMEDIATE        (_DCDC_EM01CTRL0_DRVSPEED_INTERMEDIATE << 8)    /**< Shifted mode INTERMEDIATE for DCDC_EM01CTRL0*/
#define DCDC_EM01CTRL0_DRVSPEED_BEST_EFFICIENCY     (_DCDC_EM01CTRL0_DRVSPEED_BEST_EFFICIENCY << 8) /**< Shifted mode BEST_EFFICIENCY for DCDC_EM01CTRL0*/

/* Bit fields for DCDC EM23CTRL0 */
#define _DCDC_EM23CTRL0_RESETVALUE                  0x00000103UL                                    /**< Default value for DCDC_EM23CTRL0            */
#define _DCDC_EM23CTRL0_MASK                        0x0000030FUL                                    /**< Mask for DCDC_EM23CTRL0                     */
#define _DCDC_EM23CTRL0_IPKVAL_SHIFT                0                                               /**< Shift value for DCDC_IPKVAL                 */
#define _DCDC_EM23CTRL0_IPKVAL_MASK                 0xFUL                                           /**< Bit mask for DCDC_IPKVAL                    */
#define _DCDC_EM23CTRL0_IPKVAL_DEFAULT              0x00000003UL                                    /**< Mode DEFAULT for DCDC_EM23CTRL0             */
#define _DCDC_EM23CTRL0_IPKVAL_LOAD5MA              0x00000003UL                                    /**< Mode LOAD5MA for DCDC_EM23CTRL0             */
#define _DCDC_EM23CTRL0_IPKVAL_LOAD10MA             0x00000009UL                                    /**< Mode LOAD10MA for DCDC_EM23CTRL0            */
#define DCDC_EM23CTRL0_IPKVAL_DEFAULT               (_DCDC_EM23CTRL0_IPKVAL_DEFAULT << 0)           /**< Shifted mode DEFAULT for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_IPKVAL_LOAD5MA               (_DCDC_EM23CTRL0_IPKVAL_LOAD5MA << 0)           /**< Shifted mode LOAD5MA for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_IPKVAL_LOAD10MA              (_DCDC_EM23CTRL0_IPKVAL_LOAD10MA << 0)          /**< Shifted mode LOAD10MA for DCDC_EM23CTRL0    */
#define _DCDC_EM23CTRL0_DRVSPEED_SHIFT              8                                               /**< Shift value for DCDC_DRVSPEED               */
#define _DCDC_EM23CTRL0_DRVSPEED_MASK               0x300UL                                         /**< Bit mask for DCDC_DRVSPEED                  */
#define _DCDC_EM23CTRL0_DRVSPEED_DEFAULT            0x00000001UL                                    /**< Mode DEFAULT for DCDC_EM23CTRL0             */
#define _DCDC_EM23CTRL0_DRVSPEED_BEST_EMI           0x00000000UL                                    /**< Mode BEST_EMI for DCDC_EM23CTRL0            */
#define _DCDC_EM23CTRL0_DRVSPEED_DEFAULT_SETTING    0x00000001UL                                    /**< Mode DEFAULT_SETTING for DCDC_EM23CTRL0     */
#define _DCDC_EM23CTRL0_DRVSPEED_INTERMEDIATE       0x00000002UL                                    /**< Mode INTERMEDIATE for DCDC_EM23CTRL0        */
#define _DCDC_EM23CTRL0_DRVSPEED_BEST_EFFICIENCY    0x00000003UL                                    /**< Mode BEST_EFFICIENCY for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_DRVSPEED_DEFAULT             (_DCDC_EM23CTRL0_DRVSPEED_DEFAULT << 8)         /**< Shifted mode DEFAULT for DCDC_EM23CTRL0     */
#define DCDC_EM23CTRL0_DRVSPEED_BEST_EMI            (_DCDC_EM23CTRL0_DRVSPEED_BEST_EMI << 8)        /**< Shifted mode BEST_EMI for DCDC_EM23CTRL0    */
#define DCDC_EM23CTRL0_DRVSPEED_DEFAULT_SETTING     (_DCDC_EM23CTRL0_DRVSPEED_DEFAULT_SETTING << 8) /**< Shifted mode DEFAULT_SETTING for DCDC_EM23CTRL0*/
#define DCDC_EM23CTRL0_DRVSPEED_INTERMEDIATE        (_DCDC_EM23CTRL0_DRVSPEED_INTERMEDIATE << 8)    /**< Shifted mode INTERMEDIATE for DCDC_EM23CTRL0*/
#define DCDC_EM23CTRL0_DRVSPEED_BEST_EFFICIENCY     (_DCDC_EM23CTRL0_DRVSPEED_BEST_EFFICIENCY << 8) /**< Shifted mode BEST_EFFICIENCY for DCDC_EM23CTRL0*/

/* Bit fields for DCDC IF */
#define _DCDC_IF_RESETVALUE                         0x00000000UL                        /**< Default value for DCDC_IF                   */
#define _DCDC_IF_MASK                               0x000000FFUL                        /**< Mask for DCDC_IF                            */
#define DCDC_IF_BYPSW                               (0x1UL << 0)                        /**< Bypass Switch Enabled                       */
#define _DCDC_IF_BYPSW_SHIFT                        0                                   /**< Shift value for DCDC_BYPSW                  */
#define _DCDC_IF_BYPSW_MASK                         0x1UL                               /**< Bit mask for DCDC_BYPSW                     */
#define _DCDC_IF_BYPSW_DEFAULT                      0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_BYPSW_DEFAULT                       (_DCDC_IF_BYPSW_DEFAULT << 0)       /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_WARM                                (0x1UL << 1)                        /**< DCDC Warmup Time Done                       */
#define _DCDC_IF_WARM_SHIFT                         1                                   /**< Shift value for DCDC_WARM                   */
#define _DCDC_IF_WARM_MASK                          0x2UL                               /**< Bit mask for DCDC_WARM                      */
#define _DCDC_IF_WARM_DEFAULT                       0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_WARM_DEFAULT                        (_DCDC_IF_WARM_DEFAULT << 1)        /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_RUNNING                             (0x1UL << 2)                        /**< DCDC Running                                */
#define _DCDC_IF_RUNNING_SHIFT                      2                                   /**< Shift value for DCDC_RUNNING                */
#define _DCDC_IF_RUNNING_MASK                       0x4UL                               /**< Bit mask for DCDC_RUNNING                   */
#define _DCDC_IF_RUNNING_DEFAULT                    0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_RUNNING_DEFAULT                     (_DCDC_IF_RUNNING_DEFAULT << 2)     /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_VREGINLOW                           (0x1UL << 3)                        /**< VREGVDD below threshold                     */
#define _DCDC_IF_VREGINLOW_SHIFT                    3                                   /**< Shift value for DCDC_VREGINLOW              */
#define _DCDC_IF_VREGINLOW_MASK                     0x8UL                               /**< Bit mask for DCDC_VREGINLOW                 */
#define _DCDC_IF_VREGINLOW_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_VREGINLOW_DEFAULT                   (_DCDC_IF_VREGINLOW_DEFAULT << 3)   /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_VREGINHIGH                          (0x1UL << 4)                        /**< VREGVDD above threshold                     */
#define _DCDC_IF_VREGINHIGH_SHIFT                   4                                   /**< Shift value for DCDC_VREGINHIGH             */
#define _DCDC_IF_VREGINHIGH_MASK                    0x10UL                              /**< Bit mask for DCDC_VREGINHIGH                */
#define _DCDC_IF_VREGINHIGH_DEFAULT                 0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_VREGINHIGH_DEFAULT                  (_DCDC_IF_VREGINHIGH_DEFAULT << 4)  /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_REGULATION                          (0x1UL << 5)                        /**< DCDC in regulation                          */
#define _DCDC_IF_REGULATION_SHIFT                   5                                   /**< Shift value for DCDC_REGULATION             */
#define _DCDC_IF_REGULATION_MASK                    0x20UL                              /**< Bit mask for DCDC_REGULATION                */
#define _DCDC_IF_REGULATION_DEFAULT                 0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_REGULATION_DEFAULT                  (_DCDC_IF_REGULATION_DEFAULT << 5)  /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_TMAX                                (0x1UL << 6)                        /**< Ton_max Timeout Reached                     */
#define _DCDC_IF_TMAX_SHIFT                         6                                   /**< Shift value for DCDC_TMAX                   */
#define _DCDC_IF_TMAX_MASK                          0x40UL                              /**< Bit mask for DCDC_TMAX                      */
#define _DCDC_IF_TMAX_DEFAULT                       0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_TMAX_DEFAULT                        (_DCDC_IF_TMAX_DEFAULT << 6)        /**< Shifted mode DEFAULT for DCDC_IF            */
#define DCDC_IF_EM4ERR                              (0x1UL << 7)                        /**< EM4 Entry Request Error                     */
#define _DCDC_IF_EM4ERR_SHIFT                       7                                   /**< Shift value for DCDC_EM4ERR                 */
#define _DCDC_IF_EM4ERR_MASK                        0x80UL                              /**< Bit mask for DCDC_EM4ERR                    */
#define _DCDC_IF_EM4ERR_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for DCDC_IF                    */
#define DCDC_IF_EM4ERR_DEFAULT                      (_DCDC_IF_EM4ERR_DEFAULT << 7)      /**< Shifted mode DEFAULT for DCDC_IF            */

/* Bit fields for DCDC IEN */
#define _DCDC_IEN_RESETVALUE                        0x00000000UL                        /**< Default value for DCDC_IEN                  */
#define _DCDC_IEN_MASK                              0x000000FFUL                        /**< Mask for DCDC_IEN                           */
#define DCDC_IEN_BYPSW                              (0x1UL << 0)                        /**< Bypass Switch Enabled Interrupt Enable      */
#define _DCDC_IEN_BYPSW_SHIFT                       0                                   /**< Shift value for DCDC_BYPSW                  */
#define _DCDC_IEN_BYPSW_MASK                        0x1UL                               /**< Bit mask for DCDC_BYPSW                     */
#define _DCDC_IEN_BYPSW_DEFAULT                     0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_BYPSW_DEFAULT                      (_DCDC_IEN_BYPSW_DEFAULT << 0)      /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_WARM                               (0x1UL << 1)                        /**< DCDC Warmup Time Done Interrupt Enable      */
#define _DCDC_IEN_WARM_SHIFT                        1                                   /**< Shift value for DCDC_WARM                   */
#define _DCDC_IEN_WARM_MASK                         0x2UL                               /**< Bit mask for DCDC_WARM                      */
#define _DCDC_IEN_WARM_DEFAULT                      0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_WARM_DEFAULT                       (_DCDC_IEN_WARM_DEFAULT << 1)       /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_RUNNING                            (0x1UL << 2)                        /**< DCDC Running Interrupt Enable               */
#define _DCDC_IEN_RUNNING_SHIFT                     2                                   /**< Shift value for DCDC_RUNNING                */
#define _DCDC_IEN_RUNNING_MASK                      0x4UL                               /**< Bit mask for DCDC_RUNNING                   */
#define _DCDC_IEN_RUNNING_DEFAULT                   0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_RUNNING_DEFAULT                    (_DCDC_IEN_RUNNING_DEFAULT << 2)    /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_VREGINLOW                          (0x1UL << 3)                        /**< VREGVDD below threshold Interrupt Enable    */
#define _DCDC_IEN_VREGINLOW_SHIFT                   3                                   /**< Shift value for DCDC_VREGINLOW              */
#define _DCDC_IEN_VREGINLOW_MASK                    0x8UL                               /**< Bit mask for DCDC_VREGINLOW                 */
#define _DCDC_IEN_VREGINLOW_DEFAULT                 0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_VREGINLOW_DEFAULT                  (_DCDC_IEN_VREGINLOW_DEFAULT << 3)  /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_VREGINHIGH                         (0x1UL << 4)                        /**< VREGVDD above threshold Interrupt Enable    */
#define _DCDC_IEN_VREGINHIGH_SHIFT                  4                                   /**< Shift value for DCDC_VREGINHIGH             */
#define _DCDC_IEN_VREGINHIGH_MASK                   0x10UL                              /**< Bit mask for DCDC_VREGINHIGH                */
#define _DCDC_IEN_VREGINHIGH_DEFAULT                0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_VREGINHIGH_DEFAULT                 (_DCDC_IEN_VREGINHIGH_DEFAULT << 4) /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_REGULATION                         (0x1UL << 5)                        /**< DCDC in Regulation Interrupt Enable         */
#define _DCDC_IEN_REGULATION_SHIFT                  5                                   /**< Shift value for DCDC_REGULATION             */
#define _DCDC_IEN_REGULATION_MASK                   0x20UL                              /**< Bit mask for DCDC_REGULATION                */
#define _DCDC_IEN_REGULATION_DEFAULT                0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_REGULATION_DEFAULT                 (_DCDC_IEN_REGULATION_DEFAULT << 5) /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_TMAX                               (0x1UL << 6)                        /**< Ton_max Timeout Interrupt Enable            */
#define _DCDC_IEN_TMAX_SHIFT                        6                                   /**< Shift value for DCDC_TMAX                   */
#define _DCDC_IEN_TMAX_MASK                         0x40UL                              /**< Bit mask for DCDC_TMAX                      */
#define _DCDC_IEN_TMAX_DEFAULT                      0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_TMAX_DEFAULT                       (_DCDC_IEN_TMAX_DEFAULT << 6)       /**< Shifted mode DEFAULT for DCDC_IEN           */
#define DCDC_IEN_EM4ERR                             (0x1UL << 7)                        /**< EM4 Entry Req Interrupt Enable              */
#define _DCDC_IEN_EM4ERR_SHIFT                      7                                   /**< Shift value for DCDC_EM4ERR                 */
#define _DCDC_IEN_EM4ERR_MASK                       0x80UL                              /**< Bit mask for DCDC_EM4ERR                    */
#define _DCDC_IEN_EM4ERR_DEFAULT                    0x00000000UL                        /**< Mode DEFAULT for DCDC_IEN                   */
#define DCDC_IEN_EM4ERR_DEFAULT                     (_DCDC_IEN_EM4ERR_DEFAULT << 7)     /**< Shifted mode DEFAULT for DCDC_IEN           */

/* Bit fields for DCDC STATUS */
#define _DCDC_STATUS_RESETVALUE                     0x00000000UL                          /**< Default value for DCDC_STATUS               */
#define _DCDC_STATUS_MASK                           0x0000001FUL                          /**< Mask for DCDC_STATUS                        */
#define DCDC_STATUS_BYPSW                           (0x1UL << 0)                          /**< Bypass Switch is currently enabled          */
#define _DCDC_STATUS_BYPSW_SHIFT                    0                                     /**< Shift value for DCDC_BYPSW                  */
#define _DCDC_STATUS_BYPSW_MASK                     0x1UL                                 /**< Bit mask for DCDC_BYPSW                     */
#define _DCDC_STATUS_BYPSW_DEFAULT                  0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_BYPSW_DEFAULT                   (_DCDC_STATUS_BYPSW_DEFAULT << 0)     /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_WARM                            (0x1UL << 1)                          /**< DCDC Warmup Done                            */
#define _DCDC_STATUS_WARM_SHIFT                     1                                     /**< Shift value for DCDC_WARM                   */
#define _DCDC_STATUS_WARM_MASK                      0x2UL                                 /**< Bit mask for DCDC_WARM                      */
#define _DCDC_STATUS_WARM_DEFAULT                   0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_WARM_DEFAULT                    (_DCDC_STATUS_WARM_DEFAULT << 1)      /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_RUNNING                         (0x1UL << 2)                          /**< DCDC is running                             */
#define _DCDC_STATUS_RUNNING_SHIFT                  2                                     /**< Shift value for DCDC_RUNNING                */
#define _DCDC_STATUS_RUNNING_MASK                   0x4UL                                 /**< Bit mask for DCDC_RUNNING                   */
#define _DCDC_STATUS_RUNNING_DEFAULT                0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_RUNNING_DEFAULT                 (_DCDC_STATUS_RUNNING_DEFAULT << 2)   /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_VREGIN                          (0x1UL << 3)                          /**< VREGVDD comparator status                   */
#define _DCDC_STATUS_VREGIN_SHIFT                   3                                     /**< Shift value for DCDC_VREGIN                 */
#define _DCDC_STATUS_VREGIN_MASK                    0x8UL                                 /**< Bit mask for DCDC_VREGIN                    */
#define _DCDC_STATUS_VREGIN_DEFAULT                 0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_VREGIN_DEFAULT                  (_DCDC_STATUS_VREGIN_DEFAULT << 3)    /**< Shifted mode DEFAULT for DCDC_STATUS        */
#define DCDC_STATUS_BYPCMPOUT                       (0x1UL << 4)                          /**< Bypass Comparator Output                    */
#define _DCDC_STATUS_BYPCMPOUT_SHIFT                4                                     /**< Shift value for DCDC_BYPCMPOUT              */
#define _DCDC_STATUS_BYPCMPOUT_MASK                 0x10UL                                /**< Bit mask for DCDC_BYPCMPOUT                 */
#define _DCDC_STATUS_BYPCMPOUT_DEFAULT              0x00000000UL                          /**< Mode DEFAULT for DCDC_STATUS                */
#define DCDC_STATUS_BYPCMPOUT_DEFAULT               (_DCDC_STATUS_BYPCMPOUT_DEFAULT << 4) /**< Shifted mode DEFAULT for DCDC_STATUS        */

/* Bit fields for DCDC LOCK */
#define _DCDC_LOCK_RESETVALUE                       0x00000000UL                        /**< Default value for DCDC_LOCK                 */
#define _DCDC_LOCK_MASK                             0x0000FFFFUL                        /**< Mask for DCDC_LOCK                          */
#define _DCDC_LOCK_LOCKKEY_SHIFT                    0                                   /**< Shift value for DCDC_LOCKKEY                */
#define _DCDC_LOCK_LOCKKEY_MASK                     0xFFFFUL                            /**< Bit mask for DCDC_LOCKKEY                   */
#define _DCDC_LOCK_LOCKKEY_DEFAULT                  0x00000000UL                        /**< Mode DEFAULT for DCDC_LOCK                  */
#define _DCDC_LOCK_LOCKKEY_UNLOCKKEY                0x0000ABCDUL                        /**< Mode UNLOCKKEY for DCDC_LOCK                */
#define DCDC_LOCK_LOCKKEY_DEFAULT                   (_DCDC_LOCK_LOCKKEY_DEFAULT << 0)   /**< Shifted mode DEFAULT for DCDC_LOCK          */
#define DCDC_LOCK_LOCKKEY_UNLOCKKEY                 (_DCDC_LOCK_LOCKKEY_UNLOCKKEY << 0) /**< Shifted mode UNLOCKKEY for DCDC_LOCK        */

/* Bit fields for DCDC LOCKSTATUS */
#define _DCDC_LOCKSTATUS_RESETVALUE                 0x00000000UL                          /**< Default value for DCDC_LOCKSTATUS           */
#define _DCDC_LOCKSTATUS_MASK                       0x00000001UL                          /**< Mask for DCDC_LOCKSTATUS                    */
#define DCDC_LOCKSTATUS_LOCK                        (0x1UL << 0)                          /**< Lock Status                                 */
#define _DCDC_LOCKSTATUS_LOCK_SHIFT                 0                                     /**< Shift value for DCDC_LOCK                   */
#define _DCDC_LOCKSTATUS_LOCK_MASK                  0x1UL                                 /**< Bit mask for DCDC_LOCK                      */
#define _DCDC_LOCKSTATUS_LOCK_DEFAULT               0x00000000UL                          /**< Mode DEFAULT for DCDC_LOCKSTATUS            */
#define _DCDC_LOCKSTATUS_LOCK_UNLOCKED              0x00000000UL                          /**< Mode UNLOCKED for DCDC_LOCKSTATUS           */
#define _DCDC_LOCKSTATUS_LOCK_LOCKED                0x00000001UL                          /**< Mode LOCKED for DCDC_LOCKSTATUS             */
#define DCDC_LOCKSTATUS_LOCK_DEFAULT                (_DCDC_LOCKSTATUS_LOCK_DEFAULT << 0)  /**< Shifted mode DEFAULT for DCDC_LOCKSTATUS    */
#define DCDC_LOCKSTATUS_LOCK_UNLOCKED               (_DCDC_LOCKSTATUS_LOCK_UNLOCKED << 0) /**< Shifted mode UNLOCKED for DCDC_LOCKSTATUS   */
#define DCDC_LOCKSTATUS_LOCK_LOCKED                 (_DCDC_LOCKSTATUS_LOCK_LOCKED << 0)   /**< Shifted mode LOCKED for DCDC_LOCKSTATUS     */

/** @} End of group EFR32MG22_DCDC_BitFields */
/** @} End of group EFR32MG22_DCDC */
/** @} End of group Parts */
#endif /* EFR32MG22_DCDC_H */
