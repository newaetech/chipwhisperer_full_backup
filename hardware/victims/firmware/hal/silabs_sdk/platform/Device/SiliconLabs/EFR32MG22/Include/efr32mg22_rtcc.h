/**************************************************************************//**
 * @file
 * @brief EFR32MG22 RTCC register and bit field definitions
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
#ifndef EFR32MG22_RTCC_H
#define EFR32MG22_RTCC_H
#define RTCC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG22_RTCC RTCC
 * @{
 * @brief EFR32MG22 RTCC Register Declaration.
 *****************************************************************************/

/** RTCC CC Register Group Declaration. */
typedef struct {
  __IOM uint32_t CTRL;                               /**< CC Channel Control Register                        */
  __IOM uint32_t OCVALUE;                            /**< Output Compare Value Register                      */
  __IM uint32_t  ICVALUE;                            /**< Input Capture Value Register                       */
} RTCC_CC_TypeDef;

/** RTCC Register Declaration. */
typedef struct {
  __IM uint32_t   IPVERSION;                    /**< IP VERSION                                         */
  __IOM uint32_t  EN;                           /**< Module Enable Register                             */
  __IOM uint32_t  CFG;                          /**< Configuration Register                             */
  __IOM uint32_t  CMD;                          /**< Command Register                                   */
  __IM uint32_t   STATUS;                       /**< Status register                                    */
  __IOM uint32_t  IF;                           /**< RTCC Interrupt Flags                               */
  __IOM uint32_t  IEN;                          /**< Interrupt Enable Register                          */
  __IOM uint32_t  PRECNT;                       /**< Pre-Counter Value Register                         */
  __IOM uint32_t  CNT;                          /**< Counter Value Register                             */
  __IM uint32_t   COMBCNT;                      /**< Combined Pre-Counter and Counter Valu...           */
  __IM uint32_t   SYNCBUSY;                     /**< Synchronization Busy Register                      */
  __IOM uint32_t  LOCK;                         /**< Configuration Lock Register                        */
  RTCC_CC_TypeDef CC[3U];                       /**< Capture/Compare Channel                            */
  uint32_t        RESERVED0[1003U];             /**< Reserved for future use                            */
  __IM uint32_t   IPVERSION_SET;                /**< IP VERSION                                         */
  __IOM uint32_t  EN_SET;                       /**< Module Enable Register                             */
  __IOM uint32_t  CFG_SET;                      /**< Configuration Register                             */
  __IOM uint32_t  CMD_SET;                      /**< Command Register                                   */
  __IM uint32_t   STATUS_SET;                   /**< Status register                                    */
  __IOM uint32_t  IF_SET;                       /**< RTCC Interrupt Flags                               */
  __IOM uint32_t  IEN_SET;                      /**< Interrupt Enable Register                          */
  __IOM uint32_t  PRECNT_SET;                   /**< Pre-Counter Value Register                         */
  __IOM uint32_t  CNT_SET;                      /**< Counter Value Register                             */
  __IM uint32_t   COMBCNT_SET;                  /**< Combined Pre-Counter and Counter Valu...           */
  __IM uint32_t   SYNCBUSY_SET;                 /**< Synchronization Busy Register                      */
  __IOM uint32_t  LOCK_SET;                     /**< Configuration Lock Register                        */
  RTCC_CC_TypeDef CC_SET[3U];                   /**< Capture/Compare Channel                            */
  uint32_t        RESERVED1[1003U];             /**< Reserved for future use                            */
  __IM uint32_t   IPVERSION_CLR;                /**< IP VERSION                                         */
  __IOM uint32_t  EN_CLR;                       /**< Module Enable Register                             */
  __IOM uint32_t  CFG_CLR;                      /**< Configuration Register                             */
  __IOM uint32_t  CMD_CLR;                      /**< Command Register                                   */
  __IM uint32_t   STATUS_CLR;                   /**< Status register                                    */
  __IOM uint32_t  IF_CLR;                       /**< RTCC Interrupt Flags                               */
  __IOM uint32_t  IEN_CLR;                      /**< Interrupt Enable Register                          */
  __IOM uint32_t  PRECNT_CLR;                   /**< Pre-Counter Value Register                         */
  __IOM uint32_t  CNT_CLR;                      /**< Counter Value Register                             */
  __IM uint32_t   COMBCNT_CLR;                  /**< Combined Pre-Counter and Counter Valu...           */
  __IM uint32_t   SYNCBUSY_CLR;                 /**< Synchronization Busy Register                      */
  __IOM uint32_t  LOCK_CLR;                     /**< Configuration Lock Register                        */
  RTCC_CC_TypeDef CC_CLR[3U];                   /**< Capture/Compare Channel                            */
  uint32_t        RESERVED2[1003U];             /**< Reserved for future use                            */
  __IM uint32_t   IPVERSION_TGL;                /**< IP VERSION                                         */
  __IOM uint32_t  EN_TGL;                       /**< Module Enable Register                             */
  __IOM uint32_t  CFG_TGL;                      /**< Configuration Register                             */
  __IOM uint32_t  CMD_TGL;                      /**< Command Register                                   */
  __IM uint32_t   STATUS_TGL;                   /**< Status register                                    */
  __IOM uint32_t  IF_TGL;                       /**< RTCC Interrupt Flags                               */
  __IOM uint32_t  IEN_TGL;                      /**< Interrupt Enable Register                          */
  __IOM uint32_t  PRECNT_TGL;                   /**< Pre-Counter Value Register                         */
  __IOM uint32_t  CNT_TGL;                      /**< Counter Value Register                             */
  __IM uint32_t   COMBCNT_TGL;                  /**< Combined Pre-Counter and Counter Valu...           */
  __IM uint32_t   SYNCBUSY_TGL;                 /**< Synchronization Busy Register                      */
  __IOM uint32_t  LOCK_TGL;                     /**< Configuration Lock Register                        */
  RTCC_CC_TypeDef CC_TGL[3U];                   /**< Capture/Compare Channel                            */
} RTCC_TypeDef;
/** @} End of group EFR32MG22_RTCC */

