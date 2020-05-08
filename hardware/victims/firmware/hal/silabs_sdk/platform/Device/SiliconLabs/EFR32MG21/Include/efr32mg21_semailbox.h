/**************************************************************************//**
 * @file
 * @brief EFR32MG21 SEMAILBOX register and bit field definitions
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
#ifndef EFR32MG21_SEMAILBOX_H
#define EFR32MG21_SEMAILBOX_H

/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG21_SEMAILBOX_HOST SEMAILBOX_HOST
 * @{
 * @brief EFR32MG21 SEMAILBOX_HOST Register Declaration.
 *****************************************************************************/

/** SEMAILBOX_HOST FIFO Register Group Declaration. */
typedef struct {
  __IOM uint32_t DATA;                               /**< RX/TX FIFO DATA                                    */
} SEMAILBOX_FIFO_TypeDef;

/** SEMAILBOX_HOST Register Declaration. */
typedef struct {
  SEMAILBOX_FIFO_TypeDef FIFO[16U];               /**< RX/TX FIFO                                         */
  __IM uint32_t          TX_STATUS;               /**< TX Status                                          */
  __IM uint32_t          RX_STATUS;               /**< RX Status                                          */
  __IM uint32_t          TX_PROT;                 /**< TX Protection                                      */
  __IM uint32_t          RX_PROT;                 /**< RX Protection                                      */
  __IOM uint32_t         TX_HEADER;               /**< TX Header                                          */
  __IM uint32_t          RX_HEADER;               /**< RX Header                                          */
  __IOM uint32_t         CONFIGURATION;           /**< Configuration                                      */
} SEMAILBOX_HOST_TypeDef;
/** @} End of group EFR32MG21_SEMAILBOX_HOST */

/**************************************************************************//**
 * @addtogroup EFR32MG21_SEMAILBOX_HOST
 * @{
 * @defgroup EFR32MG21_SEMAILBOX_HOST_BitFields SEMAILBOX_HOST Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SEMAILBOX DATA */
#define _SEMAILBOX_DATA_RESETVALUE                  0x00000000UL                        /**< Default value for SEMAILBOX_DATA            */
#define _SEMAILBOX_DATA_MASK                        0xFFFFFFFFUL                        /**< Mask for SEMAILBOX_DATA                     */
#define _SEMAILBOX_DATA_DATA_SHIFT                  0                                   /**< Shift value for SEMAILBOX_DATA              */
#define _SEMAILBOX_DATA_DATA_MASK                   0xFFFFFFFFUL                        /**< Bit mask for SEMAILBOX_DATA                 */
#define _SEMAILBOX_DATA_DATA_DEFAULT                0x00000000UL                        /**< Mode DEFAULT for SEMAILBOX_DATA             */
#define SEMAILBOX_DATA_DATA_DEFAULT                 (_SEMAILBOX_DATA_DATA_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_DATA     */

