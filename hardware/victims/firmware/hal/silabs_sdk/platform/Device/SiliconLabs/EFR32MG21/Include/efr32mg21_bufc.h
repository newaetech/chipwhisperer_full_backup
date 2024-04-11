/**************************************************************************//**
 * @file
 * @brief EFR32MG21 BUFC register and bit field definitions
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
#ifndef EFR32MG21_BUFC_H
#define EFR32MG21_BUFC_H
#define BUFC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_BUFC BUFC
 * @{
 * @brief EFR32MG21 BUFC Register Declaration.
 *****************************************************************************/

/** BUFC BUF Register Group Declaration. */
typedef struct {
  __IOM uint32_t CTRL;                               /**< Buffer Control                                     */
  __IOM uint32_t ADDR;                               /**< Buffer Address                                     */
  __IOM uint32_t WRITEOFFSET;                        /**< Write Offset                                       */
  __IOM uint32_t READOFFSET;                         /**< Read Offset                                        */
  uint32_t       RESERVED0[1U];                      /**< Reserved for future use                            */
  __IM uint32_t  READDATA;                           /**< Buffer Read Data                                   */
  __IOM uint32_t WRITEDATA;                          /**< Buffer Write Data                                  */
  __IOM uint32_t XWRITE;                             /**< Buffer XOR Write                                   */
  __IM uint32_t  STATUS;                             /**< Buffer Status Register                             */
  __IOM uint32_t THRESHOLDCTRL;                      /**< Threshold Control                                  */
  __IOM uint32_t CMD;                                /**< Buffer Command                                     */
  __IOM uint32_t FIFOASYNC;                          /**< New Register                                       */
} BUFC_BUF_TypeDef;