/**************************************************************************//**
 * @addtogroup EFR32MG22_RTCC
 * @{
 * @defgroup EFR32MG22_RTCC_BitFields RTCC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for RTCC IPVERSION */
#define _RTCC_IPVERSION_RESETVALUE              0x00000001UL                             /**< Default value for RTCC_IPVERSION            */
#define _RTCC_IPVERSION_MASK                    0xFFFFFFFFUL                             /**< Mask for RTCC_IPVERSION                     */
#define _RTCC_IPVERSION_IPVERSION_SHIFT         0                                        /**< Shift value for RTCC_IPVERSION              */
#define _RTCC_IPVERSION_IPVERSION_MASK          0xFFFFFFFFUL                             /**< Bit mask for RTCC_IPVERSION                 */
#define _RTCC_IPVERSION_IPVERSION_DEFAULT       0x00000001UL                             /**< Mode DEFAULT for RTCC_IPVERSION             */
#define RTCC_IPVERSION_IPVERSION_DEFAULT        (_RTCC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for RTCC_IPVERSION     */

/* Bit fields for RTCC EN */
#define _RTCC_EN_RESETVALUE                     0x00000000UL                            /**< Default value for RTCC_EN                   */
#define _RTCC_EN_MASK                           0x00000001UL                            /**< Mask for RTCC_EN                            */
#define RTCC_EN_EN                              (0x1UL << 0)                            /**< RTCC Enable                                 */
#define _RTCC_EN_EN_SHIFT                       0                                       /**< Shift value for RTCC_EN                     */
#define _RTCC_EN_EN_MASK                        0x1UL                                   /**< Bit mask for RTCC_EN                        */
#define _RTCC_EN_EN_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for RTCC_EN                    */
#define RTCC_EN_EN_DEFAULT                      (_RTCC_EN_EN_DEFAULT << 0)              /**< Shifted mode DEFAULT for RTCC_EN            */