/* Bit fields for SEMAILBOX TX_STATUS */
#define _SEMAILBOX_TX_STATUS_RESETVALUE             0x00000000UL                                 /**< Default value for SEMAILBOX_TX_STATUS       */
#define _SEMAILBOX_TX_STATUS_MASK                   0x00B0FFFFUL                                 /**< Mask for SEMAILBOX_TX_STATUS                */
#define _SEMAILBOX_TX_STATUS_BYTEREM_SHIFT          0                                            /**< Shift value for SEMAILBOX_BYTEREM           */
#define _SEMAILBOX_TX_STATUS_BYTEREM_MASK           0xFFFFUL                                     /**< Bit mask for SEMAILBOX_BYTEREM              */
#define _SEMAILBOX_TX_STATUS_BYTEREM_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_BYTEREM_DEFAULT         (_SEMAILBOX_TX_STATUS_BYTEREM_DEFAULT << 0)  /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/
#define SEMAILBOX_TX_STATUS_TXINT                   (0x1UL << 20)                                /**< Interrupt Status                            */
#define _SEMAILBOX_TX_STATUS_TXINT_SHIFT            20                                           /**< Shift value for SEMAILBOX_TXINT             */
#define _SEMAILBOX_TX_STATUS_TXINT_MASK             0x100000UL                                   /**< Bit mask for SEMAILBOX_TXINT                */
#define _SEMAILBOX_TX_STATUS_TXINT_DEFAULT          0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_TXINT_DEFAULT           (_SEMAILBOX_TX_STATUS_TXINT_DEFAULT << 20)   /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/
#define SEMAILBOX_TX_STATUS_TXFULL                  (0x1UL << 21)                                /**< TX FIFO Full                                */
#define _SEMAILBOX_TX_STATUS_TXFULL_SHIFT           21                                           /**< Shift value for SEMAILBOX_TXFULL            */
#define _SEMAILBOX_TX_STATUS_TXFULL_MASK            0x200000UL                                   /**< Bit mask for SEMAILBOX_TXFULL               */
#define _SEMAILBOX_TX_STATUS_TXFULL_DEFAULT         0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_TXFULL_DEFAULT          (_SEMAILBOX_TX_STATUS_TXFULL_DEFAULT << 21)  /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/
#define SEMAILBOX_TX_STATUS_TXERROR                 (0x1UL << 23)                                /**< TX Error Flag                               */
#define _SEMAILBOX_TX_STATUS_TXERROR_SHIFT          23                                           /**< Shift value for SEMAILBOX_TXERROR           */
#define _SEMAILBOX_TX_STATUS_TXERROR_MASK           0x800000UL                                   /**< Bit mask for SEMAILBOX_TXERROR              */
#define _SEMAILBOX_TX_STATUS_TXERROR_DEFAULT        0x00000000UL                                 /**< Mode DEFAULT for SEMAILBOX_TX_STATUS        */
#define SEMAILBOX_TX_STATUS_TXERROR_DEFAULT         (_SEMAILBOX_TX_STATUS_TXERROR_DEFAULT << 23) /**< Shifted mode DEFAULT for SEMAILBOX_TX_STATUS*/

/* Bit fields for SEMAILBOX RX_STATUS */
#define _SEMAILBOX_RX_STATUS_RESETVALUE             0x00000000UL                                  /**< Default value for SEMAILBOX_RX_STATUS       */
#define _SEMAILBOX_RX_STATUS_MASK                   0x00FFFFFFUL                                  /**< Mask for SEMAILBOX_RX_STATUS                */
#define _SEMAILBOX_RX_STATUS_BYTEREM_SHIFT          0                                             /**< Shift value for SEMAILBOX_BYTEREM           */
#define _SEMAILBOX_RX_STATUS_BYTEREM_MASK           0xFFFFUL                                      /**< Bit mask for SEMAILBOX_BYTEREM              */
#define _SEMAILBOX_RX_STATUS_BYTEREM_DEFAULT        0x00000000UL                                  /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_BYTEREM_DEFAULT         (_SEMAILBOX_RX_STATUS_BYTEREM_DEFAULT << 0)   /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define _SEMAILBOX_RX_STATUS_STATUS_SHIFT           16                                            /**< Shift value for SEMAILBOX_STATUS            */
#define _SEMAILBOX_RX_STATUS_STATUS_MASK            0xF0000UL                                     /**< Bit mask for SEMAILBOX_STATUS               */
#define _SEMAILBOX_RX_STATUS_STATUS_DEFAULT         0x00000000UL                                  /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_STATUS_DEFAULT          (_SEMAILBOX_RX_STATUS_STATUS_DEFAULT << 16)   /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXINT                   (0x1UL << 20)                                 /**< Interrupt Status                            */
#define _SEMAILBOX_RX_STATUS_RXINT_SHIFT            20                                            /**< Shift value for SEMAILBOX_RXINT             */
#define _SEMAILBOX_RX_STATUS_RXINT_MASK             0x100000UL                                    /**< Bit mask for SEMAILBOX_RXINT                */
#define _SEMAILBOX_RX_STATUS_RXINT_DEFAULT          0x00000000UL                                  /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXINT_DEFAULT           (_SEMAILBOX_RX_STATUS_RXINT_DEFAULT << 20)    /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXEMPTY                 (0x1UL << 21)                                 /**< RX FIFO Empty                               */
#define _SEMAILBOX_RX_STATUS_RXEMPTY_SHIFT          21                                            /**< Shift value for SEMAILBOX_RXEMPTY           */
#define _SEMAILBOX_RX_STATUS_RXEMPTY_MASK           0x200000UL                                    /**< Bit mask for SEMAILBOX_RXEMPTY              */
#define _SEMAILBOX_RX_STATUS_RXEMPTY_DEFAULT        0x00000000UL                                  /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXEMPTY_DEFAULT         (_SEMAILBOX_RX_STATUS_RXEMPTY_DEFAULT << 21)  /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXHEADER                (0x1UL << 22)                                 /**< RX Header                                   */
#define _SEMAILBOX_RX_STATUS_RXHEADER_SHIFT         22                                            /**< Shift value for SEMAILBOX_RXHEADER          */
#define _SEMAILBOX_RX_STATUS_RXHEADER_MASK          0x400000UL                                    /**< Bit mask for SEMAILBOX_RXHEADER             */
#define _SEMAILBOX_RX_STATUS_RXHEADER_DEFAULT       0x00000000UL                                  /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXHEADER_DEFAULT        (_SEMAILBOX_RX_STATUS_RXHEADER_DEFAULT << 22) /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/
#define SEMAILBOX_RX_STATUS_RXERROR                 (0x1UL << 23)                                 /**< RX Error Flag                               */
#define _SEMAILBOX_RX_STATUS_RXERROR_SHIFT          23                                            /**< Shift value for SEMAILBOX_RXERROR           */
#define _SEMAILBOX_RX_STATUS_RXERROR_MASK           0x800000UL                                    /**< Bit mask for SEMAILBOX_RXERROR              */
#define _SEMAILBOX_RX_STATUS_RXERROR_DEFAULT        0x00000000UL                                  /**< Mode DEFAULT for SEMAILBOX_RX_STATUS        */
#define SEMAILBOX_RX_STATUS_RXERROR_DEFAULT         (_SEMAILBOX_RX_STATUS_RXERROR_DEFAULT << 23)  /**< Shifted mode DEFAULT for SEMAILBOX_RX_STATUS*/

