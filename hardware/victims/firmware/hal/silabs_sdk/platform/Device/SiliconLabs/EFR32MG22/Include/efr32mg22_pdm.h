/**************************************************************************//**
 * @file
 * @brief EFR32MG22 PDM register and bit field definitions
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
#ifndef EFR32MG22_PDM_H
#define EFR32MG22_PDM_H
#define PDM_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG22_PDM PDM
 * @{
 * @brief EFR32MG22 PDM Register Declaration.
 *****************************************************************************/

/** PDM Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP Version ID                                      */
  __IOM uint32_t EN;                            /**< PDM Module enable Register                         */
  __IOM uint32_t CTRL;                          /**< PDM Core Control Register                          */
  __IOM uint32_t CMD;                           /**< PDM Core Command Register                          */
  __IM uint32_t  STATUS;                        /**< PDM Status register                                */
  __IOM uint32_t CFG0;                          /**< PDM Core Configuration Register0                   */
  __IOM uint32_t CFG1;                          /**< PDM Core Configuration Register1                   */
  uint32_t       RESERVED0[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  RXDATA;                        /**< PDM Received Data Register                         */
  uint32_t       RESERVED1[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF;                            /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN;                           /**< Interrupt Flag Register                            */
  uint32_t       RESERVED2[6U];                 /**< Reserved for future use                            */
  __IM uint32_t  SYNCBUSY;                      /**< Synchronization Busy Register                      */
  uint32_t       RESERVED3[999U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP Version ID                                      */
  __IOM uint32_t EN_SET;                        /**< PDM Module enable Register                         */
  __IOM uint32_t CTRL_SET;                      /**< PDM Core Control Register                          */
  __IOM uint32_t CMD_SET;                       /**< PDM Core Command Register                          */
  __IM uint32_t  STATUS_SET;                    /**< PDM Status register                                */
  __IOM uint32_t CFG0_SET;                      /**< PDM Core Configuration Register0                   */
  __IOM uint32_t CFG1_SET;                      /**< PDM Core Configuration Register1                   */
  uint32_t       RESERVED4[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  RXDATA_SET;                    /**< PDM Received Data Register                         */
  uint32_t       RESERVED5[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_SET;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_SET;                       /**< Interrupt Flag Register                            */
  uint32_t       RESERVED6[6U];                 /**< Reserved for future use                            */
  __IM uint32_t  SYNCBUSY_SET;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED7[999U];               /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP Version ID                                      */
  __IOM uint32_t EN_CLR;                        /**< PDM Module enable Register                         */
  __IOM uint32_t CTRL_CLR;                      /**< PDM Core Control Register                          */
  __IOM uint32_t CMD_CLR;                       /**< PDM Core Command Register                          */
  __IM uint32_t  STATUS_CLR;                    /**< PDM Status register                                */
  __IOM uint32_t CFG0_CLR;                      /**< PDM Core Configuration Register0                   */
  __IOM uint32_t CFG1_CLR;                      /**< PDM Core Configuration Register1                   */
  uint32_t       RESERVED8[1U];                 /**< Reserved for future use                            */
  __IM uint32_t  RXDATA_CLR;                    /**< PDM Received Data Register                         */
  uint32_t       RESERVED9[7U];                 /**< Reserved for future use                            */
  __IOM uint32_t IF_CLR;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_CLR;                       /**< Interrupt Flag Register                            */
  uint32_t       RESERVED10[6U];                /**< Reserved for future use                            */
  __IM uint32_t  SYNCBUSY_CLR;                  /**< Synchronization Busy Register                      */
  uint32_t       RESERVED11[999U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP Version ID                                      */
  __IOM uint32_t EN_TGL;                        /**< PDM Module enable Register                         */
  __IOM uint32_t CTRL_TGL;                      /**< PDM Core Control Register                          */
  __IOM uint32_t CMD_TGL;                       /**< PDM Core Command Register                          */
  __IM uint32_t  STATUS_TGL;                    /**< PDM Status register                                */
  __IOM uint32_t CFG0_TGL;                      /**< PDM Core Configuration Register0                   */
  __IOM uint32_t CFG1_TGL;                      /**< PDM Core Configuration Register1                   */
  uint32_t       RESERVED12[1U];                /**< Reserved for future use                            */
  __IM uint32_t  RXDATA_TGL;                    /**< PDM Received Data Register                         */
  uint32_t       RESERVED13[7U];                /**< Reserved for future use                            */
  __IOM uint32_t IF_TGL;                        /**< Interrupt Flag Register                            */
  __IOM uint32_t IEN_TGL;                       /**< Interrupt Flag Register                            */
  uint32_t       RESERVED14[6U];                /**< Reserved for future use                            */
  __IM uint32_t  SYNCBUSY_TGL;                  /**< Synchronization Busy Register                      */
} PDM_TypeDef;
/** @} End of group EFR32MG22_PDM */

/**************************************************************************//**
 * @addtogroup EFR32MG22_PDM
 * @{
 * @defgroup EFR32MG22_PDM_BitFields PDM Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for PDM IPVERSION */
#define _PDM_IPVERSION_RESETVALUE              0x00000000UL                             /**< Default value for PDM_IPVERSION             */
#define _PDM_IPVERSION_MASK                    0xFFFFFFFFUL                             /**< Mask for PDM_IPVERSION                      */
#define _PDM_IPVERSION_IPVERSION_SHIFT         0                                        /**< Shift value for PDM_IPVERSION               */
#define _PDM_IPVERSION_IPVERSION_MASK          0xFFFFFFFFUL                             /**< Bit mask for PDM_IPVERSION                  */
#define _PDM_IPVERSION_IPVERSION_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for PDM_IPVERSION              */
#define PDM_IPVERSION_IPVERSION_DEFAULT        (_PDM_IPVERSION_IPVERSION_DEFAULT << 0)  /**< Shifted mode DEFAULT for PDM_IPVERSION      */

/* Bit fields for PDM EN */
#define _PDM_EN_RESETVALUE                     0x00000000UL                             /**< Default value for PDM_EN                    */
#define _PDM_EN_MASK                           0x00000001UL                             /**< Mask for PDM_EN                             */
#define PDM_EN_EN                              (0x1UL << 0)                             /**< PDM enable                                  */
#define _PDM_EN_EN_SHIFT                       0                                        /**< Shift value for PDM_EN                      */
#define _PDM_EN_EN_MASK                        0x1UL                                    /**< Bit mask for PDM_EN                         */
#define _PDM_EN_EN_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for PDM_EN                     */
#define _PDM_EN_EN_DISABLE                     0x00000000UL                             /**< Mode DISABLE for PDM_EN                     */
#define _PDM_EN_EN_ENABLE                      0x00000001UL                             /**< Mode ENABLE for PDM_EN                      */
#define PDM_EN_EN_DEFAULT                      (_PDM_EN_EN_DEFAULT << 0)                /**< Shifted mode DEFAULT for PDM_EN             */
#define PDM_EN_EN_DISABLE                      (_PDM_EN_EN_DISABLE << 0)                /**< Shifted mode DISABLE for PDM_EN             */
#define PDM_EN_EN_ENABLE                       (_PDM_EN_EN_ENABLE << 0)                 /**< Shifted mode ENABLE for PDM_EN              */

/* Bit fields for PDM CTRL */
#define _PDM_CTRL_RESETVALUE                   0x00000000UL                             /**< Default value for PDM_CTRL                  */
#define _PDM_CTRL_MASK                         0x000FFF1FUL                             /**< Mask for PDM_CTRL                           */
#define _PDM_CTRL_GAIN_SHIFT                   0                                        /**< Shift value for PDM_GAIN                    */
#define _PDM_CTRL_GAIN_MASK                    0x1FUL                                   /**< Bit mask for PDM_GAIN                       */
#define _PDM_CTRL_GAIN_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for PDM_CTRL                   */
#define PDM_CTRL_GAIN_DEFAULT                  (_PDM_CTRL_GAIN_DEFAULT << 0)            /**< Shifted mode DEFAULT for PDM_CTRL           */
#define _PDM_CTRL_DSR_SHIFT                    8                                        /**< Shift value for PDM_DSR                     */
#define _PDM_CTRL_DSR_MASK                     0xFFF00UL                                /**< Bit mask for PDM_DSR                        */
#define _PDM_CTRL_DSR_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for PDM_CTRL                   */
#define PDM_CTRL_DSR_DEFAULT                   (_PDM_CTRL_DSR_DEFAULT << 8)             /**< Shifted mode DEFAULT for PDM_CTRL           */

/* Bit fields for PDM CMD */
#define _PDM_CMD_RESETVALUE                    0x00000000UL                             /**< Default value for PDM_CMD                   */
#define _PDM_CMD_MASK                          0x00010111UL                             /**< Mask for PDM_CMD                            */
#define PDM_CMD_START                          (0x1UL << 0)                             /**< Start DCF                                   */
#define _PDM_CMD_START_SHIFT                   0                                        /**< Shift value for PDM_START                   */
#define _PDM_CMD_START_MASK                    0x1UL                                    /**< Bit mask for PDM_START                      */
#define _PDM_CMD_START_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for PDM_CMD                    */
#define PDM_CMD_START_DEFAULT                  (_PDM_CMD_START_DEFAULT << 0)            /**< Shifted mode DEFAULT for PDM_CMD            */
#define PDM_CMD_STOP                           (0x1UL << 4)                             /**< Stop DCF                                    */
#define _PDM_CMD_STOP_SHIFT                    4                                        /**< Shift value for PDM_STOP                    */
#define _PDM_CMD_STOP_MASK                     0x10UL                                   /**< Bit mask for PDM_STOP                       */
#define _PDM_CMD_STOP_DEFAULT                  0x00000000UL                             /**< Mode DEFAULT for PDM_CMD                    */
#define PDM_CMD_STOP_DEFAULT                   (_PDM_CMD_STOP_DEFAULT << 4)             /**< Shifted mode DEFAULT for PDM_CMD            */
#define PDM_CMD_CLEAR                          (0x1UL << 8)                             /**< Clear DCF                                   */
#define _PDM_CMD_CLEAR_SHIFT                   8                                        /**< Shift value for PDM_CLEAR                   */
#define _PDM_CMD_CLEAR_MASK                    0x100UL                                  /**< Bit mask for PDM_CLEAR                      */
#define _PDM_CMD_CLEAR_DEFAULT                 0x00000000UL                             /**< Mode DEFAULT for PDM_CMD                    */
#define PDM_CMD_CLEAR_DEFAULT                  (_PDM_CMD_CLEAR_DEFAULT << 8)            /**< Shifted mode DEFAULT for PDM_CMD            */
#define PDM_CMD_FIFOFL                         (0x1UL << 16)                            /**< FIFO Flush                                  */
#define _PDM_CMD_FIFOFL_SHIFT                  16                                       /**< Shift value for PDM_FIFOFL                  */
#define _PDM_CMD_FIFOFL_MASK                   0x10000UL                                /**< Bit mask for PDM_FIFOFL                     */
#define _PDM_CMD_FIFOFL_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for PDM_CMD                    */
#define PDM_CMD_FIFOFL_DEFAULT                 (_PDM_CMD_FIFOFL_DEFAULT << 16)          /**< Shifted mode DEFAULT for PDM_CMD            */

/* Bit fields for PDM STATUS */
#define _PDM_STATUS_RESETVALUE                 0x00000020UL                             /**< Default value for PDM_STATUS                */
#define _PDM_STATUS_MASK                       0x00000731UL                             /**< Mask for PDM_STATUS                         */
#define PDM_STATUS_ACT                         (0x1UL << 0)                             /**< PDM is active                               */
#define _PDM_STATUS_ACT_SHIFT                  0                                        /**< Shift value for PDM_ACT                     */
#define _PDM_STATUS_ACT_MASK                   0x1UL                                    /**< Bit mask for PDM_ACT                        */
#define _PDM_STATUS_ACT_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for PDM_STATUS                 */
#define PDM_STATUS_ACT_DEFAULT                 (_PDM_STATUS_ACT_DEFAULT << 0)           /**< Shifted mode DEFAULT for PDM_STATUS         */
#define PDM_STATUS_FULL                        (0x1UL << 4)                             /**< FIFO FULL Status                            */
#define _PDM_STATUS_FULL_SHIFT                 4                                        /**< Shift value for PDM_FULL                    */
#define _PDM_STATUS_FULL_MASK                  0x10UL                                   /**< Bit mask for PDM_FULL                       */
#define _PDM_STATUS_FULL_DEFAULT               0x00000000UL                             /**< Mode DEFAULT for PDM_STATUS                 */
#define PDM_STATUS_FULL_DEFAULT                (_PDM_STATUS_FULL_DEFAULT << 4)          /**< Shifted mode DEFAULT for PDM_STATUS         */
#define PDM_STATUS_EMPTY                       (0x1UL << 5)                             /**< FIFO EMPTY Status                           */
#define _PDM_STATUS_EMPTY_SHIFT                5                                        /**< Shift value for PDM_EMPTY                   */
#define _PDM_STATUS_EMPTY_MASK                 0x20UL                                   /**< Bit mask for PDM_EMPTY                      */
#define _PDM_STATUS_EMPTY_DEFAULT              0x00000001UL                             /**< Mode DEFAULT for PDM_STATUS                 */
#define PDM_STATUS_EMPTY_DEFAULT               (_PDM_STATUS_EMPTY_DEFAULT << 5)         /**< Shifted mode DEFAULT for PDM_STATUS         */
#define _PDM_STATUS_FIFOCNT_SHIFT              8                                        /**< Shift value for PDM_FIFOCNT                 */
#define _PDM_STATUS_FIFOCNT_MASK               0x700UL                                  /**< Bit mask for PDM_FIFOCNT                    */
#define _PDM_STATUS_FIFOCNT_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for PDM_STATUS                 */
#define PDM_STATUS_FIFOCNT_DEFAULT             (_PDM_STATUS_FIFOCNT_DEFAULT << 8)       /**< Shifted mode DEFAULT for PDM_STATUS         */

/* Bit fields for PDM CFG0 */
#define _PDM_CFG0_RESETVALUE                   0x00000000UL                                /**< Default value for PDM_CFG0                  */
#define _PDM_CFG0_MASK                         0x03013713UL                                /**< Mask for PDM_CFG0                           */
#define _PDM_CFG0_FORDER_SHIFT                 0                                           /**< Shift value for PDM_FORDER                  */
#define _PDM_CFG0_FORDER_MASK                  0x3UL                                       /**< Bit mask for PDM_FORDER                     */
#define _PDM_CFG0_FORDER_DEFAULT               0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0                   */
#define _PDM_CFG0_FORDER_SECOND                0x00000000UL                                /**< Mode SECOND for PDM_CFG0                    */
#define _PDM_CFG0_FORDER_THIRD                 0x00000001UL                                /**< Mode THIRD for PDM_CFG0                     */
#define _PDM_CFG0_FORDER_FOURTH                0x00000002UL                                /**< Mode FOURTH for PDM_CFG0                    */
#define _PDM_CFG0_FORDER_FIFTH                 0x00000003UL                                /**< Mode FIFTH for PDM_CFG0                     */
#define PDM_CFG0_FORDER_DEFAULT                (_PDM_CFG0_FORDER_DEFAULT << 0)             /**< Shifted mode DEFAULT for PDM_CFG0           */
#define PDM_CFG0_FORDER_SECOND                 (_PDM_CFG0_FORDER_SECOND << 0)              /**< Shifted mode SECOND for PDM_CFG0            */
#define PDM_CFG0_FORDER_THIRD                  (_PDM_CFG0_FORDER_THIRD << 0)               /**< Shifted mode THIRD for PDM_CFG0             */
#define PDM_CFG0_FORDER_FOURTH                 (_PDM_CFG0_FORDER_FOURTH << 0)              /**< Shifted mode FOURTH for PDM_CFG0            */
#define PDM_CFG0_FORDER_FIFTH                  (_PDM_CFG0_FORDER_FIFTH << 0)               /**< Shifted mode FIFTH for PDM_CFG0             */
#define PDM_CFG0_NUMCH                         (0x1UL << 4)                                /**< Number of Channels                          */
#define _PDM_CFG0_NUMCH_SHIFT                  4                                           /**< Shift value for PDM_NUMCH                   */
#define _PDM_CFG0_NUMCH_MASK                   0x10UL                                      /**< Bit mask for PDM_NUMCH                      */
#define _PDM_CFG0_NUMCH_DEFAULT                0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0                   */
#define _PDM_CFG0_NUMCH_ONE                    0x00000000UL                                /**< Mode ONE for PDM_CFG0                       */
#define _PDM_CFG0_NUMCH_TWO                    0x00000001UL                                /**< Mode TWO for PDM_CFG0                       */
#define PDM_CFG0_NUMCH_DEFAULT                 (_PDM_CFG0_NUMCH_DEFAULT << 4)              /**< Shifted mode DEFAULT for PDM_CFG0           */
#define PDM_CFG0_NUMCH_ONE                     (_PDM_CFG0_NUMCH_ONE << 4)                  /**< Shifted mode ONE for PDM_CFG0               */
#define PDM_CFG0_NUMCH_TWO                     (_PDM_CFG0_NUMCH_TWO << 4)                  /**< Shifted mode TWO for PDM_CFG0               */
#define _PDM_CFG0_DATAFORMAT_SHIFT             8                                           /**< Shift value for PDM_DATAFORMAT              */
#define _PDM_CFG0_DATAFORMAT_MASK              0x700UL                                     /**< Bit mask for PDM_DATAFORMAT                 */
#define _PDM_CFG0_DATAFORMAT_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0                   */
#define _PDM_CFG0_DATAFORMAT_RIGHT16           0x00000000UL                                /**< Mode RIGHT16 for PDM_CFG0                   */
#define _PDM_CFG0_DATAFORMAT_DOUBLE16          0x00000001UL                                /**< Mode DOUBLE16 for PDM_CFG0                  */
#define _PDM_CFG0_DATAFORMAT_RIGHT24           0x00000002UL                                /**< Mode RIGHT24 for PDM_CFG0                   */
#define _PDM_CFG0_DATAFORMAT_FULL32BIT         0x00000003UL                                /**< Mode FULL32BIT for PDM_CFG0                 */
#define _PDM_CFG0_DATAFORMAT_LEFT16            0x00000004UL                                /**< Mode LEFT16 for PDM_CFG0                    */
#define _PDM_CFG0_DATAFORMAT_LEFT24            0x00000005UL                                /**< Mode LEFT24 for PDM_CFG0                    */
#define _PDM_CFG0_DATAFORMAT_RAW32BIT          0x00000006UL                                /**< Mode RAW32BIT for PDM_CFG0                  */
#define PDM_CFG0_DATAFORMAT_DEFAULT            (_PDM_CFG0_DATAFORMAT_DEFAULT << 8)         /**< Shifted mode DEFAULT for PDM_CFG0           */
#define PDM_CFG0_DATAFORMAT_RIGHT16            (_PDM_CFG0_DATAFORMAT_RIGHT16 << 8)         /**< Shifted mode RIGHT16 for PDM_CFG0           */
#define PDM_CFG0_DATAFORMAT_DOUBLE16           (_PDM_CFG0_DATAFORMAT_DOUBLE16 << 8)        /**< Shifted mode DOUBLE16 for PDM_CFG0          */
#define PDM_CFG0_DATAFORMAT_RIGHT24            (_PDM_CFG0_DATAFORMAT_RIGHT24 << 8)         /**< Shifted mode RIGHT24 for PDM_CFG0           */
#define PDM_CFG0_DATAFORMAT_FULL32BIT          (_PDM_CFG0_DATAFORMAT_FULL32BIT << 8)       /**< Shifted mode FULL32BIT for PDM_CFG0         */
#define PDM_CFG0_DATAFORMAT_LEFT16             (_PDM_CFG0_DATAFORMAT_LEFT16 << 8)          /**< Shifted mode LEFT16 for PDM_CFG0            */
#define PDM_CFG0_DATAFORMAT_LEFT24             (_PDM_CFG0_DATAFORMAT_LEFT24 << 8)          /**< Shifted mode LEFT24 for PDM_CFG0            */
#define PDM_CFG0_DATAFORMAT_RAW32BIT           (_PDM_CFG0_DATAFORMAT_RAW32BIT << 8)        /**< Shifted mode RAW32BIT for PDM_CFG0          */
#define _PDM_CFG0_FIFODVL_SHIFT                12                                          /**< Shift value for PDM_FIFODVL                 */
#define _PDM_CFG0_FIFODVL_MASK                 0x3000UL                                    /**< Bit mask for PDM_FIFODVL                    */
#define _PDM_CFG0_FIFODVL_DEFAULT              0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0                   */
#define _PDM_CFG0_FIFODVL_ONE                  0x00000000UL                                /**< Mode ONE for PDM_CFG0                       */
#define _PDM_CFG0_FIFODVL_TWO                  0x00000001UL                                /**< Mode TWO for PDM_CFG0                       */
#define _PDM_CFG0_FIFODVL_THREE                0x00000002UL                                /**< Mode THREE for PDM_CFG0                     */
#define _PDM_CFG0_FIFODVL_FOUR                 0x00000003UL                                /**< Mode FOUR for PDM_CFG0                      */
#define PDM_CFG0_FIFODVL_DEFAULT               (_PDM_CFG0_FIFODVL_DEFAULT << 12)           /**< Shifted mode DEFAULT for PDM_CFG0           */
#define PDM_CFG0_FIFODVL_ONE                   (_PDM_CFG0_FIFODVL_ONE << 12)               /**< Shifted mode ONE for PDM_CFG0               */
#define PDM_CFG0_FIFODVL_TWO                   (_PDM_CFG0_FIFODVL_TWO << 12)               /**< Shifted mode TWO for PDM_CFG0               */
#define PDM_CFG0_FIFODVL_THREE                 (_PDM_CFG0_FIFODVL_THREE << 12)             /**< Shifted mode THREE for PDM_CFG0             */
#define PDM_CFG0_FIFODVL_FOUR                  (_PDM_CFG0_FIFODVL_FOUR << 12)              /**< Shifted mode FOUR for PDM_CFG0              */
#define PDM_CFG0_STEREOMODECH01                (0x1UL << 16)                               /**< Stereo mode CH01                            */
#define _PDM_CFG0_STEREOMODECH01_SHIFT         16                                          /**< Shift value for PDM_STEREOMODECH01          */
#define _PDM_CFG0_STEREOMODECH01_MASK          0x10000UL                                   /**< Bit mask for PDM_STEREOMODECH01             */
#define _PDM_CFG0_STEREOMODECH01_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0                   */
#define _PDM_CFG0_STEREOMODECH01_DISABLE       0x00000000UL                                /**< Mode DISABLE for PDM_CFG0                   */
#define _PDM_CFG0_STEREOMODECH01_CH01ENABLE    0x00000001UL                                /**< Mode CH01ENABLE for PDM_CFG0                */
#define PDM_CFG0_STEREOMODECH01_DEFAULT        (_PDM_CFG0_STEREOMODECH01_DEFAULT << 16)    /**< Shifted mode DEFAULT for PDM_CFG0           */
#define PDM_CFG0_STEREOMODECH01_DISABLE        (_PDM_CFG0_STEREOMODECH01_DISABLE << 16)    /**< Shifted mode DISABLE for PDM_CFG0           */
#define PDM_CFG0_STEREOMODECH01_CH01ENABLE     (_PDM_CFG0_STEREOMODECH01_CH01ENABLE << 16) /**< Shifted mode CH01ENABLE for PDM_CFG0        */
#define PDM_CFG0_CH0CLKPOL                     (0x1UL << 24)                               /**< CH0 CLK Polarity                            */
#define _PDM_CFG0_CH0CLKPOL_SHIFT              24                                          /**< Shift value for PDM_CH0CLKPOL               */
#define _PDM_CFG0_CH0CLKPOL_MASK               0x1000000UL                                 /**< Bit mask for PDM_CH0CLKPOL                  */
#define _PDM_CFG0_CH0CLKPOL_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0                   */
#define _PDM_CFG0_CH0CLKPOL_NORMAL             0x00000000UL                                /**< Mode NORMAL for PDM_CFG0                    */
#define _PDM_CFG0_CH0CLKPOL_INVERT             0x00000001UL                                /**< Mode INVERT for PDM_CFG0                    */
#define PDM_CFG0_CH0CLKPOL_DEFAULT             (_PDM_CFG0_CH0CLKPOL_DEFAULT << 24)         /**< Shifted mode DEFAULT for PDM_CFG0           */
#define PDM_CFG0_CH0CLKPOL_NORMAL              (_PDM_CFG0_CH0CLKPOL_NORMAL << 24)          /**< Shifted mode NORMAL for PDM_CFG0            */
#define PDM_CFG0_CH0CLKPOL_INVERT              (_PDM_CFG0_CH0CLKPOL_INVERT << 24)          /**< Shifted mode INVERT for PDM_CFG0            */
#define PDM_CFG0_CH1CLKPOL                     (0x1UL << 25)                               /**< CH1 CLK Polarity                            */
#define _PDM_CFG0_CH1CLKPOL_SHIFT              25                                          /**< Shift value for PDM_CH1CLKPOL               */
#define _PDM_CFG0_CH1CLKPOL_MASK               0x2000000UL                                 /**< Bit mask for PDM_CH1CLKPOL                  */
#define _PDM_CFG0_CH1CLKPOL_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for PDM_CFG0                   */
#define _PDM_CFG0_CH1CLKPOL_NORMAL             0x00000000UL                                /**< Mode NORMAL for PDM_CFG0                    */
#define _PDM_CFG0_CH1CLKPOL_INVERT             0x00000001UL                                /**< Mode INVERT for PDM_CFG0                    */
#define PDM_CFG0_CH1CLKPOL_DEFAULT             (_PDM_CFG0_CH1CLKPOL_DEFAULT << 25)         /**< Shifted mode DEFAULT for PDM_CFG0           */
#define PDM_CFG0_CH1CLKPOL_NORMAL              (_PDM_CFG0_CH1CLKPOL_NORMAL << 25)          /**< Shifted mode NORMAL for PDM_CFG0            */
#define PDM_CFG0_CH1CLKPOL_INVERT              (_PDM_CFG0_CH1CLKPOL_INVERT << 25)          /**< Shifted mode INVERT for PDM_CFG0            */

/* Bit fields for PDM CFG1 */
#define _PDM_CFG1_RESETVALUE                   0x00000000UL                             /**< Default value for PDM_CFG1                  */
#define _PDM_CFG1_MASK                         0x030003FFUL                             /**< Mask for PDM_CFG1                           */
#define _PDM_CFG1_PRESC_SHIFT                  0                                        /**< Shift value for PDM_PRESC                   */
#define _PDM_CFG1_PRESC_MASK                   0x3FFUL                                  /**< Bit mask for PDM_PRESC                      */
#define _PDM_CFG1_PRESC_DEFAULT                0x00000000UL                             /**< Mode DEFAULT for PDM_CFG1                   */
#define PDM_CFG1_PRESC_DEFAULT                 (_PDM_CFG1_PRESC_DEFAULT << 0)           /**< Shifted mode DEFAULT for PDM_CFG1           */
#define _PDM_CFG1_DLYMUXSEL_SHIFT              24                                       /**< Shift value for PDM_DLYMUXSEL               */
#define _PDM_CFG1_DLYMUXSEL_MASK               0x3000000UL                              /**< Bit mask for PDM_DLYMUXSEL                  */
#define _PDM_CFG1_DLYMUXSEL_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for PDM_CFG1                   */
#define PDM_CFG1_DLYMUXSEL_DEFAULT             (_PDM_CFG1_DLYMUXSEL_DEFAULT << 24)      /**< Shifted mode DEFAULT for PDM_CFG1           */

/* Bit fields for PDM RXDATA */
#define _PDM_RXDATA_RESETVALUE                 0x00000000UL                             /**< Default value for PDM_RXDATA                */
#define _PDM_RXDATA_MASK                       0xFFFFFFFFUL                             /**< Mask for PDM_RXDATA                         */
#define _PDM_RXDATA_RXDATA_SHIFT               0                                        /**< Shift value for PDM_RXDATA                  */
#define _PDM_RXDATA_RXDATA_MASK                0xFFFFFFFFUL                             /**< Bit mask for PDM_RXDATA                     */
#define _PDM_RXDATA_RXDATA_DEFAULT             0x00000000UL                             /**< Mode DEFAULT for PDM_RXDATA                 */
#define PDM_RXDATA_RXDATA_DEFAULT              (_PDM_RXDATA_RXDATA_DEFAULT << 0)        /**< Shifted mode DEFAULT for PDM_RXDATA         */

/* Bit fields for PDM IF */
#define _PDM_IF_RESETVALUE                     0x00000000UL                             /**< Default value for PDM_IF                    */
#define _PDM_IF_MASK                           0x0000000FUL                             /**< Mask for PDM_IF                             */
#define PDM_IF_DV                              (0x1UL << 0)                             /**< Data Valid Interrupt Flag                   */
#define _PDM_IF_DV_SHIFT                       0                                        /**< Shift value for PDM_DV                      */
#define _PDM_IF_DV_MASK                        0x1UL                                    /**< Bit mask for PDM_DV                         */
#define _PDM_IF_DV_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for PDM_IF                     */
#define PDM_IF_DV_DEFAULT                      (_PDM_IF_DV_DEFAULT << 0)                /**< Shifted mode DEFAULT for PDM_IF             */
#define PDM_IF_DVL                             (0x1UL << 1)                             /**< Data Valid Level Interrupt Flag             */
#define _PDM_IF_DVL_SHIFT                      1                                        /**< Shift value for PDM_DVL                     */
#define _PDM_IF_DVL_MASK                       0x2UL                                    /**< Bit mask for PDM_DVL                        */
#define _PDM_IF_DVL_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for PDM_IF                     */
#define PDM_IF_DVL_DEFAULT                     (_PDM_IF_DVL_DEFAULT << 1)               /**< Shifted mode DEFAULT for PDM_IF             */
#define PDM_IF_OF                              (0x1UL << 2)                             /**< FIFO Overflow Interrupt Flag                */
#define _PDM_IF_OF_SHIFT                       2                                        /**< Shift value for PDM_OF                      */
#define _PDM_IF_OF_MASK                        0x4UL                                    /**< Bit mask for PDM_OF                         */
#define _PDM_IF_OF_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for PDM_IF                     */
#define PDM_IF_OF_DEFAULT                      (_PDM_IF_OF_DEFAULT << 2)                /**< Shifted mode DEFAULT for PDM_IF             */
#define PDM_IF_UF                              (0x1UL << 3)                             /**< FIFO Undeflow Interrupt Flag                */
#define _PDM_IF_UF_SHIFT                       3                                        /**< Shift value for PDM_UF                      */
#define _PDM_IF_UF_MASK                        0x8UL                                    /**< Bit mask for PDM_UF                         */
#define _PDM_IF_UF_DEFAULT                     0x00000000UL                             /**< Mode DEFAULT for PDM_IF                     */
#define PDM_IF_UF_DEFAULT                      (_PDM_IF_UF_DEFAULT << 3)                /**< Shifted mode DEFAULT for PDM_IF             */

/* Bit fields for PDM IEN */
#define _PDM_IEN_RESETVALUE                    0x00000000UL                             /**< Default value for PDM_IEN                   */
#define _PDM_IEN_MASK                          0x0000000FUL                             /**< Mask for PDM_IEN                            */
#define PDM_IEN_DV                             (0x1UL << 0)                             /**< Data Valid Interrupt Enable                 */
#define _PDM_IEN_DV_SHIFT                      0                                        /**< Shift value for PDM_DV                      */
#define _PDM_IEN_DV_MASK                       0x1UL                                    /**< Bit mask for PDM_DV                         */
#define _PDM_IEN_DV_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for PDM_IEN                    */
#define PDM_IEN_DV_DEFAULT                     (_PDM_IEN_DV_DEFAULT << 0)               /**< Shifted mode DEFAULT for PDM_IEN            */
#define PDM_IEN_DVL                            (0x1UL << 1)                             /**< Data Valid Level Interrupt Enable           */
#define _PDM_IEN_DVL_SHIFT                     1                                        /**< Shift value for PDM_DVL                     */
#define _PDM_IEN_DVL_MASK                      0x2UL                                    /**< Bit mask for PDM_DVL                        */
#define _PDM_IEN_DVL_DEFAULT                   0x00000000UL                             /**< Mode DEFAULT for PDM_IEN                    */
#define PDM_IEN_DVL_DEFAULT                    (_PDM_IEN_DVL_DEFAULT << 1)              /**< Shifted mode DEFAULT for PDM_IEN            */
#define PDM_IEN_OF                             (0x1UL << 2)                             /**< FIFO Overflow Interrupt Enable              */
#define _PDM_IEN_OF_SHIFT                      2                                        /**< Shift value for PDM_OF                      */
#define _PDM_IEN_OF_MASK                       0x4UL                                    /**< Bit mask for PDM_OF                         */
#define _PDM_IEN_OF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for PDM_IEN                    */
#define PDM_IEN_OF_DEFAULT                     (_PDM_IEN_OF_DEFAULT << 2)               /**< Shifted mode DEFAULT for PDM_IEN            */
#define PDM_IEN_UF                             (0x1UL << 3)                             /**< FIFO Undeflow Interrupt Enable              */
#define _PDM_IEN_UF_SHIFT                      3                                        /**< Shift value for PDM_UF                      */
#define _PDM_IEN_UF_MASK                       0x8UL                                    /**< Bit mask for PDM_UF                         */
#define _PDM_IEN_UF_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for PDM_IEN                    */
#define PDM_IEN_UF_DEFAULT                     (_PDM_IEN_UF_DEFAULT << 3)               /**< Shifted mode DEFAULT for PDM_IEN            */

/* Bit fields for PDM SYNCBUSY */
#define _PDM_SYNCBUSY_RESETVALUE               0x00000000UL                             /**< Default value for PDM_SYNCBUSY              */
#define _PDM_SYNCBUSY_MASK                     0x00000009UL                             /**< Mask for PDM_SYNCBUSY                       */
#define PDM_SYNCBUSY_SYNCBUSY                  (0x1UL << 0)                             /**< sync busy                                   */
#define _PDM_SYNCBUSY_SYNCBUSY_SHIFT           0                                        /**< Shift value for PDM_SYNCBUSY                */
#define _PDM_SYNCBUSY_SYNCBUSY_MASK            0x1UL                                    /**< Bit mask for PDM_SYNCBUSY                   */
#define _PDM_SYNCBUSY_SYNCBUSY_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for PDM_SYNCBUSY               */
#define PDM_SYNCBUSY_SYNCBUSY_DEFAULT          (_PDM_SYNCBUSY_SYNCBUSY_DEFAULT << 0)    /**< Shifted mode DEFAULT for PDM_SYNCBUSY       */
#define PDM_SYNCBUSY_FIFOFLBUSY                (0x1UL << 3)                             /**< FIFO Flush Sync busy                        */
#define _PDM_SYNCBUSY_FIFOFLBUSY_SHIFT         3                                        /**< Shift value for PDM_FIFOFLBUSY              */
#define _PDM_SYNCBUSY_FIFOFLBUSY_MASK          0x8UL                                    /**< Bit mask for PDM_FIFOFLBUSY                 */
#define _PDM_SYNCBUSY_FIFOFLBUSY_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for PDM_SYNCBUSY               */
#define PDM_SYNCBUSY_FIFOFLBUSY_DEFAULT        (_PDM_SYNCBUSY_FIFOFLBUSY_DEFAULT << 3)  /**< Shifted mode DEFAULT for PDM_SYNCBUSY       */

/** @} End of group EFR32MG22_PDM_BitFields */
/** @} End of group EFR32MG22_PDM */
/** @} End of group Parts */
#endif /* EFR32MG22_PDM_H */