/* Bit fields for RTCC CFG */
#define _RTCC_CFG_RESETVALUE                    0x00000000UL                            /**< Default value for RTCC_CFG                  */
#define _RTCC_CFG_MASK                          0x000000FFUL                            /**< Mask for RTCC_CFG                           */
#define RTCC_CFG_DEBUGRUN                       (0x1UL << 0)                            /**< Debug Mode Run Enable                       */
#define _RTCC_CFG_DEBUGRUN_SHIFT                0                                       /**< Shift value for RTCC_DEBUGRUN               */
#define _RTCC_CFG_DEBUGRUN_MASK                 0x1UL                                   /**< Bit mask for RTCC_DEBUGRUN                  */
#define _RTCC_CFG_DEBUGRUN_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for RTCC_CFG                   */
#define _RTCC_CFG_DEBUGRUN_X0                   0x00000000UL                            /**< Mode X0 for RTCC_CFG                        */
#define _RTCC_CFG_DEBUGRUN_X1                   0x00000001UL                            /**< Mode X1 for RTCC_CFG                        */
#define RTCC_CFG_DEBUGRUN_DEFAULT               (_RTCC_CFG_DEBUGRUN_DEFAULT << 0)       /**< Shifted mode DEFAULT for RTCC_CFG           */
#define RTCC_CFG_DEBUGRUN_X0                    (_RTCC_CFG_DEBUGRUN_X0 << 0)            /**< Shifted mode X0 for RTCC_CFG                */
#define RTCC_CFG_DEBUGRUN_X1                    (_RTCC_CFG_DEBUGRUN_X1 << 0)            /**< Shifted mode X1 for RTCC_CFG                */
#define RTCC_CFG_PRECNTCCV0TOP                  (0x1UL << 1)                            /**< Pre-counter CCV0 top value enable.          */
#define _RTCC_CFG_PRECNTCCV0TOP_SHIFT           1                                       /**< Shift value for RTCC_PRECNTCCV0TOP          */
#define _RTCC_CFG_PRECNTCCV0TOP_MASK            0x2UL                                   /**< Bit mask for RTCC_PRECNTCCV0TOP             */
#define _RTCC_CFG_PRECNTCCV0TOP_DEFAULT         0x00000000UL                            /**< Mode DEFAULT for RTCC_CFG                   */
#define RTCC_CFG_PRECNTCCV0TOP_DEFAULT          (_RTCC_CFG_PRECNTCCV0TOP_DEFAULT << 1)  /**< Shifted mode DEFAULT for RTCC_CFG           */
#define RTCC_CFG_CNTCCV1TOP                     (0x1UL << 2)                            /**< CCV1 top value enable                       */
#define _RTCC_CFG_CNTCCV1TOP_SHIFT              2                                       /**< Shift value for RTCC_CNTCCV1TOP             */
#define _RTCC_CFG_CNTCCV1TOP_MASK               0x4UL                                   /**< Bit mask for RTCC_CNTCCV1TOP                */
#define _RTCC_CFG_CNTCCV1TOP_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for RTCC_CFG                   */
#define RTCC_CFG_CNTCCV1TOP_DEFAULT             (_RTCC_CFG_CNTCCV1TOP_DEFAULT << 2)     /**< Shifted mode DEFAULT for RTCC_CFG           */
#define RTCC_CFG_CNTTICK                        (0x1UL << 3)                            /**< Counter prescaler mode.                     */
#define _RTCC_CFG_CNTTICK_SHIFT                 3                                       /**< Shift value for RTCC_CNTTICK                */
#define _RTCC_CFG_CNTTICK_MASK                  0x8UL                                   /**< Bit mask for RTCC_CNTTICK                   */
#define _RTCC_CFG_CNTTICK_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for RTCC_CFG                   */
#define _RTCC_CFG_CNTTICK_PRESC                 0x00000000UL                            /**< Mode PRESC for RTCC_CFG                     */
#define _RTCC_CFG_CNTTICK_CCV0MATCH             0x00000001UL                            /**< Mode CCV0MATCH for RTCC_CFG                 */
#define RTCC_CFG_CNTTICK_DEFAULT                (_RTCC_CFG_CNTTICK_DEFAULT << 3)        /**< Shifted mode DEFAULT for RTCC_CFG           */
#define RTCC_CFG_CNTTICK_PRESC                  (_RTCC_CFG_CNTTICK_PRESC << 3)          /**< Shifted mode PRESC for RTCC_CFG             */
#define RTCC_CFG_CNTTICK_CCV0MATCH              (_RTCC_CFG_CNTTICK_CCV0MATCH << 3)      /**< Shifted mode CCV0MATCH for RTCC_CFG         */
#define _RTCC_CFG_CNTPRESC_SHIFT                4                                       /**< Shift value for RTCC_CNTPRESC               */
#define _RTCC_CFG_CNTPRESC_MASK                 0xF0UL                                  /**< Bit mask for RTCC_CNTPRESC                  */
#define _RTCC_CFG_CNTPRESC_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for RTCC_CFG                   */
#define _RTCC_CFG_CNTPRESC_DIV1                 0x00000000UL                            /**< Mode DIV1 for RTCC_CFG                      */
#define _RTCC_CFG_CNTPRESC_DIV2                 0x00000001UL                            /**< Mode DIV2 for RTCC_CFG                      */
#define _RTCC_CFG_CNTPRESC_DIV4                 0x00000002UL                            /**< Mode DIV4 for RTCC_CFG                      */
#define _RTCC_CFG_CNTPRESC_DIV8                 0x00000003UL                            /**< Mode DIV8 for RTCC_CFG                      */
#define _RTCC_CFG_CNTPRESC_DIV16                0x00000004UL                            /**< Mode DIV16 for RTCC_CFG                     */
#define _RTCC_CFG_CNTPRESC_DIV32                0x00000005UL                            /**< Mode DIV32 for RTCC_CFG                     */
#define _RTCC_CFG_CNTPRESC_DIV64                0x00000006UL                            /**< Mode DIV64 for RTCC_CFG                     */
#define _RTCC_CFG_CNTPRESC_DIV128               0x00000007UL                            /**< Mode DIV128 for RTCC_CFG                    */
#define _RTCC_CFG_CNTPRESC_DIV256               0x00000008UL                            /**< Mode DIV256 for RTCC_CFG                    */
#define _RTCC_CFG_CNTPRESC_DIV512               0x00000009UL                            /**< Mode DIV512 for RTCC_CFG                    */
#define _RTCC_CFG_CNTPRESC_DIV1024              0x0000000AUL                            /**< Mode DIV1024 for RTCC_CFG                   */
#define _RTCC_CFG_CNTPRESC_DIV2048              0x0000000BUL                            /**< Mode DIV2048 for RTCC_CFG                   */
#define _RTCC_CFG_CNTPRESC_DIV4096              0x0000000CUL                            /**< Mode DIV4096 for RTCC_CFG                   */
#define _RTCC_CFG_CNTPRESC_DIV8192              0x0000000DUL                            /**< Mode DIV8192 for RTCC_CFG                   */
#define _RTCC_CFG_CNTPRESC_DIV16384             0x0000000EUL                            /**< Mode DIV16384 for RTCC_CFG                  */
#define _RTCC_CFG_CNTPRESC_DIV32768             0x0000000FUL                            /**< Mode DIV32768 for RTCC_CFG                  */
#define RTCC_CFG_CNTPRESC_DEFAULT               (_RTCC_CFG_CNTPRESC_DEFAULT << 4)       /**< Shifted mode DEFAULT for RTCC_CFG           */
#define RTCC_CFG_CNTPRESC_DIV1                  (_RTCC_CFG_CNTPRESC_DIV1 << 4)          /**< Shifted mode DIV1 for RTCC_CFG              */
#define RTCC_CFG_CNTPRESC_DIV2                  (_RTCC_CFG_CNTPRESC_DIV2 << 4)          /**< Shifted mode DIV2 for RTCC_CFG              */
#define RTCC_CFG_CNTPRESC_DIV4                  (_RTCC_CFG_CNTPRESC_DIV4 << 4)          /**< Shifted mode DIV4 for RTCC_CFG              */
#define RTCC_CFG_CNTPRESC_DIV8                  (_RTCC_CFG_CNTPRESC_DIV8 << 4)          /**< Shifted mode DIV8 for RTCC_CFG              */
#define RTCC_CFG_CNTPRESC_DIV16                 (_RTCC_CFG_CNTPRESC_DIV16 << 4)         /**< Shifted mode DIV16 for RTCC_CFG             */
#define RTCC_CFG_CNTPRESC_DIV32                 (_RTCC_CFG_CNTPRESC_DIV32 << 4)         /**< Shifted mode DIV32 for RTCC_CFG             */
#define RTCC_CFG_CNTPRESC_DIV64                 (_RTCC_CFG_CNTPRESC_DIV64 << 4)         /**< Shifted mode DIV64 for RTCC_CFG             */
#define RTCC_CFG_CNTPRESC_DIV128                (_RTCC_CFG_CNTPRESC_DIV128 << 4)        /**< Shifted mode DIV128 for RTCC_CFG            */
#define RTCC_CFG_CNTPRESC_DIV256                (_RTCC_CFG_CNTPRESC_DIV256 << 4)        /**< Shifted mode DIV256 for RTCC_CFG            */
#define RTCC_CFG_CNTPRESC_DIV512                (_RTCC_CFG_CNTPRESC_DIV512 << 4)        /**< Shifted mode DIV512 for RTCC_CFG            */
#define RTCC_CFG_CNTPRESC_DIV1024               (_RTCC_CFG_CNTPRESC_DIV1024 << 4)       /**< Shifted mode DIV1024 for RTCC_CFG           */
#define RTCC_CFG_CNTPRESC_DIV2048               (_RTCC_CFG_CNTPRESC_DIV2048 << 4)       /**< Shifted mode DIV2048 for RTCC_CFG           */
#define RTCC_CFG_CNTPRESC_DIV4096               (_RTCC_CFG_CNTPRESC_DIV4096 << 4)       /**< Shifted mode DIV4096 for RTCC_CFG           */
#define RTCC_CFG_CNTPRESC_DIV8192               (_RTCC_CFG_CNTPRESC_DIV8192 << 4)       /**< Shifted mode DIV8192 for RTCC_CFG           */
#define RTCC_CFG_CNTPRESC_DIV16384              (_RTCC_CFG_CNTPRESC_DIV16384 << 4)      /**< Shifted mode DIV16384 for RTCC_CFG          */
#define RTCC_CFG_CNTPRESC_DIV32768              (_RTCC_CFG_CNTPRESC_DIV32768 << 4)      /**< Shifted mode DIV32768 for RTCC_CFG          */

