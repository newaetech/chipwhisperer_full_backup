/**************************************************************************//**
 * @file
 * @brief EFR32MG21 RFCRC register and bit field definitions
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
#ifndef EFR32MG21_RFCRC_H
#define EFR32MG21_RFCRC_H
#define RFCRC_HAS_SET_CLEAR

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_RFCRC RFCRC
 * @{
 * @brief EFR32MG21 RFCRC Register Declaration.
 *****************************************************************************/

/** RFCRC Register Declaration. */
typedef struct {
  __IM uint32_t  IPVERSION;                     /**< IP Version                                         */
  __IOM uint32_t EN;                            /**< Enable peripheral clock to this module             */
  __IOM uint32_t CTRL;                          /**< Control Register                                   */
  __IM uint32_t  STATUS;                        /**< Status Register                                    */
  __IOM uint32_t CMD;                           /**< Command Register                                   */
  __IOM uint32_t INPUTDATA;                     /**< Input Data Register                                */
  __IOM uint32_t INIT;                          /**< CRC Initialization Value                           */
  __IM uint32_t  DATA;                          /**< CRC Data Register                                  */
  __IOM uint32_t POLY;                          /**< CRC Polynomial Value                               */
  uint32_t       RESERVED0[1015U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_SET;                 /**< IP Version                                         */
  __IOM uint32_t EN_SET;                        /**< Enable peripheral clock to this module             */
  __IOM uint32_t CTRL_SET;                      /**< Control Register                                   */
  __IM uint32_t  STATUS_SET;                    /**< Status Register                                    */
  __IOM uint32_t CMD_SET;                       /**< Command Register                                   */
  __IOM uint32_t INPUTDATA_SET;                 /**< Input Data Register                                */
  __IOM uint32_t INIT_SET;                      /**< CRC Initialization Value                           */
  __IM uint32_t  DATA_SET;                      /**< CRC Data Register                                  */
  __IOM uint32_t POLY_SET;                      /**< CRC Polynomial Value                               */
  uint32_t       RESERVED1[1015U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_CLR;                 /**< IP Version                                         */
  __IOM uint32_t EN_CLR;                        /**< Enable peripheral clock to this module             */
  __IOM uint32_t CTRL_CLR;                      /**< Control Register                                   */
  __IM uint32_t  STATUS_CLR;                    /**< Status Register                                    */
  __IOM uint32_t CMD_CLR;                       /**< Command Register                                   */
  __IOM uint32_t INPUTDATA_CLR;                 /**< Input Data Register                                */
  __IOM uint32_t INIT_CLR;                      /**< CRC Initialization Value                           */
  __IM uint32_t  DATA_CLR;                      /**< CRC Data Register                                  */
  __IOM uint32_t POLY_CLR;                      /**< CRC Polynomial Value                               */
  uint32_t       RESERVED2[1015U];              /**< Reserved for future use                            */
  __IM uint32_t  IPVERSION_TGL;                 /**< IP Version                                         */
  __IOM uint32_t EN_TGL;                        /**< Enable peripheral clock to this module             */
  __IOM uint32_t CTRL_TGL;                      /**< Control Register                                   */
  __IM uint32_t  STATUS_TGL;                    /**< Status Register                                    */
  __IOM uint32_t CMD_TGL;                       /**< Command Register                                   */
  __IOM uint32_t INPUTDATA_TGL;                 /**< Input Data Register                                */
  __IOM uint32_t INIT_TGL;                      /**< CRC Initialization Value                           */
  __IM uint32_t  DATA_TGL;                      /**< CRC Data Register                                  */
  __IOM uint32_t POLY_TGL;                      /**< CRC Polynomial Value                               */
} RFCRC_TypeDef;
/** @} End of group EFR32MG21_RFCRC */

/**************************************************************************//**
 * @addtogroup EFR32MG21_RFCRC
 * @{
 * @defgroup EFR32MG21_RFCRC_BitFields RFCRC Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for RFCRC IPVERSION */
#define _RFCRC_IPVERSION_RESETVALUE           0x00000000UL                              /**< Default value for RFCRC_IPVERSION           */
#define _RFCRC_IPVERSION_MASK                 0xFFFFFFFFUL                              /**< Mask for RFCRC_IPVERSION                    */
#define _RFCRC_IPVERSION_IPVERSION_SHIFT      0                                         /**< Shift value for RFCRC_IPVERSION             */
#define _RFCRC_IPVERSION_IPVERSION_MASK       0xFFFFFFFFUL                              /**< Bit mask for RFCRC_IPVERSION                */
#define _RFCRC_IPVERSION_IPVERSION_DEFAULT    0x00000000UL                              /**< Mode DEFAULT for RFCRC_IPVERSION            */
#define RFCRC_IPVERSION_IPVERSION_DEFAULT     (_RFCRC_IPVERSION_IPVERSION_DEFAULT << 0) /**< Shifted mode DEFAULT for RFCRC_IPVERSION    */

/* Bit fields for RFCRC EN */
#define _RFCRC_EN_RESETVALUE                  0x00000000UL                              /**< Default value for RFCRC_EN                  */
#define _RFCRC_EN_MASK                        0x00000001UL                              /**< Mask for RFCRC_EN                           */
#define RFCRC_EN_EN                           (0x1UL << 0)                              /**< Enable peripheral clock to this module      */
#define _RFCRC_EN_EN_SHIFT                    0                                         /**< Shift value for RFCRC_EN                    */
#define _RFCRC_EN_EN_MASK                     0x1UL                                     /**< Bit mask for RFCRC_EN                       */
#define _RFCRC_EN_EN_DEFAULT                  0x00000000UL                              /**< Mode DEFAULT for RFCRC_EN                   */
#define RFCRC_EN_EN_DEFAULT                   (_RFCRC_EN_EN_DEFAULT << 0)               /**< Shifted mode DEFAULT for RFCRC_EN           */

/* Bit fields for RFCRC CTRL */
#define _RFCRC_CTRL_RESETVALUE                0x00000704UL                              /**< Default value for RFCRC_CTRL                */
#define _RFCRC_CTRL_MASK                      0x00001FEFUL                              /**< Mask for RFCRC_CTRL                         */
#define RFCRC_CTRL_INPUTINV                   (0x1UL << 0)                              /**< Input Invert                                */
#define _RFCRC_CTRL_INPUTINV_SHIFT            0                                         /**< Shift value for RFCRC_INPUTINV              */
#define _RFCRC_CTRL_INPUTINV_MASK             0x1UL                                     /**< Bit mask for RFCRC_INPUTINV                 */
#define _RFCRC_CTRL_INPUTINV_DEFAULT          0x00000000UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define RFCRC_CTRL_INPUTINV_DEFAULT           (_RFCRC_CTRL_INPUTINV_DEFAULT << 0)       /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define RFCRC_CTRL_OUTPUTINV                  (0x1UL << 1)                              /**< Output Invert                               */
#define _RFCRC_CTRL_OUTPUTINV_SHIFT           1                                         /**< Shift value for RFCRC_OUTPUTINV             */
#define _RFCRC_CTRL_OUTPUTINV_MASK            0x2UL                                     /**< Bit mask for RFCRC_OUTPUTINV                */
#define _RFCRC_CTRL_OUTPUTINV_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define RFCRC_CTRL_OUTPUTINV_DEFAULT          (_RFCRC_CTRL_OUTPUTINV_DEFAULT << 1)      /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define _RFCRC_CTRL_CRCWIDTH_SHIFT            2                                         /**< Shift value for RFCRC_CRCWIDTH              */
#define _RFCRC_CTRL_CRCWIDTH_MASK             0xCUL                                     /**< Bit mask for RFCRC_CRCWIDTH                 */
#define _RFCRC_CTRL_CRCWIDTH_DEFAULT          0x00000001UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define _RFCRC_CTRL_CRCWIDTH_CRCWIDTH8        0x00000000UL                              /**< Mode CRCWIDTH8 for RFCRC_CTRL               */
#define _RFCRC_CTRL_CRCWIDTH_CRCWIDTH16       0x00000001UL                              /**< Mode CRCWIDTH16 for RFCRC_CTRL              */
#define _RFCRC_CTRL_CRCWIDTH_CRCWIDTH24       0x00000002UL                              /**< Mode CRCWIDTH24 for RFCRC_CTRL              */
#define _RFCRC_CTRL_CRCWIDTH_CRCWIDTH32       0x00000003UL                              /**< Mode CRCWIDTH32 for RFCRC_CTRL              */
#define RFCRC_CTRL_CRCWIDTH_DEFAULT           (_RFCRC_CTRL_CRCWIDTH_DEFAULT << 2)       /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define RFCRC_CTRL_CRCWIDTH_CRCWIDTH8         (_RFCRC_CTRL_CRCWIDTH_CRCWIDTH8 << 2)     /**< Shifted mode CRCWIDTH8 for RFCRC_CTRL       */
#define RFCRC_CTRL_CRCWIDTH_CRCWIDTH16        (_RFCRC_CTRL_CRCWIDTH_CRCWIDTH16 << 2)    /**< Shifted mode CRCWIDTH16 for RFCRC_CTRL      */
#define RFCRC_CTRL_CRCWIDTH_CRCWIDTH24        (_RFCRC_CTRL_CRCWIDTH_CRCWIDTH24 << 2)    /**< Shifted mode CRCWIDTH24 for RFCRC_CTRL      */
#define RFCRC_CTRL_CRCWIDTH_CRCWIDTH32        (_RFCRC_CTRL_CRCWIDTH_CRCWIDTH32 << 2)    /**< Shifted mode CRCWIDTH32 for RFCRC_CTRL      */
#define RFCRC_CTRL_INPUTBITORDER              (0x1UL << 5)                              /**< CRC input bit ordering setting              */
#define _RFCRC_CTRL_INPUTBITORDER_SHIFT       5                                         /**< Shift value for RFCRC_INPUTBITORDER         */
#define _RFCRC_CTRL_INPUTBITORDER_MASK        0x20UL                                    /**< Bit mask for RFCRC_INPUTBITORDER            */
#define _RFCRC_CTRL_INPUTBITORDER_DEFAULT     0x00000000UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define _RFCRC_CTRL_INPUTBITORDER_LSBFIRST    0x00000000UL                              /**< Mode LSBFIRST for RFCRC_CTRL                */
#define _RFCRC_CTRL_INPUTBITORDER_MSBFIRST    0x00000001UL                              /**< Mode MSBFIRST for RFCRC_CTRL                */
#define RFCRC_CTRL_INPUTBITORDER_DEFAULT      (_RFCRC_CTRL_INPUTBITORDER_DEFAULT << 5)  /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define RFCRC_CTRL_INPUTBITORDER_LSBFIRST     (_RFCRC_CTRL_INPUTBITORDER_LSBFIRST << 5) /**< Shifted mode LSBFIRST for RFCRC_CTRL        */
#define RFCRC_CTRL_INPUTBITORDER_MSBFIRST     (_RFCRC_CTRL_INPUTBITORDER_MSBFIRST << 5) /**< Shifted mode MSBFIRST for RFCRC_CTRL        */
#define RFCRC_CTRL_BYTEREVERSE                (0x1UL << 6)                              /**< Reverse CRC byte ordering over air          */
#define _RFCRC_CTRL_BYTEREVERSE_SHIFT         6                                         /**< Shift value for RFCRC_BYTEREVERSE           */
#define _RFCRC_CTRL_BYTEREVERSE_MASK          0x40UL                                    /**< Bit mask for RFCRC_BYTEREVERSE              */
#define _RFCRC_CTRL_BYTEREVERSE_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define _RFCRC_CTRL_BYTEREVERSE_NORMAL        0x00000000UL                              /**< Mode NORMAL for RFCRC_CTRL                  */
#define _RFCRC_CTRL_BYTEREVERSE_REVERSED      0x00000001UL                              /**< Mode REVERSED for RFCRC_CTRL                */
#define RFCRC_CTRL_BYTEREVERSE_DEFAULT        (_RFCRC_CTRL_BYTEREVERSE_DEFAULT << 6)    /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define RFCRC_CTRL_BYTEREVERSE_NORMAL         (_RFCRC_CTRL_BYTEREVERSE_NORMAL << 6)     /**< Shifted mode NORMAL for RFCRC_CTRL          */
#define RFCRC_CTRL_BYTEREVERSE_REVERSED       (_RFCRC_CTRL_BYTEREVERSE_REVERSED << 6)   /**< Shifted mode REVERSED for RFCRC_CTRL        */
#define RFCRC_CTRL_BITREVERSE                 (0x1UL << 7)                              /**< Reverse CRC bit ordering over air           */
#define _RFCRC_CTRL_BITREVERSE_SHIFT          7                                         /**< Shift value for RFCRC_BITREVERSE            */
#define _RFCRC_CTRL_BITREVERSE_MASK           0x80UL                                    /**< Bit mask for RFCRC_BITREVERSE               */
#define _RFCRC_CTRL_BITREVERSE_DEFAULT        0x00000000UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define _RFCRC_CTRL_BITREVERSE_NORMAL         0x00000000UL                              /**< Mode NORMAL for RFCRC_CTRL                  */
#define _RFCRC_CTRL_BITREVERSE_REVERSED       0x00000001UL                              /**< Mode REVERSED for RFCRC_CTRL                */
#define RFCRC_CTRL_BITREVERSE_DEFAULT         (_RFCRC_CTRL_BITREVERSE_DEFAULT << 7)     /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define RFCRC_CTRL_BITREVERSE_NORMAL          (_RFCRC_CTRL_BITREVERSE_NORMAL << 7)      /**< Shifted mode NORMAL for RFCRC_CTRL          */
#define RFCRC_CTRL_BITREVERSE_REVERSED        (_RFCRC_CTRL_BITREVERSE_REVERSED << 7)    /**< Shifted mode REVERSED for RFCRC_CTRL        */
#define _RFCRC_CTRL_BITSPERWORD_SHIFT         8                                         /**< Shift value for RFCRC_BITSPERWORD           */
#define _RFCRC_CTRL_BITSPERWORD_MASK          0xF00UL                                   /**< Bit mask for RFCRC_BITSPERWORD              */
#define _RFCRC_CTRL_BITSPERWORD_DEFAULT       0x00000007UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define RFCRC_CTRL_BITSPERWORD_DEFAULT        (_RFCRC_CTRL_BITSPERWORD_DEFAULT << 8)    /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define RFCRC_CTRL_PADCRCINPUT                (0x1UL << 12)                             /**< Pad CRC input data                          */
#define _RFCRC_CTRL_PADCRCINPUT_SHIFT         12                                        /**< Shift value for RFCRC_PADCRCINPUT           */
#define _RFCRC_CTRL_PADCRCINPUT_MASK          0x1000UL                                  /**< Bit mask for RFCRC_PADCRCINPUT              */
#define _RFCRC_CTRL_PADCRCINPUT_DEFAULT       0x00000000UL                              /**< Mode DEFAULT for RFCRC_CTRL                 */
#define _RFCRC_CTRL_PADCRCINPUT_X0            0x00000000UL                              /**< Mode X0 for RFCRC_CTRL                      */
#define _RFCRC_CTRL_PADCRCINPUT_X1            0x00000001UL                              /**< Mode X1 for RFCRC_CTRL                      */
#define RFCRC_CTRL_PADCRCINPUT_DEFAULT        (_RFCRC_CTRL_PADCRCINPUT_DEFAULT << 12)   /**< Shifted mode DEFAULT for RFCRC_CTRL         */
#define RFCRC_CTRL_PADCRCINPUT_X0             (_RFCRC_CTRL_PADCRCINPUT_X0 << 12)        /**< Shifted mode X0 for RFCRC_CTRL              */
#define RFCRC_CTRL_PADCRCINPUT_X1             (_RFCRC_CTRL_PADCRCINPUT_X1 << 12)        /**< Shifted mode X1 for RFCRC_CTRL              */

/* Bit fields for RFCRC STATUS */
#define _RFCRC_STATUS_RESETVALUE              0x00000000UL                              /**< Default value for RFCRC_STATUS              */
#define _RFCRC_STATUS_MASK                    0x00000001UL                              /**< Mask for RFCRC_STATUS                       */
#define RFCRC_STATUS_BUSY                     (0x1UL << 0)                              /**< CRC Running                                 */
#define _RFCRC_STATUS_BUSY_SHIFT              0                                         /**< Shift value for RFCRC_BUSY                  */
#define _RFCRC_STATUS_BUSY_MASK               0x1UL                                     /**< Bit mask for RFCRC_BUSY                     */
#define _RFCRC_STATUS_BUSY_DEFAULT            0x00000000UL                              /**< Mode DEFAULT for RFCRC_STATUS               */
#define RFCRC_STATUS_BUSY_DEFAULT             (_RFCRC_STATUS_BUSY_DEFAULT << 0)         /**< Shifted mode DEFAULT for RFCRC_STATUS       */

/* Bit fields for RFCRC CMD */
#define _RFCRC_CMD_RESETVALUE                 0x00000000UL                              /**< Default value for RFCRC_CMD                 */
#define _RFCRC_CMD_MASK                       0x00000001UL                              /**< Mask for RFCRC_CMD                          */
#define RFCRC_CMD_INITIALIZE                  (0x1UL << 0)                              /**< Initialize CRC                              */
#define _RFCRC_CMD_INITIALIZE_SHIFT           0                                         /**< Shift value for RFCRC_INITIALIZE            */
#define _RFCRC_CMD_INITIALIZE_MASK            0x1UL                                     /**< Bit mask for RFCRC_INITIALIZE               */
#define _RFCRC_CMD_INITIALIZE_DEFAULT         0x00000000UL                              /**< Mode DEFAULT for RFCRC_CMD                  */
#define RFCRC_CMD_INITIALIZE_DEFAULT          (_RFCRC_CMD_INITIALIZE_DEFAULT << 0)      /**< Shifted mode DEFAULT for RFCRC_CMD          */

/* Bit fields for RFCRC INPUTDATA */
#define _RFCRC_INPUTDATA_RESETVALUE           0x00000000UL                              /**< Default value for RFCRC_INPUTDATA           */
#define _RFCRC_INPUTDATA_MASK                 0x0000FFFFUL                              /**< Mask for RFCRC_INPUTDATA                    */
#define _RFCRC_INPUTDATA_INPUTDATA_SHIFT      0                                         /**< Shift value for RFCRC_INPUTDATA             */
#define _RFCRC_INPUTDATA_INPUTDATA_MASK       0xFFFFUL                                  /**< Bit mask for RFCRC_INPUTDATA                */
#define _RFCRC_INPUTDATA_INPUTDATA_DEFAULT    0x00000000UL                              /**< Mode DEFAULT for RFCRC_INPUTDATA            */
#define RFCRC_INPUTDATA_INPUTDATA_DEFAULT     (_RFCRC_INPUTDATA_INPUTDATA_DEFAULT << 0) /**< Shifted mode DEFAULT for RFCRC_INPUTDATA    */

/* Bit fields for RFCRC INIT */
#define _RFCRC_INIT_RESETVALUE                0x00000000UL                              /**< Default value for RFCRC_INIT                */
#define _RFCRC_INIT_MASK                      0xFFFFFFFFUL                              /**< Mask for RFCRC_INIT                         */
#define _RFCRC_INIT_INIT_SHIFT                0                                         /**< Shift value for RFCRC_INIT                  */
#define _RFCRC_INIT_INIT_MASK                 0xFFFFFFFFUL                              /**< Bit mask for RFCRC_INIT                     */
#define _RFCRC_INIT_INIT_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for RFCRC_INIT                 */
#define RFCRC_INIT_INIT_DEFAULT               (_RFCRC_INIT_INIT_DEFAULT << 0)           /**< Shifted mode DEFAULT for RFCRC_INIT         */

/* Bit fields for RFCRC DATA */
#define _RFCRC_DATA_RESETVALUE                0x00000000UL                              /**< Default value for RFCRC_DATA                */
#define _RFCRC_DATA_MASK                      0xFFFFFFFFUL                              /**< Mask for RFCRC_DATA                         */
#define _RFCRC_DATA_DATA_SHIFT                0                                         /**< Shift value for RFCRC_DATA                  */
#define _RFCRC_DATA_DATA_MASK                 0xFFFFFFFFUL                              /**< Bit mask for RFCRC_DATA                     */
#define _RFCRC_DATA_DATA_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for RFCRC_DATA                 */
#define RFCRC_DATA_DATA_DEFAULT               (_RFCRC_DATA_DATA_DEFAULT << 0)           /**< Shifted mode DEFAULT for RFCRC_DATA         */

/* Bit fields for RFCRC POLY */
#define _RFCRC_POLY_RESETVALUE                0x00000000UL                              /**< Default value for RFCRC_POLY                */
#define _RFCRC_POLY_MASK                      0xFFFFFFFFUL                              /**< Mask for RFCRC_POLY                         */
#define _RFCRC_POLY_POLY_SHIFT                0                                         /**< Shift value for RFCRC_POLY                  */
#define _RFCRC_POLY_POLY_MASK                 0xFFFFFFFFUL                              /**< Bit mask for RFCRC_POLY                     */
#define _RFCRC_POLY_POLY_DEFAULT              0x00000000UL                              /**< Mode DEFAULT for RFCRC_POLY                 */
#define RFCRC_POLY_POLY_DEFAULT               (_RFCRC_POLY_POLY_DEFAULT << 0)           /**< Shifted mode DEFAULT for RFCRC_POLY         */

/** @} End of group EFR32MG21_RFCRC_BitFields */
/** @} End of group EFR32MG21_RFCRC */
/** @} End of group Parts */
#endif /* EFR32MG21_RFCRC_H */