/* Bit fields for SEMAILBOX TX_PROT */
#define _SEMAILBOX_TX_PROT_RESETVALUE               0x00000000UL                                   /**< Default value for SEMAILBOX_TX_PROT         */
#define _SEMAILBOX_TX_PROT_MASK                     0xFFE00000UL                                   /**< Mask for SEMAILBOX_TX_PROT                  */
#define SEMAILBOX_TX_PROT_UNPROTECTED               (0x1UL << 21)                                  /**< Unprotected                                 */
#define _SEMAILBOX_TX_PROT_UNPROTECTED_SHIFT        21                                             /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_TX_PROT_UNPROTECTED_MASK         0x200000UL                                     /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_TX_PROT_UNPROTECTED_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_UNPROTECTED_DEFAULT       (_SEMAILBOX_TX_PROT_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */
#define SEMAILBOX_TX_PROT_PRIV                      (0x1UL << 22)                                  /**< Privileged Access                           */
#define _SEMAILBOX_TX_PROT_PRIV_SHIFT               22                                             /**< Shift value for SEMAILBOX_PRIV              */
#define _SEMAILBOX_TX_PROT_PRIV_MASK                0x400000UL                                     /**< Bit mask for SEMAILBOX_PRIV                 */
#define _SEMAILBOX_TX_PROT_PRIV_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_PRIV_DEFAULT              (_SEMAILBOX_TX_PROT_PRIV_DEFAULT << 22)        /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */
#define SEMAILBOX_TX_PROT_NONSEC                    (0x1UL << 23)                                  /**< Non-Secure Access                           */
#define _SEMAILBOX_TX_PROT_NONSEC_SHIFT             23                                             /**< Shift value for SEMAILBOX_NONSEC            */
#define _SEMAILBOX_TX_PROT_NONSEC_MASK              0x800000UL                                     /**< Bit mask for SEMAILBOX_NONSEC               */
#define _SEMAILBOX_TX_PROT_NONSEC_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_NONSEC_DEFAULT            (_SEMAILBOX_TX_PROT_NONSEC_DEFAULT << 23)      /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */
#define _SEMAILBOX_TX_PROT_USER_SHIFT               24                                             /**< Shift value for SEMAILBOX_USER              */
#define _SEMAILBOX_TX_PROT_USER_MASK                0xFF000000UL                                   /**< Bit mask for SEMAILBOX_USER                 */
#define _SEMAILBOX_TX_PROT_USER_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_TX_PROT          */
#define SEMAILBOX_TX_PROT_USER_DEFAULT              (_SEMAILBOX_TX_PROT_USER_DEFAULT << 24)        /**< Shifted mode DEFAULT for SEMAILBOX_TX_PROT  */