/** BUFC Register Declaration. */
typedef struct {
  __IM uint32_t    IPVERSION;                   /**< IP Version                                         */
  __IOM uint32_t   EN;                          /**< Enable peripheral clock to this module             */
  BUFC_BUF_TypeDef BUF[4U];                     /**< Data Buffer                                        */
  uint32_t         RESERVED0[6U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF;                          /**< BUFC Interrupt Flags                               */
  uint32_t         RESERVED1[2U];               /**< Reserved for future use                            */
  __IOM uint32_t   IEN;                         /**< Interrupt Enable Register                          */
  __IOM uint32_t   RAMBASEADDR;                 /**< New Register                                       */
  uint32_t         RESERVED2[963U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_SET;               /**< IP Version                                         */
  __IOM uint32_t   EN_SET;                      /**< Enable peripheral clock to this module             */
  BUFC_BUF_TypeDef BUF_SET[4U];                 /**< Data Buffer                                        */
  uint32_t         RESERVED3[6U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF_SET;                      /**< BUFC Interrupt Flags                               */
  uint32_t         RESERVED4[2U];               /**< Reserved for future use                            */
  __IOM uint32_t   IEN_SET;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   RAMBASEADDR_SET;             /**< New Register                                       */
  uint32_t         RESERVED5[963U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_CLR;               /**< IP Version                                         */
  __IOM uint32_t   EN_CLR;                      /**< Enable peripheral clock to this module             */
  BUFC_BUF_TypeDef BUF_CLR[4U];                 /**< Data Buffer                                        */
  uint32_t         RESERVED6[6U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF_CLR;                      /**< BUFC Interrupt Flags                               */
  uint32_t         RESERVED7[2U];               /**< Reserved for future use                            */
  __IOM uint32_t   IEN_CLR;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   RAMBASEADDR_CLR;             /**< New Register                                       */
  uint32_t         RESERVED8[963U];             /**< Reserved for future use                            */
  __IM uint32_t    IPVERSION_TGL;               /**< IP Version                                         */
  __IOM uint32_t   EN_TGL;                      /**< Enable peripheral clock to this module             */
  BUFC_BUF_TypeDef BUF_TGL[4U];                 /**< Data Buffer                                        */
  uint32_t         RESERVED9[6U];               /**< Reserved for future use                            */
  __IOM uint32_t   IF_TGL;                      /**< BUFC Interrupt Flags                               */
  uint32_t         RESERVED10[2U];              /**< Reserved for future use                            */
  __IOM uint32_t   IEN_TGL;                     /**< Interrupt Enable Register                          */
  __IOM uint32_t   RAMBASEADDR_TGL;             /**< New Register                                       */
} BUFC_TypeDef;
/** @} End of group EFR32MG21_BUFC */

/**************************************************************************//**
 * @addtogroup EFR32MG21_BUFC
 * @{
 * @defgroup EFR32MG21_BUFC_BitFields BUFC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for BUFC IPVERSION */
#define _BUFC_IPVERSION_RESETVALUE                           0x00000000UL                             /**< Default value for BUFC_IPVERSION            */
#define _BUFC_IPVERSION_MASK                                 0xFFFFFFFFUL                             /**< Mask for BUFC_IPVERSION                     */
#define _BUFC_IPVERSION_IPVERSION_SHIFT                      0                                        /**< Shift value for BUFC_IPVERSION              */
#define _BUFC_IPVERSION_IPVERSION_MASK                       0xFFFFFFFFUL                             /**< Bit mask for BUFC_IPVERSION                 */
#define _BUFC_IPVERSION_IPVERSION_DEFAULT                    0x00000000UL                             /**< Mode DEFAULT for BUFC_IPVERSION             */
#define BUFC_IPVERSION_IPVERSION_DEFAULT                     (_BUFC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_IPVERSION     */

/* Bit fields for BUFC EN */
#define _BUFC_EN_RESETVALUE                                  0x00000000UL               /**< Default value for BUFC_EN                   */
#define _BUFC_EN_MASK                                        0x00000001UL               /**< Mask for BUFC_EN                            */
#define BUFC_EN_EN                                           (0x1UL << 0)               /**< Enable peripheral clock to this module      */
#define _BUFC_EN_EN_SHIFT                                    0                          /**< Shift value for BUFC_EN                     */
#define _BUFC_EN_EN_MASK                                     0x1UL                      /**< Bit mask for BUFC_EN                        */
#define _BUFC_EN_EN_DEFAULT                                  0x00000000UL               /**< Mode DEFAULT for BUFC_EN                    */
#define BUFC_EN_EN_DEFAULT                                   (_BUFC_EN_EN_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_EN            */

/* Bit fields for BUFC BUF_CTRL */
#define _BUFC_BUF_CTRL_RESETVALUE                            0x00000000UL                        /**< Default value for BUFC_BUF_CTRL             */
#define _BUFC_BUF_CTRL_MASK                                  0x00000007UL                        /**< Mask for BUFC_BUF_CTRL                      */
#define _BUFC_BUF_CTRL_SIZE_SHIFT                            0                                   /**< Shift value for BUFC_SIZE                   */
#define _BUFC_BUF_CTRL_SIZE_MASK                             0x7UL                               /**< Bit mask for BUFC_SIZE                      */
#define _BUFC_BUF_CTRL_SIZE_DEFAULT                          0x00000000UL                        /**< Mode DEFAULT for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE64                           0x00000000UL                        /**< Mode SIZE64 for BUFC_BUF_CTRL               */
#define _BUFC_BUF_CTRL_SIZE_SIZE128                          0x00000001UL                        /**< Mode SIZE128 for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE256                          0x00000002UL                        /**< Mode SIZE256 for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE512                          0x00000003UL                        /**< Mode SIZE512 for BUFC_BUF_CTRL              */
#define _BUFC_BUF_CTRL_SIZE_SIZE1024                         0x00000004UL                        /**< Mode SIZE1024 for BUFC_BUF_CTRL             */
#define _BUFC_BUF_CTRL_SIZE_SIZE2048                         0x00000005UL                        /**< Mode SIZE2048 for BUFC_BUF_CTRL             */
#define _BUFC_BUF_CTRL_SIZE_SIZE4096                         0x00000006UL                        /**< Mode SIZE4096 for BUFC_BUF_CTRL             */
#define BUFC_BUF_CTRL_SIZE_DEFAULT                           (_BUFC_BUF_CTRL_SIZE_DEFAULT << 0)  /**< Shifted mode DEFAULT for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE64                            (_BUFC_BUF_CTRL_SIZE_SIZE64 << 0)   /**< Shifted mode SIZE64 for BUFC_BUF_CTRL       */
#define BUFC_BUF_CTRL_SIZE_SIZE128                           (_BUFC_BUF_CTRL_SIZE_SIZE128 << 0)  /**< Shifted mode SIZE128 for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE256                           (_BUFC_BUF_CTRL_SIZE_SIZE256 << 0)  /**< Shifted mode SIZE256 for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE512                           (_BUFC_BUF_CTRL_SIZE_SIZE512 << 0)  /**< Shifted mode SIZE512 for BUFC_BUF_CTRL      */
#define BUFC_BUF_CTRL_SIZE_SIZE1024                          (_BUFC_BUF_CTRL_SIZE_SIZE1024 << 0) /**< Shifted mode SIZE1024 for BUFC_BUF_CTRL     */
#define BUFC_BUF_CTRL_SIZE_SIZE2048                          (_BUFC_BUF_CTRL_SIZE_SIZE2048 << 0) /**< Shifted mode SIZE2048 for BUFC_BUF_CTRL     */
#define BUFC_BUF_CTRL_SIZE_SIZE4096                          (_BUFC_BUF_CTRL_SIZE_SIZE4096 << 0) /**< Shifted mode SIZE4096 for BUFC_BUF_CTRL     */

/* Bit fields for BUFC BUF_ADDR */
#define _BUFC_BUF_ADDR_RESETVALUE                            0x00000000UL                       /**< Default value for BUFC_BUF_ADDR             */
#define _BUFC_BUF_ADDR_MASK                                  0x00FFFFFFUL                       /**< Mask for BUFC_BUF_ADDR                      */
#define _BUFC_BUF_ADDR_ADDR_SHIFT                            0                                  /**< Shift value for BUFC_ADDR                   */
#define _BUFC_BUF_ADDR_ADDR_MASK                             0xFFFFFFUL                         /**< Bit mask for BUFC_ADDR                      */
#define _BUFC_BUF_ADDR_ADDR_DEFAULT                          0x00000000UL                       /**< Mode DEFAULT for BUFC_BUF_ADDR              */
#define BUFC_BUF_ADDR_ADDR_DEFAULT                           (_BUFC_BUF_ADDR_ADDR_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_ADDR      */

/* Bit fields for BUFC BUF_WRITEOFFSET */
#define _BUFC_BUF_WRITEOFFSET_RESETVALUE                     0x00000000UL                                     /**< Default value for BUFC_BUF_WRITEOFFSET      */
#define _BUFC_BUF_WRITEOFFSET_MASK                           0x00001FFFUL                                     /**< Mask for BUFC_BUF_WRITEOFFSET               */
#define _BUFC_BUF_WRITEOFFSET_WRITEOFFSET_SHIFT              0                                                /**< Shift value for BUFC_WRITEOFFSET            */
#define _BUFC_BUF_WRITEOFFSET_WRITEOFFSET_MASK               0x1FFFUL                                         /**< Bit mask for BUFC_WRITEOFFSET               */
#define _BUFC_BUF_WRITEOFFSET_WRITEOFFSET_DEFAULT            0x00000000UL                                     /**< Mode DEFAULT for BUFC_BUF_WRITEOFFSET       */
#define BUFC_BUF_WRITEOFFSET_WRITEOFFSET_DEFAULT             (_BUFC_BUF_WRITEOFFSET_WRITEOFFSET_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_WRITEOFFSET*/

/* Bit fields for BUFC BUF_READOFFSET */
#define _BUFC_BUF_READOFFSET_RESETVALUE                      0x00000000UL                                   /**< Default value for BUFC_BUF_READOFFSET       */
#define _BUFC_BUF_READOFFSET_MASK                            0x00001FFFUL                                   /**< Mask for BUFC_BUF_READOFFSET                */
#define _BUFC_BUF_READOFFSET_READOFFSET_SHIFT                0                                              /**< Shift value for BUFC_READOFFSET             */
#define _BUFC_BUF_READOFFSET_READOFFSET_MASK                 0x1FFFUL                                       /**< Bit mask for BUFC_READOFFSET                */
#define _BUFC_BUF_READOFFSET_READOFFSET_DEFAULT              0x00000000UL                                   /**< Mode DEFAULT for BUFC_BUF_READOFFSET        */
#define BUFC_BUF_READOFFSET_READOFFSET_DEFAULT               (_BUFC_BUF_READOFFSET_READOFFSET_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_READOFFSET*/

/* Bit fields for BUFC BUF_READDATA */
#define _BUFC_BUF_READDATA_RESETVALUE                        0x00000000UL                               /**< Default value for BUFC_BUF_READDATA         */
#define _BUFC_BUF_READDATA_MASK                              0x000000FFUL                               /**< Mask for BUFC_BUF_READDATA                  */
#define _BUFC_BUF_READDATA_READDATA_SHIFT                    0                                          /**< Shift value for BUFC_READDATA               */
#define _BUFC_BUF_READDATA_READDATA_MASK                     0xFFUL                                     /**< Bit mask for BUFC_READDATA                  */
#define _BUFC_BUF_READDATA_READDATA_DEFAULT                  0x00000000UL                               /**< Mode DEFAULT for BUFC_BUF_READDATA          */
#define BUFC_BUF_READDATA_READDATA_DEFAULT                   (_BUFC_BUF_READDATA_READDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_READDATA  */

/* Bit fields for BUFC BUF_WRITEDATA */
#define _BUFC_BUF_WRITEDATA_RESETVALUE                       0x00000000UL                                 /**< Default value for BUFC_BUF_WRITEDATA        */
#define _BUFC_BUF_WRITEDATA_MASK                             0x000000FFUL                                 /**< Mask for BUFC_BUF_WRITEDATA                 */
#define _BUFC_BUF_WRITEDATA_WRITEDATA_SHIFT                  0                                            /**< Shift value for BUFC_WRITEDATA              */
#define _BUFC_BUF_WRITEDATA_WRITEDATA_MASK                   0xFFUL                                       /**< Bit mask for BUFC_WRITEDATA                 */
#define _BUFC_BUF_WRITEDATA_WRITEDATA_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for BUFC_BUF_WRITEDATA         */
#define BUFC_BUF_WRITEDATA_WRITEDATA_DEFAULT                 (_BUFC_BUF_WRITEDATA_WRITEDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_WRITEDATA */

/* Bit fields for BUFC BUF_XWRITE */
#define _BUFC_BUF_XWRITE_RESETVALUE                          0x00000000UL                                 /**< Default value for BUFC_BUF_XWRITE           */
#define _BUFC_BUF_XWRITE_MASK                                0x000000FFUL                                 /**< Mask for BUFC_BUF_XWRITE                    */
#define _BUFC_BUF_XWRITE_XORWRITEDATA_SHIFT                  0                                            /**< Shift value for BUFC_XORWRITEDATA           */
#define _BUFC_BUF_XWRITE_XORWRITEDATA_MASK                   0xFFUL                                       /**< Bit mask for BUFC_XORWRITEDATA              */
#define _BUFC_BUF_XWRITE_XORWRITEDATA_DEFAULT                0x00000000UL                                 /**< Mode DEFAULT for BUFC_BUF_XWRITE            */
#define BUFC_BUF_XWRITE_XORWRITEDATA_DEFAULT                 (_BUFC_BUF_XWRITE_XORWRITEDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_XWRITE    */

/* Bit fields for BUFC BUF_STATUS */
#define _BUFC_BUF_STATUS_RESETVALUE                          0x00000000UL                                   /**< Default value for BUFC_BUF_STATUS           */
#define _BUFC_BUF_STATUS_MASK                                0x00111FFFUL                                   /**< Mask for BUFC_BUF_STATUS                    */
#define _BUFC_BUF_STATUS_BYTES_SHIFT                         0                                              /**< Shift value for BUFC_BYTES                  */
#define _BUFC_BUF_STATUS_BYTES_MASK                          0x1FFFUL                                       /**< Bit mask for BUFC_BYTES                     */
#define _BUFC_BUF_STATUS_BYTES_DEFAULT                       0x00000000UL                                   /**< Mode DEFAULT for BUFC_BUF_STATUS            */
#define BUFC_BUF_STATUS_BYTES_DEFAULT                        (_BUFC_BUF_STATUS_BYTES_DEFAULT << 0)          /**< Shifted mode DEFAULT for BUFC_BUF_STATUS    */
#define BUFC_BUF_STATUS_THRESHOLDFLAG                        (0x1UL << 20)                                  /**< Buffer Threshold Flag                       */
#define _BUFC_BUF_STATUS_THRESHOLDFLAG_SHIFT                 20                                             /**< Shift value for BUFC_THRESHOLDFLAG          */
#define _BUFC_BUF_STATUS_THRESHOLDFLAG_MASK                  0x100000UL                                     /**< Bit mask for BUFC_THRESHOLDFLAG             */
#define _BUFC_BUF_STATUS_THRESHOLDFLAG_DEFAULT               0x00000000UL                                   /**< Mode DEFAULT for BUFC_BUF_STATUS            */
#define BUFC_BUF_STATUS_THRESHOLDFLAG_DEFAULT                (_BUFC_BUF_STATUS_THRESHOLDFLAG_DEFAULT << 20) /**< Shifted mode DEFAULT for BUFC_BUF_STATUS    */

/* Bit fields for BUFC BUF_THRESHOLDCTRL */
#define _BUFC_BUF_THRESHOLDCTRL_RESETVALUE                   0x00000000UL                                              /**< Default value for BUFC_BUF_THRESHOLDCTRL    */
#define _BUFC_BUF_THRESHOLDCTRL_MASK                         0x00002FFFUL                                              /**< Mask for BUFC_BUF_THRESHOLDCTRL             */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLD_SHIFT              0                                                         /**< Shift value for BUFC_THRESHOLD              */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLD_MASK               0xFFFUL                                                   /**< Bit mask for BUFC_THRESHOLD                 */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLD_DEFAULT            0x00000000UL                                              /**< Mode DEFAULT for BUFC_BUF_THRESHOLDCTRL     */
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLD_DEFAULT             (_BUFC_BUF_THRESHOLDCTRL_THRESHOLD_DEFAULT << 0)          /**< Shifted mode DEFAULT for BUFC_BUF_THRESHOLDCTRL*/
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE                 (0x1UL << 13)                                             /**< Buffer Threshold Mode                       */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_SHIFT          13                                                        /**< Shift value for BUFC_THRESHOLDMODE          */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_MASK           0x2000UL                                                  /**< Bit mask for BUFC_THRESHOLDMODE             */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_DEFAULT        0x00000000UL                                              /**< Mode DEFAULT for BUFC_BUF_THRESHOLDCTRL     */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LARGER         0x00000000UL                                              /**< Mode LARGER for BUFC_BUF_THRESHOLDCTRL      */
#define _BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LESSOREQUAL    0x00000001UL                                              /**< Mode LESSOREQUAL for BUFC_BUF_THRESHOLDCTRL */
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_DEFAULT         (_BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_DEFAULT << 13)     /**< Shifted mode DEFAULT for BUFC_BUF_THRESHOLDCTRL*/
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LARGER          (_BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LARGER << 13)      /**< Shifted mode LARGER for BUFC_BUF_THRESHOLDCTRL*/
#define BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LESSOREQUAL     (_BUFC_BUF_THRESHOLDCTRL_THRESHOLDMODE_LESSOREQUAL << 13) /**< Shifted mode LESSOREQUAL for BUFC_BUF_THRESHOLDCTRL*/

/* Bit fields for BUFC BUF_CMD */
#define _BUFC_BUF_CMD_RESETVALUE                             0x00000000UL                          /**< Default value for BUFC_BUF_CMD              */
#define _BUFC_BUF_CMD_MASK                                   0x0000000FUL                          /**< Mask for BUFC_BUF_CMD                       */
#define BUFC_BUF_CMD_CLEAR                                   (0x1UL << 0)                          /**< Buffer Clear                                */
#define _BUFC_BUF_CMD_CLEAR_SHIFT                            0                                     /**< Shift value for BUFC_CLEAR                  */
#define _BUFC_BUF_CMD_CLEAR_MASK                             0x1UL                                 /**< Bit mask for BUFC_CLEAR                     */
#define _BUFC_BUF_CMD_CLEAR_DEFAULT                          0x00000000UL                          /**< Mode DEFAULT for BUFC_BUF_CMD               */
#define BUFC_BUF_CMD_CLEAR_DEFAULT                           (_BUFC_BUF_CMD_CLEAR_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_BUF_CMD       */
#define BUFC_BUF_CMD_PREFETCH                                (0x1UL << 1)                          /**< Prefetch                                    */
#define _BUFC_BUF_CMD_PREFETCH_SHIFT                         1                                     /**< Shift value for BUFC_PREFETCH               */
#define _BUFC_BUF_CMD_PREFETCH_MASK                          0x2UL                                 /**< Bit mask for BUFC_PREFETCH                  */
#define _BUFC_BUF_CMD_PREFETCH_DEFAULT                       0x00000000UL                          /**< Mode DEFAULT for BUFC_BUF_CMD               */
#define BUFC_BUF_CMD_PREFETCH_DEFAULT                        (_BUFC_BUF_CMD_PREFETCH_DEFAULT << 1) /**< Shifted mode DEFAULT for BUFC_BUF_CMD       */

/* Bit fields for BUFC BUF_FIFOASYNC */
#define _BUFC_BUF_FIFOASYNC_RESETVALUE                       0x00000000UL                           /**< Default value for BUFC_BUF_FIFOASYNC        */
#define _BUFC_BUF_FIFOASYNC_MASK                             0x00000001UL                           /**< Mask for BUFC_BUF_FIFOASYNC                 */
#define BUFC_BUF_FIFOASYNC_RST                               (0x1UL << 0)                           /**< Reset ASYNC                                 */
#define _BUFC_BUF_FIFOASYNC_RST_SHIFT                        0                                      /**< Shift value for BUFC_RST                    */
#define _BUFC_BUF_FIFOASYNC_RST_MASK                         0x1UL                                  /**< Bit mask for BUFC_RST                       */
#define _BUFC_BUF_FIFOASYNC_RST_DEFAULT                      0x00000000UL                           /**< Mode DEFAULT for BUFC_BUF_FIFOASYNC         */
#define BUFC_BUF_FIFOASYNC_RST_DEFAULT                       (_BUFC_BUF_FIFOASYNC_RST_DEFAULT << 0) /**< Shifted mode DEFAULT for BUFC_BUF_FIFOASYNC */

/* Bit fields for BUFC IF */
#define _BUFC_IF_RESETVALUE                                  0x00000000UL                      /**< Default value for BUFC_IF                   */
#define _BUFC_IF_MASK                                        0x8F0F0F0FUL                      /**< Mask for BUFC_IF                            */
#define BUFC_IF_BUF0OF                                       (0x1UL << 0)                      /**< Buffer 0 Overflow                           */
#define _BUFC_IF_BUF0OF_SHIFT                                0                                 /**< Shift value for BUFC_BUF0OF                 */
#define _BUFC_IF_BUF0OF_MASK                                 0x1UL                             /**< Bit mask for BUFC_BUF0OF                    */
#define _BUFC_IF_BUF0OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0OF_DEFAULT                               (_BUFC_IF_BUF0OF_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF0UF                                       (0x1UL << 1)                      /**< Buffer 0 Underflow                          */
#define _BUFC_IF_BUF0UF_SHIFT                                1                                 /**< Shift value for BUFC_BUF0UF                 */
#define _BUFC_IF_BUF0UF_MASK                                 0x2UL                             /**< Bit mask for BUFC_BUF0UF                    */
#define _BUFC_IF_BUF0UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0UF_DEFAULT                               (_BUFC_IF_BUF0UF_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF0THR                                      (0x1UL << 2)                      /**< Buffer 0 Threshold Event                    */
#define _BUFC_IF_BUF0THR_SHIFT                               2                                 /**< Shift value for BUFC_BUF0THR                */
#define _BUFC_IF_BUF0THR_MASK                                0x4UL                             /**< Bit mask for BUFC_BUF0THR                   */
#define _BUFC_IF_BUF0THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0THR_DEFAULT                              (_BUFC_IF_BUF0THR_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF0CORR                                     (0x1UL << 3)                      /**< Buffer 0 Corrupt                            */
#define _BUFC_IF_BUF0CORR_SHIFT                              3                                 /**< Shift value for BUFC_BUF0CORR               */
#define _BUFC_IF_BUF0CORR_MASK                               0x8UL                             /**< Bit mask for BUFC_BUF0CORR                  */
#define _BUFC_IF_BUF0CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF0CORR_DEFAULT                             (_BUFC_IF_BUF0CORR_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1OF                                       (0x1UL << 8)                      /**< Buffer 1 Overflow                           */
#define _BUFC_IF_BUF1OF_SHIFT                                8                                 /**< Shift value for BUFC_BUF1OF                 */
#define _BUFC_IF_BUF1OF_MASK                                 0x100UL                           /**< Bit mask for BUFC_BUF1OF                    */
#define _BUFC_IF_BUF1OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1OF_DEFAULT                               (_BUFC_IF_BUF1OF_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1UF                                       (0x1UL << 9)                      /**< Buffer 1 Underflow                          */
#define _BUFC_IF_BUF1UF_SHIFT                                9                                 /**< Shift value for BUFC_BUF1UF                 */
#define _BUFC_IF_BUF1UF_MASK                                 0x200UL                           /**< Bit mask for BUFC_BUF1UF                    */
#define _BUFC_IF_BUF1UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1UF_DEFAULT                               (_BUFC_IF_BUF1UF_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1THR                                      (0x1UL << 10)                     /**< Buffer 2 Threshold Event                    */
#define _BUFC_IF_BUF1THR_SHIFT                               10                                /**< Shift value for BUFC_BUF1THR                */
#define _BUFC_IF_BUF1THR_MASK                                0x400UL                           /**< Bit mask for BUFC_BUF1THR                   */
#define _BUFC_IF_BUF1THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1THR_DEFAULT                              (_BUFC_IF_BUF1THR_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF1CORR                                     (0x1UL << 11)                     /**< Buffer 1 Corrupt                            */
#define _BUFC_IF_BUF1CORR_SHIFT                              11                                /**< Shift value for BUFC_BUF1CORR               */
#define _BUFC_IF_BUF1CORR_MASK                               0x800UL                           /**< Bit mask for BUFC_BUF1CORR                  */
#define _BUFC_IF_BUF1CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF1CORR_DEFAULT                             (_BUFC_IF_BUF1CORR_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2OF                                       (0x1UL << 16)                     /**< Buffer 2 Overflow                           */
#define _BUFC_IF_BUF2OF_SHIFT                                16                                /**< Shift value for BUFC_BUF2OF                 */
#define _BUFC_IF_BUF2OF_MASK                                 0x10000UL                         /**< Bit mask for BUFC_BUF2OF                    */
#define _BUFC_IF_BUF2OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2OF_DEFAULT                               (_BUFC_IF_BUF2OF_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2UF                                       (0x1UL << 17)                     /**< Buffer 2 Underflow                          */
#define _BUFC_IF_BUF2UF_SHIFT                                17                                /**< Shift value for BUFC_BUF2UF                 */
#define _BUFC_IF_BUF2UF_MASK                                 0x20000UL                         /**< Bit mask for BUFC_BUF2UF                    */
#define _BUFC_IF_BUF2UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2UF_DEFAULT                               (_BUFC_IF_BUF2UF_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2THR                                      (0x1UL << 18)                     /**< Buffer 2 Threshold Event                    */
#define _BUFC_IF_BUF2THR_SHIFT                               18                                /**< Shift value for BUFC_BUF2THR                */
#define _BUFC_IF_BUF2THR_MASK                                0x40000UL                         /**< Bit mask for BUFC_BUF2THR                   */
#define _BUFC_IF_BUF2THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2THR_DEFAULT                              (_BUFC_IF_BUF2THR_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF2CORR                                     (0x1UL << 19)                     /**< Buffer 2 Corrupt                            */
#define _BUFC_IF_BUF2CORR_SHIFT                              19                                /**< Shift value for BUFC_BUF2CORR               */
#define _BUFC_IF_BUF2CORR_MASK                               0x80000UL                         /**< Bit mask for BUFC_BUF2CORR                  */
#define _BUFC_IF_BUF2CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF2CORR_DEFAULT                             (_BUFC_IF_BUF2CORR_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3OF                                       (0x1UL << 24)                     /**< Buffer 3 Overflow                           */
#define _BUFC_IF_BUF3OF_SHIFT                                24                                /**< Shift value for BUFC_BUF3OF                 */
#define _BUFC_IF_BUF3OF_MASK                                 0x1000000UL                       /**< Bit mask for BUFC_BUF3OF                    */
#define _BUFC_IF_BUF3OF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3OF_DEFAULT                               (_BUFC_IF_BUF3OF_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3UF                                       (0x1UL << 25)                     /**< Buffer 3 Underflow                          */
#define _BUFC_IF_BUF3UF_SHIFT                                25                                /**< Shift value for BUFC_BUF3UF                 */
#define _BUFC_IF_BUF3UF_MASK                                 0x2000000UL                       /**< Bit mask for BUFC_BUF3UF                    */
#define _BUFC_IF_BUF3UF_DEFAULT                              0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3UF_DEFAULT                               (_BUFC_IF_BUF3UF_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3THR                                      (0x1UL << 26)                     /**< Buffer 3 Threshold Event                    */
#define _BUFC_IF_BUF3THR_SHIFT                               26                                /**< Shift value for BUFC_BUF3THR                */
#define _BUFC_IF_BUF3THR_MASK                                0x4000000UL                       /**< Bit mask for BUFC_BUF3THR                   */
#define _BUFC_IF_BUF3THR_DEFAULT                             0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3THR_DEFAULT                              (_BUFC_IF_BUF3THR_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUF3CORR                                     (0x1UL << 27)                     /**< Buffer 3 Corrupt                            */
#define _BUFC_IF_BUF3CORR_SHIFT                              27                                /**< Shift value for BUFC_BUF3CORR               */
#define _BUFC_IF_BUF3CORR_MASK                               0x8000000UL                       /**< Bit mask for BUFC_BUF3CORR                  */
#define _BUFC_IF_BUF3CORR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUF3CORR_DEFAULT                             (_BUFC_IF_BUF3CORR_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_IF            */
#define BUFC_IF_BUSERROR                                     (0x1UL << 31)                     /**< Bus Error                                   */
#define _BUFC_IF_BUSERROR_SHIFT                              31                                /**< Shift value for BUFC_BUSERROR               */
#define _BUFC_IF_BUSERROR_MASK                               0x80000000UL                      /**< Bit mask for BUFC_BUSERROR                  */
#define _BUFC_IF_BUSERROR_DEFAULT                            0x00000000UL                      /**< Mode DEFAULT for BUFC_IF                    */
#define BUFC_IF_BUSERROR_DEFAULT                             (_BUFC_IF_BUSERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_IF            */

/* Bit fields for BUFC IEN */
#define _BUFC_IEN_RESETVALUE                                 0x00000000UL                       /**< Default value for BUFC_IEN                  */
#define _BUFC_IEN_MASK                                       0x8F0F0F0FUL                       /**< Mask for BUFC_IEN                           */
#define BUFC_IEN_BUF0OF                                      (0x1UL << 0)                       /**< BUF0OF Interrupt Enable                     */
#define _BUFC_IEN_BUF0OF_SHIFT                               0                                  /**< Shift value for BUFC_BUF0OF                 */
#define _BUFC_IEN_BUF0OF_MASK                                0x1UL                              /**< Bit mask for BUFC_BUF0OF                    */
#define _BUFC_IEN_BUF0OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0OF_DEFAULT                              (_BUFC_IEN_BUF0OF_DEFAULT << 0)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF0UF                                      (0x1UL << 1)                       /**< BUF0UF Interrupt Enable                     */
#define _BUFC_IEN_BUF0UF_SHIFT                               1                                  /**< Shift value for BUFC_BUF0UF                 */
#define _BUFC_IEN_BUF0UF_MASK                                0x2UL                              /**< Bit mask for BUFC_BUF0UF                    */
#define _BUFC_IEN_BUF0UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0UF_DEFAULT                              (_BUFC_IEN_BUF0UF_DEFAULT << 1)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF0THR                                     (0x1UL << 2)                       /**< BUF0THR Interrupt Enable                    */
#define _BUFC_IEN_BUF0THR_SHIFT                              2                                  /**< Shift value for BUFC_BUF0THR                */
#define _BUFC_IEN_BUF0THR_MASK                               0x4UL                              /**< Bit mask for BUFC_BUF0THR                   */
#define _BUFC_IEN_BUF0THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0THR_DEFAULT                             (_BUFC_IEN_BUF0THR_DEFAULT << 2)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF0CORR                                    (0x1UL << 3)                       /**< BUF0CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF0CORR_SHIFT                             3                                  /**< Shift value for BUFC_BUF0CORR               */
#define _BUFC_IEN_BUF0CORR_MASK                              0x8UL                              /**< Bit mask for BUFC_BUF0CORR                  */
#define _BUFC_IEN_BUF0CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF0CORR_DEFAULT                            (_BUFC_IEN_BUF0CORR_DEFAULT << 3)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1OF                                      (0x1UL << 8)                       /**< BUF1OF Interrupt Enable                     */
#define _BUFC_IEN_BUF1OF_SHIFT                               8                                  /**< Shift value for BUFC_BUF1OF                 */
#define _BUFC_IEN_BUF1OF_MASK                                0x100UL                            /**< Bit mask for BUFC_BUF1OF                    */
#define _BUFC_IEN_BUF1OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1OF_DEFAULT                              (_BUFC_IEN_BUF1OF_DEFAULT << 8)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1UF                                      (0x1UL << 9)                       /**< BUF1UF Interrupt Enable                     */
#define _BUFC_IEN_BUF1UF_SHIFT                               9                                  /**< Shift value for BUFC_BUF1UF                 */
#define _BUFC_IEN_BUF1UF_MASK                                0x200UL                            /**< Bit mask for BUFC_BUF1UF                    */
#define _BUFC_IEN_BUF1UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1UF_DEFAULT                              (_BUFC_IEN_BUF1UF_DEFAULT << 9)    /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1THR                                     (0x1UL << 10)                      /**< BUF1THR Interrupt Enable                    */
#define _BUFC_IEN_BUF1THR_SHIFT                              10                                 /**< Shift value for BUFC_BUF1THR                */
#define _BUFC_IEN_BUF1THR_MASK                               0x400UL                            /**< Bit mask for BUFC_BUF1THR                   */
#define _BUFC_IEN_BUF1THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1THR_DEFAULT                             (_BUFC_IEN_BUF1THR_DEFAULT << 10)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF1CORR                                    (0x1UL << 11)                      /**< BUF1CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF1CORR_SHIFT                             11                                 /**< Shift value for BUFC_BUF1CORR               */
#define _BUFC_IEN_BUF1CORR_MASK                              0x800UL                            /**< Bit mask for BUFC_BUF1CORR                  */
#define _BUFC_IEN_BUF1CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF1CORR_DEFAULT                            (_BUFC_IEN_BUF1CORR_DEFAULT << 11) /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2OF                                      (0x1UL << 16)                      /**< BUF2OF Interrupt Enable                     */
#define _BUFC_IEN_BUF2OF_SHIFT                               16                                 /**< Shift value for BUFC_BUF2OF                 */
#define _BUFC_IEN_BUF2OF_MASK                                0x10000UL                          /**< Bit mask for BUFC_BUF2OF                    */
#define _BUFC_IEN_BUF2OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2OF_DEFAULT                              (_BUFC_IEN_BUF2OF_DEFAULT << 16)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2UF                                      (0x1UL << 17)                      /**< BUF2UF Interrupt Enable                     */
#define _BUFC_IEN_BUF2UF_SHIFT                               17                                 /**< Shift value for BUFC_BUF2UF                 */
#define _BUFC_IEN_BUF2UF_MASK                                0x20000UL                          /**< Bit mask for BUFC_BUF2UF                    */
#define _BUFC_IEN_BUF2UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2UF_DEFAULT                              (_BUFC_IEN_BUF2UF_DEFAULT << 17)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2THR                                     (0x1UL << 18)                      /**< BUF2THR Interrupt Enable                    */
#define _BUFC_IEN_BUF2THR_SHIFT                              18                                 /**< Shift value for BUFC_BUF2THR                */
#define _BUFC_IEN_BUF2THR_MASK                               0x40000UL                          /**< Bit mask for BUFC_BUF2THR                   */
#define _BUFC_IEN_BUF2THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2THR_DEFAULT                             (_BUFC_IEN_BUF2THR_DEFAULT << 18)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF2CORR                                    (0x1UL << 19)                      /**< BUF2CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF2CORR_SHIFT                             19                                 /**< Shift value for BUFC_BUF2CORR               */
#define _BUFC_IEN_BUF2CORR_MASK                              0x80000UL                          /**< Bit mask for BUFC_BUF2CORR                  */
#define _BUFC_IEN_BUF2CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF2CORR_DEFAULT                            (_BUFC_IEN_BUF2CORR_DEFAULT << 19) /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3OF                                      (0x1UL << 24)                      /**< BUF3OF Interrupt Enable                     */
#define _BUFC_IEN_BUF3OF_SHIFT                               24                                 /**< Shift value for BUFC_BUF3OF                 */
#define _BUFC_IEN_BUF3OF_MASK                                0x1000000UL                        /**< Bit mask for BUFC_BUF3OF                    */
#define _BUFC_IEN_BUF3OF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3OF_DEFAULT                              (_BUFC_IEN_BUF3OF_DEFAULT << 24)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3UF                                      (0x1UL << 25)                      /**< BUF3UF Interrupt Enable                     */
#define _BUFC_IEN_BUF3UF_SHIFT                               25                                 /**< Shift value for BUFC_BUF3UF                 */
#define _BUFC_IEN_BUF3UF_MASK                                0x2000000UL                        /**< Bit mask for BUFC_BUF3UF                    */
#define _BUFC_IEN_BUF3UF_DEFAULT                             0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3UF_DEFAULT                              (_BUFC_IEN_BUF3UF_DEFAULT << 25)   /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3THR                                     (0x1UL << 26)                      /**< BUF3THR Interrupt Enable                    */
#define _BUFC_IEN_BUF3THR_SHIFT                              26                                 /**< Shift value for BUFC_BUF3THR                */
#define _BUFC_IEN_BUF3THR_MASK                               0x4000000UL                        /**< Bit mask for BUFC_BUF3THR                   */
#define _BUFC_IEN_BUF3THR_DEFAULT                            0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3THR_DEFAULT                             (_BUFC_IEN_BUF3THR_DEFAULT << 26)  /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUF3CORR                                    (0x1UL << 27)                      /**< BUF3CORR Interrupt Enable                   */
#define _BUFC_IEN_BUF3CORR_SHIFT                             27                                 /**< Shift value for BUFC_BUF3CORR               */
#define _BUFC_IEN_BUF3CORR_MASK                              0x8000000UL                        /**< Bit mask for BUFC_BUF3CORR                  */
#define _BUFC_IEN_BUF3CORR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUF3CORR_DEFAULT                            (_BUFC_IEN_BUF3CORR_DEFAULT << 27) /**< Shifted mode DEFAULT for BUFC_IEN           */
#define BUFC_IEN_BUSERROR                                    (0x1UL << 31)                      /**< BUSERROR Interrupt Enable                   */
#define _BUFC_IEN_BUSERROR_SHIFT                             31                                 /**< Shift value for BUFC_BUSERROR               */
#define _BUFC_IEN_BUSERROR_MASK                              0x80000000UL                       /**< Bit mask for BUFC_BUSERROR                  */
#define _BUFC_IEN_BUSERROR_DEFAULT                           0x00000000UL                       /**< Mode DEFAULT for BUFC_IEN                   */
#define BUFC_IEN_BUSERROR_DEFAULT                            (_BUFC_IEN_BUSERROR_DEFAULT << 31) /**< Shifted mode DEFAULT for BUFC_IEN           */

/* Bit fields for BUFC RAMBASEADDR */
#define _BUFC_RAMBASEADDR_RESETVALUE                         0x20000000UL                                  /**< Default value for BUFC_RAMBASEADDR          */
#define _BUFC_RAMBASEADDR_MASK                               0xFFFF0000UL                                  /**< Mask for BUFC_RAMBASEADDR                   */
#define _BUFC_RAMBASEADDR_RAMBASEADDR_SHIFT                  16                                            /**< Shift value for BUFC_RAMBASEADDR            */
#define _BUFC_RAMBASEADDR_RAMBASEADDR_MASK                   0xFFFF0000UL                                  /**< Bit mask for BUFC_RAMBASEADDR               */
#define _BUFC_RAMBASEADDR_RAMBASEADDR_DEFAULT                0x00002000UL                                  /**< Mode DEFAULT for BUFC_RAMBASEADDR           */
#define BUFC_RAMBASEADDR_RAMBASEADDR_DEFAULT                 (_BUFC_RAMBASEADDR_RAMBASEADDR_DEFAULT << 16) /**< Shifted mode DEFAULT for BUFC_RAMBASEADDR   */

/** @} End of group EFR32MG21_BUFC_BitFields */
/** @} End of group EFR32MG21_BUFC */
/** @} End of group Parts */
#endif /* EFR32MG21_BUFC_H */
