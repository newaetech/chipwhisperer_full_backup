/***************************************************************************//**
 * @file
 * @brief EFM32TG11B_AF_PINS register and bit field definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
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
 ******************************************************************************/

#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup EFM32TG11B_Alternate_Function Alternate Function
 * @{
 * @defgroup EFM32TG11B_AF_Pins Alternate Function Pins
 * @{
 ******************************************************************************/

#define AF_CMU_CLK0_PIN(i)          ((i) == 0 ? 2 : (i) == 1 ? 12 : (i) == 2 ? 7 : (i) == 3 ? -1 : (i) == 4 ? 2 : (i) == 5 ? 12 :  -1)                                   /**< Pin number for AF_CMU_CLK0 location number i */
#define AF_CMU_CLK1_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 8 : (i) == 2 ? 12 : (i) == 3 ? -1 : (i) == 4 ? 3 : (i) == 5 ? 11 :  -1)                                   /**< Pin number for AF_CMU_CLK1 location number i */
#define AF_CMU_CLK2_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 6 : (i) == 3 ? -1 : (i) == 4 ? 3 : (i) == 5 ? -1 :  -1)                                    /**< Pin number for AF_CMU_CLK2 location number i */
#define AF_CMU_CLKI0_PIN(i)         ((i) == 0 ? 4 : (i) == 1 ? 3 : (i) == 2 ? 8 : (i) == 3 ? 13 : (i) == 4 ? -1 : (i) == 5 ? -1 : (i) == 6 ? 12 : (i) == 7 ? 11 :  -1)   /**< Pin number for AF_CMU_CLKI0 location number i */
#define AF_CMU_DIGEXTCLK_PIN(i)     ((i) == 0 ? 14 :  -1)                                                                                                                /**< Pin number for AF_CMU_DIGEXTCLK location number i */
#define AF_CMU_IOPOVR_PIN(i)        ((i) == 0 ? 13 :  -1)                                                                                                                /**< Pin number for AF_CMU_IOPOVR location number i */
#define AF_CMU_IONOVR_PIN(i)        ((i) == 0 ? 14 :  -1)                                                                                                                /**< Pin number for AF_CMU_IONOVR location number i */
#define AF_LESENSE_CH0_PIN(i)       ((i) == 0 ? 0 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH0 location number i */
#define AF_LESENSE_CH1_PIN(i)       ((i) == 0 ? 1 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH1 location number i */
#define AF_LESENSE_CH2_PIN(i)       ((i) == 0 ? 2 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH2 location number i */
#define AF_LESENSE_CH3_PIN(i)       ((i) == 0 ? 3 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH3 location number i */
#define AF_LESENSE_CH4_PIN(i)       ((i) == 0 ? 4 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH4 location number i */
#define AF_LESENSE_CH5_PIN(i)       ((i) == 0 ? 5 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH5 location number i */
#define AF_LESENSE_CH6_PIN(i)       ((i) == 0 ? 6 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH6 location number i */
#define AF_LESENSE_CH7_PIN(i)       ((i) == 0 ? 7 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH7 location number i */
#define AF_LESENSE_CH8_PIN(i)       ((i) == 0 ? 8 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH8 location number i */
#define AF_LESENSE_CH9_PIN(i)       ((i) == 0 ? 9 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_CH9 location number i */
#define AF_LESENSE_CH10_PIN(i)      ((i) == 0 ? 10 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH10 location number i */
#define AF_LESENSE_CH11_PIN(i)      ((i) == 0 ? 11 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH11 location number i */
#define AF_LESENSE_CH12_PIN(i)      ((i) == 0 ? 12 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH12 location number i */
#define AF_LESENSE_CH13_PIN(i)      ((i) == 0 ? 13 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH13 location number i */
#define AF_LESENSE_CH14_PIN(i)      ((i) == 0 ? 14 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH14 location number i */
#define AF_LESENSE_CH15_PIN(i)      ((i) == 0 ? 15 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_CH15 location number i */
#define AF_LESENSE_ALTEX0_PIN(i)    ((i) == 0 ? 6 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_ALTEX0 location number i */
#define AF_LESENSE_ALTEX1_PIN(i)    ((i) == 0 ? 7 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_ALTEX1 location number i */
#define AF_LESENSE_ALTEX2_PIN(i)    ((i) == 0 ? 3 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_ALTEX2 location number i */
#define AF_LESENSE_ALTEX3_PIN(i)    ((i) == 0 ? 4 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_ALTEX3 location number i */
#define AF_LESENSE_ALTEX4_PIN(i)    ((i) == 0 ? 5 :  -1)                                                                                                                 /**< Pin number for AF_LESENSE_ALTEX4 location number i */
#define AF_LESENSE_ALTEX5_PIN(i)    ((i) == 0 ? 11 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX5 location number i */
#define AF_LESENSE_ALTEX6_PIN(i)    ((i) == 0 ? 12 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX6 location number i */
#define AF_LESENSE_ALTEX7_PIN(i)    ((i) == 0 ? 13 :  -1)                                                                                                                /**< Pin number for AF_LESENSE_ALTEX7 location number i */
#define AF_PRS_CH0_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 3 : (i) == 2 ? 14 : (i) == 3 ? 2 :  -1)                                                                   /**< Pin number for AF_PRS_CH0 location number i */
#define AF_PRS_CH1_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 4 : (i) == 2 ? 15 : (i) == 3 ? 12 :  -1)                                                                  /**< Pin number for AF_PRS_CH1 location number i */
#define AF_PRS_CH2_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 5 : (i) == 2 ? 10 : (i) == 3 ? 13 :  -1)                                                                  /**< Pin number for AF_PRS_CH2 location number i */
#define AF_PRS_CH3_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 8 : (i) == 2 ? 11 : (i) == 3 ? 0 :  -1)                                                                   /**< Pin number for AF_PRS_CH3 location number i */
#define AF_PRS_CH4_PIN(i)           ((i) == 0 ? 8 : (i) == 1 ? -1 : (i) == 2 ? 1 :  -1)                                                                                  /**< Pin number for AF_PRS_CH4 location number i */
#define AF_PRS_CH5_PIN(i)           ((i) == 0 ? 9 : (i) == 1 ? -1 : (i) == 2 ? 6 :  -1)                                                                                  /**< Pin number for AF_PRS_CH5 location number i */
#define AF_PRS_CH6_PIN(i)           ((i) == 0 ? 6 : (i) == 1 ? 14 : (i) == 2 ? 6 :  -1)                                                                                  /**< Pin number for AF_PRS_CH6 location number i */
#define AF_PRS_CH7_PIN(i)           ((i) == 0 ? 13 : (i) == 1 ? -1 : (i) == 2 ? 7 :  -1)                                                                                 /**< Pin number for AF_PRS_CH7 location number i */
#define AF_CAN0_RX_PIN(i)           ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 0 : (i) == 3 ? -1 : (i) == 4 ? -1 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)   /**< Pin number for AF_CAN0_RX location number i */
#define AF_CAN0_TX_PIN(i)           ((i) == 0 ? 1 : (i) == 1 ? 2 : (i) == 2 ? 1 : (i) == 3 ? -1 : (i) == 4 ? -1 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)   /**< Pin number for AF_CAN0_TX location number i */
#define AF_TIMER0_CC0_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? -1 : (i) == 2 ? 1 : (i) == 3 ? 6 : (i) == 4 ? 0 : (i) == 5 ? 4 : (i) == 6 ? 8 : (i) == 7 ? 1 :  -1)       /**< Pin number for AF_TIMER0_CC0 location number i */
#define AF_TIMER0_CC1_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? -1 : (i) == 2 ? 2 : (i) == 3 ? 0 : (i) == 4 ? 1 : (i) == 5 ? 5 : (i) == 6 ? 9 : (i) == 7 ? 0 :  -1)       /**< Pin number for AF_TIMER0_CC1 location number i */
#define AF_TIMER0_CC2_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? -1 : (i) == 2 ? 3 : (i) == 3 ? 1 : (i) == 4 ? 2 : (i) == 5 ? -1 : (i) == 6 ? 10 : (i) == 7 ? 13 :  -1)    /**< Pin number for AF_TIMER0_CC2 location number i */
#define AF_TIMER0_CC3_PIN(i)        (-1)                                                                                                                                 /**< Pin number for AF_TIMER0_CC3 location number i */
#define AF_TIMER0_CDTI0_PIN(i)      ((i) == 0 ? 3 : (i) == 1 ? 13 : (i) == 2 ? 3 : (i) == 3 ? 2 : (i) == 4 ? 7 :  -1)                                                    /**< Pin number for AF_TIMER0_CDTI0 location number i */
#define AF_TIMER0_CDTI1_PIN(i)      ((i) == 0 ? 4 : (i) == 1 ? 14 : (i) == 2 ? 4 : (i) == 3 ? 3 : (i) == 4 ? 8 :  -1)                                                    /**< Pin number for AF_TIMER0_CDTI1 location number i */
#define AF_TIMER0_CDTI2_PIN(i)      ((i) == 0 ? 5 : (i) == 1 ? 15 : (i) == 2 ? 5 : (i) == 3 ? 4 : (i) == 4 ? 11 :  -1)                                                   /**< Pin number for AF_TIMER0_CDTI2 location number i */
#define AF_TIMER0_CDTI3_PIN(i)      (-1)                                                                                                                                 /**< Pin number for AF_TIMER0_CDTI3 location number i */
#define AF_TIMER1_CC0_PIN(i)        ((i) == 0 ? 13 : (i) == 1 ? 10 : (i) == 2 ? -1 : (i) == 3 ? 7 : (i) == 4 ? 6 : (i) == 5 ? 2 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)   /**< Pin number for AF_TIMER1_CC0 location number i */
#define AF_TIMER1_CC1_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? 11 : (i) == 2 ? -1 : (i) == 3 ? 8 : (i) == 4 ? 7 : (i) == 5 ? 3 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)   /**< Pin number for AF_TIMER1_CC1 location number i */
#define AF_TIMER1_CC2_PIN(i)        ((i) == 0 ? 15 : (i) == 1 ? 12 : (i) == 2 ? -1 : (i) == 3 ? 11 : (i) == 4 ? 13 : (i) == 5 ? 4 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1) /**< Pin number for AF_TIMER1_CC2 location number i */
#define AF_TIMER1_CC3_PIN(i)        ((i) == 0 ? 12 : (i) == 1 ? 13 : (i) == 2 ? 3 : (i) == 3 ? 12 : (i) == 4 ? 14 : (i) == 5 ? -1 : (i) == 6 ? 5 : (i) == 7 ? -1 :  -1)  /**< Pin number for AF_TIMER1_CC3 location number i */
#define AF_TIMER1_CDTI0_PIN(i)      (-1)                                                                                                                                 /**< Pin number for AF_TIMER1_CDTI0 location number i */
#define AF_TIMER1_CDTI1_PIN(i)      (-1)                                                                                                                                 /**< Pin number for AF_TIMER1_CDTI1 location number i */
#define AF_TIMER1_CDTI2_PIN(i)      (-1)                                                                                                                                 /**< Pin number for AF_TIMER1_CDTI2 location number i */
#define AF_TIMER1_CDTI3_PIN(i)      (-1)                                                                                                                                 /**< Pin number for AF_TIMER1_CDTI3 location number i */
#define AF_WTIMER0_CC0_PIN(i)       ((i) == 0 ? 4 : (i) == 1 ? 6 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 15 : (i) == 5 ? -1 : (i) == 6 ? 3 : (i) == 7 ? 1 :  -1)    /**< Pin number for AF_WTIMER0_CC0 location number i */
#define AF_WTIMER0_CC1_PIN(i)       ((i) == 0 ? 5 : (i) == 1 ? -1 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 0 : (i) == 5 ? -1 : (i) == 6 ? 4 : (i) == 7 ? 2 :  -1)    /**< Pin number for AF_WTIMER0_CC1 location number i */
#define AF_WTIMER0_CC2_PIN(i)       ((i) == 0 ? 6 : (i) == 1 ? -1 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 1 : (i) == 5 ? -1 : (i) == 6 ? 5 : (i) == 7 ? 3 :  -1)    /**< Pin number for AF_WTIMER0_CC2 location number i */
#define AF_WTIMER0_CC3_PIN(i)       (-1)                                                                                                                                 /**< Pin number for AF_WTIMER0_CC3 location number i */
#define AF_WTIMER0_CDTI0_PIN(i)     ((i) == 0 ? 10 : (i) == 1 ? -1 : (i) == 2 ? 12 : (i) == 3 ? -1 : (i) == 4 ? 4 :  -1)                                                 /**< Pin number for AF_WTIMER0_CDTI0 location number i */
#define AF_WTIMER0_CDTI1_PIN(i)     ((i) == 0 ? 11 : (i) == 1 ? -1 : (i) == 2 ? 13 : (i) == 3 ? -1 : (i) == 4 ? 5 :  -1)                                                 /**< Pin number for AF_WTIMER0_CDTI1 location number i */
#define AF_WTIMER0_CDTI2_PIN(i)     ((i) == 0 ? 12 : (i) == 1 ? -1 : (i) == 2 ? 14 : (i) == 3 ? -1 : (i) == 4 ? 6 :  -1)                                                 /**< Pin number for AF_WTIMER0_CDTI2 location number i */
#define AF_WTIMER0_CDTI3_PIN(i)     (-1)                                                                                                                                 /**< Pin number for AF_WTIMER0_CDTI3 location number i */
#define AF_WTIMER1_CC0_PIN(i)       ((i) == 0 ? 13 : (i) == 1 ? 2 : (i) == 2 ? 6 : (i) == 3 ? 7 : (i) == 4 ? -1 : (i) == 5 ? 7 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)    /**< Pin number for AF_WTIMER1_CC0 location number i */
#define AF_WTIMER1_CC1_PIN(i)       ((i) == 0 ? 14 : (i) == 1 ? 3 : (i) == 2 ? 7 : (i) == 3 ? -1 : (i) == 4 ? 4 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)   /**< Pin number for AF_WTIMER1_CC1 location number i */
#define AF_WTIMER1_CC2_PIN(i)       ((i) == 0 ? 0 : (i) == 1 ? 4 : (i) == 2 ? 8 : (i) == 3 ? -1 : (i) == 4 ? 5 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)    /**< Pin number for AF_WTIMER1_CC2 location number i */
#define AF_WTIMER1_CC3_PIN(i)       ((i) == 0 ? 1 : (i) == 1 ? 5 : (i) == 2 ? 6 : (i) == 3 ? -1 : (i) == 4 ? 6 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)    /**< Pin number for AF_WTIMER1_CC3 location number i */
#define AF_WTIMER1_CDTI0_PIN(i)     (-1)                                                                                                                                 /**< Pin number for AF_WTIMER1_CDTI0 location number i */
#define AF_WTIMER1_CDTI1_PIN(i)     (-1)                                                                                                                                 /**< Pin number for AF_WTIMER1_CDTI1 location number i */
#define AF_WTIMER1_CDTI2_PIN(i)     (-1)                                                                                                                                 /**< Pin number for AF_WTIMER1_CDTI2 location number i */
#define AF_WTIMER1_CDTI3_PIN(i)     (-1)                                                                                                                                 /**< Pin number for AF_WTIMER1_CDTI3 location number i */
#define AF_USART0_TX_PIN(i)         ((i) == 0 ? 10 : (i) == 1 ? 7 : (i) == 2 ? 11 : (i) == 3 ? 13 : (i) == 4 ? 7 : (i) == 5 ? 0 :  -1)                                   /**< Pin number for AF_USART0_TX location number i */
#define AF_USART0_RX_PIN(i)         ((i) == 0 ? 11 : (i) == 1 ? 6 : (i) == 2 ? 10 : (i) == 3 ? 12 : (i) == 4 ? 8 : (i) == 5 ? 1 :  -1)                                   /**< Pin number for AF_USART0_RX location number i */
#define AF_USART0_CLK_PIN(i)        ((i) == 0 ? 12 : (i) == 1 ? 5 : (i) == 2 ? 9 : (i) == 3 ? 15 : (i) == 4 ? 13 : (i) == 5 ? 12 :  -1)                                  /**< Pin number for AF_USART0_CLK location number i */
#define AF_USART0_CS_PIN(i)         ((i) == 0 ? 13 : (i) == 1 ? 4 : (i) == 2 ? 8 : (i) == 3 ? 14 : (i) == 4 ? 14 : (i) == 5 ? 13 :  -1)                                  /**< Pin number for AF_USART0_CS location number i */
#define AF_USART0_CTS_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? -1 : (i) == 2 ? 7 : (i) == 3 ? 13 : (i) == 4 ? 6 : (i) == 5 ? 11 :  -1)                                  /**< Pin number for AF_USART0_CTS location number i */
#define AF_USART0_RTS_PIN(i)        ((i) == 0 ? 15 : (i) == 1 ? -1 : (i) == 2 ? 6 : (i) == 3 ? 12 : (i) == 4 ? 5 : (i) == 5 ? 6 :  -1)                                   /**< Pin number for AF_USART0_RTS location number i */
#define AF_USART1_TX_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 0 : (i) == 2 ? 7 : (i) == 3 ? -1 : (i) == 4 ? 1 : (i) == 5 ? 2 : (i) == 6 ? 14 :  -1)                     /**< Pin number for AF_USART1_TX location number i */
#define AF_USART1_RX_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 1 : (i) == 2 ? 6 : (i) == 3 ? -1 : (i) == 4 ? 2 : (i) == 5 ? 0 : (i) == 6 ? 2 :  -1)                      /**< Pin number for AF_USART1_RX location number i */
#define AF_USART1_CLK_PIN(i)        ((i) == 0 ? 7 : (i) == 1 ? 2 : (i) == 2 ? 0 : (i) == 3 ? 15 : (i) == 4 ? 3 : (i) == 5 ? 11 : (i) == 6 ? 5 :  -1)                     /**< Pin number for AF_USART1_CLK location number i */
#define AF_USART1_CS_PIN(i)         ((i) == 0 ? 8 : (i) == 1 ? 3 : (i) == 2 ? 1 : (i) == 3 ? 14 : (i) == 4 ? 0 : (i) == 5 ? 4 : (i) == 6 ? -1 :  -1)                     /**< Pin number for AF_USART1_CS location number i */
#define AF_USART1_CTS_PIN(i)        ((i) == 0 ? -1 : (i) == 1 ? 4 : (i) == 2 ? 3 : (i) == 3 ? 6 : (i) == 4 ? 12 : (i) == 5 ? 13 : (i) == 6 ? -1 :  -1)                   /**< Pin number for AF_USART1_CTS location number i */
#define AF_USART1_RTS_PIN(i)        ((i) == 0 ? -1 : (i) == 1 ? 5 : (i) == 2 ? 4 : (i) == 3 ? 7 : (i) == 4 ? 13 : (i) == 5 ? 14 : (i) == 6 ? -1 :  -1)                   /**< Pin number for AF_USART1_RTS location number i */
#define AF_USART2_TX_PIN(i)         ((i) == 0 ? 2 : (i) == 1 ? 3 : (i) == 2 ? -1 : (i) == 3 ? 13 : (i) == 4 ? -1 : (i) == 5 ? 0 :  -1)                                   /**< Pin number for AF_USART2_TX location number i */
#define AF_USART2_RX_PIN(i)         ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 8 : (i) == 3 ? 14 : (i) == 4 ? -1 : (i) == 5 ? 1 :  -1)                                    /**< Pin number for AF_USART2_RX location number i */
#define AF_USART2_CLK_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 9 : (i) == 3 ? 15 : (i) == 4 ? -1 : (i) == 5 ? 2 :  -1)                                    /**< Pin number for AF_USART2_CLK location number i */
#define AF_USART2_CS_PIN(i)         ((i) == 0 ? 5 : (i) == 1 ? 6 : (i) == 2 ? 10 : (i) == 3 ? 11 : (i) == 4 ? -1 : (i) == 5 ? 5 :  -1)                                   /**< Pin number for AF_USART2_CS location number i */
#define AF_USART2_CTS_PIN(i)        ((i) == 0 ? 1 : (i) == 1 ? 12 : (i) == 2 ? -1 : (i) == 3 ? -1 : (i) == 4 ? 12 : (i) == 5 ? 6 :  -1)                                  /**< Pin number for AF_USART2_CTS location number i */
#define AF_USART2_RTS_PIN(i)        ((i) == 0 ? 0 : (i) == 1 ? -1 : (i) == 2 ? 12 : (i) == 3 ? 14 : (i) == 4 ? 13 : (i) == 5 ? 8 :  -1)                                  /**< Pin number for AF_USART2_RTS location number i */
#define AF_USART3_TX_PIN(i)         ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? 3 : (i) == 3 ? -1 :  -1)                                                                   /**< Pin number for AF_USART3_TX location number i */
#define AF_USART3_RX_PIN(i)         ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? 7 : (i) == 3 ? -1 :  -1)                                                                   /**< Pin number for AF_USART3_RX location number i */
#define AF_USART3_CLK_PIN(i)        ((i) == 0 ? 2 : (i) == 1 ? 7 : (i) == 2 ? 4 : (i) == 3 ? -1 :  -1)                                                                   /**< Pin number for AF_USART3_CLK location number i */
#define AF_USART3_CS_PIN(i)         ((i) == 0 ? 3 : (i) == 1 ? 4 : (i) == 2 ? 14 : (i) == 3 ? 0 :  -1)                                                                   /**< Pin number for AF_USART3_CS location number i */
#define AF_USART3_CTS_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 5 : (i) == 2 ? 6 : (i) == 3 ? -1 :  -1)                                                                   /**< Pin number for AF_USART3_CTS location number i */
#define AF_USART3_RTS_PIN(i)        ((i) == 0 ? 5 : (i) == 1 ? 1 : (i) == 2 ? 14 : (i) == 3 ? 15 :  -1)                                                                  /**< Pin number for AF_USART3_RTS location number i */
#define AF_UART0_TX_PIN(i)          ((i) == 0 ? -1 : (i) == 1 ? -1 : (i) == 2 ? 3 : (i) == 3 ? 14 : (i) == 4 ? 4 : (i) == 5 ? 1 : (i) == 6 ? 7 :  -1)                    /**< Pin number for AF_UART0_TX location number i */
#define AF_UART0_RX_PIN(i)          ((i) == 0 ? -1 : (i) == 1 ? -1 : (i) == 2 ? 4 : (i) == 3 ? 15 : (i) == 4 ? 5 : (i) == 5 ? 2 : (i) == 6 ? 4 :  -1)                    /**< Pin number for AF_UART0_RX location number i */
#define AF_UART0_CLK_PIN(i)         (-1)                                                                                                                                 /**< Pin number for AF_UART0_CLK location number i */
#define AF_UART0_CS_PIN(i)          (-1)                                                                                                                                 /**< Pin number for AF_UART0_CS location number i */
#define AF_UART0_CTS_PIN(i)         ((i) == 0 ? -1 : (i) == 1 ? -1 : (i) == 2 ? 5 : (i) == 3 ? 13 : (i) == 4 ? 7 : (i) == 5 ? 5 :  -1)                                   /**< Pin number for AF_UART0_CTS location number i */
#define AF_UART0_RTS_PIN(i)         ((i) == 0 ? -1 : (i) == 1 ? -1 : (i) == 2 ? 6 : (i) == 3 ? 12 : (i) == 4 ? 8 : (i) == 5 ? 6 :  -1)                                   /**< Pin number for AF_UART0_RTS location number i */
#define AF_LEUART0_TX_PIN(i)        ((i) == 0 ? 4 : (i) == 1 ? 13 : (i) == 2 ? 14 : (i) == 3 ? 0 : (i) == 4 ? 2 : (i) == 5 ? 14 :  -1)                                   /**< Pin number for AF_LEUART0_TX location number i */
#define AF_LEUART0_RX_PIN(i)        ((i) == 0 ? 5 : (i) == 1 ? 14 : (i) == 2 ? 15 : (i) == 3 ? 1 : (i) == 4 ? 0 : (i) == 5 ? 15 :  -1)                                   /**< Pin number for AF_LEUART0_RX location number i */
#define AF_LETIMER0_OUT0_PIN(i)     ((i) == 0 ? 6 : (i) == 1 ? 11 : (i) == 2 ? 0 : (i) == 3 ? 4 : (i) == 4 ? 12 : (i) == 5 ? 14 : (i) == 6 ? 8 :  -1)                    /**< Pin number for AF_LETIMER0_OUT0 location number i */
#define AF_LETIMER0_OUT1_PIN(i)     ((i) == 0 ? 7 : (i) == 1 ? 12 : (i) == 2 ? 1 : (i) == 3 ? 5 : (i) == 4 ? 13 : (i) == 5 ? 15 : (i) == 6 ? 9 :  -1)                    /**< Pin number for AF_LETIMER0_OUT1 location number i */
#define AF_PCNT0_S0IN_PIN(i)        ((i) == 0 ? 13 : (i) == 1 ? -1 : (i) == 2 ? 0 : (i) == 3 ? 6 : (i) == 4 ? 0 : (i) == 5 ? -1 : (i) == 6 ? 5 : (i) == 7 ? 12 :  -1)    /**< Pin number for AF_PCNT0_S0IN location number i */
#define AF_PCNT0_S1IN_PIN(i)        ((i) == 0 ? 14 : (i) == 1 ? -1 : (i) == 2 ? 1 : (i) == 3 ? 7 : (i) == 4 ? 1 : (i) == 5 ? -1 : (i) == 6 ? 6 : (i) == 7 ? 11 :  -1)    /**< Pin number for AF_PCNT0_S1IN location number i */
#define AF_I2C0_SDA_PIN(i)          ((i) == 0 ? 0 : (i) == 1 ? 6 : (i) == 2 ? 6 : (i) == 3 ? -1 : (i) == 4 ? 0 : (i) == 5 ? 0 : (i) == 6 ? 12 : (i) == 7 ? 4 :  -1)      /**< Pin number for AF_I2C0_SDA location number i */
#define AF_I2C0_SCL_PIN(i)          ((i) == 0 ? 1 : (i) == 1 ? 7 : (i) == 2 ? 7 : (i) == 3 ? -1 : (i) == 4 ? 1 : (i) == 5 ? 1 : (i) == 6 ? 13 : (i) == 7 ? 5 :  -1)      /**< Pin number for AF_I2C0_SCL location number i */
#define AF_I2C1_SDA_PIN(i)          ((i) == 0 ? 4 : (i) == 1 ? 11 : (i) == 2 ? -1 : (i) == 3 ? 4 : (i) == 4 ? 11 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)  /**< Pin number for AF_I2C1_SDA location number i */
#define AF_I2C1_SCL_PIN(i)          ((i) == 0 ? 5 : (i) == 1 ? 12 : (i) == 2 ? -1 : (i) == 3 ? 5 : (i) == 4 ? 2 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? -1 :  -1)   /**< Pin number for AF_I2C1_SCL location number i */
#define AF_ACMP0_OUT_PIN(i)         ((i) == 0 ? 13 : (i) == 1 ? -1 : (i) == 2 ? 6 : (i) == 3 ? 11 : (i) == 4 ? 6 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? 3 :  -1)   /**< Pin number for AF_ACMP0_OUT location number i */
#define AF_ACMP1_OUT_PIN(i)         ((i) == 0 ? 2 : (i) == 1 ? -1 : (i) == 2 ? 7 : (i) == 3 ? 12 : (i) == 4 ? 14 : (i) == 5 ? -1 : (i) == 6 ? -1 : (i) == 7 ? 5 :  -1)   /**< Pin number for AF_ACMP1_OUT location number i */
#define AF_DBG_TDI_PIN(i)           ((i) == 0 ? 5 :  -1)                                                                                                                 /**< Pin number for AF_DBG_TDI location number i */
#define AF_DBG_TDO_PIN(i)           ((i) == 0 ? 2 :  -1)                                                                                                                 /**< Pin number for AF_DBG_TDO location number i */
#define AF_DBG_SWV_PIN(i)           ((i) == 0 ? -1 :  -1)                                                                                                                /**< Pin number for AF_DBG_SWV location number i */
#define AF_DBG_SWDIOTMS_PIN(i)      ((i) == 0 ? 1 :  -1)                                                                                                                 /**< Pin number for AF_DBG_SWDIOTMS location number i */
#define AF_DBG_SWCLKTCK_PIN(i)      ((i) == 0 ? 0 :  -1)                                                                                                                 /**< Pin number for AF_DBG_SWCLKTCK location number i */

/** @} */
/** @} End of group EFM32TG11B_AF_Pins */
/** @} End of group Parts */