/* Bit fields for RTCC CMD */
#define _RTCC_CMD_RESETVALUE                    0x00000000UL                            /**< Default value for RTCC_CMD                  */
#define _RTCC_CMD_MASK                          0x00000003UL                            /**< Mask for RTCC_CMD                           */
#define RTCC_CMD_START                          (0x1UL << 0)                            /**< Start RTCC main counter                     */
#define _RTCC_CMD_START_SHIFT                   0                                       /**< Shift value for RTCC_START                  */
#define _RTCC_CMD_START_MASK                    0x1UL                                   /**< Bit mask for RTCC_START                     */
#define _RTCC_CMD_START_DEFAULT                 0x00000000UL                            /**< Mode DEFAULT for RTCC_CMD                   */
#define RTCC_CMD_START_DEFAULT                  (_RTCC_CMD_START_DEFAULT << 0)          /**< Shifted mode DEFAULT for RTCC_CMD           */
#define RTCC_CMD_STOP                           (0x1UL << 1)                            /**< Stop RTCC main counter                      */
#define _RTCC_CMD_STOP_SHIFT                    1                                       /**< Shift value for RTCC_STOP                   */
#define _RTCC_CMD_STOP_MASK                     0x2UL                                   /**< Bit mask for RTCC_STOP                      */
#define _RTCC_CMD_STOP_DEFAULT                  0x00000000UL                            /**< Mode DEFAULT for RTCC_CMD                   */
#define RTCC_CMD_STOP_DEFAULT                   (_RTCC_CMD_STOP_DEFAULT << 1)           /**< Shifted mode DEFAULT for RTCC_CMD           */

/* Bit fields for RTCC STATUS */
#define _RTCC_STATUS_RESETVALUE                 0x00000000UL                                /**< Default value for RTCC_STATUS               */
#define _RTCC_STATUS_MASK                       0x00000003UL                                /**< Mask for RTCC_STATUS                        */
#define RTCC_STATUS_RUNNING                     (0x1UL << 0)                                /**< RTCC running status                         */
#define _RTCC_STATUS_RUNNING_SHIFT              0                                           /**< Shift value for RTCC_RUNNING                */
#define _RTCC_STATUS_RUNNING_MASK               0x1UL                                       /**< Bit mask for RTCC_RUNNING                   */
#define _RTCC_STATUS_RUNNING_DEFAULT            0x00000000UL                                /**< Mode DEFAULT for RTCC_STATUS                */
#define RTCC_STATUS_RUNNING_DEFAULT             (_RTCC_STATUS_RUNNING_DEFAULT << 0)         /**< Shifted mode DEFAULT for RTCC_STATUS        */
#define RTCC_STATUS_RTCCLOCKSTATUS              (0x1UL << 1)                                /**< Lock Status                                 */
#define _RTCC_STATUS_RTCCLOCKSTATUS_SHIFT       1                                           /**< Shift value for RTCC_RTCCLOCKSTATUS         */
#define _RTCC_STATUS_RTCCLOCKSTATUS_MASK        0x2UL                                       /**< Bit mask for RTCC_RTCCLOCKSTATUS            */
#define _RTCC_STATUS_RTCCLOCKSTATUS_DEFAULT     0x00000000UL                                /**< Mode DEFAULT for RTCC_STATUS                */
#define _RTCC_STATUS_RTCCLOCKSTATUS_UNLOCKED    0x00000000UL                                /**< Mode UNLOCKED for RTCC_STATUS               */
#define _RTCC_STATUS_RTCCLOCKSTATUS_LOCKED      0x00000001UL                                /**< Mode LOCKED for RTCC_STATUS                 */
#define RTCC_STATUS_RTCCLOCKSTATUS_DEFAULT      (_RTCC_STATUS_RTCCLOCKSTATUS_DEFAULT << 1)  /**< Shifted mode DEFAULT for RTCC_STATUS        */
#define RTCC_STATUS_RTCCLOCKSTATUS_UNLOCKED     (_RTCC_STATUS_RTCCLOCKSTATUS_UNLOCKED << 1) /**< Shifted mode UNLOCKED for RTCC_STATUS       */
#define RTCC_STATUS_RTCCLOCKSTATUS_LOCKED       (_RTCC_STATUS_RTCCLOCKSTATUS_LOCKED << 1)   /**< Shifted mode LOCKED for RTCC_STATUS         */