/* Bit fields for SEMAILBOX RX_PROT */
#define _SEMAILBOX_RX_PROT_RESETVALUE               0x00000000UL                                   /**< Default value for SEMAILBOX_RX_PROT         */
#define _SEMAILBOX_RX_PROT_MASK                     0xFFE00000UL                                   /**< Mask for SEMAILBOX_RX_PROT                  */
#define SEMAILBOX_RX_PROT_UNPROTECTED               (0x1UL << 21)                                  /**< Unprotected                                 */
#define _SEMAILBOX_RX_PROT_UNPROTECTED_SHIFT        21                                             /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_RX_PROT_UNPROTECTED_MASK         0x200000UL                                     /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_RX_PROT_UNPROTECTED_DEFAULT      0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_UNPROTECTED_DEFAULT       (_SEMAILBOX_RX_PROT_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */
#define SEMAILBOX_RX_PROT_PRIV                      (0x1UL << 22)                                  /**< Privileged Access                           */
#define _SEMAILBOX_RX_PROT_PRIV_SHIFT               22                                             /**< Shift value for SEMAILBOX_PRIV              */
#define _SEMAILBOX_RX_PROT_PRIV_MASK                0x400000UL                                     /**< Bit mask for SEMAILBOX_PRIV                 */
#define _SEMAILBOX_RX_PROT_PRIV_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_PRIV_DEFAULT              (_SEMAILBOX_RX_PROT_PRIV_DEFAULT << 22)        /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */
#define SEMAILBOX_RX_PROT_NONSEC                    (0x1UL << 23)                                  /**< Non-Secure Access                           */
#define _SEMAILBOX_RX_PROT_NONSEC_SHIFT             23                                             /**< Shift value for SEMAILBOX_NONSEC            */
#define _SEMAILBOX_RX_PROT_NONSEC_MASK              0x800000UL                                     /**< Bit mask for SEMAILBOX_NONSEC               */
#define _SEMAILBOX_RX_PROT_NONSEC_DEFAULT           0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_NONSEC_DEFAULT            (_SEMAILBOX_RX_PROT_NONSEC_DEFAULT << 23)      /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */
#define _SEMAILBOX_RX_PROT_USER_SHIFT               24                                             /**< Shift value for SEMAILBOX_USER              */
#define _SEMAILBOX_RX_PROT_USER_MASK                0xFF000000UL                                   /**< Bit mask for SEMAILBOX_USER                 */
#define _SEMAILBOX_RX_PROT_USER_DEFAULT             0x00000000UL                                   /**< Mode DEFAULT for SEMAILBOX_RX_PROT          */
#define SEMAILBOX_RX_PROT_USER_DEFAULT              (_SEMAILBOX_RX_PROT_USER_DEFAULT << 24)        /**< Shifted mode DEFAULT for SEMAILBOX_RX_PROT  */

/* Bit fields for SEMAILBOX TX_HEADER */
#define _SEMAILBOX_TX_HEADER_RESETVALUE             0x00000000UL                                     /**< Default value for SEMAILBOX_TX_HEADER       */
#define _SEMAILBOX_TX_HEADER_MASK                   0x0020FFFFUL                                     /**< Mask for SEMAILBOX_TX_HEADER                */
#define _SEMAILBOX_TX_HEADER_SIZE_SHIFT             0                                                /**< Shift value for SEMAILBOX_SIZE              */
#define _SEMAILBOX_TX_HEADER_SIZE_MASK              0xFFFFUL                                         /**< Bit mask for SEMAILBOX_SIZE                 */
#define _SEMAILBOX_TX_HEADER_SIZE_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for SEMAILBOX_TX_HEADER        */
#define SEMAILBOX_TX_HEADER_SIZE_DEFAULT            (_SEMAILBOX_TX_HEADER_SIZE_DEFAULT << 0)         /**< Shifted mode DEFAULT for SEMAILBOX_TX_HEADER*/
#define SEMAILBOX_TX_HEADER_UNPROTECTED             (0x1UL << 21)                                    /**< Unprotected                                 */
#define _SEMAILBOX_TX_HEADER_UNPROTECTED_SHIFT      21                                               /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_TX_HEADER_UNPROTECTED_MASK       0x200000UL                                       /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_TX_HEADER_UNPROTECTED_DEFAULT    0x00000000UL                                     /**< Mode DEFAULT for SEMAILBOX_TX_HEADER        */
#define SEMAILBOX_TX_HEADER_UNPROTECTED_DEFAULT     (_SEMAILBOX_TX_HEADER_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_TX_HEADER*/

/* Bit fields for SEMAILBOX RX_HEADER */
#define _SEMAILBOX_RX_HEADER_RESETVALUE             0x00000000UL                                     /**< Default value for SEMAILBOX_RX_HEADER       */
#define _SEMAILBOX_RX_HEADER_MASK                   0x002FFFFFUL                                     /**< Mask for SEMAILBOX_RX_HEADER                */
#define _SEMAILBOX_RX_HEADER_SIZE_SHIFT             0                                                /**< Shift value for SEMAILBOX_SIZE              */
#define _SEMAILBOX_RX_HEADER_SIZE_MASK              0xFFFFUL                                         /**< Bit mask for SEMAILBOX_SIZE                 */
#define _SEMAILBOX_RX_HEADER_SIZE_DEFAULT           0x00000000UL                                     /**< Mode DEFAULT for SEMAILBOX_RX_HEADER        */
#define SEMAILBOX_RX_HEADER_SIZE_DEFAULT            (_SEMAILBOX_RX_HEADER_SIZE_DEFAULT << 0)         /**< Shifted mode DEFAULT for SEMAILBOX_RX_HEADER*/
#define _SEMAILBOX_RX_HEADER_STATUS_SHIFT           16                                               /**< Shift value for SEMAILBOX_STATUS            */
#define _SEMAILBOX_RX_HEADER_STATUS_MASK            0xF0000UL                                        /**< Bit mask for SEMAILBOX_STATUS               */
#define _SEMAILBOX_RX_HEADER_STATUS_DEFAULT         0x00000000UL                                     /**< Mode DEFAULT for SEMAILBOX_RX_HEADER        */
#define SEMAILBOX_RX_HEADER_STATUS_DEFAULT          (_SEMAILBOX_RX_HEADER_STATUS_DEFAULT << 16)      /**< Shifted mode DEFAULT for SEMAILBOX_RX_HEADER*/
#define SEMAILBOX_RX_HEADER_UNPROTECTED             (0x1UL << 21)                                    /**< Unprotected                                 */
#define _SEMAILBOX_RX_HEADER_UNPROTECTED_SHIFT      21                                               /**< Shift value for SEMAILBOX_UNPROTECTED       */
#define _SEMAILBOX_RX_HEADER_UNPROTECTED_MASK       0x200000UL                                       /**< Bit mask for SEMAILBOX_UNPROTECTED          */
#define _SEMAILBOX_RX_HEADER_UNPROTECTED_DEFAULT    0x00000000UL                                     /**< Mode DEFAULT for SEMAILBOX_RX_HEADER        */
#define SEMAILBOX_RX_HEADER_UNPROTECTED_DEFAULT     (_SEMAILBOX_RX_HEADER_UNPROTECTED_DEFAULT << 21) /**< Shifted mode DEFAULT for SEMAILBOX_RX_HEADER*/