/* Bit fields for RTCC IF */
#define _RTCC_IF_RESETVALUE                     0x00000000UL                            /**< Default value for RTCC_IF                   */
#define _RTCC_IF_MASK                           0x000003FFUL                            /**< Mask for RTCC_IF                            */
#define RTCC_IF_OF                              (0x1UL << 0)                            /**< Overflow Interrupt Flag                     */
#define _RTCC_IF_OF_SHIFT                       0                                       /**< Shift value for RTCC_OF                     */
#define _RTCC_IF_OF_MASK                        0x1UL                                   /**< Bit mask for RTCC_OF                        */
#define _RTCC_IF_OF_DEFAULT                     0x00000000UL                            /**< Mode DEFAULT for RTCC_IF                    */
#define RTCC_IF_OF_DEFAULT                      (_RTCC_IF_OF_DEFAULT << 0)              /**< Shifted mode DEFAULT for RTCC_IF            */
#define RTCC_IF_CNTTICK                         (0x1UL << 1)                            /**< Main counter tick                           */
#define _RTCC_IF_CNTTICK_SHIFT                  1                                       /**< Shift value for RTCC_CNTTICK                */
#define _RTCC_IF_CNTTICK_MASK                   0x2UL                                   /**< Bit mask for RTCC_CNTTICK                   */
#define _RTCC_IF_CNTTICK_DEFAULT                0x00000000UL                            /**< Mode DEFAULT for RTCC_IF                    */
#define RTCC_IF_CNTTICK_DEFAULT                 (_RTCC_IF_CNTTICK_DEFAULT << 1)         /**< Shifted mode DEFAULT for RTCC_IF            */
#define RTCC_IF_CC0                             (0x1UL << 4)                            /**< CC Channel n Interrupt Flag                 */
#define _RTCC_IF_CC0_SHIFT                      4                                       /**< Shift value for RTCC_CC0                    */
#define _RTCC_IF_CC0_MASK                       0x10UL                                  /**< Bit mask for RTCC_CC0                       */
#define _RTCC_IF_CC0_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for RTCC_IF                    */
#define RTCC_IF_CC0_DEFAULT                     (_RTCC_IF_CC0_DEFAULT << 4)             /**< Shifted mode DEFAULT for RTCC_IF            */
#define RTCC_IF_CC1                             (0x1UL << 6)                            /**< CC Channel n Interrupt Flag                 */
#define _RTCC_IF_CC1_SHIFT                      6                                       /**< Shift value for RTCC_CC1                    */
#define _RTCC_IF_CC1_MASK                       0x40UL                                  /**< Bit mask for RTCC_CC1                       */
#define _RTCC_IF_CC1_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for RTCC_IF                    */
#define RTCC_IF_CC1_DEFAULT                     (_RTCC_IF_CC1_DEFAULT << 6)             /**< Shifted mode DEFAULT for RTCC_IF            */
#define RTCC_IF_CC2                             (0x1UL << 8)                            /**< CC Channel n Interrupt Flag                 */
#define _RTCC_IF_CC2_SHIFT                      8                                       /**< Shift value for RTCC_CC2                    */
#define _RTCC_IF_CC2_MASK                       0x100UL                                 /**< Bit mask for RTCC_CC2                       */
#define _RTCC_IF_CC2_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for RTCC_IF                    */
#define RTCC_IF_CC2_DEFAULT                     (_RTCC_IF_CC2_DEFAULT << 8)             /**< Shifted mode DEFAULT for RTCC_IF            */

/* Bit fields for RTCC IEN */
#define _RTCC_IEN_RESETVALUE                    0x00000000UL                            /**< Default value for RTCC_IEN                  */
#define _RTCC_IEN_MASK                          0x000003FFUL                            /**< Mask for RTCC_IEN                           */
#define RTCC_IEN_OF                             (0x1UL << 0)                            /**< OF Interrupt Enable                         */
#define _RTCC_IEN_OF_SHIFT                      0                                       /**< Shift value for RTCC_OF                     */
#define _RTCC_IEN_OF_MASK                       0x1UL                                   /**< Bit mask for RTCC_OF                        */
#define _RTCC_IEN_OF_DEFAULT                    0x00000000UL                            /**< Mode DEFAULT for RTCC_IEN                   */
#define RTCC_IEN_OF_DEFAULT                     (_RTCC_IEN_OF_DEFAULT << 0)             /**< Shifted mode DEFAULT for RTCC_IEN           */
#define RTCC_IEN_CNTTICK                        (0x1UL << 1)                            /**< CNTTICK Interrupt Enable                    */
#define _RTCC_IEN_CNTTICK_SHIFT                 1                                       /**< Shift value for RTCC_CNTTICK                */
#define _RTCC_IEN_CNTTICK_MASK                  0x2UL                                   /**< Bit mask for RTCC_CNTTICK                   */
#define _RTCC_IEN_CNTTICK_DEFAULT               0x00000000UL                            /**< Mode DEFAULT for RTCC_IEN                   */
#define RTCC_IEN_CNTTICK_DEFAULT                (_RTCC_IEN_CNTTICK_DEFAULT << 1)        /**< Shifted mode DEFAULT for RTCC_IEN           */
#define RTCC_IEN_CC0                            (0x1UL << 4)                            /**< CC Channel n Interrupt Enable               */
#define _RTCC_IEN_CC0_SHIFT                     4                                       /**< Shift value for RTCC_CC0                    */
#define _RTCC_IEN_CC0_MASK                      0x10UL                                  /**< Bit mask for RTCC_CC0                       */
#define _RTCC_IEN_CC0_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for RTCC_IEN                   */
#define RTCC_IEN_CC0_DEFAULT                    (_RTCC_IEN_CC0_DEFAULT << 4)            /**< Shifted mode DEFAULT for RTCC_IEN           */
#define RTCC_IEN_CC1                            (0x1UL << 6)                            /**< CC Channel n Interrupt Enable               */
#define _RTCC_IEN_CC1_SHIFT                     6                                       /**< Shift value for RTCC_CC1                    */
#define _RTCC_IEN_CC1_MASK                      0x40UL                                  /**< Bit mask for RTCC_CC1                       */
#define _RTCC_IEN_CC1_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for RTCC_IEN                   */
#define RTCC_IEN_CC1_DEFAULT                    (_RTCC_IEN_CC1_DEFAULT << 6)            /**< Shifted mode DEFAULT for RTCC_IEN           */
#define RTCC_IEN_CC2                            (0x1UL << 8)                            /**< CC Channel n Interrupt Enable               */
#define _RTCC_IEN_CC2_SHIFT                     8                                       /**< Shift value for RTCC_CC2                    */
#define _RTCC_IEN_CC2_MASK                      0x100UL                                 /**< Bit mask for RTCC_CC2                       */
#define _RTCC_IEN_CC2_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for RTCC_IEN                   */
#define RTCC_IEN_CC2_DEFAULT                    (_RTCC_IEN_CC2_DEFAULT << 8)            /**< Shifted mode DEFAULT for RTCC_IEN           */