/* Bit fields for SEMAILBOX CONFIGURATION */
#define _SEMAILBOX_CONFIGURATION_RESETVALUE         0x00000000UL                                    /**< Default value for SEMAILBOX_CONFIGURATION   */
#define _SEMAILBOX_CONFIGURATION_MASK               0x00000003UL                                    /**< Mask for SEMAILBOX_CONFIGURATION            */
#define SEMAILBOX_CONFIGURATION_TXINTEN             (0x1UL << 0)                                    /**< TX Interrupt Enable                         */
#define _SEMAILBOX_CONFIGURATION_TXINTEN_SHIFT      0                                               /**< Shift value for SEMAILBOX_TXINTEN           */
#define _SEMAILBOX_CONFIGURATION_TXINTEN_MASK       0x1UL                                           /**< Bit mask for SEMAILBOX_TXINTEN              */
#define _SEMAILBOX_CONFIGURATION_TXINTEN_DEFAULT    0x00000000UL                                    /**< Mode DEFAULT for SEMAILBOX_CONFIGURATION    */
#define SEMAILBOX_CONFIGURATION_TXINTEN_DEFAULT     (_SEMAILBOX_CONFIGURATION_TXINTEN_DEFAULT << 0) /**< Shifted mode DEFAULT for SEMAILBOX_CONFIGURATION*/
#define SEMAILBOX_CONFIGURATION_RXINTEN             (0x1UL << 1)                                    /**< RX Interrupt Enable                         */
#define _SEMAILBOX_CONFIGURATION_RXINTEN_SHIFT      1                                               /**< Shift value for SEMAILBOX_RXINTEN           */
#define _SEMAILBOX_CONFIGURATION_RXINTEN_MASK       0x2UL                                           /**< Bit mask for SEMAILBOX_RXINTEN              */
#define _SEMAILBOX_CONFIGURATION_RXINTEN_DEFAULT    0x00000000UL                                    /**< Mode DEFAULT for SEMAILBOX_CONFIGURATION    */
#define SEMAILBOX_CONFIGURATION_RXINTEN_DEFAULT     (_SEMAILBOX_CONFIGURATION_RXINTEN_DEFAULT << 1) /**< Shifted mode DEFAULT for SEMAILBOX_CONFIGURATION*/

/** @} End of group EFR32MG21_SEMAILBOX_HOST_BitFields */
/** @} End of group EFR32MG21_SEMAILBOX_HOST */
/**************************************************************************//**
 * @defgroup EFR32MG21_SEMAILBOX_SE SEMAILBOX_SE
 * @{
 * @brief EFR32MG21 SEMAILBOX_SE Register Declaration.
 *****************************************************************************/

/** SEMAILBOX_SE Register Declaration. */
typedef struct {
  __IOM uint32_t NEW_REG;                            /**< New Register                                       */
} SEMAILBOX_SE_TypeDef;
/** @} End of group EFR32MG21_SEMAILBOX_SE */

/**************************************************************************//**
 * @addtogroup EFR32MG21_SEMAILBOX_SE
 * @{
 * @defgroup EFR32MG21_SEMAILBOX_SE_BitFields SEMAILBOX_SE Bit Fields
 * @{
 *****************************************************************************/

/* Bit fields for SEMAILBOX NEW_REG */
#define _SEMAILBOX_NEW_REG_RESETVALUE        0x00000000UL                               /**< Default value for SEMAILBOX_NEW_REG         */
#define _SEMAILBOX_NEW_REG_MASK              0x00000001UL                               /**< Mask for SEMAILBOX_NEW_REG                  */
#define SEMAILBOX_NEW_REG_NEWBIT             (0x1UL << 0)                               /**< New BitField                                */
#define _SEMAILBOX_NEW_REG_NEWBIT_SHIFT      0                                          /**< Shift value for SEMAILBOX_NEWBIT            */
#define _SEMAILBOX_NEW_REG_NEWBIT_MASK       0x1UL                                      /**< Bit mask for SEMAILBOX_NEWBIT               */
#define _SEMAILBOX_NEW_REG_NEWBIT_DEFAULT    0x00000000UL                               /**< Mode DEFAULT for SEMAILBOX_NEW_REG          */
#define SEMAILBOX_NEW_REG_NEWBIT_DEFAULT     (_SEMAILBOX_NEW_REG_NEWBIT_DEFAULT << 0)   /**< Shifted mode DEFAULT for SEMAILBOX_NEW_REG  */

/** @} End of group EFR32MG21_SEMAILBOX_SE_BitFields */
/** @} End of group EFR32MG21_SEMAILBOX_SE */
/** @} End of group Parts */
#endif /* EFR32MG21_SEMAILBOX_H */