/* Bit fields for RTCC PRECNT */
#define _RTCC_PRECNT_RESETVALUE                 0x00000000UL                            /**< Default value for RTCC_PRECNT               */
#define _RTCC_PRECNT_MASK                       0x00007FFFUL                            /**< Mask for RTCC_PRECNT                        */
#define _RTCC_PRECNT_PRECNT_SHIFT               0                                       /**< Shift value for RTCC_PRECNT                 */
#define _RTCC_PRECNT_PRECNT_MASK                0x7FFFUL                                /**< Bit mask for RTCC_PRECNT                    */
#define _RTCC_PRECNT_PRECNT_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for RTCC_PRECNT                */
#define RTCC_PRECNT_PRECNT_DEFAULT              (_RTCC_PRECNT_PRECNT_DEFAULT << 0)      /**< Shifted mode DEFAULT for RTCC_PRECNT        */

/* Bit fields for RTCC CNT */
#define _RTCC_CNT_RESETVALUE                    0x00000000UL                            /**< Default value for RTCC_CNT                  */
#define _RTCC_CNT_MASK                          0xFFFFFFFFUL                            /**< Mask for RTCC_CNT                           */
#define _RTCC_CNT_CNT_SHIFT                     0                                       /**< Shift value for RTCC_CNT                    */
#define _RTCC_CNT_CNT_MASK                      0xFFFFFFFFUL                            /**< Bit mask for RTCC_CNT                       */
#define _RTCC_CNT_CNT_DEFAULT                   0x00000000UL                            /**< Mode DEFAULT for RTCC_CNT                   */
#define RTCC_CNT_CNT_DEFAULT                    (_RTCC_CNT_CNT_DEFAULT << 0)            /**< Shifted mode DEFAULT for RTCC_CNT           */

/* Bit fields for RTCC COMBCNT */
#define _RTCC_COMBCNT_RESETVALUE                0x00000000UL                            /**< Default value for RTCC_COMBCNT              */
#define _RTCC_COMBCNT_MASK                      0xFFFFFFFFUL                            /**< Mask for RTCC_COMBCNT                       */
#define _RTCC_COMBCNT_PRECNT_SHIFT              0                                       /**< Shift value for RTCC_PRECNT                 */
#define _RTCC_COMBCNT_PRECNT_MASK               0x7FFFUL                                /**< Bit mask for RTCC_PRECNT                    */
#define _RTCC_COMBCNT_PRECNT_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for RTCC_COMBCNT               */
#define RTCC_COMBCNT_PRECNT_DEFAULT             (_RTCC_COMBCNT_PRECNT_DEFAULT << 0)     /**< Shifted mode DEFAULT for RTCC_COMBCNT       */
#define _RTCC_COMBCNT_CNTLSB_SHIFT              15                                      /**< Shift value for RTCC_CNTLSB                 */
#define _RTCC_COMBCNT_CNTLSB_MASK               0xFFFF8000UL                            /**< Bit mask for RTCC_CNTLSB                    */
#define _RTCC_COMBCNT_CNTLSB_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for RTCC_COMBCNT               */
#define RTCC_COMBCNT_CNTLSB_DEFAULT             (_RTCC_COMBCNT_CNTLSB_DEFAULT << 15)    /**< Shifted mode DEFAULT for RTCC_COMBCNT       */

/* Bit fields for RTCC SYNCBUSY */
#define _RTCC_SYNCBUSY_RESETVALUE               0x00000000UL                            /**< Default value for RTCC_SYNCBUSY             */
#define _RTCC_SYNCBUSY_MASK                     0x0000000FUL                            /**< Mask for RTCC_SYNCBUSY                      */
#define RTCC_SYNCBUSY_START                     (0x1UL << 0)                            /**< Sync busy for START                         */
#define _RTCC_SYNCBUSY_START_SHIFT              0                                       /**< Shift value for RTCC_START                  */
#define _RTCC_SYNCBUSY_START_MASK               0x1UL                                   /**< Bit mask for RTCC_START                     */
#define _RTCC_SYNCBUSY_START_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for RTCC_SYNCBUSY              */
#define RTCC_SYNCBUSY_START_DEFAULT             (_RTCC_SYNCBUSY_START_DEFAULT << 0)     /**< Shifted mode DEFAULT for RTCC_SYNCBUSY      */
#define RTCC_SYNCBUSY_STOP                      (0x1UL << 1)                            /**< Sync busy for STOP                          */
#define _RTCC_SYNCBUSY_STOP_SHIFT               1                                       /**< Shift value for RTCC_STOP                   */
#define _RTCC_SYNCBUSY_STOP_MASK                0x2UL                                   /**< Bit mask for RTCC_STOP                      */
#define _RTCC_SYNCBUSY_STOP_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for RTCC_SYNCBUSY              */
#define RTCC_SYNCBUSY_STOP_DEFAULT              (_RTCC_SYNCBUSY_STOP_DEFAULT << 1)      /**< Shifted mode DEFAULT for RTCC_SYNCBUSY      */
#define RTCC_SYNCBUSY_PRECNT                    (0x1UL << 2)                            /**< Sync busy for PRECNT                        */
#define _RTCC_SYNCBUSY_PRECNT_SHIFT             2                                       /**< Shift value for RTCC_PRECNT                 */
#define _RTCC_SYNCBUSY_PRECNT_MASK              0x4UL                                   /**< Bit mask for RTCC_PRECNT                    */
#define _RTCC_SYNCBUSY_PRECNT_DEFAULT           0x00000000UL                            /**< Mode DEFAULT for RTCC_SYNCBUSY              */
#define RTCC_SYNCBUSY_PRECNT_DEFAULT            (_RTCC_SYNCBUSY_PRECNT_DEFAULT << 2)    /**< Shifted mode DEFAULT for RTCC_SYNCBUSY      */
#define RTCC_SYNCBUSY_CNT                       (0x1UL << 3)                            /**< Sync busy for CNT                           */
#define _RTCC_SYNCBUSY_CNT_SHIFT                3                                       /**< Shift value for RTCC_CNT                    */
#define _RTCC_SYNCBUSY_CNT_MASK                 0x8UL                                   /**< Bit mask for RTCC_CNT                       */
#define _RTCC_SYNCBUSY_CNT_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for RTCC_SYNCBUSY              */
#define RTCC_SYNCBUSY_CNT_DEFAULT               (_RTCC_SYNCBUSY_CNT_DEFAULT << 3)       /**< Shifted mode DEFAULT for RTCC_SYNCBUSY      */

/* Bit fields for RTCC LOCK */
#define _RTCC_LOCK_RESETVALUE                   0x00000000UL                            /**< Default value for RTCC_LOCK                 */
#define _RTCC_LOCK_MASK                         0x0000FFFFUL                            /**< Mask for RTCC_LOCK                          */
#define _RTCC_LOCK_LOCKKEY_SHIFT                0                                       /**< Shift value for RTCC_LOCKKEY                */
#define _RTCC_LOCK_LOCKKEY_MASK                 0xFFFFUL                                /**< Bit mask for RTCC_LOCKKEY                   */
#define _RTCC_LOCK_LOCKKEY_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for RTCC_LOCK                  */
#define _RTCC_LOCK_LOCKKEY_UNLOCK               0x0000AEE8UL                            /**< Mode UNLOCK for RTCC_LOCK                   */
#define RTCC_LOCK_LOCKKEY_DEFAULT               (_RTCC_LOCK_LOCKKEY_DEFAULT << 0)       /**< Shifted mode DEFAULT for RTCC_LOCK          */
#define RTCC_LOCK_LOCKKEY_UNLOCK                (_RTCC_LOCK_LOCKKEY_UNLOCK << 0)        /**< Shifted mode UNLOCK for RTCC_LOCK           */

/* Bit fields for RTCC CC_CTRL */
#define _RTCC_CC_CTRL_RESETVALUE                0x00000000UL                            /**< Default value for RTCC_CC_CTRL              */
#define _RTCC_CC_CTRL_MASK                      0x000000FFUL                            /**< Mask for RTCC_CC_CTRL                       */
#define _RTCC_CC_CTRL_MODE_SHIFT                0                                       /**< Shift value for RTCC_MODE                   */
#define _RTCC_CC_CTRL_MODE_MASK                 0x3UL                                   /**< Bit mask for RTCC_MODE                      */
#define _RTCC_CC_CTRL_MODE_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for RTCC_CC_CTRL               */
#define _RTCC_CC_CTRL_MODE_OFF                  0x00000000UL                            /**< Mode OFF for RTCC_CC_CTRL                   */
#define _RTCC_CC_CTRL_MODE_INPUTCAPTURE         0x00000001UL                            /**< Mode INPUTCAPTURE for RTCC_CC_CTRL          */
#define _RTCC_CC_CTRL_MODE_OUTPUTCOMPARE        0x00000002UL                            /**< Mode OUTPUTCOMPARE for RTCC_CC_CTRL         */
#define RTCC_CC_CTRL_MODE_DEFAULT               (_RTCC_CC_CTRL_MODE_DEFAULT << 0)       /**< Shifted mode DEFAULT for RTCC_CC_CTRL       */
#define RTCC_CC_CTRL_MODE_OFF                   (_RTCC_CC_CTRL_MODE_OFF << 0)           /**< Shifted mode OFF for RTCC_CC_CTRL           */
#define RTCC_CC_CTRL_MODE_INPUTCAPTURE          (_RTCC_CC_CTRL_MODE_INPUTCAPTURE << 0)  /**< Shifted mode INPUTCAPTURE for RTCC_CC_CTRL  */
#define RTCC_CC_CTRL_MODE_OUTPUTCOMPARE         (_RTCC_CC_CTRL_MODE_OUTPUTCOMPARE << 0) /**< Shifted mode OUTPUTCOMPARE for RTCC_CC_CTRL */
#define _RTCC_CC_CTRL_CMOA_SHIFT                2                                       /**< Shift value for RTCC_CMOA                   */
#define _RTCC_CC_CTRL_CMOA_MASK                 0xCUL                                   /**< Bit mask for RTCC_CMOA                      */
#define _RTCC_CC_CTRL_CMOA_DEFAULT              0x00000000UL                            /**< Mode DEFAULT for RTCC_CC_CTRL               */
#define _RTCC_CC_CTRL_CMOA_PULSE                0x00000000UL                            /**< Mode PULSE for RTCC_CC_CTRL                 */
#define _RTCC_CC_CTRL_CMOA_TOGGLE               0x00000001UL                            /**< Mode TOGGLE for RTCC_CC_CTRL                */
#define _RTCC_CC_CTRL_CMOA_CLEAR                0x00000002UL                            /**< Mode CLEAR for RTCC_CC_CTRL                 */
#define _RTCC_CC_CTRL_CMOA_SET                  0x00000003UL                            /**< Mode SET for RTCC_CC_CTRL                   */
#define RTCC_CC_CTRL_CMOA_DEFAULT               (_RTCC_CC_CTRL_CMOA_DEFAULT << 2)       /**< Shifted mode DEFAULT for RTCC_CC_CTRL       */
#define RTCC_CC_CTRL_CMOA_PULSE                 (_RTCC_CC_CTRL_CMOA_PULSE << 2)         /**< Shifted mode PULSE for RTCC_CC_CTRL         */
#define RTCC_CC_CTRL_CMOA_TOGGLE                (_RTCC_CC_CTRL_CMOA_TOGGLE << 2)        /**< Shifted mode TOGGLE for RTCC_CC_CTRL        */
#define RTCC_CC_CTRL_CMOA_CLEAR                 (_RTCC_CC_CTRL_CMOA_CLEAR << 2)         /**< Shifted mode CLEAR for RTCC_CC_CTRL         */
#define RTCC_CC_CTRL_CMOA_SET                   (_RTCC_CC_CTRL_CMOA_SET << 2)           /**< Shifted mode SET for RTCC_CC_CTRL           */
#define RTCC_CC_CTRL_COMPBASE                   (0x1UL << 4)                            /**< Capture compare channel comparison base.    */
#define _RTCC_CC_CTRL_COMPBASE_SHIFT            4                                       /**< Shift value for RTCC_COMPBASE               */
#define _RTCC_CC_CTRL_COMPBASE_MASK             0x10UL                                  /**< Bit mask for RTCC_COMPBASE                  */
#define _RTCC_CC_CTRL_COMPBASE_DEFAULT          0x00000000UL                            /**< Mode DEFAULT for RTCC_CC_CTRL               */
#define _RTCC_CC_CTRL_COMPBASE_CNT              0x00000000UL                            /**< Mode CNT for RTCC_CC_CTRL                   */
#define _RTCC_CC_CTRL_COMPBASE_PRECNT           0x00000001UL                            /**< Mode PRECNT for RTCC_CC_CTRL                */
#define RTCC_CC_CTRL_COMPBASE_DEFAULT           (_RTCC_CC_CTRL_COMPBASE_DEFAULT << 4)   /**< Shifted mode DEFAULT for RTCC_CC_CTRL       */
#define RTCC_CC_CTRL_COMPBASE_CNT               (_RTCC_CC_CTRL_COMPBASE_CNT << 4)       /**< Shifted mode CNT for RTCC_CC_CTRL           */
#define RTCC_CC_CTRL_COMPBASE_PRECNT            (_RTCC_CC_CTRL_COMPBASE_PRECNT << 4)    /**< Shifted mode PRECNT for RTCC_CC_CTRL        */
#define _RTCC_CC_CTRL_ICEDGE_SHIFT              5                                       /**< Shift value for RTCC_ICEDGE                 */
#define _RTCC_CC_CTRL_ICEDGE_MASK               0x60UL                                  /**< Bit mask for RTCC_ICEDGE                    */
#define _RTCC_CC_CTRL_ICEDGE_DEFAULT            0x00000000UL                            /**< Mode DEFAULT for RTCC_CC_CTRL               */
#define _RTCC_CC_CTRL_ICEDGE_RISING             0x00000000UL                            /**< Mode RISING for RTCC_CC_CTRL                */
#define _RTCC_CC_CTRL_ICEDGE_FALLING            0x00000001UL                            /**< Mode FALLING for RTCC_CC_CTRL               */
#define _RTCC_CC_CTRL_ICEDGE_BOTH               0x00000002UL                            /**< Mode BOTH for RTCC_CC_CTRL                  */
#define _RTCC_CC_CTRL_ICEDGE_NONE               0x00000003UL                            /**< Mode NONE for RTCC_CC_CTRL                  */
#define RTCC_CC_CTRL_ICEDGE_DEFAULT             (_RTCC_CC_CTRL_ICEDGE_DEFAULT << 5)     /**< Shifted mode DEFAULT for RTCC_CC_CTRL       */
#define RTCC_CC_CTRL_ICEDGE_RISING              (_RTCC_CC_CTRL_ICEDGE_RISING << 5)      /**< Shifted mode RISING for RTCC_CC_CTRL        */
#define RTCC_CC_CTRL_ICEDGE_FALLING             (_RTCC_CC_CTRL_ICEDGE_FALLING << 5)     /**< Shifted mode FALLING for RTCC_CC_CTRL       */
#define RTCC_CC_CTRL_ICEDGE_BOTH                (_RTCC_CC_CTRL_ICEDGE_BOTH << 5)        /**< Shifted mode BOTH for RTCC_CC_CTRL          */
#define RTCC_CC_CTRL_ICEDGE_NONE                (_RTCC_CC_CTRL_ICEDGE_NONE << 5)        /**< Shifted mode NONE for RTCC_CC_CTRL          */

/* Bit fields for RTCC CC_OCVALUE */
#define _RTCC_CC_OCVALUE_RESETVALUE             0x00000000UL                            /**< Default value for RTCC_CC_OCVALUE           */
#define _RTCC_CC_OCVALUE_MASK                   0xFFFFFFFFUL                            /**< Mask for RTCC_CC_OCVALUE                    */
#define _RTCC_CC_OCVALUE_OC_SHIFT               0                                       /**< Shift value for RTCC_OC                     */
#define _RTCC_CC_OCVALUE_OC_MASK                0xFFFFFFFFUL                            /**< Bit mask for RTCC_OC                        */
#define _RTCC_CC_OCVALUE_OC_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for RTCC_CC_OCVALUE            */
#define RTCC_CC_OCVALUE_OC_DEFAULT              (_RTCC_CC_OCVALUE_OC_DEFAULT << 0)      /**< Shifted mode DEFAULT for RTCC_CC_OCVALUE    */

/* Bit fields for RTCC CC_ICVALUE */
#define _RTCC_CC_ICVALUE_RESETVALUE             0x00000000UL                            /**< Default value for RTCC_CC_ICVALUE           */
#define _RTCC_CC_ICVALUE_MASK                   0xFFFFFFFFUL                            /**< Mask for RTCC_CC_ICVALUE                    */
#define _RTCC_CC_ICVALUE_IC_SHIFT               0                                       /**< Shift value for RTCC_IC                     */
#define _RTCC_CC_ICVALUE_IC_MASK                0xFFFFFFFFUL                            /**< Bit mask for RTCC_IC                        */
#define _RTCC_CC_ICVALUE_IC_DEFAULT             0x00000000UL                            /**< Mode DEFAULT for RTCC_CC_ICVALUE            */
#define RTCC_CC_ICVALUE_IC_DEFAULT              (_RTCC_CC_ICVALUE_IC_DEFAULT << 0)      /**< Shifted mode DEFAULT for RTCC_CC_ICVALUE    */

/** @} End of group EFR32MG22_RTCC_BitFields */
/** @} End of group EFR32MG22_RTCC */
/** @} End of group Parts */
#endif /* EFR32MG22_RTCC_H */
